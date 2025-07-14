#include "SystemTrayPrivate.h"

#include <QAction>
#include <QMenu>
#include <QWidget>

#include "StyleLoader.h"
#include "SystemTray.h"

SystemTrayPrivate::SystemTrayPrivate(QWidget* _parentWindow, QObject* _parent)
    : QObject(_parent), m_parentWindow(_parentWindow)
{
    std::invoke(&SystemTrayPrivate::initSystemTray, this);
    std::invoke(&SystemTrayPrivate::connectSignalToSlot, this);
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
}

auto SystemTrayPrivate::setTrayMessage(const QString& _title, const QString& _msg, int _msecs) noexcept -> void
{
    m_trayIcon->showMessage(_title, _msg, QSystemTrayIcon::Information, _msecs);
}

auto SystemTrayPrivate::setMenuStyleSheet(const QString& _styleStr) noexcept -> void
{
    m_trayMenu->setStyleSheet(_styleStr);
}

auto SystemTrayPrivate::initSystemTray() noexcept -> void
{
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        qDebug() << tr("当前系统不支持托盘");
        return;
    }
    if (!m_trayIcon->supportsMessages())
    {
        qDebug() << tr("当前系统不支持托盘消息！");
    }
    m_trayIcon = new QSystemTrayIcon{m_parentWindow};
    m_trayMenu = new QMenu{};
    m_trayMenu->setStyleSheet(StyleLoader::loadFromFile(":/resources/css/SystemTrayCss/SystemTray.css"));
    m_trayIcon->setContextMenu(m_trayMenu);
}

auto SystemTrayPrivate::connectSignalToSlot() noexcept -> void
{
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayPrivate::onTrayIconActivated);
}

void SystemTrayPrivate::onTrayIconActivated(QSystemTrayIcon::ActivationReason _reason)
{
    if (_reason == QSystemTrayIcon::Trigger)
    {
        if (m_parentWindow->isMinimized() || !m_parentWindow->isVisible())
        {
            m_parentWindow->showNormal();
        }
        m_parentWindow->activateWindow();
        m_parentWindow->raise();
    }
    else if (_reason == QSystemTrayIcon::Context)
    {
        m_trayMenu->popup(QCursor::pos());
    }
}
