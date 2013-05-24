##########################################################
#
#   Makefile for dxf
#
#   Note that this makefile depends on the environment
#   variable VARKON_ROOT to be set up properly in your
#   login-file, shell rc-file or elswhere. 
#   VARKON_ROOT is the top directory of your
#   Varkon-installation.
#
#  (C)Microform AB 1999-11-05 J.Kjellander
#
##########################################################

RM = rm -f

COMPILE = $(VARKON_ROOT)/bin/mbsc

LIBDIR = $(VARKON_ROOT)/lib

# MBO-files

OBJECTS = $(LIBDIR)/dxf_2D.MBO     \
          $(LIBDIR)/dxf_3D.MBO     \
          $(LIBDIR)/dxf_arc2.MBO   \
          $(LIBDIR)/dxf_arc3.MBO   \
          $(LIBDIR)/dxf_blk2.MBO   \
          $(LIBDIR)/dxf_bsec.MBO   \
          $(LIBDIR)/dxf_es2D.MBO   \
          $(LIBDIR)/dxf_es3D.MBO   \
          $(LIBDIR)/dxf_fac.MBO    \
          $(LIBDIR)/dxf_fnam.MBO   \
          $(LIBDIR)/dxf_hsec.MBO   \
          $(LIBDIR)/dxf_ins2.MBO   \
          $(LIBDIR)/dxf_lev.MBO    \
          $(LIBDIR)/dxf_lin.MBO    \
          $(LIBDIR)/dxf_mbs.MBO    \
          $(LIBDIR)/dxf_out3.MBO   \
          $(LIBDIR)/dxf_ply.MBO    \
          $(LIBDIR)/dxf_poi.MBO    \
          $(LIBDIR)/dxf_sol.MBO    \
          $(LIBDIR)/dxf_tsec.MBO   \
          $(LIBDIR)/dxf_txt.MBO    \
          $(LIBDIR)/dxf_xsec.MBO

# The rule for compiling MBS sources and moving
# the MBO-module to the Varkon library

$(LIBDIR)/%.MBO : ./%.MBS
	$(COMPILE) $<
	mv $*.MBO $(LIBDIR)
	@echo " "
	@echo " "
	@echo " "

# Targets

distr : $(OBJECTS)


clean:
	$(RM) $(LIBDIR)/dxf*.MBO

##########################################################
