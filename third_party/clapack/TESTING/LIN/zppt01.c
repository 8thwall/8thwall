#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b19 = 1.;

/* Subroutine */ int zppt01_(char *uplo, integer *n, doublecomplex *a, 
	doublecomplex *afac, doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    doublereal d__1;
    doublecomplex z__1;

    /* Builtin functions */
    double d_imag(doublecomplex *);

    /* Local variables */
    integer i__, k, kc;
    doublecomplex tc;
    doublereal tr, eps;
    extern /* Subroutine */ int zhpr_(char *, integer *, doublereal *, 
	    doublecomplex *, integer *, doublecomplex *);
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern /* Subroutine */ int zscal_(integer *, doublecomplex *, 
	    doublecomplex *, integer *);
    extern /* Double Complex */ VOID zdotc_(doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    extern /* Subroutine */ int ztpmv_(char *, char *, char *, integer *, 
	    doublecomplex *, doublecomplex *, integer *);
    extern doublereal dlamch_(char *), zlanhp_(char *, char *, 
	    integer *, doublecomplex *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZPPT01 reconstructs a Hermitian positive definite packed matrix A */
/*  from its L*L' or U'*U factorization and computes the residual */
/*     norm( L*L' - A ) / ( N * norm(A) * EPS ) or */
/*     norm( U'*U - A ) / ( N * norm(A) * EPS ), */
/*  where EPS is the machine epsilon, L' is the conjugate transpose of */
/*  L, and U' is the conjugate transpose of U. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          Hermitian matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The original Hermitian matrix A, stored as a packed */
/*          triangular matrix. */

/*  AFAC    (input/output) COMPLEX*16 array, dimension (N*(N+1)/2) */
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
    anorm = zlanhp_("1", uplo, n, &a[1], &rwork[1]);
    if (anorm <= 0.) {
	*resid = 1. / eps;
	return 0;
    }

/*     Check the imaginary parts of the diagonal elements and return with */
/*     an error code if any are nonzero. */

    kc = 1;
    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (k = 1; k <= i__1; ++k) {
	    if (d_imag(&afac[kc]) != 0.) {
		*resid = 1. / eps;
		return 0;
	    }
	    kc = kc + k + 1;
/* L10: */
	}
    } else {
	i__1 = *n;
	for (k = 1; k <= i__1; ++k) {
	    if (d_imag(&afac[kc]) != 0.) {
		*resid = 1. / eps;
		return 0;
	    }
	    kc = kc + *n - k + 1;
/* L20: */
	}
    }

/*     Compute the product U'*U, overwriting U. */

    if (lsame_(uplo, "U")) {
	kc = *n * (*n - 1) / 2 + 1;
	for (k = *n; k >= 1; --k) {

/*           Compute the (K,K) element of the result. */

	    zdotc_(&z__1, &k, &afac[kc], &c__1, &afac[kc], &c__1);
	    tr = z__1.r;
	    i__1 = kc + k - 1;
	    afac[i__1].r = tr, afac[i__1].i = 0.;

/*           Compute the rest of column K. */

	    if (k > 1) {
		i__1 = k - 1;
		ztpmv_("Upper", "Conjugate", "Non-unit", &i__1, &afac[1], &
			afac[kc], &c__1);
		kc -= k - 1;
	    }
/* L30: */
	}

/*        Compute the difference  L*L' - A */

	kc = 1;
	i__1 = *n;
	for (k = 1; k <= i__1; ++k) {
	    i__2 = k - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = kc + i__ - 1;
		i__4 = kc + i__ - 1;
		i__5 = kc + i__ - 1;
		z__1.r = afac[i__4].r - a[i__5].r, z__1.i = afac[i__4].i - a[
			i__5].i;
		afac[i__3].r = z__1.r, afac[i__3].i = z__1.i;
/* L40: */
	    }
	    i__2 = kc + k - 1;
	    i__3 = kc + k - 1;
	    i__4 = kc + k - 1;
	    d__1 = a[i__4].r;
	    z__1.r = afac[i__3].r - d__1, z__1.i = afac[i__3].i;
	    afac[i__2].r = z__1.r, afac[i__2].i = z__1.i;
	    kc += k;
/* L50: */
	}

/*     Compute the product L*L', overwriting L. */

    } else {
	kc = *n * (*n + 1) / 2;
	for (k = *n; k >= 1; --k) {

/*           Add a multiple of column K of the factor L to each of */
/*           columns K+1 through N. */

	    if (k < *n) {
		i__1 = *n - k;
		zhpr_("Lower", &i__1, &c_b19, &afac[kc + 1], &c__1, &afac[kc 
			+ *n - k + 1]);
	    }

/*           Scale column K by the diagonal element. */

	    i__1 = kc;
	    tc.r = afac[i__1].r, tc.i = afac[i__1].i;
	    i__1 = *n - k + 1;
	    zscal_(&i__1, &tc, &afac[kc], &c__1);

	    kc -= *n - k + 2;
/* L60: */
	}

/*        Compute the difference  U'*U - A */

	kc = 1;
	i__1 = *n;
	for (k = 1; k <= i__1; ++k) {
	    i__2 = kc;
	    i__3 = kc;
	    i__4 = kc;
	    d__1 = a[i__4].r;
	    z__1.r = afac[i__3].r - d__1, z__1.i = afac[i__3].i;
	    afac[i__2].r = z__1.r, afac[i__2].i = z__1.i;
	    i__2 = *n;
	    for (i__ = k + 1; i__ <= i__2; ++i__) {
		i__3 = kc + i__ - k;
		i__4 = kc + i__ - k;
		i__5 = kc + i__ - k;
		z__1.r = afac[i__4].r - a[i__5].r, z__1.i = afac[i__4].i - a[
			i__5].i;
		afac[i__3].r = z__1.r, afac[i__3].i = z__1.i;
/* L70: */
	    }
	    kc = kc + *n - k + 1;
/* L80: */
	}
    }

/*     Compute norm( L*U - A ) / ( N * norm(A) * EPS ) */

    *resid = zlanhp_("1", uplo, n, &afac[1], &rwork[1]);

    *resid = *resid / (doublereal) (*n) / anorm / eps;

    return 0;

/*     End of ZPPT01 */

} /* zppt01_ */
