#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__50 = 50;
static real c_b52 = 1.f;
static real c_b55 = 0.f;

/* Subroutine */ int schkgk_(integer *nin, integer *nout)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,\002.. test output of SGGBAK .. \002)";
    static char fmt_9998[] = "(\002 value of largest test error             "
	    "     =\002,e12.3)";
    static char fmt_9997[] = "(\002 example number where SGGBAL info is not "
	    "0    =\002,i4)";
    static char fmt_9996[] = "(\002 example number where SGGBAK(L) info is n"
	    "ot 0 =\002,i4)";
    static char fmt_9995[] = "(\002 example number where SGGBAK(R) info is n"
	    "ot 0 =\002,i4)";
    static char fmt_9994[] = "(\002 example number having largest error     "
	    "     =\002,i4)";
    static char fmt_9992[] = "(\002 number of examples where info is not 0  "
	    "     =\002,i4)";
    static char fmt_9991[] = "(\002 total number of examples tested         "
	    "     =\002,i4)";

    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2, r__3;

    /* Builtin functions */
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void), s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, 
	    char *, ftnlen);

    /* Local variables */
    real a[2500]	/* was [50][50] */, b[2500]	/* was [50][50] */, e[
	    2500]	/* was [50][50] */, f[2500]	/* was [50][50] */;
    integer i__, j, m, n;
    real af[2500]	/* was [50][50] */, bf[2500]	/* was [50][50] */, 
	    vl[2500]	/* was [50][50] */, vr[2500]	/* was [50][50] */;
    integer ihi, ilo;
    real eps, vlf[2500]	/* was [50][50] */;
    integer knt;
    real vrf[2500]	/* was [50][50] */;
    integer info, lmax[4];
    real rmax, vmax, work[2500]	/* was [50][50] */;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    integer ninfo;
    real anorm, bnorm;
    extern /* Subroutine */ int sggbak_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, integer *
), sggbal_(char *, integer *, real *, integer *, 
	    real *, integer *, integer *, integer *, real *, real *, real *, 
	    integer *);
    real lscale[50];
    extern doublereal slamch_(char *);
    real rscale[50];
    extern doublereal slange_(char *, integer *, integer *, real *, integer *, 
	     real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *);

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
    static cilist io___40 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9991, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SCHKGK tests SGGBAK, a routine for backward balancing  of */
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
    rmax = 0.f;

    eps = slamch_("Precision");

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
	    do_lio(&c__4, &c__1, (char *)&a[i__ + j * 50 - 51], (ftnlen)
		    sizeof(real));
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
	    do_lio(&c__4, &c__1, (char *)&b[i__ + j * 50 - 51], (ftnlen)
		    sizeof(real));
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
	    do_lio(&c__4, &c__1, (char *)&vl[i__ + j * 50 - 51], (ftnlen)
		    sizeof(real));
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
	    do_lio(&c__4, &c__1, (char *)&vr[i__ + j * 50 - 51], (ftnlen)
		    sizeof(real));
	}
	e_rsle();
/* L50: */
    }

    ++knt;

    anorm = slange_("M", &n, &n, a, &c__50, work);
    bnorm = slange_("M", &n, &n, b, &c__50, work);

    slacpy_("FULL", &n, &n, a, &c__50, af, &c__50);
    slacpy_("FULL", &n, &n, b, &c__50, bf, &c__50);

    sggbal_("B", &n, a, &c__50, b, &c__50, &ilo, &ihi, lscale, rscale, work, &
	    info);
    if (info != 0) {
	++ninfo;
	lmax[0] = knt;
    }

    slacpy_("FULL", &n, &m, vl, &c__50, vlf, &c__50);
    slacpy_("FULL", &n, &m, vr, &c__50, vrf, &c__50);

    sggbak_("B", "L", &n, &ilo, &ihi, lscale, rscale, &m, vl, &c__50, &info);
    if (info != 0) {
	++ninfo;
	lmax[1] = knt;
    }

    sggbak_("B", "R", &n, &ilo, &ihi, lscale, rscale, &m, vr, &c__50, &info);
    if (info != 0) {
	++ninfo;
	lmax[2] = knt;
    }

/*     Test of SGGBAK */

/*     Check tilde(VL)'*A*tilde(VR) - VL'*tilde(A)*VR */
/*     where tilde(A) denotes the transformed matrix. */

    sgemm_("N", "N", &n, &m, &n, &c_b52, af, &c__50, vr, &c__50, &c_b55, work, 
	     &c__50);
    sgemm_("T", "N", &m, &m, &n, &c_b52, vl, &c__50, work, &c__50, &c_b55, e, 
	    &c__50);

    sgemm_("N", "N", &n, &m, &n, &c_b52, a, &c__50, vrf, &c__50, &c_b55, work, 
	     &c__50);
    sgemm_("T", "N", &m, &m, &n, &c_b52, vlf, &c__50, work, &c__50, &c_b55, f, 
	     &c__50);

    vmax = 0.f;
    i__1 = m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    r__2 = vmax, r__3 = (r__1 = e[i__ + j * 50 - 51] - f[i__ + j * 50 
		    - 51], dabs(r__1));
	    vmax = dmax(r__2,r__3);
/* L60: */
	}
/* L70: */
    }
    vmax /= eps * dmax(anorm,bnorm);
    if (vmax > rmax) {
	lmax[3] = knt;
	rmax = vmax;
    }

/*     Check tilde(VL)'*B*tilde(VR) - VL'*tilde(B)*VR */

    sgemm_("N", "N", &n, &m, &n, &c_b52, bf, &c__50, vr, &c__50, &c_b55, work, 
	     &c__50);
    sgemm_("T", "N", &m, &m, &n, &c_b52, vl, &c__50, work, &c__50, &c_b55, e, 
	    &c__50);

    sgemm_("N", "N", &n, &m, &n, &c_b52, b, &c__50, vrf, &c__50, &c_b55, work, 
	     &c__50);
    sgemm_("T", "N", &m, &m, &n, &c_b52, vlf, &c__50, work, &c__50, &c_b55, f, 
	     &c__50);

    vmax = 0.f;
    i__1 = m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    r__2 = vmax, r__3 = (r__1 = e[i__ + j * 50 - 51] - f[i__ + j * 50 
		    - 51], dabs(r__1));
	    vmax = dmax(r__2,r__3);
/* L80: */
	}
/* L90: */
    }
    vmax /= eps * dmax(anorm,bnorm);
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
    do_fio(&c__1, (char *)&rmax, (ftnlen)sizeof(real));
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

/*     End of SCHKGK */

} /* schkgk_ */
