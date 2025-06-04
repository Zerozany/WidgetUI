_Pragma("once");
#include <QWidget>

// #if defined(_WIN32) || defined(__CYGWIN__)
// #ifdef BUILDING_MYLIB
// #ifdef __GNUC__
// #define MYLIB_API __attribute__((dllexport))
// #else
// #define MYLIB_API __declspec(dllexport)
// #endif
// #else
// #ifdef __GNUC__
// #define MYLIB_API __attribute__((dllimport))
// #else
// #define MYLIB_API __declspec(dllimport)
// #endif
// #endif
// #else
// #if __GNUC__ >= 4
// #define MYLIB_API __attribute__((visibility("default")))
// #else
// #define MYLIB_API
// #endif
// #endif

class QVBoxLayout;
class WidgetFramePrivate;

// class MYLIB_API WidgetFrame : public QWidget

class WidgetFrame : public QWidget
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
