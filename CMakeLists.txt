cmake_minimum_required(VERSION 3.6)
project(concaten C)

set(CMAKE_C_STANDARD 11)

set(NORMAL_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic")
set(NORMAL_LIBS)

set(TEST_FLAGS "")
set(TEST_LIBS)

# Used by things like gperftools, so I figure I might as well drop it in.
set(PROFILE_FLAGS "-static -g")
set(PROFILE_LIBS)

set(NORMAL_SRCS
        tokenizer.c stringbuilder.c method_map.c prop_map.c object.c
        data_stack.c token_stack.c code_block.c)
set(TEST_SRCS
        tests/prop_map.c tests/tokenizer.c tests/method_map.c tests/object.c
        tests/data_stack.c tests/token_stack.c tests/code_block.c
        tests/stringbuilder.c)

add_executable(concaten main.c ${NORMAL_SRCS})
set_target_properties(concaten PROPERTIES COMPILE_FLAGS
        "${NORMAL_FLAGS}")
target_link_libraries(concaten ${NORMAL_LIBS})
add_executable(tester test.c ${NORMAL_SRCS} ${TEST_SRCS})
set_target_properties(tester PROPERTIES COMPILE_FLAGS
        "${NORMAL_FLAGS} ${TEST_FLAGS}")
target_link_libraries(tester ${NORMAL_LIBS} ${TEST_LIBS})

add_executable(concaten_profile main.c ${NORMAL_SRCS})
set_target_properties(concaten_profile PROPERTIES COMPILE_FLAGS
        "${NORMAL_FLAGS} ${PROFILE_FLAGS}")
target_link_libraries(concaten_profile ${NORMAL_LIBS} ${PROFILE_LIBS})
add_executable(tester_profile test.c ${NORMAL_SRCS} ${TEST_SRCS})
set_target_properties(tester_profile PROPERTIES COMPILE_FLAGS
        "${NORMAL_FLAGS} ${TEST_FLAGS} ${PROFILE_FLAGS}")
target_link_libraries(tester_profile
        ${NORMAL_LIBS} ${TEST_LIBS} ${PROFILE_LIBS})
