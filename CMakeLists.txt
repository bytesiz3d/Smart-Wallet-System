cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Smart-Wallet-System LANGUAGES CXX C)

# Thirdparty
add_subdirectory(thirdparty/fonts)
add_subdirectory(thirdparty/fmt)
add_subdirectory(thirdparty/nlohmann-json)
find_package(OpenGL REQUIRED)
add_subdirectory(thirdparty/SDL2)
add_subdirectory(thirdparty/imgui)

# Source
add_subdirectory(Smart-Wallet-System)
add_subdirectory(GUI)
add_subdirectory(Server-App)

# Playground
add_subdirectory(playground)
