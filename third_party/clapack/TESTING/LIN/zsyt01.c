#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};

/* Subroutine */ int zsyt01_(char *uplo, integer *n, doublecomplex *a, 
	integer *lda, doublecomplex *afac, integer *ldafac, integer *ipiv, 
	doublecomplex *c__, integer *ldc, doublereal *rwork, doublereal *
	resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, afac_dim1, afac_offset, c_dim1, c_offset, i__1, 
	    i__2, i__3, i__4, i__5;
    doublecomplex z__1;

    /* Local variables */
    integer i__, j;
    doublereal eps;
    integer info;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int zlavsy_(char *, char *, char *, integer *, 
	    integer *, doublecomplex *, integer *, integer *, doublecomplex *, 
	     integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZSYT01 reconstructs a complex symmetric indefinite matrix A from its */
/*  block L*D*L' or U*D*U' factorization and computes the residual */
/*     norm( C - A ) / ( N * norm(A) * EPS ), */
/*  where C is the reconstructed matrix, EPS is the machine epsilon, */
/*  L' is the transpose of L, and U' is the transpose of U. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          complex symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The original complex symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  AFAC    (input) COMPLEX*16 array, dimension (LDAFAC,N) */
/*          The factored form of the matrix A.  AFAC contains the block */
/*          diagonal matrix D and the multipliers used to obtain the */
/*          factor L or U from the block L*D*L' or U*D*U' factorization */
/*          as computed by ZSYTRF. */

/*  LDAFAC  (input) INTEGER */
/*          The leading dimension of the array AFAC.  LDAFAC >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from ZSYTRF. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    afac_dim1 = *ldafac;
    afac_offset = 1 + afac_dim1;
    afac -= afac_offset;
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
    anorm = zlansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);

/*     Initialize C to the identity matrix. */

    zlaset_("Full", n, n, &c_b1, &c_b2, &c__[c_offset], ldc);

/*     Call ZLAVSY to form the product D * U' (or D * L' ). */

    zlavsy_(uplo, "Transpose", "Non-unit", n, n, &afac[afac_offset], ldafac, &
	    ipiv[1], &c__[c_offset], ldc, &info);

/*     Call ZLAVSY again to multiply by U (or L ). */

    zlavsy_(uplo, "No transpose", "Unit", n, n, &afac[afac_offset], ldafac, &
	    ipiv[1], &c__[c_offset], ldc, &info);

/*     Compute the difference  C - A . */

    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = i__ + j * a_dim1;
		z__1.r = c__[i__4].r - a[i__5].r, z__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = z__1.r, c__[i__3].i = z__1.i;
/* L10: */
	    }
/* L20: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = i__ + j * a_dim1;
		z__1.r = c__[i__4].r - a[i__5].r, z__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = z__1.r, c__[i__3].i = z__1.i;
/* L30: */
	    }
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

/*     End of ZSYT01 */

} /* zsyt01_ */
