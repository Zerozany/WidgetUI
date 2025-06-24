file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

file(GLOB RESOURCES_LIST
    RELATIVE ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/resources/css/*.css
    ${CMAKE_SOURCE_DIR}/resources/icon/**/*.png
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCE_FILES}
)

qt_add_resources(${PROJECT_NAME} "Main"
    PREFIX "/"
    FILES
    ${RESOURCES_LIST}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}
)

add_custom_target(${PROJECT_NAME}include ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
    DEPENDS ${CMAKE_SOURCE_DIR}/include/*
    COMMENT "Copying header files from source to build directory..."
)
