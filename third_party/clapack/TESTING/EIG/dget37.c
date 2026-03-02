#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__5 = 5;
static integer c__20 = 20;
static integer c__1200 = 1200;
static integer c__0 = 0;

/* Subroutine */ int dget37_(doublereal *rmax, integer *lmax, integer *ninfo, 
	integer *knt, integer *nin)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);

    /* Local variables */
    integer i__, j, m, n;
    doublereal s[20], t[400]	/* was [20][20] */, v, le[400]	/* was [20][
	    20] */, re[400]	/* was [20][20] */, wi[20], wr[20], val[3], 
	    dum[1], eps, sep[20], sin__[20], tol, tmp[400]	/* was [20][
	    20] */;
    integer ifnd, icmp, iscl, info, lcmp[3], kmin;
    doublereal wiin[20], vmax, tnrm, wrin[20], work[1200], vmul, stmp[20];
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *);
    doublereal sepin[20];
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    doublereal vimin, tolin, vrmin;
    integer iwork[40];
    doublereal witmp[20], wrtmp[20];
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dgehrd_(integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *), dlacpy_(char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *);
    logical select[20];
    doublereal bignum;
    extern /* Subroutine */ int dhseqr_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), dtrevc_(char *, char *, logical *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, integer *, integer *, doublereal *, integer *), dtrsna_(char *, char *, logical *, integer *, doublereal 
	    *, integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *, integer *);
    doublereal septmp[20], smlnum;

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 0, 0, 0, 0 };
    static cilist io___8 = { 0, 0, 0, 0, 0 };
    static cilist io___11 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET37 tests DTRSNA, a routine for estimating condition numbers of */
/*  eigenvalues and/or right eigenvectors of a matrix. */

/*  The test matrices are read from a file with logical unit number NIN. */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) DOUBLE PRECISION array, dimension (3) */
/*          Value of the largest test ratio. */
/*          RMAX(1) = largest ratio comparing different calls to DTRSNA */
/*          RMAX(2) = largest error in reciprocal condition */
/*                    numbers taking their conditioning into account */
/*          RMAX(3) = largest error in reciprocal condition */
/*                    numbers not taking their conditioning into */
/*                    account (may be larger than RMAX(2)) */

/*  LMAX    (output) INTEGER array, dimension (3) */
/*          LMAX(i) is example number where largest test ratio */
/*          RMAX(i) is achieved. Also: */
/*          If DGEHRD returns INFO nonzero on example i, LMAX(1)=i */
/*          If DHSEQR returns INFO nonzero on example i, LMAX(2)=i */
/*          If DTRSNA returns INFO nonzero on example i, LMAX(3)=i */

/*  NINFO   (output) INTEGER array, dimension (3) */
/*          NINFO(1) = No. of times DGEHRD returned INFO nonzero */
/*          NINFO(2) = No. of times DHSEQR returned INFO nonzero */
/*          NINFO(3) = No. of times DTRSNA returned INFO nonzero */

/*  KNT     (output) INTEGER */
/*          Total number of examples tested. */

/*  NIN     (input) INTEGER */
/*          Input logical unit number */

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
    val[2] = sqrt(bignum);

/*     Read input data until N=0.  Assume input eigenvalues are sorted */
/*     lexicographically (increasing by real part, then decreasing by */
/*     imaginary part) */

L10:
    io___5.ciunit = *nin;
    s_rsle(&io___5);
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	return 0;
    }
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___8.ciunit = *nin;
	s_rsle(&io___8);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__5, &c__1, (char *)&tmp[i__ + j * 20 - 21], (ftnlen)
		    sizeof(doublereal));
	}
	e_rsle();
/* L20: */
    }
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___11.ciunit = *nin;
	s_rsle(&io___11);
	do_lio(&c__5, &c__1, (char *)&wrin[i__ - 1], (ftnlen)sizeof(
		doublereal));
	do_lio(&c__5, &c__1, (char *)&wiin[i__ - 1], (ftnlen)sizeof(
		doublereal));
	do_lio(&c__5, &c__1, (char *)&sin__[i__ - 1], (ftnlen)sizeof(
		doublereal));
	do_lio(&c__5, &c__1, (char *)&sepin[i__ - 1], (ftnlen)sizeof(
		doublereal));
	e_rsle();
/* L30: */
    }
    tnrm = dlange_("M", &n, &n, tmp, &c__20, work);

/*     Begin test */

    for (iscl = 1; iscl <= 3; ++iscl) {

/*        Scale input matrix */

	++(*knt);
	dlacpy_("F", &n, &n, tmp, &c__20, t, &c__20);
	vmul = val[iscl - 1];
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dscal_(&n, &vmul, &t[i__ * 20 - 20], &c__1);
/* L40: */
	}
	if (tnrm == 0.) {
	    vmul = 1.;
	}

