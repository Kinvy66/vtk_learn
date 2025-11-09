#ifndef VTKEXTRUSION_H
#define VTKEXTRUSION_H

#include <QMainWindow>

namespace Ui {
class VTKExtrusion;
}
class QVTKOpenGLNativeWidget;

class VTKExtrusion : public QMainWindow
{
    Q_OBJECT

public:
    explicit VTKExtrusion(QWidget *parent = nullptr);
    ~VTKExtrusion();

private:
    Ui::VTKExtrusion *ui;
    QVTKOpenGLNativeWidget* _pVTKWidget = NULL;
    
};

#endif // VTKEXTRUSION_H
