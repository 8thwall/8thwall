#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__5 = 5;
static integer c__50 = 50;
static doublereal c_b52 = 1.;
static doublereal c_b55 = 0.;

/* Subroutine */ int dchkgk_(integer *nin, integer *nout)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,\002.. test output of DGGBAK .. \002)";
    static char fmt_9998[] = "(\002 value of largest test error             "
	    "     =\002,d12.3)";
    static char fmt_9997[] = "(\002 example number where DGGBAL info is not "
	    "0    =\002,i4)";
    static char fmt_9996[] = "(\002 example number where DGGBAK(L) info is n"
	    "ot 0 =\002,i4)";
    static char fmt_9995[] = "(\002 example number where DGGBAK(R) info is n"
	    "ot 0 =\002,i4)";
    static char fmt_9994[] = "(\002 example number having largest error     "
	    "     =\002,i4)";
    static char fmt_9993[] = "(\002 number of examples where info is not 0  "
	    "     =\002,i4)";
    static char fmt_9992[] = "(\002 total number of examples tested         "
	    "     =\002,i4)";

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void), s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, 
	    char *, ftnlen);

    /* Local variables */
    doublereal a[2500]	/* was [50][50] */, b[2500]	/* was [50][50] */, e[
	    2500]	/* was [50][50] */, f[2500]	/* was [50][50] */;
    integer i__, j, m, n;
    doublereal af[2500]	/* was [50][50] */, bf[2500]	/* was [50][50] */, 
	    vl[2500]	/* was [50][50] */, vr[2500]	/* was [50][50] */;
    integer ihi, ilo;
    doublereal eps, vlf[2500]	/* was [50][50] */;
    integer knt;
    doublereal vrf[2500]	/* was [50][50] */;
    integer info, lmax[4];
    doublereal rmax, vmax, work[2500]	/* was [50][50] */;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    integer ninfo;
    doublereal anorm, bnorm;
    extern /* Subroutine */ int dggbak_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, integer *), dggbal_(char *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    doublereal lscale[50], rscale[50];
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 0, 0, 0, 0 };
    static cilist io___10 = { 0, 0, 0, 0, 0 };
    static cilist io___13 = { 0, 0, 0, 0, 0 };
    static cilist io___15 = { 0, 0, 0, 0, 0 };
    static cilist io___17 = { 0, 0, 0, 0, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9992, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKGK tests DGGBAK, a routine for backward balancing  of */
/*  a matrix pair (A, B). */

/*  Arguments */
/*  ========= */

/*  NIN     (input) INTEGER */
/*          The logical unit number for input.  NIN > 0. */

/*  NOUT    (input) INTEGER */
/*          The logical unit number for output.  NOUT > 0. */

/*  ===================================================================== */

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

/*     Initialization */

    lmax[0] = 0;
    lmax[1] = 0;
    lmax[2] = 0;
    lmax[3] = 0;
    ninfo = 0;
    knt = 0;
    rmax = 0.;

    eps = dlamch_("Precision");

L10:
    io___6.ciunit = *nin;
    s_rsle(&io___6);
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&m, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	goto L100;
    }

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___10.ciunit = *nin;
	s_rsle(&io___10);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__5, &c__1, (char *)&a[i__ + j * 50 - 51], (ftnlen)
		    sizeof(doublereal));
	}
	e_rsle();
/* L20: */
    }

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___13.ciunit = *nin;
	s_rsle(&io___13);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__5, &c__1, (char *)&b[i__ + j * 50 - 51], (ftnlen)
		    sizeof(doublereal));
	}
	e_rsle();
/* L30: */
    }

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___15.ciunit = *nin;
	s_rsle(&io___15);
	i__2 = m;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__5, &c__1, (char *)&vl[i__ + j * 50 - 51], (ftnlen)
		    sizeof(doublereal));
	}
	e_rsle();
/* L40: */
    }

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___17.ciunit = *nin;
	s_rsle(&io___17);
	i__2 = m;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__5, &c__1, (char *)&vr[i__ + j * 50 - 51], (ftnlen)
		    sizeof(doublereal));
	}
	e_rsle();
