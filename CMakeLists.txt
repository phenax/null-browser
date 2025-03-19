cmake_minimum_required(VERSION 3.25)
set(PROJECT "null-browser")

project(${PROJECT}
  VERSION 0.0.0
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(AUTOMOC ON)
find_package(PkgConfig REQUIRED)

# Source
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
list(FILTER SOURCE_FILES EXCLUDE REGEX "src/main.cpp$")
file(GLOB_RECURSE HEADER_FILES include/*.hpp)
file(GLOB_RECURSE SPEC_FILES spec/*.cpp)

# project
add_executable(${PROJECT})
target_sources(${PROJECT} PRIVATE src/main.cpp ${SOURCE_FILES})
target_include_directories(${PROJECT} PRIVATE include/)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebEngineWidgets Test)
qt_standard_project_setup()
qt_wrap_cpp(MOC_SRCS ${HEADER_FILES})
target_sources(${PROJECT} PRIVATE ${MOC_SRCS})
target_link_libraries(${PROJECT}
  Qt6::Core
  Qt6::Widgets
  Qt6::WebEngineWidgets)

# LuaJIT
pkg_check_modules(LuaJIT REQUIRED luajit)
target_include_directories(${PROJECT} PRIVATE ${LuaJIT_INCLUDE_DIRS})
target_link_libraries(${PROJECT} ${LuaJIT_LINK_LIBRARIES})

# LibUV
pkg_check_modules(LibUV REQUIRED libuv)
target_include_directories(${PROJECT} PRIVATE ${LibUV_INCLUDE_DIRS})
target_link_libraries(${PROJECT} ${LibUV_LINK_LIBRARIES})

# Lua.LUV
pkg_check_modules(LuaLUV REQUIRED libluv)
target_include_directories(${PROJECT} PRIVATE ${LuaLUV_INCLUDE_DIRS})
target_link_libraries(${PROJECT} ${LuaLUV_LINK_LIBRARIES})


# Testing
enable_testing()
add_executable(tests)
target_sources(tests PRIVATE ${SOURCE_FILES} ${MOC_SRCS} ${SPEC_FILES})
target_include_directories(tests PRIVATE
  include/
  spec/
  ${LuaJIT_INCLUDE_DIRS}
  ${LibUV_INCLUDE_DIRS}
  ${LuaLUV_INCLUDE_DIRS})
target_link_libraries(tests PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::WebEngineWidgets
  Qt6::Test
  ${LuaJIT_LINK_LIBRARIES}
  ${LibUV_LINK_LIBRARIES}
  ${LuaLUV_LINK_LIBRARIES})
add_test(NAME MyTests COMMAND tests)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
