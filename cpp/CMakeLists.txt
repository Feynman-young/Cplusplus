cmake_minimum_required(VERSION 3.15)
project(Learn)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wno-non-pod-varargs)

set(BASIC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/basic")
set(LIBRARY_SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/library"
    "${CMAKE_SOURCE_DIR}/library/dynamic_programming"
)
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIBS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/lib")

include_directories(${INCLUDE_DIR})

file(GLOB BASIC_SOURCES "${BASIC_SOURCE_DIR}/*.cpp")

set(LIBRARY_SOURCES "")
foreach(dir ${LIBRARY_SOURCE_DIR})
    file(GLOB tmp "${dir}/*.cpp")
    list(APPEND LIBRARY_SOURCES ${tmp})
endforeach()

file(GLOB LIBS_SOURCES "${LIBS_SOURCE_DIR}/*.cpp")
add_library(Lib STATIC ${LIBS_SOURCES})

foreach(SOURCE_FILE ${BASIC_SOURCES})
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)
    get_filename_component(PARENT_DIR ${SOURCE_FILE} DIRECTORY)
    get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
    set(EXECUTABLE_NAME "${PARENT_NAME}_${EXEC_NAME}")
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
    target_link_libraries(${EXECUTABLE_NAME} Lib)
endforeach()

foreach(SOURCE_FILE ${LIBRARY_SOURCES})
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)
    get_filename_component(PARENT_DIR ${SOURCE_FILE} DIRECTORY)
    get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
    set(EXECUTABLE_NAME "${PARENT_NAME}_${EXEC_NAME}")
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
    target_link_libraries(${EXECUTABLE_NAME} Lib)
endforeach()
