#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>

#include "WidgetFrame.h"

class A : public WidgetFrame
{
public:
    explicit(true) A(WidgetFrame* _parent = nullptr) : WidgetFrame{_parent}
    {
        // initBtn();
    }
    ~A() noexcept = default;

    void initBtn()
    {
        QHBoxLayout* lay = new QHBoxLayout{};
        QPushButton* btn = new QPushButton{"CCCCC", this};
        lay->addWidget(btn);
    }
};

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    // 获取编译时间
    // qDebug() << "Compiled on: " << __DATE__ << " at " << __TIME__;
    A w{};
    w.resize(960, 600);
    w.show();
    QApplication::exec();
}
