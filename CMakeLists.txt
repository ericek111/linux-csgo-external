cmake_minimum_required(VERSION 2.8.4)
project(csgo_external)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11 -Wunused")
set(EXECUTABLE_OUTPUT_PATH ./build)

file(GLOB SOURCE_FILES *.cpp)
add_executable(csgo_external ${SOURCE_FILES})
target_link_libraries(csgo_external "-lX11 -lconfig++ -lXtst -lm -lXi -lXfixes -lXcomposite -lXext -lpthread")
