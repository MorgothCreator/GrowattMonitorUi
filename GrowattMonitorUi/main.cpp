#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont f = a.font();
    f.setFamily("Monaco");
#ifdef Q_OS_OSX
    f.setPointSize(9);
#elif defined (Q_OS_WIN32)
#else
    f.setPointSize(9);
#endif
    a.setFont(f);
    MainWindow w;
    w.show();
    return a.exec();
}
