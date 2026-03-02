#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__7 = 7;
static integer c__5 = 5;
static integer c__20 = 20;
static integer c__1200 = 1200;

/* Subroutine */ int zget38_(doublereal *rmax, integer *lmax, integer *ninfo, 
	integer *knt, integer *nin)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);
    double d_imag(doublecomplex *);

    /* Local variables */
    integer i__, j, m, n;
    doublecomplex q[400]	/* was [20][20] */;
    doublereal s;
    doublecomplex t[400]	/* was [20][20] */;
    doublereal v;
    doublecomplex w[20];
    doublereal val[3], eps, sep, sin__, tol;
    doublecomplex tmp[400]	/* was [20][20] */;
    integer ndim, iscl, info, kmin, itmp;
    doublereal vmin, vmax;
    integer ipnt[20];
    doublecomplex qsav[400]	/* was [20][20] */, tsav[400]	/* was [20][
	    20] */;
    doublereal tnrm;
    integer isrt;
    doublecomplex qtmp[400]	/* was [20][20] */;
    doublereal stmp, vmul;
    doublecomplex ttmp[400]	/* was [20][20] */, work[1200], wtmp[20];
    doublereal wsrt[20];
    doublecomplex tsav1[400]	/* was [20][20] */;
    doublereal sepin, tolin;
    extern /* Subroutine */ int zhst01_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *);
    doublereal rwork[20];
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    integer iselec[20];
    logical select[20];
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    doublereal bignum;
    extern /* Subroutine */ int zdscal_(integer *, doublereal *, 
	    doublecomplex *, integer *), zgehrd_(integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, integer *), zlacpy_(char *, integer *, 
	     integer *, doublecomplex *, integer *, doublecomplex *, integer *
);
    doublereal septmp, smlnum;
    extern /* Subroutine */ int zhseqr_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *), zunghr_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, integer *);
    doublereal result[2];
    extern /* Subroutine */ int ztrsen_(char *, char *, logical *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 0, 0, 0, 0 };
    static cilist io___9 = { 0, 0, 0, 0, 0 };
    static cilist io___12 = { 0, 0, 0, 0, 0 };
    static cilist io___15 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGET38 tests ZTRSEN, a routine for estimating condition numbers of a */
/*  cluster of eigenvalues and/or its associated right invariant subspace */

/*  The test matrices are read from a file with logical unit number NIN. */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) DOUBLE PRECISION array, dimension (3) */
/*          Values of the largest test ratios. */
/*          RMAX(1) = largest residuals from ZHST01 or comparing */
/*                    different calls to ZTRSEN */
/*          RMAX(2) = largest error in reciprocal condition */
/*                    numbers taking their conditioning into account */
/*          RMAX(3) = largest error in reciprocal condition */
/*                    numbers not taking their conditioning into */
/*                    account (may be larger than RMAX(2)) */

/*  LMAX    (output) INTEGER array, dimension (3) */
/*          LMAX(i) is example number where largest test ratio */
/*          RMAX(i) is achieved. Also: */
/*          If ZGEHRD returns INFO nonzero on example i, LMAX(1)=i */
/*          If ZHSEQR returns INFO nonzero on example i, LMAX(2)=i */
/*          If ZTRSEN returns INFO nonzero on example i, LMAX(3)=i */

/*  NINFO   (output) INTEGER array, dimension (3) */
/*          NINFO(1) = No. of times ZGEHRD returned INFO nonzero */
/*          NINFO(2) = No. of times ZHSEQR returned INFO nonzero */
/*          NINFO(3) = No. of times ZTRSEN returned INFO nonzero */

/*  KNT     (output) INTEGER */
/*          Total number of examples tested. */

/*  NIN     (input) INTEGER */
/*          Input logical unit number. */

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

    /* Parameter adjustments */
    --ninfo;
    --lmax;
    --rmax;

    /* Function Body */
    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     EPSIN = 2**(-24) = precision to which input data computed */

    eps = max(eps,5.9605e-8);
    rmax[1] = 0.;
    rmax[2] = 0.;
    rmax[3] = 0.;
    lmax[1] = 0;
    lmax[2] = 0;
    lmax[3] = 0;
    *knt = 0;
    ninfo[1] = 0;
    ninfo[2] = 0;
    ninfo[3] = 0;
    val[0] = sqrt(smlnum);
    val[1] = 1.;
    val[2] = sqrt(sqrt(bignum));

