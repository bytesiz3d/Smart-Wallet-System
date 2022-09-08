cmake_minimum_required(VERSION 3.20)

add_executable(Client-App
	include/client/App.h

	src/App.cpp
	src/main.cpp
	)

target_link_libraries(Client-App GUI Smart-Wallet-System)
target_include_directories(Client-App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
