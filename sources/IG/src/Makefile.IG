##########################################################
#
#   Makefile for IGlib
#
#  (C)Microform AB 1999-02-15 J.Kjellander
#
##########################################################

INCFILES = ../include/IG.h

# The contents of IGlib.a

OBJECTS = ../lib/IGlib.a(varkon.o)      \
          ../lib/IGlib.a(ig1.o)        \
          ../lib/IGlib.a(ig2.o)        \
          ../lib/IGlib.a(ig3.o)        \
          ../lib/IGlib.a(ig4.o)        \
          ../lib/IGlib.a(ig5.o)        \
          ../lib/IGlib.a(ig6.o)        \
          ../lib/IGlib.a(ig7.o)        \
          ../lib/IGlib.a(ig8.o)        \
          ../lib/IGlib.a(ig9.o)        \
          ../lib/IGlib.a(ig10.o)       \
          ../lib/IGlib.a(ig11.o)       \
          ../lib/IGlib.a(ig12.o)       \
          ../lib/IGlib.a(ig13.o)       \
          ../lib/IGlib.a(ig14.o)       \
          ../lib/IGlib.a(ig15.o)       \
          ../lib/IGlib.a(ig16.o)       \
          ../lib/IGlib.a(ig17.o)       \
          ../lib/IGlib.a(ig18.o)       \
          ../lib/IGlib.a(ig19.o)       \
          ../lib/IGlib.a(ig20.o)       \
          ../lib/IGlib.a(ig21.o)       \
          ../lib/IGlib.a(ig22.o)       \
          ../lib/IGlib.a(ig23.o)       \
          ../lib/IGlib.a(ig24.o)       \
          ../lib/IGlib.a(ig25.o)       \
          ../lib/IGlib.a(ig26.o)       \
          ../lib/IGlib.a(ig27.o)       \
          ../lib/IGlib.a(ig28.o)       \
          ../lib/IGlib.a(ig29.o)       \
          ../lib/IGlib.a(ig30.o)       \
          ../lib/IGlib.a(ig31.o)       \
          ../lib/IGlib.a(ig32.o)       \
          ../lib/IGlib.a(ig33.o)       \
          ../lib/IGlib.a(ig34.o)       \
          ../lib/IGlib.a(ig35.o)       \
          ../lib/IGlib.a(ig36.o)       \
          ../lib/IGlib.a(ig37.o)       \
          ../lib/IGlib.a(ig38.o)      \
          ../lib/IGlib.a(ig39.o)

# IGlib.a depends on the objects. 

distr : $(OBJECTS)

debug : $(OBJECTS)

kdebug : $(OBJECTS)

# Objects depend on sources implicitly and sources depend
# on the following include-files.

$(OBJECTS) : $(INCFILES)

# ig1 also depends on futab.h

../lib/IGlib.a(ig1.o): $(INCFILES) ../include/futab.h

clean:
	rm ../lib/IGlib.a
