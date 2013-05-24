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
#
##########################################################

RM = rm -f

COMPILE = $(VARKON_ROOT)/bin/mbsc

LIBDIR = $(VARKON_ROOT)/lib

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
          $(LIBDIR)/igs_gsec.MBO    \
          $(LIBDIR)/igs_in.MBO     \
          $(LIBDIR)/igs_rekn.MBO   \
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
