cmake_minimum_required(VERSION 2.8.12)
set(PROJ_NAME client)
project(${PROJ_NAME} CXX)

set(CMAKE_BINARY_DIR "bin/${CMAKE_BUILD_TYPE}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

if(MSVC)
    if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -D_HAS_ITERATOR_DEBUGGING=0 -D_ITERATOR_DEBUG_LEVEL=0")
    endif()
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/third_party/cpp_helpers/include
    ${CMAKE_SOURCE_DIR}/third_party/zmq/include
    )

link_directories (
    ${CMAKE_SOURCE_DIR}/third_party/zmq/lib
)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    SET( PROJ_LIBRARIES "libzmqd" )
else()
    SET( PROJ_LIBRARIES "libzmq" )
endif(CMAKE_BUILD_TYPE EQUAL "DEBUG")

add_executable(${PROJ_NAME} main.cpp key_value_client.cpp)
target_link_libraries(${PROJ_NAME} ${PROJ_LIBRARIES})