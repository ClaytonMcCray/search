cmake_minimum_required(VERSION 3.14)
project(search)
set(cmake_c_compiler clang)
set(cmake_cxx_compiler clang++)
set(cmake_cxx_standard_required on)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(search search.cpp)
set_property(TARGET search PROPERTY CXX_STANDARD 17)
target_compile_options(search PRIVATE -Werror -Wall -Wextra)
target_link_libraries(search Threads::Threads)
