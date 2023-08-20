cmake_minimum_required(VERSION 3.10)

project(client VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -O3")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Boost REQUIRED system thread)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
