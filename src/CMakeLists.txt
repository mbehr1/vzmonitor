# -*- mode: cmake; -*-

list( APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror" )

include_directories(${MICROHTTPD_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file("${CMAKE_SOURCE_DIR}/src/gitSha1.cpp.in" "${CMAKE_BINARY_DIR}/gitSha1.cpp" @ONLY)

set(vzmonitor_srcs
  vzmonitor.cpp
  config_options.cpp config_options.hpp
  rules.cpp rules.hpp
  action.cpp action.hpp
  condition.cpp condition.hpp
  functions.cpp functions.hpp
  ${CMAKE_BINARY_DIR}/gitSha1.cpp
)

add_executable(vzmonitor ${vzmonitor_srcs})
target_link_libraries(vzmonitor ${JSON_LIBRARY} ${MICROHTTPD_LIBRARY} ${GNUTLS_LIBRARIES} pthread m dl)

INSTALL(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/vzmonitor
  DESTINATION bin)

