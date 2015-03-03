cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bunsan_bacs_system)

find_package(BunsanCMake CONFIG REQUIRED)

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
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_utility bunsan_utility)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_bacs_common bunsan_bacs_common)
bunsan_use_bunsan_package(${PROJECT_NAME} yandex_contest_invoker yandex_contest_invoker)
bunsan_use_package(${PROJECT_NAME} XMLRPC c++2 libwww-client)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_programs(
    src/bin/${PROJECT_NAME}_build_mono_develop.py
    RENAME ${PROJECT_NAME}_build_mono_develop
)
bunsan_install_project()

bunsan_include_tests()
