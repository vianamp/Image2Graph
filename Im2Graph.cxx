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

class _Im2Graph {

    igraph_t iG;
    igraph_vector_t Weight;
    int _nb, _dx[8], _dy[8];

  public:
    
    int CreateUnGraphTopologyFrom2DImage(vtkImageData *Image, vtkMutableUndirectedGraph *G);
    
    int SetKernelSize(int nb);

    int ViewGraph(vtkMutableUndirectedGraph *G);

    int ProbeImageAndAssignToGraph(vtkImageData *Image, vtkMutableUndirectedGraph *G, const char Name[]);

    int CreateInternaliGraphInstance(vtkMutableUndirectedGraph *G);

    int CopyScalarVector(vtkDataArray *Vector);

    int ShortestPath(igraph_integer_t node_i, igraph_integer_t node_j, std::vector<vtkIdType> *Route);

    _Im2Graph() {

      _nb = 4;
      _dx[0]=-1;_dx[1]=0;_dx[2]=1;_dx[3]=0;_dx[4]=-1;_dx[5]=1;_dx[6]=1;_dx[7]=-1;
      _dy[0]=0;_dy[1]=1;_dy[2]=0;_dy[3]=-1;_dy[4]=1;_dy[5]=1;_dy[6]=-1;_dy[7]=-1;
    }
    ~_Im2Graph();
  
};

int _Im2Graph::SetKernelSize(int nb) {
  if ( (nb!=4) && (nb!=8) ) {
    return EXIT_FAILURE;
  } else {
    _nb = nb;
  }
  return EXIT_SUCCESS;
}

int _Im2Graph::CreateUnGraphTopologyFrom2DImage(vtkImageData *Image, vtkMutableUndirectedGraph *G) {
  int k;
  vtkIdType i, j;
  double r[3], u[3];
  int *Dim = Image -> GetDimensions();
  vtkIdType N = Image -> GetNumberOfPoints();
  for (i = 0; i < N; i++) G -> AddVertex();

  printf("N = %d\n",(int)G->GetNumberOfVertices());

  vtkSmartPointer<vtkPoints> Points = vtkSmartPointer<vtkPoints>::New();

  for (i = 0; i < N; i++) {
    Image -> GetPoint(i,r);
    Points -> InsertNextPoint(r[0],r[1],0);
    for (k = _nb; k--;) {
      j = Image -> FindPoint(r[0]+_dx[k],r[1]+_dy[k],0);
      if (j > i) {
        G -> AddEdge(i,j);
      }
    }
  }

  G -> SetPoints(Points);

  printf("E = %d\n",(int)G->GetNumberOfEdges());

  return EXIT_SUCCESS;
}

int _Im2Graph::ViewGraph(vtkMutableUndirectedGraph *G) {
  vtkSmartPointer<vtkGraphLayoutView> graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
  graphLayoutView -> SetLayoutStrategy("Pass Through");
  graphLayoutView -> AddRepresentationFromInput(G);

  graphLayoutView -> SetEdgeColorArrayName("GrayLevel");
  graphLayoutView -> ColorEdgesOn();

  graphLayoutView -> SetVertexColorArrayName("GrayLevel");
  graphLayoutView -> ColorVerticesOn();

  graphLayoutView -> ResetCamera();
  graphLayoutView -> Render();
  graphLayoutView -> GetInteractor()->Start();

  return EXIT_SUCCESS;
}

int _Im2Graph::ProbeImageAndAssignToGraph(vtkImageData *Image, vtkMutableUndirectedGraph *G, const char Name[]) {

  vtkSmartPointer<vtkEdgeListIterator> eit = vtkSmartPointer<vtkEdgeListIterator>::New();
  G -> GetEdges(eit);
  vtkSmartPointer<vtkVertexListIterator> vit = vtkSmartPointer<vtkVertexListIterator>::New();
  G -> GetVertices(vit);
 
  vtkSmartPointer<vtkDoubleArray> W = vtkSmartPointer<vtkDoubleArray>::New();
  W -> SetNumberOfComponents(1);
  W -> SetName(Name);

  vtkSmartPointer<vtkDoubleArray> Wv = vtkSmartPointer<vtkDoubleArray>::New();
  Wv -> SetNumberOfComponents(1);
  Wv -> SetName(Name);
 
  double vi, vj;
  vtkEdgeType e;
  while( eit->HasNext() ) {
    e = eit -> Next();
    vi = Image -> GetPointData() -> GetScalars() -> GetTuple1(e.Source);
    vj = Image -> GetPointData() -> GetScalars() -> GetTuple1(e.Target);
    W -> InsertNextValue(0.5*(vi+vj));
  }

  G -> GetEdgeData() -> AddArray(W);

  vtkIdType i;
  while( vit->HasNext() ) {
    i = vit -> Next();
    vi = Image -> GetPointData() -> GetScalars() -> GetTuple1(i);
    Wv -> InsertNextValue(vi);
  }

  G -> GetVertexData() -> AddArray(Wv);

  return EXIT_SUCCESS;
}