/* L50: */
    }

    ++knt;

    anorm = dlange_("M", &n, &n, a, &c__50, work);
    bnorm = dlange_("M", &n, &n, b, &c__50, work);

    dlacpy_("FULL", &n, &n, a, &c__50, af, &c__50);
    dlacpy_("FULL", &n, &n, b, &c__50, bf, &c__50);

    dggbal_("B", &n, a, &c__50, b, &c__50, &ilo, &ihi, lscale, rscale, work, &
	    info);
    if (info != 0) {
	++ninfo;
	lmax[0] = knt;
    }

    dlacpy_("FULL", &n, &m, vl, &c__50, vlf, &c__50);
    dlacpy_("FULL", &n, &m, vr, &c__50, vrf, &c__50);

    dggbak_("B", "L", &n, &ilo, &ihi, lscale, rscale, &m, vl, &c__50, &info);
    if (info != 0) {
	++ninfo;
	lmax[1] = knt;
    }

    dggbak_("B", "R", &n, &ilo, &ihi, lscale, rscale, &m, vr, &c__50, &info);
    if (info != 0) {
	++ninfo;
	lmax[2] = knt;
    }

/*     Test of DGGBAK */

/*     Check tilde(VL)'*A*tilde(VR) - VL'*tilde(A)*VR */
/*     where tilde(A) denotes the transformed matrix. */

    dgemm_("N", "N", &n, &m, &n, &c_b52, af, &c__50, vr, &c__50, &c_b55, work, 
	     &c__50);
    dgemm_("T", "N", &m, &m, &n, &c_b52, vl, &c__50, work, &c__50, &c_b55, e, 
	    &c__50);

    dgemm_("N", "N", &n, &m, &n, &c_b52, a, &c__50, vrf, &c__50, &c_b55, work, 
	     &c__50);
    dgemm_("T", "N", &m, &m, &n, &c_b52, vlf, &c__50, work, &c__50, &c_b55, f, 
	     &c__50);

    vmax = 0.;
    i__1 = m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = vmax, d__3 = (d__1 = e[i__ + j * 50 - 51] - f[i__ + j * 50 
		    - 51], abs(d__1));
	    vmax = max(d__2,d__3);
/* L60: */
	}
/* L70: */
    }
    vmax /= eps * max(anorm,bnorm);
    if (vmax > rmax) {
	lmax[3] = knt;
	rmax = vmax;
    }

/*     Check tilde(VL)'*B*tilde(VR) - VL'*tilde(B)*VR */

    dgemm_("N", "N", &n, &m, &n, &c_b52, bf, &c__50, vr, &c__50, &c_b55, work, 
	     &c__50);
    dgemm_("T", "N", &m, &m, &n, &c_b52, vl, &c__50, work, &c__50, &c_b55, e, 
	    &c__50);

    dgemm_("N", "N", &n, &m, &n, &c_b52, b, &c__50, vrf, &c__50, &c_b55, work, 
	     &c__50);
    dgemm_("T", "N", &m, &m, &n, &c_b52, vlf, &c__50, work, &c__50, &c_b55, f, 
	     &c__50);

    vmax = 0.;
    i__1 = m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = vmax, d__3 = (d__1 = e[i__ + j * 50 - 51] - f[i__ + j * 50 
		    - 51], abs(d__1));
	    vmax = max(d__2,d__3);
/* L80: */
	}
/* L90: */
    }
    vmax /= eps * max(anorm,bnorm);
    if (vmax > rmax) {
	lmax[3] = knt;
	rmax = vmax;
    }

    goto L10;

L100:

    io___34.ciunit = *nout;
    s_wsfe(&io___34);
    e_wsfe();

    io___35.ciunit = *nout;
    s_wsfe(&io___35);
    do_fio(&c__1, (char *)&rmax, (ftnlen)sizeof(doublereal));
    e_wsfe();
    io___36.ciunit = *nout;
    s_wsfe(&io___36);
    do_fio(&c__1, (char *)&lmax[0], (ftnlen)sizeof(integer));
    e_wsfe();
    io___37.ciunit = *nout;
    s_wsfe(&io___37);
    do_fio(&c__1, (char *)&lmax[1], (ftnlen)sizeof(integer));
    e_wsfe();
    io___38.ciunit = *nout;
    s_wsfe(&io___38);
    do_fio(&c__1, (char *)&lmax[2], (ftnlen)sizeof(integer));
    e_wsfe();
    io___39.ciunit = *nout;
    s_wsfe(&io___39);
    do_fio(&c__1, (char *)&lmax[3], (ftnlen)sizeof(integer));
    e_wsfe();
    io___40.ciunit = *nout;
    s_wsfe(&io___40);
    do_fio(&c__1, (char *)&ninfo, (ftnlen)sizeof(integer));
    e_wsfe();
    io___41.ciunit = *nout;
    s_wsfe(&io___41);
    do_fio(&c__1, (char *)&knt, (ftnlen)sizeof(integer));
    e_wsfe();

    return 0;

/*     End of DCHKGK */

} /* dchkgk_ */
