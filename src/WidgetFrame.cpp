#include "WidgetFrame.h"

#include <QMouseEvent>
#include <QWindow>

#include "WidgetFramePrivate.h"
#include "WidgetTitleBar.h"

constinit static bool tag{false};

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
    MSG* msg{static_cast<MSG*>(_message)};
    if (_eventType != static_cast<QByteArray>("windows_generic_MSG"))
    {
        goto NO_WINDOWS_GENERIC_MSG;
    }
    switch (msg->message)
    {
        /// @brief 计算窗口客户区大小（用于去掉系统边框）
        case WM_NCCALCSIZE:
        {
            Win32Function::adjustCustomerArea(msg);
            *_result = 0;
            return true;
        }
        /// @brief 用于通知窗口 是否获得或失去焦点（激活状态）
        case WM_ACTIVATE:
        {
            const HRESULT hr{Win32Function::achieveRoundedCorners(msg)};
            *_result = hr;
            return true;
        }
        /// @brief 判断鼠标在窗口所在区域
        case WM_NCHITTEST:
        {
            // 获取鼠标的（屏幕）所在坐标
            POINT mouse{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
            if (*_result != 0)
            {
                return true;
            }
            /// @brief 检测鼠标位置在标题栏
            // 获取DPI下鼠标的（界面）所在坐标
            QPoint   pos{Win32Function::coordinateMapping(mouse, this, d->m_titleBar)};
            QWidget* child{d->m_titleBar->childAt(pos)};
            if (!child && d->m_titleBar->rect().contains(pos))
            {
                *_result = HTCAPTION;
                return true;
            }
            /// @brief 判断鼠标是否悬停在最大化按钮上, win11触发snap layout
            if (child == d->m_titleBar->getMaximizeBtn())
            {
                // DPI 缩放校正
                const QPoint globalPos{Win32Function::scalingCorrection(mouse, this->window()->windowHandle())};
                // 转化为最大化按钮的局部坐标
                const QPoint localPos{Win32Function::coordinateMapping(mouse, this, d->m_titleBar->getMaximizeBtn())};
                QMouseEvent  mouseEvent{d->m_titleBar->getMaximizeBtn()->underMouse() ? QEvent::MouseMove : QEvent::Enter, localPos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
                tag      = true;
                *_result = HTMAXBUTTON;
                return true;
            }
            if (tag && child != d->m_titleBar->getMaximizeBtn())
            {
                QMouseEvent mouseEvent{QEvent::Leave, QPoint(), QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
                tag = false;
                return true;
            }
            *_result = HTCLIENT;
            return false;
        }
        case WM_NCLBUTTONDOWN:  // 鼠标在非客户区按下
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                QMouseEvent mouseEvent{QEvent::MouseButtonPress, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                *_result = HTNOWHERE;
                return true;
            }
            break;
        }
        case WM_NCLBUTTONUP:  // 鼠标在非客户区释放
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                QMouseEvent mouseEvent(QEvent::MouseButtonRelease, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                *_result = HTNOWHERE;
                return true;
            }
            break;
        }
        case WM_NCMOUSEMOVE:  // 当鼠标移动到窗口的“非客户区域”（如标题栏）时触发此消息
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                *_result = HTNOWHERE;
                return true;
            }
            break;
        }
        case WM_NCMOUSEHOVER:  // 鼠标在非客户区悬停一段时间后触发
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                *_result = HTNOWHERE;
                return true;
            }
            break;
        }
        case WM_NCMOUSELEAVE:  // 鼠标离开非客户区后触发
        {
            if (tag)
            {
                qDebug() << "*-*-";
                QMouseEvent mouseEvent{QEvent::Leave, QPoint(), QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
                tag = false;
                return true;
            }
            break;
        }
        case WM_MOUSELEAVE:  // 鼠标离开客户区后触发
        {
            [[fallthrough]];
        }
        case WM_LBUTTONUP:  // 用户松开鼠标左键
        {
            [[fallthrough]];
        }
        case WM_NCRBUTTONDOWN:  // 鼠标在非客户区右键按下
        {
            [[fallthrough]];
        }
        case WM_NCRBUTTONUP:  // 鼠标在非客户区右键释放
        {
            [[fallthrough]];
        }
        case WM_NCRBUTTONDBLCLK:  // 鼠标在非客户区右键双击
        {
            [[fallthrough]];
        }
        case WM_NCMBUTTONDOWN:  // 鼠标在非客户区中键按下
        {
            [[fallthrough]];
        }
        case WM_NCMBUTTONUP:  // 鼠标在非客户区中键释放
        {
            [[fallthrough]];
        }
        case WM_NCMBUTTONDBLCLK:  // 鼠标在非客户区中键双击
        {
            [[fallthrough]];
        }
        case WM_NCLBUTTONDBLCLK:  // 鼠标在非客户区域双击左键时触发
        {
            if (msg->wParam == HTMAXBUTTON)
            {
                *_result = 0;
                // 处理鼠标事件
                return true;
            }
            break;
        }
        default:
        {
            break;
        }
    }
#endif
NO_WINDOWS_GENERIC_MSG:
    return QWidget::nativeEvent(_eventType, _message, _result);
}
