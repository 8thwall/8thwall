#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int cspt03_(char *uplo, integer *n, complex *a, complex *
	ainv, complex *work, integer *ldw, real *rwork, real *rcond, real *
	resid)
{
    /* System generated locals */
    integer work_dim1, work_offset, i__1, i__2, i__3, i__4, i__5;
    complex q__1, q__2;

    /* Local variables */
    integer i__, j, k;
    complex t;
    real eps;
    integer icol, jcol, kcol, nall;
    extern logical lsame_(char *, char *);
    real anorm;
    extern /* Complex */ VOID cdotu_(complex *, integer *, complex *, integer 
	    *, complex *, integer *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *), clansp_(char 
	    *, char *, integer *, complex *, real *);
    real ainvnm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CSPT03 computes the residual for a complex symmetric packed matrix */
/*  times its inverse: */
/*     norm( I - A*AINV ) / ( N * norm(A) * norm(AINV) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          complex symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The original complex symmetric matrix A, stored as a packed */
/*          triangular matrix. */

/*  AINV    (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The (symmetric) inverse of the matrix A, stored as a packed */
/*          triangular matrix. */

/*  WORK    (workspace) COMPLEX array, dimension (LDWORK,N) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK.  LDWORK >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RCOND   (output) REAL */
/*          The reciprocal of the condition number of A, computed as */
/*          ( 1/norm(A) ) / norm(AINV). */

/*  RESID   (output) REAL */
/*          norm(I - A*AINV) / ( N * norm(A) * norm(AINV) * EPS ) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick exit if N = 0. */

    /* Parameter adjustments */
    --a;
    --ainv;
    work_dim1 = *ldw;
    work_offset = 1 + work_dim1;
    work -= work_offset;
    --rwork;

    /* Function Body */
    if (*n <= 0) {
	*rcond = 1.f;
	*resid = 0.f;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0 or AINVNM = 0. */

    eps = slamch_("Epsilon");
    anorm = clansp_("1", uplo, n, &a[1], &rwork[1]);
    ainvnm = clansp_("1", uplo, n, &ainv[1], &rwork[1]);
    if (anorm <= 0.f || ainvnm <= 0.f) {
	*rcond = 0.f;
	*resid = 1.f / eps;
	return 0;
    }
    *rcond = 1.f / anorm / ainvnm;

/*     Case where both A and AINV are upper triangular: */
/*     Each element of - A * AINV is computed by taking the dot product */
/*     of a row of A with a column of AINV. */

    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    icol = (i__ - 1) * i__ / 2 + 1;

/*           Code when J <= I */

	    i__2 = i__;
	    for (j = 1; j <= i__2; ++j) {
		jcol = (j - 1) * j / 2 + 1;
		cdotu_(&q__1, &j, &a[icol], &c__1, &ainv[jcol], &c__1);
		t.r = q__1.r, t.i = q__1.i;
		jcol = jcol + (j << 1) - 1;
		kcol = icol - 1;
		i__3 = i__;
		for (k = j + 1; k <= i__3; ++k) {
		    i__4 = kcol + k;
		    i__5 = jcol;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    jcol += k;
/* L10: */
		}
		kcol += i__ << 1;
		i__3 = *n;
		for (k = i__ + 1; k <= i__3; ++k) {
		    i__4 = kcol;
		    i__5 = jcol;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    kcol += k;
		    jcol += k;
/* L20: */
		}
		i__3 = i__ + j * work_dim1;
		q__1.r = -t.r, q__1.i = -t.i;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L30: */
	    }

/*           Code when J > I */

	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		jcol = (j - 1) * j / 2 + 1;
		cdotu_(&q__1, &i__, &a[icol], &c__1, &ainv[jcol], &c__1);
		t.r = q__1.r, t.i = q__1.i;
		--jcol;
		kcol = icol + (i__ << 1) - 1;
		i__3 = j;
		for (k = i__ + 1; k <= i__3; ++k) {
		    i__4 = kcol;
		    i__5 = jcol + k;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    kcol += k;
/* L40: */
		}
		jcol += j << 1;
		i__3 = *n;
		for (k = j + 1; k <= i__3; ++k) {
		    i__4 = kcol;
		    i__5 = jcol;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    kcol += k;
		    jcol += k;
/* L50: */
		}
		i__3 = i__ + j * work_dim1;
		q__1.r = -t.r, q__1.i = -t.i;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L60: */
	    }
/* L70: */
	}
    } else {

/*        Case where both A and AINV are lower triangular */

	nall = *n * (*n + 1) / 2;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Code when J <= I */

	    icol = nall - (*n - i__ + 1) * (*n - i__ + 2) / 2 + 1;
	    i__2 = i__;
	    for (j = 1; j <= i__2; ++j) {
		jcol = nall - (*n - j) * (*n - j + 1) / 2 - (*n - i__);
		i__3 = *n - i__ + 1;
		cdotu_(&q__1, &i__3, &a[icol], &c__1, &ainv[jcol], &c__1);
		t.r = q__1.r, t.i = q__1.i;
		kcol = i__;
		jcol = j;
		i__3 = j - 1;
		for (k = 1; k <= i__3; ++k) {
		    i__4 = kcol;
		    i__5 = jcol;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    jcol = jcol + *n - k;
		    kcol = kcol + *n - k;
/* L80: */
		}
		jcol -= j;
		i__3 = i__ - 1;
		for (k = j; k <= i__3; ++k) {
		    i__4 = kcol;
		    i__5 = jcol + k;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    kcol = kcol + *n - k;
/* L90: */
		}
		i__3 = i__ + j * work_dim1;
		q__1.r = -t.r, q__1.i = -t.i;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L100: */
	    }

/*           Code when J > I */

	    icol = nall - (*n - i__) * (*n - i__ + 1) / 2;
	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		jcol = nall - (*n - j + 1) * (*n - j + 2) / 2 + 1;
		i__3 = *n - j + 1;
		cdotu_(&q__1, &i__3, &a[icol - *n + j], &c__1, &ainv[jcol], &
			c__1);
		t.r = q__1.r, t.i = q__1.i;
		kcol = i__;
		jcol = j;
		i__3 = i__ - 1;
		for (k = 1; k <= i__3; ++k) {
		    i__4 = kcol;
		    i__5 = jcol;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    jcol = jcol + *n - k;
		    kcol = kcol + *n - k;
/* L110: */
		}
		kcol -= i__;
		i__3 = j - 1;
		for (k = i__; k <= i__3; ++k) {
		    i__4 = kcol + k;
		    i__5 = jcol;
		    q__2.r = a[i__4].r * ainv[i__5].r - a[i__4].i * ainv[i__5]
			    .i, q__2.i = a[i__4].r * ainv[i__5].i + a[i__4].i 
			    * ainv[i__5].r;
		    q__1.r = t.r + q__2.r, q__1.i = t.i + q__2.i;
		    t.r = q__1.r, t.i = q__1.i;
		    jcol = jcol + *n - k;
/* L120: */
		}
		i__3 = i__ + j * work_dim1;
		q__1.r = -t.r, q__1.i = -t.i;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L130: */
	    }
/* L140: */
	}
    }

/*     Add the identity matrix to WORK . */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * work_dim1;
	i__3 = i__ + i__ * work_dim1;
	q__1.r = work[i__3].r + 1.f, q__1.i = work[i__3].i;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L150: */
    }

/*     Compute norm(I - A*AINV) / (N * norm(A) * norm(AINV) * EPS) */

    *resid = clange_("1", n, n, &work[work_offset], ldw, &rwork[1])
	    ;

    *resid = *resid * *rcond / eps / (real) (*n);

    return 0;

/*     End of CSPT03 */

} /* cspt03_ */
