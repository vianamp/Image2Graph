#include "_Im2Graph.h"

  // Selection of 4 or 8 connectivity
  int _Im2Graph::SetKernelSize(int nb) {
    if ( (nb!=4) && (nb!=8) ) {
      return EXIT_FAILURE;
    } else {
      _nb = nb;
    }
    return EXIT_SUCCESS;
  }

  // Creating the graph from a given 2D image
  int _Im2Graph::CreateUnGraphTopologyFrom2DImage(vtkImageData *Image, vtkMutableUndirectedGraph *G) {
    vtkIdType i, j;
    double r[3], u[3];
    int k, *Dim = Image -> GetDimensions();
    vtkIdType N = Image -> GetNumberOfPoints();
    for (i = 0; i < N; i++) G -> AddVertex();

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

    #ifdef DEBUG
        printf("N = %d\n",(int)G->GetNumberOfVertices());
        printf("E = %d\n",(int)G->GetNumberOfEdges());
    #endif

    return EXIT_SUCCESS;
  }

  // Grapg visualization
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

  // Probe a vtkImage and assign the scalars to nodes and edges
  int _Im2Graph::ProbeImageAndAssignToGraph(vtkImageData *Image, vtkMutableUndirectedGraph *G, const char Name[]) {

    // Iterators for vertices and edges
    vtkSmartPointer<vtkEdgeListIterator> eit = vtkSmartPointer<vtkEdgeListIterator>::New();
    vtkSmartPointer<vtkVertexListIterator> vit = vtkSmartPointer<vtkVertexListIterator>::New();
    G -> GetEdges(eit);
    G -> GetVertices(vit);

    // Scalar array for edges
    vtkSmartPointer<vtkDoubleArray> We = vtkSmartPointer<vtkDoubleArray>::New();
    We -> SetNumberOfComponents(1);
    We -> SetName(Name);

    // Scalar array for vertices
    vtkSmartPointer<vtkDoubleArray> Wv = vtkSmartPointer<vtkDoubleArray>::New();
    Wv -> SetNumberOfComponents(1);
    Wv -> SetName(Name);
   
    // Probe edges
    double vi, vj;
    vtkEdgeType e;
    while( eit->HasNext() ) {
      e = eit -> Next();
      vi = Image -> GetPointData() -> GetScalars() -> GetTuple1(e.Source);
      vj = Image -> GetPointData() -> GetScalars() -> GetTuple1(e.Target);
      We -> InsertNextValue( 0.5 * (vi+vj) );
    }

    // Probe nodes
    vtkIdType i;
    while( vit->HasNext() ) {
      i = vit -> Next();
      vi = Image -> GetPointData() -> GetScalars() -> GetTuple1(i);
      Wv -> InsertNextValue(vi);
    }

    // Assign to the Graph
    G -> GetEdgeData() -> AddArray(We);
    G -> GetVertexData() -> AddArray(Wv);

    return EXIT_SUCCESS;
  }

  // Create an internal instance of iGraph
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

    #ifdef DEBUG
        printf("E = %d (from iGraph)\n",igraph_ecount(&iG));
    #endif

    return EXIT_SUCCESS;
  }

  // Assign the valus of Vector to the internal vector Weight
  int _Im2Graph::CopyScalarVector(vtkDataArray *Vector) {
    igraph_vector_destroy(&Weight);
    igraph_vector_init(&Weight,igraph_ecount(&iG));
    for (igraph_integer_t i = 0; i < igraph_ecount(&iG); i++) {
      VECTOR(Weight)[i] = Vector -> GetTuple1((vtkIdType)i);
    }
    return EXIT_SUCCESS;
  }

  // Calculate the shortest weighted path between two nodes
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

    for (igraph_integer_t i = 0; i < igraph_vector_size(Path); i++) {
      Route -> push_back((vtkIdType)VECTOR(*Path)[i]);
    }

    igraph_vector_ptr_destroy(&vecs);
    igraph_vector_ptr_destroy(&evecs);
    return EXIT_SUCCESS;
  }
