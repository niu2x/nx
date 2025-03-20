build-nx:
	cmake -S. -Bbuild/nx -DNX_STRICT=ON -DNX_BUILD_TEST=ON;
	cmake --build build/nx;

install-nx: build-nx
	cmake --install build/nx --prefix dist

test: build-nx
	cd build/nx && ctest --output-on-failure

cmake-library: install-nx
	nx_DIR=dist/lib/cmake/niu2x cmake -S tests/cmake-library -Bbuild/tests/cmake-library
	cmake --build build/tests/cmake-library
	

update-version:
	change-version -i -v $(ver) CMake ./CMakeLists.txt
	change-version -i -v $(ver) Doxygen ./Doxyfile