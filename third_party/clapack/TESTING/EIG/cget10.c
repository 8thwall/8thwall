#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static complex c_b9 = {-1.f,0.f};

/* Subroutine */ int cget10_(integer *m, integer *n, complex *a, integer *lda, 
	 complex *b, integer *ldb, complex *work, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;
    real r__1, r__2;

    /* Local variables */
    integer j;
    real eps, unfl, anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), caxpy_(integer *, complex *, complex *, 
	    integer *, complex *, integer *);
    real wnorm;
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

/*  CGET10 compares two matrices A and B and computes the ratio */
/*  RESULT = norm( A - B ) / ( norm(A) * M * EPS ) */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrices A and B. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (input) COMPLEX array, dimension (LDB,N) */
/*          The m by n matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,M). */

/*  WORK    (workspace) COMPLEX array, dimension (M) */

/*  RWORK   (workspace) COMPLEX array, dimension (M) */

/*  RESULT  (output) REAL */
/*          RESULT = norm( A - B ) / ( norm(A) * M * EPS ) */

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
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;
    --rwork;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	*result = 0.f;
	return 0;
    }

    unfl = slamch_("Safe minimum");
    eps = slamch_("Precision");

    wnorm = 0.f;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	ccopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
	caxpy_(m, &c_b9, &b[j * b_dim1 + 1], &c__1, &work[1], &c__1);
/* Computing MAX */
	r__1 = wnorm, r__2 = scasum_(n, &work[1], &c__1);
	wnorm = dmax(r__1,r__2);
/* L10: */
    }

/* Computing MAX */
    r__1 = clange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);

    if (anorm > wnorm) {
	*result = wnorm / anorm / (*m * eps);
    } else {
	if (anorm < 1.f) {
/* Computing MIN */
	    r__1 = wnorm, r__2 = *m * anorm;
	    *result = dmin(r__1,r__2) / anorm / (*m * eps);
	} else {
/* Computing MIN */
	    r__1 = wnorm / anorm, r__2 = (real) (*m);
	    *result = dmin(r__1,r__2) / (*m * eps);
	}
    }

    return 0;

/*     End of CGET10 */

} /* cget10_ */
