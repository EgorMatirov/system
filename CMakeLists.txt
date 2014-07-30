project(bunsan_bacs_system)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(
    common
    process
    utility
    bacs_common
)
bunsan_use_xmlrpc(c++2 libwww-client)
bunsan_use_boost(system filesystem regex)
bunsan_use_protobuf()
bunsan_use(
    yandex_contest_common
    yandex_contest_system
    yandex_contest_invoker
)

bunsan_add_shared_library(${PROJECT_NAME}
    src/lib/builder.cpp
    src/lib/callback.cpp
    src/lib/file.cpp
    src/lib/process.cpp

    src/lib/builders/compilable.cpp
    src/lib/builders/native_compilable.cpp
    src/lib/builders/gcc.cpp
    src/lib/builders/fpc.cpp
    src/lib/builders/interpretable.cpp
    src/lib/builders/python.cpp
    src/lib/builders/java.cpp
    src/lib/builders/mono.cpp

    src/lib/builders/project.cpp
    src/lib/builders/mono_develop.cpp

    src/lib/callbacks/xmlrpc.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_programs(
    src/bin/${PROJECT_NAME}_mono_develop_build.py
    RENAME ${PROJECT_NAME}_mono_develop_build
)

bunsan_include_tests()