/*        Compute eigenvalues and eigenvectors */

	i__1 = 1200 - n;
	dgehrd_(&n, &c__1, &n, t, &c__20, work, &work[n], &i__1, &info);
	if (info != 0) {
	    lmax[1] = *knt;
	    ++ninfo[1];
	    goto L240;
	}
	i__1 = n - 2;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = n;
	    for (i__ = j + 2; i__ <= i__2; ++i__) {
		t[i__ + j * 20 - 21] = 0.;
/* L50: */
	    }
/* L60: */
	}

/*        Compute Schur form */

	dhseqr_("S", "N", &n, &c__1, &n, t, &c__20, wr, wi, dum, &c__1, work, 
		&c__1200, &info);
	if (info != 0) {
	    lmax[2] = *knt;
	    ++ninfo[2];
	    goto L240;
	}

/*        Compute eigenvectors */

	dtrevc_("Both", "All", select, &n, t, &c__20, le, &c__20, re, &c__20, 
		&n, &m, work, &info);

/*        Compute condition numbers */

	dtrsna_("Both", "All", select, &n, t, &c__20, le, &c__20, re, &c__20, 
		s, sep, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}

/*        Sort eigenvalues and condition numbers lexicographically */
/*        to compare with inputs */

	dcopy_(&n, wr, &c__1, wrtmp, &c__1);
	dcopy_(&n, wi, &c__1, witmp, &c__1);
	dcopy_(&n, s, &c__1, stmp, &c__1);
	dcopy_(&n, sep, &c__1, septmp, &c__1);
	d__1 = 1. / vmul;
	dscal_(&n, &d__1, septmp, &c__1);
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
/* L70: */
	    }
	    wrtmp[kmin - 1] = wrtmp[i__ - 1];
	    witmp[kmin - 1] = witmp[i__ - 1];
	    wrtmp[i__ - 1] = vrmin;
	    witmp[i__ - 1] = vimin;
	    vrmin = stmp[kmin - 1];
	    stmp[kmin - 1] = stmp[i__ - 1];
	    stmp[i__ - 1] = vrmin;
	    vrmin = septmp[kmin - 1];
	    septmp[kmin - 1] = septmp[i__ - 1];
	    septmp[i__ - 1] = vrmin;
/* L80: */
	}

/*        Compare condition numbers for eigenvalues */
/*        taking their condition numbers into account */

/* Computing MAX */
	d__1 = (doublereal) n * 2. * eps * tnrm;
	v = max(d__1,smlnum);
	if (tnrm == 0.) {
	    v = 1.;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (v > septmp[i__ - 1]) {
		tol = 1.;
	    } else {
		tol = v / septmp[i__ - 1];
	    }
	    if (v > sepin[i__ - 1]) {
		tolin = 1.;
	    } else {
		tolin = v / sepin[i__ - 1];
	    }
/* Computing MAX */
	    d__1 = tol, d__2 = smlnum / eps;
	    tol = max(d__1,d__2);
/* Computing MAX */
	    d__1 = tolin, d__2 = smlnum / eps;
	    tolin = max(d__1,d__2);
	    if (eps * (sin__[i__ - 1] - tolin) > stmp[i__ - 1] + tol) {
		vmax = 1. / eps;
	    } else if (sin__[i__ - 1] - tolin > stmp[i__ - 1] + tol) {
		vmax = (sin__[i__ - 1] - tolin) / (stmp[i__ - 1] + tol);
	    } else if (sin__[i__ - 1] + tolin < eps * (stmp[i__ - 1] - tol)) {
		vmax = 1. / eps;
	    } else if (sin__[i__ - 1] + tolin < stmp[i__ - 1] - tol) {
		vmax = (stmp[i__ - 1] - tol) / (sin__[i__ - 1] + tolin);
	    } else {
		vmax = 1.;
	    }
	    if (vmax > rmax[2]) {
		rmax[2] = vmax;
		if (ninfo[2] == 0) {
		    lmax[2] = *knt;
		}
	    }
/* L90: */
	}

/*        Compare condition numbers for eigenvectors */
/*        taking their condition numbers into account */

	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (v > septmp[i__ - 1] * stmp[i__ - 1]) {
		tol = septmp[i__ - 1];
	    } else {
		tol = v / stmp[i__ - 1];
	    }
	    if (v > sepin[i__ - 1] * sin__[i__ - 1]) {
		tolin = sepin[i__ - 1];
	    } else {
		tolin = v / sin__[i__ - 1];
	    }
/* Computing MAX */
	    d__1 = tol, d__2 = smlnum / eps;
	    tol = max(d__1,d__2);
