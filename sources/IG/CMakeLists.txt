include_directories(
  ${CMAKE_CURRENT_SOURCE_DIRECTORY}/src
  )

add_definitions(-DUNIX)

set(IG_SRCS
  src/iginit.c
  src/igsurf.c
  src/igview.c
  src/igcurve.c
  src/igpoint.c
  src/igpart.c
  src/igtext.c
  src/igcsy.c
  src/igbplane.c
  src/igline.c
  src/igarc.c
  src/igsysdat.c
  src/igdelete.c
  src/igexpr.c
  src/igstatem.c
  src/igplot.c
  src/igID.c
  src/igdim.c
  src/iggroup.c
  src/igsymb.c
  src/ighelp.c
  src/igmodule.c
  src/igprompt.c
  src/igmenu.c
  src/iginput.c
  src/igjob.c
  src/igset.c
  src/igenvpath.c
  src/igattedit.c
  src/igerror.c
  src/igmalloc.c
  src/igPID.c
  src/igtform.c
  src/igfile.c
  src/igdebug.c
  )

# Build IG library
add_library(ig SHARED ${IG_SRCS})
target_link_libraries(ig ${M_LIBRARY})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

