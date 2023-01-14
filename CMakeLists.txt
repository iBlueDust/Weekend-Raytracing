# CMakeList.txt : CMake project for Weekend Raytracing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Weekend Raytracing")

# Add source to this project's executable.
add_executable (WeekendRaytracing "src/main.cpp" "src/main.h" "src/vec3.h" "src/color.h" "src/ray.h" "src/hittable.h" "src/sphere.h" "src/hittable_list.h" "src/commons.h" "src/camera.h" "src/rng.h" "src/mesh.h"  "src/bounding_box.h"  "src/bounding_volume_hierarchy.h")

# Flags
if (NOT CMAKE_BUILD_TYPE)
	seet(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WeekendRaytracing PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
