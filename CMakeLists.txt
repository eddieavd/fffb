cmake_minimum_required( VERSION 3.23 )

project( fffb VERSION 0.1.0 )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set( CMAKE_CXX_STANDARD            23 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

set( CMAKE_OSX_ARCHITECTURES "x86_64" )

add_compile_options( -Wall -Wextra -pedantic -fno-exceptions -fno-rtti -O3 -DUTI_RELEASE -DFFFB_LOGS )

add_library( fffb SHARED source/fffb/fffb.cxx )

target_include_directories( fffb PUBLIC
                            ${PROJECT_SOURCE_DIR}/include
                            ${PROJECT_SOURCE_DIR}/deps
                            ${PROJECT_SOURCE_DIR}/deps/scs
                            ${PROJECT_SOURCE_DIR}/deps/scs/common
                            ${PROJECT_SOURCE_DIR}/deps/scs/amtrucks
                            ${PROJECT_SOURCE_DIR}/deps/scs/eurotrucks2
)
target_link_libraries( fffb "-framework CoreFoundation" )
target_link_libraries( fffb "-framework          IOKit" )
