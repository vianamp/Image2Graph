#include "_Im2Graph.h"
#include "_InteractionCallBack.h"

  // Callback initialization
  void _InteractionCallBack::Initialize(vtkPropPicker *picker, vtkCornerAnnotation *annotation, vtkImageViewer2 *viewer, vtkMutableUndirectedGraph *G, _Im2Graph *im2g, int wsize) {
    this -> _wsize = wsize;
    this -> Picker = picker;
    this -> Viewer = viewer;
    this -> vtkG = G;
    this -> Im2Graph = im2g;
    this -> Annotation = annotation;

    // Create _ss masks
    for (int i = -_wsize; i <= _wsize; i++) {
      for (int j = -_wsize; j <= _wsize; j++) {
        _ssdx.push_back(i);
        _ssdy.push_back(j);
      }
    }

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

    vtkImageData *image = this -> Viewer -> GetInput();
    vtkRenderer *renderer = this -> Viewer -> GetRenderer();
    vtkImageActor *actor = this -> Viewer -> GetImageActor();
    vtkRenderWindowInteractor *interactor = this -> Viewer -> GetRenderWindow() -> GetInteractor();
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());

    this -> Picker -> Pick(interactor->GetEventPosition()[0],interactor->GetEventPosition()[1],0.0, renderer);

    vtkAssemblyPath *path = this -> Picker -> GetPath();
    bool validPick = false;
   
    if ( path ) {
      vtkAssemblyNode *node;
      vtkCollectionSimpleIterator sit;
      path -> InitTraversal(sit);
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
    
    // Update point coordinate at the left bottom corner
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

    // Shortest path between the two selected points
    if ( ControlPoints.size() > 1 ) {
      std::vector<vtkIdType> Path;
      vtkIdType node_i = ControlPoints[ControlPoints.size()-1];
      vtkIdType node_j = ControlPoints[ControlPoints.size()-2];
      Im2Graph -> ShortestPath(node_i,node_j,&Path);

      //Smooth Path here?

      #ifdef DEBUG
          printf("Change pixels values...\n");
      #endif

      LinesArray -> InsertNextCell(Path.size());
      for (int i = 0; i < Path.size(); i++) {
        image -> GetPoint(Path[i],pos);
        for (int j = 0; j < _ssdx.size(); j++) {
          id = image -> FindPoint(pos[0]+_ssdx[j],pos[1]+_ssdy[j],pos[2]);
          if (id > 0) {
            image -> GetPointData() -> GetScalars() -> SetTuple1(id,65535);
            Buffer.push_back(id);
          }
        }
        LinesArray -> InsertCellPoint(Path[i]);
      }
      LinesArray -> Modified();
      Lines -> SetLines(LinesArray);
      Lines -> Modified();

      #ifdef DEBUG
          printf("Path information:\n");
          printf("\t#segments = %lld\n",Lines->GetNumberOfLines());
          printf("\tlength[%d,%d] = %ld\n",(int)node_i,(int)node_j,Path.size());
      #endif

      #ifdef DEBUG
          printf("Updating graph weights...\n");
      #endif

      Im2Graph -> UpdateWeights(image,vtkG,Buffer);
      Buffer.clear();

      Path.clear();
    }

    image -> Modified();

    this -> Annotation -> SetText( 0, message.c_str() );
    interactor -> Render();
    style -> OnMouseMove();
  }
   
