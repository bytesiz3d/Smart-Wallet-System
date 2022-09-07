cmake_minimum_required(VERSION 3.20)

add_library(json INTERFACE)
add_library(nlohmann::json ALIAS json)

target_include_directories(json
	INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_sources(json
	INTERFACE include/nlohmann/json.hpp)
