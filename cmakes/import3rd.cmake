find_package(toml11 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    toml11::toml11
)
