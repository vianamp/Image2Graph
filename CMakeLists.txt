
cmake_minimum_required(VERSION 2.8)

PROJECT(Im2Graph)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Im2Graph MACOSX_BUNDLE Im2Graph)

if(VTK_LIBRARIES)
  target_link_libraries(Im2Graph ${VTK_LIBRARIES})
else()
  target_link_libraries(Im2Graph vtkHybrid vtkWidgets)
endif()

set(CMAKE_CXX_FLAGS "-I/usr/local/include/ -L/usr/local/lib/ -ligraph")
