#include "_Im2Graph.h"

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
