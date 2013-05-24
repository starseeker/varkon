##########################################################
#
#   Makefile for the EX demo programs
#
#  (C)Microform AB 1999-03-01 J.Kjellander
#
##########################################################


# Options to the C-compiler

CFLAGS   = -ansi                          \
           -ffloat-store                  \
           -fsigned-char                  \
           -fwritable-strings             \
           -m486                          \
           -O                             \
           -DUNIX

# Libraries that the linker needs to know

LIBES    = ../lib/EXlib.a ../../DB/lib/DBlib.a ../../VU/lib/VUlib.a

# Programs to make

PROGRAMS = EXdemo1 EXdemo2

# Target

EXdemos: $(PROGRAMS)

# Dependency rules

EXdemo1 : EXdemo1.c ../lib/EXlib.a
	$(CC) $(CFLAGS) $< $(LIBES) -o $@

EXdemo2 : EXdemo2.c ../lib/EXlib.a
	$(CC) $(CFLAGS) $< $(LIBES) -o $@

clean:
	rm $(PROGRAMS) *.RIT
