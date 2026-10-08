cmake_minimum_required(VERSION 4.1)
project(SearchFile)

set(CMAKE_CXX_STANDARD 26)

add_executable(SearchFile main.cpp
        RechercheFichiers.h
        RechercheFichiers.cpp)
