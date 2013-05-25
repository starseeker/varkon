
# Build mbsc
add_definitions(-DUNIX)
add_definitions(-DANALYZER)
add_definitions(-DVARKON)
set(MBSC_SRCS
  AN/src/anascan.c
  AN/src/anascan.c
  AN/src/anlog.c
  AN/src/annaml.c
  AN/src/anpar11.c
  AN/src/anpar12.c
  AN/src/anpar31.c
  AN/src/anpar40.c
  AN/src/anpar51.c
  AN/src/anpar52.c
  AN/src/mbsc.c
  PM/src/inacc.c
  PM/src/ingpar.c
  PM/src/infunc.c
  PM/src/inac10.c
  PM/src/inac11.c
  PM/src/inac19.c
  PM/src/pmac1.c
  PM/src/pmac2.c
  PM/src/pmac5.c
  PM/src/pmallo.c
  PM/src/pmlist.c
  PM/src/evmath.c
  PM/src/evvec.c
  PM/src/evstr.c
  PM/src/stacc.c
  PM/src/stac10.c
  IG/src/igenvpath.c
  IG/src/igmalloc.c
  )

add_executable(mbsc ${MBSC_SRCS})
target_link_libraries(mbsc ${M_LIBRARY})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

