/******************************************************************************
 * File     : VTKPlatform.cpp
 * Brief    : vtk 可视化方法
 * Author   : Kinvy
 * Email    : kinvy66@163.com
 * Date     : 2025-11-04
 * Version  : V0.0.1
 * Detail   : 1. 颜色映射
 *            2. 等值面
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

VTKPlatform::VTKPlatform(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VTKPlatform)
{
    ui->setupUi(this);
    
    _pVTKWidget = new QVTKOpenGLNativeWidget();
    this->setCentralWidget(_pVTKWidget);
    
    // 1.read data   
    vtkSmartPointer<vtkMultiBlockPLOT3DReader> pl3d = vtkSmartPointer<vtkMultiBlockPLOT3DReader>::New();
    pl3d->SetXYZFileName("../../../Data/combxyz.bin");
    pl3d->SetQFileName("../../../Data/combq.bin");
    pl3d->SetScalarFunctionNumber(100);
    pl3d->SetVectorFunctionNumber(202);
    
    // qDebug() <<"update before: "  <<  pl3d->GetOutput()->GetNumberOfBlocks();
    pl3d->Update();
    // qDebug() << "update after: " << pl3d->GetOutput()->GetNumberOfBlocks();

    vtkDataSet* pl3d_ouput = (vtkDataSet*)(pl3d->GetOutput()->GetBlock(0));
    
    vtkDataArray* scalars = pl3d_ouput->GetPointData()->GetScalars();
    double* range = scalars->GetRange();
    qDebug() << range[0] << ":" << range[1];
    
    // 等值面
    // vtkSmartPointer<vtkContourFilter> contours = vtkSmartPointer<vtkContourFilter>::New();
    // contours->SetInputData(pl3d_ouput);
    // contours->GenerateValues(5, range[0], range[1]);  // 在range[0]到range[1]之间均匀提取5个值
    
    // 使用函数获取等值面
    vtkSmartPointer<vtkQuadric> quadric = vtkSmartPointer<vtkQuadric>::New();
    /*
    * vtkQuadric evaluates the quadric function F(x,y,z) = a0*x^2 + a1*y^2 +
    * a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x + a7*y + a8*z + a9. vtkQuadric is
    * a concrete implementation of vtkImplicitFunction.
    */
    quadric->SetCoefficients(.5, 1, .2, 0, .1, 0, 0, .2, 0, 0);
    
    // 数据采样
    vtkSmartPointer<vtkSampleFunction> sample =  vtkSmartPointer<vtkSampleFunction>::New();
    sample->SetSampleDimensions(30, 30, 30);  // 每个维度的采样数量
    sample->SetImplicitFunction(quadric);     // 设置采样函数
    
    vtkSmartPointer<vtkContourFilter> contours = vtkSmartPointer<vtkContourFilter>::New();
    contours->SetInputConnection(sample->GetOutputPort());
    contours->GenerateValues(5, 0.0, 1.2);  // 在range[0]到range[1]之间均匀提取5个值
    
    vtkSmartPointer<vtkPolyDataMapper> contourMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    contourMapper->SetInputConnection(contours->GetOutputPort());
    
    vtkSmartPointer<vtkActor> contourActor = vtkSmartPointer<vtkActor>::New();
    contourActor->SetMapper(contourMapper);
    
     
    // get a plane 平面颜色映射
    vtkSmartPointer<vtkStructuredGridGeometryFilter> plane = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
    plane->SetInputData(pl3d_ouput);
    plane->SetExtent(1, 100, 1, 100, 7, 7); // 在3维空间提取一个面，第一个维度1-100， 第二个维度1-100， 第三个维度7-7(一个平面)
    
    // lookup table
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(256);
    lut->Build();
    
    // plane Mapper
    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetLookupTable(lut);
    planeMapper->SetInputConnection(plane->GetOutputPort());
    planeMapper->SetScalarRange(pl3d_ouput->GetScalarRange());

    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor>::New();
    planeActor->SetMapper(planeMapper);
    
    
    // 添加边框
    vtkSmartPointer<vtkStructuredGridOutlineFilter> outline = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
    outline->SetInputData(pl3d_ouput);
    vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(outline->GetOutputPort());
    
    vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色


    // renderer 
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    // 上方颜色 (230, 242, 255)
    renderer->SetBackground(230.0 / 255.0, 242.0 / 255.0, 255.0 / 255.0);
    // 下方颜色 (250, 251, 255)
    renderer->SetBackground2(250.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0);
    // 启用渐变背景
    renderer->GradientBackgroundOn();
    

    // renderer->AddActor(planeActor);
    // renderer->AddActor(outlineActor);    
    renderer->AddActor(contourActor);   
    
    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}

VTKPlatform::~VTKPlatform()
{
    delete ui;
}
