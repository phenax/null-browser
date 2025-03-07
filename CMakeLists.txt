cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(web-browser
  VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

# add_definitions(-DCEF_USE_QT_EVENT_LOOP)

# set(CEF_PACKAGE_PATH "$ENV{CEF_PACKAGE_PATH}")
# message(STATUS "CEF path: ${CEF_PACKAGE_PATH}")
# include_directories(${CEF_PACKAGE_PATH} ${CEF_PACKAGE_PATH}/include)
# link_directories(${CEF_PACKAGE_PATH}/lib)

find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

add_executable(web-browser src/main.cpp)
target_link_libraries(web-browser Qt6::Widgets Qt6::WebEngineWidgets cef cef_dll_wrapper)
