cmake_minimum_required(VERSION 2.8...3.5)

project(AlienAssignment)

add_executable(Assignment1.out Assignment1.cpp)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( Assignment1.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )