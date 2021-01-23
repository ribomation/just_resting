cmake_minimum_required(VERSION 3.16)
project(just-resting
        LANGUAGES       CXX
        VERSION         1.0
        DESCRIPTION     "An ultra-fast light-weight REST-WS server based on features of C++17"
        HOMEPAGE_URL    https://just-resting.org/
        )

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(WARNING_FLAGS               -Wall -Wextra -Werror -Wfatal-errors)
set(DIST_DIR                    ${CMAKE_CURRENT_SOURCE_DIR}/dist)

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(demo EXCLUDE_FROM_ALL)
add_subdirectory(test-e2e EXCLUDE_FROM_ALL)

add_custom_target(dist ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DIST_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_DIR}/include ${DIST_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/lib/api ${DIST_DIR}/include
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:just_resting> ${DIST_DIR}/lib/just_resting.a
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${DIST_DIR}
        )

add_custom_target(test ALL
        COMMAND unit-tests
        WORKING_DIRECTORY test
        )

add_custom_target(demo
        COMMAND books
        WORKING_DIRECTORY demo
        USES_TERMINAL
        )


