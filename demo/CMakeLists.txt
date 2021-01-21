
add_executable(books)
target_sources(books PRIVATE
        libs/json.hpp
        src/books.hxx
        src/auth-filter.hxx
        src/log-filter.hxx
        src/json-filter.hxx
        src/books-app.cxx
        )
target_include_directories(books PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libs
        )
target_link_libraries(books PRIVATE
        just_resting --static
        )

add_custom_command(TARGET books    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:books>/assets
        )
