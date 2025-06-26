file(GLOB SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/**/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/**/*.cpp
)

target_sources(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${SOURCE_FILES}
)

target_include_directories(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/WidgetFramePrivate
)
