
set (CMAKE_CXX_STANDARD 14)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

set (SOURCES
  LinearLayout.cc
  GridLayout.cc
  SelectorLayout.cc
  PictureWidget.cc
  LabelWidget.cc
  SelectorWidget.cc
  ComboBox.cc
  TabWidget.cc
  TextBox.cc
  IntValidator.cc
  FloatValidator.cc
  VirtualLayoutItem.cc
  ScrollArea.cc
  ScrollBar.cc
  ScrollableWidget.cc
  Button.cc
  ProgressBar.cc
  Gradient.cc
  )

add_library (sdl_graphic SHARED
  ${SOURCES}
  )

set(SDL_GRAPHIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)

target_link_libraries(sdl_graphic
  core_utils
  sdl_engine
  sdl_core
  )

target_include_directories (sdl_graphic PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
