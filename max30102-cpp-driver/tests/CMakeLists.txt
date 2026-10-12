cmake_minimum_required(VERSION 3.16)

project(MAX30102HostTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_max30102
    test_max30102.cpp
    FakeI2C.cpp
    ../MAX30102.cpp
)

target_include_directories(test_max30102 PRIVATE ..)

enable_testing()

add_test(NAME MAX30102UnitTests COMMAND test_max30102)

target_compile_options(test_max30102 PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)