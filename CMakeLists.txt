cmake_minimum_required(VERSION 2.8)
project(uvcppprj)

set(UVCPP_VER_MAJOR 1)
set(UVCPP_VER_MINOR 1)
set(UVCPP_VER_PATCH 1)
set(UVCPP_VER ${UVCPP_VER_MAJOR}.${UVCPP_VER_MINOR}.${UVCPP_VER_PATCH} )

set(CMAKE_CXX_STANDARD 11)
IF(UNIX)
    set(CMAKE_CXX_FLAGS "-pthread -std=c++0x ${CMAKE_CXX_FLAGS}")
ELSEIF(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF(UNIX)

set(DEP_LIB uv)
IF(WIN32)
set(DEP_LIB ${DEP_LIB} ws2_32)
ENDIF(WIN32)
IF(UNIX)
 set(DEP_LIB ${DEP_LIB} pthread)
ENDIF(UNIX)

file(GLOB UVCPPSRCS "uvcpp/*.cpp")
set(SOURCE_FILES ${UVCPPSRCS})
add_library(uvcpp SHARED ${SOURCE_FILES} uvcpp/uvcpp.cpp)
target_link_libraries(uvcpp ${DEP_LIB})
set_target_properties(uvcpp PROPERTIES SOVERSION ${UVCPP_VER_MAJOR}.${UVCPP_VER_MINOR} VERSION ${UVCPP_VER})

add_library(uvcpp_static STATIC ${SOURCE_FILES} uvcpp/uvcpp.cpp)
target_link_libraries(uvcpp_static ${DEP_LIB})

install(TARGETS uvcpp DESTINATION lib )
install(DIRECTORY ${PROJECT_SOURCE_DIR}/uvcpp/ DESTINATION include/uvcpp
        FILES_MATCHING PATTERN "*.h")

install(TARGETS uvcpp_static DESTINATION lib )

file(GLOB TESTSRCS "test/*.cpp")
add_executable(testuvcpp EXCLUDE_FROM_ALL ${SOURCE_FILES} ${TESTSRCS} test/test_basic.cpp test/test_ipc.cpp)
target_link_libraries(testuvcpp gtest ${DEP_LIB})

add_executable(chatsvr EXCLUDE_FROM_ALL ${UVCPPSRCS} "samples/chatserver.cpp" )
target_link_libraries(chatsvr ${DEP_LIB} )

add_executable(chatclient EXCLUDE_FROM_ALL ${UVCPPSRCS} "samples/chatclient.cpp" )
target_link_libraries(chatclient ${DEP_LIB} )