#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__2 = 2;
static integer c__4 = 4;
static integer c__5 = 5;
static integer c__6 = 6;
static integer c__7 = 7;
static integer c__8 = 8;

/* Subroutine */ int alahdg_(integer *iounit, char *path)
{
    /* Format strings */
    static char fmt_9991[] = "(/1x,a3,\002: GQR factorization of general mat"
	    "rices\002)";
    static char fmt_9992[] = "(/1x,a3,\002: GRQ factorization of general mat"
	    "rices\002)";
    static char fmt_9993[] = "(/1x,a3,\002: LSE Problem\002)";
    static char fmt_9994[] = "(/1x,a3,\002: GLM Problem\002)";
    static char fmt_9995[] = "(/1x,a3,\002: Generalized Singular Value Decom"
	    "position\002)";
    static char fmt_9999[] = "(1x,a)";
    static char fmt_9950[] = "(3x,i2,\002: A-diagonal matrix  B-upper triang"
	    "ular\002)";
    static char fmt_9952[] = "(3x,i2,\002: A-upper triangular B-upper triang"
	    "ular\002)";
    static char fmt_9954[] = "(3x,i2,\002: A-lower triangular B-upper triang"
	    "ular\002)";
    static char fmt_9955[] = "(3x,i2,\002: Random matrices cond(A)=100, cond"
	    "(B)=10,\002)";
    static char fmt_9956[] = "(3x,i2,\002: Random matrices cond(A)= sqrt( 0."
	    "1/EPS ) \002,\002cond(B)= sqrt( 0.1/EPS )\002)";
    static char fmt_9957[] = "(3x,i2,\002: Random matrices cond(A)= 0.1/EPS"
	    " \002,\002cond(B)= 0.1/EPS\002)";
    static char fmt_9961[] = "(3x,i2,\002: Matrix scaled near underflow li"
	    "mit\002)";
    static char fmt_9962[] = "(3x,i2,\002: Matrix scaled near overflow limi"
	    "t\002)";
    static char fmt_9951[] = "(3x,i2,\002: A-diagonal matrix  B-lower triang"
	    "ular\002)";
    static char fmt_9953[] = "(3x,i2,\002: A-lower triangular B-diagonal tri"
	    "angular\002)";
    static char fmt_9959[] = "(3x,i2,\002: Random matrices cond(A)= sqrt( 0."
	    "1/EPS ) \002,\002cond(B)=  0.1/EPS \002)";
    static char fmt_9960[] = "(3x,i2,\002: Random matrices cond(A)= 0.1/EPS"
	    " \002,\002cond(B)=  sqrt( 0.1/EPS )\002)";
    static char fmt_9930[] = "(3x,i2,\002: norm( R - Q' * A ) / ( min( N, M "
	    ")*norm( A )\002,\002* EPS )\002)";
    static char fmt_9931[] = "(3x,i2,\002: norm( T * Z - Q' * B )  / ( min(P"
	    ",N)*norm(B)\002,\002* EPS )\002)";
    static char fmt_9932[] = "(3x,i2,\002: norm( I - Q'*Q )   / ( N * EPS "
	    ")\002)";
    static char fmt_9933[] = "(3x,i2,\002: norm( I - Z'*Z )   / ( P * EPS "
	    ")\002)";
    static char fmt_9934[] = "(3x,i2,\002: norm( R - A * Q' ) / ( min( N,M )"
	    "*norm(A) * \002,\002EPS )\002)";
    static char fmt_9935[] = "(3x,i2,\002: norm( T * Q - Z' * B )  / ( min( "
	    "P,N ) * nor\002,\002m(B)*EPS )\002)";
    static char fmt_9937[] = "(3x,i2,\002: norm( A*x - c )  / ( norm(A)*norm"
	    "(x) * EPS )\002)";
    static char fmt_9938[] = "(3x,i2,\002: norm( B*x - d )  / ( norm(B)*norm"
	    "(x) * EPS )\002)";
    static char fmt_9939[] = "(3x,i2,\002: norm( d - A*x - B*y ) / ( (norm(A"
	    ")+norm(B) )*\002,\002(norm(x)+norm(y))*EPS )\002)";
    static char fmt_9940[] = "(3x,i2,\002: norm( U' * A * Q - D1 * R ) / ( m"
	    "in( M, N )*\002,\002norm( A ) * EPS )\002)";
    static char fmt_9941[] = "(3x,i2,\002: norm( V' * B * Q - D2 * R ) / ( m"
	    "in( P, N )*\002,\002norm( B ) * EPS )\002)";
    static char fmt_9942[] = "(3x,i2,\002: norm( I - U'*U )   / ( M * EPS "
	    ")\002)";
    static char fmt_9943[] = "(3x,i2,\002: norm( I - V'*V )   / ( P * EPS "
	    ")\002)";
    static char fmt_9944[] = "(3x,i2,\002: norm( I - Q'*Q )   / ( N * EPS "
	    ")\002)";

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    char c2[3];
    integer itype;
    extern logical lsamen_(integer *, char *, char *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___4 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___5 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___6 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___7 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___8 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___9 = { 0, 0, 0, fmt_9950, 0 };
    static cilist io___10 = { 0, 0, 0, fmt_9952, 0 };
    static cilist io___11 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___13 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___16 = { 0, 0, 0, fmt_9962, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9951, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_9953, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___21 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9962, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9950, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9952, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9951, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9953, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9950, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9952, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9930, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9931, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9932, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9933, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9934, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9935, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9932, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9933, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9937, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9938, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9939, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9940, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9941, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9942, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9943, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9944, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ALAHDG prints header information for the different test paths. */

/*  Arguments */
/*  ========= */

/*  IOUNIT  (input) INTEGER */
/*          The unit number to which the header information should be */
/*          printed. */

/*  PATH    (input) CHARACTER*3 */
/*          The name of the path for which the header information is to */
/*          be printed.  Current paths are */
/*             GQR:  GQR (general matrices) */
/*             GRQ:  GRQ (general matrices) */
/*             LSE:  LSE Problem */
/*             GLM:  GLM Problem */
/*             GSV:  Generalized Singular Value Decomposition */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (*iounit <= 0) {
	return 0;
    }
    s_copy(c2, path, (ftnlen)3, (ftnlen)3);

/*     First line describing matrices in this path */

    if (lsamen_(&c__3, c2, "GQR")) {
	itype = 1;
	io___3.ciunit = *iounit;
	s_wsfe(&io___3);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    } else if (lsamen_(&c__3, c2, "GRQ")) {
	itype = 2;
	io___4.ciunit = *iounit;
	s_wsfe(&io___4);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    } else if (lsamen_(&c__3, c2, "LSE")) {
	itype = 3;
	io___5.ciunit = *iounit;
	s_wsfe(&io___5);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    } else if (lsamen_(&c__3, c2, "GLM")) {
	itype = 4;
	io___6.ciunit = *iounit;
	s_wsfe(&io___6);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    } else if (lsamen_(&c__3, c2, "GSV")) {
	itype = 5;
	io___7.ciunit = *iounit;
	s_wsfe(&io___7);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }

/*     Matrix types */

    io___8.ciunit = *iounit;
    s_wsfe(&io___8);
    do_fio(&c__1, "Matrix types: ", (ftnlen)14);
    e_wsfe();

    if (itype == 1) {
	io___9.ciunit = *iounit;
	s_wsfe(&io___9);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___10.ciunit = *iounit;
	s_wsfe(&io___10);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___11.ciunit = *iounit;
	s_wsfe(&io___11);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___12.ciunit = *iounit;
	s_wsfe(&io___12);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___13.ciunit = *iounit;
	s_wsfe(&io___13);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___14.ciunit = *iounit;
	s_wsfe(&io___14);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___15.ciunit = *iounit;
	s_wsfe(&io___15);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___16.ciunit = *iounit;
	s_wsfe(&io___16);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
    } else if (itype == 2) {
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
	io___24.ciunit = *iounit;
	s_wsfe(&io___24);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
    } else if (itype == 3) {
	io___25.ciunit = *iounit;
	s_wsfe(&io___25);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___26.ciunit = *iounit;
	s_wsfe(&io___26);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___27.ciunit = *iounit;
	s_wsfe(&io___27);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___28.ciunit = *iounit;
	s_wsfe(&io___28);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___29.ciunit = *iounit;
	s_wsfe(&io___29);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___30.ciunit = *iounit;
	s_wsfe(&io___30);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___31.ciunit = *iounit;
	s_wsfe(&io___31);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___32.ciunit = *iounit;
	s_wsfe(&io___32);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
    } else if (itype == 4) {
	io___33.ciunit = *iounit;
	s_wsfe(&io___33);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___34.ciunit = *iounit;
	s_wsfe(&io___34);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___35.ciunit = *iounit;
	s_wsfe(&io___35);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___36.ciunit = *iounit;
	s_wsfe(&io___36);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___37.ciunit = *iounit;
	s_wsfe(&io___37);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___38.ciunit = *iounit;
	s_wsfe(&io___38);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___39.ciunit = *iounit;
	s_wsfe(&io___39);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___40.ciunit = *iounit;
	s_wsfe(&io___40);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
    } else if (itype == 5) {
	io___41.ciunit = *iounit;
	s_wsfe(&io___41);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___42.ciunit = *iounit;
	s_wsfe(&io___42);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___43.ciunit = *iounit;
	s_wsfe(&io___43);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___44.ciunit = *iounit;
	s_wsfe(&io___44);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___45.ciunit = *iounit;
	s_wsfe(&io___45);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___46.ciunit = *iounit;
	s_wsfe(&io___46);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___47.ciunit = *iounit;
	s_wsfe(&io___47);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___48.ciunit = *iounit;
	s_wsfe(&io___48);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
    }

/*     Tests performed */

    io___49.ciunit = *iounit;
    s_wsfe(&io___49);
    do_fio(&c__1, "Test ratios: ", (ftnlen)13);
    e_wsfe();

    if (itype == 1) {

/*        GQR decomposition of rectangular matrices */

	io___50.ciunit = *iounit;
	s_wsfe(&io___50);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___51.ciunit = *iounit;
	s_wsfe(&io___51);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___52.ciunit = *iounit;
	s_wsfe(&io___52);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___53.ciunit = *iounit;
	s_wsfe(&io___53);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
    } else if (itype == 2) {

/*        GRQ decomposition of rectangular matrices */

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
    } else if (itype == 3) {

/*        LSE Problem */

	io___58.ciunit = *iounit;
	s_wsfe(&io___58);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___59.ciunit = *iounit;
	s_wsfe(&io___59);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
    } else if (itype == 4) {

/*        GLM Problem */

	io___60.ciunit = *iounit;
	s_wsfe(&io___60);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
    } else if (itype == 5) {

/*        GSVD */

	io___61.ciunit = *iounit;
	s_wsfe(&io___61);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___62.ciunit = *iounit;
	s_wsfe(&io___62);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___63.ciunit = *iounit;
	s_wsfe(&io___63);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___64.ciunit = *iounit;
	s_wsfe(&io___64);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___65.ciunit = *iounit;
	s_wsfe(&io___65);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
    }







/*     GQR test ratio */


/*     GRQ test ratio */


/*     LSE test ratio */


/*     GLM test ratio */


/*     GSVD test ratio */

    return 0;

/*     End of ALAHDG */

} /* alahdg_ */
