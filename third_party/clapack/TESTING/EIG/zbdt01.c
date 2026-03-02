#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublecomplex c_b7 = {-1.,-0.};
static doublecomplex c_b10 = {1.,0.};

/* Subroutine */ int zbdt01_(integer *m, integer *n, integer *kd, 
	doublecomplex *a, integer *lda, doublecomplex *q, integer *ldq, 
	doublereal *d__, doublereal *e, doublecomplex *pt, integer *ldpt, 
	doublecomplex *work, doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, pt_dim1, pt_offset, q_dim1, q_offset, i__1, 
	    i__2, i__3, i__4, i__5, i__6, i__7;
    doublereal d__1, d__2;
    doublecomplex z__1, z__2, z__3;

    /* Local variables */
    integer i__, j;
    doublereal eps, anorm;
    extern /* Subroutine */ int zgemv_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *), 
	    zcopy_(integer *, doublecomplex *, integer *, doublecomplex *, 
	    integer *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *), 
	    dzasum_(integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZBDT01 reconstructs a general matrix A from its bidiagonal form */
/*     A = Q * B * P' */
/*  where Q (m by min(m,n)) and P' (min(m,n) by n) are unitary */
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

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  Q       (input) COMPLEX*16 array, dimension (LDQ,N) */
/*          The m by min(m,n) unitary matrix Q in the reduction */
/*          A = Q * B * P'. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,M). */

/*  D       (input) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The diagonal elements of the bidiagonal matrix B. */

/*  E       (input) DOUBLE PRECISION array, dimension (min(M,N)-1) */
/*          The superdiagonal elements of the bidiagonal matrix B if */
/*          m >= n, or the subdiagonal elements of B if m < n. */

/*  PT      (input) COMPLEX*16 array, dimension (LDPT,N) */
/*          The min(m,n) by n unitary matrix P' in the reduction */
/*          A = Q * B * P'. */

/*  LDPT    (input) INTEGER */
/*          The leading dimension of the array PT. */
/*          LDPT >= max(1,min(M,N)). */

/*  WORK    (workspace) COMPLEX*16 array, dimension (M+N) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

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
    --rwork;

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
		zcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *n - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    z__2.r = d__[i__4] * pt[i__5].r, z__2.i = d__[i__4] * pt[
			    i__5].i;
		    i__6 = i__;
		    i__7 = i__ + 1 + j * pt_dim1;
		    z__3.r = e[i__6] * pt[i__7].r, z__3.i = e[i__6] * pt[i__7]
			    .i;
		    z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L10: */
		}
		i__2 = *m + *n;
		i__3 = *n;
		i__4 = *n + j * pt_dim1;
		z__1.r = d__[i__3] * pt[i__4].r, z__1.i = d__[i__3] * pt[i__4]
			.i;
		work[i__2].r = z__1.r, work[i__2].i = z__1.i;
		zgemv_("No transpose", m, n, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dzasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L20: */
	    }
	} else if (*kd < 0) {

/*           B is upper bidiagonal and M < N. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		zcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    z__2.r = d__[i__4] * pt[i__5].r, z__2.i = d__[i__4] * pt[
			    i__5].i;
		    i__6 = i__;
		    i__7 = i__ + 1 + j * pt_dim1;
		    z__3.r = e[i__6] * pt[i__7].r, z__3.i = e[i__6] * pt[i__7]
			    .i;
		    z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L30: */
		}
		i__2 = *m + *m;
		i__3 = *m;
		i__4 = *m + j * pt_dim1;
		z__1.r = d__[i__3] * pt[i__4].r, z__1.i = d__[i__3] * pt[i__4]
			.i;
		work[i__2].r = z__1.r, work[i__2].i = z__1.i;
		zgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dzasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L40: */
	    }
	} else {

/*           B is lower bidiagonal. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		zcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m + 1;
		i__3 = j * pt_dim1 + 1;
		z__1.r = d__[1] * pt[i__3].r, z__1.i = d__[1] * pt[i__3].i;
		work[i__2].r = z__1.r, work[i__2].i = z__1.i;
		i__2 = *m;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__ - 1;
		    i__5 = i__ - 1 + j * pt_dim1;
		    z__2.r = e[i__4] * pt[i__5].r, z__2.i = e[i__4] * pt[i__5]
			    .i;
		    i__6 = i__;
		    i__7 = i__ + j * pt_dim1;
		    z__3.r = d__[i__6] * pt[i__7].r, z__3.i = d__[i__6] * pt[
			    i__7].i;
		    z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L50: */
		}
		zgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dzasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L60: */
	    }
	}
    } else {

/*        B is diagonal. */

	if (*m >= *n) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		zcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    z__1.r = d__[i__4] * pt[i__5].r, z__1.i = d__[i__4] * pt[
			    i__5].i;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L70: */
		}
		zgemv_("No transpose", m, n, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dzasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L80: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		zcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    z__1.r = d__[i__4] * pt[i__5].r, z__1.i = d__[i__4] * pt[
			    i__5].i;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L90: */
		}
		zgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		d__1 = *resid, d__2 = dzasum_(m, &work[1], &c__1);
		*resid = max(d__1,d__2);
/* L100: */
	    }
	}
    }

/*     Compute norm(A - Q * B * P') / ( n * norm(A) * EPS ) */

    anorm = zlange_("1", m, n, &a[a_offset], lda, &rwork[1]);
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

/*     End of ZBDT01 */

} /* zbdt01_ */
