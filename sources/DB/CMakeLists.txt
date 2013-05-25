include_directories(
  ${CMAKE_CURRENT_SOURCE_DIRECTORY}/src
  )

add_definitions(-DUNIX)

set(DB_SRCS
  src/db1.c
  src/db2.c
  src/db3.c
  src/db4.c
  src/dbarc.c
  src/dbbplane.c
  src/dbcsys.c
  src/dbcurve.c
  src/dbdelete.c
  src/dbdim.c
  src/dbfile.c
  src/dbgroup.c
  src/dbheader.c
  src/dbid.c
  src/dbjob.c
  src/dbkey.c
  src/dbline.c
  src/dbmesh.c
  src/dbpart.c
  src/dbpoint.c
  src/dbsurf.c
  src/dbtext.c
  src/dbtform.c
  src/dbtraverse.c
  src/dbxhatch.c
  ) 

# Build DB library
add_library(db SHARED ${DB_SRCS})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

