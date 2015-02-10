list( APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror" )

include_directories(
    ${GMOCK_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# all *.cpp files here will be used.
set(test_sources main.cpp
  ut_rules.cpp
  ../src/rules.cpp
  ../src/action.cpp
  ../src/condition.cpp
  ../src/functions.cpp
  ../src/config_options.cpp
)

add_executable(vzmonitor_unit_tests ${test_sources})

target_link_libraries(vzmonitor_unit_tests
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GMOCK_LIBS_DIR}/libgmock.a
    ${JSON_LIBRARY}
    dl
    pthread)

add_subdirectory(mocks)
