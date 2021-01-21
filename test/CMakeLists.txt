

add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE libs)

add_executable(unit-tests)
target_sources(unit-tests PUBLIC
        src/test-driver.cxx
        src/string-utils-test.cxx
        src/assets-handler-test.cxx
        src/response-test.cxx
        src/http-handler-test.cxx
        )
target_compile_options(unit-tests       PUBLIC ${WARNING_FLAGS})
target_include_directories(unit-tests   PUBLIC ../lib/api ../lib/impl)
target_link_libraries(unit-tests        just_resting catch2)

add_custom_command(TARGET unit-tests    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:unit-tests>/data
        )
