#include "WidgetTitleBar.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

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

constexpr static std::string_view TitleBarConfigName{"TitleBarConfig.toml"};

constexpr static QSize        BUTTON_ICON_SIZE{15, 15};
constexpr static std::uint8_t BUTTON_WIDTH{45};
constexpr static std::uint8_t BORDER_TOP_SIZE{5};
constexpr static std::uint8_t BORDER_BOTTOM_SIZE{5};
constexpr static std::uint8_t BORDER_RIGHT_SIZE{5};
constexpr static std::uint8_t BORDER_LEFT_SIZE{5};
constexpr static std::uint8_t CORNER_SIZE{8};
constexpr static std::uint8_t TITLEBAR_HEIGHT{35};

constinit static QRect  g_startGeometry{}; /*当前窗口大小缓存区域*/
constinit static QPoint g_startPoint{};    /*开启伸缩标记点*/
constinit static bool   g_resizing{false}; /*窗口伸缩句柄*/

WidgetTitleBar::WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent) : QWidget{_parent}, m_widget{_widget}
{
    std::invoke(&WidgetTitleBar::initTitleBarConfig, this);
    std::invoke(&WidgetTitleBar::initTitleBarHandle, this);
    std::invoke(&WidgetTitleBar::initTitleBarLayout, this);
    std::invoke(&WidgetTitleBar::connectSignalToSlot, this);
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
        return QPixmap{};
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

auto WidgetTitleBar::setWindowCursor(const QPixmap& _arrow, const QPixmap& _sizeVer, const QPixmap& _sizeHor, const QPixmap& _sizeFDiag, const QPixmap& _sizeBDiag) const noexcept -> void
{
    if (_arrow.isNull())
    {
        m_cursorCursors.value("arrow") = Qt::ArrowCursor;
    }
    else
    {
        QPoint arrowPoint{_arrow.width() / 2, _arrow.height() / 2};
        m_cursorCursors.value("arrow") = QCursor{_arrow, arrowPoint.x(), arrowPoint.y()};
    }
    if (_sizeVer.isNull())
    {
        m_cursorCursors.value("sizeVer") = Qt::SizeVerCursor;
    }
    else
    {
        QPoint sizeVerPoint{_sizeVer.width() / 2, _sizeVer.height() / 2};
        m_cursorCursors.value("sizeVer") = QCursor{_sizeVer, sizeVerPoint.x(), sizeVerPoint.y()};
    }
    if (_sizeHor.isNull())
    {
        m_cursorCursors.value("sizeHor") = Qt::SizeHorCursor;
    }
    else
    {
        QPoint sizeHorPoint{_sizeHor.width() / 2, _sizeHor.height() / 2};
        m_cursorCursors.value("sizeHor") = QCursor{_sizeHor, sizeHorPoint.x(), sizeHorPoint.y()};
    }
    if (_sizeFDiag.isNull())
    {
        m_cursorCursors.value("sizeFDiag") = Qt::SizeFDiagCursor;
    }
    else
    {
        QPoint sizeFDiagPoint{_sizeFDiag.width() / 2, _sizeFDiag.height() / 2};
        m_cursorCursors.value("sizeFDiag") = QCursor{_sizeFDiag, sizeFDiagPoint.x(), sizeFDiagPoint.y()};
    }
    if (_sizeBDiag.isNull())
    {
        m_cursorCursors.value("sizeBDiag") = Qt::SizeBDiagCursor;
    }
    else
    {
        QPoint sizeBDiagPoint{_sizeBDiag.width() / 2, _sizeBDiag.height() / 2};
        m_cursorCursors.value("sizeBDiag") = QCursor{_sizeBDiag, sizeBDiagPoint.x(), sizeBDiagPoint.y()};
    }
}

auto WidgetTitleBar::addTitleAction(QWidget* _action) noexcept -> void
{
    _action->setFixedHeight(TITLEBAR_HEIGHT - (BORDER_TOP_SIZE * 2));
    m_titleBarLayouts.at("titleActionLayout")->addWidget(_action);
}

auto WidgetTitleBar::addTitleState(QWidget* _action) noexcept -> void
{
    _action->setFixedHeight(TITLEBAR_HEIGHT - (BORDER_TOP_SIZE * 2));
    m_titleBarLayouts.at("titleStateLayout")->addWidget(_action);
}

/*              初始化固定逻辑              */
auto WidgetTitleBar::initTitleBarHandle() noexcept -> void
{
#ifdef Q_OS_WIN
    this->m_hwnd = reinterpret_cast<HWND>(m_widget->winId());
#endif
    this->m_configLoader = new ConfigLoader{TitleBarConfigName.data(), "Config", QCoreApplication::applicationDirPath(), this};
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
    m_windowIcon->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_windowTitle->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_titleBarLayouts.at("titleActionLayout")->setSpacing(10);
    m_titleBarLayouts.at("titleStateLayout")->setSpacing(0);
    m_titleBarLayouts.at("titleActionLayout")->setContentsMargins(5, 0, 0, 0);
    m_titleBarLayouts.at("titleStateLayout")->setContentsMargins(0, 0, 0, 0);
    m_titleBarLayouts.at("titleBarLayout")->setContentsMargins(0, 0, 0, 0);

    m_titleBarLayouts.at("titleActionLayout")->addWidget(m_windowIcon);
    m_titleBarLayouts.at("titleActionLayout")->addWidget(m_windowTitle);

    m_titleBarLayouts.at("titleStateLayout")->setDirection(QBoxLayout::RightToLeft);
    m_titleBarLayouts.at("titleStateLayout")->addWidget(m_titleBarButtons.at("close"));
    m_titleBarLayouts.at("titleStateLayout")->addWidget(m_titleBarButtons.at("maximize"));
    m_titleBarLayouts.at("titleStateLayout")->addWidget(m_titleBarButtons.at("minimize"));

    m_titleBarLayouts.at("titleBarLayout")->addLayout(m_titleBarLayouts.at("titleActionLayout"));
    m_titleBarLayouts.at("titleBarLayout")->addStretch();
    m_titleBarLayouts.at("titleBarLayout")->addLayout(m_titleBarLayouts.at("titleStateLayout"));
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
        "close = \"close\"\n",
    };

    if (QDir configDir{"./config"}; !configDir.exists())
    {
        Q_UNUSED(configDir.mkpath("."));
    }
    QFile configFile{QStringLiteral("./config/") + QString::fromUtf8(TitleBarConfigName)};
    bool  hasContent{};
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream{&configFile};
        hasContent = stream.readLine().trimmed().isEmpty();
        configFile.close();
    }
    if (hasContent)
    {
        return;
    }
    if (configFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream wstream{&configFile};
        wstream << TitleBarConfigText;
        configFile.close();
    }
}

