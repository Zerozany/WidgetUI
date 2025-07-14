if(MSVC)
    message(STATUS "The compiler type is Visual Studio")
    target_compile_options(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:/Od>"
        "$<$<CONFIG:Release>:/Ox>"
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "The compiler type is Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:-O0>"
        "$<$<CONFIG:Release>:-O3 -march=native>"
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "The compiler type is GCC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:-O0>"
        "$<$<CONFIG:Release>:-O3 -march=native>"
    )
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    UILIB_HANDLE
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    DEBUG_POSTFIX "d"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin"
)
