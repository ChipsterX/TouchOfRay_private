cmake_minimum_required(VERSION 2.8)
project(xrLuabind)

option(LUABIND_BUILD_TESTING "Build luabind testing" OFF)
option(LUABIND_BUILD_SHARED "Build luabind as a shared library?" ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

if(NOT LUA_FOUND AND NOT LUA51_FOUND)
	find_package(Lua51 REQUIRED)
	set(LUA_INCLUDE_DIRS "${LUA_INCLUDE_DIR}")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(LUABIND_BUILD_SHARED)
	add_definitions(-DLUABIND_DYNAMIC_LINK)
endif()

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	${LUA_INCLUDE_DIR}
)

add_subdirectory(src)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	if(LUABIND_BUILD_TESTING)
		add_subdirectory(test)
	endif()
	add_subdirectory(doc)
endif()
