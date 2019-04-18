cmake_minimum_required(VERSION 2.8)

project(tcp-chat-server)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
