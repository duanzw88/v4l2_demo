set(PROJECT CProject_main)
message("Making ${PROJECT} ...")
cmake_minimum_required(VERSION 3.16)
project(${PROJECT})
include_directories(./include)
aux_source_directory(./src SRC_FILE)
add_executable(run ${SRC_FILE})
message("Make ${PROJECT} ok, please execute run")
