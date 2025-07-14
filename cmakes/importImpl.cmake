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

file(GLOB INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/module/*
    ${CMAKE_CURRENT_SOURCE_DIR}/Private/*
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/*
)

foreach(HEADERDIR ${INCLUDE_DIR})
    if(IS_DIRECTORY ${HEADERDIR})
        target_include_directories(${PROJECT_NAME}
            PRIVATE
            ${HEADERDIR}
        )
    endif()
endforeach()
