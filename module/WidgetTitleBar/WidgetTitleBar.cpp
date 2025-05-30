#include "WidgetTitleBar.h"

#include <windows.h>

#include <QMouseEvent>
#include <QWindow>
#include <ranges>
#include <set>

#include "StyleLoader.h"
#include "WidgetFrame.h"

constexpr static QSize        BUTTON_ICON_SIZE{15, 15};
constexpr static std::uint8_t BUTTON_WIDTH{45};
constexpr static std::uint8_t BORDER_TOP_WIDTH{5};
constexpr static std::uint8_t BORDER_BOTTOM_WIDTH{5};
constexpr static std::uint8_t BORDER_LEFT_WIDTH{5};
constexpr static std::uint8_t BORDER_RIGHT_WIDTH{5};
constexpr static std::uint8_t CORNER_SIZE{5};

constinit static QRect  g_startGeometry{}; /*当前窗口大小缓存区域*/
constinit static QPoint g_startPoint{};    /*开启伸缩标记点*/

WidgetTitleBar::WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent) : QWidget{_parent}, m_widget{_widget}
{
    std::invoke(&WidgetTitleBar::initTitleBarHandle, this);
    std::invoke(&WidgetTitleBar::initTitleBarLayout, this);
    std::invoke(&WidgetTitleBar::connectSignalToSlot, this);
}

auto WidgetTitleBar::setCursorType(const QPoint& _pos) noexcept -> void
{
    // 判断是否在角落（使用更大的 CORNER_SIZE）
    const bool topLeft{(_pos.x() < CORNER_SIZE && _pos.y() < CORNER_SIZE)};
    const bool topRight{(_pos.x() > m_widget->width() - CORNER_SIZE && _pos.y() < CORNER_SIZE)};
    const bool bottomLeft{(_pos.x() < CORNER_SIZE && _pos.y() > m_widget->height() - CORNER_SIZE)};
    const bool bottomRight{(_pos.x() > m_widget->width() - CORNER_SIZE && _pos.y() > m_widget->height() - CORNER_SIZE)};
    // 判断是否在边缘（使用较小的 BORDER_*_WIDTH）
    const bool left{(_pos.x() < BORDER_LEFT_WIDTH) && !topLeft && !bottomLeft};
    const bool right{(_pos.x() > m_widget->width() - BORDER_RIGHT_WIDTH) && !topRight && !bottomRight};
    const bool top{(_pos.y() < BORDER_TOP_WIDTH) && !topLeft && !topRight};
    const bool bottom{(_pos.y() > m_widget->height() - BORDER_BOTTOM_WIDTH) && !bottomLeft && !bottomRight};
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
    if (top || left || topLeft && ::IsZoomed(m_hwnd))
    {
        m_resizingTag = true;
    }
    else
    {
        m_resizingTag = false;
    }
    if (this->m_cursorType != tmp)
    {
        this->m_cursorType = tmp;
        Q_EMIT cursorType(this->m_cursorType);
    }
    if (this->m_cursorType == CursorType::None)
    {
        Q_EMIT mouseLeave(false);
    }
    else
    {
        Q_EMIT mouseLeave(true);
    }
}

void WidgetTitleBar::onMinimizeChanged() noexcept
{
    m_widget->showMinimized();
}

void WidgetTitleBar::onMousePressChanged(const QMouseEvent* _event) noexcept
{
    if ((_event->button() & Qt::LeftButton) && !::IsZoomed(m_hwnd) && m_cursorType != CursorType::None)
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
        // 恢复鼠标至默认状态
        m_widget->setCursor(Qt::ArrowCursor);
    }
}

void WidgetTitleBar::onCursorTypeChanged(const CursorType& _cursorTyupe) noexcept
{
    if (m_widget->isMaximized() || (QApplication::mouseButtons() & Qt::LeftButton))
    {
        return;
    }
    switch (_cursorTyupe)
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
    if (::IsZoomed(this->m_hwnd))
    {
        m_titleBarButtons.at("maximize")->setIcon(QIcon{R"(:/resources/icon/maximize.png)"});
        ::ShowWindow(this->m_hwnd, SW_RESTORE);
    }
    else
    {
        m_titleBarButtons.at("maximize")->setIcon(QIcon{R"(:/resources/icon/normal.png)"});
        ::ShowWindow(this->m_hwnd, SW_MAXIMIZE);
    }
}

void WidgetTitleBar::onCloseChanged() noexcept
{
    m_widget->close();
}

auto WidgetTitleBar::getResizing() const noexcept -> bool
{
    return this->m_resizingTag;
}

/*              初始化固定逻辑              */
auto WidgetTitleBar::initTitleBarHandle() noexcept -> void
{
    this->setMouseTracking(true);
    this->setAttribute(Qt::WA_StyledBackground);
    this->setStyleSheet(StyleLoader::loadFromFile(R"(:/resources/css/WidgetTitleBar.css)"));
    this->m_hwnd = reinterpret_cast<HWND>(m_widget->winId());
}

auto WidgetTitleBar::getMaximizeBtn() const noexcept -> QPushButton*
{
    return this->m_titleBarButtons.at("maximize");
}

auto WidgetTitleBar::getMinimizeBtn() const noexcept -> QPushButton*
{
    return this->m_titleBarButtons.at("minimize");
}

auto WidgetTitleBar::getCloseBtn() const noexcept -> QPushButton*
{
    return this->m_titleBarButtons.at("close");
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
        __btn->setFixedWidth(BUTTON_WIDTH);
        __btn->setProperty("propertyName", __property);
        __btn->setIcon(QIcon{__icon});
        __btn->setIconSize(QSize(BUTTON_ICON_SIZE));
        __btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    m_titleLayout->setContentsMargins(0, 0, 0, 0);
    m_titleLayout->setSpacing(0);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_titleBarButtons.at("minimize"));
    m_titleLayout->addWidget(m_titleBarButtons.at("maximize"));
    m_titleLayout->addWidget(m_titleBarButtons.at("close"));
}

auto WidgetTitleBar::connectSignalToSlot() noexcept -> void
{
    connect(m_titleBarButtons.at("minimize"), &QPushButton::clicked, this, &WidgetTitleBar::onMinimizeChanged);
    connect(m_titleBarButtons.at("maximize"), &QPushButton::clicked, this, &WidgetTitleBar::onMaximizeChanged);
    connect(m_titleBarButtons.at("close"), &QPushButton::clicked, this, &WidgetTitleBar::onCloseChanged);
    connect(this, &WidgetTitleBar::mousePress, this, &WidgetTitleBar::onMousePressChanged);
    connect(this, &WidgetTitleBar::mouseMove, this, &WidgetTitleBar::onMouseMoveChanged);
    connect(this, &WidgetTitleBar::mouseRelease, this, &WidgetTitleBar::onMouseReleaseChanged);
    connect(this, &WidgetTitleBar::cursorType, this, &WidgetTitleBar::onCursorTypeChanged);
    connect(this, &WidgetTitleBar::mouseLeave, this, &WidgetTitleBar::onMouseLeaveChanged);
    connect(this, &WidgetTitleBar::mouseDouble, this, &WidgetTitleBar::onMouseDoubleChanged);
}
