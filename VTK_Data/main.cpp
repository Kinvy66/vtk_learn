#include "MainWindow.h"
#include "VTKGridData.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VTKGridData w;
    w.show();
    return a.exec();
}
