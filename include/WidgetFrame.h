_Pragma("once");
#include <QWidget>

#if defined(_WIN32) && defined(_MSC_VER)
#ifdef MYLIB_EXPORTS
#define MYLIB_API Q_DECL_EXPORT
#else
#define MYLIB_API Q_DECL_IMPORT
#endif
#else
#define MYLIB_API
#endif

class QResizeEvent;
class QVBoxLayout;
class WidgetFramePrivate;

class MYLIB_API WidgetFrame : public QWidget
{
    Q_OBJECT
public:
    enum TitleBarFlags : std::uint8_t
    {
        IconHint     = 0x01,
        TitleHint    = 0x02,
        MinimizeHint = 0x04,
        MaximizeHint = 0x08,
        CloseHint    = 0x10
    };
    Q_DECLARE_FLAGS(TitleBarFlag, TitleBarFlags)
    Q_FLAGS(TitleBarFlags)

public:
    explicit(true) WidgetFrame(QWidget* _parent = nullptr);
    virtual ~WidgetFrame() noexcept;

public:
    /// @brief 标题栏控件类型选项
    auto setTitleFlags(const char _flags) noexcept -> void;

    /// @brief 设置最小化按钮图标
    auto setMinimizeIcon(const QIcon& _icon) noexcept -> void;

    /// @brief 设置最大化按钮图标
    auto setMaximizeIcon(const QIcon& _icon) noexcept -> void;

    /// @brief 设置还原按钮图标
    auto setNormalIcon(const QIcon& _icon) noexcept -> void;

    /// @brief 设置关闭窗口按钮图标
    auto setCloseIcon(const QIcon& _icon) noexcept -> void;

    /// @brief 设置窗口图标
    auto setWindowIcon(const QPixmap& _pixmap) noexcept -> void;

    /// @brief 设置窗口标题
    auto setWindowTitle(const QString& _title) noexcept -> void;

    /// @brief 设置窗口伸缩光标样式
    auto setWindowCursor(const QPixmap& _arrow, const QPixmap& _sizeVer, const QPixmap& _sizeHor, const QPixmap& _sizeFDiag, const QPixmap& _sizeBDiag) noexcept -> void;

    /// @brief 获取窗口客户区父布局
    auto customerLayout() noexcept -> QVBoxLayout*;

    /// @brief 添加功能选项控件
    auto addTitleAction(QWidget* _action) noexcept -> void;

    /// @brief 添加窗口状态控件
    auto addTitleState(QWidget* _action) noexcept -> void;

    /// @brief 设置标题栏CSS风格
    auto setTitleBarStyleSheet(const QString& _styleStr) noexcept -> void;

    /// @brief 最小化按钮添加属性
    auto setMinBtnProperty(const char* _proPertyName, const QString& _minProperty) noexcept -> void;

    /// @brief 最大化按钮添加属性
    auto setMaxBtnProperty(const char* _proPertyName, const QString& _maxProperty) noexcept -> void;

    /// @brief 关闭窗口按钮添加属性
    auto setcloseBtnProperty(const char* _proPertyName, const QString& _closeProperty) noexcept -> void;

protected:
    void mousePressEvent(QMouseEvent* _event) override;

    void mouseMoveEvent(QMouseEvent* _event) override;

    void mouseReleaseEvent(QMouseEvent* _event) override;

    void mouseDoubleClickEvent(QMouseEvent* _event) override;

    void resizeEvent(QResizeEvent* _event) override;

    bool nativeEvent(const QByteArray& _eventType, void* _message, qintptr* _result) override;

private:
    Q_DECLARE_PRIVATE(WidgetFrame)
    Q_DISABLE_COPY(WidgetFrame)
    WidgetFramePrivate* d_ptr{nullptr};
};
