#include "WidgetTitleBar.h"

#include <windows.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMouseEvent>
#include <QStyle>
#include <QTextStream>
#include <QWindow>
#include <ranges>
#include <set>

#include "StyleLoader.h"
#include "WidgetFrame.h"

constexpr const char* TitleBarConfigName{R"(TitleBarConfig.toml)"};

constexpr static QSize        BUTTON_ICON_SIZE{15, 15};
constexpr static std::uint8_t BUTTON_WIDTH{45};
constexpr static std::uint8_t BORDER_TOP_SIZE{5};
constexpr static std::uint8_t BORDER_BOTTOM_SIZE{5};
constexpr static std::uint8_t BORDER_LEFT_SIZE{5};
constexpr static std::uint8_t BORDER_RIGHT_SIZE{5};
constexpr static std::uint8_t CORNER_SIZE{8};
constexpr static std::uint8_t TITLEBAR_HEIGHT{30};

constinit static QRect  g_startGeometry{}; /*当前窗口大小缓存区域*/
constinit static QPoint g_startPoint{};    /*开启伸缩标记点*/
constinit static bool   g_resizing{false}; /*窗口伸缩句柄*/

WidgetTitleBar::WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent) : QWidget{_parent}, m_widget{_widget}
{
    std::invoke(&WidgetTitleBar::initTitleBarConfig, this);
    std::invoke(&WidgetTitleBar::initTitleBarHandle, this);
    std::invoke(&WidgetTitleBar::initTitleBarLayout, this);
    std::invoke(&WidgetTitleBar::connectSignalToSlot, this);
    /// @brief 暂无法对外开放
    Q_EMIT titleFlag(TitleFlags::IconHint | TitleFlags::TitleHint | TitleFlags::MinimizeHint | TitleFlags::MaximizeHint | TitleFlags::CloseHint);
}

/// @brief Q_PROPERTY
auto WidgetTitleBar::getMinimizeIcon() const noexcept -> QIcon
{
    return m_titleBarIcons.value("minimizeIcon");
}

auto WidgetTitleBar::setMinimizeIcon(const QIcon& _icon) noexcept -> void
{
    if (m_titleBarIcons.value("minimizeIcon").cacheKey() == _icon.cacheKey())
    {
        return;
    }
    m_titleBarIcons.value("minimizeIcon") = _icon;
    Q_EMIT this->minimizeIconChanged();
}

auto WidgetTitleBar::getMaximizeIcon() const noexcept -> QIcon
{
    return m_titleBarIcons.value("maximizeIcon");
}

auto WidgetTitleBar::setMaximizeIcon(const QIcon& _icon) noexcept -> void
{
    if (m_titleBarIcons.value("maximizeIcon").cacheKey() == _icon.cacheKey())
    {
        return;
    }
    m_titleBarIcons.value("maximizeIcon") = _icon;
    Q_EMIT this->maximizeIconChanged();
}

auto WidgetTitleBar::getNormalIcon() const noexcept -> QIcon
{
    return m_titleBarIcons.value("normalIcon");
}

auto WidgetTitleBar::setNormalIcon(const QIcon& _icon) noexcept -> void
{
    if (m_titleBarIcons.value("normalIcon").cacheKey() == _icon.cacheKey())
    {
        return;
    }
    m_titleBarIcons.value("normalIcon") = _icon;
    Q_EMIT this->normalIconChanged();
}

auto WidgetTitleBar::getCloseIcon() const noexcept -> QIcon
{
    return m_titleBarIcons.value("closeIcon");
}

auto WidgetTitleBar::setCloseIcon(const QIcon& _icon) noexcept -> void
{
    if (m_titleBarIcons.value("closeIcon").cacheKey() == _icon.cacheKey())
    {
        return;
    }
    m_titleBarIcons.value("closeIcon") = _icon;
    Q_EMIT this->closeIconChanged();
}

