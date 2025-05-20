default: build-wasm test cmake-library

EMSDK := /opt/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

build-wasm:
	cmake -S. -DCMAKE_BUILD_TYPE=Debug -Bbuild/nx/wasm -DNX_STRICT=ON -DNX_BUILD_TEST=OFF -DNX_STATIC=ON -DCMAKE_TOOLCHAIN_FILE=$(EMSDK)
	cmake --build build/nx/wasm

build-nx-static:
	cmake -S. -DCMAKE_BUILD_TYPE=Debug -Bbuild/nx/static -DNX_STRICT=ON -DNX_BUILD_TEST=ON -DNX_STATIC=ON -DNX_BUILD_ZLIB=ON -DNX_BUILD_LIBZIP=OFF -Dlibzip_DIR=/home/niu2x/project/libzip/build/dist/lib/cmake/libzip;
	cmake --build build/nx/static;

build-nx-shared:
	cmake -S. -DCMAKE_BUILD_TYPE=Debug -Bbuild/nx/shared -DNX_STRICT=ON -DNX_BUILD_TEST=ON -DNX_STATIC=OFF -DNX_BUILD_ZLIB=ON -DNX_BUILD_LIBZIP=OFF -Dlibzip_DIR=/home/niu2x/project/libzip/build/dist/lib/cmake/libzip;
	cmake --build build/nx/shared;

install-nx-static: build-nx-static
	cmake --install build/nx/static --prefix dist/static

install-nx-shared: build-nx-shared
	cmake --install build/nx/shared --prefix dist/shared

test: build-nx-shared build-nx-static
	cd build/nx/static && ctest --output-on-failure
	cd build/nx/shared && ctest --output-on-failure

cmake-library-static: install-nx-static
	nx_DIR=dist/static/lib/cmake/niu2x cmake -S tests/cmake-library -DCMAKE_BUILD_TYPE=Debug -Bbuild/tests/cmake-library/static -Dlibzip_DIR=/home/niu2x/project/libzip/build/dist/lib/cmake/libzip;
	cmake --build build/tests/cmake-library/static

cmake-library-shared: install-nx-shared
	nx_DIR=dist/shared/lib/cmake/niu2x cmake -S tests/cmake-library -DCMAKE_BUILD_TYPE=Debug -Bbuild/tests/cmake-library/shared -Dlibzip_DIR=/home/niu2x/project/libzip/build/dist/lib/cmake/libzip;
	cmake --build build/tests/cmake-library/shared

cmake-library: cmake-library-shared cmake-library-static

update-version:
	change-version -i -v $(ver) CMake ./CMakeLists.txt
	change-version -i -v $(ver) Doxygen ./Doxyfile