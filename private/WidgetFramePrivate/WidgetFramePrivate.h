_Pragma("once");
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Win32Kit.hpp"

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
    QVBoxLayout*    m_customerLayout{nullptr};

private:
    QWidget* m_customerWidget{nullptr};

private:
    WidgetFrame* q_ptr{nullptr};
};
