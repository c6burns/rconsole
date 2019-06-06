cmake_minimum_required(VERSION 3.1)
project(rconsole LANGUAGES C CXX)

set(RC_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(RC_LINK_LIBS "")

if(NOT EXISTS ${RC_DEPS_DIR})
	file(MAKE_DIRECTORY ${RC_DEPS_DIR})
endif()

#
# build replxx
if(NOT EXISTS ${RC_DEPS_DIR}/replxx)
	execute_process(COMMAND git clone https://github.com/c6burns/replxx WORKING_DIRECTORY ${RC_DEPS_DIR})
endif()

add_custom_target(replxx_update
	COMMAND git checkout master
	COMMAND git pull
	WORKING_DIRECTORY ${RC_DEPS_DIR}/replxx
)

set(REPLXX_BuildExamples OFF CACHE BOOL "" FORCE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/replxx/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/replxx)
add_dependencies(replxx replxx_update)
list(APPEND RC_LINK_LIBS replxx)


#
# build tn
if(NOT EXISTS ${RC_DEPS_DIR}/tn)
	execute_process(COMMAND git clone https://github.com/c6burns/tn WORKING_DIRECTORY ${RC_DEPS_DIR})
endif()

add_custom_target(tn_update
	COMMAND git checkout master
	COMMAND git pull
	WORKING_DIRECTORY ${RC_DEPS_DIR}/tn
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/tn/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/tn)
include_directories(${TN_INCLUDE_DIRS})
add_dependencies(tn tn_update)
list(APPEND RC_LINK_LIBS tn)


#
# build rconsole
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(rconsole
	src/plugin.c
	include/plugin.h
	src/buf.c
	include/rc/buf.h
	src/util.c
	include/util.h
)
target_link_libraries(rconsole ${RC_LINK_LIBS})
add_dependencies(rconsole tn replxx)

add_executable(playground src/playground.c)
target_link_libraries(playground ${RC_LINK_LIBS})