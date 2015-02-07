include_directories(BEFORE .)

add_executable(mock_test1 mock_test1.cpp
)

target_link_libraries(mock_test1 
		${GTEST_LIBS_DIR}/libgtest.a
		${GMOCK_LIBS_DIR}/libgmock.a
		pthread
		${JSON_LIBRARY}
		dl
	  )

add_test(mock_test1 mock_test1)
