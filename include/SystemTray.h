_Pragma("once");
#include <QObject>

#if defined(_WIN32) && defined(_MSC_VER)
#ifdef UILIB_HANDLE
#define UILIB_API Q_DECL_EXPORT
#else
#define UILIB_API Q_DECL_IMPORT
#endif
#else
#define UILIB_API
#endif

class SystemTrayPrivate;
class QWidget;
class QAction;
class QSystemTrayIcon;

class UILIB_API SystemTray : public QObject
{
    Q_OBJECT
public:
    explicit(true) SystemTray(QWidget* _parentWindow, QObject* _parent = nullptr);
    explicit(true) SystemTray(QWidget* _parentWindow, QIcon& _icon, QObject* _parent = nullptr);
    ~SystemTray() noexcept;

public:
    /// @brief 设置托盘图标
    auto setTrayIcon(const QIcon& _icon) noexcept -> void;

    /// @brief 显示托盘图标
    auto show() noexcept -> void;

    /// @brief 添加托盘选项
    auto addTrayAction(QAction* _action) noexcept -> void;

    /// @brief 设置托盘栏CSS
    auto setMenuStyleSheet(const QString& _styleStr) noexcept -> void;

    /// @brief 获取托盘指针对象实例
    auto getTrayInstance() noexcept -> QSystemTrayIcon* const;

private:
    Q_DECLARE_PRIVATE(SystemTray)
    Q_DISABLE_COPY(SystemTray)
    SystemTrayPrivate* d_ptr{nullptr};
};
