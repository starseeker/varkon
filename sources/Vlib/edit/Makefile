##########################################################
#
#   Makefile for edit
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

OBJECTS = $(LIBDIR)/ed_adm.MBO      \
          $(LIBDIR)/ed_arc.MBO      \
          $(LIBDIR)/ed_arc1p.MBO    \
          $(LIBDIR)/ed_arc2p.MBO    \
          $(LIBDIR)/ed_arc3p.MBO    \
          $(LIBDIR)/ed_arcf.MBO     \
          $(LIBDIR)/ed_arco.MBO     \
          $(LIBDIR)/ed_bpl.MBO      \
          $(LIBDIR)/ed_cdm.MBO      \
          $(LIBDIR)/ed_csy.MBO      \
          $(LIBDIR)/ed_csy1.MBO     \
          $(LIBDIR)/ed_csy3.MBO     \
          $(LIBDIR)/ed_cuap.MBO     \
          $(LIBDIR)/ed_cuis.MBO     \
          $(LIBDIR)/ed_cur.MBO      \
          $(LIBDIR)/ed_curc.MBO     \
          $(LIBDIR)/ed_curi.MBO     \
          $(LIBDIR)/ed_curo.MBO     \
          $(LIBDIR)/ed_curs.MBO     \
          $(LIBDIR)/ed_cusi.MBO     \
          $(LIBDIR)/ed_cutr.MBO     \
          $(LIBDIR)/ed_hk.MBO       \
          $(LIBDIR)/ed_ldm.MBO      \
          $(LIBDIR)/ed_lin.MBO      \
          $(LIBDIR)/ed_lina.MBO     \
          $(LIBDIR)/ed_linf.MBO     \
          $(LIBDIR)/ed_lino.MBO     \
          $(LIBDIR)/ed_linp.MBO     \
          $(LIBDIR)/ed_lint1.MBO    \
          $(LIBDIR)/ed_lint2.MBO    \
          $(LIBDIR)/ed_poi.MBO      \
          $(LIBDIR)/ed_rdm.MBO      \
          $(LIBDIR)/ed_sur.MBO      \
          $(LIBDIR)/ed_surc.MBO     \
          $(LIBDIR)/ed_surcy.MBO    \
          $(LIBDIR)/ed_surex.MBO    \
          $(LIBDIR)/ed_surof.MBO    \
          $(LIBDIR)/ed_surro.MBO    \
          $(LIBDIR)/ed_surru.MBO    \
          $(LIBDIR)/ed_sursw.MBO    \
          $(LIBDIR)/ed_surtr.MBO    \
          $(LIBDIR)/ed_text.MBO     \
          $(LIBDIR)/ed_xht.MBO

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
	$(RM) $(LIBDIR)/ed*.MBO

##########################################################