int _Im2Graph::CreateInternaliGraphInstance(vtkMutableUndirectedGraph *G) {

  vtkEdgeType e;
  igraph_vector_t Edges;
  igraph_vector_init(&Edges,0);
  igraph_empty(&iG,(long int)G->GetNumberOfVertices(),0);

  vtkSmartPointer<vtkEdgeListIterator> eit = vtkSmartPointer<vtkEdgeListIterator>::New();
  G -> GetEdges(eit);

  while( eit->HasNext() ) {
    e = eit -> Next();
    igraph_vector_push_back(&Edges,e.Source);
    igraph_vector_push_back(&Edges,e.Target);
  }
  igraph_add_edges(&iG,&Edges,NULL);
  igraph_vector_destroy(&Edges);

  printf("E = %d (from iGraph)\n",igraph_ecount(&iG));

  return EXIT_SUCCESS;
}

int _Im2Graph::CopyScalarVector(vtkDataArray *Vector) {
  igraph_vector_destroy(&Weight);
  igraph_vector_init(&Weight,igraph_ecount(&iG));
  for (igraph_integer_t i = 0; i < igraph_ecount(&iG); i++) {
    VECTOR(Weight)[i] = Vector -> GetTuple1((vtkIdType)i);
  }
  return EXIT_SUCCESS;
}

int _Im2Graph::ShortestPath(igraph_integer_t node_i, igraph_integer_t node_j, std::vector<vtkIdType> *Route) {
  igraph_vector_ptr_t vecs, evecs;
  igraph_vector_ptr_init(&vecs,1);
  igraph_vector_ptr_init(&evecs,1);
  for (igraph_integer_t i = 0; i < igraph_vector_ptr_size(&vecs); i++) {
    VECTOR(vecs)[i] = calloc(1, sizeof(igraph_vector_t));
    igraph_vector_init((igraph_vector_t*)VECTOR(vecs)[i], 0);
    VECTOR(evecs)[i] = calloc(1, sizeof(igraph_vector_t));
    igraph_vector_init((igraph_vector_t*)VECTOR(evecs)[i], 0);
  }
  igraph_get_shortest_paths_dijkstra(&iG,&vecs,&evecs,node_i,igraph_vss_1(node_j),&Weight,IGRAPH_ALL,0,0);

  igraph_vector_t *Path = (igraph_vector_t*)VECTOR(vecs)[0];

  printf("L = %ld\n",igraph_vector_size(Path));

  for (igraph_integer_t i = 0; i < igraph_vector_size(Path); i++) {
    Route -> push_back((vtkIdType)VECTOR(*Path)[i]);
  }

  return EXIT_SUCCESS;
}

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

 
class vtkImageInteractionCallback : public vtkCommand {
  public:
    static vtkImageInteractionCallback *New() {
      return new vtkImageInteractionCallback;
    }
   
    vtkImageInteractionCallback() {
      this -> Viewer     = NULL;
      this -> Picker     = NULL;
      this -> Annotation = NULL;
    }
   
    ~vtkImageInteractionCallback() {
      this -> Viewer     = NULL;
      this -> Picker     = NULL;
      this -> Annotation = NULL;
    }
   
    void Initialize(vtkPropPicker *picker, vtkCornerAnnotation *annotation, vtkImageViewer2 *viewer, _Im2Graph *im2g) {
      this -> Picker = picker;
      this -> Viewer = viewer;
      this -> Im2Graph = im2g;
      this -> Annotation = annotation;

      double r[3];
      Lines = vtkPolyData::New();
      LinesArray = vtkCellArray::New();
      vtkSmartPointer<vtkPoints> Points = vtkSmartPointer<vtkPoints>::New();
      for (vtkIdType id = 0; id < Viewer->GetInput()->GetNumberOfPoints(); id++) {
        Viewer -> GetInput() -> GetPoint(id,r);
        Points -> InsertNextPoint(r[0],r[1],r[2]);
      }
      Lines -> SetPoints(Points);

      vtkSmartPointer<vtkPolyDataMapper> LinesMapper =  vtkSmartPointer<vtkPolyDataMapper>::New();
      LinesMapper -> SetInputData(Lines);
      vtkSmartPointer<vtkActor> LinesActor = vtkSmartPointer<vtkActor>::New();
      LinesActor -> SetMapper(LinesMapper);
      LinesActor -> GetProperty() -> SetColor(1,0,0);
      LinesActor -> GetProperty() -> SetLineWidth(5);
      this -> Viewer -> GetRenderer() -> AddActor(LinesActor);
    }
   
