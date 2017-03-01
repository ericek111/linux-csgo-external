cmake_minimum_required(VERSION 2.8.4)
project(csgo_external)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11")
set(EXECUTABLE_OUTPUT_PATH ./build)

set(SOURCE_FILES log.cpp remote.cpp hack.cpp main.cpp draw.cpp ewindows/EComponent.cpp ewindows/EView.cpp ewindows/ELabel.cpp ewindows/ETheme.cpp ewindows/EWindow.cpp ewindows/EWindowsManager.cpp)
#add_subdirectory(ewindows)
add_executable(csgo_external ${SOURCE_FILES})

target_link_libraries(csgo_external -lX11 -lXtst -lm -lXi -lXfixes -lXcomposite -lXext -lpthread)
