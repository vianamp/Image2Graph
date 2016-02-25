#include <list>
#include <cmath>
#include <cstdio>
#include <random>
#include <cstdlib>
#include <cstring>
#include <algorithm>

//VTK

  // CORE
  #include <vtkMath.h>
  #include <vtkPoints.h>
  #include <vtkCommand.h>
  #include <vtkRenderer.h>
  #include <vtkProperty.h>
  #include <vtkImageData.h>
  #include <vtkPointData.h>
  #include <vtkImageCast.h>
  #include <vtkPropPicker.h>
  #include <vtkSmartPointer.h>
  #include <vtkDataSetAttributes.h>

  // ARRAY
  #include <vtkDataArray.h>
  #include <vtkDoubleArray.h>
  #include <vtkUnsignedShortArray.h>

  // GRAPH
  #include <vtkAssemblyPath.h>
  #include <vtkGraphLayoutView.h>
  #include <vtkEdgeListIterator.h>
  #include <vtkVertexListIterator.h>
  #include <vtkMutableUndirectedGraph.h>
  #include <vtkDijkstraGraphGeodesicPath.h>


  // VIEW
  #include <vtkImageActor.h>
  #include <vtkTextProperty.h>
  #include <vtkRenderWindow.h>
  #include <vtkImageViewer2.h>
  #include <vtkPolyDataMapper.h>
  #include <vtkCornerAnnotation.h>
  #include <vtkInteractorStyleImage.h>
  #include <vtkRenderWindowInteractor.h>

  // IO
  #include <vtkTIFFReader.h>
  #include <vtkTIFFWriter.h>

// iGraph

#include <igraph/igraph.h>

// MACROS

#define DEBUG