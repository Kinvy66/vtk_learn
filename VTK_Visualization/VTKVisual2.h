#ifndef VTKVISUAL2_H
#define VTKVISUAL2_H

#include <QMainWindow>

namespace Ui {
class VTKVisual2;
}

class QVTKOpenGLNativeWidget;

class VTKVisual2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit VTKVisual2(QWidget *parent = nullptr);
    ~VTKVisual2();

private:
    Ui::VTKVisual2 *ui;
     QVTKOpenGLNativeWidget* _pVTKWidget = NULL;
};

#endif // VTKVISUAL2_H
