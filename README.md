# Image2Graph
Tool to perform graph-based segmentation tasks on images.

# Introduction
We use iGraph to create a graph representation of a VTK image and perform smart segmentation on these images. For now only 2D images are supported.

# Method
After the user interactvely define one start and one end point, we calculate the shortest path between these points to create an image segmentation. The combination iGraph + VTK allows us to work on a one million nodes graph on the fly.

[iGraph]
http://igraph.org/redirect.html

[ VTK ]
http://www.vtk.org/
