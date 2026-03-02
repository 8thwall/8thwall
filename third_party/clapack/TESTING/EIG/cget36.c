#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__3 = 3;
static integer c__1 = 1;
static integer c__6 = 6;
static integer c__10 = 10;
static integer c__11 = 11;
static integer c__200 = 200;

/* Subroutine */ int cget36_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt, integer *nin)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);

    /* Local variables */
    integer i__, j, n;
    complex q[100]	/* was [10][10] */, t1[100]	/* was [10][10] */, 
	    t2[100]	/* was [10][10] */;
    real eps, res;
    complex tmp[100]	/* was [10][10] */, diag[10];
    integer ifst, ilst;
    complex work[200];
    integer info1, info2;
    extern /* Subroutine */ int chst01_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *);
    complex ctemp;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *);
    real rwork[10];
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *), ctrexc_(char *, integer *, complex *, integer *, complex 
	    *, integer *, integer *, integer *, integer *);
    real result[2];

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 0, 0, 0, 0 };
    static cilist io___7 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGET36 tests CTREXC, a routine for reordering diagonal entries of a */
/*  matrix in complex Schur form. Thus, CLAEXC computes a unitary matrix */
/*  Q such that */

/*     Q' * T1 * Q  = T2 */

/*  and where one of the diagonal blocks of T1 (the one at row IFST) has */
/*  been moved to position ILST. */

/*  The test code verifies that the residual Q'*T1*Q-T2 is small, that T2 */
/*  is in Schur form, and that the final position of the IFST block is */
/*  ILST. */

/*  The test matrices are read from a file with logical unit number NIN. */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) REAL */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER */
/*          Number of examples where INFO is nonzero. */

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
/*     .. Executable Statements .. */

    eps = slamch_("P");
    *rmax = 0.f;
    *lmax = 0;
    *knt = 0;
    *ninfo = 0;

/*     Read input data until N=0 */

L10:
    io___2.ciunit = *nin;
    s_rsle(&io___2);
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&ifst, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&ilst, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	return 0;
    }
    ++(*knt);
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___7.ciunit = *nin;
	s_rsle(&io___7);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__6, &c__1, (char *)&tmp[i__ + j * 10 - 11], (ftnlen)
		    sizeof(complex));
	}
	e_rsle();
/* L20: */
    }
    clacpy_("F", &n, &n, tmp, &c__10, t1, &c__10);
    clacpy_("F", &n, &n, tmp, &c__10, t2, &c__10);
    res = 0.f;

/*     Test without accumulating Q */

    claset_("Full", &n, &n, &c_b1, &c_b2, q, &c__10);
    ctrexc_("N", &n, t1, &c__10, q, &c__10, &ifst, &ilst, &info1);
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = i__ + j * 10 - 11;
	    if (i__ == j && (q[i__3].r != 1.f || q[i__3].i != 0.f)) {
		res += 1.f / eps;
	    }
	    i__3 = i__ + j * 10 - 11;
	    if (i__ != j && (q[i__3].r != 0.f || q[i__3].i != 0.f)) {
		res += 1.f / eps;
	    }
/* L30: */
	}
/* L40: */
    }

/*     Test with accumulating Q */

    claset_("Full", &n, &n, &c_b1, &c_b2, q, &c__10);
    ctrexc_("V", &n, t2, &c__10, q, &c__10, &ifst, &ilst, &info2);

/*     Compare T1 with T2 */

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = i__ + j * 10 - 11;
	    i__4 = i__ + j * 10 - 11;
	    if (t1[i__3].r != t2[i__4].r || t1[i__3].i != t2[i__4].i) {
		res += 1.f / eps;
	    }
/* L50: */
	}
/* L60: */
    }
    if (info1 != 0 || info2 != 0) {
	++(*ninfo);
    }
    if (info1 != info2) {
	res += 1.f / eps;
    }

/*     Test for successful reordering of T2 */

    ccopy_(&n, tmp, &c__11, diag, &c__1);
    if (ifst < ilst) {
	i__1 = ilst;
	for (i__ = ifst + 1; i__ <= i__1; ++i__) {
	    i__2 = i__ - 1;
	    ctemp.r = diag[i__2].r, ctemp.i = diag[i__2].i;
	    i__2 = i__ - 1;
	    i__3 = i__ - 2;
	    diag[i__2].r = diag[i__3].r, diag[i__2].i = diag[i__3].i;
	    i__2 = i__ - 2;
	    diag[i__2].r = ctemp.r, diag[i__2].i = ctemp.i;
/* L70: */
	}
    } else if (ifst > ilst) {
	i__1 = ilst;
	for (i__ = ifst - 1; i__ >= i__1; --i__) {
	    i__2 = i__;
	    ctemp.r = diag[i__2].r, ctemp.i = diag[i__2].i;
	    i__2 = i__;
	    i__3 = i__ - 1;
	    diag[i__2].r = diag[i__3].r, diag[i__2].i = diag[i__3].i;
	    i__2 = i__ - 1;
	    diag[i__2].r = ctemp.r, diag[i__2].i = ctemp.i;
/* L80: */
	}
    }
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * 10 - 11;
	i__3 = i__ - 1;
	if (t2[i__2].r != diag[i__3].r || t2[i__2].i != diag[i__3].i) {
	    res += 1.f / eps;
	}
/* L90: */
    }

/*     Test for small residual, and orthogonality of Q */

    chst01_(&n, &c__1, &n, tmp, &c__10, t2, &c__10, q, &c__10, work, &c__200, 
	    rwork, result);
    res = res + result[0] + result[1];

/*     Test for T2 being in Schur form */

    i__1 = n - 1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = n;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * 10 - 11;
	    if (t2[i__3].r != 0.f || t2[i__3].i != 0.f) {
		res += 1.f / eps;
	    }
/* L100: */
	}
/* L110: */
    }
    if (res > *rmax) {
	*rmax = res;
	*lmax = *knt;
    }
    goto L10;

/*     End of CGET36 */

} /* cget36_ */
