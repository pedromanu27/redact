#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    MainWindow *mainWin = new MainWindow;
    mainWin->show();

    return a.exec();
}
