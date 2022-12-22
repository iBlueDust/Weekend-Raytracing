# CMakeList.txt : CMake project for Weekend Raytracing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Weekend Raytracing")

# Add source to this project's executable.
add_executable (WeekendRaytracing "src/main.cpp" "src/main.h" "src/vec3.h" "src/color.h" "src/ray.h" "src/hittable.h" "src/sphere.h" "src/hittable_list.h" "src/commons.h" "src/camera.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WeekendRaytracing PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
