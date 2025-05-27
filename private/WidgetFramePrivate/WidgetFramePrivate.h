_Pragma("once");
#include <QPushButton>
#include <QVBoxLayout>
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

public:
    explicit(true) WidgetFramePrivate(WidgetFrame* _q, QObject* _parent = nullptr);
    ~WidgetFramePrivate() noexcept = default;

private:
    auto initWidgetFrame() noexcept -> void;

    auto initWidgetLayout() noexcept -> void;

public:
    WidgetTitleBar* m_titleBar{nullptr};
    QVBoxLayout*    m_mainLayout{nullptr};

private:
    WidgetFrame* q_ptr{nullptr};
};
