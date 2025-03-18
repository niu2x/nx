cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(NX VERSION 0.1.2 LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(warning_as_error)
include(CMakePackageConfigHelpers)
# include(library)

set(LIB_NAME nx)
add_library(${LIB_NAME} STATIC "")

option(NX_STRICT "strict mode" OFF)
option(NX_BUILD_TEST "build test" OFF)

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
install(TARGETS ${LIB_NAME} EXPORT ${LIB_NAME}-targets DESTINATION lib/niu2x)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/nx DESTINATION include)
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


if(NX_BUILD_TEST)
    message(STATUS "build test")
    include(CTest)
    include(FetchContent)
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