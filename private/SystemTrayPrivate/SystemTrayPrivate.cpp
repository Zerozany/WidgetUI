#include "SystemTrayPrivate.h"

#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QWidget>
#include <ranges>

SystemTrayPrivate::SystemTrayPrivate(QWidget* _parentWindow, QObject* _parent)
    : QObject(_parent), m_parentWindow(_parentWindow)
{
    std::invoke(&SystemTrayPrivate::initSystemTray, this);
}

auto SystemTrayPrivate::setTrayIcon(const QIcon& _icon) noexcept -> void
{
    m_trayIcon->setIcon(_icon);
}

auto SystemTrayPrivate::show() noexcept -> void
{
    m_trayIcon->show();
}

auto SystemTrayPrivate::addTrayAction(QAction* _action) noexcept -> void
{
    m_trayMenu->addAction(_action);
    m_trayMenu->addSeparator();
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
        qDebug() << "当前系统不支持托盘";
        return;
    }
    if (!m_trayIcon->supportsMessages())
    {
        qWarning() << "当前系统不支持托盘消息！";
    }
    m_trayIcon = new QSystemTrayIcon{m_parentWindow};
    m_trayMenu = new QMenu{};
    m_trayIcon->setContextMenu(m_trayMenu);
    // m_trayIcon->showMessage("提示", "程序已最小化到托盘！", QSystemTrayIcon::Information, 3000);
}
