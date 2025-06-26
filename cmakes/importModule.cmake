file(GLOB MODULE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/module/**/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/module/**/*.cpp
)

target_sources(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${MODULE_FILES}
)

target_include_directories(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/module/WidgetTitleBar
)
