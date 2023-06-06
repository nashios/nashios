set(SOURCES
    Main.cpp
    Service.cpp)

nashios_bin(SystemService)
target_link_libraries(SystemService PRIVATE LibCore)
