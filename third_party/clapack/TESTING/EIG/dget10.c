#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b7 = -1.;

/* Subroutine */ int dget10_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *b, integer *ldb, doublereal *work, doublereal *
	result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j;
    doublereal eps, unfl;
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), daxpy_(integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal wnorm;
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

/*  DGET10 compares two matrices A and B and computes the ratio */
/*  RESULT = norm( A - B ) / ( norm(A) * M * EPS ) */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrices A and B. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,N) */
/*          The m by n matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESULT  (output) DOUBLE PRECISION */
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

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	*result = 0.;
	return 0;
    }

    unfl = dlamch_("Safe minimum");
    eps = dlamch_("Precision");

    wnorm = 0.;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	dcopy_(m, &a[j * a_dim1 + 1], &c__1, &work[1], &c__1);
	daxpy_(m, &c_b7, &b[j * b_dim1 + 1], &c__1, &work[1], &c__1);
/* Computing MAX */
	d__1 = wnorm, d__2 = dasum_(n, &work[1], &c__1);
	wnorm = max(d__1,d__2);
/* L10: */
    }

/* Computing MAX */
    d__1 = dlange_("1", m, n, &a[a_offset], lda, &work[1]);
    anorm = max(d__1,unfl);

    if (anorm > wnorm) {
	*result = wnorm / anorm / (*m * eps);
    } else {
	if (anorm < 1.) {
/* Computing MIN */
	    d__1 = wnorm, d__2 = *m * anorm;
	    *result = min(d__1,d__2) / anorm / (*m * eps);
	} else {
/* Computing MIN */
	    d__1 = wnorm / anorm, d__2 = (doublereal) (*m);
	    *result = min(d__1,d__2) / (*m * eps);
	}
    }

    return 0;

/*     End of DGET10 */

} /* dget10_ */
