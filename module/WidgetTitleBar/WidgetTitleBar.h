_Pragma("once");
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <map>

#include "ConfigLoader.h"

class WidgetFrame;

class WidgetTitleBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QIcon minimizeIcon READ getMinimizeIcon WRITE setMinimizeIcon NOTIFY minimizeIconChanged);
    Q_PROPERTY(QIcon maximizeIcon READ getMaximizeIcon WRITE setMaximizeIcon NOTIFY maximizeIconChanged);
    Q_PROPERTY(QIcon normalIcon READ getNormalIcon WRITE setNormalIcon NOTIFY normalIconChanged);
    Q_PROPERTY(QIcon closeIcon READ getCloseIcon WRITE setCloseIcon NOTIFY closeIconChanged);
    Q_PROPERTY(QPixmap windowIcon READ getWindowIcon WRITE setWindowIcon NOTIFY windowIconChanged);
    Q_PROPERTY(QString windowTitle READ getWindowTitle WRITE setWindowTitle NOTIFY windowTitleChanged);
    Q_PROPERTY(QPushButton* maximizeBtn READ getMaximizeBtn);
    Q_PROPERTY(QPushButton* minimizeBtn READ getMinimizeBtn);
    Q_PROPERTY(QPushButton* closeBtn READ getCloseBtn);

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
        CloseHint    = 0x16
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
    /// @brief Q_PROPERTY
    auto getMinimizeIcon() const noexcept -> QIcon;
    auto setMinimizeIcon(const QIcon& _icon) noexcept -> void;

    auto getMaximizeIcon() const noexcept -> QIcon;
    auto setMaximizeIcon(const QIcon& _icon) noexcept -> void;

    auto getNormalIcon() const noexcept -> QIcon;
    auto setNormalIcon(const QIcon& _icon) noexcept -> void;

    auto getCloseIcon() const noexcept -> QIcon;
    auto setCloseIcon(const QIcon& _icon) noexcept -> void;

    auto getWindowIcon() const noexcept -> QPixmap;
    auto setWindowIcon(const QPixmap& _pixmap) noexcept -> void;

    auto getWindowTitle() const noexcept -> QString;
    auto setWindowTitle(const QString& _title) noexcept -> void;

public:
    /// @brief 主框架nativeEvent调用函数
    auto getMaximizeBtn() const noexcept -> QPushButton*;

    auto getMinimizeBtn() const noexcept -> QPushButton*;

    auto getCloseBtn() const noexcept -> QPushButton*;

    auto getResizeTag() const noexcept -> bool;

    auto setWindowCursor(const QPixmap& _arrow, const QPixmap& _sizeVer, const QPixmap& _sizeHor, const QPixmap& _sizeFDiag, const QPixmap& _sizeBDiag) const noexcept -> void;

private:
    auto initTitleBarConfig() noexcept -> void;

    auto initTitleBarHandle() noexcept -> void;

    auto initTitleBarLayout() noexcept -> void;

    auto setCursorType(const QPoint& _pos) noexcept -> void;

    auto connectSignalToSlot() noexcept -> void;

Q_SIGNALS:
    /// @brief Q_PROPERTY 按钮图标更改信号
    void minimizeIconChanged();

    void maximizeIconChanged();

    void normalIconChanged();

    void closeIconChanged();

    void windowIconChanged();

    void windowTitleChanged();

Q_SIGNALS:
    void mousePress(const QMouseEvent* _event);

    void mouseMove(const QMouseEvent* _event);

    void mouseRelease(const QMouseEvent* _event);

    void mouseDouble(const QMouseEvent* _event);

    void mouseLeave(const bool _flag);

    void titleFlagChanged(const TitleFlags& _flag);

    void cursorTypeChanged();

private Q_SLOTS:
    void onMousePressChanged(const QMouseEvent* _event) noexcept;

    void onMouseMoveChanged(const QMouseEvent* _event) noexcept;

    void onMouseReleaseChanged(const QMouseEvent* _event) noexcept;

    void onMouseDoubleChanged(const QMouseEvent* _event) noexcept;

    void onMouseLeaveChanged(const bool _flag) noexcept;

    void onTitleFlagChanged(const TitleFlags& _flag) noexcept;

    void onCursorTypeChanged() noexcept;

private Q_SLOTS:
    void onMinimizeClicked() noexcept;

    void onMaximizeClicked() noexcept;

    void onCloseClicked() noexcept;

private:
    WidgetFrame*  m_widget{nullptr};               /*无边框窗口指针*/
    ConfigLoader* m_configLoader{nullptr};         /*配置文件读取对象*/
    QLabel*       m_windowIcon{new QLabel{this}};  /*标题栏图标*/
    QLabel*       m_windowTitle{new QLabel{this}}; /*标题栏文本标题*/
    CursorType    m_cursorType{CursorType::None};  /*指针图标类型*/
    bool          m_resizeTag{false};              /*窗口伸缩鼠标指针显示句柄*/
    HWND          m_hwnd{};                        /*窗口界面句柄*/

private:
    std::map<QString, QHBoxLayout*> m_titleBarLayouts{
        {"titleBarLayout", new QHBoxLayout{this}},
        {"titleBtnLayout", new QHBoxLayout{}},
        {"titleActionLayout", new QHBoxLayout{}},
    };

    std::map<QString, QPushButton*> m_titleBarButtons{
        {"minimize", new QPushButton{this}},
        {"maximize", new QPushButton{this}},
        {"close", new QPushButton{this}},
    };

    QMap<QString, QIcon> m_titleBarIcons{
        {"minimizeIcon", QIcon{":/resources/icon/minimize.png"}},
        {"maximizeIcon", QIcon{":/resources/icon/maximize.png"}},
        {"normalIcon", QIcon{":/resources/icon/normal.png"}},
        {"closeIcon", QIcon{":/resources/icon/close.png"}},
    };

    QMap<QString, QCursor> m_cursorCursors{
        {"arrow", Qt::ArrowCursor},
        {"sizeVer", Qt::SizeVerCursor},
        {"sizeHor", Qt::SizeHorCursor},
        {"sizeFDiag", Qt::SizeFDiagCursor},
        {"sizeBDiag", Qt::SizeBDiagCursor},
    };
};