    virtual void Execute(vtkObject *, unsigned long vtkNotUsed(event), void*) {

      vtkRenderWindowInteractor *interactor = this -> Viewer -> GetRenderWindow() -> GetInteractor();
      vtkImageData *image = this -> Viewer -> GetInput();
      vtkRenderer *renderer = this -> Viewer -> GetRenderer();
      vtkImageActor *actor = this -> Viewer -> GetImageActor();
      vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());
    
      this -> Picker -> Pick(interactor->GetEventPosition()[0],interactor->GetEventPosition()[1],0.0, renderer);
   
      vtkAssemblyPath *path = this -> Picker -> GetPath();
      bool validPick = false;
   
      if (path) {
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
   
      if (!validPick) {
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
   
      // Do something

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
   
  private:
    vtkPolyData *Lines;
    vtkCellArray *LinesArray;
    _Im2Graph *Im2Graph;
    vtkPropPicker *Picker;
    vtkImageViewer2 *Viewer;
    vtkCornerAnnotation *Annotation;
    std::vector<vtkIdType> ControlPoints;
};

int main(int argc, char *argv[]) {

  vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
 
  std::string inputFilename = argv[1];

  vtkSmartPointer<vtkTIFFReader> tiffReader = vtkSmartPointer<vtkTIFFReader>::New();
  tiffReader -> SetFileName(inputFilename.c_str());
  tiffReader -> Update();

  vtkSmartPointer<vtkImageData> Image = tiffReader -> GetOutput();

  // Create the graph

  vtkSmartPointer<vtkMutableUndirectedGraph> G = vtkSmartPointer<vtkMutableUndirectedGraph>::New();
  _Im2Graph *Im2Graph = new _Im2Graph();
  Im2Graph -> SetKernelSize(8);
  Im2Graph -> CreateUnGraphTopologyFrom2DImage(Image,G);
  Im2Graph -> ProbeImageAndAssignToGraph(Image,G,"GrayLevel");
  Im2Graph -> CreateInternaliGraphInstance(G);
  Im2Graph -> CopyScalarVector(G->GetEdgeData()->GetArray("GrayLevel"));

  // Visualization

  imageViewer -> SetInputData(Image);

  vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
  propPicker -> PickFromListOn();

  vtkImageActor *imageActor = imageViewer -> GetImageActor();
  imageActor -> SetOpacity(0.5);
  propPicker -> AddPickList(imageActor);

  imageActor -> InterpolateOff();

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  imageViewer -> SetupInteractor(renderWindowInteractor);
  imageViewer -> SetSize(600, 600);

  vtkRenderer *renderer = imageViewer -> GetRenderer();
  renderer -> ResetCamera();
  renderer -> GradientBackgroundOn();
  renderer -> SetBackground(0.5, 0.5, 0.5);
  renderer -> SetBackground2(0.2, 0.2, 0.2);

  vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  cornerAnnotation -> SetLinearFontScaleFactor(2);
  cornerAnnotation -> SetNonlinearFontScaleFactor(1);
  cornerAnnotation -> SetMaximumFontSize(20);
  cornerAnnotation -> SetText(0, "Off Image");
  cornerAnnotation -> GetTextProperty() -> SetColor(1.0,0.7,0.0);

  imageViewer -> GetRenderer() -> AddViewProp(cornerAnnotation);

  vtkSmartPointer<vtkImageInteractionCallback> callback = vtkSmartPointer<vtkImageInteractionCallback>::New();
  callback -> Initialize(propPicker,cornerAnnotation,imageViewer,Im2Graph);

  vtkInteractorStyleImage *imageStyle = imageViewer -> GetInteractorStyle();
  imageStyle -> AddObserver(vtkCommand::LeftButtonPressEvent, callback);

  renderWindowInteractor -> Initialize();
  renderWindowInteractor -> Start();

  return EXIT_SUCCESS;
}