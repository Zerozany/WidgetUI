_Pragma("once");
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#ifdef Q_OS_WIN
#include "Win32Kit.h"
#endif

class WidgetFrame;
class WidgetTitleBar;

class WidgetFramePrivate : public QObject
{
    Q_OBJECT
public:
    explicit(true) WidgetFramePrivate(WidgetFrame* _q, QObject* _parent = nullptr);
    ~WidgetFramePrivate() noexcept = default;

private:
    auto initWidgetFrame() noexcept -> void;

    auto initWidgetLayout() noexcept -> void;

public:
    WidgetTitleBar* m_titleBar{nullptr};
    QVBoxLayout*    m_customerLayout{nullptr};

private:
    QVBoxLayout* m_mainLayout{nullptr};
    QWidget*     m_customerWidget{nullptr};

private:
    Q_DECLARE_PUBLIC(WidgetFrame)
    WidgetFrame* q_ptr{nullptr};
};