/*     Read input data until N=0.  Assume input eigenvalues are sorted */
/*     lexicographically (increasing by real part, then decreasing by */
/*     imaginary part) */

L10:
    io___5.ciunit = *nin;
    s_rsle(&io___5);
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&ndim, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&isrt, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	return 0;
    }
    io___9.ciunit = *nin;
    s_rsle(&io___9);
    i__1 = ndim;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&iselec[i__ - 1], (ftnlen)sizeof(integer)
		);
    }
    e_rsle();
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___12.ciunit = *nin;
	s_rsle(&io___12);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__7, &c__1, (char *)&tmp[i__ + j * 20 - 21], (ftnlen)
		    sizeof(doublecomplex));
	}
	e_rsle();
/* L20: */
    }
    io___15.ciunit = *nin;
    s_rsle(&io___15);
    do_lio(&c__5, &c__1, (char *)&sin__, (ftnlen)sizeof(doublereal));
    do_lio(&c__5, &c__1, (char *)&sepin, (ftnlen)sizeof(doublereal));
    e_rsle();

    tnrm = zlange_("M", &n, &n, tmp, &c__20, rwork);
    for (iscl = 1; iscl <= 3; ++iscl) {

/*        Scale input matrix */

	++(*knt);
	zlacpy_("F", &n, &n, tmp, &c__20, t, &c__20);
	vmul = val[iscl - 1];
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    zdscal_(&n, &vmul, &t[i__ * 20 - 20], &c__1);
/* L30: */
	}
	if (tnrm == 0.) {
	    vmul = 1.;
	}
	zlacpy_("F", &n, &n, t, &c__20, tsav, &c__20);

/*        Compute Schur form */

	i__1 = 1200 - n;
	zgehrd_(&n, &c__1, &n, t, &c__20, work, &work[n], &i__1, &info);
	if (info != 0) {
	    lmax[1] = *knt;
	    ++ninfo[1];
	    goto L200;
	}

/*        Generate unitary matrix */

	zlacpy_("L", &n, &n, t, &c__20, q, &c__20);
	i__1 = 1200 - n;
	zunghr_(&n, &c__1, &n, q, &c__20, work, &work[n], &i__1, &info);

/*        Compute Schur form */

	i__1 = n - 2;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = n;
	    for (i__ = j + 2; i__ <= i__2; ++i__) {
		i__3 = i__ + j * 20 - 21;
		t[i__3].r = 0., t[i__3].i = 0.;
/* L40: */
	    }
/* L50: */
	}
	zhseqr_("S", "V", &n, &c__1, &n, t, &c__20, w, q, &c__20, work, &
		c__1200, &info);
	if (info != 0) {
	    lmax[2] = *knt;
	    ++ninfo[2];
	    goto L200;
	}

/*        Sort, select eigenvalues */

	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ipnt[i__ - 1] = i__;
	    select[i__ - 1] = FALSE_;
/* L60: */
	}
	if (isrt == 0) {
	    i__1 = n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = i__ - 1;
		wsrt[i__ - 1] = w[i__2].r;
/* L70: */
	    }
	} else {
	    i__1 = n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		wsrt[i__ - 1] = d_imag(&w[i__ - 1]);
/* L80: */
	    }
	}
	i__1 = n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    kmin = i__;
	    vmin = wsrt[i__ - 1];
	    i__2 = n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		if (wsrt[j - 1] < vmin) {
		    kmin = j;
		    vmin = wsrt[j - 1];
		}
/* L90: */
	    }
	    wsrt[kmin - 1] = wsrt[i__ - 1];
	    wsrt[i__ - 1] = vmin;
	    itmp = ipnt[i__ - 1];
	    ipnt[i__ - 1] = ipnt[kmin - 1];
	    ipnt[kmin - 1] = itmp;
/* L100: */
	}
	i__1 = ndim;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    select[ipnt[iselec[i__ - 1] - 1] - 1] = TRUE_;
/* L110: */
	}

