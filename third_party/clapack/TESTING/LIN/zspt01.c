#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};

/* Subroutine */ int zspt01_(char *uplo, integer *n, doublecomplex *a, 
	doublecomplex *afac, integer *ipiv, doublecomplex *c__, integer *ldc, 
	doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer c_dim1, c_offset, i__1, i__2, i__3, i__4, i__5;
    doublecomplex z__1;

    /* Local variables */
    integer i__, j, jc;
    doublereal eps;
    integer info;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);
    extern doublereal zlansp_(char *, char *, integer *, doublecomplex *, 
	    doublereal *);
    extern /* Subroutine */ int zlavsp_(char *, char *, char *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     integer *);
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZSPT01 reconstructs a symmetric indefinite packed matrix A from its */
/*  diagonal pivoting factorization A = U*D*U' or A = L*D*L' and computes */
/*  the residual */
/*     norm( C - A ) / ( N * norm(A) * EPS ), */
/*  where C is the reconstructed matrix and EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          Hermitian matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The original symmetric matrix A, stored as a packed */
/*          triangular matrix. */

/*  AFAC    (input) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The factored form of the matrix A, stored as a packed */
/*          triangular matrix.  AFAC contains the block diagonal matrix D */
/*          and the multipliers used to obtain the factor L or U from the */
/*          L*D*L' or U*D*U' factorization as computed by ZSPTRF. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from ZSPTRF. */

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
    anorm = zlansp_("1", uplo, n, &a[1], &rwork[1]);

/*     Initialize C to the identity matrix. */

    zlaset_("Full", n, n, &c_b1, &c_b2, &c__[c_offset], ldc);

/*     Call ZLAVSP to form the product D * U' (or D * L' ). */

    zlavsp_(uplo, "Transpose", "Non-unit", n, n, &afac[1], &ipiv[1], &c__[
	    c_offset], ldc, &info);

/*     Call ZLAVSP again to multiply by U ( or L ). */

    zlavsp_(uplo, "No transpose", "Unit", n, n, &afac[1], &ipiv[1], &c__[
	    c_offset], ldc, &info);

/*     Compute the difference  C - A . */

    if (lsame_(uplo, "U")) {
	jc = 0;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = jc + i__;
		z__1.r = c__[i__4].r - a[i__5].r, z__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = z__1.r, c__[i__3].i = z__1.i;
/* L10: */
	    }
	    jc += j;
/* L20: */
	}
    } else {
	jc = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = jc + i__ - j;
		z__1.r = c__[i__4].r - a[i__5].r, z__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = z__1.r, c__[i__3].i = z__1.i;
/* L30: */
	    }
	    jc = jc + *n - j + 1;
/* L40: */
	}
    }

/*     Compute norm( C - A ) / ( N * norm(A) * EPS ) */

    *resid = zlansy_("1", uplo, n, &c__[c_offset], ldc, &rwork[1]);

    if (anorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	*resid = *resid / (doublereal) (*n) / anorm / eps;
    }

    return 0;

/*     End of ZSPT01 */

} /* zspt01_ */
