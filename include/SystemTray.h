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

class QWidget;
class SystemTrayPrivate;
class QAction;

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

    /// @brief 设置程序气泡弹窗消息
    auto setTrayMessage(const QString& _title, const QString& _msg, int _msecs = 3000) noexcept -> void;

    /// @brief 设置托盘栏CSS
    auto setMenuStyleSheet(const QString& _styleStr) noexcept -> void;

private:
    Q_DECLARE_PRIVATE(SystemTray)
    Q_DISABLE_COPY(SystemTray)
    SystemTrayPrivate* d_ptr{nullptr};
};
