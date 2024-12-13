cmake_minimum_required(VERSION 3.15)
project(Learn)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")

set(CMAKE_CXX_STANDARD 23)

set(BASIC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/basic")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIBS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/lib")


include_directories(${INCLUDE_DIR})

file(GLOB BASIC_SOURCES "${BASIC_SOURCE_DIR}/*.cpp")
file(GLOB LIBS_SOURCES "${LIBS_SOURCE_DIR}/*.cpp")

add_library(Lib STATIC ${LIBS_SOURCES})

foreach(SOURCE_FILE ${BASIC_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
    target_link_libraries(${EXECUTABLE_NAME} Lib)
endforeach()
