cmake_minimum_required(VERSION 3.25)
set(PROJECT "web-browser")

project(${PROJECT}
  VERSION 0.0.0
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE HEADER_FILES include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(${PROJECT})
target_sources(${PROJECT} PRIVATE ${SOURCE_FILES})
target_include_directories(${PROJECT} PRIVATE include/)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebEngineWidgets)
qt_standard_project_setup()
qt_wrap_cpp(MOC_SRCS ${HEADER_FILES})
target_sources(${PROJECT} PRIVATE ${MOC_SRCS})
set_target_properties(${PROJECT} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT}
  Qt6::Core
  Qt6::Widgets
  Qt6::WebEngineWidgets)

# LuaJIT
pkg_check_modules(LuaJIT REQUIRED luajit)
include_directories(${LuaJIT_INCLUDE_DIRS})
link_directories(${LuaJIT_LIBRARY_DIRS})
target_link_libraries(${PROJECT} ${LuaJIT_LINK_LIBRARIES})

# CEF (experiment)
# set(CEF_PACKAGE_PATH "$ENV{CEF_PACKAGE_PATH}")
# message(STATUS "CEF path: ${CEF_PACKAGE_PATH}")
# include_directories(${CEF_PACKAGE_PATH} ${CEF_PACKAGE_PATH}/include)
# link_directories(${CEF_PACKAGE_PATH}/lib)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
