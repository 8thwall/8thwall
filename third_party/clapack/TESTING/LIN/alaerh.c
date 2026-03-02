#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__5 = 5;

/* Subroutine */ int alaerh_(char *path, char *subnam, integer *info, integer 
	*infoe, char *opts, integer *m, integer *n, integer *kl, integer *ku, 
	integer *n5, integer *imat, integer *nfail, integer *nerrs, integer *
	nout)
{
    /* Format strings */
    static char fmt_9988[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> M =\002,i5,\002, N "
	    "=\002,i5,\002, NB =\002,i4,\002, type \002,i2)";
    static char fmt_9975[] = "(\002 *** Error code from \002,a6,\002=\002,"
	    "i5,\002 for M=\002,i5,\002, N=\002,i5,\002, NB=\002,i4,\002, typ"
	    "e \002,i2)";
    static char fmt_9949[] = "(\002 ==> Doing only the condition estimate fo"
	    "r this case\002)";
    static char fmt_9984[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> N =\002,i5,\002, NRHS ="
	    "\002,i4,\002, type \002,i2)";
    static char fmt_9970[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,\002 for N =\002,i5,\002, NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9992[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> FACT='\002,a1,\002', T"
	    "RANS='\002,a1,\002', N =\002,i5,\002, NRHS =\002,i4,\002, type"
	    " \002,i2)";
    static char fmt_9997[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> FACT='\002,a1,\002', TRANS='\002,a1,\002', N =\002,i"
	    "5,\002, NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9971[] = "(\002 *** Error code from \002,a6,\002=\002,"
	    "i5,\002 for N=\002,i5,\002, NB=\002,i4,\002, type \002,i2)";
    static char fmt_9978[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,\002 for M =\002,i5,\002, N =\002,i5,\002, type \002,i2)";
    static char fmt_9969[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,\002 for NORM = '\002,a1,\002', N =\002,i5,\002, type \002,i2)";
    static char fmt_9965[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> TRANS = '\002,a1,\002', M =\002,i5,\002, N =\002,i5"
	    ",\002, NRHS =\002,i4,\002, NB =\002,i4,\002, type \002,i2)";
    static char fmt_9974[] = "(\002 *** Error code from \002,a6,\002=\002,i5"
	    ",/\002 ==> M =\002,i5,\002, N =\002,i5,\002, NRHS =\002,i4,\002,"
	    " NB =\002,i4,\002, type \002,i2)";
    static char fmt_9963[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> TRANS = '\002,a1,\002', N =\002,i5,\002, NRHS =\002,"
	    "i4,\002, type \002,i2)";
    static char fmt_9989[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> M = \002,i5,\002, N "
	    "=\002,i5,\002, KL =\002,i5,\002, KU =\002,i5,\002, NB =\002,i4"
	    ",\002, type \002,i2)";
    static char fmt_9976[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> M = \002,i5,\002, N =\002,i5,\002, KL =\002,i5,\002,"
	    " KU =\002,i5,\002, NB =\002,i4,\002, type \002,i2)";
    static char fmt_9986[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> N =\002,i5,\002, KL "
	    "=\002,i5,\002, KU =\002,i5,\002, NRHS =\002,i4,\002, type \002,i"
	    "2)";
    static char fmt_9972[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> N =\002,i5,\002, KL =\002,i5,\002, KU =\002,i5,\002,"
	    " NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9993[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> FACT='\002,a1,\002', T"
	    "RANS='\002,a1,\002', N=\002,i5,\002, KL=\002,i5,\002, KU=\002,i5,"
	    "\002, NRHS=\002,i4,\002, type \002,i1)";
    static char fmt_9998[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> FACT='\002,a1,\002', TRANS='\002,a1,\002', N=\002,i5,"
	    "\002, KL=\002,i5,\002, KU=\002,i5,\002, NRHS=\002,i4,\002, type"
	    " \002,i1)";
    static char fmt_9977[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> M = \002,i5,\002, N =\002,i5,\002, KL =\002,i5,\002,"
	    " KU =\002,i5,\002, type \002,i2)";
    static char fmt_9968[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> NORM ='\002,a1,\002', N =\002,i5,\002, KL =\002,i5"
	    ",\002, KU =\002,i5,\002, type \002,i2)";
    static char fmt_9964[] = "(\002 *** Error code from \002,a6,\002=\002,i5"
	    ",/\002 ==> TRANS='\002,a1,\002', N =\002,i5,\002, KL =\002,i5"
	    ",\002, KU =\002,i5,\002, NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9987[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,\002 for N=\002,i5,\002, type"
	    " \002,i2)";
    static char fmt_9973[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,\002 for N =\002,i5,\002, type \002,i2)";
    static char fmt_9980[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> UPLO = '\002,a1,\002',"
	    " N =\002,i5,\002, NB =\002,i4,\002, type \002,i2)";
    static char fmt_9956[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO = '\002,a1,\002', N =\002,i5,\002, NB =\002,i4"
	    ",\002, type \002,i2)";
    static char fmt_9979[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> UPLO = '\002,a1,\002',"
	    " N =\002,i5,\002, NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9955[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO = '\002,a1,\002', N =\002,i5,\002, NRHS =\002,i"
	    "4,\002, type \002,i2)";
    static char fmt_9990[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> FACT='\002,a1,\002', U"
	    "PLO='\002,a1,\002', N =\002,i5,\002, NRHS =\002,i4,\002, type"
	    " \002,i2)";
    static char fmt_9995[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> FACT='\002,a1,\002', UPLO='\002,a1,\002', N =\002,i5,"
	    "\002, NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9960[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,\002 for UPLO = '\002,a1,\002', N =\002,i5,\002, type \002,i2)";
    static char fmt_9983[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> UPLO = '\002,a1,\002',"
	    " N =\002,i5,\002, type \002,i2)";
    static char fmt_9982[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> UPLO = '\002,a1,\002',"
	    " N =\002,i5,\002, KD =\002,i5,\002, NB =\002,i4,\002, type \002,"
	    "i2)";
    static char fmt_9958[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO = '\002,a1,\002', N =\002,i5,\002, KD =\002,i5"
	    ",\002, NB =\002,i4,\002, type \002,i2)";
    static char fmt_9981[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> UPLO='\002,a1,\002', N"
	    " =\002,i5,\002, KD =\002,i5,\002, NRHS =\002,i4,\002, type \002,"
	    "i2)";
    static char fmt_9957[] = "(\002 *** Error code from \002,a6,\002=\002,i5"
	    ",/\002 ==> UPLO = '\002,a1,\002', N =\002,i5,\002, KD =\002,i5"
	    ",\002, NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9991[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> FACT='\002,a1,\002', U"
	    "PLO='\002,a1,\002', N=\002,i5,\002, KD=\002,i5,\002, NRHS=\002,i"
	    "4,\002, type \002,i2)";
    static char fmt_9996[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> FACT='\002,a1,\002', UPLO='\002,a1,\002', N=\002,i5"
	    ",\002, KD=\002,i5,\002, NRHS=\002,i4,\002, type \002,i2)";
    static char fmt_9959[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO = '\002,a1,\002', N =\002,i5,\002, KD =\002,i5"
	    ",\002, type \002,i2)";
    static char fmt_9994[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> FACT='\002,a1,\002', N"
	    " =\002,i5,\002, NRHS =\002,i4,\002, type \002,i2)";
    static char fmt_9999[] = "(\002 *** Error code from \002,a6,\002=\002,"
	    "i5,\002, FACT='\002,a1,\002', N=\002,i5,\002, NRHS=\002,i4,\002,"
	    " type \002,i2)";
    static char fmt_9961[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO='\002,a1,\002', DIAG ='\002,a1,\002', N =\002,i"
	    "5,\002, NB =\002,i4,\002, type \002,i2)";
    static char fmt_9967[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> NORM='\002,a1,\002', UPLO ='\002,a1,\002', DIAG='"
	    "\002,a1,\002', N =\002,i5,\002, type \002,i2)";
    static char fmt_9952[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO='\002,a1,\002', TRANS='\002,a1,\002', DIAG='"
	    "\002,a1,\002', NORMIN='\002,a1,\002', N =\002,i5,\002, type \002"
	    ",i2)";
    static char fmt_9953[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO='\002,a1,\002', TRANS='\002,a1,\002', DIAG='"
	    "\002,a1,\002', N =\002,i5,\002, NRHS =\002,i4,\002, type \002,i2)"
	    ;
    static char fmt_9962[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO='\002,a1,\002', DIAG ='\002,a1,\002', N =\002,i"
	    "5,\002, type \002,i2)";
    static char fmt_9966[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> NORM='\002,a1,\002', UPLO ='\002,a1,\002', DIAG='"
	    "\002,a1,\002', N=\002,i5,\002, KD=\002,i5,\002, type \002,i2)";
    static char fmt_9951[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO='\002,a1,\002', TRANS='\002,a1,\002', DIAG='"
	    "\002,a1,\002', NORMIN='\002,a1,\002', N=\002,i5,\002, KD=\002,i5,"
	    "\002, type \002,i2)";
    static char fmt_9954[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5,/\002 ==> UPLO='\002,a1,\002', TRANS='\002,a1,\002', DIAG='"
	    "\002,a1,\002', N=\002,i5,\002, KD=\002,i5,\002, NRHS=\002,i4,"
	    "\002, type \002,i2)";
    static char fmt_9985[] = "(\002 *** \002,a6,\002 returned with INFO ="
	    "\002,i5,\002 instead of \002,i2,/\002 ==> N =\002,i5,\002, NB "
	    "=\002,i4,\002, type \002,i2)";
    static char fmt_9950[] = "(\002 *** Error code from \002,a6,\002 =\002,i"
	    "5)";

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    char c3[3], p2[2], uplo[1];
    extern /* Subroutine */ int alahd_(integer *, char *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int aladhd_(integer *, char *);
    extern logical lsamen_(integer *, char *, char *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___4 = { 0, 0, 0, fmt_9975, 0 };
    static cilist io___5 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___6 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___7 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___8 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___9 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___10 = { 0, 0, 0, fmt_9971, 0 };
    static cilist io___11 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_9969, 0 };
    static cilist io___13 = { 0, 0, 0, fmt_9965, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_9963, 0 };
    static cilist io___16 = { 0, 0, 0, fmt_9989, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9986, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___21 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9968, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9964, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9969, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9963, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9990, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9990, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9983, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9990, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9982, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___71 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_9969, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_9963, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___84 = { 0, 0, 0, fmt_9967, 0 };
    static cilist io___85 = { 0, 0, 0, fmt_9952, 0 };
    static cilist io___86 = { 0, 0, 0, fmt_9953, 0 };
    static cilist io___87 = { 0, 0, 0, fmt_9962, 0 };
    static cilist io___88 = { 0, 0, 0, fmt_9967, 0 };
    static cilist io___89 = { 0, 0, 0, fmt_9952, 0 };
    static cilist io___90 = { 0, 0, 0, fmt_9953, 0 };
    static cilist io___91 = { 0, 0, 0, fmt_9966, 0 };
    static cilist io___92 = { 0, 0, 0, fmt_9951, 0 };
    static cilist io___93 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___94 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___95 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___96 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___97 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___98 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___99 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___100 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___101 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___102 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___103 = { 0, 0, 0, fmt_9975, 0 };
    static cilist io___104 = { 0, 0, 0, fmt_9985, 0 };
    static cilist io___105 = { 0, 0, 0, fmt_9971, 0 };
    static cilist io___106 = { 0, 0, 0, fmt_9950, 0 };



/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ALAERH is an error handler for the LAPACK routines.  It prints the */
/*  header if this is the first error message and prints the error code */
/*  and form of recovery, if any.  The character evaluations in this */
/*  routine may make it slow, but it should not be called once the LAPACK */
/*  routines are fully debugged. */

/*  Arguments */
/*  ========= */

/*  PATH    (input) CHARACTER*3 */
/*          The LAPACK path name of subroutine SUBNAM. */

/*  SUBNAM  (input) CHARACTER*6 */
/*          The name of the subroutine that returned an error code. */

/*  INFO    (input) INTEGER */
/*          The error code returned from routine SUBNAM. */

/*  INFOE   (input) INTEGER */
/*          The expected error code from routine SUBNAM, if SUBNAM were */
/*          error-free.  If INFOE = 0, an error message is printed, but */
/*          if INFOE.NE.0, we assume only the return code INFO is wrong. */

/*  OPTS    (input) CHARACTER*(*) */
/*          The character options to the subroutine SUBNAM, concatenated */
/*          into a single character string.  For example, UPLO = 'U', */
/*          TRANS = 'T', and DIAG = 'N' for a triangular routine would */
/*          be specified as OPTS = 'UTN'. */

/*  M       (input) INTEGER */
/*          The matrix row dimension. */

/*  N       (input) INTEGER */
/*          The matrix column dimension.  Accessed only if PATH = xGE or */
/*          xGB. */

/*  KL      (input) INTEGER */
/*          The number of sub-diagonals of the matrix.  Accessed only if */
/*          PATH = xGB, xPB, or xTB.  Also used for NRHS for PATH = xLS. */

/*  KU      (input) INTEGER */
/*          The number of super-diagonals of the matrix.  Accessed only */
/*          if PATH = xGB. */

/*  N5      (input) INTEGER */
/*          A fifth integer parameter, may be the blocksize NB or the */
/*          number of right hand sides NRHS. */

/*  IMAT    (input) INTEGER */
/*          The matrix type. */

/*  NFAIL   (input) INTEGER */
/*          The number of prior tests that did not pass the threshold; */
/*          used to determine if the header should be printed. */

/*  NERRS   (input/output) INTEGER */
/*          On entry, the number of errors already detected; used to */
/*          determine if the header should be printed. */
/*          On exit, NERRS is increased by 1. */

/*  NOUT    (input) INTEGER */
/*          The unit number on which results are to be printed. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    if (*info == 0) {
	return 0;
    }
    s_copy(p2, path + 1, (ftnlen)2, (ftnlen)2);
    s_copy(c3, subnam + 3, (ftnlen)3, (ftnlen)3);

/*     Print the header if this is the first error message. */

    if (*nfail == 0 && *nerrs == 0) {
	if (lsamen_(&c__3, c3, "SV ") || lsamen_(&c__3, 
		c3, "SVX")) {
	    aladhd_(nout, path);
	} else {
	    alahd_(nout, path);
	}
    }
    ++(*nerrs);

/*     Print the message detailing the error and form of recovery, */
/*     if any. */

    if (lsamen_(&c__2, p2, "GE")) {

/*        xGE:  General matrices */

	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___3.ciunit = *nout;
		s_wsfe(&io___3);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___4.ciunit = *nout;
		s_wsfe(&io___4);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___5.ciunit = *nout;
		s_wsfe(&io___5);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___6.ciunit = *nout;
		s_wsfe(&io___6);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___7.ciunit = *nout;
		s_wsfe(&io___7);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___8.ciunit = *nout;
		s_wsfe(&io___8);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___9.ciunit = *nout;
		s_wsfe(&io___9);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "TRI")) {

	    io___10.ciunit = *nout;
	    s_wsfe(&io___10);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else if (lsamen_(&c__5, subnam + 1, "LATMS")) 
		{

	    io___11.ciunit = *nout;
	    s_wsfe(&io___11);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else if (lsamen_(&c__3, c3, "CON")) {

	    io___12.ciunit = *nout;
	    s_wsfe(&io___12);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else if (lsamen_(&c__3, c3, "LS ")) {

	    io___13.ciunit = *nout;
	    s_wsfe(&io___13);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else if (lsamen_(&c__3, c3, "LSX") || lsamen_(
		&c__3, c3, "LSS")) {

	    io___14.ciunit = *nout;
	    s_wsfe(&io___14);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else {

	    io___15.ciunit = *nout;
	    s_wsfe(&io___15);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "GB")) {

/*        xGB:  General band matrices */

	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___16.ciunit = *nout;
		s_wsfe(&io___16);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___17.ciunit = *nout;
		s_wsfe(&io___17);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___18.ciunit = *nout;
		s_wsfe(&io___18);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___19.ciunit = *nout;
		s_wsfe(&io___19);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___20.ciunit = *nout;
		s_wsfe(&io___20);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___21.ciunit = *nout;
		s_wsfe(&io___21);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___22.ciunit = *nout;
		s_wsfe(&io___22);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__5, subnam + 1, "LATMS")) 
		{

	    io___23.ciunit = *nout;
	    s_wsfe(&io___23);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else if (lsamen_(&c__3, c3, "CON")) {

	    io___24.ciunit = *nout;
	    s_wsfe(&io___24);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else {

	    io___25.ciunit = *nout;
	    s_wsfe(&io___25);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*ku), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "GT")) {

/*        xGT:  General tridiagonal matrices */

	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___26.ciunit = *nout;
		s_wsfe(&io___26);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___27.ciunit = *nout;
		s_wsfe(&io___27);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___28.ciunit = *nout;
		s_wsfe(&io___28);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___29.ciunit = *nout;
		s_wsfe(&io___29);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___30.ciunit = *nout;
		s_wsfe(&io___30);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___31.ciunit = *nout;
		s_wsfe(&io___31);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___32.ciunit = *nout;
		s_wsfe(&io___32);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "CON")) {

	    io___33.ciunit = *nout;
	    s_wsfe(&io___33);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else {

	    io___34.ciunit = *nout;
	    s_wsfe(&io___34);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "PO")) {

/*        xPO:  Symmetric or Hermitian positive definite matrices */

	*(unsigned char *)uplo = *(unsigned char *)opts;
	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___36.ciunit = *nout;
		s_wsfe(&io___36);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___37.ciunit = *nout;
		s_wsfe(&io___37);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___38.ciunit = *nout;
		s_wsfe(&io___38);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___39.ciunit = *nout;
		s_wsfe(&io___39);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___40.ciunit = *nout;
		s_wsfe(&io___40);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___41.ciunit = *nout;
		s_wsfe(&io___41);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___42.ciunit = *nout;
		s_wsfe(&io___42);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "TRI")) {

	    io___43.ciunit = *nout;
	    s_wsfe(&io___43);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else if (lsamen_(&c__5, subnam + 1, "LATMS") 
		|| lsamen_(&c__3, c3, "CON")) {

	    io___44.ciunit = *nout;
	    s_wsfe(&io___44);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else {

	    io___45.ciunit = *nout;
	    s_wsfe(&io___45);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "SY") || lsamen_(&
	    c__2, p2, "HE")) {

/*        xHE, or xSY:  Symmetric or Hermitian indefinite matrices */

	*(unsigned char *)uplo = *(unsigned char *)opts;
	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___46.ciunit = *nout;
		s_wsfe(&io___46);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___47.ciunit = *nout;
		s_wsfe(&io___47);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___48.ciunit = *nout;
		s_wsfe(&io___48);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___49.ciunit = *nout;
		s_wsfe(&io___49);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___50.ciunit = *nout;
		s_wsfe(&io___50);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___51.ciunit = *nout;
		s_wsfe(&io___51);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___52.ciunit = *nout;
		s_wsfe(&io___52);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__5, subnam + 1, "LATMS") 
		|| lsamen_(&c__3, c3, "TRI") || lsamen_(
		&c__3, c3, "CON")) {

	    io___53.ciunit = *nout;
	    s_wsfe(&io___53);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else {

	    io___54.ciunit = *nout;
	    s_wsfe(&io___54);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "PP") || lsamen_(&
	    c__2, p2, "SP") || lsamen_(&c__2, p2, "HP")) {

/*        xPP, xHP, or xSP:  Symmetric or Hermitian packed matrices */

	*(unsigned char *)uplo = *(unsigned char *)opts;
	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___55.ciunit = *nout;
		s_wsfe(&io___55);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___56.ciunit = *nout;
		s_wsfe(&io___56);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___57.ciunit = *nout;
		s_wsfe(&io___57);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___58.ciunit = *nout;
		s_wsfe(&io___58);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___59.ciunit = *nout;
		s_wsfe(&io___59);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___60.ciunit = *nout;
		s_wsfe(&io___60);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___61.ciunit = *nout;
		s_wsfe(&io___61);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__5, subnam + 1, "LATMS") 
		|| lsamen_(&c__3, c3, "TRI") || lsamen_(
		&c__3, c3, "CON")) {

	    io___62.ciunit = *nout;
	    s_wsfe(&io___62);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else {

	    io___63.ciunit = *nout;
	    s_wsfe(&io___63);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "PB")) {

/*        xPB:  Symmetric (Hermitian) positive definite band matrix */

	*(unsigned char *)uplo = *(unsigned char *)opts;
	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___64.ciunit = *nout;
		s_wsfe(&io___64);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___65.ciunit = *nout;
		s_wsfe(&io___65);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___66.ciunit = *nout;
		s_wsfe(&io___66);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___67.ciunit = *nout;
		s_wsfe(&io___67);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___68.ciunit = *nout;
		s_wsfe(&io___68);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, uplo, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___69.ciunit = *nout;
		s_wsfe(&io___69);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___70.ciunit = *nout;
		s_wsfe(&io___70);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, opts + 1, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__5, subnam + 1, "LATMS") 
		|| lsamen_(&c__3, c3, "CON")) {

	    io___71.ciunit = *nout;
	    s_wsfe(&io___71);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();

	} else {

	    io___72.ciunit = *nout;
	    s_wsfe(&io___72);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, uplo, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "PT")) {

/*        xPT:  Positive definite tridiagonal matrices */

	if (lsamen_(&c__3, c3, "TRF")) {
	    if (*info != *infoe && *infoe != 0) {
		io___73.ciunit = *nout;
		s_wsfe(&io___73);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___74.ciunit = *nout;
		s_wsfe(&io___74);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    if (*info != 0) {
		io___75.ciunit = *nout;
		s_wsfe(&io___75);
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SV ")) {

	    if (*info != *infoe && *infoe != 0) {
		io___76.ciunit = *nout;
		s_wsfe(&io___76);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___77.ciunit = *nout;
		s_wsfe(&io___77);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "SVX")) {

	    if (*info != *infoe && *infoe != 0) {
		io___78.ciunit = *nout;
		s_wsfe(&io___78);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___79.ciunit = *nout;
		s_wsfe(&io___79);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else if (lsamen_(&c__3, c3, "CON")) {

	    if (lsame_(subnam, "S") || lsame_(subnam, 
		    "D")) {
		io___80.ciunit = *nout;
		s_wsfe(&io___80);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___81.ciunit = *nout;
		s_wsfe(&io___81);
		do_fio(&c__1, subnam, (ftnlen)6);
		do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
		do_fio(&c__1, opts, (ftnlen)1);
		do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	} else {

	    io___82.ciunit = *nout;
	    s_wsfe(&io___82);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "TR")) {

/*        xTR:  Triangular matrix */

	if (lsamen_(&c__3, c3, "TRI")) {
	    io___83.ciunit = *nout;
	    s_wsfe(&io___83);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__3, c3, "CON")) {
	    io___84.ciunit = *nout;
	    s_wsfe(&io___84);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__5, subnam + 1, "LATRS")) 
		{
	    io___85.ciunit = *nout;
	    s_wsfe(&io___85);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, opts + 3, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___86.ciunit = *nout;
	    s_wsfe(&io___86);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "TP")) {

/*        xTP:  Triangular packed matrix */

	if (lsamen_(&c__3, c3, "TRI")) {
	    io___87.ciunit = *nout;
	    s_wsfe(&io___87);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__3, c3, "CON")) {
	    io___88.ciunit = *nout;
	    s_wsfe(&io___88);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__5, subnam + 1, "LATPS")) 
		{
	    io___89.ciunit = *nout;
	    s_wsfe(&io___89);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, opts + 3, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___90.ciunit = *nout;
	    s_wsfe(&io___90);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "TB")) {

/*        xTB:  Triangular band matrix */

	if (lsamen_(&c__3, c3, "CON")) {
	    io___91.ciunit = *nout;
	    s_wsfe(&io___91);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__5, subnam + 1, "LATBS")) 
		{
	    io___92.ciunit = *nout;
	    s_wsfe(&io___92);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, opts + 3, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___93.ciunit = *nout;
	    s_wsfe(&io___93);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, opts, (ftnlen)1);
	    do_fio(&c__1, opts + 1, (ftnlen)1);
	    do_fio(&c__1, opts + 2, (ftnlen)1);
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "QR")) {

/*        xQR:  QR factorization */

	if (lsamen_(&c__3, c3, "QRS")) {
	    io___94.ciunit = *nout;
	    s_wsfe(&io___94);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__5, subnam + 1, "LATMS")) 
		{
	    io___95.ciunit = *nout;
	    s_wsfe(&io___95);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "LQ")) {

/*        xLQ:  LQ factorization */

	if (lsamen_(&c__3, c3, "LQS")) {
	    io___96.ciunit = *nout;
	    s_wsfe(&io___96);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__5, subnam + 1, "LATMS")) 
		{
	    io___97.ciunit = *nout;
	    s_wsfe(&io___97);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "QL")) {

/*        xQL:  QL factorization */

	if (lsamen_(&c__3, c3, "QLS")) {
	    io___98.ciunit = *nout;
	    s_wsfe(&io___98);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__5, subnam + 1, "LATMS")) 
		{
	    io___99.ciunit = *nout;
	    s_wsfe(&io___99);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "RQ")) {

/*        xRQ:  RQ factorization */

	if (lsamen_(&c__3, c3, "RQS")) {
	    io___100.ciunit = *nout;
	    s_wsfe(&io___100);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*kl), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else if (lsamen_(&c__5, subnam + 1, "LATMS")) 
		{
	    io___101.ciunit = *nout;
	    s_wsfe(&io___101);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "LU")) {

	if (*info != *infoe && *infoe != 0) {
	    io___102.ciunit = *nout;
	    s_wsfe(&io___102);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___103.ciunit = *nout;
	    s_wsfe(&io___103);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, p2, "CH")) {

	if (*info != *infoe && *infoe != 0) {
	    io___104.ciunit = *nout;
	    s_wsfe(&io___104);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*infoe), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___105.ciunit = *nout;
	    s_wsfe(&io___105);
	    do_fio(&c__1, subnam, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*m), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n5), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*imat), (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else {

/*        Print a generic message if the path is unknown. */

	io___106.ciunit = *nout;
	s_wsfe(&io___106);
	do_fio(&c__1, subnam, (ftnlen)6);
	do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	e_wsfe();
    }

/*     Description of error message (alphabetical, left to right) */

/*     SUBNAM, INFO, FACT, N, NRHS, IMAT */


/*     SUBNAM, INFO, FACT, TRANS, N, KL, KU, NRHS, IMAT */


/*     SUBNAM, INFO, FACT, TRANS, N, NRHS, IMAT */


/*     SUBNAM, INFO, FACT, UPLO, N, KD, NRHS, IMAT */


/*     SUBNAM, INFO, FACT, UPLO, N, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, FACT, N, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, FACT, TRANS, N, KL, KU, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, FACT, TRANS, N, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, FACT, UPLO, N, KD, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, FACT, UPLO, N, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, M, N, KL, KU, NB, IMAT */


/*     SUBNAM, INFO, INFOE, M, N, NB, IMAT */


/*     SUBNAM, INFO, INFOE, N, IMAT */


/*     SUBNAM, INFO, INFOE, N, KL, KU, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, N, NB, IMAT */


/*     SUBNAM, INFO, INFOE, N, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, UPLO, N, IMAT */


/*     SUBNAM, INFO, INFOE, UPLO, N, KD, NB, IMAT */


/*     SUBNAM, INFO, INFOE, UPLO, N, KD, NRHS, IMAT */


/*     SUBNAM, INFO, INFOE, UPLO, N, NB, IMAT */


/*     SUBNAM, INFO, INFOE, UPLO, N, NRHS, IMAT */


/*     SUBNAM, INFO, M, N, IMAT */


/*     SUBNAM, INFO, M, N, KL, KU, IMAT */


/*     SUBNAM, INFO, M, N, KL, KU, NB, IMAT */


/*     SUBNAM, INFO, M, N, NB, IMAT */


/*     SUBNAM, INFO, M, N, NRHS, NB, IMAT */


/*     SUBNAM, INFO, N, IMAT */


/*     SUBNAM, INFO, N, KL, KU, NRHS, IMAT */


/*     SUBNAM, INFO, N, NB, IMAT */


/*     SUBNAM, INFO, N, NRHS, IMAT */


/*     SUBNAM, INFO, NORM, N, IMAT */


/*     SUBNAM, INFO, NORM, N, KL, KU, IMAT */


/*     SUBNAM, INFO, NORM, UPLO, DIAG, N, IMAT */


/*     SUBNAM, INFO, NORM, UPLO, DIAG, N, KD, IMAT */


/*     SUBNAM, INFO, TRANS, M, N, NRHS, NB, IMAT */


/*     SUBNAM, INFO, TRANS, N, KL, KU, NRHS, IMAT */


/*     SUBNAM, INFO, TRANS, N, NRHS, IMAT */


/*     SUBNAM, INFO, UPLO, DIAG, N, IMAT */


/*     SUBNAM, INFO, UPLO, DIAG, N, NB, IMAT */


/*     SUBNAM, INFO, UPLO, N, IMAT */


/*     SUBNAM, INFO, UPLO, N, KD, IMAT */


/*     SUBNAM, INFO, UPLO, N, KD, NB, IMAT */


/*     SUBNAM, INFO, UPLO, N, KD, NRHS, IMAT */


/*     SUBNAM, INFO, UPLO, N, NB, IMAT */


/*     SUBNAM, INFO, UPLO, N, NRHS, IMAT */


/*     SUBNAM, INFO, UPLO, TRANS, DIAG, N, KD, NRHS, IMAT */


/*     SUBNAM, INFO, UPLO, TRANS, DIAG, N, NRHS, IMAT */


/*     SUBNAM, INFO, UPLO, TRANS, DIAG, NORMIN, N, IMAT */


/*     SUBNAM, INFO, UPLO, TRANS, DIAG, NORMIN, N, KD, IMAT */


/*     Unknown type */


/*     What we do next */


    return 0;

/*     End of ALAERH */

} /* alaerh_ */
