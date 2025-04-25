#include "WidgetFramePrivate.h"

#include "WidgetFrame.h"
#include "WidgetTitleBar.h"

WidgetFramePrivate::WidgetFramePrivate(WidgetFrame* _q, QObject* _parent) : QObject{_parent}, q_ptr{_q}
{
    std::invoke(&WidgetFramePrivate::initWidgetHandle, this);
    std::invoke(&WidgetFramePrivate::initWidgetFrame, this);
}

auto WidgetFramePrivate::initWidgetFrame() noexcept -> void
{
    Q_Q(WidgetFrame);
    m_titleBar = new WidgetTitleBar{q};
}

auto WidgetFramePrivate::initWidgetHandle() noexcept -> void
{
    Q_Q(WidgetFrame);
    /// @brief 开启鼠标活动监听
    q->setMouseTracking(true);
    /// @brief 启用窗口的最大化按钮
    /// @brief 启用窗口的最小化按钮
    /// @brief 移除窗口的默认标题栏和边框
    q->setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);
    /// @brief 绑定窗口句柄
    HWND hwnd{(HWND)q->winId()};
    LONG style{GetWindowLongW(hwnd, GWL_STYLE)};
    /// @brief 启用最小化按钮
    /// @brief 启用最大化按钮
    /// @brief 显示标题栏
    /// @brief 允许鼠标双击事件
    /// @brief 允许窗口调整大小
    SetWindowLong(hwnd, GWL_STYLE, style | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | CS_DBLCLKS | WS_THICKFRAME);
}

auto WidgetFramePrivate::conncetSignalsToSlots() noexcept -> void
{
}
