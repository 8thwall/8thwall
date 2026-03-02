#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b12 = 1.f;
static real c_b13 = 0.f;

/* Subroutine */ int sstt22_(integer *n, integer *m, integer *kband, real *ad, 
	 real *ae, real *sd, real *se, real *u, integer *ldu, real *work, 
	integer *ldwork, real *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, work_dim1, work_offset, i__1, i__2, i__3;
    real r__1, r__2, r__3, r__4, r__5;

    /* Local variables */
    integer i__, j, k;
    real ulp, aukj, unfl;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm, wnorm;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *), slansy_(char *, 
	    char *, integer *, real *, integer *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSTT22  checks a set of M eigenvalues and eigenvectors, */

/*      A U = U S */

/*  where A is symmetric tridiagonal, the columns of U are orthogonal, */
/*  and S is diagonal (if KBAND=0) or symmetric tridiagonal (if KBAND=1). */
/*  Two tests are performed: */

/*     RESULT(1) = | U' A U - S | / ( |A| m ulp ) */

/*     RESULT(2) = | I - U'U | / ( m ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SSTT22 does nothing. */
/*          It must be at least zero. */

/*  M       (input) INTEGER */
/*          The number of eigenpairs to check.  If it is zero, SSTT22 */
/*          does nothing.  It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and SE is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AD      (input) REAL array, dimension (N) */
/*          The diagonal of the original (unfactored) matrix A.  A is */
/*          assumed to be symmetric tridiagonal. */

/*  AE      (input) REAL array, dimension (N) */
/*          The off-diagonal of the original (unfactored) matrix A.  A */
/*          is assumed to be symmetric tridiagonal.  AE(1) is ignored, */
/*          AE(2) is the (1,2) and (2,1) element, etc. */

/*  SD      (input) REAL array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix S. */

/*  SE      (input) REAL array, dimension (N) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix S. */
/*          Not referenced if KBSND=0.  If KBAND=1, then AE(1) is */
/*          ignored, SE(2) is the (1,2) and (2,1) element, etc. */

/*  U       (input) REAL array, dimension (LDU, N) */
/*          The orthogonal matrix in the decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N. */

/*  WORK    (workspace) REAL array, dimension (LDWORK, M+1) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of WORK.  LDWORK must be at least */
/*          max(1,M). */

/*  RESULT  (output) REAL array, dimension (2) */
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
    result[1] = 0.f;
    result[2] = 0.f;
    if (*n <= 0 || *m <= 0) {
	return 0;
    }

    unfl = slamch_("Safe minimum");
    ulp = slamch_("Epsilon");

/*     Do Test 1 */

/*     Compute the 1-norm of A. */

    if (*n > 1) {
	anorm = dabs(ad[1]) + dabs(ae[1]);
	i__1 = *n - 1;
	for (j = 2; j <= i__1; ++j) {
/* Computing MAX */
	    r__4 = anorm, r__5 = (r__1 = ad[j], dabs(r__1)) + (r__2 = ae[j], 
		    dabs(r__2)) + (r__3 = ae[j - 1], dabs(r__3));
	    anorm = dmax(r__4,r__5);
/* L10: */
	}
/* Computing MAX */
	r__3 = anorm, r__4 = (r__1 = ad[*n], dabs(r__1)) + (r__2 = ae[*n - 1],
		 dabs(r__2));
	anorm = dmax(r__3,r__4);
    } else {
	anorm = dabs(ad[1]);
    }
    anorm = dmax(anorm,unfl);

/*     Norm of U'AU - S */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *m;
	for (j = 1; j <= i__2; ++j) {
	    work[i__ + j * work_dim1] = 0.f;
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

    wnorm = slansy_("1", "L", m, &work[work_offset], m, &work[(*m + 1) * 
	    work_dim1 + 1]);

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*m * ulp);
    } else {
	if (anorm < 1.f) {
/* Computing MIN */
	    r__1 = wnorm, r__2 = *m * anorm;
	    result[1] = dmin(r__1,r__2) / anorm / (*m * ulp);
	} else {
/* Computing MIN */
	    r__1 = wnorm / anorm, r__2 = (real) (*m);
	    result[1] = dmin(r__1,r__2) / (*m * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  U'U - I */

    sgemm_("T", "N", m, m, n, &c_b12, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b13, &work[work_offset], m);

    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	work[j + j * work_dim1] += -1.f;
/* L50: */
    }

/* Computing MIN */
    r__1 = (real) (*m), r__2 = slange_("1", m, m, &work[work_offset], m, &
	    work[(*m + 1) * work_dim1 + 1]);
    result[2] = dmin(r__1,r__2) / (*m * ulp);

    return 0;

/*     End of SSTT22 */

} /* sstt22_ */
