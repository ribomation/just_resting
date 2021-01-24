
set(API
        api/just_resting.hxx
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
target_compile_options(just_resting         PUBLIC ${WARNING_FLAGS} -Wno-unused-parameter)
target_compile_features(just_resting        PUBLIC cxx_std_17)


