add_executable(test_dfa "test_dfa.cpp")
target_link_libraries(test_dfa nxs_regex)

add_executable(test_regex "test_regex.cpp")
target_link_libraries(test_regex nxs_regex)

