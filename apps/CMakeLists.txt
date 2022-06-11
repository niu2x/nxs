add_executable(lexer "")

target_sources(lexer PRIVATE
	lexer.cpp
)

target_include_directories(lexer PUBLIC	
	${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(lexer PUBLIC 
	nxs
)