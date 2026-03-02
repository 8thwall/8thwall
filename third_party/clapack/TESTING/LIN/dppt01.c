#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b14 = 1.;

/* Subroutine */ int dppt01_(char *uplo, integer *n, doublereal *a, 
	doublereal *afac, doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, k;
    doublereal t;
    integer kc;
    doublereal eps;
    integer npp;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    extern /* Subroutine */ int dspr_(char *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *), dscal_(integer *, 
	    doublereal *, doublereal *, integer *);
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern /* Subroutine */ int dtpmv_(char *, char *, char *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlansp_(char *, char *, 
	    integer *, doublereal *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DPPT01 reconstructs a symmetric positive definite packed matrix A */
/*  from its L*L' or U'*U factorization and computes the residual */
/*     norm( L*L' - A ) / ( N * norm(A) * EPS ) or */
/*     norm( U'*U - A ) / ( N * norm(A) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The original symmetric matrix A, stored as a packed */
/*          triangular matrix. */

/*  AFAC    (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the factor L or U from the L*L' or U'*U */
/*          factorization of A, stored as a packed triangular matrix. */
/*          Overwritten with the reconstructed matrix, and then with the */
/*          difference L*L' - A (or U'*U - A). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          If UPLO = 'L', norm(L*L' - A) / ( N * norm(A) * EPS ) */
/*          If UPLO = 'U', norm(U'*U - A) / ( N * norm(A) * EPS ) */

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

/*     Quick exit if N = 0 */

    /* Parameter adjustments */
    --rwork;
    --afac;
    --a;

    /* Function Body */
    if (*n <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = dlamch_("Epsilon");
    anorm = dlansp_("1", uplo, n, &a[1], &rwork[1]);
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Compute the product U'*U, overwriting U. */

    if (lsame_(uplo, "U")) {
	kc = *n * (*n - 1) / 2 + 1;
	for (k = *n; k >= 1; --k) {

/*           Compute the (K,K) element of the result. */

	    t = ddot_(&k, &afac[kc], &c__1, &afac[kc], &c__1);
	    afac[kc + k - 1] = t;

/*           Compute the rest of column K. */

	    if (k > 1) {
		i__1 = k - 1;
		dtpmv_("Upper", "Transpose", "Non-unit", &i__1, &afac[1], &
			afac[kc], &c__1);
		kc -= k - 1;
	    }
/* L10: */
	}

/*     Compute the product L*L', overwriting L. */

    } else {
	kc = *n * (*n + 1) / 2;
	for (k = *n; k >= 1; --k) {

/*           Add a multiple of column K of the factor L to each of */
/*           columns K+1 through N. */

	    if (k < *n) {
		i__1 = *n - k;
		dspr_("Lower", &i__1, &c_b14, &afac[kc + 1], &c__1, &afac[kc 
			+ *n - k + 1]);
	    }

/*           Scale column K by the diagonal element. */

	    t = afac[kc];
	    i__1 = *n - k + 1;
	    dscal_(&i__1, &t, &afac[kc], &c__1);

	    kc -= *n - k + 2;
/* L20: */
	}
    }

/*     Compute the difference  L*L' - A (or U'*U - A). */

    npp = *n * (*n + 1) / 2;
    i__1 = npp;
    for (i__ = 1; i__ <= i__1; ++i__) {
	afac[i__] -= a[i__];
/* L30: */
    }

/*     Compute norm( L*U - A ) / ( N * norm(A) * EPS ) */

    *resid = dlansp_("1", uplo, n, &afac[1], &rwork[1]);

    *resid = *resid / (doublereal) (*n) / anorm / eps;

    return 0;

/*     End of DPPT01 */

} /* dppt01_ */
