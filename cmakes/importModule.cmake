file(GLOB MODULE_FILES
    ${CMAKE_SOURCE_DIR}/module/**/*.h
    ${CMAKE_SOURCE_DIR}/module/**/*.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${MODULE_FILES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/module/WidgetTitleBar
)
