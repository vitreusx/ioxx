cmake_minimum_required(VERSION 3.14)
project(ioxx LANGUAGES CXX)

add_subdirectory(ioxx)

add_executable(main main.cpp)
target_link_libraries(main
    PRIVATE ioxx)

target_compile_options(main
    PRIVATE
    $<$<CONFIG:Debug>:$<$<COMPILE_LANGUAGE:CXX>:-g>>
    $<$<CONFIG:Release>:$<$<COMPILE_LANGUAGE:CXX>:-Ofast;-march=native>>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic>)

add_custom_command(
    TARGET main
    POST_BUILD
    COMMAND objdump -drlwC -Mintel main >main.s
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
