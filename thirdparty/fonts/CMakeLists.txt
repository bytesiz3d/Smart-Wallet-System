cmake_minimum_required(VERSION 3.20)

add_library(fonts INTERFACE)
target_sources(fonts INTERFACE fonts/DroidSans.h)
target_include_directories(fonts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
