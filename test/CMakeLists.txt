
enable_testing()

find_package ( GTest REQUIRED )

file ( GLOB_RECURSE TestSrc *.cpp )

add_executable ( panda_test ${TestSrc} )
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories ( panda_test PRIVATE ${GTest_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_link_libraries( panda_test ${GTEST_BOTH_LIBRARIES} )
set_property(TARGET panda_test PROPERTY CXX_STANDARD 17)

gtest_add_tests( TARGET panda_test )
