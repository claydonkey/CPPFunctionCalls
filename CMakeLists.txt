cmake_minimum_required(VERSION 2.8.9)
project (PointerToMember)
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -save-temps")
add_executable(PointerToMember main.cpp)

add_custom_command(TARGET ${PROJECT_NAME}
	POST_BUILD
    COMMAND ${CMAKE_CXX_COMPILER} -g -O3 -std=c++11 -Wa,-aslh ${CMAKE_SOURCE_DIR}/main.cpp > list.txt
	VERBATIM # optional
)