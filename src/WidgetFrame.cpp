#include "WidgetFrame.h"

#include "WidgetFramePrivate.h"

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
    // 鼠标靠近窗口边缘5个像素触发可拖拽窗口
    constexpr int boundary{5};
    MSG*          msg{static_cast<MSG*>(_message)};
    switch (msg->message)
    {
        case WM_NCCALCSIZE:  // 无边框
        {
            *_result = HTNOWHERE;
            return true;
        }
        case WM_ACTIVATE:  // win11 圆角
        {
            MARGINS margins{1, 1, 0, 1};
            HRESULT hr{DwmExtendFrameIntoClientArea(msg->hwnd, &margins)};
            *_result = hr;
            return true;
        }
        case WM_NCHITTEST:
        {
            // POINT mouse{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
            // RECT  rc{};
            // GetWindowRect(msg->hwnd, &rc);
            // RECT rcFrame{0};
            // AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);
            // /// @brief 判断鼠标是否悬停在最大化按钮上, win11 触发snap layout, 以下代码确保不同分辨率下正确触发
            // QAbstractButton* maximize{d->m_titleBar->maximizeButton()};
            // assert(d->m_titleBar != nullptr && maximize != nullptr && "titleBar or maximizeButton() returned a null pointer!");
            // QPoint   globalPos(mouse.x, mouse.y);
            // QWindow* handle{this->window()->windowHandle()};
            // if (handle && handle->screen())
            // {
            //     QScreen* screen = handle->screen();
            //     QPoint   offset = screen->geometry().topLeft();
            //     globalPos       = (globalPos - offset) / screen->devicePixelRatio() + offset;
            // }
            // const QPoint& localPos = maximize->mapFromGlobal(globalPos);
            // // 判断鼠标是否在最大化按钮上，如果在创建一个鼠标进入按钮的事件，发送给QT事件，否则QT无法收到这个事件 QSS失效
            // if (maximize->rect().contains(localPos))
            // {
            //     // 鼠标进入按钮事件和在按钮上移动事件
            //     QMouseEvent mouseEvent(maximize->underMouse() ? QEvent::MouseMove : QEvent::Enter,
            //                            localPos, globalPos,
            //                            Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            //     QCoreApplication::sendEvent(maximize, &mouseEvent);
            //     maximize->update();
            //     *_result = HTMAXBUTTON;
            //     return true;
            // }
            // else
            // {
            //     // 如果没有在最大化按钮上面查看鼠标是否移出了按钮，发送鼠标移出按钮事件
            //     if (maximize->underMouse())
            //     {
            //         QMouseEvent mouseEvent(QEvent::Leave, localPos, globalPos,
            //                                Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            //         QCoreApplication::sendEvent(maximize, &mouseEvent);
            //         maximize->update();
            //         return true;
            //     }
            // }

            // *_result = HTCLIENT;
            return false;
        }
            // 按下最大化按钮事件，发送一个鼠标按下事件
        case WM_NCLBUTTONDOWN:
        {
            // if (msg->wParam == HTMAXBUTTON)
            // {
            //     QAbstractButton* maximize = d_ptr->m_titleBar->maximizeButton();
            //     QMouseEvent      mouseEvent(QEvent::MouseButtonPress,
            //                                 QPoint(), QPoint(),
            //                                 Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            //     QCoreApplication::sendEvent(maximize, &mouseEvent);
            //     *_result = HTNOWHERE;
            //     return true;
            // }
        }
            // 鼠标释放事件，确保是一次完整的点击事件
        case WM_NCLBUTTONUP:
        {
            // if (msg->wParam == HTMAXBUTTON)
            // {
            //     QAbstractButton* maximize = d_ptr->m_titleBar->maximizeButton();
            //     QMouseEvent      mouseEvent(QEvent::MouseButtonRelease,
            //                                 QPoint(), QPoint(),
            //                                 Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            //     QCoreApplication::sendEvent(maximize, &mouseEvent);
            //     *_result = HTNOWHERE;
            //     return true;
            // }
        }
            // 这里是鼠标移入移出移动事件，上面已经处理过所以直接返回true 这个事件已经处理过 QT不必在处理
        case WM_NCMOUSEHOVER:
        case WM_NCMOUSELEAVE:
        case WM_NCMOUSEMOVE:
            if (msg->wParam == HTMAXBUTTON)
            {
                *_result = HTNOWHERE;
                return true;
            }
    }
    return false;
}
