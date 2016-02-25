#ifndef _INTERCALLBACK_H
#define _INTERCALLBACK_H

#include "includes.h"

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
