project ( xlib CXX )

cmake_minimum_required(VERSION 3.11)
  
find_package ( Boost REQUIRED )

option ( PANDA_ENABLE_TESTS "Enable building and running tests" ON )

include_directories ( ${CMAKE_SOURCE_DIR}/include )

set( CMAKE_CXX_FLAGS_Debug          "-g -O0" )
set( CMAKE_CXX_FLAGS_Release        "-O3"    )
set( CMAKE_CXX_FLAGS_RelWithDebInfo "-g -O3" )
set( CMAKE_CXX_FLAGS_MinSizeRel     "-Os"    )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif()
message( STATUS "Build mode: " ${CMAKE_BUILD_TYPE} )


if ( ${PANDA_ENABLE_TESTS} )
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif ()

