cmake_minimum_required(VERSION 3.1)
project(rconsole LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/libuv/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/libuv)

SET(REPLXX_BuildExamples OFF CACHE BOOL "Build replxx examples")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/replxx/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/replxx)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(rnet-plugin-native
	${CMAKE_CURRENT_SOURCE_DIR}/src/plugin.c
	${CMAKE_CURRENT_SOURCE_DIR}/include/plugin.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/util.c
	${CMAKE_CURRENT_SOURCE_DIR}/include/util.h
)
target_link_libraries(rnet-plugin-native replxx uv_a)
