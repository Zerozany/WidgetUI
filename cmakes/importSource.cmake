file(GLOB SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

file(GLOB RESOURCES_LIST
    RELATIVE ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/resources/css/*.css
    ${CMAKE_SOURCE_DIR}/resources/icon/*.png
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
)
