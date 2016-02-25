#ifndef _IM2GRAPH_H
#define _IM2GRAPH_H

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

#endif
