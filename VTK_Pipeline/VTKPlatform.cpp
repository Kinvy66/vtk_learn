#include "VTKPlatform.h"
#include "./ui_VTKPlatform.h"
#include <QVTKOpenGLNativeWidget.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkShrinkPolyData.h>
#include <vtkCamera.h>
#include <vtkLight.h>


VTKPlatform::VTKPlatform(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VTKPlatform)
{
    ui->setupUi(this);
    _pVTKWidget = new QVTKOpenGLNativeWidget();
    this->setCentralWidget(_pVTKWidget);
    
    // 1.gennerate data
    vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
    cone->SetResolution(360);
    
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName("../../../Data/42400-IDGH.stl");
    
    // 2. mapper
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    
    // 3.actor
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    
    // 4. renderer
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    // 上方颜色 (230, 242, 255)
    renderer->SetBackground(230.0 / 255.0, 242.0 / 255.0, 255.0 / 255.0);
    // 下方颜色 (250, 251, 255)
    renderer->SetBackground2(250.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0);
    // 启用渐变背景
    renderer->GradientBackgroundOn();
    
    vtkSmartPointer<vtkShrinkPolyData> shrink = vtkSmartPointer<vtkShrinkPolyData>::New();
    
    
    // 5. connect them
    // shrink->SetInputConnection(cone->GetOutputPort());
    mapper->SetInputConnection(cone->GetOutputPort());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
    
    // camer operator
    vtkCamera* camera = renderer->GetActiveCamera();
    camera->SetFocalPoint(0, 0, 0);   // 设置相机焦点
    camera->SetPosition(0, 0, 5);     // 设置相机位置  
    camera->SetViewUp(1, 0, 0);       // 向上方向
    
    // camera->Zoom(0.5);  // 缩小
    // camera->Elevation(60);  // 竖直方向旋转
    camera->SetPosition(5, 0, 0);
    camera->SetViewUp(0, 1, 0);
    // camera->Azimuth(60);  // 水平方向旋转

    // renderer->ResetCamera();
    
    
    // light operator
    vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
    light->SetPosition(camera->GetFocalPoint());
    light->SetPosition(0, 1, 0);
    light->SetColor(1, 0, 0);
    renderer->AddLight(light);
        
    
    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
    
}

VTKPlatform::~VTKPlatform()
{
    delete ui;
}
