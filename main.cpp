#include <QApplication>
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
        QPushButton* btn = new QPushButton{"CCCCC", this};
        btn->move(0, 50);
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
