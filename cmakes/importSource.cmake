file(GLOB SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB RESOURCES_LIST
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/css/*.css
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/**/*.png
)

target_sources(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${SOURCE_FILES}
)

qt_add_resources(${QWIDGETUI_LIB_NAME} "Main"
    PREFIX "/"
    FILES
    ${RESOURCES_LIST}
)

target_include_directories(${QWIDGETUI_LIB_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}
)

add_custom_target(${QWIDGETUI_LIB_NAME}include ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*
    COMMENT "Copying header files from source to build directory..."
)
