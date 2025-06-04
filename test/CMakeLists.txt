add_executable(${PROJECT_NAME}Demo
    ${PROJECT_SOURCE_DIR}/test/main.cpp
)

add_dependencies(${PROJECT_NAME}Demo ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}Demo PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
)

find_package(QT NAMES Qt6 Qt5 REQUIRED
    COMPONENTS
    Widgets
    Core
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    COMPONENTS
    Widgets
    Core
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME}Demo)
endif()

target_compile_features(${PROJECT_NAME}Demo
    PRIVATE
    cxx_std_23
)

target_include_directories(${PROJECT_NAME}Demo
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}Demo
    PRIVATE
    ${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME}Demo
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(${PROJECT_NAME}Demo PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE
)