auto WidgetTitleBar::setCursorType(const QPoint& _pos) noexcept -> void
{
#ifdef Q_OS_WIN
    if (::IsZoomed(m_hwnd))
    {
        m_resizeTag = false;
        return;
    }
#endif
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
        Q_EMIT cursorTypeChanged();
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
    connect(m_titleBarButtons.at("minimize"), &QPushButton::clicked, this, &WidgetTitleBar::onMinimizeClicked, Qt::AutoConnection);
    connect(m_titleBarButtons.at("maximize"), &QPushButton::clicked, this, &WidgetTitleBar::onMaximizeClicked, Qt::AutoConnection);
    connect(m_titleBarButtons.at("close"), &QPushButton::clicked, this, &WidgetTitleBar::onCloseClicked);
    connect(this, &WidgetTitleBar::mousePress, this, &WidgetTitleBar::onMousePressChanged, Qt::AutoConnection);
    connect(this, &WidgetTitleBar::mouseMove, this, &WidgetTitleBar::onMouseMoveChanged, Qt::AutoConnection);
    connect(this, &WidgetTitleBar::mouseRelease, this, &WidgetTitleBar::onMouseReleaseChanged, Qt::AutoConnection);
    connect(this, &WidgetTitleBar::cursorTypeChanged, this, &WidgetTitleBar::onCursorTypeChanged, Qt::AutoConnection);
    connect(this, &WidgetTitleBar::mouseLeave, this, &WidgetTitleBar::onMouseLeaveChanged, Qt::AutoConnection);
    connect(this, &WidgetTitleBar::mouseDouble, this, &WidgetTitleBar::onMouseDoubleChanged, Qt::AutoConnection);
    connect(this, &WidgetTitleBar::titleFlagChanged, this, &WidgetTitleBar::onTitleFlagChanged, Qt::AutoConnection);
}

void WidgetTitleBar::onMousePressChanged(const QMouseEvent* _event) noexcept
{
#ifdef Q_OS_WIN
    /// @brief 窗口伸缩事件开启
    if ((_event->button() & Qt::LeftButton) && !::IsZoomed(m_hwnd) && m_cursorType != CursorType::None)
    {
        g_resizing      = true;
        g_startPoint    = _event->globalPos();
        g_startGeometry = m_widget->geometry();
    }
#endif
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
#ifdef Q_OS_WIN
        HWND hwnd{reinterpret_cast<HWND>(m_widget->window()->winId())};
        // 获取鼠标全局位置
        const QPoint globalPos{_event->globalPos()};
        // 转换为 Win32 坐标
        const LPARAM lParam{MAKELPARAM(globalPos.x(), globalPos.y())};
        // 双击窗口顶部边缘
        ::PostMessage(hwnd, WM_NCLBUTTONDBLCLK, HTTOP, lParam);
#endif
        // 恢复鼠标至默认状态
        m_widget->setCursor(Qt::ArrowCursor);
    }
}

void WidgetTitleBar::onCursorTypeChanged() noexcept
{
    /// @brief 光标样式监听
    if (m_widget->isMaximized() || (QApplication::mouseButtons() & Qt::LeftButton))
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

void WidgetTitleBar::onTitleFlagChanged(const char _flag) noexcept
{
    this->m_windowIcon->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::IconHint);
    this->m_windowTitle->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::TitleHint);
    this->m_titleBarButtons.at("minimize")->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::MinimizeHint);
    this->m_titleBarButtons.at("maximize")->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::MaximizeHint);
    this->m_titleBarButtons.at("close")->setVisible(static_cast<TitleFlags>(_flag) & TitleFlags::CloseHint);
}

void WidgetTitleBar::onMinimizeClicked() noexcept
{
    m_widget->showMinimized();
}

void WidgetTitleBar::onMaximizeClicked() noexcept
{
#ifdef Q_OS_WIN
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
#endif
}

void WidgetTitleBar::onCloseClicked() noexcept
{
    m_widget->close();
}
