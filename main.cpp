#include "enterinterface.h"
#include "maininterface.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*EnterInterface enterInterface;
    enterInterface.show();*/
    MainInterface m;
    m.show();
    return a.exec();
}
