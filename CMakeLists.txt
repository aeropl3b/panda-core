project ( panda CXX )

cmake_minimum_required(VERSION 3.11)

option ( ENABLE_TESTS "Enable building and running tests" ON )

include_directories ( ${CMAKE_SOURCE_DIR}/include )

set( CMAKE_CXX_FLAGS_Debug          "-P -g -O0" )
set( CMAKE_CXX_FLAGS_Release        "-P -O3"    )
set( CMAKE_CXX_FLAGS_RelWithDebInfo "-P -g -O3" )
set( CMAKE_CXX_FLAGS_MinSizeRel     "-P -Os"    )

# set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -E")


if ( ${ENABLE_TESTS} )

   enable_testing()
   find_package ( GTest REQUIRED )
   find_package ( Boost REQUIRED )

   file ( GLOB_RECURSE TestSrc test/*.cpp )

   add_executable ( panda_test ${TestSrc} )
   target_link_libraries( panda_test GTest::GTest GTest::Main )
   target_include_directories( panda_test PUBLIC ${Boost_INCLUDE_DIRS} )
   set_property( TARGET panda_test PROPERTY CXX_STANDARD 17 )

   gtest_add_tests( TARGET panda_test )

endif ()

