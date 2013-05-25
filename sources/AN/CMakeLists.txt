include_directories(
  ${CMAKE_CURRENT_SOURCE_DIRECTORY}/src
  )

add_definitions(-DUNIX)
add_definitions(-DANALYZER)
add_definitions(-DVARKON)

set(MBSC_SRCS
  src/anascan.c
  src/anascan.c
  src/anlog.c
  src/annaml.c
  src/anpar11.c
  src/anpar12.c
  src/anpar31.c
  src/anpar40.c
  src/anpar51.c
  src/anpar52.c
  src/mbsc.c
  ../PM/src/inacc.c
  ../PM/src/ingpar.c
  ../PM/src/infunc.c
  ../PM/src/inac10.c
  ../PM/src/inac11.c
  ../PM/src/inac19.c
  ../PM/src/pmac1.c
  ../PM/src/pmac2.c
  ../PM/src/pmac5.c
  ../PM/src/pmallo.c
  ../PM/src/pmlist.c
  ../PM/src/evmath.c
  ../PM/src/evvec.c
  ../PM/src/evstr.c
  ../PM/src/stacc.c
  ../PM/src/stac10.c
  ../IG/src/igenvpath.c
  ../IG/src/igmalloc.c
  )

# Build mbsc
add_executable(mbsc ${MBSC_SRCS})
target_link_libraries(mbsc ${M_LIBRARY})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

