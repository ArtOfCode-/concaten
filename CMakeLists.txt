cmake_minimum_required(VERSION 3.6)
project(concaten C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

set(SOURCE_FILES
        tokenizer.c stringbuilder.c method_map.c prop_map.c object.c
        data_stack.c token_stack.c code_block.c)
add_executable(concaten main.c ${SOURCE_FILES})
set(TEST_SOURCES
        tests/prop_map.c tests/tokenizer.c tests/method_map.c tests/object.c
        tests/data_stack.c tests/token_stack.c tests/code_block.c
        tests/stringbuilder.c)
add_executable(tester test.c ${SOURCE_FILES} ${TEST_SOURCES})