/* Computing MAX */
	    d__1 = tolin, d__2 = smlnum / eps;
	    tolin = max(d__1,d__2);
	    if (eps * (sepin[i__ - 1] - tolin) > septmp[i__ - 1] + tol) {
		vmax = 1. / eps;
	    } else if (sepin[i__ - 1] - tolin > septmp[i__ - 1] + tol) {
		vmax = (sepin[i__ - 1] - tolin) / (septmp[i__ - 1] + tol);
	    } else if (sepin[i__ - 1] + tolin < eps * (septmp[i__ - 1] - tol))
		     {
		vmax = 1. / eps;
	    } else if (sepin[i__ - 1] + tolin < septmp[i__ - 1] - tol) {
		vmax = (septmp[i__ - 1] - tol) / (sepin[i__ - 1] + tolin);
	    } else {
		vmax = 1.;
	    }
	    if (vmax > rmax[2]) {
		rmax[2] = vmax;
		if (ninfo[2] == 0) {
		    lmax[2] = *knt;
		}
	    }
/* L100: */
	}

/*        Compare condition numbers for eigenvalues */
/*        without taking their condition numbers into account */

	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (sin__[i__ - 1] <= (doublereal) (n << 1) * eps && stmp[i__ - 1]
		     <= (doublereal) (n << 1) * eps) {
		vmax = 1.;
	    } else if (eps * sin__[i__ - 1] > stmp[i__ - 1]) {
		vmax = 1. / eps;
	    } else if (sin__[i__ - 1] > stmp[i__ - 1]) {
		vmax = sin__[i__ - 1] / stmp[i__ - 1];
	    } else if (sin__[i__ - 1] < eps * stmp[i__ - 1]) {
		vmax = 1. / eps;
	    } else if (sin__[i__ - 1] < stmp[i__ - 1]) {
		vmax = stmp[i__ - 1] / sin__[i__ - 1];
	    } else {
		vmax = 1.;
	    }
	    if (vmax > rmax[3]) {
		rmax[3] = vmax;
		if (ninfo[3] == 0) {
		    lmax[3] = *knt;
		}
	    }
/* L110: */
	}

/*        Compare condition numbers for eigenvectors */
/*        without taking their condition numbers into account */

	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (sepin[i__ - 1] <= v && septmp[i__ - 1] <= v) {
		vmax = 1.;
	    } else if (eps * sepin[i__ - 1] > septmp[i__ - 1]) {
		vmax = 1. / eps;
	    } else if (sepin[i__ - 1] > septmp[i__ - 1]) {
		vmax = sepin[i__ - 1] / septmp[i__ - 1];
	    } else if (sepin[i__ - 1] < eps * septmp[i__ - 1]) {
		vmax = 1. / eps;
	    } else if (sepin[i__ - 1] < septmp[i__ - 1]) {
		vmax = septmp[i__ - 1] / sepin[i__ - 1];
	    } else {
		vmax = 1.;
	    }
	    if (vmax > rmax[3]) {
		rmax[3] = vmax;
		if (ninfo[3] == 0) {
		    lmax[3] = *knt;
		}
	    }
/* L120: */
	}

/*        Compute eigenvalue condition numbers only and compare */

	vmax = 0.;
	dum[0] = -1.;
	dcopy_(&n, dum, &c__0, stmp, &c__1);
	dcopy_(&n, dum, &c__0, septmp, &c__1);
	dtrsna_("Eigcond", "All", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (stmp[i__ - 1] != s[i__ - 1]) {
		vmax = 1. / eps;
	    }
	    if (septmp[i__ - 1] != dum[0]) {
		vmax = 1. / eps;
	    }
/* L130: */
	}

/*        Compute eigenvector condition numbers only and compare */

	dcopy_(&n, dum, &c__0, stmp, &c__1);
	dcopy_(&n, dum, &c__0, septmp, &c__1);
	dtrsna_("Veccond", "All", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (stmp[i__ - 1] != dum[0]) {
		vmax = 1. / eps;
	    }
	    if (septmp[i__ - 1] != sep[i__ - 1]) {
		vmax = 1. / eps;
	    }
/* L140: */
	}

/*        Compute all condition numbers using SELECT and compare */

	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    select[i__ - 1] = TRUE_;
/* L150: */
	}
	dcopy_(&n, dum, &c__0, stmp, &c__1);
	dcopy_(&n, dum, &c__0, septmp, &c__1);
	dtrsna_("Bothcond", "Some", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (septmp[i__ - 1] != sep[i__ - 1]) {
		vmax = 1. / eps;
	    }
	    if (stmp[i__ - 1] != s[i__ - 1]) {
		vmax = 1. / eps;
	    }
/* L160: */
	}

/*        Compute eigenvalue condition numbers using SELECT and compare */

	dcopy_(&n, dum, &c__0, stmp, &c__1);
	dcopy_(&n, dum, &c__0, septmp, &c__1);
	dtrsna_("Eigcond", "Some", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (stmp[i__ - 1] != s[i__ - 1]) {
		vmax = 1. / eps;
	    }
	    if (septmp[i__ - 1] != dum[0]) {
		vmax = 1. / eps;
	    }
/* L170: */
	}

