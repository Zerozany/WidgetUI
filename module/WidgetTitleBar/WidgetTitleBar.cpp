#include "WidgetTitleBar.h"

#include <windows.h>

#include <QMouseEvent>
#include <QWindow>
#include <ranges>
#include <set>

#include "StyleLoader.h"
#include "WidgetFrame.h"

#ifndef _ButtonWidth
#define _ButtonWidth 45
#endif

#ifndef _ButtonIconSize
#define _ButtonIconSize {15, 15}
#endif

#ifndef BORDER_TOP_WIDTH
#define BORDER_TOP_WIDTH 5
#endif

#ifndef BORDER_BOTTOM_WIDTH
#define BORDER_BOTTOM_WIDTH 5
#endif

#ifndef BORDER_LEFT_WIDTH
#define BORDER_LEFT_WIDTH 5
#endif

#ifndef BORDER_RIGHT_WIDTH
#define BORDER_RIGHT_WIDTH 5
#endif

#ifndef CORNER_SIZE
#define CORNER_SIZE 7
#endif

constinit static QRect  g_startGeometry{}; /*当前窗口大小缓存区域*/
constinit static QPoint g_startPoint{};    /*开启伸缩标记点*/

WidgetTitleBar::WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent) : QWidget{_parent}, m_widget{_widget}
{
    std::invoke(&WidgetTitleBar::setMouseTracking, this, true);
    std::invoke(&WidgetTitleBar::initTitleBarHandle, this);
    std::invoke(&WidgetTitleBar::initTitleBarLayout, this);
    std::invoke(&WidgetTitleBar::connectSignalToSlot, this);
}

auto WidgetTitleBar::getMaximizeBtn() const noexcept -> QPushButton*
{
    return m_titleBarButtons.at("maximize");
}

auto WidgetTitleBar::getMinimizeBtn() const noexcept -> QPushButton*
{
    return m_titleBarButtons.at("minimize");
}

auto WidgetTitleBar::getCloseBtn() const noexcept -> QPushButton*
{
    return m_titleBarButtons.at("close");
}

auto WidgetTitleBar::getResizing() const noexcept -> bool
{
    return this->m_resizingTag;
}

auto WidgetTitleBar::initTitleBarHandle() noexcept -> void
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->setStyleSheet(StyleLoader::loadFromFile(R"(:/resources/css/WidgetTitleBar.css)"));
    // this->setStyleSheet("background-color: red;");
}

auto WidgetTitleBar::initTitleBarLayout() noexcept -> void
{
    std::set<QString> __titleBarIcons{
        ":/resources/icon/minimize.png",
        ":/resources/icon/maximize.png",
        ":/resources/icon/close.png",
    };

    std::set<QString> __titleBarProperty{
        "min",
        "max",
        "close",
    };

    for (const auto& [__btn, __icon, __property] : std::views::zip(m_titleBarButtons | std::views::values, __titleBarIcons, __titleBarProperty))
    {
        __btn->setFixedWidth(_ButtonWidth);
        __btn->setProperty("propertyName", __property);
        __btn->setIcon(QIcon{__icon});
        __btn->setIconSize(QSize(_ButtonIconSize));
        __btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    m_titleLayout->setContentsMargins(0, 0, 0, 0);
    m_titleLayout->setSpacing(0);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_titleBarButtons.at("minimize"));
    m_titleLayout->addWidget(m_titleBarButtons.at("maximize"));
    m_titleLayout->addWidget(m_titleBarButtons.at("close"));
}

