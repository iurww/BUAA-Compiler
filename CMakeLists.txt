cmake_minimum_required(VERSION 3.16)
project(compiler)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(include/frontend)
include_directories(include/mir)
include_directories(include/util)
include_directories(include/backend)

aux_source_directory(src/frontend SRC_FRONTEND)
aux_source_directory(src/util SRC_UTIL)
aux_source_directory(src/mir SRC_MIR)
aux_source_directory(src/backend SRC_BACKEND)

add_executable(compiler src/main.cpp ${SRC_FRONTEND} ${SRC_UTIL} ${SRC_MIR} ${SRC_BACKEND})
