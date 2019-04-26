cmake_minimum_required (VERSION 3.5)
project (Grab)

# Adapt this to your local situation.
find_package (OpenCV 3.4.1 REQUIRED)
include_directories (${OpenCV_INCLUDE_DIRS})
link_directories (${OpenCV_LINK_DIRS})

set (CMAKE_CXX_STANDARD 11)
add_executable (Grab Grab.cpp)
target_link_libraries (Grab ${OpenCV_LIBS})
