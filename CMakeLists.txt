project(bunsan_bacs_system)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common bacs_common)
bunsan_use_boost(system filesystem)
bunsan_use_protobuf()

bunsan_add_shared_library(${PROJECT_NAME}
    src/process.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
