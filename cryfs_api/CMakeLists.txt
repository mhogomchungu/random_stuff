
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

project(cryfs_api)

set( LIB_VERSION "1.0.0" )

add_definitions( -Wextra -Wall -pedantic -std=c99 )
add_definitions( -D_DEFAULT_SOURCE -fstack-protector-all -D_FORTIFY_SOURCE=2 --param ssp-buffer-size=4 )
include_directories( ${PROJECT_BINARY_DIR} )

if( LIB_SUFFIX )
	set ( LIB_PATH "${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX}" )
else( LIB_SUFFIX )
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set ( LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib64" )
	else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set ( LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib" )
	endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
endif( LIB_SUFFIX )

include_directories( ${PROJECT_SOURCE_DIR}/src/string )
include_directories( ${PROJECT_SOURCE_DIR}/src/process )

SET( LIB
src/string/String.c
src/string/StringList.c
src/process/process.c
src/cryfs_api.c
)

add_library( cryfs_api SHARED ${LIB} )
set_target_properties( cryfs_api PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pthread  -pedantic " )
set_target_properties( cryfs_api PROPERTIES SOVERSION ${LIB_VERSION} )
target_link_libraries( cryfs_api pthread )

add_executable( test_api src/test_api.c  )
set_target_properties( test_api PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )

target_link_libraries( test_api cryfs_api )

file( WRITE ${PROJECT_BINARY_DIR}/cryfs_api.pc
"prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${LIB_PATH}
includedir=${CMAKE_INSTALL_PREFIX}/include

Name: cryfs_api
Description: a shared library offering cryfs functionality
Version: ${LIB_VERSION}
Libs:
Cflags: -I${CMAKE_INSTALL_PREFIX}/include
\n")

install(FILES ${PROJECT_BINARY_DIR}/cryfs_api.pc DESTINATION ${LIB_PATH}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ )

install(TARGETS cryfs_api LIBRARY DESTINATION ${LIB_PATH} )

install(FILES ${PROJECT_SOURCE_DIR}/src/cryfs_api.h DESTINATION include/cryfs_api )

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

