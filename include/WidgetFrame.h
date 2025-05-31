_Pragma("once");
#include <QWidget>

#define ICONHINT 0x01
#define TITLEHINT 0x02
#define MINIMIZEHINT 0x04
#define MAXIMIZEHINT 0x08
#define CLOSEHINT 0x16

class WidgetFramePrivate;

class WidgetFrame : public QWidget
{
    Q_OBJECT
public:
    explicit(true) WidgetFrame(QWidget* _parent = nullptr);
    virtual ~WidgetFrame() noexcept;

public:
    auto setMinimizeIcon(const QString& _iconPath) noexcept -> void;

    auto setMaximizeIcon(const QString& _iconMaximizePath, const QString& _iconNormalPath) noexcept -> void;

    auto setCloseIcon(const QString& _iconPath) noexcept -> void;

    auto setWindowIcon(const QString& _iconPath) noexcept -> void;

    auto setWindowTitle(const QString& _title) noexcept -> void;

protected:
    bool nativeEvent(const QByteArray& _eventType, void* _message, qintptr* _result) override;

    void mousePressEvent(QMouseEvent* _event) override;

    void mouseMoveEvent(QMouseEvent* _event) override;

    void mouseReleaseEvent(QMouseEvent* _event) override;

    void mouseDoubleClickEvent(QMouseEvent* _event) override;

    // void resizeEvent(QResizeEvent* _event) override;

    // void paintEvent(QPaintEvent* _event) override;

    // void changeEvent(QEvent* _event) override;

private:
    Q_DECLARE_PRIVATE(WidgetFrame)
    WidgetFramePrivate* d_ptr{nullptr};
};
