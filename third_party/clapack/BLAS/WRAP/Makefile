include ../../make.inc

#######################################################################
#  This is the makefile to create a wrapper for the CBLAS.
#######################################################################

all: libfblaswr.a libcblaswr.a

libcblaswr.a: cblaswr.o crotg.o zrotg.o
	$(ARCH) $(ARCHFLAGS) libcblaswr.a cblaswr.o crotg.o zrotg.o

libfblaswr.a: fblaswr.o crotg.o zrotg.o
	$(ARCH) $(ARCHFLAGS) libfblaswr.a fblaswr.o crotg.o zrotg.o

clean:
	rm -f *.o *.a

.c.o: 
	$(CC) $(CFLAGS) -c $*.c

