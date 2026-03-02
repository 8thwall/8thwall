#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__10 = 10;
static real c_b21 = 0.f;
static real c_b22 = 1.f;
static integer c__200 = 200;

/* Subroutine */ int sget36_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt, integer *nin)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);
    double r_sign(real *, real *);

    /* Local variables */
    integer i__, j, n;
    real q[100]	/* was [10][10] */, t1[100]	/* was [10][10] */, t2[100]	
	    /* was [10][10] */;
    integer loc;
    real eps, res, tmp[100]	/* was [10][10] */;
    integer ifst, ilst;
    real work[200];
    integer info1, info2, ifst1, ifst2, ilst1, ilst2;
    extern /* Subroutine */ int shst01_(integer *, integer *, integer *, real 
	    *, integer *, real *, integer *, real *, integer *, real *, 
	    integer *, real *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *), strexc_(
	    char *, integer *, real *, integer *, real *, integer *, integer *
, integer *, real *, integer *);
    integer ifstsv;
    real result[2];
    integer ilstsv;

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 0, 0, 0, 0 };
    static cilist io___7 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET36 tests STREXC, a routine for moving blocks (either 1 by 1 or */
/*  2 by 2) on the diagonal of a matrix in real Schur form.  Thus, SLAEXC */
/*  computes an orthogonal matrix Q such that */

/*     Q' * T1 * Q  = T2 */

/*  and where one of the diagonal blocks of T1 (the one at row IFST) has */
/*  been moved to position ILST. */

/*  The test code verifies that the residual Q'*T1*Q-T2 is small, that T2 */
/*  is in Schur form, and that the final position of the IFST block is */
/*  ILST (within +-1). */

/*  The test matrices are read from a file with logical unit number NIN. */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) REAL */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER array, dimension (3) */
/*          NINFO(J) is the number of examples where INFO=J. */

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

    /* Function Body */
    eps = slamch_("P");
    *rmax = 0.f;
    *lmax = 0;
    *knt = 0;
    ninfo[1] = 0;
    ninfo[2] = 0;
    ninfo[3] = 0;

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
	    do_lio(&c__4, &c__1, (char *)&tmp[i__ + j * 10 - 11], (ftnlen)
		    sizeof(real));
	}
	e_rsle();
/* L20: */
    }
    slacpy_("F", &n, &n, tmp, &c__10, t1, &c__10);
    slacpy_("F", &n, &n, tmp, &c__10, t2, &c__10);
    ifstsv = ifst;
    ilstsv = ilst;
    ifst1 = ifst;
    ilst1 = ilst;
    ifst2 = ifst;
    ilst2 = ilst;
    res = 0.f;

/*     Test without accumulating Q */

    slaset_("Full", &n, &n, &c_b21, &c_b22, q, &c__10);
    strexc_("N", &n, t1, &c__10, q, &c__10, &ifst1, &ilst1, work, &info1);
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    if (i__ == j && q[i__ + j * 10 - 11] != 1.f) {
		res += 1.f / eps;
	    }
	    if (i__ != j && q[i__ + j * 10 - 11] != 0.f) {
		res += 1.f / eps;
	    }
/* L30: */
	}
/* L40: */
    }

/*     Test with accumulating Q */

    slaset_("Full", &n, &n, &c_b21, &c_b22, q, &c__10);
    strexc_("V", &n, t2, &c__10, q, &c__10, &ifst2, &ilst2, work, &info2);

/*     Compare T1 with T2 */

    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    if (t1[i__ + j * 10 - 11] != t2[i__ + j * 10 - 11]) {
		res += 1.f / eps;
	    }
/* L50: */
	}
/* L60: */
    }
    if (ifst1 != ifst2) {
	res += 1.f / eps;
    }
    if (ilst1 != ilst2) {
	res += 1.f / eps;
    }
    if (info1 != info2) {
	res += 1.f / eps;
    }

/*     Test for successful reordering of T2 */

    if (info2 != 0) {
	++ninfo[info2];
    } else {
	if ((i__1 = ifst2 - ifstsv, abs(i__1)) > 1) {
	    res += 1.f / eps;
	}
	if ((i__1 = ilst2 - ilstsv, abs(i__1)) > 1) {
	    res += 1.f / eps;
	}
    }

/*     Test for small residual, and orthogonality of Q */

    shst01_(&n, &c__1, &n, tmp, &c__10, t2, &c__10, q, &c__10, work, &c__200, 
	    result);
    res = res + result[0] + result[1];

/*     Test for T2 being in Schur form */

    loc = 1;
L70:
    if (t2[loc + 1 + loc * 10 - 11] != 0.f) {

/*        2 by 2 block */

	if (t2[loc + (loc + 1) * 10 - 11] == 0.f || t2[loc + loc * 10 - 11] !=
		 t2[loc + 1 + (loc + 1) * 10 - 11] || r_sign(&c_b22, &t2[loc 
		+ (loc + 1) * 10 - 11]) == r_sign(&c_b22, &t2[loc + 1 + loc * 
		10 - 11])) {
	    res += 1.f / eps;
	}
	i__1 = n;
	for (i__ = loc + 2; i__ <= i__1; ++i__) {
	    if (t2[i__ + loc * 10 - 11] != 0.f) {
		res += 1.f / res;
	    }
	    if (t2[i__ + (loc + 1) * 10 - 11] != 0.f) {
		res += 1.f / res;
	    }
/* L80: */
	}
	loc += 2;
    } else {

/*        1 by 1 block */

	i__1 = n;
	for (i__ = loc + 1; i__ <= i__1; ++i__) {
	    if (t2[i__ + loc * 10 - 11] != 0.f) {
		res += 1.f / res;
	    }
/* L90: */
	}
	++loc;
    }
    if (loc < n) {
	goto L70;
    }
    if (res > *rmax) {
	*rmax = res;
	*lmax = *knt;
    }
    goto L10;

/*     End of SGET36 */

} /* sget36_ */
