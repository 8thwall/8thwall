#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__20 = 20;

/* Subroutine */ int schkbk_(integer *nin, integer *nout)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,\002.. test output of SGEBAK .. \002)";
    static char fmt_9998[] = "(1x,\002value of largest test error           "
	    "  = \002,e12.3)";
    static char fmt_9997[] = "(1x,\002example number where info is not zero "
	    "  = \002,i4)";
    static char fmt_9996[] = "(1x,\002example number having largest error   "
	    "  = \002,i4)";
    static char fmt_9995[] = "(1x,\002number of examples where info is not 0"
	    "  = \002,i4)";
    static char fmt_9994[] = "(1x,\002total number of examples tested       "
	    "  = \002,i4)";

    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void), s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, 
	    char *, ftnlen);

    /* Local variables */
    real e[400]	/* was [20][20] */;
    integer i__, j, n;
    real x;
    integer ihi;
    real ein[400]	/* was [20][20] */;
    integer ilo;
    real eps;
    integer knt, info, lmax[2];
    real rmax, vmax, scale[20];
    integer ninfo;
    extern /* Subroutine */ int sgebak_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, integer *);
    extern doublereal slamch_(char *);
    real safmin;

    /* Fortran I/O blocks */
    static cilist io___7 = { 0, 0, 0, 0, 0 };
    static cilist io___11 = { 0, 0, 0, 0, 0 };
    static cilist io___14 = { 0, 0, 0, 0, 0 };
    static cilist io___17 = { 0, 0, 0, 0, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9994, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SCHKBK tests SGEBAK, a routine for backward transformation of */
/*  the computed right or left eigenvectors if the orginal matrix */
/*  was preprocessed by balance subroutine SGEBAL. */

/*  Arguments */
/*  ========= */

/*  NIN     (input) INTEGER */
/*          The logical unit number for input.  NIN > 0. */

/*  NOUT    (input) INTEGER */
/*          The logical unit number for output.  NOUT > 0. */

/* ====================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    lmax[0] = 0;
    lmax[1] = 0;
    ninfo = 0;
    knt = 0;
    rmax = 0.f;
    eps = slamch_("E");
    safmin = slamch_("S");

L10:

    io___7.ciunit = *nin;
    s_rsle(&io___7);
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&ilo, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&ihi, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	goto L60;
    }

    io___11.ciunit = *nin;
    s_rsle(&io___11);
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__4, &c__1, (char *)&scale[i__ - 1], (ftnlen)sizeof(real));
    }
    e_rsle();
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___14.ciunit = *nin;
	s_rsle(&io___14);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__4, &c__1, (char *)&e[i__ + j * 20 - 21], (ftnlen)
		    sizeof(real));
	}
	e_rsle();
/* L20: */
    }

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___17.ciunit = *nin;
	s_rsle(&io___17);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__4, &c__1, (char *)&ein[i__ + j * 20 - 21], (ftnlen)
		    sizeof(real));
	}
	e_rsle();
/* L30: */
    }

    ++knt;
    sgebak_("B", "R", &n, &ilo, &ihi, scale, &n, e, &c__20, &info);

    if (info != 0) {
	++ninfo;
	lmax[0] = knt;
    }

    vmax = 0.f;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    x = (r__1 = e[i__ + j * 20 - 21] - ein[i__ + j * 20 - 21], dabs(
		    r__1)) / eps;
	    if ((r__1 = e[i__ + j * 20 - 21], dabs(r__1)) > safmin) {
		x /= (r__2 = e[i__ + j * 20 - 21], dabs(r__2));
	    }
	    vmax = dmax(vmax,x);
/* L40: */
	}
/* L50: */
    }

    if (vmax > rmax) {
	lmax[1] = knt;
	rmax = vmax;
    }

    goto L10;

L60:

    io___22.ciunit = *nout;
    s_wsfe(&io___22);
    e_wsfe();

    io___23.ciunit = *nout;
    s_wsfe(&io___23);
    do_fio(&c__1, (char *)&rmax, (ftnlen)sizeof(real));
    e_wsfe();
    io___24.ciunit = *nout;
    s_wsfe(&io___24);
    do_fio(&c__1, (char *)&lmax[0], (ftnlen)sizeof(integer));
    e_wsfe();
    io___25.ciunit = *nout;
    s_wsfe(&io___25);
    do_fio(&c__1, (char *)&lmax[1], (ftnlen)sizeof(integer));
    e_wsfe();
    io___26.ciunit = *nout;
    s_wsfe(&io___26);
    do_fio(&c__1, (char *)&ninfo, (ftnlen)sizeof(integer));
    e_wsfe();
    io___27.ciunit = *nout;
    s_wsfe(&io___27);
    do_fio(&c__1, (char *)&knt, (ftnlen)sizeof(integer));
    e_wsfe();

    return 0;

/*     End of SCHKBK */

} /* schkbk_ */
