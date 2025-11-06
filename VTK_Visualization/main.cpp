#include "VTKPlatform.h"
#include "VTKVisual2.h"
#include "VTKVisual3.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VTKVisual3 w;
    w.show();
    return a.exec();
}
