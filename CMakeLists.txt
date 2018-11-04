cmake_minimum_required (VERSION 3.7)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_VERBOSE_MAKEFILE ON)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

project(sdl_graphic_library_executable)

add_subdirectory(src)

set (SOURCES
  main.cpp
  )

add_executable(sdl_graphic_library_executable
  ${SOURCES}
  )

target_include_directories (sdl_graphic_library_executable PRIVATE
  )

target_link_libraries(sdl_graphic_library_executable
  sdl_graphic_library
  )
