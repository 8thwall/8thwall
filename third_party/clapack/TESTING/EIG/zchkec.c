#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__3 = 3;

/* Subroutine */ int zchkec_(doublereal *thresh, logical *tsterr, integer *
	nin, integer *nout)
{
    /* Format strings */
    static char fmt_9994[] = "(\002 Tests of the Nonsymmetric eigenproblem c"
	    "ondition\002,\002 estimation routines\002,/\002 ZTRSYL, CTREXC, "
	    "CTRSNA, CTRSEN\002,/)";
    static char fmt_9993[] = "(\002 Relative machine precision (EPS) = \002,"
	    "d16.6,/\002 Safe minimum (SFMIN)             = \002,d16.6,/)";
    static char fmt_9992[] = "(\002 Routines pass computational tests if tes"
	    "t ratio is \002,\002less than\002,f8.2,//)";
    static char fmt_9999[] = "(\002 Error in ZTRSYL: RMAX =\002,d12.3,/\002 "
	    "LMAX = \002,i8,\002 NINFO=\002,i8,\002 KNT=\002,i8)";
    static char fmt_9998[] = "(\002 Error in ZTREXC: RMAX =\002,d12.3,/\002 "
	    "LMAX = \002,i8,\002 NINFO=\002,i8,\002 KNT=\002,i8)";
    static char fmt_9997[] = "(\002 Error in ZTRSNA: RMAX =\002,3d12.3,/\002"
	    " LMAX = \002,3i8,\002 NINFO=\002,3i8,\002 KNT=\002,i8)";
    static char fmt_9996[] = "(\002 Error in ZTRSEN: RMAX =\002,3d12.3,/\002"
	    " LMAX = \002,3i8,\002 NINFO=\002,3i8,\002 KNT=\002,i8)";
    static char fmt_9995[] = "(/1x,\002All tests for \002,a3,\002 routines p"
	    "assed the threshold (\002,i6,\002 tests run)\002)";

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, char *, ftnlen);

    /* Local variables */
    logical ok;
    doublereal eps;
    char path[3];
    doublereal sfmin;
    extern /* Subroutine */ int zget35_(doublereal *, integer *, integer *, 
	    integer *, integer *), zget36_(doublereal *, integer *, integer *, 
	     integer *, integer *), zget37_(doublereal *, integer *, integer *
, integer *, integer *), zget38_(doublereal *, integer *, integer 
	    *, integer *, integer *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int zerrec_(char *, integer *);
    integer ktrexc, ltrexc, ktrsna, ntrexc, ltrsna[3], ntrsna[3], ktrsen;
    doublereal rtrexc;
    integer ltrsen[3], ntrsen[3];
    doublereal rtrsna[3], rtrsen[3];
    integer ntests, ktrsyl, ltrsyl, ntrsyl;
    doublereal rtrsyl;

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___5 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___6 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9995, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZCHKEC tests eigen- condition estimation routines */
/*         ZTRSYL, CTREXC, CTRSNA, CTRSEN */

/*  In all cases, the routine runs through a fixed set of numerical */
/*  examples, subjects them to various tests, and compares the test */
/*  results to a threshold THRESH. In addition, ZTRSNA and CTRSEN are */
/*  tested by reading in precomputed examples from a file (on input unit */
/*  NIN).  Output is written to output unit NOUT. */

/*  Arguments */
/*  ========= */

/*  THRESH  (input) DOUBLE PRECISION */
/*          Threshold for residual tests.  A computed test ratio passes */
/*          the threshold if it is less than THRESH. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  NIN     (input) INTEGER */
/*          The logical unit number for input. */

/*  NOUT    (input) INTEGER */
/*          The logical unit number for output. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "EC", (ftnlen)2, (ftnlen)2);
    eps = dlamch_("P");
    sfmin = dlamch_("S");
    io___4.ciunit = *nout;
    s_wsfe(&io___4);
    e_wsfe();
    io___5.ciunit = *nout;
    s_wsfe(&io___5);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&sfmin, (ftnlen)sizeof(doublereal));
    e_wsfe();
    io___6.ciunit = *nout;
    s_wsfe(&io___6);
    do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(doublereal));
    e_wsfe();

/*     Test error exits if TSTERR is .TRUE. */

    if (*tsterr) {
	zerrec_(path, nout);
    }

    ok = TRUE_;
    zget35_(&rtrsyl, &ltrsyl, &ntrsyl, &ktrsyl, nin);
    if (rtrsyl > *thresh) {
	ok = FALSE_;
	io___12.ciunit = *nout;
	s_wsfe(&io___12);
	do_fio(&c__1, (char *)&rtrsyl, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&ltrsyl, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ntrsyl, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ktrsyl, (ftnlen)sizeof(integer));
	e_wsfe();
    }

    zget36_(&rtrexc, &ltrexc, &ntrexc, &ktrexc, nin);
    if (rtrexc > *thresh || ntrexc > 0) {
	ok = FALSE_;
	io___17.ciunit = *nout;
	s_wsfe(&io___17);
	do_fio(&c__1, (char *)&rtrexc, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&ltrexc, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ntrexc, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ktrexc, (ftnlen)sizeof(integer));
	e_wsfe();
    }

    zget37_(rtrsna, ltrsna, ntrsna, &ktrsna, nin);
    if (rtrsna[0] > *thresh || rtrsna[1] > *thresh || ntrsna[0] != 0 || 
	    ntrsna[1] != 0 || ntrsna[2] != 0) {
	ok = FALSE_;
	io___22.ciunit = *nout;
	s_wsfe(&io___22);
	do_fio(&c__3, (char *)&rtrsna[0], (ftnlen)sizeof(doublereal));
	do_fio(&c__3, (char *)&ltrsna[0], (ftnlen)sizeof(integer));
	do_fio(&c__3, (char *)&ntrsna[0], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ktrsna, (ftnlen)sizeof(integer));
	e_wsfe();
    }

    zget38_(rtrsen, ltrsen, ntrsen, &ktrsen, nin);
    if (rtrsen[0] > *thresh || rtrsen[1] > *thresh || ntrsen[0] != 0 || 
	    ntrsen[1] != 0 || ntrsen[2] != 0) {
	ok = FALSE_;
	io___27.ciunit = *nout;
	s_wsfe(&io___27);
	do_fio(&c__3, (char *)&rtrsen[0], (ftnlen)sizeof(doublereal));
	do_fio(&c__3, (char *)&ltrsen[0], (ftnlen)sizeof(integer));
	do_fio(&c__3, (char *)&ntrsen[0], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ktrsen, (ftnlen)sizeof(integer));
	e_wsfe();
    }

    ntests = ktrsyl + ktrexc + ktrsna + ktrsen;
    if (ok) {
	io___29.ciunit = *nout;
	s_wsfe(&io___29);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&ntests, (ftnlen)sizeof(integer));
	e_wsfe();
    }

    return 0;

/*     End of ZCHKEC */

} /* zchkec_ */
