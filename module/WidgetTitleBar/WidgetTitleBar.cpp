#include "WidgetTitleBar.h"

#include <windows.h>

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

WidgetTitleBar::WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent) : QWidget{_parent}, m_widget{_widget}
{
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

auto WidgetTitleBar::initTitleBarHandle() noexcept -> void
{
    this->setAttribute(Qt::WA_StyledBackground);
    this->setMouseTracking(true);
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

auto WidgetTitleBar::connectSignalToSlot() noexcept -> void
{
    connect(m_titleBarButtons.at("minimize"), &QPushButton::clicked, this, &WidgetTitleBar::onMinimizeChanged);
    connect(m_titleBarButtons.at("maximize"), &QPushButton::clicked, this, &WidgetTitleBar::onMaximizeChanged);
}

void WidgetTitleBar::onMinimizeChanged() noexcept
{
    m_widget->showMinimized();
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
