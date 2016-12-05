cmake_minimum_required(VERSION 3.5)
project(uvcppprj)

set(UVCPP_VER_MAJOR 1)
set(UVCPP_VER_MINOR 1)
set(UVCPP_VER_PATCH 1)
set(UVCPP_VER ${UVCPP_VER_MAJOR}.${UVCPP_VER_MINOR}.${UVCPP_VER_PATCH} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

file(GLOB UVCPPSRCS "uvcpp/*.cpp")
set(SOURCE_FILES ${UVCPPSRCS} test/testmain.cpp)

file(GLOB TESTSRCS "test/*.cpp")

add_library(uvcpp SHARED ${SOURCE_FILES} uvcpp/uvcpp.cpp)
target_link_libraries(uvcpp uv nmdutil)
set_target_properties(uvcpp PROPERTIES SOVERSION ${UVCPP_VER_MAJOR}.${UVCPP_VER_MINOR} VERSION ${UVCPP_VER})
install(TARGETS uvcpp DESTINATION lib )
install(DIRECTORY ${PROJECT_SOURCE_DIR}/uvcpp/ DESTINATION include/uvcpp
        FILES_MATCHING PATTERN "*.h")

#add_library(uvcpp_static STATIC ${SOURCE_FILES})

add_executable(testuvcpp ${SOURCE_FILES} ${TESTSRCS} test/test_basic.cpp test/test_ipc.cpp)
target_link_libraries(testuvcpp pthread uv gtest)