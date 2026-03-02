#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static real c_b14 = 1.f;

/* Subroutine */ int spot01_(char *uplo, integer *n, real *a, integer *lda, 
	real *afac, integer *ldafac, real *rwork, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, afac_dim1, afac_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k;
    real t, eps;
    extern doublereal sdot_(integer *, real *, integer *, real *, integer *);
    extern /* Subroutine */ int ssyr_(char *, integer *, real *, real *, 
	    integer *, real *, integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    real anorm;
    extern /* Subroutine */ int strmv_(char *, char *, char *, integer *, 
	    real *, integer *, real *, integer *);
    extern doublereal slamch_(char *), slansy_(char *, char *, 
	    integer *, real *, integer *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SPOT01 reconstructs a symmetric positive definite matrix  A  from */
/*  its L*L' or U'*U factorization and computes the residual */
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

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The original symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N) */

/*  AFAC    (input/output) REAL array, dimension (LDAFAC,N) */
/*          On entry, the factor L or U from the L*L' or U'*U */
/*          factorization of A. */
/*          Overwritten with the reconstructed matrix, and then with the */
/*          difference L*L' - A (or U'*U - A). */

/*  LDAFAC  (input) INTEGER */
/*          The leading dimension of the array AFAC.  LDAFAC >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESID   (output) REAL */
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

/*     Quick exit if N = 0. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    afac_dim1 = *ldafac;
    afac_offset = 1 + afac_dim1;
    afac -= afac_offset;
    --rwork;

    /* Function Body */
    if (*n <= 0) {
	*resid = 0.f;
	return 0;
    }

/*     Exit with RESID = 1/EPS if ANORM = 0. */

    eps = slamch_("Epsilon");
    anorm = slansy_("1", uplo, n, &a[a_offset], lda, &rwork[1]);
    if (anorm <= 0.f) {
	*resid = 1.f / eps;
	return 0;
    }

/*     Compute the product U'*U, overwriting U. */

    if (lsame_(uplo, "U")) {
	for (k = *n; k >= 1; --k) {

/*           Compute the (K,K) element of the result. */

	    t = sdot_(&k, &afac[k * afac_dim1 + 1], &c__1, &afac[k * 
		    afac_dim1 + 1], &c__1);
	    afac[k + k * afac_dim1] = t;

/*           Compute the rest of column K. */

	    i__1 = k - 1;
	    strmv_("Upper", "Transpose", "Non-unit", &i__1, &afac[afac_offset]
, ldafac, &afac[k * afac_dim1 + 1], &c__1);

/* L10: */
	}

/*     Compute the product L*L', overwriting L. */

    } else {
	for (k = *n; k >= 1; --k) {

/*           Add a multiple of column K of the factor L to each of */
/*           columns K+1 through N. */

	    if (k + 1 <= *n) {
		i__1 = *n - k;
		ssyr_("Lower", &i__1, &c_b14, &afac[k + 1 + k * afac_dim1], &
			c__1, &afac[k + 1 + (k + 1) * afac_dim1], ldafac);
	    }

/*           Scale column K by the diagonal element. */

	    t = afac[k + k * afac_dim1];
	    i__1 = *n - k + 1;
	    sscal_(&i__1, &t, &afac[k + k * afac_dim1], &c__1);

/* L20: */
	}
    }

/*     Compute the difference  L*L' - A (or U'*U - A). */

    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		afac[i__ + j * afac_dim1] -= a[i__ + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j; i__ <= i__2; ++i__) {
		afac[i__ + j * afac_dim1] -= a[i__ + j * a_dim1];
/* L50: */
	    }
/* L60: */
	}
    }

/*     Compute norm( L*U - A ) / ( N * norm(A) * EPS ) */

    *resid = slansy_("1", uplo, n, &afac[afac_offset], ldafac, &rwork[1]);

    *resid = *resid / (real) (*n) / anorm / eps;

    return 0;

/*     End of SPOT01 */

} /* spot01_ */
