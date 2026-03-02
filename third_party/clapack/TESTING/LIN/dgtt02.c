#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b6 = -1.;
static doublereal c_b7 = 1.;
static integer c__1 = 1;

/* Subroutine */ int dgtt02_(char *trans, integer *n, integer *nrhs, 
	doublereal *dl, doublereal *d__, doublereal *du, doublereal *x, 
	integer *ldx, doublereal *b, integer *ldb, doublereal *rwork, 
	doublereal *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j;
    doublereal eps;
    extern logical lsame_(char *, char *);
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm, bnorm, xnorm;
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int dlagtm_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *);
    extern doublereal dlangt_(char *, integer *, doublereal *, doublereal *, 
	    doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGTT02 computes the residual for the solution to a tridiagonal */
/*  system of equations: */
/*     RESID = norm(B - op(A)*X) / (norm(A) * norm(X) * EPS), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER */
/*          Specifies the form of the residual. */
/*          = 'N':  B - A * X  (No transpose) */
/*          = 'T':  B - A'* X  (Transpose) */
/*          = 'C':  B - A'* X  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGTER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of A. */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The computed solution vectors X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side vectors for the system of */
/*          linear equations. */
/*          On exit, B is overwritten with the difference B - op(A)*X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          norm(B - op(A)*X) / (norm(A) * norm(X) * EPS) */

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

/*     Quick exit if N = 0 or NRHS = 0 */

    /* Parameter adjustments */
    --dl;
    --d__;
    --du;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --rwork;

    /* Function Body */
    *resid = 0.;
    if (*n <= 0 || *nrhs == 0) {
	return 0;
    }

/*     Compute the maximum over the number of right hand sides of */
/*        norm(B - op(A)*X) / ( norm(A) * norm(X) * EPS ). */

    if (lsame_(trans, "N")) {
	anorm = dlangt_("1", n, &dl[1], &d__[1], &du[1]);
    } else {
	anorm = dlangt_("I", n, &dl[1], &d__[1], &du[1]);
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = dlamch_("Epsilon");
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Compute B - op(A)*X. */

    dlagtm_(trans, n, nrhs, &c_b6, &dl[1], &d__[1], &du[1], &x[x_offset], ldx, 
	     &c_b7, &b[b_offset], ldb);

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = dasum_(n, &b[j * b_dim1 + 1], &c__1);
	xnorm = dasum_(n, &x[j * x_dim1 + 1], &c__1);
	if (xnorm <= 0.) {
	    *resid = 1. / eps;
	} else {
/* Computing MAX */
	    d__1 = *resid, d__2 = bnorm / anorm / xnorm / eps;
	    *resid = max(d__1,d__2);
	}
/* L10: */
    }

    return 0;

/*     End of DGTT02 */

} /* dgtt02_ */
