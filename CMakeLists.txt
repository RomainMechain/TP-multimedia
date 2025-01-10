cmake_minimum_required(VERSION 2.8...3.27.6)


project(td)

add_definitions(-std=c++17)
set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)

set(OpenGL_GL_PREFERENCE LEGACY)




SET(SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${SHADERS})


add_executable(${PROJECT_NAME} ${SRC_LIST})

set (MY_SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set (MY_RES_PATH ${CMAKE_CURRENT_SOURCE_DIR})
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories(${PROJECT_BINARY_DIR})


find_package(GLEW 2.0 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(glm REQUIRED)
find_package(GLUT REQUIRED)

target_include_directories(td PUBLIC ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIR} )


if (APPLE)
 message(STATUS "APPLE detected")
 target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
 target_link_libraries(${PROJECT_NAME} "-framework GLUT")
 target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
 SET(NOWARNAPPLE "-D__gl_h_ -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED")
endif (APPLE)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
 message(STATUS "Linux detected")
 SET(addonflagslinux "-DGLEW_STATIC")
 set(GLEW_LIBRARY "-lGLEW")
 set(GLUT_LIBRARY "-lglut")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${NOWARNAPPLE} ${addonflagslinux}" )

SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${NOWARNAPPLE} ${addonflagslinux}" )


target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRAIRIES} ${GLEW_LIBRARY} ${OPENGL_gl_LIBRARY} ${OpenCV_LIBS} ${GLUT_LIBRARY})





 
# Add a test:
enable_testing()
add_test(SimpleTest ${PROJECT_NAME})

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${PROJECT_NAME})


