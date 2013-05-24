##########################################################
#
#   Platform independant Makefile for Varkon plotterdrivers
#
#  (C)2008-01-12 S.Larsson
#
##########################################################

# Include files

INCFILES = ../include/PL.h              \
           ../include/params.h          \
           ../../DB/include/DB.h        \
           ../../IG/include/IG.h

# The contents of PLlib.a

OBJECTS = ../lib/PLlib.a(pl1.o)         \
          ../lib/PLlib.a(pl2.o)         \
          ../lib/PLlib.a(pl3.o)         \
          ../lib/PLlib.a(pl4.o)         \
          ../lib/PLlib.a(pl5.o)         \
          ../lib/PLlib.a(pl6.o)

# Objects depend on sources implicitly and sources depend
# on the following include-files.

$(OBJECTS) : $(INCFILES)

# Each driver is a target

POSTSCRIPT = ../../../bin/postscript
HP7475     = ../../../bin/hp7475
HPCL5      = ../../../bin/hpcl5
HPGL2      = ../../../bin/hpgl2
PLLIB      = ../lib/PLlib.a

$(POSTSCRIPT) : postscript.c ../../IG/src/igenvpath.c $(PLLIB)
	$(COMPILE) postscript.c ../lib/PLlib.a ../../IG/src/igenvpath.c -lm -o ../../../bin/postscript
	strip ../../../bin/postscript

$(HP7475): hp7475.c  ../../IG/src/igenvpath.c $(PLLIB)
	$(COMPILE) hp7475.c     ../lib/PLlib.a ../../IG/src/igenvpath.c -lm -o ../../../bin/hp7475
	strip ../../../bin/hp7475

$(HPCL5):  hpcl5.c ../../IG/src/igenvpath.c $(PLLIB)
	$(COMPILE) hpcl5.c      ../lib/PLlib.a ../../IG/src/igenvpath.c -lm -o ../../../bin/hpcl5
	strip ../../../bin/hpcl5

$(HPGL2):  hpgl2.c ../../IG/src/igenvpath.c $(PLLIB)
	$(COMPILE) hpgl2.c      ../lib/PLlib.a ../../IG/src/igenvpath.c  -lm -o ../../../bin/hpgl2
	strip ../../../bin/hpgl2

# PLlib.a depends on the objects. Rule is implicit, no need to specify
$(PLLIB) : $(OBJECTS)


# Current build target transfered from parent makefile is one of these:

PLLIB : $(PLLIB)

POSTSCRIPT: $(POSTSCRIPT)

HP7475: $(HP7475)

HPCL5: $(HPCL5)

HPGL2: $(HPGL2)

clean:
	rm -f ../lib/PLlib.a

##########################################################
