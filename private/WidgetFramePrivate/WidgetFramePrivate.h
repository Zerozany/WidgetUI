_Pragma("once");
#include <QWidget>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "user32.lib")
#endif

class WidgetFrame;
class WidgetTitleBar;

class WidgetFramePrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(WidgetFrame)
    friend class WidgetTitleBar;

public:
    explicit(true) WidgetFramePrivate(WidgetFrame* _q, QObject* _parent = nullptr);
    ~WidgetFramePrivate() noexcept = default;

private:
    auto initWidgetFrame() noexcept -> void;

    auto initWidgetHandle() noexcept -> void;

    auto conncetSignalsToSlots() noexcept -> void;

public:
    WidgetTitleBar* m_titleBar{nullptr};

private:
    WidgetFrame* q_ptr{nullptr};
};
