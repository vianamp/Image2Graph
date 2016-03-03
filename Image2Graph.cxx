// Image2Graph
// Image operations based on graphs.
// Matheus Viana, vianamp@gmail.com [25.01.2016]

#include "_Im2Graph.h"
#include "_InteractionCallBack.h"

int main(int argc, char *argv[]) {

  #ifdef DEBUG
      printf("Debug mode in [on]\n");
  #endif

  // Load image from command line
  vtkSmartPointer<vtkTIFFReader> TIFFReader = vtkSmartPointer<vtkTIFFReader>::New();
  TIFFReader -> SetFileName(argv[1]);
  TIFFReader -> Update();

  vtkSmartPointer<vtkImageData> Image = TIFFReader -> GetOutput(); 

  // Create the image-graph converter
  vtkSmartPointer<vtkMutableUndirectedGraph> G = vtkSmartPointer<vtkMutableUndirectedGraph>::New();
  _Im2Graph *Im2Graph = new _Im2Graph();
  Im2Graph -> SetKernelSize(8);
  Im2Graph -> CreateUnGraphTopologyFrom2DImage(Image,G);
  Im2Graph -> ProbeImageAndAssignToGraph(Image,G,"GrayLevel");
  Im2Graph -> CreateInternaliGraphInstance(G);
  Im2Graph -> CopyScalarVector(G->GetEdgeData()->GetArray("GrayLevel"));

  // Generating visualization and interactor
  vtkSmartPointer<vtkImageViewer2> ImViewer = vtkSmartPointer<vtkImageViewer2>::New();
  ImViewer -> SetInputData(Image);

  // vtkPropPicker is used to pick an actor/prop given a selection point (in display coordinates) and a renderer. This class uses graphics hardware/rendering system to pick rapidly (as compared to using ray casting as does vtkCellPicker and vtkPointPicker). This class determines the actor/prop and pick position in world coordinates; point and cell ids are not determined.
  vtkSmartPointer<vtkPropPicker> PropPicker = vtkSmartPointer<vtkPropPicker>::New();
  PropPicker -> PickFromListOn();

  vtkImageActor *ImActor = ImViewer -> GetImageActor();
  ImActor -> SetOpacity(0.5);
  ImActor -> InterpolateOff();
  PropPicker -> AddPickList(ImActor);

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  ImViewer -> SetupInteractor(renderWindowInteractor);
  ImViewer -> SetSize(600, 600);

  vtkRenderer *Renderer = ImViewer -> GetRenderer();
  Renderer -> ResetCamera();
  Renderer -> GradientBackgroundOn();
  Renderer -> SetBackground(0.5, 0.5, 0.5);
  Renderer -> SetBackground2(0.2, 0.2, 0.2);

  // This is an annotation object that manages four text actors / mappers to provide annotation in the four corners of a viewport.
  vtkSmartPointer<vtkCornerAnnotation> CAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  CAnnotation -> SetLinearFontScaleFactor(2);
  CAnnotation -> SetNonlinearFontScaleFactor(1);
  CAnnotation -> SetMaximumFontSize(20);
  CAnnotation -> SetText(0,"Off Image");
  CAnnotation -> GetTextProperty() -> SetColor(1.0,0.7,0.0);

  ImViewer -> GetRenderer() -> AddViewProp(CAnnotation);

  // Local class _InteractionCallBack. Extracted from example:
  // http://www.vtk.org/Wiki/VTK/Examples/Cxx/Images/PickPixel2
  vtkSmartPointer<_InteractionCallBack> CallBack = vtkSmartPointer<_InteractionCallBack>::New();
  CallBack -> Initialize(PropPicker,CAnnotation,ImViewer,G,Im2Graph,1);

  // vtkInteractorStyleImage allows the user to interactively manipulate (rotate, pan, zoom etc.)
  vtkInteractorStyleImage *imageStyle = ImViewer -> GetInteractorStyle();
  imageStyle -> AddObserver(vtkCommand::LeftButtonPressEvent, CallBack);

  renderWindowInteractor -> Initialize();
  renderWindowInteractor -> Start();

  return EXIT_SUCCESS;
}
