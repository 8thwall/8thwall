#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b5 = 0.;
static integer c__1 = 1;
static doublereal c_b19 = 1.;

/* Subroutine */ int dstt21_(integer *n, integer *kband, doublereal *ad, 
	doublereal *ae, doublereal *sd, doublereal *se, doublereal *u, 
	integer *ldu, doublereal *work, doublereal *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1;
    doublereal d__1, d__2, d__3;

    /* Local variables */
    integer j;
    doublereal ulp, unfl;
    extern /* Subroutine */ int dsyr_(char *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal temp1, temp2;
    extern /* Subroutine */ int dsyr2_(char *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *), dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    doublereal anorm, wnorm;
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *);
    extern doublereal dlansy_(char *, char *, integer *, doublereal *, 
	    integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTT21 checks a decomposition of the form */

/*     A = U S U' */

/*  where ' means transpose, A is symmetric tridiagonal, U is orthogonal, */
/*  and S is diagonal (if KBAND=0) or symmetric tridiagonal (if KBAND=1). */
/*  Two tests are performed: */

/*     RESULT(1) = | A - U S U' | / ( |A| n ulp ) */

/*     RESULT(2) = | I - UU' | / ( n ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, DSTT21 does nothing. */
/*          It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and SE is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AD      (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal of the original (unfactored) matrix A.  A is */
/*          assumed to be symmetric tridiagonal. */

/*  AE      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal of the original (unfactored) matrix A.  A */
/*          is assumed to be symmetric tridiagonal.  AE(1) is the (1,2) */
/*          and (2,1) element, AE(2) is the (2,3) and (3,2) element, etc. */

/*  SD      (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix S. */

/*  SE      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix S. */
/*          Not referenced if KBSND=0.  If KBAND=1, then AE(1) is the */
/*          (1,2) and (2,1) element, SE(2) is the (2,3) and (3,2) */
/*          element, etc. */

/*  U       (input) DOUBLE PRECISION array, dimension (LDU, N) */
/*          The orthogonal matrix in the decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N*(N+1)) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
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
    result[1] = 0.;
    result[2] = 0.;
    if (*n <= 0) {
	return 0;
    }

    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Precision");

/*     Do Test 1 */

/*     Copy A & Compute its 1-Norm: */

    dlaset_("Full", n, n, &c_b5, &c_b5, &work[1], n);

    anorm = 0.;
    temp1 = 0.;

    i__1 = *n - 1;
    for (j = 1; j <= i__1; ++j) {
	work[(*n + 1) * (j - 1) + 1] = ad[j];
	work[(*n + 1) * (j - 1) + 2] = ae[j];
	temp2 = (d__1 = ae[j], abs(d__1));
/* Computing MAX */
	d__2 = anorm, d__3 = (d__1 = ad[j], abs(d__1)) + temp1 + temp2;
	anorm = max(d__2,d__3);
	temp1 = temp2;
/* L10: */
    }

/* Computing 2nd power */
    i__1 = *n;
    work[i__1 * i__1] = ad[*n];
/* Computing MAX */
    d__2 = anorm, d__3 = (d__1 = ad[*n], abs(d__1)) + temp1, d__2 = max(d__2,
	    d__3);
    anorm = max(d__2,unfl);

/*     Norm of A - USU' */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	d__1 = -sd[j];
	dsyr_("L", n, &d__1, &u[j * u_dim1 + 1], &c__1, &work[1], n);
/* L20: */
    }

    if (*n > 1 && *kband == 1) {
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    d__1 = -se[j];
	    dsyr2_("L", n, &d__1, &u[j * u_dim1 + 1], &c__1, &u[(j + 1) * 
		    u_dim1 + 1], &c__1, &work[1], n);
/* L30: */
	}
    }

/* Computing 2nd power */
    i__1 = *n;
    wnorm = dlansy_("1", "L", n, &work[1], n, &work[i__1 * i__1 + 1]);

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*n * ulp);
    } else {
	if (anorm < 1.) {
/* Computing MIN */
	    d__1 = wnorm, d__2 = *n * anorm;
	    result[1] = min(d__1,d__2) / anorm / (*n * ulp);
	} else {
/* Computing MIN */
	    d__1 = wnorm / anorm, d__2 = (doublereal) (*n);
	    result[1] = min(d__1,d__2) / (*n * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  UU' - I */

    dgemm_("N", "C", n, n, n, &c_b19, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b5, &work[1], n);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	work[(*n + 1) * (j - 1) + 1] += -1.;
/* L40: */
    }

/* Computing MIN */
/* Computing 2nd power */
    i__1 = *n;
    d__1 = (doublereal) (*n), d__2 = dlange_("1", n, n, &work[1], n, &work[
	    i__1 * i__1 + 1]);
    result[2] = min(d__1,d__2) / (*n * ulp);

    return 0;

/*     End of DSTT21 */

} /* dstt21_ */
