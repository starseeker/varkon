##########################################################
#
#   Makefile for the PM demo program
#
#  (C)2007-01-18 J.Kjellander
#
##########################################################


# Options to the C-compiler

CFLAGS   = -ansi                          \
           -ffloat-store                  \
           -fsigned-char                  \
           -DUNIX

# Libraries that the linker needs to know

LIBS    = ../../VU/lib/VUlib.a

# Includefiles

INCFILES = ../include/my_evfuncs.h ../include/my_formpar.h \
           ../include/my_mbsrout.h ../include/my_newrout.h

# Object files that the linker needs to know

OBJS    = pmac1.o pmac2.o pmallo.o stacc.o inacc.o \
          inac10.o inrts.o innpar.o inproc.o evpart.o \
          inac13.o ingeop.o inac17.o infunc.o ingpar.o \
          inac20.o stac10.o ex2.o anlog.o igenvpath.o \
          anascan.o anpar31.o anpar51.o anpar52.o \
          annaml.o anpar12.o anpar11.o evset.o

# Programs to make

PROGRAMS = PMdemo

# Target

PMdemos: $(PROGRAMS)

# Dependency rules

PMdemo : PMdemo.c pmac1.o pmac2.o pmallo.o stacc.o inacc.o \
          inac10.o inrts.o innpar.o infunc.o ingeop.o inproc.o \
          evpart.o inac13.o inac17.o ingpar.o inac20.o stac10.o \
          ex2.o anlog.o anascan.o anpar31.o anpar51.o igenvpath.o  \
          anpar52.o annaml.o anpar12.o anpar11.o evset.o
	$(CC) $(CFLAGS) $< $(OBJS) $(LIBS) -lm -o $@

pmac1.o : ../src/pmac1.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/pmac1.c

pmac2.o : ../src/pmac2.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/pmac2.c

pmallo.o : ../src/pmallo.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/pmallo.c

stacc.o : ../src/stacc.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/stacc.c

stac10.o : ../src/stac10.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/stac10.c

inacc.o : ../src/inacc.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/inacc.c

inac10.o : ../src/inac10.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/inac10.c

evpart.o : ../src/evpart.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/evpart.c

evset.o : ../src/evset.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/evset.c

inac13.o : ../src/inac13.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/inac13.c

inac17.o : ../src/inac17.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/inac17.c

inac20.o : ../src/inac20.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/inac20.c

inrts.o : ../src/inrts.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/inrts.c

innpar.o : ../src/innpar.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/innpar.c

infunc.o : ../src/infunc.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/infunc.c

ingeop.o : ../src/ingeop.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/ingeop.c

inproc.o : ../src/inproc.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/inproc.c

ingpar.o : ../src/ingpar.c $(INCFILES)
	$(CC) -c $(CFLAGS) ../src/ingpar.c

ex2.o  : ex2.c
	$(CC) -c $(CFLAGS) ex2.c

anlog.o : ../../AN/src/anlog.c
	$(CC) -c $(CFLAGS) ../../AN/src/anlog.c

anascan.o : ../../AN/src/anascan.c
	$(CC) -c $(CFLAGS) ../../AN/src/anascan.c

anpar11.o : ../../AN/src/anpar11.c
	$(CC) -c $(CFLAGS) ../../AN/src/anpar11.c

anpar12.o : ../../AN/src/anpar12.c
	$(CC) -c $(CFLAGS) ../../AN/src/anpar12.c

anpar31.o : ../../AN/src/anpar31.c
	$(CC) -c $(CFLAGS) ../../AN/src/anpar31.c

anpar51.o : ../../AN/src/anpar51.c
	$(CC) -c $(CFLAGS) ../../AN/src/anpar51.c

anpar52.o : ../../AN/src/anpar52.c
	$(CC) -c $(CFLAGS) ../../AN/src/anpar52.c

annaml.o : ../../AN/src/annaml.c
	$(CC) -c $(CFLAGS) ../../AN/src/annaml.c

igenvpath.o : ../../IG/src/igenvpath.c
	$(CC) -c $(CFLAGS) ../../IG/src/igenvpath.c

clean:
	rm $(PROGRAMS) *.o
