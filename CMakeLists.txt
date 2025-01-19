cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

project(NX VERSION 0.0.3 LANGUAGES CXX C)

set(LIB_NAME nx)

add_library(${LIB_NAME} STATIC "")

set_target_properties(${LIB_NAME} PROPERTIES CXX_STANDARD 17)

target_sources(${LIB_NAME} PRIVATE
	src/nx.cpp
)

target_include_directories(${LIB_NAME} PUBLIC include/)
target_include_directories(${LIB_NAME} PRIVATE src/)