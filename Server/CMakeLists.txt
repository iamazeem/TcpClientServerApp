cmake_minimum_required(VERSION 3.10)

project(Server VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(Server ${SOURCES})

find_package(Boost REQUIRED system thread)
target_link_libraries(Server ${Boost_LIBRARIES})
