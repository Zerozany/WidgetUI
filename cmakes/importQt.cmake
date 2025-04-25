set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOUIC ON
    AUTOMOC ON
    AUTORCC ON
)

find_package(QT NAMES Qt6 Qt5 REQUIRED
    COMPONENTS
    Widgets
    Core
    Network
    Concurrent
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    COMPONENTS
    Widgets
    Core
    Network
    Concurrent
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
