#include "ashurawind.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AshuraWind w;
    w.setAttribute(Qt::WA_TranslucentBackground);
    w.setAttribute(Qt::WA_ShowWithoutActivating);
    w.setWindowFlags( Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint );
    w.show();
    w.activateWindow();

    return a.exec();
}
