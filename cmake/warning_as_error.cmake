# enable warning_as_error in current dir and all subdirs.
function(warning_as_error_enable target)
	if (MSVC)
	    # warning level 4 and all warnings as errors
	    target_compile_options(${target} PRIVATE /W4 /WX)
	else()
	    # lots of warnings and all warnings as errors
	    # target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	    target_compile_options(${target} PRIVATE -Wall -Wextra -Werror)
	endif()
endfunction()

# disable warning_as_error in current dir and all subdirs.
# function(warning_as_error_disable)
# 	if (MSVC)
# 	else()
# 	    add_compile_options(-Wno-all -Wno-extra -Wno-pedantic -Wno-error)
# 	endif()
# endfunction()
