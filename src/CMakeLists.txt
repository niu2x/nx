target_sources(${LIB_NAME} PRIVATE
	file_system.cpp
	type.cpp
	md5.cpp
	crc32.cpp
	fmt_print.cpp
)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	target_compile_definitions(${LIB_NAME} PRIVATE USE_STD_FILE_SYSTEM)
endif()		