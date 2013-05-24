##########################################################
#
#   Makefile.EX
#   -----------
#
#  Make rules for the EX module
#
#  (C)Microform AB 1999-02-22 J.Kjellander
#
##########################################################

INCFILES = ../include/EX.h              \
           ../../DB/include/DB.h

OBJECTS = ../lib/EXlib.a(ex1.o)         \
          ../lib/EXlib.a(ex2.o)         \
          ../lib/EXlib.a(ex4.o)         \
          ../lib/EXlib.a(ex5.o)         \
          ../lib/EXlib.a(ex6.o)         \
          ../lib/EXlib.a(ex7.o)         \
          ../lib/EXlib.a(ex8.o)         \
          ../lib/EXlib.a(ex9.o)         \
          ../lib/EXlib.a(ex10.o)        \
          ../lib/EXlib.a(ex11.o)        \
          ../lib/EXlib.a(ex12.o)        \
          ../lib/EXlib.a(ex13.o)        \
          ../lib/EXlib.a(ex15.o)        \
          ../lib/EXlib.a(ex16.o)        \
          ../lib/EXlib.a(ex17.o)        \
          ../lib/EXlib.a(ex18.o)        \
          ../lib/EXlib.a(ex19.o)        \
          ../lib/EXlib.a(ex20.o)        \
          ../lib/EXlib.a(ex21.o)        \
          ../lib/EXlib.a(ex22.o)        \
          ../lib/EXlib.a(ex23.o)        \
          ../lib/EXlib.a(ex24.o)        \
          ../lib/EXlib.a(ex25.o)        \
          ../lib/EXlib.a(ex26.o)        \
          ../lib/EXlib.a(ex27.o)        \
          ../lib/EXlib.a(ex28.o)        \
          ../lib/EXlib.a(expoint.o)     \
          ../lib/EXlib.a(excurve.o)     \
          ../lib/EXlib.a(extcp.o)       \
          ../lib/EXlib.a(extime.o)      \
          ../lib/EXlib.a(exos.o)        \
          ../lib/EXlib.a(excugl.o)      \
          ../lib/EXlib.a(exsugl.o)      \
          ifeq ($(DV3RAP),TRUE)         \
          ../lib/EXlib.a(rap_clnt.o)    \
          ../lib/EXlib.a(rap_xdr.o)     \
          endif                         \
          ../lib/EXlib.a(exrap.o)       \
          ../lib/EXlib.a(exoru.o)       \
          ../lib/EXlib.a(exmesh.o)

# EXlib.a depends on the objects. 

distr : $(OBJECTS)

debug : $(OBJECTS)

clean:
	rm ../lib/EXlib.a


# Objects depend on sources implicitly and sources depend
# on the following include-files.

$(OBJECTS) : $(INCFILES)
 

# Special rules needed for RAP-RPC
#-----------------------------------
ifeq ($(DV3RAP),TRUE)

rap.h rap_clnt.c rap_xdr.c: rap.x
	$(RPCGENERATE) rap.x
	rm -f rap_svc.c

../lib/EXlib.a(rap_clnt.o): rap_clnt.c rap.h
	$(CC) -c $(RAPCFLAGS) rap_clnt.c
	ar r ../lib/EXlib.a rap_clnt.o
	$(RM) rap_clnt.o

../lib/EXlib.a(rap_xdr.o): rap_xdr.c rap.h
	$(CC) -c $(RAPCFLAGS)  rap_xdr.c
	ar r ../lib/EXlib.a rap_xdr.o
	$(RM) rap_xdr.o

../lib/EXlib.a(exrap.o): exrap.c rap.h $(INCFILES)
	$(CC) -c $(RAPCFLAGS) -DV3RAP exrap.c
	ar r ../lib/EXlib.a exrap.o
	$(RM) exrap.o

endif



# Special rules for ORU specific routines
#------------------------------------------
        
ifeq ($(DV3ORU),TRUE)

../lib/EXlib.a(exoru.o): exoru.c $(INCFILES)
	$(CC) -c $(CFLAGS) -DV3ORU exoru.c
	ar r ../lib/EXlib.a exoru.o
	$(RM) exoru.o
 
endif  



