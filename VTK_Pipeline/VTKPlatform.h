#ifndef VTKPLATFORM_H
#define VTKPLATFORM_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class VTKPlatform;
}
QT_END_NAMESPACE


class QVTKOpenGLNativeWidget;

class VTKPlatform : public QMainWindow
{
    Q_OBJECT

public:
    VTKPlatform(QWidget *parent = nullptr);
    ~VTKPlatform();

private:
    Ui::VTKPlatform *ui;
    QVTKOpenGLNativeWidget* _pVTKWidget = NULL;
};
#endif // VTKPLATFORM_H
