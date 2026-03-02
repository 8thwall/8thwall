#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__20 = 20;
static integer c__1200 = 1200;
static integer c__400 = 400;

/* Subroutine */ int sget38_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt, integer *nin)
{
    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);

    /* Local variables */
    integer i__, j, m, n;
    real q[400]	/* was [20][20] */, s, t[400]	/* was [20][20] */, v, wi[20],
	     wr[20], val[3], eps, sep, sin__, tol, tmp[400]	/* was [20][
	    20] */;
    integer ndim, iscl, info, kmin, itmp, ipnt[20];
    real vmax, qsav[400]	/* was [20][20] */, tsav[400]	/* was [20][
	    20] */, tnrm, qtmp[400]	/* was [20][20] */, work[1200], stmp, 
	    vmul, ttmp[400]	/* was [20][20] */, tsav1[400]	/* was [20][
	    20] */;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    real sepin, vimin;
    extern /* Subroutine */ int shst01_(integer *, integer *, integer *, real 
	    *, integer *, real *, integer *, real *, integer *, real *, 
	    integer *, real *);
    real tolin, vrmin;
    integer iwork[400];
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    real witmp[20], wrtmp[20];
    extern /* Subroutine */ int slabad_(real *, real *);
    integer iselec[20];
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int sgehrd_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *);
    logical select[20];
    real bignum;
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), sorghr_(integer *, integer 
	    *, integer *, real *, integer *, real *, real *, integer *, 
	    integer *), shseqr_(char *, char *, integer *, integer *, integer 
	    *, real *, integer *, real *, real *, real *, integer *, real *, 
	    integer *, integer *);
    real septmp, smlnum, result[2];
    extern /* Subroutine */ int strsen_(char *, char *, logical *, integer *, 
	    real *, integer *, real *, integer *, real *, real *, integer *, 
	    real *, real *, real *, integer *, integer *, integer *, integer *
);

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 0, 0, 0, 0 };
    static cilist io___8 = { 0, 0, 0, 0, 0 };
    static cilist io___11 = { 0, 0, 0, 0, 0 };
    static cilist io___14 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET38 tests STRSEN, a routine for estimating condition numbers of a */
/*  cluster of eigenvalues and/or its associated right invariant subspace */

/*  The test matrices are read from a file with logical unit number NIN. */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) REAL array, dimension (3) */
/*          Values of the largest test ratios. */
/*          RMAX(1) = largest residuals from SHST01 or comparing */
/*                    different calls to STRSEN */
/*          RMAX(2) = largest error in reciprocal condition */
/*                    numbers taking their conditioning into account */
/*          RMAX(3) = largest error in reciprocal condition */
/*                    numbers not taking their conditioning into */
/*                    account (may be larger than RMAX(2)) */

/*  LMAX    (output) INTEGER array, dimension (3) */
/*          LMAX(i) is example number where largest test ratio */
/*          RMAX(i) is achieved. Also: */
/*          If SGEHRD returns INFO nonzero on example i, LMAX(1)=i */
/*          If SHSEQR returns INFO nonzero on example i, LMAX(2)=i */
/*          If STRSEN returns INFO nonzero on example i, LMAX(3)=i */

/*  NINFO   (output) INTEGER array, dimension (3) */
/*          NINFO(1) = No. of times SGEHRD returned INFO nonzero */
/*          NINFO(2) = No. of times SHSEQR returned INFO nonzero */
/*          NINFO(3) = No. of times STRSEN returned INFO nonzero */

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
    eps = slamch_("P");
    smlnum = slamch_("S") / eps;
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);

/*     EPSIN = 2**(-24) = precision to which input data computed */

    eps = dmax(eps,5.9605e-8f);
    rmax[1] = 0.f;
    rmax[2] = 0.f;
    rmax[3] = 0.f;
    lmax[1] = 0;
    lmax[2] = 0;
    lmax[3] = 0;
    *knt = 0;
    ninfo[1] = 0;
    ninfo[2] = 0;
    ninfo[3] = 0;

    val[0] = sqrt(smlnum);
    val[1] = 1.f;
    val[2] = sqrt(sqrt(bignum));

/*     Read input data until N=0.  Assume input eigenvalues are sorted */
/*     lexicographically (increasing by real part, then decreasing by */
/*     imaginary part) */

L10:
    io___5.ciunit = *nin;
    s_rsle(&io___5);
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&ndim, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	return 0;
    }
    io___8.ciunit = *nin;
    s_rsle(&io___8);
    i__1 = ndim;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&iselec[i__ - 1], (ftnlen)sizeof(integer)
		);
    }
    e_rsle();
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___11.ciunit = *nin;
	s_rsle(&io___11);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__4, &c__1, (char *)&tmp[i__ + j * 20 - 21], (ftnlen)
		    sizeof(real));
	}
	e_rsle();
