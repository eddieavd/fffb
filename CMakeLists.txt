cmake_minimum_required( VERSION 3.21 )

project( fffb VERSION 0.0.3 )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set( CMAKE_CXX_STANDARD            23 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

set( CMAKE_OSX_ARCHITECTURES "x86_64" )

add_compile_options( -Wall -Wextra -pedantic -Werror -fno-exceptions -fno-rtti -O3 -DUTI_RELEASE -DFFFB_LOGS )

add_library( fffb_scs SHARED source/fffb/scs/fffb_scs.cpp )

target_include_directories( fffb_scs PUBLIC
                            include
                            include/uti
                            include/fffb
                            include/scs/include
                            include/scs/include/common
                            include/scs/include/amtrucks
                            include/scs/include/eurotrucks2
)
target_link_libraries( fffb_scs "-framework CoreFoundation" )
target_link_libraries( fffb_scs "-framework          IOKit" )

add_executable( flt source/flt.cpp )

target_include_directories( flt PUBLIC
                            include
                            include/uti
                            include/fffb
                            include/scs/include
                            include/scs/include/common
                            include/scs/include/amtrucks
                            include/scs/include/eurotrucks2
)
target_link_libraries( flt "-framework CoreFoundation" )
target_link_libraries( flt "-framework          IOKit" )
