cmake_minimum_required(VERSION 3.20)
add_executable(playground main.cpp)

target_link_libraries(playground PRIVATE 
	OpenGL::GL 
	SDL2
	imgui
	fonts
)