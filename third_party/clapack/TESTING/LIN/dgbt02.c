#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b8 = -1.;
static doublereal c_b10 = 1.;

/* Subroutine */ int dgbt02_(char *trans, integer *m, integer *n, integer *kl, 
	 integer *ku, integer *nrhs, doublereal *a, integer *lda, doublereal *
	x, integer *ldx, doublereal *b, integer *ldb, doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, i__1, i__2, 
	    i__3;
    doublereal d__1, d__2;

    /* Local variables */
    integer j, i1, i2, n1, kd;
    doublereal eps;
    extern /* Subroutine */ int dgbmv_(char *, integer *, integer *, integer *
, integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    extern logical lsame_(char *, char *);
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm, bnorm, xnorm;
    extern doublereal dlamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBT02 computes the residual for a solution of a banded system of */
/*  equations  A*x = b  or  A'*x = b: */
/*     RESID = norm( B - A*X ) / ( norm(A) * norm(X) * EPS). */
/*  where EPS is the machine precision. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A *x = b */
/*          = 'T':  A'*x = b, where A' is the transpose of A */
/*          = 'C':  A'*x = b, where A' is the transpose of A */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of B.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The original matrix A in band storage, stored in rows 1 to */
/*          KL+KU+1. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,KL+KU+1). */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The computed solution vectors for the system of linear */
/*          equations. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  If TRANS = 'N', */
/*          LDX >= max(1,N); if TRANS = 'T' or 'C', LDX >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side vectors for the system of */
/*          linear equations. */
/*          On exit, B is overwritten with the difference B - A*X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  IF TRANS = 'N', */
/*          LDB >= max(1,M); if TRANS = 'T' or 'C', LDB >= max(1,N). */

/*  RESID   (output) DOUBLE PRECISION */
/*          The maximum over the number of right hand sides of */
/*          norm(B - A*X) / ( norm(A) * norm(X) * EPS ). */

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

/*     Quick return if N = 0 pr NRHS = 0 */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0 || *nrhs <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = dlamch_("Epsilon");
    kd = *ku + 1;
    anorm = 0.;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__2 = kd + 1 - j;
	i1 = max(i__2,1);
/* Computing MIN */
	i__2 = kd + *m - j, i__3 = *kl + kd;
	i2 = min(i__2,i__3);
/* Computing MAX */
	i__2 = i2 - i1 + 1;
	d__1 = anorm, d__2 = dasum_(&i__2, &a[i1 + j * a_dim1], &c__1);
	anorm = max(d__1,d__2);
/* L10: */
    }
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

    if (lsame_(trans, "T") || lsame_(trans, "C")) {
	n1 = *n;
    } else {
	n1 = *m;
    }

/*     Compute  B - A*X (or  B - A'*X ) */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	dgbmv_(trans, m, n, kl, ku, &c_b8, &a[a_offset], lda, &x[j * x_dim1 + 
		1], &c__1, &c_b10, &b[j * b_dim1 + 1], &c__1);
/* L20: */
    }

/*     Compute the maximum over the number of right hand sides of */
/*        norm(B - A*X) / ( norm(A) * norm(X) * EPS ). */

    *resid = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = dasum_(&n1, &b[j * b_dim1 + 1], &c__1);
	xnorm = dasum_(&n1, &x[j * x_dim1 + 1], &c__1);
	if (xnorm <= 0.) {
	    *resid = 1. / eps;
	} else {
/* Computing MAX */
	    d__1 = *resid, d__2 = bnorm / anorm / xnorm / eps;
	    *resid = max(d__1,d__2);
	}
/* L30: */
    }

    return 0;

/*     End of DGBT02 */

} /* dgbt02_ */
