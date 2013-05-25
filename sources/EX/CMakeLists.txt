include_directories(
  ${CMAKE_CURRENT_SOURCE_DIRECTORY}/src
  )

add_definitions(-DUNIX)
add_definitions(-DVARKON)

set(EX_SRCS
  src/exarc.c
  src/exbplane.c
  src/excsy.c
  src/excugl.c
  src/excur1.c
  src/excur2.c
  src/exdia1.c
  src/exdia2.c
  src/exdim.c
  src/exdraw.c
  src/exgatt.c
  src/exgeofun1.c
  src/exgeofun2.c
  src/exget1.c
  src/exget2.c
  src/exgroup.c
  src/exline.c
  src/exmesh.c
  src/exmove.c
  src/exodbc.c
  src/exoru.c
  src/exos.c
  src/expart.c
  src/expoint.c
  src/exrap.c
  src/exref.c
  src/exjob.c
  src/exsugl.c
  src/exsur1.c
  src/exsur2.c
  src/exsur3.c
  src/exsurcur.c
  src/exsurexd.c
  src/exsurtrim.c
  src/exsurusr.c
  src/exsymb.c
  src/extcp.c
  src/extext.c
  src/extform.c
  src/extime.c
  src/exview.c
  src/exlevel.c
  src/exxhatch.c
  ) 

if(ENABLE_RAP_SUPPORT)
  set(EX_SRCS ${EX_SRCS} src/rap_clnt.c)
endif(ENABLE_RAP_SUPPORT)

if(ENABLE_ATLAS_SUPPORT)
  set(EX_SRCS ${EX_SRCS} src/exmatrix.c)
endif(ENABLE_ATLAS_SUPPORT)
  
# Build EX library
add_library(ex SHARED ${EX_SRCS})
target_link_libraries(ex ${BLAS_LIBRARIES})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

