cmake_minimum_required(VERSION 3.25)
set(PROJECT "null-browser")

project(${PROJECT}
  VERSION 0.0.0
  LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(AUTOMOC ON)
set(PKG_CONFIG_USE_STATIC_LIBS ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
find_package(PkgConfig REQUIRED)

# message(COMMENT "${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}")

# Lua path
if(NOT DEFINED NULL_LUA_PREFIX OR NOT NULL_LUA_PREFIX)
  set(NULL_LUA_PREFIX "${CMAKE_SOURCE_DIR}/lua")
endif()
set(PROJECT_LUA_PATH "${NULL_LUA_PREFIX}/?.lua")
add_compile_definitions(PROJECT_LUA_PATH="${PROJECT_LUA_PATH}")

# Source
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
list(FILTER SOURCE_FILES EXCLUDE REGEX "src/main.cpp$")
file(GLOB_RECURSE HEADER_FILES src/*.hpp)
file(GLOB_RECURSE SPEC_FILES spec/*.cpp)

# Project
add_executable(${PROJECT})
target_sources(${PROJECT} PRIVATE src/main.cpp ${SOURCE_FILES})
target_include_directories(${PROJECT} PRIVATE src/)


# Env config
if(DEFINED ENV{RELEASE})
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS}")
elseif(DEFINED ENV{DEBUG})
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
else()
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif()


# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebEngineWidgets Test)
qt_standard_project_setup()
qt_wrap_cpp(MOC_SRCS ${HEADER_FILES})
target_sources(${PROJECT} PRIVATE ${MOC_SRCS})
target_link_libraries(${PROJECT}
  Qt6::Core
  Qt6::Widgets
  Qt6::WebEngineWidgets)

# Lua.LUV
pkg_check_modules(LuaLUV REQUIRED libluv)
target_include_directories(${PROJECT} PRIVATE ${LuaLUV_INCLUDE_DIRS})
target_link_libraries(${PROJECT} ${LuaLUV_LINK_LIBRARIES})

# LuaJIT
pkg_check_modules(LuaJIT REQUIRED luajit)
target_include_directories(${PROJECT} PRIVATE ${LuaJIT_INCLUDE_DIRS})
target_link_libraries(${PROJECT} ${LuaJIT_LINK_LIBRARIES})

# LibUV
pkg_check_modules(LibUV REQUIRED libuv)
target_include_directories(${PROJECT} PRIVATE ${LibUV_INCLUDE_DIRS})
target_link_libraries(${PROJECT} ${LibUV_LINK_LIBRARIES})


# Testing
enable_testing()
add_executable(tests)
target_sources(tests PRIVATE ${SOURCE_FILES} ${MOC_SRCS} ${SPEC_FILES})
target_include_directories(tests PRIVATE
  src/
  spec/
  ${LuaLUV_INCLUDE_DIRS}
  ${LuaJIT_INCLUDE_DIRS}
  ${LibUV_INCLUDE_DIRS})
target_link_libraries(tests PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::WebEngineWidgets
  Qt6::Test
  ${LuaLUV_LINK_LIBRARIES}
  ${LuaJIT_LINK_LIBRARIES}
  ${LibUV_LINK_LIBRARIES})
add_test(NAME NullBrowserTests COMMAND tests)


# Install
install(TARGETS ${PROJECT}
  EXPORT               "${TARGETS_EXPORT_NAME}"
  RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY lua/ DESTINATION lua)
# file(COPY "${CMAKE_SOURCE_DIR}/lua" DESTINATION "${CMAKE_INSTALL_PREFIX}")

