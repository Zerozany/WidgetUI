cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
else()
    message(FATAL_ERROR "Vcpkg environment variable VCPKG_ROOT not found")
endif()

set(QWIDGETUI_LIB_NAME "WidgetUI")

project(${QWIDGETUI_LIB_NAME}
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "A versatile and customizable UI component designed to enhance user interaction and streamline interface development"
    HOMEPAGE_URL "https://github.com/Zerozany/WidgetUI"
)

option(TEST_SWITCH "This is a switch that controls whether to compile test examples" OFF)

option(BUILD_SHARED_LIBS "Build shared libraries by default" ON)

add_library(${QWIDGETUI_LIB_NAME}
)

target_compile_features(${QWIDGETUI_LIB_NAME}
    PRIVATE
    cxx_std_23
)

include(${PROJECT_SOURCE_DIR}/cmakes/importProjectConfig.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importQt.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importPrivate.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importSource.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importModule.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importUtils.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/import3rd.cmake)

if(TEST_SWITCH)
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()
