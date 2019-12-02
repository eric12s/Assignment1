cmake_minimum_required(VERSION 3.15)
project(SPLFlix)

set(CMAKE_CXX_STANDARD 14)

add_executable(SPLFlix src/Main.cpp src/Session.cpp src/User.cpp src/Action.cpp src/Watchable.cpp
        include/Action.h include/json.hpp include/Session.h include/User.h include/Watchable.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Weffc++ -std=c++11")