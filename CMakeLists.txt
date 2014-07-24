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
    src/builder.cpp
    src/callback.cpp
    src/file.cpp
    src/process.cpp

    src/builders/compilable.cpp
    src/builders/native_compilable.cpp
    src/builders/gcc.cpp
    src/builders/fpc.cpp
    src/builders/interpretable.cpp
    src/builders/python.cpp
    src/builders/java.cpp
    src/builders/mono.cpp

    src/builders/project.cpp
    src/builders/mono_develop.cpp

    src/callbacks/xmlrpc.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
