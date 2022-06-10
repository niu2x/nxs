add_library(nxs "")

target_sources(nxs PRIVATE
	lexer.cpp
)

target_include_directories(nxs PUBLIC	
	${CMAKE_SOURCE_DIR}/include
)