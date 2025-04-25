#include <QApplication>

#include "WidgetFrame.h"

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    WidgetFrame  w{};
    w.resize(960, 600);
    w.show();
    QApplication::exec();
}
