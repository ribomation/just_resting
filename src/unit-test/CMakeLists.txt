cmake_minimum_required(VERSION 3.10)
project(just-resting-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(unit-tests
        test-driver.cxx
        string-utils-test.cxx
        assets-handler-test.cxx
        response-test.cxx
        )
target_compile_options(unit-tests       PUBLIC -Wall -Wextra -Werror -Wfatal-errors)
target_include_directories(unit-tests   PUBLIC libs ../api ../impl)
target_link_libraries(unit-tests        just_resting)
add_custom_command(TARGET unit-tests    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/unit-test/data
        $<TARGET_FILE_DIR:unit-tests>/data
        )
