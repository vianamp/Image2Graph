#ifndef _IM2GRAPH_H
#define _IM2GRAPH_H

#include "includes.h"

class _Im2Graph {

    igraph_t iG;
    igraph_vector_t Weight;
    int _nb, _dx[8], _dy[8];

  public:
    
    int CreateUnGraphTopologyFrom2DImage(vtkImageData *Image, vtkMutableUndirectedGraph *G);
    
    int SetKernelSize(int nb);

    int ViewGraph(vtkMutableUndirectedGraph *G);

    int ProbeImageAndAssignToGraph(vtkImageData *Image, vtkMutableUndirectedGraph *G, const char Name[]);

    int UpdateWeights(vtkImageData *Image, vtkMutableUndirectedGraph *G, std::vector<vtkIdType> Buffer);

    int CreateInternaliGraphInstance(vtkMutableUndirectedGraph *G);

    int CopyScalarVector(vtkDataArray *Vector);

    int ShortestPath(igraph_integer_t node_i, igraph_integer_t node_j, std::vector<vtkIdType> *Route);

    _Im2Graph() {

      _nb = 4;
      igraph_vector_init(&Weight,0);
      _dx[0]=-1;_dx[1]=0;_dx[2]=1;_dx[3]=0;_dx[4]=-1;_dx[5]=1;_dx[6]=1;_dx[7]=-1;
      _dy[0]=0;_dy[1]=1;_dy[2]=0;_dy[3]=-1;_dy[4]=1;_dy[5]=1;_dy[6]=-1;_dy[7]=-1;
    }
    ~_Im2Graph();
  
};

#endif
