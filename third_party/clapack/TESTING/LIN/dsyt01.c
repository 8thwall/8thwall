#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b5 = 0.;
static doublereal c_b6 = 1.;

/* Subroutine */ int dsyt01_(char *uplo, integer *n, doublereal *a, integer *
	lda, doublereal *afac, integer *ldafac, integer *ipiv, doublereal *
	c__, integer *ldc, doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, afac_dim1, afac_offset, c_dim1, c_offset, i__1, 
	    i__2;

    /* Local variables */
    integer i__, j;
    doublereal eps;
    integer info;
    extern logical lsame_(char *, char *);
    doublereal anorm;
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *);
    extern doublereal dlansy_(char *, char *, integer *, doublereal *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int dlavsy_(char *, char *, char *, integer *, 
	    integer *, doublereal *, integer *, integer *, doublereal *, 
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

/*  DSYT01 reconstructs a symmetric indefinite matrix A from its */
/*  block L*D*L' or U*D*U' factorization and computes the residual */
/*     norm( C - A ) / ( N * norm(A) * EPS ), */
/*  where C is the reconstructed matrix and EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The original symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  AFAC    (input) DOUBLE PRECISION array, dimension (LDAFAC,N) */
/*          The factored form of the matrix A.  AFAC contains the block */
/*          diagonal matrix D and the multipliers used to obtain the */
/*          factor L or U from the block L*D*L' or U*D*U' factorization */
/*          as computed by DSYTRF. */

/*  LDAFAC  (input) INTEGER */
/*          The leading dimension of the array AFAC.  LDAFAC >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from DSYTRF. */

/*  C       (workspace) DOUBLE PRECISION array, dimension (LDC,N) */

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
    anorm = dlansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);

/*     Initialize C to the identity matrix. */

    dlaset_("Full", n, n, &c_b5, &c_b6, &c__[c_offset], ldc);

/*     Call DLAVSY to form the product D * U' (or D * L' ). */

    dlavsy_(uplo, "Transpose", "Non-unit", n, n, &afac[afac_offset], ldafac, &
	    ipiv[1], &c__[c_offset], ldc, &info);

/*     Call DLAVSY again to multiply by U (or L ). */

    dlavsy_(uplo, "No transpose", "Unit", n, n, &afac[afac_offset], ldafac, &
	    ipiv[1], &c__[c_offset], ldc, &info);

/*     Compute the difference  C - A . */

    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		c__[i__ + j * c_dim1] -= a[i__ + j * a_dim1];
/* L10: */
	    }
/* L20: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j; i__ <= i__2; ++i__) {
		c__[i__ + j * c_dim1] -= a[i__ + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}
    }

/*     Compute norm( C - A ) / ( N * norm(A) * EPS ) */

    *resid = dlansy_("1", uplo, n, &c__[c_offset], ldc, &rwork[1]);

    if (anorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	*resid = *resid / (doublereal) (*n) / anorm / eps;
    }

    return 0;

/*     End of DSYT01 */

} /* dsyt01_ */
