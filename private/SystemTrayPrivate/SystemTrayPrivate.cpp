#include "SystemTrayPrivate.h"

#include <QMenu>
#include <QSystemTrayIcon>
#include <QWidget>
#include <ranges>

SystemTrayPrivate::SystemTrayPrivate(QWidget* _parentWindow, QObject* _parent)
    : QObject(_parent), m_parentWindow(_parentWindow)
{
    m_trayIcon = new QSystemTrayIcon{m_parentWindow};
    m_trayMenu = new QMenu{};
    std::invoke(&SystemTrayPrivate::setTrayIcon, this, QIcon{":/resources/icon/normal.png"});
    std::invoke(&SystemTrayPrivate::initSystemTray, this);
}

auto SystemTrayPrivate::setTrayIcon(const QIcon& _icon) noexcept -> void
{
    m_trayIcon->setIcon(_icon);
}

auto SystemTrayPrivate::initSystemTray() noexcept -> void
{
    // macOS特殊处理
#ifdef Q_OS_MAC
    // macOS需要设置以下属性才能正确显示菜单
    qt_mac_set_dock_menu(m_trayMenu);

    // 禁用macOS的默认"显示"菜单项
    QMenu::setDefaultAction(nullptr);
#endif
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        qDebug() << "Error: The system does not support tray function";
        return;
    }
    m_trayActions.value("restore")->setText(tr("设置"));
    m_trayActions.value("quit")->setText(tr("退出"));
    for (const auto _actions : m_trayActions.toStdMap() | std::views::values)
    {
        m_trayMenu->addAction(_actions);
        m_trayMenu->addSeparator();
    }
    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
}
