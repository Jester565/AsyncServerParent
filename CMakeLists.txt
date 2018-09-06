cmake_minimum_required ( VERSION 3.1 )
set (PROJECT_NAME AsyncServerParent)

project(${PROJECT_NAME})

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_GENERATOR_PLATFORM x64)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.66.0 COMPONENTS thread REQUIRED)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/asyncserverparent)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADERS
	${HEADER_DIR}/Client.h
	${HEADER_DIR}/ClientManager.h
    ${HEADER_DIR}/HeaderManager.h
    ${HEADER_DIR}/IPacket.h
    ${HEADER_DIR}/Logger.h
    ${HEADER_DIR}/Macros.h
    ${HEADER_DIR}/OPacket.h
    ${HEADER_DIR}/PacketManager.h
    ${HEADER_DIR}/PKey.h
    ${HEADER_DIR}/PKeyOwner.h
    ${HEADER_DIR}/Server.h
    ${HEADER_DIR}/ServicePool.h
    ${HEADER_DIR}/TCPAcceptor.h
    ${HEADER_DIR}/TCPConnection.h)

set(SOURCES
	${SOURCE_DIR}/Client.cpp
	${SOURCE_DIR}/ClientManager.cpp
    ${SOURCE_DIR}/HeaderManager.cpp
    ${SOURCE_DIR}/IPacket.cpp
    ${SOURCE_DIR}/OPacket.cpp
    ${SOURCE_DIR}/PacketManager.cpp
    ${SOURCE_DIR}/PKey.cpp
    ${SOURCE_DIR}/PKeyOwner.cpp
    ${SOURCE_DIR}/Server.cpp
    ${SOURCE_DIR}/ServicePool.cpp
    ${SOURCE_DIR}/TCPAcceptor.cpp
    ${SOURCE_DIR}/TCPConnection.cpp)

include_directories(${HEADER_DIR})

include_directories(${Boost_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 11)