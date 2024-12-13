cmake_minimum_required(VERSION 3.15)
project(Learn)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")

set(CMAKE_CXX_STANDARD 23)

set(BASIC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

include_directories(${INCLUDE_DIR})

file(GLOB BASIC_SOURCES "${BASIC_SOURCE_DIR}/*.cpp")

foreach(SOURCE_FILE ${BASIC_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
endforeach()
