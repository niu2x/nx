build-nx-static:
	cmake -S. -Bbuild/nx/static -DNX_STRICT=ON -DNX_BUILD_TEST=ON -DNX_STATIC=ON;
	cmake --build build/nx/static;

build-nx-shared:
	cmake -S. -Bbuild/nx/shared -DNX_STRICT=ON -DNX_BUILD_TEST=ON -DNX_STATIC=OFF;
	cmake --build build/nx/shared;

install-nx-static: build-nx-static
	cmake --install build/nx/static --prefix dist/static

install-nx-shared: build-nx-shared
	cmake --install build/nx/shared --prefix dist/shared

test: build-nx-shared build-nx-static
	cd build/nx/static && ctest --output-on-failure
	cd build/nx/shared && ctest --output-on-failure

cmake-library-static: install-nx-static
	nx_DIR=dist/lib/cmake/niu2x cmake -S tests/cmake-library -Bbuild/tests/cmake-library/static
	cmake --build build/tests/cmake-library/static

cmake-library-shared: install-nx-shared
	nx_DIR=dist/lib/cmake/niu2x cmake -S tests/cmake-library -Bbuild/tests/cmake-library/shared
	cmake --build build/tests/cmake-library/shared

cmake-library: cmake-library-shared cmake-library-static

update-version:
	change-version -i -v $(ver) CMake ./CMakeLists.txt
	change-version -i -v $(ver) Doxygen ./Doxyfile