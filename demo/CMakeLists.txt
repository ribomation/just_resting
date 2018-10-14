cmake_minimum_required(VERSION 3.10)
project(just-resting-demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(books
        src/books.hxx
        src/auth-filter.hxx
        src/log-filter.hxx
        libs/json.hpp
        src/books-app.cxx
        )

target_link_libraries(books
        just_resting
        stdc++fs
        )



