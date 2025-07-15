#include "SystemTray.h"

#include "SystemTrayPrivate.h"

SystemTray::SystemTray(QWidget* _parentWindow, QObject* _parent)
    : QObject(_parent), d_ptr(new SystemTrayPrivate(_parentWindow, this))
{
}

SystemTray::SystemTray(QWidget* _parentWindow, QIcon& _icon, QObject* _parent)
    : QObject(_parent), d_ptr(new SystemTrayPrivate(_parentWindow, this))
{
    this->setTrayIcon(_icon);
}

SystemTray::~SystemTray() noexcept
{
    Q_D(SystemTray);
    delete d;
}

auto SystemTray::setTrayIcon(const QIcon& _icon) noexcept -> void
{
    Q_D(SystemTray);
    d->setTrayIcon(_icon);
}

auto SystemTray::show() noexcept -> void
{
    Q_D(SystemTray);
    d->show();
}

auto SystemTray::addTrayAction(QAction* _action) noexcept -> void
{
    Q_D(SystemTray);
    d->addTrayAction(_action);
}

auto SystemTray::setMenuStyleSheet(const QString& _styleStr) noexcept -> void
{
    Q_D(SystemTray);
    d->setMenuStyleSheet(_styleStr);
}

auto SystemTray::getTrayInstance() noexcept -> QSystemTrayIcon* const
{
    Q_D(SystemTray);
    return d->getTrayInstance();
}
