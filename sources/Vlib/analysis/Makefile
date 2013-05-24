##########################################################
#
#   Makefile for analysis
#
#   Note that this makefile depends on the environment
#   variable VARKON_ROOT to be set up properly in your
#   login-file, shell rc-file or elswhere. 
#   VARKON_ROOT is the top directory of your
#   Varkon-installation.
#
#  (C)Microform AB 1999-11-05 J.Kjellander
#  2006-09-21 Changed, Sören Larsson, Örebro University
#             now not dependent of VARKON_ROOT
#
##########################################################

RM = rm -f

#COMPILE = $(VARKON_ROOT)/bin/mbsc
COMPILE = ../../../bin/mbsc

#LIBDIR = $(VARKON_ROOT)/lib
LIBDIR = ../../../lib

# MBO-files

OBJECTS = $(LIBDIR)/l_akedja.MBO     \
          $(LIBDIR)/l_an1pos.MBO     \
          $(LIBDIR)/l_anaadm.MBO     \
          $(LIBDIR)/l_anaarc.MBO     \
          $(LIBDIR)/l_anabpl.MBO     \
          $(LIBDIR)/l_anacdm.MBO     \
          $(LIBDIR)/l_anacsy.MBO     \
          $(LIBDIR)/l_anacur.MBO     \
          $(LIBDIR)/l_anagrp.MBO     \
          $(LIBDIR)/l_analdm.MBO     \
          $(LIBDIR)/l_analin.MBO     \
          $(LIBDIR)/l_anapoi.MBO     \
          $(LIBDIR)/l_anaprt.MBO     \
          $(LIBDIR)/l_anardm.MBO     \
          $(LIBDIR)/l_anastr.MBO     \
          $(LIBDIR)/l_anasur.MBO     \
          $(LIBDIR)/l_anamsh.MBO     \
          $(LIBDIR)/l_anatrf.MBO     \
          $(LIBDIR)/l_anatxt.MBO     \
          $(LIBDIR)/l_anavst.MBO     \
          $(LIBDIR)/l_anaxht.MBO     \
          $(LIBDIR)/l_anbino.MBO     \
          $(LIBDIR)/l_aneval.MBO     \
          $(LIBDIR)/l_anhk.MBO       \
          $(LIBDIR)/l_ankrok.MBO     \
          $(LIBDIR)/l_anlgd.MBO      \
          $(LIBDIR)/l_anligr.MBO     \
          $(LIBDIR)/l_annagr.MBO     \
          $(LIBDIR)/l_annapa.MBO     \
          $(LIBDIR)/l_annorm.MBO     \
          $(LIBDIR)/l_antang.MBO     \
          $(LIBDIR)/l_aneval.MBO     \
          $(LIBDIR)/l_anvink.MBO     \
          $(LIBDIR)/l_area.MBO       \
          $(LIBDIR)/l_cgrav.MBO      \
          $(LIBDIR)/l_formel.MBO     \
          $(LIBDIR)/l_inval.MBO      \
          $(LIBDIR)/l_invcur.MBO     \
          $(LIBDIR)/l_invon.MBO      \
          $(LIBDIR)/l_invtan.MBO     \
          $(LIBDIR)/l_ligm.MBO       \
          $(LIBDIR)/l_lisp.MBO       \
          $(LIBDIR)/l_lniv1.MBO      \
          $(LIBDIR)/l_lniv2.MBO      \
          $(LIBDIR)/l_pcen.MBO       \
          $(LIBDIR)/l_rmbs1.MBO      \
          $(LIBDIR)/l_rmbs2.MBO      \
          $(LIBDIR)/l_searea.MBO     \
          $(LIBDIR)/l_sniv1.MBO      \
          $(LIBDIR)/l_sniv2.MBO      \
          $(LIBDIR)/l_stattr.MBO     \
          $(LIBDIR)/l_stgraf.MBO     \
          $(LIBDIR)/l_stjobb.MBO     \
          $(LIBDIR)/l_suarea.MBO     \
          $(LIBDIR)/l_sureval.MBO    \
          $(LIBDIR)/l_sysdat.MBO     \
          $(LIBDIR)/l_tniv1.MBO      \
          $(LIBDIR)/l_tniv2.MBO

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
	$(RM) $(LIBDIR)/l_*.MBO

##########################################################
