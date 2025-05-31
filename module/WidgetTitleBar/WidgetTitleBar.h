_Pragma("once");
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <map>

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

    enum struct TitleFlags : std::uint8_t
    {
        IconHint     = 0x01,
        TitleHint    = 0x02,
        MinimizeHint = 0x04,
        MaximizeHint = 0x08,
        CloseHint    = 0x10
    };
    Q_ENUM(TitleFlags)

public:
    friend inline TitleFlags operator|(const TitleFlags& _ltf, const TitleFlags& _rtf)
    {
        return static_cast<TitleFlags>(static_cast<std::uint8_t>(_ltf) | static_cast<std::uint8_t>(_rtf));
    }

    friend inline bool operator&(const TitleFlags& _ltf, const TitleFlags& _rtf)
    {
        return static_cast<std::underlying_type_t<TitleFlags>>(static_cast<std::uint8_t>(_ltf) & static_cast<std::underlying_type_t<TitleFlags>>(_rtf));
    }

public:
    explicit(true) WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent = nullptr);
    ~WidgetTitleBar() noexcept = default;

public:
    auto getMaximizeBtn() const noexcept -> QPushButton*;

    auto getMinimizeBtn() const noexcept -> QPushButton*;

    auto getCloseBtn() const noexcept -> QPushButton*;

    auto getResizeTag() const noexcept -> bool;

private:
    auto initTitleBarHandle() noexcept -> void;

    auto initTitleBarLayout() noexcept -> void;

    auto setCursorType(const QPoint& _pos) noexcept -> void;

    auto connectSignalToSlot() noexcept -> void;

Q_SIGNALS:
    void mousePress(const QMouseEvent* _event);

    void mouseMove(const QMouseEvent* _event);

    void mouseRelease(const QMouseEvent* _event);

    void mouseDouble(const QMouseEvent* _event);

    void cursorType(const CursorType& _cursorTyupe);

    void mouseLeave(const bool _flag);

    void titleFlag(const TitleFlags& _flag);

public Q_SLOTS:
    void onMousePressChanged(const QMouseEvent* _event) noexcept;

    void onMouseMoveChanged(const QMouseEvent* _event) noexcept;

    void onMouseReleaseChanged(const QMouseEvent* _event) noexcept;

    void onMouseDoubleChanged(const QMouseEvent* _event) noexcept;

    void onCursorTypeChanged(const CursorType& _cursorTyupe) noexcept;

    void onMouseLeaveChanged(const bool _flag) noexcept;

    void onTitleFlagChanged(const TitleFlags& _flag) noexcept;

private Q_SLOTS:
    void onMinimizeChanged() noexcept;

    void onMaximizeChanged() noexcept;

    void onCloseChanged() noexcept;

private:
    WidgetFrame* m_widget{nullptr};                    /*无边框窗口指针*/
    QHBoxLayout* m_titleLayout{new QHBoxLayout{this}}; /*标题栏布局容器*/
    QLabel*      m_titleIcon{new QLabel{this}};        /*标题栏图标*/
    QLabel*      m_titleText{new QLabel{this}};        /*标题栏文本标题*/
    CursorType   m_cursorType{CursorType::None};       /*指针图标类型*/
    bool         m_resizing{false};                    /*窗口伸缩句柄*/
    bool         m_resizeTag{false};                   /*窗口伸缩鼠标指针显示句柄*/
    HWND         m_hwnd{};

    std::map<QString, QPushButton*> m_titleBarButtons{
        {"minimize", new QPushButton{this}},
        {"maximize", new QPushButton{this}},
        {"close", new QPushButton{this}},
    };
};