/*        Compute condition numbers */

	zlacpy_("F", &n, &n, q, &c__20, qsav, &c__20);
	zlacpy_("F", &n, &n, t, &c__20, tsav1, &c__20);
	ztrsen_("B", "V", select, &n, t, &c__20, q, &c__20, wtmp, &m, &s, &
		sep, work, &c__1200, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L200;
	}
	septmp = sep / vmul;
	stmp = s;

/*        Compute residuals */

	zhst01_(&n, &c__1, &n, tsav, &c__20, t, &c__20, q, &c__20, work, &
		c__1200, rwork, result);
	vmax = max(result[0],result[1]);
	if (vmax > rmax[1]) {
	    rmax[1] = vmax;
	    if (ninfo[1] == 0) {
		lmax[1] = *knt;
	    }
	}

/*        Compare condition number for eigenvalue cluster */
/*        taking its condition number into account */

/* Computing MAX */
	d__1 = (doublereal) n * 2. * eps * tnrm;
	v = max(d__1,smlnum);
	if (tnrm == 0.) {
	    v = 1.;
	}
	if (v > septmp) {
	    tol = 1.;
	} else {
	    tol = v / septmp;
	}
	if (v > sepin) {
	    tolin = 1.;
	} else {
	    tolin = v / sepin;
	}
/* Computing MAX */
	d__1 = tol, d__2 = smlnum / eps;
	tol = max(d__1,d__2);
/* Computing MAX */
	d__1 = tolin, d__2 = smlnum / eps;
	tolin = max(d__1,d__2);
	if (eps * (sin__ - tolin) > stmp + tol) {
	    vmax = 1. / eps;
	} else if (sin__ - tolin > stmp + tol) {
	    vmax = (sin__ - tolin) / (stmp + tol);
	} else if (sin__ + tolin < eps * (stmp - tol)) {
	    vmax = 1. / eps;
	} else if (sin__ + tolin < stmp - tol) {
	    vmax = (stmp - tol) / (sin__ + tolin);
	} else {
	    vmax = 1.;
	}
	if (vmax > rmax[2]) {
	    rmax[2] = vmax;
	    if (ninfo[2] == 0) {
		lmax[2] = *knt;
	    }
	}

/*        Compare condition numbers for invariant subspace */
/*        taking its condition number into account */

	if (v > septmp * stmp) {
	    tol = septmp;
	} else {
	    tol = v / stmp;
	}
	if (v > sepin * sin__) {
	    tolin = sepin;
	} else {
	    tolin = v / sin__;
	}
/* Computing MAX */
	d__1 = tol, d__2 = smlnum / eps;
	tol = max(d__1,d__2);
/* Computing MAX */
	d__1 = tolin, d__2 = smlnum / eps;
	tolin = max(d__1,d__2);
	if (eps * (sepin - tolin) > septmp + tol) {
	    vmax = 1. / eps;
	} else if (sepin - tolin > septmp + tol) {
	    vmax = (sepin - tolin) / (septmp + tol);
	} else if (sepin + tolin < eps * (septmp - tol)) {
	    vmax = 1. / eps;
	} else if (sepin + tolin < septmp - tol) {
	    vmax = (septmp - tol) / (sepin + tolin);
	} else {
	    vmax = 1.;
	}
	if (vmax > rmax[2]) {
	    rmax[2] = vmax;
	    if (ninfo[2] == 0) {
		lmax[2] = *knt;
	    }
	}

/*        Compare condition number for eigenvalue cluster */
/*        without taking its condition number into account */

	if (sin__ <= (doublereal) (n << 1) * eps && stmp <= (doublereal) (n <<
		 1) * eps) {
	    vmax = 1.;
	} else if (eps * sin__ > stmp) {
	    vmax = 1. / eps;
	} else if (sin__ > stmp) {
	    vmax = sin__ / stmp;
	} else if (sin__ < eps * stmp) {
	    vmax = 1. / eps;
	} else if (sin__ < stmp) {
	    vmax = stmp / sin__;
	} else {
	    vmax = 1.;
	}
	if (vmax > rmax[3]) {
	    rmax[3] = vmax;
	    if (ninfo[3] == 0) {
		lmax[3] = *knt;
	    }
	}

