include_directories(
  ${CMAKE_CURRENT_SOURCE_DIRECTORY}/src
  )

add_definitions(-DUNIX)
add_definitions(-DVARKON)

set(PM_SRCS
  src/evact.c
  src/eval1.c
  src/evarc.c
  src/evarea.c
  src/evbpln.c
  src/evcray.c
  src/evcsy.c
  src/evcsys.c
  src/evcur.c
  src/evdim.c
  src/evdll.c
  src/evoru.c
  src/evexit.c
  src/evexst.c
  src/evfile.c
  src/evget.c
  src/evgfuncs.c
  src/evgprocs.c
  src/evgrid.c
  src/evgrp.c
  src/evlev.c
  src/evlin.c
  src/evmath.c
  src/evmatrix.c
  src/evmbs.c
  src/evmesh.c
  src/evos.c
  src/evodbc.c
  src/evpart.c
  src/evplot.c
  src/evpoi.c
  src/evtime.c
  src/evset.c
  src/evcolor.c
  src/evstr.c
  src/evsur.c
  src/evsymb.c
  src/evtcp.c
  src/evrap.c
  src/evtext.c
  src/evtform.c
  src/evui.c
  src/evvec.c
  src/evvfile.c
  src/evview.c
  src/evwin.c
  src/evxht.c
  src/inac10.c
  src/inac11.c
  src/inac13.c
  src/inac17.c
  src/inac19.c
  src/inac20.c
  src/inacc.c
  src/infunc.c
  src/ingeop.c
  src/ingpar.c
  src/innpar.c
  src/inproc.c
  src/inrts.c
  src/pmac1.c
  src/pmac2.c
  src/pmac3.c
  src/pmac4.c
  src/pmac5.c
  src/pmac6.c
  src/pmallo.c
  src/pmlist.c
  src/pretty.c
  src/stacc.c
  src/stac10.c
  src/calext.c
  ../AN/src/anaexp.c
  ../AN/src/anascan.c
  ../AN/src/annaml.c
  ../AN/src/anpar11.c
  ../AN/src/anpar12.c
  ../AN/src/anpar31.c
  ../AN/src/anpar51.c
  ../AN/src/anpar52.c
  ../AN/src/anlog.c
  src/evpinc.c
  )

# Build PM library
add_library(pm SHARED ${PM_SRCS})
target_link_libraries(pm ${M_LIBRARY})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

