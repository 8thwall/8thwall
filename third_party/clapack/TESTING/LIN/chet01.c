#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};

/* Subroutine */ int chet01_(char *uplo, integer *n, complex *a, integer *lda, 
	 complex *afac, integer *ldafac, integer *ipiv, complex *c__, integer 
	*ldc, real *rwork, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, afac_dim1, afac_offset, c_dim1, c_offset, i__1, 
	    i__2, i__3, i__4, i__5;
    real r__1;
    complex q__1;

    /* Builtin functions */
    double r_imag(complex *);

    /* Local variables */
    integer i__, j;
    real eps;
    integer info;
    extern logical lsame_(char *, char *);
    real anorm;
    extern doublereal clanhe_(char *, char *, integer *, complex *, integer *, 
	     real *);
    extern /* Subroutine */ int clavhe_(char *, char *, char *, integer *, 
	    integer *, complex *, integer *, integer *, complex *, integer *, 
	    integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CHET01 reconstructs a Hermitian indefinite matrix A from its */
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

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The original Hermitian matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  AFAC    (input) COMPLEX array, dimension (LDAFAC,N) */
/*          The factored form of the matrix A.  AFAC contains the block */
/*          diagonal matrix D and the multipliers used to obtain the */
/*          factor L or U from the block L*D*L' or U*D*U' factorization */
/*          as computed by CHETRF. */

/*  LDAFAC  (input) INTEGER */
/*          The leading dimension of the array AFAC.  LDAFAC >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from CHETRF. */

/*  C       (workspace) COMPLEX array, dimension (LDC,N) */

/*  LDC     (integer) INTEGER */
/*          The leading dimension of the array C.  LDC >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESID   (output) REAL */
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
	*resid = 0.f;
	return 0;
    }

/*     Determine EPS and the norm of A. */

    eps = slamch_("Epsilon");
    anorm = clanhe_("1", uplo, n, &a[a_offset], lda, &rwork[1]);

/*     Check the imaginary parts of the diagonal elements and return with */
/*     an error code if any are nonzero. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (r_imag(&afac[j + j * afac_dim1]) != 0.f) {
	    *resid = 1.f / eps;
	    return 0;
	}
/* L10: */
    }

/*     Initialize C to the identity matrix. */

    claset_("Full", n, n, &c_b1, &c_b2, &c__[c_offset], ldc);

/*     Call CLAVHE to form the product D * U' (or D * L' ). */

    clavhe_(uplo, "Conjugate", "Non-unit", n, n, &afac[afac_offset], ldafac, &
	    ipiv[1], &c__[c_offset], ldc, &info);

/*     Call CLAVHE again to multiply by U (or L ). */

    clavhe_(uplo, "No transpose", "Unit", n, n, &afac[afac_offset], ldafac, &
	    ipiv[1], &c__[c_offset], ldc, &info);

/*     Compute the difference  C - A . */

    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = i__ + j * a_dim1;
		q__1.r = c__[i__4].r - a[i__5].r, q__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
/* L20: */
	    }
	    i__2 = j + j * c_dim1;
	    i__3 = j + j * c_dim1;
	    i__4 = j + j * a_dim1;
	    r__1 = a[i__4].r;
	    q__1.r = c__[i__3].r - r__1, q__1.i = c__[i__3].i;
	    c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
/* L30: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j + j * c_dim1;
	    i__3 = j + j * c_dim1;
	    i__4 = j + j * a_dim1;
	    r__1 = a[i__4].r;
	    q__1.r = c__[i__3].r - r__1, q__1.i = c__[i__3].i;
	    c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	    i__2 = *n;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * c_dim1;
		i__4 = i__ + j * c_dim1;
		i__5 = i__ + j * a_dim1;
		q__1.r = c__[i__4].r - a[i__5].r, q__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
/* L40: */
	    }
/* L50: */
	}
    }

/*     Compute norm( C - A ) / ( N * norm(A) * EPS ) */

    *resid = clanhe_("1", uplo, n, &c__[c_offset], ldc, &rwork[1]);

    if (anorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	*resid = *resid / (real) (*n) / anorm / eps;
    }

    return 0;

/*     End of CHET01 */

} /* chet01_ */
