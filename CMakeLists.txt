cmake_minimum_required(VERSION 3.13.0)

include(FetchContent)

project(ywviewer)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 6.0
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.92.7-docking
)

FetchContent_MakeAvailable(raylib)
FetchContent_MakeAvailable(imgui)

add_library(imgui
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE INCLUDE_FILES "include/*.h")

add_executable(ywviewer ${SRC_FILES} ${INCLUDE_FILES})

target_compile_definitions(ywviewer PRIVATE _CRT_SECURE_NO_WARNINGS NO_FONT_AWESOME)
target_include_directories(ywviewer PRIVATE include)
target_link_libraries(ywviewer PRIVATE raylib imgui)
