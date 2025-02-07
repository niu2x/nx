build-nx:
	cmake -S. -Bbuild/nx -DNX_STRICT=ON;
	cmake --build build/nx;

install-nx: build-nx
	cmake --install build/nx --prefix dist

test-cmake-library: install-nx
	nx_DIR=dist/lib/cmake/niu2x cmake -S tests/cmake-library -Bbuild/tests/cmake-library
	cmake --build build/tests/cmake-library


