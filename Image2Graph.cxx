// Image2Graph
// Image operations based on graphs.
// Matheus Viana, vianamp@gmail.com

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

#include "_Im2Graph.h"
#include "_InteractionCallBack.h"

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

  vtkSmartPointer<_InteractionCallBack> callback = vtkSmartPointer<_InteractionCallBack>::New();
  callback -> Initialize(propPicker,cornerAnnotation,imageViewer,Im2Graph);

  vtkInteractorStyleImage *imageStyle = imageViewer -> GetInteractorStyle();
  imageStyle -> AddObserver(vtkCommand::LeftButtonPressEvent, callback);

  renderWindowInteractor -> Initialize();
  renderWindowInteractor -> Start();

  return EXIT_SUCCESS;
}
