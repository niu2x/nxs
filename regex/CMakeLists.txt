add_library(nxs_regex "")
target_sources(nxs_regex PRIVATE 
	"dfa.cpp"
	"nfa.cpp"
)
target_include_directories(nxs_regex PUBLIC 
	${CMAKE_SOURCE_DIR}/include
)