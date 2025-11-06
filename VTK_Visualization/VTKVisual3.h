#ifndef VTKVISUAL3_H
#define VTKVISUAL3_H

#include <QMainWindow>

namespace Ui {
class VTKVisual3;
}
class QVTKOpenGLNativeWidget;

class VTKVisual3 : public QMainWindow
{
    Q_OBJECT

public:
    explicit VTKVisual3(QWidget *parent = nullptr);
    ~VTKVisual3();

private:
    Ui::VTKVisual3 *ui;
    QVTKOpenGLNativeWidget* _pVTKWidget = NULL;
};

#endif // VTKVISUAL3_H
