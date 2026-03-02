#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b5 = 0.f;
static integer c__1 = 1;
static real c_b19 = 1.f;

/* Subroutine */ int sstt21_(integer *n, integer *kband, real *ad, real *ae, 
	real *sd, real *se, real *u, integer *ldu, real *work, real *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1;
    real r__1, r__2, r__3;

    /* Local variables */
    integer j;
    real ulp, unfl;
    extern /* Subroutine */ int ssyr_(char *, integer *, real *, real *, 
	    integer *, real *, integer *);
    real temp1, temp2;
    extern /* Subroutine */ int ssyr2_(char *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *), sgemm_(
	    char *, char *, integer *, integer *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, real *, integer *);
    real anorm, wnorm;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slaset_(char *, integer *, integer *, real *, 
	    real *, real *, integer *);
    extern doublereal slansy_(char *, char *, integer *, real *, integer *, 
	    real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSTT21 checks a decomposition of the form */

/*     A = U S U' */

/*  where ' means transpose, A is symmetric tridiagonal, U is orthogonal, */
/*  and S is diagonal (if KBAND=0) or symmetric tridiagonal (if KBAND=1). */
/*  Two tests are performed: */

/*     RESULT(1) = | A - U S U' | / ( |A| n ulp ) */

/*     RESULT(2) = | I - UU' | / ( n ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SSTT21 does nothing. */
/*          It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and SE is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AD      (input) REAL array, dimension (N) */
/*          The diagonal of the original (unfactored) matrix A.  A is */
/*          assumed to be symmetric tridiagonal. */

/*  AE      (input) REAL array, dimension (N-1) */
/*          The off-diagonal of the original (unfactored) matrix A.  A */
/*          is assumed to be symmetric tridiagonal.  AE(1) is the (1,2) */
/*          and (2,1) element, AE(2) is the (2,3) and (3,2) element, etc. */

/*  SD      (input) REAL array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix S. */

/*  SE      (input) REAL array, dimension (N-1) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix S. */
/*          Not referenced if KBSND=0.  If KBAND=1, then AE(1) is the */
/*          (1,2) and (2,1) element, SE(2) is the (2,3) and (3,2) */
/*          element, etc. */

/*  U       (input) REAL array, dimension (LDU, N) */
/*          The orthogonal matrix in the decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N. */

/*  WORK    (workspace) REAL array, dimension (N*(N+1)) */

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

    slaset_("Full", n, n, &c_b5, &c_b5, &work[1], n);

    anorm = 0.f;
    temp1 = 0.f;

    i__1 = *n - 1;
    for (j = 1; j <= i__1; ++j) {
	work[(*n + 1) * (j - 1) + 1] = ad[j];
	work[(*n + 1) * (j - 1) + 2] = ae[j];
	temp2 = (r__1 = ae[j], dabs(r__1));
/* Computing MAX */
	r__2 = anorm, r__3 = (r__1 = ad[j], dabs(r__1)) + temp1 + temp2;
	anorm = dmax(r__2,r__3);
	temp1 = temp2;
/* L10: */
    }

/* Computing 2nd power */
    i__1 = *n;
    work[i__1 * i__1] = ad[*n];
/* Computing MAX */
    r__2 = anorm, r__3 = (r__1 = ad[*n], dabs(r__1)) + temp1, r__2 = max(r__2,
	    r__3);
    anorm = dmax(r__2,unfl);

/*     Norm of A - USU' */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	r__1 = -sd[j];
	ssyr_("L", n, &r__1, &u[j * u_dim1 + 1], &c__1, &work[1], n);
/* L20: */
    }

    if (*n > 1 && *kband == 1) {
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    r__1 = -se[j];
	    ssyr2_("L", n, &r__1, &u[j * u_dim1 + 1], &c__1, &u[(j + 1) * 
		    u_dim1 + 1], &c__1, &work[1], n);
/* L30: */
	}
    }

/* Computing 2nd power */
    i__1 = *n;
    wnorm = slansy_("1", "L", n, &work[1], n, &work[i__1 * i__1 + 1]);

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

/*     Compute  UU' - I */

    sgemm_("N", "C", n, n, n, &c_b19, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b5, &work[1], n);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	work[(*n + 1) * (j - 1) + 1] += -1.f;
/* L40: */
    }

/* Computing MIN */
/* Computing 2nd power */
    i__1 = *n;
    r__1 = (real) (*n), r__2 = slange_("1", n, n, &work[1], n, &work[i__1 * 
	    i__1 + 1]);
    result[2] = dmin(r__1,r__2) / (*n * ulp);

    return 0;

/*     End of SSTT21 */

} /* sstt21_ */
