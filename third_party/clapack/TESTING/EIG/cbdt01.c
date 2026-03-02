#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static complex c_b7 = {-1.f,-0.f};
static complex c_b10 = {1.f,0.f};

/* Subroutine */ int cbdt01_(integer *m, integer *n, integer *kd, complex *a, 
	integer *lda, complex *q, integer *ldq, real *d__, real *e, complex *
	pt, integer *ldpt, complex *work, real *rwork, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, pt_dim1, pt_offset, q_dim1, q_offset, i__1, 
	    i__2, i__3, i__4, i__5, i__6, i__7;
    real r__1, r__2;
    complex q__1, q__2, q__3;

    /* Local variables */
    integer i__, j;
    real eps;
    extern /* Subroutine */ int cgemv_(char *, integer *, integer *, complex *
, complex *, integer *, complex *, integer *, complex *, complex *
, integer *);
    real anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *), scasum_(
	    integer *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CBDT01 reconstructs a general matrix A from its bidiagonal form */
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

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  Q       (input) COMPLEX array, dimension (LDQ,N) */
/*          The m by min(m,n) unitary matrix Q in the reduction */
/*          A = Q * B * P'. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,M). */

/*  D       (input) REAL array, dimension (min(M,N)) */
/*          The diagonal elements of the bidiagonal matrix B. */

/*  E       (input) REAL array, dimension (min(M,N)-1) */
/*          The superdiagonal elements of the bidiagonal matrix B if */
/*          m >= n, or the subdiagonal elements of B if m < n. */

/*  PT      (input) COMPLEX array, dimension (LDPT,N) */
/*          The min(m,n) by n unitary matrix P' in the reduction */
/*          A = Q * B * P'. */

/*  LDPT    (input) INTEGER */
/*          The leading dimension of the array PT. */
/*          LDPT >= max(1,min(M,N)). */

/*  WORK    (workspace) COMPLEX array, dimension (M+N) */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESID   (output) REAL */
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
	*resid = 0.f;
	return 0;
    }

/*     Compute A - Q * B * P' one column at a time. */

    *resid = 0.f;
    if (*kd != 0) {

/*        B is bidiagonal. */

	if (*kd != 0 && *m >= *n) {

/*           B is upper bidiagonal and M >= N. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		ccopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *n - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    q__2.r = d__[i__4] * pt[i__5].r, q__2.i = d__[i__4] * pt[
			    i__5].i;
		    i__6 = i__;
		    i__7 = i__ + 1 + j * pt_dim1;
		    q__3.r = e[i__6] * pt[i__7].r, q__3.i = e[i__6] * pt[i__7]
			    .i;
		    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
		    work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L10: */
		}
		i__2 = *m + *n;
		i__3 = *n;
		i__4 = *n + j * pt_dim1;
		q__1.r = d__[i__3] * pt[i__4].r, q__1.i = d__[i__3] * pt[i__4]
			.i;
		work[i__2].r = q__1.r, work[i__2].i = q__1.i;
		cgemv_("No transpose", m, n, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		r__1 = *resid, r__2 = scasum_(m, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L20: */
	    }
	} else if (*kd < 0) {

/*           B is upper bidiagonal and M < N. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		ccopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    q__2.r = d__[i__4] * pt[i__5].r, q__2.i = d__[i__4] * pt[
			    i__5].i;
		    i__6 = i__;
		    i__7 = i__ + 1 + j * pt_dim1;
		    q__3.r = e[i__6] * pt[i__7].r, q__3.i = e[i__6] * pt[i__7]
			    .i;
		    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
		    work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L30: */
		}
		i__2 = *m + *m;
		i__3 = *m;
		i__4 = *m + j * pt_dim1;
		q__1.r = d__[i__3] * pt[i__4].r, q__1.i = d__[i__3] * pt[i__4]
			.i;
		work[i__2].r = q__1.r, work[i__2].i = q__1.i;
		cgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		r__1 = *resid, r__2 = scasum_(m, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L40: */
	    }
	} else {

/*           B is lower bidiagonal. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		ccopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m + 1;
		i__3 = j * pt_dim1 + 1;
		q__1.r = d__[1] * pt[i__3].r, q__1.i = d__[1] * pt[i__3].i;
		work[i__2].r = q__1.r, work[i__2].i = q__1.i;
		i__2 = *m;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__ - 1;
		    i__5 = i__ - 1 + j * pt_dim1;
		    q__2.r = e[i__4] * pt[i__5].r, q__2.i = e[i__4] * pt[i__5]
			    .i;
		    i__6 = i__;
		    i__7 = i__ + j * pt_dim1;
		    q__3.r = d__[i__6] * pt[i__7].r, q__3.i = d__[i__6] * pt[
			    i__7].i;
		    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
		    work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L50: */
		}
		cgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		r__1 = *resid, r__2 = scasum_(m, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L60: */
	    }
	}
    } else {

/*        B is diagonal. */

	if (*m >= *n) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		ccopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    q__1.r = d__[i__4] * pt[i__5].r, q__1.i = d__[i__4] * pt[
			    i__5].i;
		    work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L70: */
		}
		cgemv_("No transpose", m, n, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		r__1 = *resid, r__2 = scasum_(m, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L80: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		ccopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = *m + i__;
		    i__4 = i__;
		    i__5 = i__ + j * pt_dim1;
		    q__1.r = d__[i__4] * pt[i__5].r, q__1.i = d__[i__4] * pt[
			    i__5].i;
		    work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L90: */
		}
		cgemv_("No transpose", m, m, &c_b7, &q[q_offset], ldq, &work[*
			m + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
		r__1 = *resid, r__2 = scasum_(m, &work[1], &c__1);
		*resid = dmax(r__1,r__2);
/* L100: */
	    }
	}
    }

/*     Compute norm(A - Q * B * P') / ( n * norm(A) * EPS ) */

    anorm = clange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    eps = slamch_("Precision");

    if (anorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	if (anorm >= *resid) {
	    *resid = *resid / anorm / ((real) (*n) * eps);
	} else {
	    if (anorm < 1.f) {
/* Computing MIN */
		r__1 = *resid, r__2 = (real) (*n) * anorm;
		*resid = dmin(r__1,r__2) / anorm / ((real) (*n) * eps);
	    } else {
/* Computing MIN */
		r__1 = *resid / anorm, r__2 = (real) (*n);
		*resid = dmin(r__1,r__2) / ((real) (*n) * eps);
	    }
	}
    }

    return 0;

/*     End of CBDT01 */

} /* cbdt01_ */
