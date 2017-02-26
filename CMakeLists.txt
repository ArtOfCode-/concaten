cmake_minimum_required(VERSION 3.6)
project(concaten C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES tokenizer.c stringbuilder.c prop_map.c)
set(TEST_SOURCES test/tokenizer.c test/prop_map.c)
add_executable(concaten main.c ${SOURCE_FILES})
add_executable(tester test.c ${SOURCE_FILES} ${TEST_SOURCES})