/*        Compute eigenvector condition numbers using SELECT and compare */

	dcopy_(&n, dum, &c__0, stmp, &c__1);
	dcopy_(&n, dum, &c__0, septmp, &c__1);
	dtrsna_("Veccond", "Some", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (stmp[i__ - 1] != dum[0]) {
		vmax = 1. / eps;
	    }
	    if (septmp[i__ - 1] != sep[i__ - 1]) {
		vmax = 1. / eps;
	    }
/* L180: */
	}
	if (vmax > rmax[1]) {
	    rmax[1] = vmax;
	    if (ninfo[1] == 0) {
		lmax[1] = *knt;
	    }
	}

/*        Select first real and first complex eigenvalue */

	if (wi[0] == 0.) {
	    lcmp[0] = 1;
	    ifnd = 0;
	    i__1 = n;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		if (ifnd == 1 || wi[i__ - 1] == 0.) {
		    select[i__ - 1] = FALSE_;
		} else {
		    ifnd = 1;
		    lcmp[1] = i__;
		    lcmp[2] = i__ + 1;
		    dcopy_(&n, &re[i__ * 20 - 20], &c__1, &re[20], &c__1);
		    dcopy_(&n, &re[(i__ + 1) * 20 - 20], &c__1, &re[40], &
			    c__1);
		    dcopy_(&n, &le[i__ * 20 - 20], &c__1, &le[20], &c__1);
		    dcopy_(&n, &le[(i__ + 1) * 20 - 20], &c__1, &le[40], &
			    c__1);
		}
/* L190: */
	    }
	    if (ifnd == 0) {
		icmp = 1;
	    } else {
		icmp = 3;
	    }
	} else {
	    lcmp[0] = 1;
	    lcmp[1] = 2;
	    ifnd = 0;
	    i__1 = n;
	    for (i__ = 3; i__ <= i__1; ++i__) {
		if (ifnd == 1 || wi[i__ - 1] != 0.) {
		    select[i__ - 1] = FALSE_;
		} else {
		    lcmp[2] = i__;
		    ifnd = 1;
		    dcopy_(&n, &re[i__ * 20 - 20], &c__1, &re[40], &c__1);
		    dcopy_(&n, &le[i__ * 20 - 20], &c__1, &le[40], &c__1);
		}
/* L200: */
	    }
	    if (ifnd == 0) {
		icmp = 2;
	    } else {
		icmp = 3;
	    }
	}

/*        Compute all selected condition numbers */

	dcopy_(&icmp, dum, &c__0, stmp, &c__1);
	dcopy_(&icmp, dum, &c__0, septmp, &c__1);
	dtrsna_("Bothcond", "Some", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = icmp;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = lcmp[i__ - 1];
	    if (septmp[i__ - 1] != sep[j - 1]) {
		vmax = 1. / eps;
	    }
	    if (stmp[i__ - 1] != s[j - 1]) {
		vmax = 1. / eps;
	    }
/* L210: */
	}

/*        Compute selected eigenvalue condition numbers */

	dcopy_(&icmp, dum, &c__0, stmp, &c__1);
	dcopy_(&icmp, dum, &c__0, septmp, &c__1);
	dtrsna_("Eigcond", "Some", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = icmp;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = lcmp[i__ - 1];
	    if (stmp[i__ - 1] != s[j - 1]) {
		vmax = 1. / eps;
	    }
	    if (septmp[i__ - 1] != dum[0]) {
		vmax = 1. / eps;
	    }
/* L220: */
	}

/*        Compute selected eigenvector condition numbers */

	dcopy_(&icmp, dum, &c__0, stmp, &c__1);
	dcopy_(&icmp, dum, &c__0, septmp, &c__1);
	dtrsna_("Veccond", "Some", select, &n, t, &c__20, le, &c__20, re, &
		c__20, stmp, septmp, &n, &m, work, &n, iwork, &info);
	if (info != 0) {
	    lmax[3] = *knt;
	    ++ninfo[3];
	    goto L240;
	}
	i__1 = icmp;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = lcmp[i__ - 1];
	    if (stmp[i__ - 1] != dum[0]) {
		vmax = 1. / eps;
	    }
	    if (septmp[i__ - 1] != sep[j - 1]) {
		vmax = 1. / eps;
	    }
/* L230: */
	}
	if (vmax > rmax[1]) {
	    rmax[1] = vmax;
	    if (ninfo[1] == 0) {
		lmax[1] = *knt;
	    }
	}
L240:
	;
    }
    goto L10;

/*     End of DGET37 */

} /* dget37_ */
