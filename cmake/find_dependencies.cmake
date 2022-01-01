find_package(Boost 
	# COMPONENTS 
		# program_options 
		# log
	REQUIRED
)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
# set(OPENSSL_ROOT_DIR /usr/local/include)
# message(STATUS "CMAKE_SYSTEM_PREFIX_PATH ${CMAKE_SYSTEM_PREFIX_PATH}")
# find_package(OpenSSL  REQUIRED PATHS /usr/local/include/openssl )
# find_package(OpenSSL  REQUIRED )
# message(STATUS "OPENSSL_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR}")
# message(STATUS "OPENSSL_CRYPTO_LIBRARIES ${OPENSSL_CRYPTO_LIBRARIES}")


find_package(glfw3)
find_package(OpenGL)
find_package(GLEW)
