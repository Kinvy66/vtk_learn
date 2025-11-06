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
    
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;
    mapper->SetInputData(profile);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色
    
    
    
    // renderer 
    // vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(230.0 / 255.0, 242.0 / 255.0, 255.0 / 255.0);
    renderer->SetBackground2(250.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0);
    renderer->GradientBackgroundOn();
    
    renderer->AddActor(actor);
    
    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}

VTKPlatform::~VTKPlatform()
{
    delete ui;
}
