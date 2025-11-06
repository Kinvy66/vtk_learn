/******************************************************************************
 * File     : VTKPlatform.cpp
 * Brief    : vtk 可视化方法
 * Author   : Kinvy
 * Email    : kinvy66@163.com
 * Date     : 2025-11-04
 * Version  : V0.0.1
 * Detail   : 5. Cut probing
 * Copyright: Copyright By Kinvy, All Rights Reserved
******************************************************************************/
#include "VTKVisual3.h"
#include "ui_VTKVisual3.h"
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

VTKVisual3::VTKVisual3(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VTKVisual3)
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
    pl3d->Update();
    vtkDataSet* pl3d_ouput = (vtkDataSet*)(pl3d->GetOutput()->GetBlock(0));
    
    // 添加边框
    vtkSmartPointer<vtkStructuredGridOutlineFilter> outline = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
    outline->SetInputData(pl3d_ouput);
    vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(outline->GetOutputPort());
    
    vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色
        
    // get a plane 平面颜色映射， 沿网格截取平面
    vtkSmartPointer<vtkStructuredGridGeometryFilter> plane = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
    plane->SetInputData(pl3d_ouput);
    plane->SetExtent(1, 100, 1, 100, 7, 7); // 在3维空间提取一个面，第一个维度1-100， 第二个维度1-100， 第三个维度7-7(一个平面)
    // plane Mapper
    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputConnection(plane->GetOutputPort());
    planeMapper->SetScalarRange(pl3d_ouput->GetScalarRange());
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor>::New();
    planeActor->SetMapper(planeMapper);
    
    // cutting  使用隐函数截取
    vtkSmartPointer<vtkPlane> cuttingPlane = vtkSmartPointer<vtkPlane>::New();
    cuttingPlane->SetOrigin(pl3d_ouput->GetCenter());   // 设置平面中心为数据中心
    cuttingPlane->SetNormal(1, 0, 0.3); // 设置法向
    
    vtkSmartPointer<vtkCutter> planeCut = vtkSmartPointer<vtkCutter>::New();
    planeCut->SetInputData(pl3d_ouput);
    planeCut->SetCutFunction(cuttingPlane);
    
    double range[2] = {-5, 5};
    planeCut->GenerateValues(3, range);  // 在给定范围生成3个值， 3个平面
    
    vtkSmartPointer<vtkPolyDataMapper> cuttingMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    cuttingMapper->SetInputConnection(planeCut->GetOutputPort());
    cuttingMapper->SetScalarRange(pl3d_ouput->GetScalarRange());  // 设置平面颜色范围
        
    vtkSmartPointer<vtkActor> cuttingActor = vtkSmartPointer<vtkActor>::New();
    cuttingActor->SetMapper(cuttingMapper);
    
    // probing 使用数据截取平面
    vtkSmartPointer<vtkPlaneSource> planeData = vtkSmartPointer<vtkPlaneSource>::New();
    planeData->SetResolution(50, 50);  // x y 维度的分辨率
    
    vtkSmartPointer<vtkTransform> transP1 = vtkSmartPointer<vtkTransform>::New();
    transP1->Translate(3.7, .0, 28.37);  // 空间变换
    transP1->Scale(5, 5, 5);
    transP1->RotateY(90);
    vtkSmartPointer<vtkTransformPolyDataFilter> tpd1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    tpd1->SetInputConnection(planeData->GetOutputPort());
    tpd1->SetTransform(transP1);
    vtkSmartPointer<vtkOutlineFilter> outTpd1 = vtkSmartPointer<vtkOutlineFilter>::New();
    outTpd1->SetInputConnection(tpd1->GetOutputPort());
    vtkSmartPointer<vtkPolyDataMapper> tpdMapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    tpdMapper1->SetInputConnection(outTpd1->GetOutputPort());
    vtkSmartPointer<vtkActor> tpdActor1 = vtkSmartPointer<vtkActor>::New();
    tpdActor1->SetMapper(tpdMapper1);
    tpdActor1->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色
    
    vtkSmartPointer<vtkTransform> transP2 = vtkSmartPointer<vtkTransform>::New();
    transP2->Translate(9.2, .0, 31.20);  // 空间变换
    transP2->Scale(5, 5, 5);
    transP2->RotateY(90);
    vtkSmartPointer<vtkTransformPolyDataFilter> tpd2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    tpd2->SetInputConnection(planeData->GetOutputPort());
    tpd2->SetTransform(transP2);
    vtkSmartPointer<vtkOutlineFilter> outTpd2 = vtkSmartPointer<vtkOutlineFilter>::New();
    outTpd2->SetInputConnection(tpd2->GetOutputPort());
    vtkSmartPointer<vtkPolyDataMapper> tpdMapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    tpdMapper2->SetInputConnection(outTpd2->GetOutputPort());
    vtkSmartPointer<vtkActor> tpdActor2 = vtkSmartPointer<vtkActor>::New();
    tpdActor2->SetMapper(tpdMapper2);
    tpdActor2->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色
    
    
    vtkSmartPointer<vtkTransform> transP3 = vtkSmartPointer<vtkTransform>::New();
    transP3->Translate(13.27, .0, 33.30);  // 空间变换
    transP3->Scale(5, 5, 5);
    transP3->RotateY(90);
    vtkSmartPointer<vtkTransformPolyDataFilter> tpd3 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    tpd3->SetInputConnection(planeData->GetOutputPort());
    tpd3->SetTransform(transP3);
    vtkSmartPointer<vtkOutlineFilter> outTpd3 = vtkSmartPointer<vtkOutlineFilter>::New();
    outTpd3->SetInputConnection(tpd3->GetOutputPort());
    vtkSmartPointer<vtkPolyDataMapper> tpdMapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
    tpdMapper3->SetInputConnection(outTpd3->GetOutputPort());
    vtkSmartPointer<vtkActor> tpdActor3 = vtkSmartPointer<vtkActor>::New();
    tpdActor3->SetMapper(tpdMapper3);
    tpdActor3->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色
    
    vtkSmartPointer<vtkAppendPolyData> appendF = vtkSmartPointer<vtkAppendPolyData>::New();
    appendF->AddInputConnection(tpd1->GetOutputPort());
    appendF->AddInputConnection(tpd2->GetOutputPort());
    appendF->AddInputConnection(tpd3->GetOutputPort());
    
    vtkSmartPointer<vtkProbeFilter> prob = vtkSmartPointer<vtkProbeFilter>::New();
    prob->SetInputConnection(appendF->GetOutputPort());
    prob->SetSourceData(pl3d_ouput);
    
    
    vtkSmartPointer<vtkPolyDataMapper> proMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    proMapper->SetInputConnection(prob->GetOutputPort());
    proMapper->SetScalarRange(pl3d_ouput->GetScalarRange());
    
    vtkSmartPointer<vtkActor> probActor = vtkSmartPointer<vtkActor>::New();
    probActor->SetMapper(proMapper);
    // probActor->GetProperty()->SetColor(75.0 / 255.0, 75.0 / 255.0, 75.0 / 255.0); // 设置边框颜色
    
    
    
    // renderer 
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    // 上方颜色 (230, 242, 255)
    renderer->SetBackground(230.0 / 255.0, 242.0 / 255.0, 255.0 / 255.0);
    // 下方颜色 (250, 251, 255)
    renderer->SetBackground2(250.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0);
    // 启用渐变背景
    renderer->GradientBackgroundOn();
    
    
    // renderer->AddActor(franActor);
    // renderer->AddActor(spikeActor);
    renderer->AddActor(outlineActor);
    // renderer->AddActor(planeActor);
    // renderer->AddActor(cuttingActor);  // 按照源数据的网格截取
    renderer->AddActor(tpdActor1);   // probing 按指定的平面和网格截取
    renderer->AddActor(tpdActor2);
    renderer->AddActor(tpdActor3);
    renderer->AddActor(probActor);

    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}

VTKVisual3::~VTKVisual3()
{
    delete ui;
}
