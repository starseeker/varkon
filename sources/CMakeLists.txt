# Build PM
add_subdirectory(PM)

# Build EX
add_subdirectory(EX)

# Build DB
add_subdirectory(DB)

# Build IG
add_subdirectory(IG)

# Build GE
add_subdirectory(GE)

# Build WP
add_subdirectory(WP)

# Build xvarkon
add_definitions(-DUNIX)
add_executable(xvarkon IG/src/varkon.c)
target_link_libraries(xvarkon ig pm ex db ge wp ${X11_LIBRARIES} ${OPENGL_gl_LIBRARY})

# Build mbsc
add_subdirectory(AN)

# Build PL

# Build Vlib

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

