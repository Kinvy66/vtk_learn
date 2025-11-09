/******************************************************************************
 * File     : VTKGridData.cpp
 * Brief    : 网格数据
 * Author   : Kinvy
 * Email    : kinvy66@163.com
 * Date     : 2025-11-07
 * Version  : V0.0.1
 * Detail   :
 * Copyright: Copyright By Kinvy, All Rights Reserved
******************************************************************************/
#include "VTKGridData.h"
#include "ui_VTKGridData.h"
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
#include <vtkColor.h>
#include <vtkNamedColors.h>
#include <vtkImageData.h>
#include <vtkDataSetMapper.h>
#include <vtkRectilinearGrid.h>
#include <vtkDoubleArray.h>
#include <vtkStructuredGrid.h>

VTKGridData::VTKGridData(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VTKGridData)
{
    ui->setupUi(this);    _pVTKWidget = new QVTKOpenGLNativeWidget();
    this->setCentralWidget(_pVTKWidget);
    
    vtkNew<vtkRenderer> renderer;
    this->_pVTKWidget->renderWindow()->SetWindowName("VTK Data");    
    this->_pVTKWidget->renderWindow()->AddRenderer(renderer);
    
    this->_pVTKWidget->renderWindow()->Render();
    
    vtkNew<vtkNamedColors> colors;
    
    
    int gridSize = 5;
    
    vtkNew<vtkImageData> imageData;
    
    imageData->SetExtent(0, gridSize - 1, 0, gridSize - 1, 0, gridSize - 1);
    double dpSpace[] = {1, 2, 3};
    imageData->SetSpacing(dpSpace);
    
    vtkNew<vtkDataSetMapper> imageDataMapper;
    imageDataMapper->SetInputData(imageData);
    vtkNew<vtkActor> imageDataActor;
    imageDataActor->GetProperty()->SetRepresentationToWireframe();   // 设置成线框格式
    imageDataActor->SetMapper(imageDataMapper);
    
    renderer->AddActor(imageDataActor);
    


     vtkNew<vtkRenderer> rendererL;
    this->_pVTKWidget->renderWindow()->AddRenderer(rendererL);
    
     vtkNew<vtkRenderer> rendererR;
     this->_pVTKWidget->renderWindow()->AddRenderer(rendererR);
     
     double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
     double centerViewport[4] = {0.33, 0.0, 0.66, 1.0};
     double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};
    
    rendererL->SetViewport(leftViewport);
    renderer->SetViewport(centerViewport);
    rendererR->SetViewport(rightViewport);
    rendererL->SetBackground(colors->GetColor3d("BurlyWood").GetData());
    renderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());
    rendererR->SetBackground(colors->GetColor3d("Plum").GetData());
    
    // RectilinearGrid
    vtkNew<vtkRectilinearGrid> rectilinearGrid;
    rectilinearGrid->SetExtent(0, gridSize - 1, 0, gridSize - 1, 0, gridSize - 1);
    
    vtkNew<vtkDoubleArray> xCoords;
    xCoords->SetNumberOfComponents(1);
    vtkNew<vtkDoubleArray> yCoords;
    yCoords->SetNumberOfComponents(1);
    vtkNew<vtkDoubleArray> zCoords;
    zCoords->SetNumberOfComponents(1);
    
    for (size_t i = 0; i < gridSize; ++i) {
        if (i ==0) {
            xCoords->InsertNextValue(0);
            yCoords->InsertNextValue(0);
            zCoords->InsertNextValue(0);
            continue;
            
        }
        double oldx = xCoords->GetValue(i - 1);
        double oldy = yCoords->GetValue(i - 1);
        double oldz = zCoords->GetValue(i - 1);
        xCoords->InsertNextValue(oldx + i * i);
        yCoords->InsertNextValue(oldy + i * i);
        zCoords->InsertNextValue(oldz + i * i);
        
    }
    rectilinearGrid->SetXCoordinates(xCoords);
    rectilinearGrid->SetYCoordinates(yCoords);
    rectilinearGrid->SetZCoordinates(zCoords);
    
    vtkNew<vtkDataSetMapper> rectilinearGridMapper;
    rectilinearGridMapper->SetInputData(rectilinearGrid);
    
    vtkNew<vtkActor> rectilinearGridActor;
    rectilinearGridActor->GetProperty()->SetRepresentationToWireframe();   // 设置成线框格式
    rectilinearGridActor->SetMapper(rectilinearGridMapper);
    
    rendererL->AddActor(rectilinearGridActor);
    
    // struct grid
    vtkNew<vtkStructuredGrid> structuredGrid;
    structuredGrid->SetExtent(0, gridSize - 1, 0, gridSize - 1, 0, gridSize - 1);
    vtkNew<vtkPoints> points;
    
    for (size_t i = 0; i < gridSize; i++) {
        for (size_t j = 0; j < gridSize; j++) {
            for (size_t k = 0; k < gridSize; k++) {
                double p[3] = {i, j + i * .5, k};
                points->InsertNextPoint(p[0], p[1], p[2]);
            }
        }
    }
    
    structuredGrid->SetPoints(points);
    
    vtkNew<vtkDataSetMapper> structuredGridMapper;
    structuredGridMapper->SetInputData(structuredGrid);
    
    vtkNew<vtkActor> structuredGridActor;
    structuredGridActor->GetProperty()->SetRepresentationToWireframe();   // 设置成线框格式
    structuredGridActor->SetMapper(structuredGridMapper);
    
    rendererR->AddActor(structuredGridActor);
    
    
       
}

VTKGridData::~VTKGridData()
{
    delete ui;
}