/* L20: */
    }
    io___14.ciunit = *nin;
    s_rsle(&io___14);
    do_lio(&c__4, &c__1, (char *)&sin__, (ftnlen)sizeof(real));
    do_lio(&c__4, &c__1, (char *)&sepin, (ftnlen)sizeof(real));
    e_rsle();

    tnrm = slange_("M", &n, &n, tmp, &c__20, work);
    for (iscl = 1; iscl <= 3; ++iscl) {

/*        Scale input matrix */

	++(*knt);
	slacpy_("F", &n, &n, tmp, &c__20, t, &c__20);
	vmul = val[iscl - 1];
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    sscal_(&n, &vmul, &t[i__ * 20 - 20], &c__1);
/* L30: */
	}
	if (tnrm == 0.f) {
	    vmul = 1.f;
	}
	slacpy_("F", &n, &n, t, &c__20, tsav, &c__20);

/*        Compute Schur form */

	i__1 = 1200 - n;
	sgehrd_(&n, &c__1, &n, t, &c__20, work, &work[n], &i__1, &info);
	if (info != 0) {
	    lmax[1] = *knt;
	    ++ninfo[1];
	    goto L160;
	}

/*        Generate orthogonal matrix */

	slacpy_("L", &n, &n, t, &c__20, q, &c__20);
	i__1 = 1200 - n;
	sorghr_(&n, &c__1, &n, q, &c__20, work, &work[n], &i__1, &info);

/*        Compute Schur form */

	shseqr_("S", "V", &n, &c__1, &n, t, &c__20, wr, wi, q, &c__20, work, &
		c__1200, &info);
	if (info != 0) {
	    lmax[2] = *knt;
	    ++ninfo[2];
	    goto L160;
	}

/*        Sort, select eigenvalues */

	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ipnt[i__ - 1] = i__;
	    select[i__ - 1] = FALSE_;
/* L40: */
	}
	scopy_(&n, wr, &c__1, wrtmp, &c__1);
	scopy_(&n, wi, &c__1, witmp, &c__1);
	i__1 = n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    kmin = i__;
	    vrmin = wrtmp[i__ - 1];
	    vimin = witmp[i__ - 1];
	    i__2 = n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		if (wrtmp[j - 1] < vrmin) {
		    kmin = j;
		    vrmin = wrtmp[j - 1];
		    vimin = witmp[j - 1];
		}
/* L50: */
	    }
	    wrtmp[kmin - 1] = wrtmp[i__ - 1];
	    witmp[kmin - 1] = witmp[i__ - 1];
	    wrtmp[i__ - 1] = vrmin;
	    witmp[i__ - 1] = vimin;
	    itmp = ipnt[i__ - 1];
	    ipnt[i__ - 1] = ipnt[kmin - 1];
	    ipnt[kmin - 1] = itmp;
/* L60: */
	}
	i__1 = ndim;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    select[ipnt[iselec[i__ - 1] - 1] - 1] = TRUE_;
/* L70: */
	}

/*        Compute condition numbers */

	slacpy_("F", &n, &n, q, &c__20, qsav, &c__20);
	slacpy_("F", &n, &n, t, &c__20, tsav1, &c__20);
	strsen_("B", "V", select, &n, t, &c__20, q, &c__20, wrtmp, witmp, &m, 
		&s, &sep, work, &c__1200, iwork, &c__400, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L160;
	}
	septmp = sep / vmul;
	stmp = s;

/*        Compute residuals */

	shst01_(&n, &c__1, &n, tsav, &c__20, t, &c__20, q, &c__20, work, &
		c__1200, result);
	vmax = dmax(result[0],result[1]);
	if (vmax > rmax[1]) {
	    rmax[1] = vmax;
	    if (ninfo[1] == 0) {
		lmax[1] = *knt;
	    }
	}

/*        Compare condition number for eigenvalue cluster */
/*        taking its condition number into account */

/* Computing MAX */
	r__1 = (real) n * 2.f * eps * tnrm;
	v = dmax(r__1,smlnum);
	if (tnrm == 0.f) {
	    v = 1.f;
	}
	if (v > septmp) {
	    tol = 1.f;
	} else {
	    tol = v / septmp;
	}
	if (v > sepin) {
	    tolin = 1.f;
	} else {
	    tolin = v / sepin;
	}
/* Computing MAX */
	r__1 = tol, r__2 = smlnum / eps;
	tol = dmax(r__1,r__2);
