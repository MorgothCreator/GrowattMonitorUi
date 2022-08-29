#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    if(argc > 1)
        qDebug() << "argv[1]" << argv[1];
    if(argc > 2)
        qDebug() << "argv[2]" << argv[2];
    QApplication a(argc, argv);
    QFont f = a.font();
    f.setFamily("Monaco");
#ifdef Q_OS_OSX
    f.setPointSize(9);
#elif defined (Q_OS_WIN32)
#else
    //f.setPointSize(9);
#endif
    a.setFont(f);
    MainWindow w;
    w.show();
    return a.exec();
}
