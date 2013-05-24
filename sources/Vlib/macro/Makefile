##########################################################
#
#   Makefile for various macros
#
#   Note that this makefile depends on the environment
#   variable VARKON_ROOT to be set up properly in your
#   login-file, shell rc-file or elswhere. 
#   VARKON_ROOT is the top directory of your
#   Varkon-installation.
#
#  (C)Microform AB 1999-11-05 J.Kjellander
#  2006-09-21 , Sören Larsson, Örebro university
#               Changed, now not dependent of VARKON_ROOT
#
##########################################################

RM = rm -f

#COMPILE = $(VARKON_ROOT)/bin/mbsc
COMPILE = ../../../bin/mbsc

#LIBDIR = $(VARKON_ROOT)/lib
LIBDIR = ../../../lib

# MBO-files

OBJECTS = $(LIBDIR)/ma_posw.MBO   \
          $(LIBDIR)/ma_posw1.MBO  \
          $(LIBDIR)/ma_posw2.MBO  \
          $(LIBDIR)/ma_posw3.MBO  \
          $(LIBDIR)/ma_posw4.MBO  \
          $(LIBDIR)/ma_posw5.MBO  \
          $(LIBDIR)/ma_posw6.MBO  \
          $(LIBDIR)/ma_text2.MBO  \
          $(LIBDIR)/ma_textc.MBO  \
          $(LIBDIR)/ma_texth.MBO  \
          $(LIBDIR)/ma_view8.MBO

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
	$(RM) $(LIBDIR)/ma*.MBO

##########################################################
