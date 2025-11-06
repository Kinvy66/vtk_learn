#include "VTKPlatform.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VTKPlatform w;
    w.show();
    return a.exec();
}
