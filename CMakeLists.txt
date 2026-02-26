cmake_minimum_required(VERSION 3.10)
project(EventStream)

set(CMAKE_CXX_STANDARD 17)

add_executable(eventstream main.cpp)
add_executable(eventstream_client test_client.cpp)

if (WIN32)
	target_link_libraries(eventstream ws2_32)
	target_link_libraries(eventstream_client ws2_32)
endif()
