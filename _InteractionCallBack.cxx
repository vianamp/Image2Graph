#include "_Im2Graph.h"
#include "_InteractionCallBack.h"

  // Callback initialization
  void _InteractionCallBack::Initialize(vtkPropPicker *picker, vtkCornerAnnotation *annotation, vtkImageViewer2 *viewer, _Im2Graph *im2g) {
    this -> Picker = picker;
    this -> Viewer = viewer;
    this -> Im2Graph = im2g;
    this -> Annotation = annotation;

    // Add all image points to polydata
    double r[3];
    Lines = vtkPolyData::New();
    LinesArray = vtkCellArray::New();
    vtkSmartPointer<vtkPoints> Points = vtkSmartPointer<vtkPoints>::New();
    for (vtkIdType id = 0; id < Viewer->GetInput()->GetNumberOfPoints(); id++) {
      Viewer -> GetInput() -> GetPoint(id,r);
      Points -> InsertNextPoint(r[0],r[1],r[2]);
    }
    Lines -> SetPoints(Points);

    // Create PolyData visualization
    vtkSmartPointer<vtkPolyDataMapper> LinesMapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
    LinesMapper -> SetInputData(Lines);
    vtkSmartPointer<vtkActor> LinesActor = vtkSmartPointer<vtkActor>::New();
    LinesActor -> SetMapper(LinesMapper);
    LinesActor -> GetProperty() -> SetColor(1,0,0);
    LinesActor -> GetProperty() -> SetLineWidth(5);

    // Add polydata to randerer
    this -> Viewer -> GetRenderer() -> AddActor(LinesActor);
  }
  
  // Setting callback function for user interaction
  void _InteractionCallBack::Execute(vtkObject *, unsigned long vtkNotUsed(event), void*) {

    vtkRenderWindowInteractor *interactor = this -> Viewer -> GetRenderWindow() -> GetInteractor();
    vtkImageData *image = this -> Viewer -> GetInput();
    vtkRenderer *renderer = this -> Viewer -> GetRenderer();
    vtkImageActor *actor = this -> Viewer -> GetImageActor();
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());

    this -> Picker -> Pick(interactor->GetEventPosition()[0],interactor->GetEventPosition()[1],0.0, renderer);

    vtkAssemblyPath *path = this -> Picker -> GetPath();
    bool validPick = false;
   
    if ( path ) {
      vtkCollectionSimpleIterator sit;
      path -> InitTraversal(sit);
      vtkAssemblyNode *node;
      for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i) {
        node = path -> GetNextNode(sit);
        if (actor == vtkImageActor::SafeDownCast(node->GetViewProp())) {
          validPick = true;
        }
      }
    }
   
    if ( !validPick ) {
      this -> Annotation -> SetText(0, "Off Image");
      interactor -> Render();
      return;
    }
 
    double pos[3];
    this -> Picker -> GetPickPosition(pos);
 
    int image_coordinate[3];
    image_coordinate[2] = 0;
    image_coordinate[1] = vtkMath::Round(pos[1]);
    image_coordinate[0] = vtkMath::Round(pos[0]);
    
    std::string message = "Location: ( ";
    message += vtkVariant(image_coordinate[0]).ToString() + ", ";
    message += vtkVariant(image_coordinate[1]).ToString() + ", ";
    message += vtkVariant(image_coordinate[2]).ToString() + " )\nValue: ( ";
   
    switch (image->GetScalarType()) {
      vtkTemplateMacro((vtkValueMessageTemplate<VTK_TT>(image,image_coordinate,message)));
      default:
        return;
    }
   
    vtkIdType id = image -> FindPoint(image_coordinate[0],image_coordinate[1],0);
    ControlPoints.push_back(id);

    if (ControlPoints.size() > 1) {
      std::vector<vtkIdType> Path;
      vtkIdType node_i = ControlPoints[ControlPoints.size()-1];
      vtkIdType node_j = ControlPoints[ControlPoints.size()-2];
      Im2Graph -> ShortestPath(node_i,node_j,&Path);
      printf("L[%d,%d] = %ld\n",(int)node_i,(int)node_j,Path.size());
      LinesArray -> InsertNextCell(Path.size());
      for (int i = 0; i < Path.size(); i++) {
        image -> GetPointData() -> GetScalars() -> SetTuple1(Path[i],65535);
        LinesArray -> InsertCellPoint(Path[i]);
      }
      LinesArray -> Modified();
      Lines -> SetLines(LinesArray);
      Lines -> Modified();
    }

    printf("testing polydata...\n");
    printf(">> %lld\n",Lines->GetNumberOfPoints());
    printf(">> %lld\n",Lines->GetNumberOfLines());

    image -> Modified();

    this -> Annotation -> SetText( 0, message.c_str() );
    interactor -> Render();
    style -> OnMouseMove();
  }
   
