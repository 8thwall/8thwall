#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b7 = -1.;
static doublereal c_b9 = 1.;

/* Subroutine */ int dbdt01_(integer *m, integer *n, integer *kd, doublereal *
	a, integer *lda, doublereal *q, integer *ldq, doublereal *d__, 
	doublereal *e, doublereal *pt, integer *ldpt, doublereal *work, 
	doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, pt_dim1, pt_offset, q_dim1, q_offset, i__1, 
	    i__2;
    doublereal d__1, d__2;

    /* Local variables */
    integer i__, j;
    doublereal eps;
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DBDT01 reconstructs a general matrix A from its bidiagonal form */
/*     A = Q * B * P' */
/*  where Q (m by min(m,n)) and P' (min(m,n) by n) are orthogonal */
/*  matrices and B is bidiagonal. */

/*  The test ratio to test the reduction is */
/*     RESID = norm( A - Q * B * PT ) / ( n * norm(A) * EPS ) */
/*  where PT = P' and EPS is the machine precision. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrices A and Q. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and P'. */

/*  KD      (input) INTEGER */
/*          If KD = 0, B is diagonal and the array E is not referenced. */
/*          If KD = 1, the reduction was performed by xGEBRD; B is upper */
/*          bidiagonal if M >= N, and lower bidiagonal if M < N. */
/*          If KD = -1, the reduction was performed by xGBBRD; B is */
/*          always upper bidiagonal. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  Q       (input) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          The m by min(m,n) orthogonal matrix Q in the reduction */
/*          A = Q * B * P'. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,M). */

/*  D       (input) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The diagonal elements of the bidiagonal matrix B. */

/*  E       (input) DOUBLE PRECISION array, dimension (min(M,N)-1) */
/*          The superdiagonal elements of the bidiagonal matrix B if */
/*          m >= n, or the subdiagonal elements of B if m < n. */

/*  PT      (input) DOUBLE PRECISION array, dimension (LDPT,N) */
/*          The min(m,n) by n orthogonal matrix P' in the reduction */
/*          A = Q * B * P'. */

/*  LDPT    (input) INTEGER */
/*          The leading dimension of the array PT. */
/*          LDPT >= max(1,min(M,N)). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (M+N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          The test ratio:  norm(A - Q * B * P') / ( n * norm(A) * EPS ) */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --d__;
    --e;
    pt_dim1 = *ldpt;
    pt_offset = 1 + pt_dim1;
    pt -= pt_offset;
    --work;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Compute A - Q * B * P' one column at a time. */

    *resid = 0.;
    if (*kd != 0) {

/*        B is bidiagonal. */

	if (*kd != 0 && *m >= *n) {

/*           B is upper bidiagonal and M >= N. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *n - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*m + i__] = d__[i__] * pt[i__ + j * pt_dim1] + e[i__]
			     * pt[i__ + 1 + j * pt_dim1];
/* L10: */
		}
		work[*m + *n] = d__[*n] * pt[*n + j * pt_dim1];
		dgemv_("No transpose", m, n, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b9, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L20: */
	    }
	} else if (*kd < 0) {

/*           B is upper bidiagonal and M < N. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*m + i__] = d__[i__] * pt[i__ + j * pt_dim1] + e[i__]
			     * pt[i__ + 1 + j * pt_dim1];
/* L30: */
		}
		work[*m + *m] = d__[*m] * pt[*m + j * pt_dim1];
		dgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b9, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L40: */
	    }
	} else {

/*           B is lower bidiagonal. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		work[*m + 1] = d__[1] * pt[j * pt_dim1 + 1];
		i__2 = *m;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    work[*m + i__] = e[i__ - 1] * pt[i__ - 1 + j * pt_dim1] + 
			    d__[i__] * pt[i__ + j * pt_dim1];
/* L50: */
		}
		dgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b9, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L60: */
	    }
	}
    } else {

/*        B is diagonal. */

	if (*m >= *n) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*m + i__] = d__[i__] * pt[i__ + j * pt_dim1];
/* L70: */
		}
		dgemv_("No transpose", m, n, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b9, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L80: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*m + i__] = d__[i__] * pt[i__ + j * pt_dim1];
/* L90: */
		}
		dgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b9, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L100: */
	    }
	}
    }

/*     Compute norm(A - Q * B * P') / ( n * norm(A) * EPS ) */

    anorm = dlange_("1", m, n, &a[a_offset], lda, &work[1]);
    eps = dlamch_("Precision");

    if (anorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	if (anorm >= *resid) {
	    *resid = *resid / anorm / ((doublereal) (*n) * eps);
	} else {
	    if (anorm < 1.) {
/* Computing MIN */
		d__1 = *resid, d__2 = (doublereal) (*n) * anorm;
		*resid = min(d__1,d__2) / anorm / ((doublereal) (*n) * eps);
	    } else {
/* Computing MIN */
		d__1 = *resid / anorm, d__2 = (doublereal) (*n);
		*resid = min(d__1,d__2) / ((doublereal) (*n) * eps);
	    }
	}
    }

    return 0;

/*     End of DBDT01 */

} /* dbdt01_ */
