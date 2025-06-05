_Pragma("once");
#include <QWidget>

#ifdef DLL_BUILD
#define DLL_EXPORT Q_DECL_EXPORT
#else
#define DLL_EXPORT Q_DECL_IMPORT
#endif

class QVBoxLayout;
class WidgetFramePrivate;

class DLL_EXPORT WidgetFrame : public QWidget
{
    Q_OBJECT
public:
    explicit(true) WidgetFrame(QWidget* _parent = nullptr);
    virtual ~WidgetFrame() noexcept;

public:
    auto setMinimizeIcon(const QIcon& _icon) noexcept -> void;

    auto setMaximizeIcon(const QIcon& _icon) noexcept -> void;

    auto setNormalIcon(const QIcon& _icon) noexcept -> void;

    auto setCloseIcon(const QIcon& _icon) noexcept -> void;

    auto setWindowIcon(const QPixmap& _pixmap) noexcept -> void;

    auto setWindowTitle(const QString& _title) noexcept -> void;

    auto customerLayout() noexcept -> QVBoxLayout*;

protected:
    bool nativeEvent(const QByteArray& _eventType, void* _message, qintptr* _result) override;

    void mousePressEvent(QMouseEvent* _event) override;

    void mouseMoveEvent(QMouseEvent* _event) override;

    void mouseReleaseEvent(QMouseEvent* _event) override;

    void mouseDoubleClickEvent(QMouseEvent* _event) override;

    // void changeEvent(QEvent* _event) override;

private:
    Q_DECLARE_PRIVATE(WidgetFrame)
    WidgetFramePrivate* d_ptr{nullptr};
};
