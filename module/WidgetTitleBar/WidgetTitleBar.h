_Pragma("once");
#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

class QMouseEvent;
class WidgetFrame;

class WidgetTitleBar : public QWidget
{
    Q_OBJECT
public:
    enum struct CursorType : std::uint8_t
    {
        None,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };
    Q_ENUM(CursorType)
public:
    explicit(true) WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent = nullptr);
    ~WidgetTitleBar() noexcept;

private:
    auto initWidgetTitleBar() noexcept -> void;

    auto setCursorType(const QPoint& _pos) noexcept -> void;

    auto conncetSignalToSlot() noexcept -> void;

Q_SIGNALS:
    auto cursorTypeChanged() -> void;

    auto titleStatusChanged(const bool _flag) -> void;

private Q_SLOTS:
    auto setCursorEvent() noexcept -> void;

    auto setTitleStatus(const bool _flag) noexcept -> void;

public Q_SLOTS:
    void mousePressSlot(QMouseEvent* _event);

    void mouseMoveSlot(QMouseEvent* _event);

    void mouseReleaseSlot(QMouseEvent* _event);

    void mouseDoubleClickSlot(QMouseEvent* _event);

    void mouseLeaveSlot(QMouseEvent* _event);

private:
    WidgetFrame* m_widget{nullptr};
    QHBoxLayout* m_titleLayout{new QHBoxLayout{this}};
    QPushButton* m_minimizeButton{new QPushButton{this}};
    QPushButton* m_maximizeButton{new QPushButton{this}};
    QPushButton* m_closeButton{new QPushButton{this}};

private:
    bool       m_resizing{false};                          /*窗口伸缩句柄*/
    CursorType m_cursorType{};                             /*指针图标类型*/
    bool       m_mouseHandle{};                            /*鼠标移动窗口句柄*/
    QScreen*   m_screen{QGuiApplication::primaryScreen()}; /*获取主屏幕*/
};
