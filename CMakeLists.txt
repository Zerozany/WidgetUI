cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(QWidgetUI VERSION 1.0.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/main.cpp
)

include(${PROJECT_SOURCE_DIR}/cmakes/importQt.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importPrivate.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importSource.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importModule.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importUtils.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/import3rd.cmake)

target_compile_features(${PROJECT_NAME}
    PRIVATE
    cxx_std_23
)
