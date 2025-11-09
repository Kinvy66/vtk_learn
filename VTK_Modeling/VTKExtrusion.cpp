/******************************************************************************
 * File     : VTKExtrusion.cpp
 * Brief    : 拉伸建模
 * Author   : Kinvy
 * Email    : kinvy66@163.com
 * Date     : 2025-11-06
 * Version  : V0.0.1
 * Detail   :
 * Copyright: Copyright By Kinvy, All Rights Reserved
******************************************************************************/
#include "VTKExtrusion.h"
#include "ui_VTKExtrusion.h"
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
#include <vtkNamedColors.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkRotationalExtrusionFilter.h>
#include <vtkVectorText.h>

namespace       
{
class myRotationalExtrusionFilter : public vtkRotationalExtrusionFilter {
    // TODO
};
} // namespace      

VTKExtrusion::VTKExtrusion(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VTKExtrusion)
{
    ui->setupUi(this);
    _pVTKWidget = new QVTKOpenGLNativeWidget();
    this->setCentralWidget(_pVTKWidget);
    
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRegularPolygonSource> circle;
    circle->GeneratePolygonOn();
    circle->SetNumberOfSides(360);
    circle->SetRadius(2);
    circle->SetCenter(0, 0, 0);
     
    vtkNew<vtkLinearExtrusionFilter> linearExtrusion;
    linearExtrusion->SetInputConnection(circle->GetOutputPort());
    linearExtrusion->SetExtrusionTypeToPointExtrusion();   // 选择从一个点挤压
    linearExtrusion->SetExtrusionPoint(0, 0, 5);
    linearExtrusion->SetScaleFactor(.2);
    
    vtkNew<vtkPolyDataMapper> cylinderMapper;
    vtkNew<vtkActor> cylinderActor;
    cylinderMapper->SetInputConnection(linearExtrusion->GetOutputPort());
    cylinderActor->SetMapper(cylinderMapper);
    
    // extrusion
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> lines;
    vtkNew<vtkPolyData> profile;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(2, 0, 0);
    points->InsertNextPoint(2, 0, 1);
    points->InsertNextPoint(0, 0, 1);
    lines->InsertNextCell(4);
    lines->InsertCellPoint(0);
    lines->InsertCellPoint(1);
    lines->InsertCellPoint(2);
    lines->InsertCellPoint(3);
    profile->SetPoints(points);
    profile->SetLines(lines);
    
    vtkNew<vtkRotationalExtrusionFilter> rotationalExtrusion;
    rotationalExtrusion->SetInputData(profile);
    rotationalExtrusion->SetAngle(360);
    rotationalExtrusion->SetResolution(360);
    
    vtkNew<vtkPolyDataMapper> cylindeMapper2;
    vtkNew<vtkActor> cylinderActor2;
    cylindeMapper2->SetInputConnection(rotationalExtrusion->GetOutputPort());
    cylinderActor2->SetMapper(cylindeMapper2);
    cylinderActor2->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());
    cylinderActor2->SetPosition(5, 0, 0);
    
    // 文字挤压
    vtkNew<vtkVectorText> text;
    text->SetText("Hello World!");
    vtkNew<vtkLinearExtrusionFilter> textExtrusion;
    textExtrusion->SetInputConnection(text->GetOutputPort());
    textExtrusion->SetVector(0, 0, 1);
    textExtrusion->SetExtrusionTypeToVectorExtrusion();
    
    vtkNew<vtkPolyDataMapper> textMapper;
    vtkNew<vtkActor> textActor;
    textMapper->SetInputConnection(textExtrusion->GetOutputPort());
    textActor->SetMapper(textMapper);
    textActor->SetPosition(0, 5, 0);
    
    // 弹簧
    vtkNew<vtkRegularPolygonSource> circle2;
    circle2->GeneratePolygonOn();
    circle2->SetNumberOfSides(50);
    circle2->SetRadius(0.2);
    circle2->SetCenter(1, 0, 0);
    circle2->SetNormal(0, 1, 0);  // 设置法向
    
    vtkNew<vtkRotationalExtrusionFilter> rotationalExtrusion2;
    rotationalExtrusion2->SetInputConnection(circle2->GetOutputPort());
    rotationalExtrusion2->SetAngle(2160);
    rotationalExtrusion2->SetResolution(360);
    rotationalExtrusion2->SetTranslation(6);
    rotationalExtrusion2->SetDeltaRadius(1.0);
    
    vtkNew<vtkPolyDataMapper> springMapper;
    vtkNew<vtkActor> springActor;
    springMapper->SetInputConnection(rotationalExtrusion2->GetOutputPort());
    springActor->SetMapper(springMapper);
    // springActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());
    springActor->SetPosition(10, 0, 0);
    
      
    
    // renderer 
    vtkNew<vtkRenderer> renderer;
    // renderer->SetBackground(230.0 / 255.0, 242.0 / 255.0, 255.0 / 255.0);
    // renderer->SetBackground2(250.0 / 255.0, 251.0 / 255.0, 255.0 / 255.0);
    // renderer->GradientBackgroundOn();
    
    // renderer->AddActor(cylinderActor);
    // renderer->AddActor(cylinderActor2);
    // renderer->AddActor(textActor);
    renderer->AddActor(springActor);

    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    this->_pVTKWidget->renderWindow()->Render();
}

VTKExtrusion::~VTKExtrusion()
{
    delete ui;
}
