#ifndef VTKGRIDDATA_H
#define VTKGRIDDATA_H

#include <QMainWindow>

namespace Ui {
class VTKGridData;
}
class QVTKOpenGLNativeWidget;

class VTKGridData : public QMainWindow
{
    Q_OBJECT

public:
    explicit VTKGridData(QWidget *parent = nullptr);
    ~VTKGridData();

private:
    Ui::VTKGridData *ui;
    QVTKOpenGLNativeWidget* _pVTKWidget = NULL;
    
};

#endif // VTKGRIDDATA_H
