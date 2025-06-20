#include "SystemTrayPrivate.h"

#include <QAction>
#include <QMenu>
#include <QWidget>
#include <ranges>

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

auto SystemTrayPrivate::initSystemTray() noexcept -> void
{
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        qDebug() << "当前系统不支持托盘";
        return;
    }
    if (!m_trayIcon->supportsMessages())
    {
        qDebug() << "当前系统不支持托盘消息！";
    }
    m_trayIcon = new QSystemTrayIcon{m_parentWindow};
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
            m_parentWindow->activateWindow();
        }
        else
        {
            m_parentWindow->activateWindow();
        }
        m_parentWindow->raise();
    }
    else if (_reason == QSystemTrayIcon::Context)
    {
        // 右键
    }
}
