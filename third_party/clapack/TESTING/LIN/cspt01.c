#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};

/* Subroutine */ int cspt01_(char *uplo, integer *n, complex *a, complex *
	afac, integer *ipiv, complex *c__, integer *ldc, real *rwork, real *
	resid)
{
    /* System generated locals */
    integer c_dim1, c_offset, i__1, i__2, i__3, i__4, i__5;
    complex q__1;

    /* Local variables */
    integer i__, j, jc;
    real eps;
    integer info;
    extern logical lsame_(char *, char *);
    real anorm;
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);
    extern doublereal clansp_(char *, char *, integer *, complex *, real *);
    extern /* Subroutine */ int clavsp_(char *, char *, char *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, integer *);
    extern doublereal clansy_(char *, char *, integer *, complex *, integer *, 
	     real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CSPT01 reconstructs a symmetric indefinite packed matrix A from its */
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

/*  A       (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The original symmetric matrix A, stored as a packed */
/*          triangular matrix. */

/*  AFAC    (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The factored form of the matrix A, stored as a packed */
/*          triangular matrix.  AFAC contains the block diagonal matrix D */
/*          and the multipliers used to obtain the factor L or U from the */
/*          L*D*L' or U*D*U' factorization as computed by CSPTRF. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from CSPTRF. */

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
    --a;
    --afac;
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
    anorm = clansp_("1", uplo, n, &a[1], &rwork[1]);

/*     Initialize C to the identity matrix. */

    claset_("Full", n, n, &c_b1, &c_b2, &c__[c_offset], ldc);

/*     Call CLAVSP to form the product D * U' (or D * L' ). */

    clavsp_(uplo, "Transpose", "Non-unit", n, n, &afac[1], &ipiv[1], &c__[
	    c_offset], ldc, &info);

/*     Call CLAVSP again to multiply by U ( or L ). */

    clavsp_(uplo, "No transpose", "Unit", n, n, &afac[1], &ipiv[1], &c__[
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
		q__1.r = c__[i__4].r - a[i__5].r, q__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
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
		q__1.r = c__[i__4].r - a[i__5].r, q__1.i = c__[i__4].i - a[
			i__5].i;
		c__[i__3].r = q__1.r, c__[i__3].i = q__1.i;
/* L30: */
	    }
	    jc = jc + *n - j + 1;
/* L40: */
	}
    }

/*     Compute norm( C - A ) / ( N * norm(A) * EPS ) */

    *resid = clansy_("1", uplo, n, &c__[c_offset], ldc, &rwork[1]);

    if (anorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	*resid = *resid / (real) (*n) / anorm / eps;
    }

    return 0;

/*     End of CSPT01 */

} /* cspt01_ */
