#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>

#include "SystemTray.h"
#include "WidgetFrame.h"

class A : public WidgetFrame
{
public:
    explicit(true) A(WidgetFrame* _parent = nullptr) : WidgetFrame{_parent}
    {
#if 0
        this->setTitleFlags(WidgetFrame::TitleBarFlags::CloseHint | WidgetFrame::TitleBarFlags::IconHint);
        initBtn();
        QPushButton* btn  = new QPushButton{"XXXXX", this};
        QPushButton* btn1 = new QPushButton{"WWWww", this};
        this->addTitleAction(btn);
        this->addTitleState(btn1);
#endif
    }
    ~A() noexcept = default;

    void initBtn()
    {
        QHBoxLayout* lay = new QHBoxLayout{};
        QPushButton* btn = new QPushButton{"CCCCC", this};
        lay->addWidget(btn);
        customerLayout()->addLayout(lay);
    }
};

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    // 获取编译时间
    // qDebug() << "Compiled on: " << __DATE__ << " at " << __TIME__;
    A          w{};
    SystemTray tray{&w};
    w.resize(960, 600);
    w.show();
    QApplication::exec();
}