auto WidgetTitleBar::getWindowIcon() const noexcept -> QPixmap
{
    if (m_windowIcon->pixmap().isNull())
    {
        return QPixmap();
    }
    return m_windowIcon->pixmap();
}

auto WidgetTitleBar::setWindowIcon(const QPixmap& _pixmap) noexcept -> void
{
    if (m_windowIcon->pixmap().toImage() == _pixmap.toImage())
    {
        return;
    }
    m_windowIcon->setPixmap(_pixmap);
    Q_EMIT this->windowIconChanged();
}

auto WidgetTitleBar::getWindowTitle() const noexcept -> QString
{
    if (m_windowTitle->text().isEmpty())
    {
        return QString{};
    }
    return m_windowTitle->text();
}

auto WidgetTitleBar::setWindowTitle(const QString& _title) noexcept -> void
{
    if (m_windowTitle->text() == _title)
    {
        return;
    }
    m_windowTitle->setText(_title);
    Q_EMIT this->windowTitleChanged();
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

auto WidgetTitleBar::getResizeTag() const noexcept -> bool
{
    return this->m_resizeTag;
}

/*              初始化固定逻辑              */
auto WidgetTitleBar::initTitleBarHandle() noexcept -> void
{
    this->m_hwnd         = reinterpret_cast<HWND>(m_widget->winId());
    this->m_configLoader = new ConfigLoader{TitleBarConfigName, "Config", QCoreApplication::applicationDirPath(), this};
    this->setMouseTracking(true);
    this->setAttribute(Qt::WA_StyledBackground);
    this->setFixedHeight(TITLEBAR_HEIGHT);
    this->setStyleSheet(StyleLoader::loadFromFile(m_configLoader->loadFromFile("TitleBarStyleCss", "TitleBarStyle")));
}

auto WidgetTitleBar::initTitleBarLayout() noexcept -> void
{
    QList<QIcon> __titleBarIcons{
        m_titleBarIcons.value("closeIcon"),
        m_titleBarIcons.value("maximizeIcon"),
        m_titleBarIcons.value("minimizeIcon"),
    };

    std::set<QString> __titleBarProperty{
        m_configLoader->loadFromFile("minimize", "TitleBarButtonProperty"),
        m_configLoader->loadFromFile("maximize", "TitleBarButtonProperty"),
        m_configLoader->loadFromFile("close", "TitleBarButtonProperty"),
    };

    for (const auto& [__btn, __icon, __property] : std::views::zip(m_titleBarButtons | std::views::values, __titleBarIcons, __titleBarProperty))
    {
        __btn->setFixedWidth(BUTTON_WIDTH);
        __btn->setProperty("propertyName", __property);
        __btn->setIcon(__icon);
        __btn->setIconSize(QSize(BUTTON_ICON_SIZE));
        __btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    m_windowIcon->setFixedHeight(TITLEBAR_HEIGHT - (BORDER_TOP_SIZE * 2));
    m_windowTitle->setFixedHeight(TITLEBAR_HEIGHT - (BORDER_TOP_SIZE * 2));
    m_windowIcon->setPixmap(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon).pixmap(TITLEBAR_HEIGHT - (BORDER_TOP_SIZE * 2), TITLEBAR_HEIGHT - (BORDER_TOP_SIZE * 2)));
    m_windowTitle->setText(QApplication::applicationName());

    m_titleLayout->setContentsMargins(10, 0, 0, 0);
    m_titleLayout->setSpacing(0);
    m_titleLayout->addWidget(m_windowIcon);
    m_titleLayout->addSpacing(10);
    m_titleLayout->addWidget(m_windowTitle);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_titleBarButtons.at("minimize"));
    m_titleLayout->addWidget(m_titleBarButtons.at("maximize"));
    m_titleLayout->addWidget(m_titleBarButtons.at("close"));
}

