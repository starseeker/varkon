include_directories(
  ${CMAKE_CURRENT_SOURCE_DIRECTORY}/src
  )

add_definitions(-DUNIX)

set(WP_SRCS
  src/wpinit.c
  src/wpLWIN.c
  src/wpfont.c
  src/wpwait.c
  src/wpgetkey.c
  src/wpmenu.c
  src/wpgetalt.c
  src/wpgetstr.c
  src/wpRWIN.c
  src/wpw.c
  src/wpIWIN.c
  src/wpBUTT.c
  src/wpEDIT.c
  src/wpICON.c
  src/wpGWIN.c
  src/wpviews.c
  src/wpzoom.c
  src/wpplylin.c
  src/wpDF.c
  src/wppoint.c
  src/wpline.c
  src/wparc.c
  src/wpcurve.c
  src/wpsurf.c
  src/wptext.c
  src/wpxhatch.c
  src/wpdim.c
  src/wpcsy.c
  src/wpbplane.c
  src/wpedmbs.c
  src/wpcolor.c
  src/wpGLlist.c
  src/wpplot.c
  src/wpprint_X11.c
  src/wpprint_GL.c
  src/wphide.c
  src/wpDXF.c
  src/wpgrid.c
  src/wplight.c
  src/wpmesh.c
  src/wplevels.c
  src/wpgetpos.c
  src/wptooltip.c
  src/wpattributes.c
  src/wpjob.c
  src/wpMCWIN.c
  src/wpSBAR.c
  src/wpDECRN.c
  src/wpfsect.c
  )

# Build wp library
add_library(wp SHARED ${WP_SRCS})
if(X11_Xpm_FOUND)
  set(X11_LIBRARIES ${X11_LIBRARIES} ${X11_Xpm_LIB})
endif(X11_Xpm_FOUND)
target_link_libraries(wp ${X11_LIBRARIES} ${OPENGL_LIBRARIES})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

