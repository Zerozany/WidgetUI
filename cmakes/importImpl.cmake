file(GLOB IMPL_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/module/**/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/module/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/**/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/**/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/**/*.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${IMPL_SOURCE}
)

foreach(HEADERDIR ${IMPL_SOURCE})
    get_filename_component(DIR ${HEADERDIR} DIRECTORY)
    target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${DIR}
    )
endforeach()
