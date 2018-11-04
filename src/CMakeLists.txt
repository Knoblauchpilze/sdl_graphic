project (sdl_graphic)

set (CMAKE_CXX_STANDARD 14)

#set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package (SDL2 REQUIRED)
find_package (SDL2_ttf REQUIRED)

set (SOURCES
  Font.cc
  LinearLayout.cc
  GridLayout.cc
  )

add_library (sdl_graphic SHARED
  ${SOURCES}
  ${SDL2_INCLUDE_DIR}
  ${SDL2_TTF_INCLUDE_DIRS}
  )

set(SDL_GRAPHIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)

target_link_libraries(sdl_graphic
  ${SDL2_LIBRARY}
  ${SDL2_TTF_LIBRARIES}
  sdl_core
  )

target_include_directories (sdl_graphic PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL2_INCLUDE_DIR}
  ${SDL2_TTF_INCLUDE_DIRS}
  )
