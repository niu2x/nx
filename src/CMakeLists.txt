target_sources(${LIB_NAME} PRIVATE
	file_system.cpp
	type.cpp
	md5.cpp
	crc32.cpp
	fmt_print.cpp
)

target_compile_definitions(${LIB_NAME} PRIVATE NX_USE_STD_FS)