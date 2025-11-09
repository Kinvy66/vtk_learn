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
#include "VTKPlatform.h"
#include "./ui_VTKPlatform.h"
#include <QDebug>
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
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkDataSet.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkContourFilter.h>
#include <vtkQuadric.h>
#include <vtkSampleFunction.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataNormals.h>
#include <vtkMaskPoints.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkGlyph3D.h>
#include <vtkRungeKutta4.h>
#include <vtkStreamTracer.h>
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkPlaneSource.h>
#include <vtkOutlineFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkProbeFilter.h>
#include <vtkImplicitModeller.h>
#include <vtkRegularPolygonSource.h>
#include <vtkWarpTo.h>

VTKPlatform::VTKPlatform(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VTKPlatform)
{
    ui->setupUi(this);
    _pVTKWidget = new QVTKOpenGLNativeWidget();
    this->setCentralWidget(_pVTKWidget);
    
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> lines;
    vtkNew<vtkPolyData> profile;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(0, 2, 0);
    points->InsertNextPoint(2, 4, 0);
    points->InsertNextPoint(4, 4, 0);
    lines->InsertNextCell(4);
    lines->InsertCellPoint(0);
    lines->InsertCellPoint(1);
    lines->InsertCellPoint(2);
    lines->InsertCellPoint(3);

    profile->SetPoints(points);
    profile->SetLines(lines);
    
    
    vtkNew<vtkTubeFilter> tube;
    tube->SetInputData(profile);
    tube->SetRadius(.5);
    tube->SetNumberOfSides(12);
    
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;
    mapper->SetInputData(profile);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色
    
    vtkNew<vtkPolyDataMapper> tubeMapper;
    vtkNew<vtkActor> tubeActor;
    tubeMapper->SetInputConnection(tube->GetOutputPort());
    tubeActor->SetMapper(tubeMapper);
    
    // implicit modeller
    vtkNew<vtkImplicitModeller> imp;
    imp->SetInputData(profile);
    imp->SetSampleDimensions(100, 100, 100);
    imp->SetMaximumDistance(0.25);
    imp->SetModelBounds(-10, 10, -10, 10, -10, 10);
    vtkNew<vtkContourFilter> contour;
    contour->SetInputConnection(imp->GetOutputPort());
    contour->SetValue(0, .5);
    
    vtkNew<vtkPolyDataMapper> impMapper;
    vtkNew<vtkActor> impActor;
    impMapper->SetInputConnection(contour->GetOutputPort());
    impActor->SetMapper(impMapper);
    impActor->SetPosition(5, 0, 0);
    
    // circle
    vtkNew<vtkRegularPolygonSource> circle;
    circle->GeneratePolygonOn();
    circle->SetNumberOfSides(50);
    circle->SetRadius(2);
    circle->SetCenter(0, 0, 0);

    vtkNew<vtkImplicitModeller> circleImp;
    circleImp->SetInputConnection(circle->GetOutputPort());
    circleImp->SetSampleDimensions(100, 100, 8);
    circleImp->SetModelBounds(-3, 3, -3, 3, -.2, .2);
    vtkNew<vtkContourFilter> circleCF;
    circleCF->SetInputConnection(circleImp->GetOutputPort());
    circleCF->SetValue(0, .2);
    
    vtkNew<vtkWarpTo> circleWT;
    circleWT->SetInputConnection(circleCF->GetOutputPort());
    circleWT->SetPosition(0, 0, 5);
    circleWT->SetScaleFactor(0.85);
    circleWT->AbsoluteOn();
    
    vtkNew<vtkPolyDataMapper> circleMapper;
    vtkNew<vtkActor> circleActor;
    circleMapper->SetInputConnection(circleWT->GetOutputPort());
    circleActor->SetMapper(circleMapper);
    circleActor->SetPosition(12, 0, 0);
    
    
    // renderer 
    // vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(230.0 / 255.0, 242.0 / 255.0, 255.0 / 255.0);
    renderer->SetBackground2(250.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0);
    renderer->GradientBackgroundOn();
    
    renderer->AddActor(actor);
    renderer->AddActor(tubeActor);
    renderer->AddActor(impActor);
    renderer->AddActor(circleActor);
    
    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}

VTKPlatform::~VTKPlatform()
{
    delete ui;
}
