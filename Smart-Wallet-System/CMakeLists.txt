cmake_minimum_required(VERSION 3.20)

add_library(Smart-Wallet-System
	SHARED
	include/sws/Base.h
	include/sws/TCP.h
	include/sws/Message.h
	include/sws/Command.h
	include/sws/Client_Info.h
	include/sws/Transaction.h
	include/sws/Undo_Redo.h
	include/sws/Server.h
	include/sws/Client.h

	src/Base.cpp
	src/TCP.cpp
	src/Message.cpp
	src/Command.cpp
	src/Client_Info.cpp
	src/Transaction.cpp
	src/Undo_Redo.cpp
	src/Server.cpp
	src/Client.cpp
	)

target_include_directories(Smart-Wallet-System
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
	)

target_link_libraries(Smart-Wallet-System
	PUBLIC nlohmann::json fmt)

include(GenerateExportHeader)
generate_export_header(Smart-Wallet-System
	EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/sws/Exports.h")

