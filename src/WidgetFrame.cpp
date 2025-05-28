#include "WidgetFrame.h"

#include <QMouseEvent>
#include <QWindow>

#include "WidgetFramePrivate.h"
#include "WidgetTitleBar.h"

WidgetFrame::WidgetFrame(QWidget* _parent) : QWidget{_parent}, d_ptr{new WidgetFramePrivate{this}}
{
}

WidgetFrame::~WidgetFrame() noexcept
{
    Q_D(WidgetFrame);
    delete d;
}

bool WidgetFrame::nativeEvent(const QByteArray& _eventType, void* _message, qintptr* _result)
{
    Q_D(WidgetFrame);
#ifdef Q_OS_WIN
    /// @brief 转换为Windows MSG结构
    MSG* msg{static_cast<MSG*>(_message)};
    if (_eventType != QByteArray{"windows_generic_MSG"})
    {
        goto NO_WIN;
    }
    switch (msg->message)
    {
        case WM_NCCALCSIZE:  // 计算窗口客户区大小（用于去掉系统边框）
        {
            NCCALCSIZE_PARAMS* params{reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam)};
            // 判断是否最大化
            WINDOWPLACEMENT wp{};
            wp.length = sizeof(WINDOWPLACEMENT);
            ::GetWindowPlacement(msg->hwnd, &wp);
            const bool isMaximized{(wp.showCmd == SW_SHOWMAXIMIZED)};
            if (isMaximized)
            {
                // 从屏幕边缘缩进几像素
                params->rgrc[0].top += 8;
                params->rgrc[0].left += 8;
                params->rgrc[0].right -= 8;
                params->rgrc[0].bottom -= 8;
            }
            *_result = 0;
            return true;
        }
        case WM_ACTIVATE:  // win11 圆角,窗口激活/非激活（可以用来重新绘制边框）
        {
            constexpr MARGINS margins{1, 1, 1, 1};
            HRESULT           hr{DwmExtendFrameIntoClientArea(msg->hwnd, &margins)};
            *_result = hr;
            return true;
        }
        case WM_NCHITTEST:  // 判断鼠标在哪个窗口区域
        {
            // 获取鼠标的屏幕坐标
            POINT mouse{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
            RECT  rc{};
            // 把窗口在屏幕上的位置放入 rc 中
            GetWindowRect(msg->hwnd, &rc);
            RECT rcFrame{};
            // x,y 为鼠标在屏幕的坐标
            long x{GET_X_LPARAM(msg->lParam)};
            long y{GET_Y_LPARAM(msg->lParam)};
            if (0 != *_result)
            {
                return true;
            }
            double dpr{this->devicePixelRatioF()};
            QPoint pos{d->m_titleBar->mapFromGlobal(QPoint(x / dpr, y / dpr))};
            if (!d->m_titleBar->rect().contains(pos))
            {
                return false;
            }
            QWidget* child{d->m_titleBar->childAt(pos)};
            if (!child)
            {
                *_result = HTCAPTION;
                return true;
            }
            // 计算默认窗口边框的大小（不包含标题栏）
            ::AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);
            /// @brief 判断鼠标是否悬停在最大化按钮上, win11 触发snap layout, 以下代码确保不同分辨率下正确触发
            QAbstractButton* maximize{d->m_titleBar->getMaximizeBtn()};
            QPoint           globalPos(mouse.x, mouse.y);
            QWindow*         handle{this->window()->windowHandle()};
            if (handle && handle->screen())
            {
                QScreen* screen{handle->screen()};
                QPoint   offset{screen->geometry().topLeft()};
                globalPos = (globalPos - offset) / screen->devicePixelRatio() + offset;
            }
            const QPoint& localPos{maximize->mapFromGlobal(globalPos)};
            // 判断鼠标是否在最大化按钮上，如果在创建一个鼠标进入按钮的事件，发送给QT事件，否则QT无法收到这个事件 QSS失效
            if (maximize->rect().contains(localPos))
            {
                // 鼠标进入按钮事件和在按钮上移动事件
                QMouseEvent mouseEvent{maximize->underMouse() ? QEvent::MouseMove : QEvent::Enter, localPos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(maximize, &mouseEvent);
                maximize->update();
                *_result = HTMAXBUTTON;
                return true;
            }
            // 如果没有在最大化按钮上面,查看鼠标是否移出了按钮，发送鼠标移出按钮事件
            if (maximize->underMouse())
            {
                QMouseEvent mouseEvent{QEvent::Leave, localPos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(maximize, &mouseEvent);
                maximize->update();
                return true;
            }
            *_result = HTCLIENT;
            return false;
        }
        case WM_NCLBUTTONDOWN:  // 鼠标在非客户区按下
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                QAbstractButton* maximize{d->m_titleBar->getMaximizeBtn()};
                QMouseEvent      mouseEvent{QEvent::MouseButtonPress, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier};
                QCoreApplication::sendEvent(maximize, &mouseEvent);
                *_result = HTNOWHERE;
                return true;
            }
        }
        case WM_NCLBUTTONUP:  // 鼠标在非客户区释放
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                QAbstractButton* maximize{d->m_titleBar->getMaximizeBtn()};
                QMouseEvent      mouseEvent(QEvent::MouseButtonRelease, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                QCoreApplication::sendEvent(maximize, &mouseEvent);
                *_result = HTNOWHERE;
                return true;
            }
        }
        case WM_NCMOUSEMOVE:  // 防止系统最大化按钮弹出 Snap Layout 菜单
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                *_result = HTNOWHERE;
                return true;
            }
        }
    }
#endif
NO_WIN:
    return QWidget::nativeEvent(_eventType, _message, _result);
}
