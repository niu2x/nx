cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project ( NX VERSION 0.1.6 LANGUAGES CXX C )      

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(warning_as_error)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(FetchContent)

set(LIB_NAME nx)

option(NX_STRICT "strict mode" OFF)
option(NX_BUILD_TEST "build test" OFF)
option(NX_BUILD_ZLIB "build zlib" ON)
option(NX_BUILD_LIBZIP "build libzip" ON)
option(NX_STATIC "build static library" OFF)

if(NX_STATIC)
    add_library(${LIB_NAME} STATIC "")
    target_compile_definitions(${LIB_NAME} PUBLIC NX_STATIC_DEFINE)
else()
    add_library(${LIB_NAME} SHARED "")
    target_compile_definitions(${LIB_NAME} PRIVATE nx_EXPORTS)
endif()

generate_export_header(${LIB_NAME}
    EXPORT_MACRO_NAME NX_API
    EXPORT_FILE_NAME include/nx/api.h
)

if(NX_STRICT)
    warning_as_error_enable(${LIB_NAME})
endif()

set_target_properties(${LIB_NAME} PROPERTIES 
	CXX_STANDARD 17
	C_VISIBILITY_PRESET hidden
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
	POSITION_INDEPENDENT_CODE ON
)

add_subdirectory(src)

target_include_directories(${LIB_NAME} PUBLIC
    $<INSTALL_INTERFACE:include> 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  
)

target_include_directories(${LIB_NAME} PRIVATE src/)

target_link_directories(${LIB_NAME} PUBLIC
    $<INSTALL_INTERFACE:lib> 
)

# 

if(NOT NX_STATIC)
    install(TARGETS ${LIB_NAME} EXPORT ${LIB_NAME}-targets DESTINATION lib/niu2x)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/nx DESTINATION include)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/nx DESTINATION include)
    install(EXPORT ${LIB_NAME}-targets
        FILE ${LIB_NAME}-targets.cmake
        DESTINATION lib/cmake/niu2x/
        # NAMESPACE niu2x::
    )

    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_install_config.in
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-config.cmake"
        INSTALL_DESTINATION "lib/cmake/niu2x"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-config-version.cmake"
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}-config-version.cmake
        DESTINATION lib/cmake/niu2x
    )
endif()



if(NX_BUILD_ZLIB)
    function(configure_zlib)
        message(STATUS "build zlib")
        set(SKIP_INSTALL_ALL ON)
        add_subdirectory(zlib)
        target_compile_definitions(${LIB_NAME} PRIVATE USE_ZLIB)
        target_link_libraries(${LIB_NAME} PRIVATE zlibstatic)
    endfunction()
    configure_zlib()
endif()

if(NX_BUILD_LIBZIP AND NX_BUILD_ZLIB)
    message(STATUS "build libzip")
    
    # FetchContent_Declare(
    #     zlib
    #     GIT_REPOSITORY https://github.com/madler/zlib.git
    #     GIT_TAG v1.3.1
    # )
    # FetchContent_MakeAvailable(zlib)
    # target_compile_definitions(${LIB_NAME} PRIVATE USE_ZLIB)
    function(configure_libzip)
        message(STATUS "build libzip")
        option(BUILD_DOC "" OFF)
        option(BUILD_EXAMPLES "" OFF)
        option(BUILD_OSSFUZZ "" OFF)
        option(BUILD_REGRESS "" OFF)
        option(BUILD_TOOLS "" OFF)
        option(ENABLE_FDOPEN "" OFF)
        option(ENABLE_ZSTD "" OFF)
        option(ENABLE_LZMA "" OFF)
        option(ENABLE_BZIP2 "" OFF)
        option(ENABLE_WINDOWS_CRYPTO "" OFF)
        option(ENABLE_OPENSSL "" OFF)
        option(ENABLE_MBEDTLS "" OFF)
        option(ENABLE_COMMONCRYPTO "" OFF)
        option(ENABLE_GNUTLS "" OFF)

        add_subdirectory(libzip)
        target_compile_definitions(${LIB_NAME} PRIVATE USE_LIBZIP)
        target_link_libraries(${LIB_NAME} PRIVATE libzip::zip)
    endfunction()
    configure_libzip()
endif()


if(NX_BUILD_TEST)
    message(STATUS "build test")
    include(CTest)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(unittest tests/test.cpp)
    target_link_libraries(unittest PRIVATE gtest_main ${LIB_NAME})

    set_target_properties(unittest PROPERTIES 
        CXX_STANDARD 17
    )
    add_test(NAME UnitTest COMMAND unittest)

endif()

