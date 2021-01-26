project ( xlib CXX )

cmake_minimum_required(VERSION 3.11)

option ( ENABLE_TESTS "Enable building and running tests" ON )

include_directories ( ${CMAKE_SOURCE_DIR}/include )

set( CMAKE_CXX_FLAGS_Debug          "-g -O0" )
set( CMAKE_CXX_FLAGS_Release        "-O3"    )
set( CMAKE_CXX_FLAGS_RelWithDebInfo "-g -O3" )
set( CMAKE_CXX_FLAGS_MinSizeRel     "-Os"    )


if ( ${ENABLE_TESTS} )

   enable_testing()
   
   find_package ( GTest REQUIRED )
   
   file ( GLOB_RECURSE TestSrc
     test/*.cpp
   )

   add_executable ( xlib_test ${TestSrc} )
   target_include_directories ( xlib_test PRIVATE ${GTest_INCLUDE_DIRS} )
   target_link_libraries( xlib_test ${GTEST_BOTH_LIBRARIES} )
   set_property(TARGET xlib_test PROPERTY CXX_STANDARD 17)
   
   gtest_add_tests( TARGET xlib_test
   )

endif ()

