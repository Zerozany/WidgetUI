file(GLOB SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/**/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/**/*.hpp
)

target_sources(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${SOURCE_FILES}
)

target_include_directories(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/StyleLoader
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/Win32Kit
)
