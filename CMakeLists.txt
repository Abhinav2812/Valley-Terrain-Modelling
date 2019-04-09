cmake_minimum_required(VERSION 2.8.9)
project (sim)

include_directories(include)
include_directories(src)
include_directories(res)
include_directories(shader)

file(GLOB SOURCES "src/*.cpp")
 file(GLOB helloworld_SRC
    "*.h"
    "*.cpp"
)

add_executable(sim ${SOURCES})
file(GLOB RES
  "res/*"
)
file(GLOB SHADER
  "shader/*"
)
file(COPY ${RES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})  
target_link_libraries(sim glut GLU GL glfw X11 pthread Xi dl GLEW)