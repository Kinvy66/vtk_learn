/******************************************************************************
 * File     : MainWindow.cpp
 * Brief    : vtk数据类型 
 * Author   : Kinvy
 * Email    : kinvy66@163.com
 * Date     : 2025-11-07
 * Version  : V0.0.1
 * Detail   :
 * Copyright: Copyright By Kinvy, All Rights Reserved
******************************************************************************/
#include "MainWindow.h"
#include "./ui_MainWindow.h"
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
#include <array>
#include <vtkFloatArray.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _pVTKWidget = new QVTKOpenGLNativeWidget();
    this->setCentralWidget(_pVTKWidget);
    
    /**
    vtkNew<vtkPoints> points;
    points->InsertPoint(0, 0.0, 0.0, 0.0);
    points->InsertPoint(1, 0.0, 1.0, 0.0);
    points->InsertPoint(2, 1.0, 0.0, 0.0);
    points->InsertPoint(3, 1.0, 1.0, 0.0);
    points->InsertPoint(4, 2.0, 0.0, 0.0);
    points->InsertPoint(5, 2.0, 1.0, 0.0);
    points->InsertPoint(6, 3.0, 0.0, 0.0);
    points->InsertPoint(7, 3.0, 1.0, 0.0);
    
    vtkNew<vtkCellArray> strips;    // 三角形带
    strips->InsertNextCell(8);      // 指定8个顶点
    strips->InsertCellPoint(0);
    strips->InsertCellPoint(1);
    strips->InsertCellPoint(2);
    strips->InsertCellPoint(3);
    strips->InsertCellPoint(4);
    strips->InsertCellPoint(5);
    strips->InsertCellPoint(6);
    strips->InsertCellPoint(7);
    
    vtkNew<vtkPolyData> poly;
    poly->SetPoints(points);
    poly->SetStrips(strips);  // 三角带
    // poly->SetLines(strips);   // 折线
    poly->SetVerts(strips);  // 点
    
    **/
    
    // 立方体
    std::array<std::array<double,3>,8> pts ={ // 8顶点
        {{{0, 0, 0}},
         {{1, 0, 0}},
         {{1, 1, 0}},
         {{0, 1, 0}},
         {{0, 0, 1}},
         {{1, 0, 1}},
         {{1, 1, 1}},
         {{0, 1, 1}}}
    };
    
    // 定义每个面用到的顶点
    std::array<std::array<vtkIdType, 4>,6> ordering = {
        {{{0, 1, 2, 3}},
         {{4, 5, 6, 7}},
         {{0, 1, 5, 4}},
         {{1, 2, 6, 5}},
         {{2, 3, 7, 6}},
         {{3, 0, 4, 7}}}
    };
    
    // 定义立方体
    vtkNew<vtkPolyData> cube;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> polys;
    vtkNew<vtkFloatArray> scalars;      // 定义属性
    
    
    // 把定义的点数据添加到vtk points
    for (size_t i = 0; i < pts.size(); ++i) {
        points->InsertPoint(i, pts[i].data());
        scalars->InsertTuple1(i, i);
        
    }
    
    for (auto&& i : ordering) {
        polys->InsertNextCell(vtkIdType(i.size()), i.data());
    }
    
    cube->SetPoints(points);
    cube->SetPolys(polys);
    cube->GetPointData()->SetScalars(scalars);
    
    
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;
    mapper->SetInputData(cube);
    mapper->SetScalarRange(cube->GetScalarRange());
    actor->SetMapper(mapper);
    
    // renderer 
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(230.0 / 255.0, 242.0 / 255.0, 255.0 / 255.0);
    renderer->SetBackground2(250.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0);
    renderer->GradientBackgroundOn();
    
    
    
    renderer->AddActor(actor);
    
    
    this->_pVTKWidget->renderWindow()->SetWindowName("VTK Data");    
    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}

MainWindow::~MainWindow()
{
    delete ui;
}
