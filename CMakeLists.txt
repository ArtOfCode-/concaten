cmake_minimum_required(VERSION 2.8.5)
project(concaten C)

set(CMAKE_C_STANDARD 11)

set(NORMAL_FLAGS "-Wall -Wextra -Werror -pedantic -O3 -std=c11")
set(NORMAL_LIBS m)

set(TEST_FLAGS "-g")
set(TEST_LIBS)

if(ENABLE_ASAN)
    set(PROFILE_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
    set(PROFILE_LIBS asan)
else()
    set(PROFILE_FLAGS "-g -O0")
    set(PROFILE_LIBS)
endif()

set(NORMAL_SRCS
        tokenizer.c stringbuilder.c method_map.c prop_map.c object.c
        data_stack.c token_stack.c code_block.c tkn_value_func.c runnable.c
        scope_stack.c type.c stl.c ctno_ctors.c stl/integral.c stl/string.c
        stl/globals.c stl/real.c)
set(TEST_SRCS
        tests/prop_map.c tests/tokenizer.c tests/method_map.c tests/object.c
        tests/data_stack.c tests/token_stack.c tests/code_block.c
        tests/stringbuilder.c tests/tkn_value_func.c tests/runnable.c
        tests/scope_stack.c tests/type.c)

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
