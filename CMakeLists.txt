cmake_minimum_required(VERSION 3.6)
project(concaten C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES tokenizer.c stringbuilder.c method_map.c prop_map.c object.c data_stack.c)
add_executable(concaten main.c ${SOURCE_FILES})
set(TEST_SOURCES test/prop_map.c test/tokenizer.c test/method_map.c test/object.c)
add_executable(tester test.c ${SOURCE_FILES} ${TEST_SOURCES})
