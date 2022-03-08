cmake_minimum_required(VERSION 3.11)

project(tests_time_index)

file(GLOB TEST_HEADER_FILES *.h *.hpp)
file(GLOB TEST_SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})

add_definitions(-DBUILDING_DLL)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  GTest::gmock
  GTest::gmock_main
  time_index
)

add_test(NAME time_index COMMAND ${PROJECT_NAME})

