#
#  Top Level Makefile for CLAPACK
#  Version 3.1.1 threadsafe version
#  October 2008
#

include make.inc

#all: lapack_install lib lapack_testing blas_testing 
all: f2clib lapack_install lib lapack_testing blas_testing 

#lib: lapacklib tmglib
#lib: f2clib lapacklib tmglib
lib: blaslib lapacklib tmglib

clean: cleanlib cleantesting 

lapack_install:
	( cd INSTALL; $(MAKE); ./testlsame; ./testslamch; \
	  ./testdlamch; ./testsecond; ./testdsecnd )

blaslib:
	( cd BLAS/SRC; $(MAKE) )

cblaswrap: blaswrap_copy
	( cd BLAS/WRAP; $(MAKE) libcblaswr.a; cp libcblaswr.a ../.. )

fblaswrap: blaswrap_copy
	( cd BLAS/WRAP; $(MAKE) libfblaswr.a; cp libfblaswr.a ../.. )

lapacklib:
	( cd SRC; $(MAKE) )

tmglib:
	( cd TESTING/MATGEN; $(MAKE) )

f2clib:
	( cd F2CLIBS/libf2c; $(MAKE) )

lapack_testing:
	( cd TESTING ; $(MAKE) )

blas_testing:
	( cd BLAS/TESTING; $(MAKE) -f Makeblat1 )
	( cd BLAS; ./xblat1s > sblat1.out; \
	           ./xblat1d > dblat1.out; \
	           ./xblat1c > cblat1.out; \
	           ./xblat1z > zblat1.out )

	( cd BLAS/TESTING; $(MAKE) -f Makeblat2 )
	( cd BLAS; ./xblat2s < sblat2.in ; \
	           ./xblat2d < dblat2.in ; \
	           ./xblat2c < cblat2.in ; \
	           ./xblat2z < zblat2.in )

	( cd BLAS/TESTING; $(MAKE) -f Makeblat3 )
	( cd BLAS; ./xblat3s < sblat3.in ; \
	           ./xblat3d < dblat3.in ; \
	           ./xblat3c < cblat3.in ; \
	           ./xblat3z < zblat3.in )

cleanlib:
	( cd INSTALL; $(MAKE) clean )
	( cd BLAS/SRC; $(MAKE) clean )
	( cd SRC; $(MAKE) clean )
	( cd TESTING/MATGEN; $(MAKE) clean )
	( cd F2CLIBS/libf2c; $(MAKE) clean )

cleantesting:
	( cd BLAS/TESTING ; rm -f *.o)
	( cd TESTING/LIN; $(MAKE) clean )
	( cd TESTING/EIG; $(MAKE) clean )
	( cd TESTING; rm -f xlin* xeig* )

cleanall: clean
	rm -f *.a BLAS/WRAP/*.a F2CLIBS/*.a INSTALL/test* BLAS/x* BLAS/*.out

blaswrap_copy:
	cp BLAS/WRAP/blaswrap.h SRC
	cp BLAS/WRAP/blaswrap.h BLAS/SRC
	cp BLAS/WRAP/blaswrap.h BLAS/TESTING
	cp BLAS/WRAP/blaswrap.h INSTALL
	cp BLAS/WRAP/blaswrap.h TESTING/LIN
	cp BLAS/WRAP/blaswrap.h TESTING/EIG
	cp BLAS/WRAP/blaswrap.h TESTING/MATGEN

