cmake_minimum_required(VERSION 3.17)
project(mapabsas)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/sdl2")
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIR})

add_executable(mapabsas data.cpp data.h expand.cpp expand.h mapa.cpp mapa.h shaders.cpp shaders.h triangulate.cpp triangulate.h video.cpp video.h GL/gl3w.c GL/gl3w.h)
target_link_libraries(mapabsas ${SDL2_LIBRARIES} ${OPENGL_LIBRARY} ${SDL2_IMAGE_LIBRARY})
