cmake_minimum_required(VERSION 3.20)

add_executable(Server-App
	include/server/App.h

	src/App.cpp
	src/main.cpp
	)

target_link_libraries(Server-App GUI Smart-Wallet-System)
target_include_directories(Server-App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)