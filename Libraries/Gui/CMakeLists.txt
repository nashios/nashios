set(SOURCES
    Application.cpp)

nashios_lib(LibGui Gui)
target_link_libraries(LibGui PRIVATE LibCore)
