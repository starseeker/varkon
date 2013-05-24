##########################################################
#
#   Makefile for IGES import.
#
#  (C)2007-03-03 J.Kjellander
#
##########################################################

RM = rm -f

COMPILE = ../../../bin/mbsc

LIBDIR = ../../../lib

# MBO-files

OBJECTS = $(LIBDIR)/igs_0.MBO         \
          $(LIBDIR)/igs_100.MBO       \
          $(LIBDIR)/igs_102.MBO       \
          $(LIBDIR)/igs_104.MBO       \
          $(LIBDIR)/igs_106.MBO       \
          $(LIBDIR)/igs_108.MBO       \
          $(LIBDIR)/igs_110.MBO       \
          $(LIBDIR)/igs_112.MBO       \
          $(LIBDIR)/igs_116.MBO       \
          $(LIBDIR)/igs_120.MBO       \
          $(LIBDIR)/igs_122.MBO       \
          $(LIBDIR)/igs_124.MBO       \
          $(LIBDIR)/igs_126.MBO       \
          $(LIBDIR)/igs_128.MBO       \
          $(LIBDIR)/igs_142.MBO       \
          $(LIBDIR)/igs_144.MBO       \
          $(LIBDIR)/igs_curve.MBO     \
          $(LIBDIR)/igs_dsec.MBO      \
          $(LIBDIR)/igs_ell.MBO       \
          $(LIBDIR)/igs_ents.MBO      \
          $(LIBDIR)/igs_fnam.MBO      \
          $(LIBDIR)/igs_gflt_gsec.MBO \
          $(LIBDIR)/igs_gflt.MBO      \
          $(LIBDIR)/igs_gint_gsec.MBO \
          $(LIBDIR)/igs_gint.MBO      \
          $(LIBDIR)/igs_gsec.MBO      \
          $(LIBDIR)/igs_gstr.MBO      \
          $(LIBDIR)/igs_in.MBO        \
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
