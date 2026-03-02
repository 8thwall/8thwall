#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};

/* Subroutine */ int cstt22_(integer *n, integer *m, integer *kband, real *ad, 
	 real *ae, real *sd, real *se, complex *u, integer *ldu, complex *
	work, integer *ldwork, real *rwork, real *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, work_dim1, work_offset, i__1, i__2, i__3, i__4, 
	    i__5, i__6;
    real r__1, r__2, r__3, r__4, r__5;
    complex q__1, q__2;

    /* Local variables */
    integer i__, j, k;
    real ulp;
    complex aukj;
    real unfl;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    real anorm, wnorm;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *), clansy_(char 
	    *, char *, integer *, complex *, integer *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CSTT22  checks a set of M eigenvalues and eigenvectors, */

/*      A U = U S */

/*  where A is Hermitian tridiagonal, the columns of U are unitary, */
/*  and S is diagonal (if KBAND=0) or Hermitian tridiagonal (if KBAND=1). */
/*  Two tests are performed: */

/*     RESULT(1) = | U* A U - S | / ( |A| m ulp ) */

/*     RESULT(2) = | I - U*U | / ( m ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, CSTT22 does nothing. */
/*          It must be at least zero. */

/*  M       (input) INTEGER */
/*          The number of eigenpairs to check.  If it is zero, CSTT22 */
/*          does nothing.  It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and SE is not referenced.  If */
/*          one, then S is Hermitian tri-diagonal. */

/*  AD      (input) REAL array, dimension (N) */
/*          The diagonal of the original (unfactored) matrix A.  A is */
/*          assumed to be Hermitian tridiagonal. */

/*  AE      (input) REAL array, dimension (N) */
/*          The off-diagonal of the original (unfactored) matrix A.  A */
/*          is assumed to be Hermitian tridiagonal.  AE(1) is ignored, */
/*          AE(2) is the (1,2) and (2,1) element, etc. */

/*  SD      (input) REAL array, dimension (N) */
/*          The diagonal of the (Hermitian tri-) diagonal matrix S. */

/*  SE      (input) REAL array, dimension (N) */
/*          The off-diagonal of the (Hermitian tri-) diagonal matrix S. */
/*          Not referenced if KBSND=0.  If KBAND=1, then AE(1) is */
/*          ignored, SE(2) is the (1,2) and (2,1) element, etc. */

/*  U       (input) REAL array, dimension (LDU, N) */
/*          The unitary matrix in the decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N. */

/*  WORK    (workspace) COMPLEX array, dimension (LDWORK, M+1) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of WORK.  LDWORK must be at least */
/*          max(1,M). */

/*  RWORK   (workspace) REAL array, dimension (N) */

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
    --rwork;
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

/*     Norm of U*AU - S */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *m;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = i__ + j * work_dim1;
	    work[i__3].r = 0.f, work[i__3].i = 0.f;
	    i__3 = *n;
	    for (k = 1; k <= i__3; ++k) {
		i__4 = k;
		i__5 = k + j * u_dim1;
		q__1.r = ad[i__4] * u[i__5].r, q__1.i = ad[i__4] * u[i__5].i;
		aukj.r = q__1.r, aukj.i = q__1.i;
		if (k != *n) {
		    i__4 = k;
		    i__5 = k + 1 + j * u_dim1;
		    q__2.r = ae[i__4] * u[i__5].r, q__2.i = ae[i__4] * u[i__5]
			    .i;
		    q__1.r = aukj.r + q__2.r, q__1.i = aukj.i + q__2.i;
		    aukj.r = q__1.r, aukj.i = q__1.i;
		}
		if (k != 1) {
		    i__4 = k - 1;
		    i__5 = k - 1 + j * u_dim1;
		    q__2.r = ae[i__4] * u[i__5].r, q__2.i = ae[i__4] * u[i__5]
			    .i;
		    q__1.r = aukj.r + q__2.r, q__1.i = aukj.i + q__2.i;
		    aukj.r = q__1.r, aukj.i = q__1.i;
		}
		i__4 = i__ + j * work_dim1;
		i__5 = i__ + j * work_dim1;
		i__6 = k + i__ * u_dim1;
		q__2.r = u[i__6].r * aukj.r - u[i__6].i * aukj.i, q__2.i = u[
			i__6].r * aukj.i + u[i__6].i * aukj.r;
		q__1.r = work[i__5].r + q__2.r, q__1.i = work[i__5].i + 
			q__2.i;
		work[i__4].r = q__1.r, work[i__4].i = q__1.i;
/* L20: */
	    }
/* L30: */
	}
	i__2 = i__ + i__ * work_dim1;
	i__3 = i__ + i__ * work_dim1;
	i__4 = i__;
	q__1.r = work[i__3].r - sd[i__4], q__1.i = work[i__3].i;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
	if (*kband == 1) {
	    if (i__ != 1) {
		i__2 = i__ + (i__ - 1) * work_dim1;
		i__3 = i__ + (i__ - 1) * work_dim1;
		i__4 = i__ - 1;
		q__1.r = work[i__3].r - se[i__4], q__1.i = work[i__3].i;
		work[i__2].r = q__1.r, work[i__2].i = q__1.i;
	    }
	    if (i__ != *n) {
		i__2 = i__ + (i__ + 1) * work_dim1;
		i__3 = i__ + (i__ + 1) * work_dim1;
		i__4 = i__;
		q__1.r = work[i__3].r - se[i__4], q__1.i = work[i__3].i;
		work[i__2].r = q__1.r, work[i__2].i = q__1.i;
	    }
	}
/* L40: */
    }

    wnorm = clansy_("1", "L", m, &work[work_offset], m, &rwork[1]);

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

/*     Compute  U*U - I */

    cgemm_("T", "N", m, m, n, &c_b2, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b1, &work[work_offset], m);

    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j + j * work_dim1;
	i__3 = j + j * work_dim1;
	q__1.r = work[i__3].r - 1.f, q__1.i = work[i__3].i;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L50: */
    }

/* Computing MIN */
    r__1 = (real) (*m), r__2 = clange_("1", m, m, &work[work_offset], m, &
	    rwork[1]);
    result[2] = dmin(r__1,r__2) / (*m * ulp);

    return 0;

/*     End of CSTT22 */

} /* cstt22_ */