/*        Compare condition numbers for invariant subspace */
/*        without taking its condition number into account */

	if (sepin <= v && septmp <= v) {
	    vmax = 1.;
	} else if (eps * sepin > septmp) {
	    vmax = 1. / eps;
	} else if (sepin > septmp) {
	    vmax = sepin / septmp;
	} else if (sepin < eps * septmp) {
	    vmax = 1. / eps;
	} else if (sepin < septmp) {
	    vmax = septmp / sepin;
	} else {
	    vmax = 1.;
	}
	if (vmax > rmax[3]) {
	    rmax[3] = vmax;
	    if (ninfo[3] == 0) {
		lmax[3] = *knt;
	    }
	}

/*        Compute eigenvalue condition number only and compare */
/*        Update Q */

	vmax = 0.;
	zlacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	zlacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.;
	stmp = -1.;
	ztrsen_("E", "V", select, &n, ttmp, &c__20, qtmp, &c__20, wtmp, &m, &
		stmp, &septmp, work, &c__1200, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L200;
	}
	if (s != stmp) {
	    vmax = 1. / eps;
	}
	if (-1. != septmp) {
	    vmax = 1. / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (ttmp[i__3].r != t[i__4].r || ttmp[i__3].i != t[i__4].i) {
		    vmax = 1. / eps;
		}
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (qtmp[i__3].r != q[i__4].r || qtmp[i__3].i != q[i__4].i) {
		    vmax = 1. / eps;
		}
/* L120: */
	    }
/* L130: */
	}

/*        Compute invariant subspace condition number only and compare */
/*        Update Q */

	zlacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	zlacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.;
	stmp = -1.;
	ztrsen_("V", "V", select, &n, ttmp, &c__20, qtmp, &c__20, wtmp, &m, &
		stmp, &septmp, work, &c__1200, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L200;
	}
	if (-1. != stmp) {
	    vmax = 1. / eps;
	}
	if (sep != septmp) {
	    vmax = 1. / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (ttmp[i__3].r != t[i__4].r || ttmp[i__3].i != t[i__4].i) {
		    vmax = 1. / eps;
		}
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (qtmp[i__3].r != q[i__4].r || qtmp[i__3].i != q[i__4].i) {
		    vmax = 1. / eps;
		}
/* L140: */
	    }
/* L150: */
	}

/*        Compute eigenvalue condition number only and compare */
/*        Do not update Q */

	zlacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	zlacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.;
	stmp = -1.;
	ztrsen_("E", "N", select, &n, ttmp, &c__20, qtmp, &c__20, wtmp, &m, &
		stmp, &septmp, work, &c__1200, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L200;
	}
	if (s != stmp) {
	    vmax = 1. / eps;
	}
	if (-1. != septmp) {
	    vmax = 1. / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (ttmp[i__3].r != t[i__4].r || ttmp[i__3].i != t[i__4].i) {
		    vmax = 1. / eps;
		}
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (qtmp[i__3].r != qsav[i__4].r || qtmp[i__3].i != qsav[i__4]
			.i) {
		    vmax = 1. / eps;
		}
/* L160: */
	    }
/* L170: */
	}

/*        Compute invariant subspace condition number only and compare */
/*        Do not update Q */

	zlacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	zlacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.;
	stmp = -1.;
	ztrsen_("V", "N", select, &n, ttmp, &c__20, qtmp, &c__20, wtmp, &m, &
		stmp, &septmp, work, &c__1200, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L200;
	}
	if (-1. != stmp) {
	    vmax = 1. / eps;
	}
	if (sep != septmp) {
	    vmax = 1. / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (ttmp[i__3].r != t[i__4].r || ttmp[i__3].i != t[i__4].i) {
		    vmax = 1. / eps;
		}
		i__3 = i__ + j * 20 - 21;
		i__4 = i__ + j * 20 - 21;
		if (qtmp[i__3].r != qsav[i__4].r || qtmp[i__3].i != qsav[i__4]
			.i) {
		    vmax = 1. / eps;
		}
/* L180: */
	    }
/* L190: */
	}
	if (vmax > rmax[1]) {
	    rmax[1] = vmax;
	    if (ninfo[1] == 0) {
		lmax[1] = *knt;
	    }
	}
L200:
	;
    }
    goto L10;

/*     End of ZGET38 */

} /* zget38_ */
