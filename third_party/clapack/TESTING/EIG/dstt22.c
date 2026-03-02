#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b12 = 1.;
static doublereal c_b13 = 0.;

/* Subroutine */ int dstt22_(integer *n, integer *m, integer *kband, 
	doublereal *ad, doublereal *ae, doublereal *sd, doublereal *se, 
	doublereal *u, integer *ldu, doublereal *work, integer *ldwork, 
	doublereal *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, work_dim1, work_offset, i__1, i__2, i__3;
    doublereal d__1, d__2, d__3, d__4, d__5;

    /* Local variables */
    integer i__, j, k;
    doublereal ulp, aukj, unfl;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    doublereal anorm, wnorm;
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *), 
	    dlansy_(char *, char *, integer *, doublereal *, integer *, 
	    doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTT22  checks a set of M eigenvalues and eigenvectors, */

/*      A U = U S */

/*  where A is symmetric tridiagonal, the columns of U are orthogonal, */
/*  and S is diagonal (if KBAND=0) or symmetric tridiagonal (if KBAND=1). */
/*  Two tests are performed: */

/*     RESULT(1) = | U' A U - S | / ( |A| m ulp ) */

/*     RESULT(2) = | I - U'U | / ( m ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, DSTT22 does nothing. */
/*          It must be at least zero. */

/*  M       (input) INTEGER */
/*          The number of eigenpairs to check.  If it is zero, DSTT22 */
/*          does nothing.  It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and SE is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AD      (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal of the original (unfactored) matrix A.  A is */
/*          assumed to be symmetric tridiagonal. */

/*  AE      (input) DOUBLE PRECISION array, dimension (N) */
/*          The off-diagonal of the original (unfactored) matrix A.  A */
/*          is assumed to be symmetric tridiagonal.  AE(1) is ignored, */
/*          AE(2) is the (1,2) and (2,1) element, etc. */

/*  SD      (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix S. */

/*  SE      (input) DOUBLE PRECISION array, dimension (N) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix S. */
/*          Not referenced if KBSND=0.  If KBAND=1, then AE(1) is */
/*          ignored, SE(2) is the (1,2) and (2,1) element, etc. */

/*  U       (input) DOUBLE PRECISION array, dimension (LDU, N) */
/*          The orthogonal matrix in the decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LDWORK, M+1) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of WORK.  LDWORK must be at least */
/*          max(1,M). */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
/*          The values computed by the two tests described above.  The */
/*          values are currently limited to 1/ulp, to avoid overflow. */

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

    /* Parameter adjustments */
    --ad;
    --ae;
    --sd;
    --se;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1;
    work -= work_offset;
    --result;

    /* Function Body */
    result[1] = 0.;
    result[2] = 0.;
    if (*n <= 0 || *m <= 0) {
	return 0;
    }

    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Epsilon");

/*     Do Test 1 */

/*     Compute the 1-norm of A. */

    if (*n > 1) {
	anorm = abs(ad[1]) + abs(ae[1]);
	i__1 = *n - 1;
	for (j = 2; j <= i__1; ++j) {
/* Computing MAX */
	    d__4 = anorm, d__5 = (d__1 = ad[j], abs(d__1)) + (d__2 = ae[j], 
		    abs(d__2)) + (d__3 = ae[j - 1], abs(d__3));
	    anorm = max(d__4,d__5);
/* L10: */
	}
/* Computing MAX */
	d__3 = anorm, d__4 = (d__1 = ad[*n], abs(d__1)) + (d__2 = ae[*n - 1], 
		abs(d__2));
	anorm = max(d__3,d__4);
    } else {
	anorm = abs(ad[1]);
    }
    anorm = max(anorm,unfl);

/*     Norm of U'AU - S */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *m;
	for (j = 1; j <= i__2; ++j) {
	    work[i__ + j * work_dim1] = 0.;
	    i__3 = *n;
	    for (k = 1; k <= i__3; ++k) {
		aukj = ad[k] * u[k + j * u_dim1];
		if (k != *n) {
		    aukj += ae[k] * u[k + 1 + j * u_dim1];
		}
		if (k != 1) {
		    aukj += ae[k - 1] * u[k - 1 + j * u_dim1];
		}
		work[i__ + j * work_dim1] += u[k + i__ * u_dim1] * aukj;
/* L20: */
	    }
/* L30: */
	}
	work[i__ + i__ * work_dim1] -= sd[i__];
	if (*kband == 1) {
	    if (i__ != 1) {
		work[i__ + (i__ - 1) * work_dim1] -= se[i__ - 1];
	    }
	    if (i__ != *n) {
		work[i__ + (i__ + 1) * work_dim1] -= se[i__];
	    }
	}
/* L40: */
    }

    wnorm = dlansy_("1", "L", m, &work[work_offset], m, &work[(*m + 1) * 
	    work_dim1 + 1]);

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*m * ulp);
    } else {
	if (anorm < 1.) {
/* Computing MIN */
	    d__1 = wnorm, d__2 = *m * anorm;
	    result[1] = min(d__1,d__2) / anorm / (*m * ulp);
	} else {
/* Computing MIN */
	    d__1 = wnorm / anorm, d__2 = (doublereal) (*m);
	    result[1] = min(d__1,d__2) / (*m * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  U'U - I */

    dgemm_("T", "N", m, m, n, &c_b12, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b13, &work[work_offset], m);

    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	work[j + j * work_dim1] += -1.;
/* L50: */
    }

/* Computing MIN */
    d__1 = (doublereal) (*m), d__2 = dlange_("1", m, m, &work[work_offset], m, 
	     &work[(*m + 1) * work_dim1 + 1]);
    result[2] = min(d__1,d__2) / (*m * ulp);

    return 0;

/*     End of DSTT22 */

} /* dstt22_ */