auto WidgetTitleBar::setCursorType(const QPoint& _pos) noexcept -> void
{
    const int x{_pos.x()};
    const int y{_pos.y()};
    const int w{m_widget->width()};
    const int h{m_widget->height()};
    // 判断是否在角落（使用更大的 CORNER_SIZE）
    const bool topLeft{(x < CORNER_SIZE && y < CORNER_SIZE)};
    const bool topRight{(x > w - CORNER_SIZE && y < CORNER_SIZE)};
    const bool bottomLeft{(x < CORNER_SIZE && y > h - CORNER_SIZE)};
    const bool bottomRight{(x > w - CORNER_SIZE && y > h - CORNER_SIZE)};
    // 判断是否在边缘（使用较小的 BORDER_*_WIDTH）
    const bool left{(x < BORDER_LEFT_WIDTH) && !topLeft && !bottomLeft};
    const bool right{(x > w - BORDER_RIGHT_WIDTH) && !topRight && !bottomRight};
    const bool top{(y < BORDER_TOP_WIDTH) && !topLeft && !topRight};
    const bool bottom{(y > h - BORDER_BOTTOM_WIDTH) && !bottomLeft && !bottomRight};
    CursorType tmp{};
    if (topLeft)
    {
        tmp = CursorType::TopLeft;
    }
    else if (topRight)
    {
        tmp = CursorType::TopRight;
    }
    else if (bottomLeft)
    {
        tmp = CursorType::BottomLeft;
    }
    else if (bottomRight)
    {
        tmp = CursorType::BottomRight;
    }
    else if (top)
    {
        tmp = CursorType::Top;
    }
    else if (bottom)
    {
        tmp = CursorType::Bottom;
    }
    else if (left)
    {
        tmp = CursorType::Left;
    }
    else if (right)
    {
        tmp = CursorType::Right;
    }
    else
    {
        tmp = CursorType::None;
    }
    if (top || left || topLeft)
    {
        m_resizingTag = true;
    }
    else
    {
        m_resizingTag = false;
    }
    if (this->m_cursorType == tmp)
    {
        return;
    }
    this->m_cursorType = tmp;
    emit cursorType();
    if (this->m_cursorType == CursorType::None)
    {
        emit mouseLeave(false);
    }
    else
    {
        emit mouseLeave(true);
    }
}

auto WidgetTitleBar::connectSignalToSlot() noexcept -> void
{
    connect(m_titleBarButtons.at("minimize"), &QPushButton::clicked, this, &WidgetTitleBar::onMinimizeChanged);
    connect(m_titleBarButtons.at("maximize"), &QPushButton::clicked, this, &WidgetTitleBar::onMaximizeChanged);
    connect(this, &WidgetTitleBar::mousePress, this, &WidgetTitleBar::onMousePressChanged);
    connect(this, &WidgetTitleBar::mouseMove, this, &WidgetTitleBar::onMouseMoveChanged);
    connect(this, &WidgetTitleBar::mouseRelease, this, &WidgetTitleBar::onMouseReleaseChanged);
    connect(this, &WidgetTitleBar::cursorType, this, &WidgetTitleBar::onCursorTypeChanged);
    connect(this, &WidgetTitleBar::mouseLeave, this, &WidgetTitleBar::onMouseLeaveChanged);
    connect(this, &WidgetTitleBar::mouseDouble, this, &WidgetTitleBar::onMouseDoubleChanged);
}

void WidgetTitleBar::onMinimizeChanged() noexcept
{
    m_widget->showMinimized();
}

void WidgetTitleBar::onMousePressChanged(const QMouseEvent* _event) noexcept
{
    if ((_event->button() & Qt::LeftButton) && !m_widget->isMaximized() && m_cursorType != CursorType::None)
    {
        m_resizing      = true;
        g_startPoint    = _event->globalPos();
        g_startGeometry = m_widget->geometry();
    }
}

