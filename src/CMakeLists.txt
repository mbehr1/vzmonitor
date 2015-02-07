# -*- mode: cmake; -*-

include_directories(${MICROHTTPD_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file("${CMAKE_SOURCE_DIR}/src/gitSha1.cpp.in" "${CMAKE_BINARY_DIR}/gitSha1.cpp" @ONLY)

set(vzmonitor_srcs
  vzmonitor.cpp
  ${CMAKE_BINARY_DIR}/gitSha1.cpp
)

add_executable(vzmonitor ${vzmonitor_srcs})
target_link_libraries(vzmonitor ${JSON_LIBRARY} ${MICROHTTPD_LIBRARY} pthread m dl)

INSTALL(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/vzmonitor
  DESTINATION bin)

