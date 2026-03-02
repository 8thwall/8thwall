#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__1 = 1;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__5 = 5;
static integer c__6 = 6;
static integer c__7 = 7;

/* Subroutine */ int aladhd_(integer *iounit, char *path)
{
    /* Format strings */
    static char fmt_9999[] = "(/1x,a3,\002 drivers:  General dense matrice"
	    "s\002)";
    static char fmt_9989[] = "(4x,\0021. Diagonal\002,24x,\0027. Last n/2 co"
	    "lumns zero\002,/4x,\0022. Upper triangular\002,16x,\0028. Random"
	    ", CNDNUM = sqrt(0.1/EPS)\002,/4x,\0023. Lower triangular\002,16x,"
	    "\0029. Random, CNDNUM = 0.1/EPS\002,/4x,\0024. Random, CNDNUM = 2"
	    "\002,13x,\00210. Scaled near underflow\002,/4x,\0025. First colu"
	    "mn zero\002,14x,\00211. Scaled near overflow\002,/4x,\0026. Last"
	    " column zero\002)";
    static char fmt_9981[] = "(3x,i2,\002: norm( L * U - A )  / ( N * norm(A"
	    ") * EPS )\002)";
    static char fmt_9980[] = "(3x,i2,\002: norm( B - A * X )  / \002,\002( n"
	    "orm(A) * norm(X) * EPS )\002)";
    static char fmt_9979[] = "(3x,i2,\002: norm( X - XACT )   / \002,\002( n"
	    "orm(XACT) * CNDNUM * EPS )\002)";
    static char fmt_9978[] = "(3x,i2,\002: norm( X - XACT )   / \002,\002( n"
	    "orm(XACT) * (error bound) )\002)";
    static char fmt_9977[] = "(3x,i2,\002: (backward error)   / EPS\002)";
    static char fmt_9976[] = "(3x,i2,\002: RCOND * CNDNUM - 1.0\002)";
    static char fmt_9972[] = "(3x,i2,\002: abs( WORK(1) - RPVGRW ) /\002,"
	    "\002 ( max( WORK(1), RPVGRW ) * EPS )\002)";
    static char fmt_9998[] = "(/1x,a3,\002 drivers:  General band matrice"
	    "s\002)";
    static char fmt_9988[] = "(4x,\0021. Random, CNDNUM = 2\002,14x,\0025. R"
	    "andom, CNDNUM = sqrt(0.1/EPS)\002,/4x,\0022. First column zer"
	    "o\002,15x,\0026. Random, CNDNUM = 0.1/EPS\002,/4x,\0023. Last co"
	    "lumn zero\002,16x,\0027. Scaled near underflow\002,/4x,\0024. La"
	    "st n/2 columns zero\002,11x,\0028. Scaled near overflow\002)";
    static char fmt_9997[] = "(/1x,a3,\002 drivers:  General tridiagonal\002)"
	    ;
    static char fmt_9987[] = "(\002 Matrix types (1-6 have specified conditi"
	    "on numbers):\002,/4x,\0021. Diagonal\002,24x,\0027. Random, unsp"
	    "ecified CNDNUM\002,/4x,\0022. Random, CNDNUM = 2\002,14x,\0028. "
	    "First column zero\002,/4x,\0023. Random, CNDNUM = sqrt(0.1/EPS"
	    ")\002,2x,\0029. Last column zero\002,/4x,\0024. Random, CNDNUM ="
	    " 0.1/EPS\002,7x,\00210. Last n/2 columns zero\002,/4x,\0025. Sca"
	    "led near underflow\002,10x,\00211. Scaled near underflow\002,/4x,"
	    "\0026. Scaled near overflow\002,11x,\00212. Scaled near overflo"
	    "w\002)";
    static char fmt_9996[] = "(/1x,a3,\002 drivers:  \002,a9,\002 positive d"
	    "efinite matrices\002)";
    static char fmt_9995[] = "(/1x,a3,\002 drivers:  \002,a9,\002 positive d"
	    "efinite packed matrices\002)";
    static char fmt_9985[] = "(4x,\0021. Diagonal\002,24x,\0026. Random, CND"
	    "NUM = sqrt(0.1/EPS)\002,/4x,\0022. Random, CNDNUM = 2\002,14x"
	    ",\0027. Random, CNDNUM = 0.1/EPS\002,/3x,\002*3. First row and c"
	    "olumn zero\002,7x,\0028. Scaled near underflow\002,/3x,\002*4. L"
	    "ast row and column zero\002,8x,\0029. Scaled near overflow\002,/"
	    "3x,\002*5. Middle row and column zero\002,/3x,\002(* - tests err"
	    "or exits from \002,a3,\002TRF, no test ratios are computed)\002)";
    static char fmt_9975[] = "(3x,i2,\002: norm( U' * U - A ) / ( N * norm(A"
	    ") * EPS )\002,\002, or\002,/7x,\002norm( L * L' - A ) / ( N * no"
	    "rm(A) * EPS )\002)";
    static char fmt_9994[] = "(/1x,a3,\002 drivers:  \002,a9,\002 positive d"
	    "efinite band matrices\002)";
    static char fmt_9984[] = "(4x,\0021. Random, CNDNUM = 2\002,14x,\0025. R"
	    "andom, CNDNUM = sqrt(0.1/EPS)\002,/3x,\002*2. First row and colu"
	    "mn zero\002,7x,\0026. Random, CNDNUM = 0.1/EPS\002,/3x,\002*3. L"
	    "ast row and column zero\002,8x,\0027. Scaled near underflow\002,"
	    "/3x,\002*4. Middle row and column zero\002,6x,\0028. Scaled near"
	    " overflow\002,/3x,\002(* - tests error exits from \002,a3,\002TR"
	    "F, no test ratios are computed)\002)";
    static char fmt_9993[] = "(/1x,a3,\002 drivers:  \002,a9,\002 positive d"
	    "efinite tridiagonal\002)";
    static char fmt_9986[] = "(\002 Matrix types (1-6 have specified conditi"
	    "on numbers):\002,/4x,\0021. Diagonal\002,24x,\0027. Random, unsp"
	    "ecified CNDNUM\002,/4x,\0022. Random, CNDNUM = 2\002,14x,\0028. "
	    "First row and column zero\002,/4x,\0023. Random, CNDNUM = sqrt(0"
	    ".1/EPS)\002,2x,\0029. Last row and column zero\002,/4x,\0024. Ra"
	    "ndom, CNDNUM = 0.1/EPS\002,7x,\00210. Middle row and column zer"
	    "o\002,/4x,\0025. Scaled near underflow\002,10x,\00211. Scaled ne"
	    "ar underflow\002,/4x,\0026. Scaled near overflow\002,11x,\00212."
	    " Scaled near overflow\002)";
    static char fmt_9973[] = "(3x,i2,\002: norm( U'*D*U - A ) / ( N * norm(A"
	    ") * EPS )\002,\002, or\002,/7x,\002norm( L*D*L' - A ) / ( N * no"
	    "rm(A) * EPS )\002)";
    static char fmt_9992[] = "(/1x,a3,\002 drivers:  \002,a9,\002 indefinite"
	    " matrices\002)";
    static char fmt_9991[] = "(/1x,a3,\002 drivers:  \002,a9,\002 indefinite"
	    " packed matrices\002)";
    static char fmt_9983[] = "(4x,\0021. Diagonal\002,24x,\0026. Last n/2 ro"
	    "ws and columns zero\002,/4x,\0022. Random, CNDNUM = 2\002,14x"
	    ",\0027. Random, CNDNUM = sqrt(0.1/EPS)\002,/4x,\0023. First row "
	    "and column zero\002,7x,\0028. Random, CNDNUM = 0.1/EPS\002,/4x"
	    ",\0024. Last row and column zero\002,8x,\0029. Scaled near under"
	    "flow\002,/4x,\0025. Middle row and column zero\002,5x,\00210. Sc"
	    "aled near overflow\002)";
    static char fmt_9982[] = "(4x,\0021. Diagonal\002,24x,\0027. Random, CND"
	    "NUM = sqrt(0.1/EPS)\002,/4x,\0022. Random, CNDNUM = 2\002,14x"
	    ",\0028. Random, CNDNUM = 0.1/EPS\002,/4x,\0023. First row and co"
	    "lumn zero\002,7x,\0029. Scaled near underflow\002,/4x,\0024. Las"
	    "t row and column zero\002,7x,\00210. Scaled near overflow\002,/4"
	    "x,\0025. Middle row and column zero\002,5x,\00211. Block diagona"
	    "l matrix\002,/4x,\0026. Last n/2 rows and columns zero\002)";
    static char fmt_9974[] = "(3x,i2,\002: norm( U*D*U' - A ) / ( N * norm(A"
	    ") * EPS )\002,\002, or\002,/7x,\002norm( L*D*L' - A ) / ( N * no"
	    "rm(A) * EPS )\002)";
    static char fmt_9990[] = "(/1x,a3,\002:  No header available\002)";

    /* System generated locals */
    cilist ci__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    char c1[1], c3[1], p2[2], sym[9];
    logical sord, corz;
    extern logical lsame_(char *, char *), lsamen_(integer *, 
	    char *, char *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___7 = { 0, 0, 0, fmt_9989, 0 };
    static cilist io___8 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___9 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___10 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___11 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___13 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___16 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___21 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9985, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9975, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9975, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9986, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9983, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9982, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___71 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_9983, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9990, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ALADHD prints header information for the driver routines test paths. */

/*  Arguments */
/*  ========= */

/*  IOUNIT  (input) INTEGER */
/*          The unit number to which the header information should be */
/*          printed. */

/*  PATH    (input) CHARACTER*3 */
/*          The name of the path for which the header information is to */
/*          be printed.  Current paths are */
/*             _GE:  General matrices */
/*             _GB:  General band */
/*             _GT:  General Tridiagonal */
/*             _PO:  Symmetric or Hermitian positive definite */
/*             _PP:  Symmetric or Hermitian positive definite packed */
/*             _PB:  Symmetric or Hermitian positive definite band */
/*             _PT:  Symmetric or Hermitian positive definite tridiagonal */
/*             _SY:  Symmetric indefinite */
/*             _SP:  Symmetric indefinite packed */
/*             _HE:  (complex) Hermitian indefinite */
/*             _HP:  (complex) Hermitian indefinite packed */
/*          The first character must be one of S, D, C, or Z (C or Z only */
/*          if complex). */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (*iounit <= 0) {
	return 0;
    }
    *(unsigned char *)c1 = *(unsigned char *)path;
    *(unsigned char *)c3 = *(unsigned char *)&path[2];
    s_copy(p2, path + 1, (ftnlen)2, (ftnlen)2);
    sord = lsame_(c1, "S") || lsame_(c1, "D");
    corz = lsame_(c1, "C") || lsame_(c1, "Z");
    if (! (sord || corz)) {
	return 0;
    }

    if (lsamen_(&c__2, p2, "GE")) {

/*        GE: General dense */

	io___6.ciunit = *iounit;
	s_wsfe(&io___6);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___7.ciunit = *iounit;
	s_wsfe(&io___7);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___8.ciunit = *iounit;
	s_wsfe(&io___8);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___9.ciunit = *iounit;
	s_wsfe(&io___9);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___10.ciunit = *iounit;
	s_wsfe(&io___10);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___11.ciunit = *iounit;
	s_wsfe(&io___11);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___12.ciunit = *iounit;
	s_wsfe(&io___12);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___13.ciunit = *iounit;
	s_wsfe(&io___13);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___14.ciunit = *iounit;
	s_wsfe(&io___14);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "GB")) {

/*        GB: General band */

	io___15.ciunit = *iounit;
	s_wsfe(&io___15);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___16.ciunit = *iounit;
	s_wsfe(&io___16);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___17.ciunit = *iounit;
	s_wsfe(&io___17);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___18.ciunit = *iounit;
	s_wsfe(&io___18);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___19.ciunit = *iounit;
	s_wsfe(&io___19);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___20.ciunit = *iounit;
	s_wsfe(&io___20);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___21.ciunit = *iounit;
	s_wsfe(&io___21);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___22.ciunit = *iounit;
	s_wsfe(&io___22);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___23.ciunit = *iounit;
	s_wsfe(&io___23);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "GT")) {

/*        GT: General tridiagonal */

	io___24.ciunit = *iounit;
	s_wsfe(&io___24);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___25.ciunit = *iounit;
	s_wsfe(&io___25);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___26.ciunit = *iounit;
	s_wsfe(&io___26);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___27.ciunit = *iounit;
	s_wsfe(&io___27);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___28.ciunit = *iounit;
	s_wsfe(&io___28);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___29.ciunit = *iounit;
	s_wsfe(&io___29);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___30.ciunit = *iounit;
	s_wsfe(&io___30);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___31.ciunit = *iounit;
	s_wsfe(&io___31);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "PO") || lsamen_(&
	    c__2, p2, "PP")) {

/*        PO: Positive definite full */
/*        PP: Positive definite packed */

	if (sord) {
	    s_copy(sym, "Symmetric", (ftnlen)9, (ftnlen)9);
	} else {
	    s_copy(sym, "Hermitian", (ftnlen)9, (ftnlen)9);
	}
	if (lsame_(c3, "O")) {
	    io___33.ciunit = *iounit;
	    s_wsfe(&io___33);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, sym, (ftnlen)9);
	    e_wsfe();
	} else {
	    io___34.ciunit = *iounit;
	    s_wsfe(&io___34);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, sym, (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___35.ciunit = *iounit;
	s_wsfe(&io___35);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___36.ciunit = *iounit;
	s_wsfe(&io___36);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___37.ciunit = *iounit;
	s_wsfe(&io___37);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___38.ciunit = *iounit;
	s_wsfe(&io___38);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___39.ciunit = *iounit;
	s_wsfe(&io___39);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___40.ciunit = *iounit;
	s_wsfe(&io___40);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___41.ciunit = *iounit;
	s_wsfe(&io___41);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "PB")) {

/*        PB: Positive definite band */

	if (sord) {
	    io___42.ciunit = *iounit;
	    s_wsfe(&io___42);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___43.ciunit = *iounit;
	    s_wsfe(&io___43);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___44.ciunit = *iounit;
	s_wsfe(&io___44);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___45.ciunit = *iounit;
	s_wsfe(&io___45);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___46.ciunit = *iounit;
	s_wsfe(&io___46);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___47.ciunit = *iounit;
	s_wsfe(&io___47);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___48.ciunit = *iounit;
	s_wsfe(&io___48);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___49.ciunit = *iounit;
	s_wsfe(&io___49);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___50.ciunit = *iounit;
	s_wsfe(&io___50);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "PT")) {

/*        PT: Positive definite tridiagonal */

	if (sord) {
	    io___51.ciunit = *iounit;
	    s_wsfe(&io___51);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___52.ciunit = *iounit;
	    s_wsfe(&io___52);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	}
	io___53.ciunit = *iounit;
	s_wsfe(&io___53);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___54.ciunit = *iounit;
	s_wsfe(&io___54);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___55.ciunit = *iounit;
	s_wsfe(&io___55);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___56.ciunit = *iounit;
	s_wsfe(&io___56);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___57.ciunit = *iounit;
	s_wsfe(&io___57);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___58.ciunit = *iounit;
	s_wsfe(&io___58);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___59.ciunit = *iounit;
	s_wsfe(&io___59);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "SY") || lsamen_(&
	    c__2, p2, "SP")) {

/*        SY: Symmetric indefinite full */
/*        SP: Symmetric indefinite packed */

	if (lsame_(c3, "Y")) {
	    io___60.ciunit = *iounit;
	    s_wsfe(&io___60);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___61.ciunit = *iounit;
	    s_wsfe(&io___61);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	if (sord) {
	    io___62.ciunit = *iounit;
	    s_wsfe(&io___62);
	    e_wsfe();
	} else {
	    io___63.ciunit = *iounit;
	    s_wsfe(&io___63);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___64.ciunit = *iounit;
	s_wsfe(&io___64);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___65.ciunit = *iounit;
	s_wsfe(&io___65);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___66.ciunit = *iounit;
	s_wsfe(&io___66);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___67.ciunit = *iounit;
	s_wsfe(&io___67);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___68.ciunit = *iounit;
	s_wsfe(&io___68);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___69.ciunit = *iounit;
	s_wsfe(&io___69);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "HE") || lsamen_(&
	    c__2, p2, "HP")) {

/*        HE: Hermitian indefinite full */
/*        HP: Hermitian indefinite packed */

	if (lsame_(c3, "E")) {
	    io___70.ciunit = *iounit;
	    s_wsfe(&io___70);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___71.ciunit = *iounit;
	    s_wsfe(&io___71);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___72.ciunit = *iounit;
	s_wsfe(&io___72);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___73.ciunit = *iounit;
	s_wsfe(&io___73);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___74.ciunit = *iounit;
	s_wsfe(&io___74);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___75.ciunit = *iounit;
	s_wsfe(&io___75);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___76.ciunit = *iounit;
	s_wsfe(&io___76);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___77.ciunit = *iounit;
	s_wsfe(&io___77);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___78.ciunit = *iounit;
	s_wsfe(&io___78);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else {

/*        Print error message if no header is available. */

	io___79.ciunit = *iounit;
	s_wsfe(&io___79);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }

/*     First line of header */


/*     GE matrix types */


/*     GB matrix types */


/*     GT matrix types */


/*     PT matrix types */


/*     PO, PP matrix types */


/*     PB matrix types */


/*     SSY, SSP, CHE, CHP matrix types */


/*     CSY, CSP matrix types */


/*     Test ratios */


    return 0;

/*     End of ALADHD */

} /* aladhd_ */
