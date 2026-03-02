#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b6 = -1.f;
static integer c__1 = 1;
static real c_b8 = 0.f;

/* Subroutine */ int sbdt03_(char *uplo, integer *n, integer *kd, real *d__, 
	real *e, real *u, integer *ldu, real *s, real *vt, integer *ldvt, 
	real *work, real *resid)
{
    /* System generated locals */
    integer u_dim1, u_offset, vt_dim1, vt_offset, i__1, i__2;
    real r__1, r__2, r__3, r__4;

    /* Local variables */
    integer i__, j;
    real eps;
    extern logical lsame_(char *, char *);
    real bnorm;
    extern /* Subroutine */ int sgemv_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *);
    extern doublereal sasum_(integer *, real *, integer *), slamch_(char *);
    extern integer isamax_(integer *, real *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SBDT03 reconstructs a bidiagonal matrix B from its SVD: */
/*     S = U' * B * V */
/*  where U and V are orthogonal matrices and S is diagonal. */

/*  The test ratio to test the singular value decomposition is */
/*     RESID = norm( B - U * S * VT ) / ( n * norm(B) * EPS ) */
/*  where VT = V' and EPS is the machine precision. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix B is upper or lower bidiagonal. */
/*          = 'U':  Upper bidiagonal */
/*          = 'L':  Lower bidiagonal */

/*  N       (input) INTEGER */
/*          The order of the matrix B. */

/*  KD      (input) INTEGER */
/*          The bandwidth of the bidiagonal matrix B.  If KD = 1, the */
/*          matrix B is bidiagonal, and if KD = 0, B is diagonal and E is */
/*          not referenced.  If KD is greater than 1, it is assumed to be */
/*          1, and if KD is less than 0, it is assumed to be 0. */

/*  D       (input) REAL array, dimension (N) */
/*          The n diagonal elements of the bidiagonal matrix B. */

/*  E       (input) REAL array, dimension (N-1) */
/*          The (n-1) superdiagonal elements of the bidiagonal matrix B */
/*          if UPLO = 'U', or the (n-1) subdiagonal elements of B if */
/*          UPLO = 'L'. */

/*  U       (input) REAL array, dimension (LDU,N) */
/*          The n by n orthogonal matrix U in the reduction B = U'*A*P. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= max(1,N) */

/*  S       (input) REAL array, dimension (N) */
/*          The singular values from the SVD of B, sorted in decreasing */
/*          order. */

/*  VT      (input) REAL array, dimension (LDVT,N) */
/*          The n by n orthogonal matrix V' in the reduction */
/*          B = U * S * V'. */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of the array VT. */

/*  WORK    (workspace) REAL array, dimension (2*N) */

/*  RESID   (output) REAL */
/*          The test ratio:  norm(B - U * S * V') / ( n * norm(A) * EPS ) */

/* ====================================================================== */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --s;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --work;

    /* Function Body */
    *resid = 0.f;
    if (*n <= 0) {
	return 0;
    }

/*     Compute B - U * S * V' one column at a time. */

    bnorm = 0.f;
    if (*kd >= 1) {

/*        B is bidiagonal. */

	if (lsame_(uplo, "U")) {

/*           B is upper bidiagonal. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = s[i__] * vt[i__ + j * vt_dim1];
/* L10: */
		}
		sgemv_("No transpose", n, n, &c_b6, &u[u_offset], ldu, &work[*
			n + 1], &c__1, &c_b8, &work[1], &c__1);
		work[j] += d__[j];
		if (j > 1) {
		    work[j - 1] += e[j - 1];
/* Computing MAX */
		    r__3 = bnorm, r__4 = (r__1 = d__[j], dabs(r__1)) + (r__2 =
			     e[j - 1], dabs(r__2));
		    bnorm = dmax(r__3,r__4);
		} else {
/* Computing MAX */
		    r__2 = bnorm, r__3 = (r__1 = d__[j], dabs(r__1));
		    bnorm = dmax(r__2,r__3);
		}
/* Computing MAX */
		r__1 = *resid, r__2 = sasum_(n, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L20: */
	    }
	} else {

/*           B is lower bidiagonal. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = s[i__] * vt[i__ + j * vt_dim1];
/* L30: */
		}
		sgemv_("No transpose", n, n, &c_b6, &u[u_offset], ldu, &work[*
			n + 1], &c__1, &c_b8, &work[1], &c__1);
		work[j] += d__[j];
		if (j < *n) {
		    work[j + 1] += e[j];
/* Computing MAX */
		    r__3 = bnorm, r__4 = (r__1 = d__[j], dabs(r__1)) + (r__2 =
			     e[j], dabs(r__2));
		    bnorm = dmax(r__3,r__4);
		} else {
/* Computing MAX */
		    r__2 = bnorm, r__3 = (r__1 = d__[j], dabs(r__1));
		    bnorm = dmax(r__2,r__3);
		}
/* Computing MAX */
		r__1 = *resid, r__2 = sasum_(n, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L40: */
	    }
	}
    } else {

/*        B is diagonal. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[*n + i__] = s[i__] * vt[i__ + j * vt_dim1];
/* L50: */
	    }
	    sgemv_("No transpose", n, n, &c_b6, &u[u_offset], ldu, &work[*n + 
		    1], &c__1, &c_b8, &work[1], &c__1);
	    work[j] += d__[j];
/* Computing MAX */
	    r__1 = *resid, r__2 = sasum_(n, &work[1], &c__1);
	    *resid = dmax(r__1,r__2);
/* L60: */
	}
	j = isamax_(n, &d__[1], &c__1);
	bnorm = (r__1 = d__[j], dabs(r__1));
    }

/*     Compute norm(B - U * S * V') / ( n * norm(B) * EPS ) */

    eps = slamch_("Precision");

    if (bnorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	if (bnorm >= *resid) {
	    *resid = *resid / bnorm / ((real) (*n) * eps);
	} else {
	    if (bnorm < 1.f) {
/* Computing MIN */
		r__1 = *resid, r__2 = (real) (*n) * bnorm;
		*resid = dmin(r__1,r__2) / bnorm / ((real) (*n) * eps);
	    } else {
/* Computing MIN */
		r__1 = *resid / bnorm, r__2 = (real) (*n);
		*resid = dmin(r__1,r__2) / ((real) (*n) * eps);
	    }
	}
    }

    return 0;

/*     End of SBDT03 */

} /* sbdt03_ */
