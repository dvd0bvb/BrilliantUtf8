cmake_minimum_required(VERSION 3.27)

project(BrilliantUtf8 
    VERSION 0.0.1
    LANGUAGES CXX)

add_subdirectory(include)

enable_testing()
add_subdirectory(test)