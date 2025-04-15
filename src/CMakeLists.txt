target_sources(${LIB_NAME} PRIVATE
	file_system.cpp
	archive.cpp

	compress.cpp

	type.cpp

	md5.cpp
	sha256.cpp
	crc32.cpp
	digest.cpp

	log.cpp
	fmt_print.cpp
)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
else()
	target_compile_definitions(${LIB_NAME} PRIVATE USE_STD_FILE_SYSTEM)
endif()

target_sources(${LIB_NAME} PRIVATE
	url-parser/url.cpp
	url-parser/url.hpp
	url-parser/string.hpp
)