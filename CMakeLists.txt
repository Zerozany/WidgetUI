cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(QModules VERSION 1.0.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/main.cpp
)

include(${PROJECT_SOURCE_DIR}/cmakes/importQt.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importPrivate.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importSource.cmake)
include(${PROJECT_SOURCE_DIR}/cmakes/importModule.cmake)

# include(${PROJECT_SOURCE_DIR}/cmakes/importUtils.cmake)
target_compile_features(${PROJECT_NAME}
    PRIVATE
    cxx_std_23
)
