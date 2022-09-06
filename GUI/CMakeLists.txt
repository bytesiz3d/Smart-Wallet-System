cmake_minimum_required(VERSION 3.20)

add_library(GUI
	SHARED
		include/gui/Window.h
		src/Window.cpp
)

target_include_directories(GUI
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(GUI
	PRIVATE
		OpenGL::GL
		SDL2
		fonts
	PUBLIC
		imgui
)

include(GenerateExportHeader)
generate_export_header(GUI
	EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/gui/Exports.h")