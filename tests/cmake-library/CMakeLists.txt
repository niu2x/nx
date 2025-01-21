cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)
project(TestLibrary VERSION 0.1.0 LANGUAGES CXX C)

find_package(nx REQUIRED)

add_executable(test-library test.cpp)
set_target_properties(test-library PROPERTIES 
	CXX_STANDARD 17
)
target_link_libraries(test-library PRIVATE nx)
