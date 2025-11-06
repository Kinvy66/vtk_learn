/******************************************************************************
 * File     : VTKPlatform.h
 * Brief    : 建模方法
 * Author   : Kinvy
 * Email    : kinvy66@163.com
 * Date     : 2025-11-05
 * Version  : V0.0.1
 * Detail   :
 * Copyright: Copyright By Kinvy, All Rights Reserved
******************************************************************************/
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
