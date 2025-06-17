_Pragma("once");
#include <QAction>
#include <QApplication>
#include <QObject>

class QMenu;
class QSystemTrayIcon;
class SystemTray;
class QWidget;

class SystemTrayPrivate : public QObject
{
    Q_OBJECT
public:
    explicit(true) SystemTrayPrivate(QWidget* _parentWindow, QObject* _parent = nullptr);
    ~SystemTrayPrivate() noexcept = default;

public:
    auto setTrayIcon(const QIcon& _icon = QApplication::windowIcon()) noexcept -> void;

private:
    auto initSystemTray() noexcept -> void;

private:
    QWidget*         m_parentWindow{nullptr};
    QSystemTrayIcon* m_trayIcon{nullptr};
    QMenu*           m_trayMenu{nullptr};

    QMap<QString, QAction*> m_trayActions{
        {"restore", new QAction{}},
        {"quit", new QAction{}},
    };

private:
    Q_DECLARE_PUBLIC(SystemTray)
    SystemTray* q_ptr{nullptr};
};
