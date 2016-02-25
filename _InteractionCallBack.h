#ifndef _INTERCALLBACK_H
#define _INTERCALLBACK_H

#include <list>
#include <cmath>
#include <cstdio>
#include <random>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include <vtkSmartPointer.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkCircularLayoutStrategy.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkGraphLayoutView.h>
#include <vtkIntArray.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSimple2DLayoutStrategy.h>
#include <vtkPoints.h>
#include <vtkVersion.h>
#include <vtkAssemblyPath.h>
#include <vtkCell.h>
#include <vtkLine.h>
#include <vtkPolyLine.h>
#include <vtkCommand.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkTIFFReader.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkUnsignedShortArray.h>
#include <vtkDoubleArray.h>
#include <vtkTIFFWriter.h>
#include <vtkPropPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkImageNoiseSource.h>
#include <vtkImageCast.h>
#include <vtkMath.h>
#include <vtkEdgeListIterator.h>
#include <vtkVertexListIterator.h>
#include <vtkGraphToPolyData.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <igraph/igraph.h>


template<typename T> void vtkValueMessageTemplate(vtkImageData* image, int* position, std::string& message) {
  T* tuple = ((T*)image -> GetScalarPointer(position));
  int components = image -> GetNumberOfScalarComponents();
  for (int c = 0; c < components; ++c) {
    message += vtkVariant(tuple[c]).ToString();
    if (c != (components - 1)) {
      message += ", ";
    }
  }
  message += " )";
}

 
class _InteractionCallBack : public vtkCommand {

  private:

    vtkPolyData *Lines;
    vtkCellArray *LinesArray;
    _Im2Graph *Im2Graph;
    vtkPropPicker *Picker;
    vtkImageViewer2 *Viewer;
    vtkCornerAnnotation *Annotation;
    std::vector<vtkIdType> ControlPoints;

  public:

    _InteractionCallBack() {
      this -> Viewer     = NULL;
      this -> Picker     = NULL;
      this -> Annotation = NULL;
    }
   
    ~_InteractionCallBack() {
      this -> Viewer     = NULL;
      this -> Picker     = NULL;
      this -> Annotation = NULL;
    }

    static _InteractionCallBack *New() {
      return new _InteractionCallBack;
    }
   
    void Initialize(vtkPropPicker *picker, vtkCornerAnnotation *annotation, vtkImageViewer2 *viewer, _Im2Graph *im2g);

    virtual void Execute(vtkObject *, unsigned long vtkNotUsed(event), void*);

};

#endif
