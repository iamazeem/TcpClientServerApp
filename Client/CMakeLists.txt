cmake_minimum_required(VERSION 3.10)

project(Client VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(Client ${SOURCES})

find_package(Boost REQUIRED system)
target_link_libraries(Client ${Boost_LIBRARIES})
