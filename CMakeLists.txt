cmake_minimum_required(VERSION 3.16)

project(cdatasetLoader VERSION 1.0.0 LANGUAGES C)

option(BUILD_SHARED_LIBS "Build as a shared library" ON)

enable_testing()

find_package(CURL REQUIRED)



add_library(cdatasetLoader SHARED ./src/cdataset_loader.c)
target_include_directories(cdatasetLoader PUBLIC ./include)
target_link_libraries(cdatasetLoader PRIVATE CURL::libcurl)

add_subdirectory(examples)