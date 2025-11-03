
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(VTK_Hello LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VTK_ROOT "C:/opt/vtk/install")                       # VTK 安装根目录
set(QT_ROOT "C:/Qt/6.5.3/msvc2019_64")                   # Qt 安装根目录

# -----------------------------
# 自动推断 CMake 配置路径
# -----------------------------
set(VTK_DIR "${VTK_ROOT}/lib/cmake/vtk-9.4")
set(Qt6_DIR "${QT_ROOT}/lib/cmake/Qt6")

find_package(VTK REQUIRED REQUIRED PATHS ${VTK_DIR})

if (NOT VTK_FOUND)
message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()


file(GLOB PROJECT_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB PROJECT_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
add_executable(VTK_Hello  ${PROJECT_SOURCE_FILES})
target_link_libraries(VTK_Hello PRIVATE ${VTK_LIBRARIES}
)
# vtk_module_autoinit is needed
vtk_module_autoinit(
TARGETS VTK_Hello
MODULES ${VTK_LIBRARIES}
)

