#include <QAction>
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
#if false

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

    void test1()
    {
        this->setTitleBarBackColor("#555FFF");
        this->setTitleFlags(TitleBarFlags::CloseHint | TitleBarFlags::IconHint);
        initBtn();
        QPushButton* btn  = new QPushButton{"XXXXX", this};
        QPushButton* btn1 = new QPushButton{"WWWww", this};
        this->addTitleAction(btn);
        this->addTitleState(btn1);
    }

    void tray()
    {
        SystemTray* tray = new SystemTray{this};
        tray->setTrayIcon(QIcon{":/resources/icon/TitleBarIcons/close.png"});
        tray->addTrayAction(new QAction{"quit"});
        tray->addTrayAction(new QAction{QIcon{":/resources/icon/TitleBarIcons/close.png"}, "setting"});
        tray->show();
        tray->setTrayMessage(QApplication::applicationName(), "正在运行中", 3000);
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
