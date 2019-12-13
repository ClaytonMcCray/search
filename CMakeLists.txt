cmake_minimum_required(VERSION 3.14)
project(search)
set(cmake_c_compiler clang)
set(cmake_cxx_compiler clang++)
set(cmake_cxx_standard_required on)

add_executable(search search.cpp)
set_property(TARGET search PROPERTY CXX_STANDARD 17)
target_compile_options(search PRIVATE -Werror -Wall -Wextra)
