#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b6 = {-1.f,-0.f};
static integer c__1 = 1;
static complex c_b9 = {0.f,0.f};

/* Subroutine */ int cbdt03_(char *uplo, integer *n, integer *kd, real *d__, 
	real *e, complex *u, integer *ldu, real *s, complex *vt, integer *
	ldvt, complex *work, real *resid)
{
    /* System generated locals */
    integer u_dim1, u_offset, vt_dim1, vt_offset, i__1, i__2, i__3, i__4, 
	    i__5;
    real r__1, r__2, r__3, r__4;
    complex q__1;

    /* Local variables */
    integer i__, j;
    real eps;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int cgemv_(char *, integer *, integer *, complex *
, complex *, integer *, complex *, integer *, complex *, complex *
, integer *);
    real bnorm;
    extern doublereal slamch_(char *);
    extern integer isamax_(integer *, real *, integer *);
    extern doublereal scasum_(integer *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CBDT03 reconstructs a bidiagonal matrix B from its SVD: */
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

/*  U       (input) COMPLEX array, dimension (LDU,N) */
/*          The n by n orthogonal matrix U in the reduction B = U'*A*P. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= max(1,N) */

/*  S       (input) REAL array, dimension (N) */
/*          The singular values from the SVD of B, sorted in decreasing */
/*          order. */

/*  VT      (input) COMPLEX array, dimension (LDVT,N) */
/*          The n by n orthogonal matrix V' in the reduction */
/*          B = U * S * V'. */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of the array VT. */

/*  WORK    (workspace) COMPLEX array, dimension (2*N) */

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
		    i__3 = *n + i__;
		    i__4 = i__;
		    i__5 = i__ + j * vt_dim1;
		    q__1.r = s[i__4] * vt[i__5].r, q__1.i = s[i__4] * vt[i__5]
			    .i;
		    work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L10: */
		}
		cgemv_("No transpose", n, n, &c_b6, &u[u_offset], ldu, &work[*
			n + 1], &c__1, &c_b9, &work[1], &c__1);
		i__2 = j;
		i__3 = j;
		i__4 = j;
		q__1.r = work[i__3].r + d__[i__4], q__1.i = work[i__3].i;
		work[i__2].r = q__1.r, work[i__2].i = q__1.i;
		if (j > 1) {
		    i__2 = j - 1;
		    i__3 = j - 1;
		    i__4 = j - 1;
		    q__1.r = work[i__3].r + e[i__4], q__1.i = work[i__3].i;
		    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
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
		r__1 = *resid, r__2 = scasum_(n, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L20: */
	    }
	} else {

/*           B is lower bidiagonal. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *n + i__;
		    i__4 = i__;
		    i__5 = i__ + j * vt_dim1;
		    q__1.r = s[i__4] * vt[i__5].r, q__1.i = s[i__4] * vt[i__5]
			    .i;
		    work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L30: */
		}
		cgemv_("No transpose", n, n, &c_b6, &u[u_offset], ldu, &work[*
			n + 1], &c__1, &c_b9, &work[1], &c__1);
		i__2 = j;
		i__3 = j;
		i__4 = j;
		q__1.r = work[i__3].r + d__[i__4], q__1.i = work[i__3].i;
		work[i__2].r = q__1.r, work[i__2].i = q__1.i;
		if (j < *n) {
		    i__2 = j + 1;
		    i__3 = j + 1;
		    i__4 = j;
		    q__1.r = work[i__3].r + e[i__4], q__1.i = work[i__3].i;
		    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
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
		r__1 = *resid, r__2 = scasum_(n, &work[1], &c__1);
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
		i__3 = *n + i__;
		i__4 = i__;
		i__5 = i__ + j * vt_dim1;
		q__1.r = s[i__4] * vt[i__5].r, q__1.i = s[i__4] * vt[i__5].i;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L50: */
	    }
	    cgemv_("No transpose", n, n, &c_b6, &u[u_offset], ldu, &work[*n + 
		    1], &c__1, &c_b9, &work[1], &c__1);
	    i__2 = j;
	    i__3 = j;
	    i__4 = j;
	    q__1.r = work[i__3].r + d__[i__4], q__1.i = work[i__3].i;
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* Computing MAX */
	    r__1 = *resid, r__2 = scasum_(n, &work[1], &c__1);
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

/*     End of CBDT03 */

} /* cbdt03_ */
