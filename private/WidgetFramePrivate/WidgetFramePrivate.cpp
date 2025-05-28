#include "WidgetFramePrivate.h"

#include "WidgetFrame.h"
#include "WidgetTitleBar.h"

WidgetFramePrivate::WidgetFramePrivate(WidgetFrame* _q, QObject* _parent) : QObject{_parent}, q_ptr{_q}
{
    std::invoke(&WidgetFramePrivate::initWidgetFrame, this);
    std::invoke(&WidgetFramePrivate::initWidgetLayout, this);
}

auto WidgetFramePrivate::initWidgetFrame() noexcept -> void
{
    Q_Q(WidgetFrame);
    /// @brief 启用鼠标实时监听
    q->setMouseTracking(true);
    /// @brief 移除窗口的默认标题栏和边框
    q->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    /// @brief 设置背景透明
    // q->setAttribute(Qt::WA_TranslucentBackground);
    /// @brief 绑定窗口句柄
    HWND hwnd{(HWND)q->winId()};
    LONG style{::GetWindowLongW(hwnd, GWL_STYLE)};
    /// @brief 启用最大化按钮
    /// @brief 显示标题栏
    /// @brief 允许窗口调整大小
    ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | CS_DBLCLKS | WS_THICKFRAME);
}

auto WidgetFramePrivate::initWidgetLayout() noexcept -> void
{
    Q_Q(WidgetFrame);
    m_mainLayout = new QVBoxLayout{q};
    m_titleBar   = new WidgetTitleBar{q};
    // 设置 layout 和样式
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    q->setLayout(m_mainLayout);
    // 设置 titleBar 的固定高度
    m_titleBar->setFixedHeight(30);
    // 添加控件到布局
    m_mainLayout->addWidget(m_titleBar);
    m_mainLayout->addStretch();
}
