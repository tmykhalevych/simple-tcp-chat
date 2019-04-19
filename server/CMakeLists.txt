cmake_minimum_required(VERSION 2.6)

project(tcp-chat-server)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

# Find Boost and include dependencies
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS system thread regex)
if(Boost_FOUND)
    include_directories($ENV{BOOST_ROOT}/include)#${Boost_INCLUDE_DIRS}) # FIXME: Does not work. Find out why!
    link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

# Find Protobuf
find_package(Protobuf REQUIRED)

# Add hesders
set(_HEADER_DIR_ include)
set(_INFRA_HEADER_DIR_ ../infra/include)
include_directories(${_HEADER_DIR_})
include_directories(${_INFRA_HEADER_DIR_})

# Add sources
set(_SOURCE_FILES_
    ../infra/source/comm.pb.cc
    main.cpp
    source/server.cpp
    source/connection.cpp
)
add_executable(${CMAKE_PROJECT_NAME} ${_SOURCE_FILES_})

# Add aditional libs
SET(_BOOST_LIBS_
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_REGEX_LIBRARY}
)
target_link_libraries(${CMAKE_PROJECT_NAME} ${_BOOST_LIBS_} ${Protobuf_LIBRARIES})

# Produce binarie
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
