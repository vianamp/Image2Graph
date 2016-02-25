
cmake_minimum_required(VERSION 2.8)

PROJECT(Image2Graph)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Image2Graph MACOSX_BUNDLE Image2Graph)

if(VTK_LIBRARIES)
  target_link_libraries(Image2Graph ${VTK_LIBRARIES})
else()
  target_link_libraries(Image2Graph vtkHybrid vtkWidgets)
endif()

set(CMAKE_CXX_FLAGS "-I/usr/local/include/ -L/usr/local/lib/ -ligraph")
