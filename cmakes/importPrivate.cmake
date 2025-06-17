file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/Private/**/*.h
    ${CMAKE_SOURCE_DIR}/Private/**/*.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/Private/WidgetFramePrivate
    ${CMAKE_SOURCE_DIR}/Private/SystemTrayPrivate
)
