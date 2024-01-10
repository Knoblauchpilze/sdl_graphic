
target_include_directories (sdl_graphic PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	)

target_sources (sdl_graphic PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/LinearLayout.cc
	${CMAKE_CURRENT_SOURCE_DIR}/GridLayout.cc
	${CMAKE_CURRENT_SOURCE_DIR}/SelectorLayout.cc
	${CMAKE_CURRENT_SOURCE_DIR}/PictureWidget.cc
	${CMAKE_CURRENT_SOURCE_DIR}/LabelWidget.cc
	${CMAKE_CURRENT_SOURCE_DIR}/SelectorWidget.cc
	${CMAKE_CURRENT_SOURCE_DIR}/ComboBox.cc
	${CMAKE_CURRENT_SOURCE_DIR}/TabWidget.cc
	${CMAKE_CURRENT_SOURCE_DIR}/TextBox.cc
	${CMAKE_CURRENT_SOURCE_DIR}/IntValidator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/FloatValidator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/VirtualLayoutItem.cc
	${CMAKE_CURRENT_SOURCE_DIR}/ScrollArea.cc
	${CMAKE_CURRENT_SOURCE_DIR}/ScrollBar.cc
	${CMAKE_CURRENT_SOURCE_DIR}/ScrollableWidget.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Button.cc
	${CMAKE_CURRENT_SOURCE_DIR}/ProgressBar.cc
	${CMAKE_CURRENT_SOURCE_DIR}/GradientWidget.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Checkbox.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Slider.cc
	)
