cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)
project(nxs VERSION 0.1.0 LANGUAGES CXX C)

# use c++17 standard

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# pic
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# hide all default
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# custom cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


add_subdirectory(regex)
add_subdirectory(test)

include(warning_as_error)
# warning_as_error_enable(niu2x)

# add_subdirectory(test)

# install(TARGETS niu2x DESTINATION lib)
# install(DIRECTORY include/niu2x DESTINATION include)
# install(DIRECTORY build/niu2x/ DESTINATION include/niu2x)