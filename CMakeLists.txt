cmake_minimum_required(VERSION 3.16)
set(PROJECT "web-browser")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT}
  VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)

# set(CEF_PACKAGE_PATH "$ENV{CEF_PACKAGE_PATH}")
# message(STATUS "CEF path: ${CEF_PACKAGE_PATH}")
# include_directories(${CEF_PACKAGE_PATH} ${CEF_PACKAGE_PATH}/include)
# link_directories(${CEF_PACKAGE_PATH}/lib)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

# LuaJIT
pkg_check_modules(LuaJIT REQUIRED luajit)
include_directories(${LuaJIT_INCLUDE_DIRS})
link_directories(${LuaJIT_LIBRARY_DIRS})
message(STATUS ":::: Lua path = ${LuaJIT_INCLUDE_DIRS} ${LuaJIT_LINK_LIBRARIES}")

add_executable(${PROJECT} src/main.cpp)
target_link_libraries(${PROJECT}
  Qt6::Widgets
  Qt6::WebEngineWidgets
  ${LuaJIT_LINK_LIBRARIES})
