include(cmake/CPM.cmake)
CPMAddPackage("gh:CodeFinder2/rapidxml@1.13")

cmake_minimum_required(VERSION 3.22)
project(Prototype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(prototype main.cpp)
target_link_libraries(prototype rapidxml)
