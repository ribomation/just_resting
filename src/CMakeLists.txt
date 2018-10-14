cmake_minimum_required(VERSION 3.10)
project(just-resting-lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(API
        api/application.hxx
        api/filter.hxx
        api/interface.hxx
        api/options.hxx
        api/request.hxx
        api/response.hxx
        api/route.hxx
        api/string-value.hxx
        )

set(IMPL
        impl/application.cxx
        impl/asset-handler.hxx
        impl/asset-handler.cxx
        impl/engine.hxx
        impl/engine.cxx
        impl/http-handler.hxx
        impl/http-handler.cxx
        impl/request-impl.hxx
        impl/request-impl.cxx
        impl/response-impl.hxx
        impl/response-impl.cxx
        impl/string-utils.hxx
        )

add_library(just_resting                    STATIC  ${API} ${IMPL})
target_include_directories(just_resting     PUBLIC  api )
target_link_libraries(just_resting          PUBLIC stdc++fs )
target_compile_options(just_resting         PUBLIC -Wall -Wextra -Werror -Wfatal-errors -Wno-unused-parameter)



