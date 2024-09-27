# declare source files list
set(SOURCE_FILES
        includes/handler.cc
        includes/request.cc
        includes/response.cc
        includes/server.cc
        utils/logger.cc
    )

cmake_minimum_required(VERSION 3.28.2)
project(potion)
set(CMAKE_CXX_STANDARD 20)

# set compiler to be clang
set(CMAKE_CXX_COMPILER clang++)

# create compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(potion-example example.cpp ${SOURCE_FILES})

# add include directories
target_include_directories(potion-example PRIVATE includes PRIVATE utils)

# add compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# add debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("********** Debug build **********")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("********** Release build **********")
else()
    message("********** Unknown build type **********")
endif()




