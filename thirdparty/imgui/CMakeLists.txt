cmake_minimum_required(VERSION 3.20)

add_library(imgui SHARED IMPORTED GLOBAL)
set_target_properties(imgui PROPERTIES
	IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libimgui.so")

target_include_directories(imgui 
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_sources(imgui
	INTERFACE
		include/imconfig.h
		include/imgui.h
		include/imgui_impl_opengl3.h
		include/imgui_impl_opengl3_loader.h
		include/imgui_impl_sdl.h
		include/imgui_internal.h
		include/imstb_rectpack.h
		include/imstb_textedit.h
		include/imstb_truetype.h
		include/implot.h
		include/implot_internal.h)