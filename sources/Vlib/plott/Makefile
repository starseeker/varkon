##########################################################
#
#   Makefile for  plott
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

OBJECTS = $(LIBDIR)/p_autop.MBO    \
          $(LIBDIR)/p_chgppa.MBO   \
          $(LIBDIR)/p_getpar.MBO   \
          $(LIBDIR)/p_getplt.MBO   \
          $(LIBDIR)/p_getsl.MBO    \
          $(LIBDIR)/p_plott.MBO    \
          $(LIBDIR)/p_pwin.MBO     \
          $(LIBDIR)/p_redcnf.MBO   \
          $(LIBDIR)/p_savcnf.MBO   \
          $(LIBDIR)/p_setpar.MBO   \
          $(LIBDIR)/p_stripb.MBO   \
          $(LIBDIR)/p_trenv.MBO    \
          $(LIBDIR)/p_unix.MBO     \
          $(LIBDIR)/p_win95.MBO

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
	$(RM) $(LIBDIR)/p_*.MBO

##########################################################
