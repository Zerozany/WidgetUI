#include "WidgetFrame.h"

#include <QMouseEvent>
#include <QWindow>

#include "WidgetFramePrivate.h"
#include "WidgetTitleBar.h"

constinit static bool g_snapLayoutTag{false};
constinit static bool g_pressedTag{true};

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
    constinit static QWidget* child{nullptr};
    constinit static POINT    mouse{};
    constinit static QPoint   pos{};
    constinit static HWND     hwnd{};
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
            /// @brief 开启窗口伸缩事件
            if (d->m_titleBar->getResizing())
            {
                return false;
            }
            // 获取鼠标的（屏幕）所在坐标
            mouse = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
            // 获取DPI转换后（屏幕）所在坐标
            pos = {Win32Function::coordinateMapping(mouse, this, d->m_titleBar)};
            // 获取窗口状态句柄
            hwnd = reinterpret_cast<HWND>(this->winId());
            if (*_result != 0)
            {
                return true;
            }
            // 检索标题栏中鼠标所在元素
            child = d->m_titleBar->childAt(pos);
            /// @brief 检测鼠标位置在标题栏
            if (!child && d->m_titleBar->rect().contains(pos))
            {
                if (::IsZoomed(hwnd) || (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
                {
                    *_result = HTCAPTION;
                    return true;
                }
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
                g_snapLayoutTag = true;
                // 检测某个按键的当前状态（是否被按下）
                if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
                {
                    g_pressedTag = false;
                }
                else
                {
                    g_pressedTag = true;
                }
                *_result = g_pressedTag ? HTMAXBUTTON : HTCLIENT;
                return true;
            }
            *_result = HTCLIENT;
            return false;
        }
        case WM_LBUTTONDOWN:  // 鼠标左键按下（客户区）
        {
            if (g_snapLayoutTag)
            {
                // 当最大化按钮被点击，关闭Snap Layout
                QMouseEvent mouseEvent{QEvent::MouseButtonPress, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
            }
        }
        case WM_NCLBUTTONDOWN:  // 鼠标左键在非客户区按下
        {
            break;
        }
        case WM_NCLBUTTONUP:  // 鼠标左键在非客户区释放
        {
            if (g_snapLayoutTag)
            {
                QMouseEvent mouseEvent{QEvent::MouseButtonRelease, QPoint(), QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
                g_snapLayoutTag = false;
                return true;
            }
            break;
        }
        /// @brief 鼠标离开非客户区后触发
        case WM_NCMOUSELEAVE:
        {
            if (g_snapLayoutTag)
            {
                QMouseEvent mouseEvent{QEvent::Leave, QPoint(), QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
                g_snapLayoutTag = false;
                return false;
            }
            break;
        }
        /// @brief 鼠标离开客户区后触发
        case WM_MOUSELEAVE:
        {
            if (g_snapLayoutTag)
            {
                QMouseEvent mouseEvent{QEvent::Leave, QPoint(), QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier};
                QCoreApplication::sendEvent(d->m_titleBar->getMaximizeBtn(), &mouseEvent);
                d->m_titleBar->getMaximizeBtn()->update();
                g_snapLayoutTag = false;
                return false;
            }
            break;
        }
        case WM_NCLBUTTONDBLCLK:  // 鼠标在非客户区域双击左键时触发
        {
            if (!child && d->m_titleBar->rect().contains(pos))
            {
                if (::IsZoomed(hwnd))
                {
                    d->m_titleBar->getMaximizeBtn()->setIcon(QIcon{R"(:/resources/icon/maximize.png)"});
                }
                else
                {
                    d->m_titleBar->getMaximizeBtn()->setIcon(QIcon{R"(:/resources/icon/normal.png)"});
                }
                return false;
            }
            break;
        }
        /// @brief 窗口大小状态变化事件
        case WM_SIZE:
        {
            if (msg->wParam == SIZE_MAXIMIZED)
            {
                d->m_titleBar->getMaximizeBtn()->setIcon(QIcon{R"(:/resources/icon/normal.png)"});
            }
            else if (msg->wParam == SIZE_RESTORED)
            {
                d->m_titleBar->getMaximizeBtn()->setIcon(QIcon{R"(:/resources/icon/maximize.png)"});
            }
            break;
        }

        /// @brief 鼠标在非客户区域内移动
        case WM_NCMOUSEMOVE:
        {
            [[fallthrough]];
        }
        case WM_NCMOUSEHOVER:  // 鼠标在非客户区悬停一段时间后触发
        {
            [[fallthrough]];
        }
        case WM_LBUTTONUP:  // 用户松开鼠标左键（客户区）
        {
            [[fallthrough]];
        }
        case WM_MOUSEMOVE:
        {
            [[fallthrough]];
        }
        case WM_LBUTTONDBLCLK:  // 鼠标左键双击（客户区）
        {
            [[fallthrough]];
        }
        case WM_RBUTTONDOWN:  // 鼠标右键按下（客户区）
        {
            [[fallthrough]];
        }
        case WM_RBUTTONUP:  // 鼠标右键释放（客户区）
        {
            [[fallthrough]];
        }
        case WM_RBUTTONDBLCLK:  // 鼠标右键双击（客户区）
        {
            [[fallthrough]];
        }
        case WM_MOUSEHOVER:  // 鼠标悬停（需要调用 TrackMouseEvent 开启监听）（客户区）
        {
            [[fallthrough]];
        }
        case WM_CAPTURECHANGED:  // 捕获的窗口改变（例如鼠标按住拖动时，焦点窗口改变）
        {
            [[fallthrough]];
        }
        case WM_MOUSEACTIVATE:  // 鼠标点击激活窗口前的处理
        {
            [[fallthrough]];
        }
        case WM_MBUTTONDOWN:
        {
            [[fallthrough]];
        }  // 鼠标中键按下
        case WM_MBUTTONUP:
        {
            [[fallthrough]];
        }  // 鼠标中键释放
        case WM_MBUTTONDBLCLK:
        {
            [[fallthrough]];
        }  // 鼠标中键双击
        case WM_MOUSEWHEEL:
        {
            [[fallthrough]];
        }  // 鼠标滚轮滚动（垂直方向）
        case WM_MOUSEHWHEEL:
        {
            [[fallthrough]];
        }  // 鼠标滚轮滚动（水平方向）
        case WM_NCRBUTTONDOWN:
        {
            [[fallthrough]];
        }  // 鼠标在非客户区右键按下
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
        default:
        {
            break;
        }
    }
#endif
NO_WINDOWS_GENERIC_MSG:
    return QWidget::nativeEvent(_eventType, _message, _result);
}

void WidgetFrame::mousePressEvent(QMouseEvent* _event)
{
    Q_D(WidgetFrame);
    Q_EMIT d->m_titleBar->mousePress(_event);
}

void WidgetFrame::mouseMoveEvent(QMouseEvent* _event)
{
    Q_D(WidgetFrame);
    Q_EMIT d->m_titleBar->mouseMove(_event);
}

void WidgetFrame::mouseReleaseEvent(QMouseEvent* _event)
{
    Q_D(WidgetFrame);
    Q_EMIT d->m_titleBar->mouseRelease(_event);
}

void WidgetFrame::mouseDoubleClickEvent(QMouseEvent* _event)
{
    Q_D(WidgetFrame);
    Q_EMIT d->m_titleBar->mouseDouble(_event);
}
