_Pragma("once");
#include <QWidget>

class WidgetFramePrivate;

class WidgetFrame : public QWidget
{
    Q_OBJECT
public:
    explicit(true) WidgetFrame(QWidget* _parent = nullptr);
    virtual ~WidgetFrame() noexcept;

protected:
    bool nativeEvent(const QByteArray& _eventType, void* _message, qintptr* _result) override;

    void mousePressEvent(QMouseEvent* _event) override;

    void mouseMoveEvent(QMouseEvent* _event) override;

    void mouseReleaseEvent(QMouseEvent* _event) override;

    void mouseDoubleClickEvent(QMouseEvent* _event) override;

    // void paintEvent(QPaintEvent* _event) override;

    // void changeEvent(QEvent* _event) override;

private:
    Q_DECLARE_PRIVATE(WidgetFrame)
    WidgetFramePrivate* d_ptr{nullptr};
};
