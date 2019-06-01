cmake_minimum_required(VERSION 3.1)
project(rconsole LANGUAGES C CXX)
set(RC_LINK_LIBS "")


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/tn/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/tn)
include_directories(${TN_INCLUDE_DIRS})
list(APPEND RC_LINK_LIBS tn)


set(REPLXX_BuildExamples OFF CACHE BOOL "" FORCE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/replxx/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/replxx)
list(APPEND RC_LINK_LIBS replxx)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(rnet-plugin-native
	${CMAKE_CURRENT_SOURCE_DIR}/src/plugin.c
	${CMAKE_CURRENT_SOURCE_DIR}/include/plugin.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/util.c
	${CMAKE_CURRENT_SOURCE_DIR}/include/util.h
)
target_link_libraries(rnet-plugin-native ${RC_LINK_LIBS})
