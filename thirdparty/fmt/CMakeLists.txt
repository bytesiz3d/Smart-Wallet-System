cmake_minimum_required(VERSION 3.20)

add_library(fmt SHARED IMPORTED GLOBAL)
set_target_properties(fmt PROPERTIES
	IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libfmt.so")

target_include_directories(fmt
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_sources(fmt
	INTERFACE
	include/fmt/args.h
	include/fmt/chrono.h
	include/fmt/color.h
	include/fmt/compile.h
	include/fmt/core.h
	include/fmt/format-inl.h
	include/fmt/format.h
	include/fmt/os.h
	include/fmt/ostream.h
	include/fmt/printf.h
	include/fmt/ranges.h
	include/fmt/std.h
	include/fmt/xchar.h)
