/******************************************************************************
 * File     : VTKPlatform.cpp
 * Brief    : vtk 可视化方法
 * Author   : Kinvy
 * Email    : kinvy66@163.com
 * Date     : 2025-11-04
 * Version  : V0.0.1
 * Detail   : 3. Glyph
 *            4. 流线、流管、流面、流带
 * Copyright: Copyright By Kinvy, All Rights Reserved
******************************************************************************/
#include "VTKVisual2.h"
#include "ui_VTKVisual2.h"
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

VTKVisual2::VTKVisual2(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VTKVisual2)
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
    
    // 微分求解器
    vtkSmartPointer<vtkRungeKutta4> integ = vtkSmartPointer<vtkRungeKutta4>::New();
    vtkSmartPointer<vtkStreamTracer> streamer = vtkSmartPointer<vtkStreamTracer>::New();
    streamer->SetIntegrator(integ);
    streamer->SetInputData(pl3d_ouput);
    streamer->SetStartPosition(15, 5, 32);
    streamer->SetMaximumPropagation(100);
    streamer->SetInitialIntegrationStep(.1);
    streamer->SetIntegrationDirectionToBackward();
    
    // 流线 流管
    vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
    streamTube->SetInputConnection(streamer->GetOutputPort());
    streamTube->SetRadius(0.06);
    streamTube->SetNumberOfSides(12);
    
    vtkSmartPointer<vtkPolyDataMapper> singleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    singleMapper->SetInputConnection(streamTube->GetOutputPort());
    singleMapper->SetScalarRange(pl3d_ouput->GetPointData()->GetScalars()->GetRange());
    
    vtkSmartPointer<vtkActor> singleActor = vtkSmartPointer<vtkActor>::New();
    singleActor->SetMapper(singleMapper);
    
    vtkSmartPointer<vtkLineSource> seeds = vtkSmartPointer<vtkLineSource>::New();
    seeds->SetPoint1(15, -5, 32);
    seeds->SetPoint2(15, 5, 32);
    seeds->SetResolution(21);
    
    vtkSmartPointer<vtkStreamTracer> streamer2 = vtkSmartPointer<vtkStreamTracer>::New();
    streamer2->SetIntegrator(integ);
    streamer2->SetInputData(pl3d_ouput);
    // streamer2->SetStartPosition(15, 5, 32);
    streamer2->SetMaximumPropagation(100);
    streamer2->SetInitialIntegrationStep(.1);
    streamer2->SetIntegrationDirectionToBackward();
    streamer2->SetSourceConnection(seeds->GetOutputPort());
    
    // 定义流面
    vtkSmartPointer<vtkRuledSurfaceFilter> scalarSurface = vtkSmartPointer<vtkRuledSurfaceFilter>::New();
    scalarSurface->SetInputConnection(streamer2->GetOutputPort());
    scalarSurface->SetRuledModeToPointWalk();
    scalarSurface->SetOnRatio(2);
    
    
    // 多根流线组成流面
    vtkSmartPointer<vtkPolyDataMapper> multipleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    multipleMapper->SetInputConnection(scalarSurface->GetOutputPort());
    multipleMapper->SetScalarRange(pl3d_ouput->GetPointData()->GetScalars()->GetRange());
    
    vtkSmartPointer<vtkActor> multipleActor = vtkSmartPointer<vtkActor>::New();
    multipleActor->SetMapper(multipleMapper);
    
    
    // vtkDataArray* scalars = pl3d_ouput->GetPointData()->GetScalars();
    // double* range = scalars->GetRange();
    // qDebug() << range[0] << ":" << range[1];
    
    // glyph
    vtkSmartPointer<vtkPolyDataReader> fran = vtkSmartPointer<vtkPolyDataReader>::New();
    fran->SetFileName("../../../Data/fran_cut.vtk");
    
    // 计算法向量
    vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputConnection(fran->GetOutputPort());   
    normals->FlipNormalsOn();  // 使法向量指向外侧
    
    vtkSmartPointer<vtkMaskPoints> ptMask = vtkSmartPointer<vtkMaskPoints>::New();
    ptMask->SetInputConnection(normals->GetOutputPort());
    ptMask->SetOnRatio(10);     // 每10个点取一个
    ptMask->RandomModeOn();     // 设置为随机采样每10个中随机取一个
    
    // 使用圆锥表示向量
    vtkSmartPointer<vtkConeSource> cone =  vtkSmartPointer<vtkConeSource>::New();
    cone->SetResolution(6);
    // 坐标变换，使原点移动到圆锥的底部位置
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(.5, 0, 5);
    
    vtkSmartPointer<vtkTransformPolyDataFilter> transforF = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transforF->SetInputConnection(cone->GetOutputPort());
    transforF->SetTransform(transform);
    
    // 创建glyph
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(ptMask->GetOutputPort());
    glyph->SetSourceConnection(transforF->GetOutputPort());   // 设置用圆锥表示
    glyph->SetVectorModeToUseNormal();   // 设置向量使用法向， 不使用法向的话都是指向一个方向
    glyph->SetScaleModeToScaleByVector();  // 设置尺度变换
    glyph->SetScaleFactor(0.004);
    
    vtkSmartPointer<vtkPolyDataMapper> spikeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    spikeMapper->SetInputConnection(glyph->GetOutputPort());
    
    vtkSmartPointer<vtkActor> spikeActor = vtkSmartPointer<vtkActor>::New();
    spikeActor->SetMapper(spikeMapper);
    
    vtkSmartPointer<vtkPolyDataMapper> franMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    franMapper->SetInputConnection(normals->GetOutputPort());
    
    vtkSmartPointer<vtkActor> franActor = vtkSmartPointer<vtkActor>::New();
    franActor->SetMapper(franMapper);
    
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
    
    
    // renderer->AddActor(franActor);
    // renderer->AddActor(spikeActor);
    renderer->AddActor(singleActor);
    renderer->AddActor(outlineActor);
    renderer->AddActor(multipleActor);
    
    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}

VTKVisual2::~VTKVisual2()
{
    delete ui;
}
