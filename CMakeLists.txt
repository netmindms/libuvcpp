cmake_minimum_required(VERSION 3.5)
project(uvcppprj)

set(CMAKE_CXX_STANDARD 11)

file(GLOB UVCPPSRCS "uvcpp/*.cpp")
set(SOURCE_FILES ${UVCPPSRCS} test/testmain.cpp)

file(GLOB TESTSRCS "test/*.cpp")

add_library(uvcpp SHARED ${SOURCE_FILES} uvcpp/UvUdpHandle.cpp uvcpp/UvUdpHandle.h uvcpp/UvUdpWriteInfo.cpp uvcpp/UvUdpWriteInfo.h uvcpp/UvUdp.cpp uvcpp/UvUdp.h)
target_link_libraries(uvcpp uv nmdutil)

add_executable(testuvcpp ${SOURCE_FILES} ${TESTSRCS} test/test_basic.cpp)
target_link_libraries(testuvcpp pthread uv nmdutil gtest)