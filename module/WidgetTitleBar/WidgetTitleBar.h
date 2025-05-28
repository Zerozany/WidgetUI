_Pragma("once");
#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <map>

class WidgetFrame;

class WidgetTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit(true) WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent = nullptr);
    ~WidgetTitleBar() noexcept = default;

public:
    auto getMaximizeBtn() noexcept -> QPushButton*;

    auto getMinimizeBtn() noexcept -> QPushButton*;

    auto getCloseBtn() noexcept -> QPushButton*;

private:
    auto initTitleBarHandle() noexcept -> void;

    auto initTitleBarLayout() noexcept -> void;

    auto connectSignalToSlot() noexcept -> void;

private Q_SLOTS:
    void minimizeChanged() noexcept;

    void maximizeChanged() noexcept;

    void closeChanged() noexcept;

private:
    WidgetFrame* m_widget{nullptr};
    QHBoxLayout* m_titleLayout{new QHBoxLayout{this}};

    std::map<QString, QPushButton*> m_titleBarButtons{
        {"minimize", new QPushButton{this}},
        {"maximize", new QPushButton{this}},
        {"close", new QPushButton{this}},
    };
};
