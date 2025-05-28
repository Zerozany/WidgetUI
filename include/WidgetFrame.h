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

private:
    Q_DECLARE_PRIVATE(WidgetFrame)
    WidgetFramePrivate* d_ptr{nullptr};
};
