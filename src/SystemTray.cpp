#include "SystemTray.h"

#include "SystemTrayPrivate.h"

SystemTray::SystemTray(QWidget* _parentWindow, QObject* _parent)
    : QObject(_parent), d_ptr(new SystemTrayPrivate(_parentWindow, this))
{
}

SystemTray::~SystemTray() noexcept
{
    Q_D(SystemTray);
    delete d;
}
