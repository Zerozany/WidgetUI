#include <QApplication>

#include "WidgetFrame.h"

class A : public WidgetFrame
{
public:
    explicit(true) A() = default;
    ~A() noexcept      = default;
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