void WidgetTitleBar::onMouseMoveChanged(const QMouseEvent* _event) noexcept
{
    if (QRect end_geometry{g_startGeometry}; m_resizing)
    {
        switch (QPoint tmp_point{_event->globalPos() - g_startPoint}; m_cursorType)
        {
            case CursorType::Top:
            {
                end_geometry.setTop(end_geometry.top() + tmp_point.y());
                break;
            }
            case CursorType::Bottom:
            {
                end_geometry.setBottom(end_geometry.bottom() + tmp_point.y());
                break;
            }
            case CursorType::Left:
            {
                end_geometry.setLeft(end_geometry.left() + tmp_point.x());
                break;
            }
            case CursorType::Right:
            {
                end_geometry.setRight(end_geometry.right() + tmp_point.x());
                break;
            }
            case CursorType::TopLeft:
            {
                end_geometry.setTop(end_geometry.top() + tmp_point.y());
                end_geometry.setLeft(end_geometry.left() + tmp_point.x());
                break;
            }
            case CursorType::TopRight:
            {
                end_geometry.setTop(end_geometry.top() + tmp_point.y());
                end_geometry.setRight(end_geometry.right() + tmp_point.x());
                break;
            }
            case CursorType::BottomLeft:
            {
                end_geometry.setBottom(end_geometry.bottom() + tmp_point.y());
                end_geometry.setLeft(end_geometry.left() + tmp_point.x());
                break;
            }
            case CursorType::BottomRight:
            {
                end_geometry.setBottom(end_geometry.bottom() + tmp_point.y());
                end_geometry.setRight(end_geometry.right() + tmp_point.x());
                break;
            }
            default:
            {
                std::unreachable();
            }
        }
        if (end_geometry.width() >= m_widget->minimumWidth() && end_geometry.height() >= m_widget->minimumHeight())
        {
            m_widget->setGeometry(end_geometry);
        }
    }
    else
    {
        this->setCursorType(_event->pos());
    }
}

void WidgetTitleBar::onMouseReleaseChanged(const QMouseEvent* _event) noexcept
{
    this->m_resizing = false;
}

void WidgetTitleBar::onMouseDoubleChanged(const QMouseEvent* _event) noexcept
{
    if (_event->button() == Qt::LeftButton)
    {
        HWND hwnd{reinterpret_cast<HWND>(m_widget->window()->winId())};
        // 获取鼠标全局位置
        const QPoint globalPos{_event->globalPos()};
        // 转换为 Win32 坐标
        const LPARAM lParam{MAKELPARAM(globalPos.x(), globalPos.y())};
        // 双击窗口顶部边缘
        ::PostMessage(hwnd, WM_NCLBUTTONDBLCLK, HTTOP, lParam);
    }
}

void WidgetTitleBar::onCursorTypeChanged() noexcept
{
    if (m_widget->isMaximized())
    {
        return;
    }
    switch (this->m_cursorType)
    {
        case CursorType::Top:
        {
            [[fallthrough]];
        }
        case CursorType::Bottom:
        {
            m_widget->setCursor(Qt::SizeVerCursor);
            break;
        }
        case CursorType::Left:
        {
            [[fallthrough]];
        }
        case CursorType::Right:
        {
            m_widget->setCursor(Qt::SizeHorCursor);
            break;
        }
        case CursorType::TopLeft:
        {
            [[fallthrough]];
        }
        case CursorType::BottomRight:
        {
            m_widget->setCursor(Qt::SizeFDiagCursor);
            break;
        }
        case CursorType::TopRight:
        {
            [[fallthrough]];
        }
        case CursorType::BottomLeft:
        {
            m_widget->setCursor(Qt::SizeBDiagCursor);
            break;
        }
        default:
        {
            m_widget->setCursor(Qt::ArrowCursor);
            break;
        }
    }
}

void WidgetTitleBar::onMouseLeaveChanged(const bool _flag) noexcept
{
    if (_flag)
    {
        QEvent leaveEvent(QEvent::Leave);
        for (auto& __btn : m_titleBarButtons | std::views::values)
        {
            QApplication::sendEvent(__btn, &leaveEvent);
            __btn->update();
        }
    }
    this->setAttribute(Qt::WA_TransparentForMouseEvents, _flag);
}

void WidgetTitleBar::onMaximizeChanged() noexcept
{
    HWND hwnd{reinterpret_cast<HWND>(m_widget->winId())};
    if (::IsZoomed(hwnd))
    {
        m_titleBarButtons.at("maximize")->setIcon(QIcon{R"(:/resources/icon/maximize.png)"});
        ::ShowWindow(hwnd, SW_RESTORE);
    }
    else
    {
        m_titleBarButtons.at("maximize")->setIcon(QIcon{R"(:/resources/icon/normal.png)"});
        ::ShowWindow(hwnd, SW_MAXIMIZE);
    }
}

void WidgetTitleBar::onCloseChanged() noexcept
{
}
