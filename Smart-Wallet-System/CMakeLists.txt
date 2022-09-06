cmake_minimum_required(VERSION 3.20)

add_library(Smart-Wallet-System
	SHARED
	include/sws/Base.h
	include/sws/TCP.h
	include/sws/Message.h
	include/sws/Server.h
	include/sws/Serializer.h
	include/sws/Command.h

	include/sws/Client_Info.h
	include/sws/Transaction.h

	src/TCP.cpp
	src/Base.cpp
	src/Message.cpp
	src/Command.cpp
	src/Client_Info.cpp
	src/Transaction.cpp
	)

target_include_directories(Smart-Wallet-System
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
	)

include(GenerateExportHeader)
generate_export_header(Smart-Wallet-System
	EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/sws/Exports.h")

