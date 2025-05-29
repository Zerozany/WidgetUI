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
    constinit static POINT    mouse{};
    constinit static QPoint   pos{};
    constinit static QWidget* child{nullptr};
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
            mouse = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
            // 获取DPI转换后（屏幕）所在坐标
            pos = {Win32Function::coordinateMapping(mouse, this, d->m_titleBar)};
            if (*_result != 0)
            {
                return true;
            }
            // 检索标题栏中鼠标所在元素
            child = {d->m_titleBar->childAt(pos)};
            /// @brief 检测鼠标位置在标题栏
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
            else
            {
                qDebug() << "leave";
            }
            // qDebug() << "custom";
            *_result = HTCLIENT;
            return false;
        }
        case WM_NCLBUTTONDOWN:  // 鼠标左键在非客户区按下
        {
            qDebug() << "no custom pressed";
            if (msg->wParam == HTMAXBUTTON || tag)
            {
                QMouseEvent mouseEvent{QEvent::MouseButtonPress, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                *_result = HTNOWHERE;
                return true;
            }
            break;
        }
        case WM_NCLBUTTONUP:  // 鼠标左键在非客户区释放
        {
            qDebug() << "no custom released";
            if (tag)
            {
                QMouseEvent mouseEvent(QEvent::MouseButtonRelease, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                *_result = HTNOWHERE;
                return true;
            }
            break;
        }
        /// @brief 鼠标在非客户区域内移动
        case WM_NCMOUSEMOVE:
        {
            qDebug() << "maximize moveing";
            if (msg->wParam == HTMAXBUTTON)
            {
                *_result = HTNOWHERE;
                return false;
            }
            break;
        }
        case WM_NCMOUSEHOVER:  // 鼠标在非客户区悬停一段时间后触发
        {
            qDebug() << "in no custom some time";

            // if (msg->wParam == HTMAXBUTTON)
            // {
            //     *_result = HTNOWHERE;
            //     return true;
            // }
            break;
        }
        /// @brief 鼠标离开非客户区后触发
        case WM_NCMOUSELEAVE:
        {
            qDebug() << "remove to no custom";
            if (tag)
            {
                QMouseEvent mouseEvent{QEvent::Leave, QPoint(), QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
                tag = false;
                return false;
            }
            break;
        }
        /// @brief 鼠标离开客户区后触发
        case WM_MOUSELEAVE:
        {
            qDebug() << "remove to custom ";

            // [[fallthrough]];
            break;
        }
        case WM_LBUTTONUP:  // 用户松开鼠标左键
        {
            qDebug() << "realease left button";

            // [[fallthrough]];
            break;
        }
        case WM_MOUSEMOVE:
        {
            if (child != d->m_titleBar->getMaximizeBtn())
            {
                qDebug() << "WM_MOUSEMOVE";
                QWidget* maxBtn   = d->m_titleBar->getMaximizeBtn();
                QPoint   localPos = maxBtn->mapFromGlobal(QCursor::pos());
                if (!maxBtn->rect().contains(localPos))
                {
                    QEvent leaveEvent(QEvent::Leave);
                    QCoreApplication::sendEvent(maxBtn, &leaveEvent);

                    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, localPos, QCursor::pos(),
                                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                    QCoreApplication::sendEvent(maxBtn, &releaseEvent);

                    maxBtn->update();
                    tag = false;
                    return true;
                }
            }
            break;
        }
        // case WM_LBUTTONDOWN:  // 鼠标左键按下（客户区）
        // {
        // }
        // case WM_LBUTTONDBLCLK:  // 鼠标左键双击（客户区）
        // {
        // }
        // case WM_RBUTTONDOWN:  // 鼠标右键按下（客户区）
        // {
        // }
        // case WM_RBUTTONUP:  // 鼠标右键释放（客户区）
        // {
        // }
        // case WM_RBUTTONDBLCLK:  // 鼠标右键双击（客户区）
        // {
        // }
        case WM_MOUSEHOVER:  // 鼠标悬停（需要调用 TrackMouseEvent 开启监听）（客户区）
        {
            qDebug() << "mouse hovering";
            break;
        }
        case WM_CAPTURECHANGED:  // 捕获的窗口改变（例如鼠标按住拖动时，焦点窗口改变）
        {
            qDebug() << "windwo change";
            break;
        }
        case WM_MOUSEACTIVATE:  // 鼠标点击激活窗口前的处理
        {
            qDebug() << "track before ready";
            break;
        }
#if 0
        case WM_MBUTTONDOWN:    // 鼠标中键按下
        case WM_MBUTTONUP:      // 鼠标中键释放
        case WM_MBUTTONDBLCLK:  // 鼠标中键双击
        case WM_MOUSEWHEEL:     // 鼠标滚轮滚动（垂直方向）
        case WM_MOUSEHWHEEL:    // 鼠标滚轮滚动（水平方向）
#endif
        case WM_NCRBUTTONDOWN:  // 鼠标在非客户区右键按下
        {
            qDebug() << "mouse in no custom right button pressed";

            // [[fallthrough]];
            break;
        }
        case WM_NCRBUTTONUP:  // 鼠标在非客户区右键释放
        {
            qDebug() << "mouse in no custom right button release";

            // [[fallthrough]];
            break;
        }
        case WM_NCRBUTTONDBLCLK:  // 鼠标在非客户区右键双击
        {
            qDebug() << "mouse in no custom right button double";

            // [[fallthrough]];
            break;
        }
        case WM_NCMBUTTONDOWN:  // 鼠标在非客户区中键按下
        {
            qDebug() << "mouse in no custom mide button pressed";

            // [[fallthrough]];
            break;
        }
        case WM_NCMBUTTONUP:  // 鼠标在非客户区中键释放
        {
            qDebug() << "mouse in no custom mide button release";

            // [[fallthrough]];
            break;
        }
        case WM_NCMBUTTONDBLCLK:  // 鼠标在非客户区中键双击
        {
            qDebug() << "mouse in no custom mide button double";

            // [[fallthrough]];
            break;
        }
        case WM_NCLBUTTONDBLCLK:  // 鼠标在非客户区域双击左键时触发
        {
            qDebug() << "mouse in no custom left button double";

            // if (msg->wParam == HTMAXBUTTON)
            // {
            //     *_result = 0;
            //     // 处理鼠标事件
            //     return true;
            // }
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