/* Computing MAX */
	r__1 = tolin, r__2 = smlnum / eps;
	tolin = dmax(r__1,r__2);
	if (eps * (sin__ - tolin) > stmp + tol) {
	    vmax = 1.f / eps;
	} else if (sin__ - tolin > stmp + tol) {
	    vmax = (sin__ - tolin) / (stmp + tol);
	} else if (sin__ + tolin < eps * (stmp - tol)) {
	    vmax = 1.f / eps;
	} else if (sin__ + tolin < stmp - tol) {
	    vmax = (stmp - tol) / (sin__ + tolin);
	} else {
	    vmax = 1.f;
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
	r__1 = tol, r__2 = smlnum / eps;
	tol = dmax(r__1,r__2);
/* Computing MAX */
	r__1 = tolin, r__2 = smlnum / eps;
	tolin = dmax(r__1,r__2);
	if (eps * (sepin - tolin) > septmp + tol) {
	    vmax = 1.f / eps;
	} else if (sepin - tolin > septmp + tol) {
	    vmax = (sepin - tolin) / (septmp + tol);
	} else if (sepin + tolin < eps * (septmp - tol)) {
	    vmax = 1.f / eps;
	} else if (sepin + tolin < septmp - tol) {
	    vmax = (septmp - tol) / (sepin + tolin);
	} else {
	    vmax = 1.f;
	}
	if (vmax > rmax[2]) {
	    rmax[2] = vmax;
	    if (ninfo[2] == 0) {
		lmax[2] = *knt;
	    }
	}

/*        Compare condition number for eigenvalue cluster */
/*        without taking its condition number into account */

	if (sin__ <= (real) (n << 1) * eps && stmp <= (real) (n << 1) * eps) {
	    vmax = 1.f;
	} else if (eps * sin__ > stmp) {
	    vmax = 1.f / eps;
	} else if (sin__ > stmp) {
	    vmax = sin__ / stmp;
	} else if (sin__ < eps * stmp) {
	    vmax = 1.f / eps;
	} else if (sin__ < stmp) {
	    vmax = stmp / sin__;
	} else {
	    vmax = 1.f;
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
	    vmax = 1.f;
	} else if (eps * sepin > septmp) {
	    vmax = 1.f / eps;
	} else if (sepin > septmp) {
	    vmax = sepin / septmp;
	} else if (sepin < eps * septmp) {
	    vmax = 1.f / eps;
	} else if (sepin < septmp) {
	    vmax = septmp / sepin;
	} else {
	    vmax = 1.f;
	}
	if (vmax > rmax[3]) {
	    rmax[3] = vmax;
	    if (ninfo[3] == 0) {
		lmax[3] = *knt;
	    }
	}

/*        Compute eigenvalue condition number only and compare */
/*        Update Q */

	vmax = 0.f;
	slacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	slacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.f;
	stmp = -1.f;
	strsen_("E", "V", select, &n, ttmp, &c__20, qtmp, &c__20, wrtmp, 
		witmp, &m, &stmp, &septmp, work, &c__1200, iwork, &c__400, &
		info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L160;
	}
	if (s != stmp) {
	    vmax = 1.f / eps;
	}
	if (-1.f != septmp) {
	    vmax = 1.f / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		if (ttmp[i__ + j * 20 - 21] != t[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
		if (qtmp[i__ + j * 20 - 21] != q[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
/* L80: */
	    }
/* L90: */
	}

/*        Compute invariant subspace condition number only and compare */
/*        Update Q */

	slacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	slacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.f;
	stmp = -1.f;
	strsen_("V", "V", select, &n, ttmp, &c__20, qtmp, &c__20, wrtmp, 
		witmp, &m, &stmp, &septmp, work, &c__1200, iwork, &c__400, &
		info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L160;
	}
	if (-1.f != stmp) {
	    vmax = 1.f / eps;
	}
	if (sep != septmp) {
	    vmax = 1.f / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		if (ttmp[i__ + j * 20 - 21] != t[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
		if (qtmp[i__ + j * 20 - 21] != q[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
/* L100: */
	    }
/* L110: */
	}

/*        Compute eigenvalue condition number only and compare */
/*        Do not update Q */

	slacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	slacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.f;
	stmp = -1.f;
	strsen_("E", "N", select, &n, ttmp, &c__20, qtmp, &c__20, wrtmp, 
		witmp, &m, &stmp, &septmp, work, &c__1200, iwork, &c__400, &
		info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L160;
	}
	if (s != stmp) {
	    vmax = 1.f / eps;
	}
	if (-1.f != septmp) {
	    vmax = 1.f / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		if (ttmp[i__ + j * 20 - 21] != t[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
		if (qtmp[i__ + j * 20 - 21] != qsav[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
/* L120: */
	    }
/* L130: */
	}

/*        Compute invariant subspace condition number only and compare */
/*        Do not update Q */

	slacpy_("F", &n, &n, tsav1, &c__20, ttmp, &c__20);
	slacpy_("F", &n, &n, qsav, &c__20, qtmp, &c__20);
	septmp = -1.f;
	stmp = -1.f;
	strsen_("V", "N", select, &n, ttmp, &c__20, qtmp, &c__20, wrtmp, 
		witmp, &m, &stmp, &septmp, work, &c__1200, iwork, &c__400, &
		info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L160;
	}
	if (-1.f != stmp) {
	    vmax = 1.f / eps;
	}
	if (sep != septmp) {
	    vmax = 1.f / eps;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = n;
	    for (j = 1; j <= i__2; ++j) {
		if (ttmp[i__ + j * 20 - 21] != t[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
		if (qtmp[i__ + j * 20 - 21] != qsav[i__ + j * 20 - 21]) {
		    vmax = 1.f / eps;
		}
/* L140: */
	    }
/* L150: */
	}
	if (vmax > rmax[1]) {
	    rmax[1] = vmax;
	    if (ninfo[1] == 0) {
		lmax[1] = *knt;
	    }
	}
L160:
	;
    }
    goto L10;

/*     End of SGET38 */

} /* sget38_ */
