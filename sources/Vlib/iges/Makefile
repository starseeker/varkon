##########################################################
#
#   Makefile for IGES import.
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

OBJECTS = $(LIBDIR)/igs_106.MBO    \
          $(LIBDIR)/igs_110.MBO    \
          $(LIBDIR)/igs_126.MBO    \
          $(LIBDIR)/igs_128.MBO    \
          $(LIBDIR)/igs_dsec.MBO   \
          $(LIBDIR)/igs_ents.MBO   \
          $(LIBDIR)/igs_fnam.MBO   \
          $(LIBDIR)/igs_gflt.MBO   \
          $(LIBDIR)/igs_gint.MBO   \
          $(LIBDIR)/igs_gsec.MBO   \
          $(LIBDIR)/igs_in.MBO     \
          $(LIBDIR)/igs_ssec.MBO

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
	$(RM) $(LIBDIR)/igs*.MBO

##########################################################
