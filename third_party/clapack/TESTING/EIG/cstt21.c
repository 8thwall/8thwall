#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__1 = 1;

/* Subroutine */ int cstt21_(integer *n, integer *kband, real *ad, real *ae, 
	real *sd, real *se, complex *u, integer *ldu, complex *work, real *
	rwork, real *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1, i__2, i__3;
    real r__1, r__2, r__3;
    complex q__1, q__2;

    /* Local variables */
    integer j;
    real ulp;
    extern /* Subroutine */ int cher_(char *, integer *, real *, complex *, 
	    integer *, complex *, integer *);
    real unfl;
    extern /* Subroutine */ int cher2_(char *, integer *, complex *, complex *
, integer *, complex *, integer *, complex *, integer *);
    real temp1, temp2;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    real anorm, wnorm;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), clanhe_(char *, char *, integer *, 
	    complex *, integer *, real *), slamch_(char *);
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CSTT21  checks a decomposition of the form */

/*     A = U S U* */

/*  where * means conjugate transpose, A is real symmetric tridiagonal, */
/*  U is unitary, and S is real and diagonal (if KBAND=0) or symmetric */
/*  tridiagonal (if KBAND=1).  Two tests are performed: */

/*     RESULT(1) = | A - U S U* | / ( |A| n ulp ) */

/*     RESULT(2) = | I - UU* | / ( n ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, CSTT21 does nothing. */
/*          It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and SE is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AD      (input) REAL array, dimension (N) */
/*          The diagonal of the original (unfactored) matrix A.  A is */
/*          assumed to be real symmetric tridiagonal. */

/*  AE      (input) REAL array, dimension (N-1) */
/*          The off-diagonal of the original (unfactored) matrix A.  A */
/*          is assumed to be symmetric tridiagonal.  AE(1) is the (1,2) */
/*          and (2,1) element, AE(2) is the (2,3) and (3,2) element, etc. */

/*  SD      (input) REAL array, dimension (N) */
/*          The diagonal of the real (symmetric tri-) diagonal matrix S. */

/*  SE      (input) REAL array, dimension (N-1) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix S. */
/*          Not referenced if KBSND=0.  If KBAND=1, then AE(1) is the */
/*          (1,2) and (2,1) element, SE(2) is the (2,3) and (3,2) */
/*          element, etc. */

/*  U       (input) COMPLEX array, dimension (LDU, N) */
/*          The unitary matrix in the decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N. */

/*  WORK    (workspace) COMPLEX array, dimension (N**2) */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESULT  (output) REAL array, dimension (2) */
/*          The values computed by the two tests described above.  The */
/*          values are currently limited to 1/ulp, to avoid overflow. */
/*          RESULT(1) is always modified. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     1)      Constants */

    /* Parameter adjustments */
    --ad;
    --ae;
    --sd;
    --se;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --work;
    --rwork;
    --result;

    /* Function Body */
    result[1] = 0.f;
    result[2] = 0.f;
    if (*n <= 0) {
	return 0;
    }

    unfl = slamch_("Safe minimum");
    ulp = slamch_("Precision");

/*     Do Test 1 */

/*     Copy A & Compute its 1-Norm: */

    claset_("Full", n, n, &c_b1, &c_b1, &work[1], n);

    anorm = 0.f;
    temp1 = 0.f;

    i__1 = *n - 1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = (*n + 1) * (j - 1) + 1;
	i__3 = j;
	work[i__2].r = ad[i__3], work[i__2].i = 0.f;
	i__2 = (*n + 1) * (j - 1) + 2;
	i__3 = j;
	work[i__2].r = ae[i__3], work[i__2].i = 0.f;
	temp2 = (r__1 = ae[j], dabs(r__1));
/* Computing MAX */
	r__2 = anorm, r__3 = (r__1 = ad[j], dabs(r__1)) + temp1 + temp2;
	anorm = dmax(r__2,r__3);
	temp1 = temp2;
/* L10: */
    }

/* Computing 2nd power */
    i__2 = *n;
    i__1 = i__2 * i__2;
    i__3 = *n;
    work[i__1].r = ad[i__3], work[i__1].i = 0.f;
/* Computing MAX */
    r__2 = anorm, r__3 = (r__1 = ad[*n], dabs(r__1)) + temp1, r__2 = max(r__2,
	    r__3);
    anorm = dmax(r__2,unfl);

/*     Norm of A - USU* */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	r__1 = -sd[j];
	cher_("L", n, &r__1, &u[j * u_dim1 + 1], &c__1, &work[1], n);
/* L20: */
    }

    if (*n > 1 && *kband == 1) {
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    q__2.r = se[i__2], q__2.i = 0.f;
	    q__1.r = -q__2.r, q__1.i = -q__2.i;
	    cher2_("L", n, &q__1, &u[j * u_dim1 + 1], &c__1, &u[(j + 1) * 
		    u_dim1 + 1], &c__1, &work[1], n);
/* L30: */
	}
    }

    wnorm = clanhe_("1", "L", n, &work[1], n, &rwork[1])
	    ;

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*n * ulp);
    } else {
	if (anorm < 1.f) {
/* Computing MIN */
	    r__1 = wnorm, r__2 = *n * anorm;
	    result[1] = dmin(r__1,r__2) / anorm / (*n * ulp);
	} else {
/* Computing MIN */
	    r__1 = wnorm / anorm, r__2 = (real) (*n);
	    result[1] = dmin(r__1,r__2) / (*n * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  UU* - I */

    cgemm_("N", "C", n, n, n, &c_b2, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b1, &work[1], n);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = (*n + 1) * (j - 1) + 1;
	i__3 = (*n + 1) * (j - 1) + 1;
	q__1.r = work[i__3].r - 1.f, q__1.i = work[i__3].i - 0.f;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L40: */
    }

/* Computing MIN */
    r__1 = (real) (*n), r__2 = clange_("1", n, n, &work[1], n, &rwork[1]);
    result[2] = dmin(r__1,r__2) / (*n * ulp);

    return 0;

/*     End of CSTT21 */

} /* cstt21_ */
