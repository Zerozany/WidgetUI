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

auto SystemTrayPrivate::addTrayAction(QAction* _action) noexcept -> void
{
    m_trayMenu->addAction(_action);
    m_trayMenu->addSeparator();
}

auto SystemTrayPrivate::initSystemTray() noexcept -> void
{
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
    //     m_trayMenu->setStyleSheet(R"(
    //     QMenu {
    //         background-color: #333;      /* 背景色 */
    //         color: white;                /* 字体颜色 */
    //         border: 1px solid #555;      /* 边框 */
    //     }
    //     QMenu::item {
    //         padding: 5px 20px;           /* 项目内边距 */
    //     }
    //     QMenu::item:selected {
    //         background-color: #666;      /* 鼠标悬停 */
    //     }
    //     QMenu::separator {
    //         height: 1px;
    //         background: #888;
    //         margin: 4px 0;
    //     }
    // )");
}

auto SystemTrayPrivate::connectSignalToSlot() noexcept -> void
{
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayPrivate::onTrayIconActivated);
}

void SystemTrayPrivate::onTrayIconActivated(QSystemTrayIcon::ActivationReason _reason)
{
    if (_reason == QSystemTrayIcon::Trigger)
    {
        // 这里的 m_parentWindow 就是你的主窗口
        if (m_parentWindow->isMinimized() || !m_parentWindow->isVisible())
        {
            m_parentWindow->showNormal();
            m_parentWindow->activateWindow();
        }
        else
        {
            // 如果已经显示，则把它提到最前面
            m_parentWindow->activateWindow();
        }
        m_parentWindow->raise();
    }
    else if (_reason == QSystemTrayIcon::Context)
    {
        // 右键
        // 在当前鼠标位置弹出菜单
        m_trayMenu->popup(QCursor::pos());
    }
}
