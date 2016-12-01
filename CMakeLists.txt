cmake_minimum_required(VERSION 3.5)
project(uvcppprj)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

file(GLOB UVCPPSRCS "uvcpp/*.cpp")
set(SOURCE_FILES ${UVCPPSRCS} test/testmain.cpp)

file(GLOB TESTSRCS "test/*.cpp")

add_library(uvcpp SHARED ${SOURCE_FILES} uvcpp/UvUdpWriteInfo.cpp uvcpp/UvUdpWriteInfo.h uvcpp/UvUdp.cpp uvcpp/UvUdp.h uvcpp/UvCheck.cpp uvcpp/UvCheck.h uvcpp/UvHandle.cpp uvcpp/UvPrepare.cpp uvcpp/UvPrepare.h uvcpp/UvAsync.cpp uvcpp/UvAsync.h uvcpp/UvHandleOwner.cpp uvcpp/UvHandleOwner.h uvcpp/UvIdle.cpp uvcpp/UvIdle.h uvcpp/IpcMsg.cpp uvcpp/IpcMsg.h uvcpp/UvObj.cpp uvcpp/UvObj.h uvcpp/Ipc.cpp uvcpp/Ipc.h)
target_link_libraries(uvcpp uv nmdutil)

add_executable(testuvcpp ${SOURCE_FILES} ${TESTSRCS} test/test_basic.cpp test/test_ipc.cpp)
target_link_libraries(testuvcpp pthread uv nmdutil gtest)