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

auto SystemTray::setTrayMessage(const QString& _title, const QString& _msg, int _msecs) noexcept -> void
{
    Q_D(SystemTray);
    d->setTrayMessage(_title, _msg, _msecs);
}
