cmake_minimum_required(VERSION 3.10)
project(cli-calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(calculator
    main.cpp
    tokenizer.cpp
    parser.cpp
    evaluator.cpp
    math_functions.cpp
    utils.cpp
)

target_include_directories(calculator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_options(calculator PRIVATE /W4)
else()
    target_compile_options(calculator PRIVATE -Wall -Wextra -O3)
endif()
