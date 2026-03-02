#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__20 = 20;

/* Subroutine */ int schkbl_(integer *nin, integer *nout)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,\002.. test output of SGEBAL .. \002)";
    static char fmt_9998[] = "(1x,\002value of largest test error           "
	    " = \002,e12.3)";
    static char fmt_9997[] = "(1x,\002example number where info is not zero "
	    " = \002,i4)";
    static char fmt_9996[] = "(1x,\002example number where ILO or IHI wrong "
	    " = \002,i4)";
    static char fmt_9995[] = "(1x,\002example number having largest error   "
	    " = \002,i4)";
    static char fmt_9994[] = "(1x,\002number of examples where info is not 0"
	    " = \002,i4)";
    static char fmt_9993[] = "(1x,\002total number of examples tested       "
	    " = \002,i4)";

    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2, r__3;

    /* Builtin functions */
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void), s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, 
	    char *, ftnlen);

    /* Local variables */
    real a[400]	/* was [20][20] */;
    integer i__, j, n;
    real ain[400]	/* was [20][20] */;
    integer ihi, ilo, knt, info, lmax[3];
    real meps, temp, rmax, vmax, scale[20];
    integer ihiin, ninfo, iloin;
    real anorm, sfmin, dummy[1];
    extern /* Subroutine */ int sgebal_(char *, integer *, real *, integer *, 
	    integer *, integer *, real *, integer *);
    extern doublereal slamch_(char *);
    real scalin[20];
    extern doublereal slange_(char *, integer *, integer *, real *, integer *, 
	     real *);

    /* Fortran I/O blocks */
    static cilist io___8 = { 0, 0, 0, 0, 0 };
    static cilist io___11 = { 0, 0, 0, 0, 0 };
    static cilist io___14 = { 0, 0, 0, 0, 0 };
    static cilist io___17 = { 0, 0, 0, 0, 0 };
    static cilist io___19 = { 0, 0, 0, 0, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9993, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SCHKBL tests SGEBAL, a routine for balancing a general real */
/*  matrix and isolating some of its eigenvalues. */

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
    lmax[2] = 0;
    ninfo = 0;
    knt = 0;
    rmax = 0.f;
    vmax = 0.f;
    sfmin = slamch_("S");
    meps = slamch_("E");

L10:

    io___8.ciunit = *nin;
    s_rsle(&io___8);
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	goto L70;
    }
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___11.ciunit = *nin;
	s_rsle(&io___11);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__4, &c__1, (char *)&a[i__ + j * 20 - 21], (ftnlen)
		    sizeof(real));
	}
	e_rsle();
/* L20: */
    }

    io___14.ciunit = *nin;
    s_rsle(&io___14);
    do_lio(&c__3, &c__1, (char *)&iloin, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&ihiin, (ftnlen)sizeof(integer));
    e_rsle();
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___17.ciunit = *nin;
	s_rsle(&io___17);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__4, &c__1, (char *)&ain[i__ + j * 20 - 21], (ftnlen)
		    sizeof(real));
	}
	e_rsle();
/* L30: */
    }
    io___19.ciunit = *nin;
    s_rsle(&io___19);
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__4, &c__1, (char *)&scalin[i__ - 1], (ftnlen)sizeof(real));
    }
    e_rsle();

    anorm = slange_("M", &n, &n, a, &c__20, dummy);
    ++knt;

    sgebal_("B", &n, a, &c__20, &ilo, &ihi, scale, &info);

    if (info != 0) {
	++ninfo;
	lmax[0] = knt;
    }

    if (ilo != iloin || ihi != ihiin) {
	++ninfo;
	lmax[1] = knt;
    }

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
	    r__1 = a[i__ + j * 20 - 21], r__2 = ain[i__ + j * 20 - 21];
	    temp = dmax(r__1,r__2);
	    temp = dmax(temp,sfmin);
/* Computing MAX */
	    r__2 = vmax, r__3 = (r__1 = a[i__ + j * 20 - 21] - ain[i__ + j * 
		    20 - 21], dabs(r__1)) / temp;
	    vmax = dmax(r__2,r__3);
/* L40: */
	}
/* L50: */
    }

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	r__1 = scale[i__ - 1], r__2 = scalin[i__ - 1];
	temp = dmax(r__1,r__2);
	temp = dmax(temp,sfmin);
/* Computing MAX */
	r__2 = vmax, r__3 = (r__1 = scale[i__ - 1] - scalin[i__ - 1], dabs(
		r__1)) / temp;
	vmax = dmax(r__2,r__3);
/* L60: */
    }


    if (vmax > rmax) {
	lmax[2] = knt;
	rmax = vmax;
    }

    goto L10;

L70:

    io___28.ciunit = *nout;
    s_wsfe(&io___28);
    e_wsfe();

    io___29.ciunit = *nout;
    s_wsfe(&io___29);
    do_fio(&c__1, (char *)&rmax, (ftnlen)sizeof(real));
    e_wsfe();
    io___30.ciunit = *nout;
    s_wsfe(&io___30);
    do_fio(&c__1, (char *)&lmax[0], (ftnlen)sizeof(integer));
    e_wsfe();
    io___31.ciunit = *nout;
    s_wsfe(&io___31);
    do_fio(&c__1, (char *)&lmax[1], (ftnlen)sizeof(integer));
    e_wsfe();
    io___32.ciunit = *nout;
    s_wsfe(&io___32);
    do_fio(&c__1, (char *)&lmax[2], (ftnlen)sizeof(integer));
    e_wsfe();
    io___33.ciunit = *nout;
    s_wsfe(&io___33);
    do_fio(&c__1, (char *)&ninfo, (ftnlen)sizeof(integer));
    e_wsfe();
    io___34.ciunit = *nout;
    s_wsfe(&io___34);
    do_fio(&c__1, (char *)&knt, (ftnlen)sizeof(integer));
    e_wsfe();

    return 0;

/*     End of SCHKBL */

} /* schkbl_ */
