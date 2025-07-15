_Pragma("once");
#include <QObject>
#include <QSystemTrayIcon>

class SystemTray;
class QWidget;
class QMenu;
class QAction;

class SystemTrayPrivate : public QObject
{
    Q_OBJECT
public:
    explicit(true) SystemTrayPrivate(QWidget* _parentWindow, QObject* _parent = nullptr);
    ~SystemTrayPrivate() noexcept = default;

public:
    auto setTrayIcon(const QIcon& _icon) noexcept -> void;

    auto show() noexcept -> void;

    auto addTrayAction(QAction* _action) noexcept -> void;

    auto setMenuStyleSheet(const QString& _styleStr) noexcept -> void;

    auto getTrayInstance() noexcept -> QSystemTrayIcon* const;

private:
    auto initSystemTray() noexcept -> void;

    auto connectSignalToSlot() noexcept -> void;

private Q_SLOTS:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason _reason);

private:
    QWidget*         m_parentWindow{nullptr};
    QSystemTrayIcon* m_trayIcon{nullptr};
    QMenu*           m_trayMenu{nullptr};

private:
    Q_DECLARE_PUBLIC(SystemTray)
    SystemTray* q_ptr{nullptr};
};
