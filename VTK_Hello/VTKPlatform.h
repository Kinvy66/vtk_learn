/**
* @file VTKPlatform.h
* @author Kinvy
* @email kinvy66@163.com
* @date: 2025/10/28 9:53
* @description:
**/
#include <QtWidgets/QMainWindow>
#include <QVTKOpenGLNativeWidget.h>

class VTKPlatform: public QMainWindow
{
    Q_OBJECT
public:
    VTKPlatform(QWidget* parent = Q_NULLPTR);

private:
    QVTKOpenGLNativeWidget* _pVTKWidget = NULL;
};