auto WidgetTitleBar::initTitleBarConfig() noexcept -> void
{
    constexpr const char* TitleBarConfigText{
        "[TitleBarStyle]\n"
        "TitleBarStyleCss = \":/resources/css/WidgetTitleBar.css\"\n"
        "\n"
        "[TitleBarButtonProperty]\n"
        "minimize = \"min\"\n"
        "maximize = \"max\"\n"
        "close = \"close\"\n"};

    if (QDir configDir{"./config"}; !configDir.exists())
    {
        if (!configDir.mkpath("."))
        {
            qWarning() << "Failed to create config directory";
        }
    }
    QFile   configFile{"./config/TitleBarConfig.toml"};
    QString configText{};
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream rstream{&configFile};
        configText = rstream.readAll();
        configFile.close();
    }
    if (configText.trimmed().isEmpty())
    {
        if (configFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream wstream{&configFile};
            wstream << TitleBarConfigText;
            configFile.close();
        }
    }
}

auto WidgetTitleBar::setCursorType(const QPoint& _pos) noexcept -> void
{
    if (::IsZoomed(m_hwnd))
    {
        m_resizeTag = false;
        return;
    }
    // 判断是否在角落（使用更大的 CORNER_SIZE）
    const bool topLeft{(_pos.x() < CORNER_SIZE && _pos.y() < CORNER_SIZE)};
    const bool topRight{(_pos.x() > m_widget->width() - CORNER_SIZE && _pos.y() < CORNER_SIZE)};
    const bool bottomLeft{(_pos.x() < CORNER_SIZE && _pos.y() > m_widget->height() - CORNER_SIZE)};
    const bool bottomRight{(_pos.x() > m_widget->width() - CORNER_SIZE && _pos.y() > m_widget->height() - CORNER_SIZE)};
    // 判断是否在边缘（使用较小的 BORDER_*_WIDTH）
    const bool left{(_pos.x() < BORDER_LEFT_SIZE) && !topLeft && !bottomLeft};
    const bool right{(_pos.x() > m_widget->width() - BORDER_RIGHT_SIZE) && !topRight && !bottomRight};
    const bool top{(_pos.y() < BORDER_TOP_SIZE) && !topLeft && !topRight};
    const bool bottom{(_pos.y() > m_widget->height() - BORDER_BOTTOM_SIZE) && !bottomLeft && !bottomRight};
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
    if (tmp != CursorType::None)
    {
        m_resizeTag = true;
    }
    else
    {
        m_resizeTag = false;
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

auto WidgetTitleBar::connectSignalToSlot() noexcept -> void
{
    connect(m_titleBarButtons.at("minimize"), &QPushButton::clicked, this, &WidgetTitleBar::onMinimizeClicked);
    connect(m_titleBarButtons.at("maximize"), &QPushButton::clicked, this, &WidgetTitleBar::onMaximizeClicked);
    connect(m_titleBarButtons.at("close"), &QPushButton::clicked, this, &WidgetTitleBar::onCloseClicked);
    connect(this, &WidgetTitleBar::mousePress, this, &WidgetTitleBar::onMousePressChanged);
    connect(this, &WidgetTitleBar::mouseMove, this, &WidgetTitleBar::onMouseMoveChanged);
    connect(this, &WidgetTitleBar::mouseRelease, this, &WidgetTitleBar::onMouseReleaseChanged);
    connect(this, &WidgetTitleBar::cursorType, this, &WidgetTitleBar::onCursorTypeChanged);
    connect(this, &WidgetTitleBar::mouseLeave, this, &WidgetTitleBar::onMouseLeaveChanged);
    connect(this, &WidgetTitleBar::mouseDouble, this, &WidgetTitleBar::onMouseDoubleChanged);
    connect(this, &WidgetTitleBar::titleFlag, this, &WidgetTitleBar::onTitleFlagChanged);
}

void WidgetTitleBar::onMousePressChanged(const QMouseEvent* _event) noexcept
{
    /// @brief 窗口伸缩事件开启
    if ((_event->button() & Qt::LeftButton) && !::IsZoomed(m_hwnd) && m_cursorType != CursorType::None)
    {
        g_resizing      = true;
        g_startPoint    = _event->globalPos();
        g_startGeometry = m_widget->geometry();
    }
}

void WidgetTitleBar::onMouseMoveChanged(const QMouseEvent* _event) noexcept
{
    /// @brief 窗口伸缩事件
    if (QRect endGeometry{g_startGeometry}; g_resizing)
    {
        switch (QPoint tmpPoint{_event->globalPos() - g_startPoint}; m_cursorType)
        {
            case CursorType::Top:
            {
                endGeometry.setTop(endGeometry.top() + tmpPoint.y());
                break;
            }
            case CursorType::Bottom:
            {
                endGeometry.setBottom(endGeometry.bottom() + tmpPoint.y());
                break;
            }
            case CursorType::Left:
            {
                endGeometry.setLeft(endGeometry.left() + tmpPoint.x());
                break;
            }
            case CursorType::Right:
            {
                endGeometry.setRight(endGeometry.right() + tmpPoint.x());
                break;
            }
            case CursorType::TopLeft:
            {
                endGeometry.setTop(endGeometry.top() + tmpPoint.y());
                endGeometry.setLeft(endGeometry.left() + tmpPoint.x());
                break;
            }
            case CursorType::TopRight:
            {
                endGeometry.setTop(endGeometry.top() + tmpPoint.y());
                endGeometry.setRight(endGeometry.right() + tmpPoint.x());
                break;
            }
            case CursorType::BottomLeft:
            {
                endGeometry.setBottom(endGeometry.bottom() + tmpPoint.y());
                endGeometry.setLeft(endGeometry.left() + tmpPoint.x());
                break;
            }
            case CursorType::BottomRight:
            {
                endGeometry.setBottom(endGeometry.bottom() + tmpPoint.y());
                endGeometry.setRight(endGeometry.right() + tmpPoint.x());
                break;
            }
            default:
            {
                std::unreachable();
            }
        }
        if (endGeometry.width() >= m_widget->minimumWidth() && endGeometry.height() >= m_widget->minimumHeight())
        {
            m_widget->setGeometry(endGeometry);
        }
    }
    else
    {
        this->setCursorType(_event->pos());
    }
}

void WidgetTitleBar::onMouseReleaseChanged(const QMouseEvent* _event) noexcept
{
    /// @brief 窗口伸缩事件关闭
    g_resizing = false;
}

void WidgetTitleBar::onMouseDoubleChanged(const QMouseEvent* _event) noexcept
{
    /// @brief 窗口顶部底部双击伸缩事件
    if (_event->button() == Qt::LeftButton && (m_cursorType == CursorType::Top || m_cursorType == CursorType::Bottom))
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
    /// @brief 光标样式监听
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
    /// @brief 窗口伸缩光标开启时使标题栏以及标题栏按钮失效
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

void WidgetTitleBar::onTitleFlagChanged(const TitleFlags& _flag) noexcept
{
    this->m_titleBarButtons.at("minimize")->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::MinimizeHint);
    this->m_titleBarButtons.at("maximize")->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::MaximizeHint);
    this->m_titleBarButtons.at("close")->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::CloseHint);
    this->m_windowIcon->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::IconHint);
    this->m_windowTitle->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::TitleHint);
}

void WidgetTitleBar::onMinimizeClicked() noexcept
{
    m_widget->showMinimized();
}

void WidgetTitleBar::onMaximizeClicked() noexcept
{
    if (::IsZoomed(this->m_hwnd))
    {
        m_titleBarButtons.at("maximize")->setIcon(m_titleBarIcons.value("maximizeIcon"));
        ::ShowWindow(this->m_hwnd, SW_RESTORE);
    }
    else
    {
        m_titleBarButtons.at("maximize")->setIcon(m_titleBarIcons.value("normalIcon"));
        ::ShowWindow(this->m_hwnd, SW_MAXIMIZE);
    }
}

void WidgetTitleBar::onCloseClicked() noexcept
{
    m_widget->close();
}
