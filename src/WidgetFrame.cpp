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
            Win32Function::adjustCustomerArea(msg);
            *_result = 0;
            return true;
        }
        case WM_ACTIVATE:  // win11 圆角,窗口激活/非激活（可以用来重新绘制边框）
        {
            const HRESULT hr{Win32Function::achieveRoundedCorners(msg)};
            *_result = hr;
            return true;
        }
        case WM_NCHITTEST:  // 判断鼠标在哪个窗口区域
        {
            // 获取鼠标的屏幕坐标
            POINT mouse{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
            if (*_result != 0)
            {
                return true;
            }
            QPoint pos{Win32Function::coordinateMapping(mouse, this, d->m_titleBar)};
            if (!d->m_titleBar->rect().contains(pos))
            {
                return false;
            }
            /// @brief 检测鼠标位置在标题栏
            QWidget* child{d->m_titleBar->childAt(pos)};
            if (!child)
            {
                *_result = HTCAPTION;
                return true;
            }
            /// @brief 判断鼠标是否悬停在最大化按钮上, win11 触发snap layout
            // DPI 缩放校正
            QPoint globalPos(Win32Function::scalingCorrection(mouse, this->window()->windowHandle()));
            // 转化为最大化按钮的局部坐标
            QPushButton*  maximize{d->m_titleBar->getMaximizeBtn()};
            const QPoint& localPos{maximize->mapFromGlobal(globalPos)};
            // 判断鼠标是否在最大化按钮上，如果在创建一个鼠标进入按钮的事件，发送给QT事件，否则QT无法收到这个事件 QSS失效
            if (child == d->m_titleBar->getMaximizeBtn())
            {
                QMouseEvent mouseEvent{maximize->underMouse() ? QEvent::MouseMove : QEvent::Enter, localPos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(maximize, &mouseEvent);
                maximize->update();
                *_result = HTMAXBUTTON;
                return true;
            }
            // 如果没有在最大化按钮上面,查看鼠标是否移出了按钮，发送鼠标移出按钮事件
            if (maximize->underMouse())
            {
                QMouseEvent mouseEvent{QEvent::Leave, QPoint(), QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier};
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
        case WM_NCMOUSEMOVE:
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
