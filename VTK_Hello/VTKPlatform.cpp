/**
* @file VTKPlatform.cpp
* @author Kinvy
* @email kinvy66@163.com
* @date: 2025/10/28 9:53
* @description:
**/
#include "VTKPlatform.h"
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <array>
#include <vtkConeSource.h>

VTKPlatform::VTKPlatform(QWidget* parent)
    : QMainWindow(parent) {
    _pVTKWidget = new QVTKOpenGLNativeWidget(this);
    this->setCentralWidget(_pVTKWidget);
    this->resize(800, 600);

    // 1. generate data
    vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();

    // 2. mapper
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    // 3. actor
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

    // 4. renderer
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(.3, .6, .3);

    // 5. connect them
    mapper->SetInputConnection(cone->GetOutputPort());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);

    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}
