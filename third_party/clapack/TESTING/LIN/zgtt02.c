#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b6 = -1.;
static doublereal c_b7 = 1.;
static integer c__1 = 1;

/* Subroutine */ int zgtt02_(char *trans, integer *n, integer *nrhs, 
	doublecomplex *dl, doublecomplex *d__, doublecomplex *du, 
	doublecomplex *x, integer *ldx, doublecomplex *b, integer *ldb, 
	doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j;
    doublereal eps;
    extern logical lsame_(char *, char *);
    doublereal anorm, bnorm, xnorm;
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int zlagtm_(char *, integer *, integer *, 
	    doublereal *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *);
    extern doublereal zlangt_(char *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *), dzasum_(integer *, 
	    doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGTT02 computes the residual for the solution to a tridiagonal */
/*  system of equations: */
/*     RESID = norm(B - op(A)*X) / (norm(A) * norm(X) * EPS), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER */
/*          Specifies the form of the residual. */
/*          = 'N':  B - A * X     (No transpose) */
/*          = 'T':  B - A**T * X  (Transpose) */
/*          = 'C':  B - A**H * X  (Conjugate transpose) */

/*  N       (input) INTEGTER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  DL      (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of A. */

/*  D       (input) COMPLEX*16 array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of A. */

/*  X       (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          The computed solution vectors X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  B       (input/output) COMPLEX*16 array, dimension (LDB,NRHS) */
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
	anorm = zlangt_("1", n, &dl[1], &d__[1], &du[1]);
    } else {
	anorm = zlangt_("I", n, &dl[1], &d__[1], &du[1]);
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = dlamch_("Epsilon");
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Compute B - op(A)*X. */

    zlagtm_(trans, n, nrhs, &c_b6, &dl[1], &d__[1], &du[1], &x[x_offset], ldx, 
	     &c_b7, &b[b_offset], ldb);

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = dzasum_(n, &b[j * b_dim1 + 1], &c__1);
	xnorm = dzasum_(n, &x[j * x_dim1 + 1], &c__1);
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

/*     End of ZGTT02 */

} /* zgtt02_ */
