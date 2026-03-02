#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b6 = -1.f;
static real c_b7 = 1.f;
static integer c__1 = 1;

/* Subroutine */ int sgtt02_(char *trans, integer *n, integer *nrhs, real *dl, 
	 real *d__, real *du, real *x, integer *ldx, real *b, integer *ldb, 
	real *rwork, real *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;
    real r__1, r__2;

    /* Local variables */
    integer j;
    real eps;
    extern logical lsame_(char *, char *);
    real anorm, bnorm;
    extern doublereal sasum_(integer *, real *, integer *);
    real xnorm;
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int slagtm_(char *, integer *, integer *, real *, 
	    real *, real *, real *, real *, integer *, real *, real *, 
	    integer *);
    extern doublereal slangt_(char *, integer *, real *, real *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGTT02 computes the residual for the solution to a tridiagonal */
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

/*  DL      (input) REAL array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of A. */

/*  D       (input) REAL array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) REAL array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of A. */

/*  X       (input) REAL array, dimension (LDX,NRHS) */
/*          The computed solution vectors X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  B       (input/output) REAL array, dimension (LDB,NRHS) */
/*          On entry, the right hand side vectors for the system of */
/*          linear equations. */
/*          On exit, B is overwritten with the difference B - op(A)*X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESID   (output) REAL */
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
    *resid = 0.f;
    if (*n <= 0 || *nrhs == 0) {
	return 0;
    }

/*     Compute the maximum over the number of right hand sides of */
/*        norm(B - op(A)*X) / ( norm(A) * norm(X) * EPS ). */

    if (lsame_(trans, "N")) {
	anorm = slangt_("1", n, &dl[1], &d__[1], &du[1]);
    } else {
	anorm = slangt_("I", n, &dl[1], &d__[1], &du[1]);
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = slamch_("Epsilon");
    if (anorm <= 0.f) {
	*resid = 1.f / eps;
	return 0;
    }

/*     Compute B - op(A)*X. */

    slagtm_(trans, n, nrhs, &c_b6, &dl[1], &d__[1], &du[1], &x[x_offset], ldx, 
	     &c_b7, &b[b_offset], ldb);

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	bnorm = sasum_(n, &b[j * b_dim1 + 1], &c__1);
	xnorm = sasum_(n, &x[j * x_dim1 + 1], &c__1);
	if (xnorm <= 0.f) {
	    *resid = 1.f / eps;
	} else {
/* Computing MAX */
	    r__1 = *resid, r__2 = bnorm / anorm / xnorm / eps;
	    *resid = dmax(r__1,r__2);
	}
/* L10: */
    }

    return 0;

/*     End of SGTT02 */

} /* sgtt02_ */
