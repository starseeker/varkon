##########################################################
#
#   Makefile.GP
#   ----------
#
#  (C)Microform AB 1999-04-01 J.Kjellander
#
##########################################################

# Include files

INCFILES = ../include/GP.h

# The contents of GPlib.a

OBJECTS = ../lib/GPlib.a(gp1.o)        \
          ../lib/GPlib.a(gp2.o)        \
          ../lib/GPlib.a(gp3.o)        \
          ../lib/GPlib.a(gp4.o)        \
          ../lib/GPlib.a(gp5.o)        \
          ../lib/GPlib.a(gp6.o)        \
          ../lib/GPlib.a(gp8.o)        \
          ../lib/GPlib.a(gp9.o)        \
          ../lib/GPlib.a(gp10.o)       \
          ../lib/GPlib.a(gp11.o)       \
          ../lib/GPlib.a(gp12.o)       \
          ../lib/GPlib.a(gp13.o)       \
          ../lib/GPlib.a(gp14.o)       \
          ../lib/GPlib.a(gp15.o)       \
          ../lib/GPlib.a(gp16.o)       \
          ../lib/GPlib.a(gp17.o)       \
          ../lib/GPlib.a(gp18.o)       \
          ../lib/GPlib.a(gp19.o)       \
          ../lib/GPlib.a(gp20.o)       \
          ../lib/GPlib.a(gp22.o)       \
          ../lib/GPlib.a(gp23.o)       \
          ../lib/GPlib.a(gp24.o)       \
          ../lib/GPlib.a(gp25.o)       \
          ../lib/GPlib.a(gp26.o)       \
          ../lib/GPlib.a(gp27.o)       \
          ../lib/GPlib.a(gp28.o)       \
          ../lib/GPlib.a(gp29.o)

# GPlib.a depends on the objects. 

distr : $(OBJECTS)

debug : $(OBJECTS)

kdebug : $(OBJECTS)

# Objects depend on sources implicitly and sources depend
# on the following include-files.

$(OBJECTS) : $(INCFILES)

# gp6 also depends on font0.h

../lib/GPlib.a(gp6.o): $(INCFILES) ../include/font0.h

clean:
	rm ../lib/GPlib.a

##########################################################
