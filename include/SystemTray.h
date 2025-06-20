_Pragma("once");

#include <QObject>

#ifdef DLL_BUILD
#define DLL_BUILDTYPE Q_DECL_EXPORT
#else
#define DLL_BUILDTYPE Q_DECL_IMPORT
#endif

class QWidget;
class SystemTrayPrivate;

class DLL_BUILDTYPE SystemTray : public QObject
{
    Q_OBJECT
public:
    explicit(true) SystemTray(QWidget* _parentWindow, QObject* _parent = nullptr);
    ~SystemTray() noexcept;

public:
    auto setTrayIcon(const QIcon& _icon) noexcept -> void;

    auto show() noexcept -> void;

private:
    Q_DECLARE_PRIVATE(SystemTray)
    Q_DISABLE_COPY(SystemTray)
    SystemTrayPrivate* d_ptr{nullptr};
};
