#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};

/* Subroutine */ int zhpt01_(char *uplo, integer *n, doublecomplex *a, 
	doublecomplex *afac, integer *ipiv, doublecomplex *c__, integer *ldc, 
	doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer c_dim1, c_offset, i__1, i__2, i__3, i__4, i__5;
    doublereal d__1;
    doublecomplex z__1;

    /* Builtin functions */
    double d_imag(doublecomplex *);

    /* Local variables */
    integer i__, j, jc;
    doublereal eps;
    integer info;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern doublereal dlamch_(char *), zlanhe_(char *, char *, 
	    integer *, doublecomplex *, integer *, doublereal *), zlanhp_(char *, char *, integer *, doublecomplex *, 
	    doublereal *);
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *), zlavhp_(char *, char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZHPT01 reconstructs a Hermitian indefinite packed matrix A from its */
/*  block L*D*L' or U*D*U' factorization and computes the residual */
/*     norm( C - A ) / ( N * norm(A) * EPS ), */
/*  where C is the reconstructed matrix, EPS is the machine epsilon, */
/*  L' is the conjugate transpose of L, and U' is the conjugate transpose */
/*  of U. */

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

/*  AFAC    (input) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The factored form of the matrix A, stored as a packed */
/*          triangular matrix.  AFAC contains the block diagonal matrix D */
/*          and the multipliers used to obtain the factor L or U from the */
/*          block L*D*L' or U*D*U' factorization as computed by ZHPTRF. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from ZHPTRF. */

/*  C       (workspace) COMPLEX*16 array, dimension (LDC,N) */

/*  LDC     (integer) INTEGER */
/*          The leading dimension of the array C.  LDC >= max(1,N). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          If UPLO = 'L', norm(L*D*L' - A) / ( N * norm(A) * EPS ) */
/*          If UPLO = 'U', norm(U*D*U' - A) / ( N * norm(A) * EPS ) */

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

/*     Quick exit if N = 0. */

    /* Parameter adjustments */
    --a;
    --afac;
    --ipiv;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --rwork;

    /* Function Body */
    if (*n <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Determine EPS and the norm of A. */

    eps = dlamch_("Epsilon");
    anorm = zlanhp_("1", uplo, n, &a[1], &rwork[1]);

/*     Check the imaginary parts of the diagonal elements and return with */
/*     an error code if any are nonzero. */

    jc = 1;
    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (d_imag(&afac[jc]) != 0.) {
		*resid = 1. / eps;
		return 0;
	    }
	    jc = jc + j + 1;
/* L10: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (d_imag(&afac[jc]) != 0.) {
		*resid = 1. / eps;
		return 0;
	    }
	    jc = jc + *n - j + 1;
/* L20: */
	}
    }

/*     Initialize C to the identity matrix. */

    zlaset_("Full", n, n, &c_b1, &c_b2, &c__[c_offset], ldc);

/*     Call ZLAVHP to form the product D * U' (or D * L' ). */

    zlavhp_(uplo, "Conjugate", "Non-unit", n, n, &afac[1], &ipiv[1], &c__[
	    c_offset], ldc, &info);

/*     Call ZLAVHP again to multiply by U ( or L ). */

    zlavhp_(uplo, "No transpose", "Unit", n, n, &afac[1], &ipiv[1], &c__[
	    c_offset], ldc, &info);

/*     Compute the difference  C - A . */

    if (lsame_(uplo, "U")) {
	jc = 0;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = jc + i__;
		z__1.r = c__[i__4].r - a[i__5].r, z__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = z__1.r, c__[i__3].i = z__1.i;
/* L30: */
	    }
	    i__2 = j + j * c_dim1;
	    i__3 = j + j * c_dim1;
	    i__4 = jc + j;
	    d__1 = a[i__4].r;
	    z__1.r = c__[i__3].r - d__1, z__1.i = c__[i__3].i;
	    c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	    jc += j;
/* L40: */
	}
    } else {
	jc = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j + j * c_dim1;
	    i__3 = j + j * c_dim1;
	    i__4 = jc;
	    d__1 = a[i__4].r;
	    z__1.r = c__[i__3].r - d__1, z__1.i = c__[i__3].i;
	    c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	    i__2 = *n;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = jc + i__ - j;
		z__1.r = c__[i__4].r - a[i__5].r, z__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = z__1.r, c__[i__3].i = z__1.i;
/* L50: */
	    }
	    jc = jc + *n - j + 1;
/* L60: */
	}
    }

/*     Compute norm( C - A ) / ( N * norm(A) * EPS ) */

    *resid = zlanhe_("1", uplo, n, &c__[c_offset], ldc, &rwork[1]);

    if (anorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	*resid = *resid / (doublereal) (*n) / anorm / eps;
    }

    return 0;

/*     End of ZHPT01 */

} /* zhpt01_ */
