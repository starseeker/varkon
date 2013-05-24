##########################################################
#
#   Platform independant Makefile for Varkon plotterdrivers
#
#  (C)Microform AB 1999-02-22 J.Kjellander
#  2006-09-21 Sören Larsson, Örebro University
#             Changed, now not dependet of VARKON_ROOT 
#
##########################################################

# Include files

INCFILES = ../include/PL.h              \
           ../include/params.h          \
           ../../DB/include/DB.h        \
           ../../IG/include/IG.h        \
           
# The contents of PLlib.a

OBJECTS = ../lib/PLlib.a(pl1.o)         \
          ../lib/PLlib.a(pl2.o)         \
          ../lib/PLlib.a(pl3.o)         \
          ../lib/PLlib.a(pl4.o)         \
          ../lib/PLlib.a(pl5.o)         \
          ../lib/PLlib.a(pl6.o)

# Targets
#POSTSCRIPT = $(VARKON_ROOT)/bin/postscript
POSTSCRIPT = ../../../bin/postscript
#EPSON      = $(VARKON_ROOT)/bin/epson
EPSON      = ../../../bin/epson
#IBMPRO     = $(VARKON_ROOT)/bin/ibmpro
IBMPRO     = ../../../bin/ibmpro
#HP7475     = $(VARKON_ROOT)/bin/hp7475
HP7475     = ../../../bin/hp7475
#HPCL5      = $(VARKON_ROOT)/bin/hpcl5
HPCL5      = ../../../bin/hpcl5
#HPGL2      = $(VARKON_ROOT)/bin/hpgl2
HPGL2      = ../../../bin/hpgl2
#GERBER     = $(VARKON_ROOT)/bin/gerber        
GERBER     = ../../../bin/gerber

# PLlib.a depends on the objects. 
PLlib : $(OBJECTS)


# Objects depend on sources implicitly and sources depend
# on the following include-files.

$(OBJECTS) : $(INCFILES)


# Each driver is a target

$(POSTSCRIPT): postscript.c  $(OBJECTS)  $(INCFILES)
	$(COMPILE) postscript.c ../lib/PLlib.a -lm ../../IG/src/igenvpath.c -o $(POSTSCRIPT)
	strip $(POSTSCRIPT)

$(EPSON) : epson.c  $(OBJECTS) $(INCFILES)
	$(COMPILE) epson.c ../lib/PLlib.a -lm ../../IG/src/igenvpath.c -o $(EPSON)
	strip $(EPSON)

$(IBMPRO) : ibmpro.c  $(OBJECTS) $(INCFILES)
	$(COMPILE) ibmpro.c  ../lib/PLlib.a -lm ../../IG/src/igenvpath.c  ../lib/PLlib.a -lm -o $(IBMPRO)
	strip $(IBMPRO)

$(HP7475) : hp7475.c  $(OBJECTS) $(INCFILES)
	$(COMPILE) hp7475.c  ../lib/PLlib.a -lm ../../IG/src/igenvpath.c  ../lib/PLlib.a -lm -o $(HP7475)
	strip $(HP7475)

$(HPCL5)  : hpcl5.c $(OBJECTS) $(INCFILES)
	$(COMPILE) hpcl5.c  ../lib/PLlib.a -lm ../../IG/src/igenvpath.c  ../lib/PLlib.a -lm -o $(HPCL5)
	strip $(HPCL5)

$(HPGL2)  : hpgl2.c  $(OBJECTS) $(INCFILES)
	$(COMPILE) hpgl2.c  ../lib/PLlib.a -lm ../../IG/src/igenvpath.c  ../lib/PLlib.a -lm -o $(HPGL2)
	strip $(HPGL2)

$(GERBER) : gerber.c  $(OBJECTS) $(INCFILES)
	$(COMPILE) gerber.c  ../lib/PLlib.a -lm ../../IG/src/igenvpath.c  ../lib/PLlib.a -lm -o $(GERBER)
	strip $(GERBER)

  
all: $(POSTSCRIPT) $(EPSON) $(IBMPRO) $(HP7475) $(HPCL5) $(HPGL2) $(GERBER)


clean:
	rm -f ../lib/PLlib.a
#	rm -f $(VARKON_ROOT)/bin/postscript
#	rm -f $(VARKON_ROOT)/bin/epson
#	rm -f $(VARKON_ROOT)/bin/ibmpro
#	rm -f $(VARKON_ROOT)/bin/hp7475
#	rm -f $(VARKON_ROOT)/bin/hpcl5
#	rm -f $(VARKON_ROOT)/bin/hpgl2
#	rm -f $(VARKON_ROOT)/bin/gerber
