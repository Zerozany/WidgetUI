file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/utils/**/*.h
    ${CMAKE_SOURCE_DIR}/utils/**/*.cpp
    ${CMAKE_SOURCE_DIR}/utils/**/*.hpp
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/utils/StyleLoader
    ${CMAKE_SOURCE_DIR}/utils/Win32Tools
    ${CMAKE_SOURCE_DIR}/utils/ConfigLoader
)
