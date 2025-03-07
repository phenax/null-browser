cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(web-browser
  VERSION 0.0.0
  LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

add_executable(web-browser src/main.cpp)

target_link_libraries(web-browser Qt6::Widgets Qt6::WebEngineWidgets)
