#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b11 = 1.;
static integer c_n1 = -1;

/* Subroutine */ int dget01_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *afac, integer *ldafac, integer *ipiv, doublereal *
	rwork, doublereal *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, afac_dim1, afac_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k;
    doublereal t, eps;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *), dgemv_(char *, integer *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *);
    doublereal anorm;
    extern /* Subroutine */ int dtrmv_(char *, char *, char *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlaswp_(integer *, doublereal *, integer *, 
	    integer *, integer *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET01 reconstructs a matrix A from its L*U factorization and */
/*  computes the residual */
/*     norm(L*U - A) / ( N * norm(A) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========== */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The original M x N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  AFAC    (input/output) DOUBLE PRECISION array, dimension (LDAFAC,N) */
/*          The factored form of the matrix A.  AFAC contains the factors */
/*          L and U from the L*U factorization as computed by DGETRF. */
/*          Overwritten with the reconstructed matrix, and then with the */
/*          difference L*U - A. */

/*  LDAFAC  (input) INTEGER */
/*          The leading dimension of the array AFAC.  LDAFAC >= max(1,M). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from DGETRF. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESID   (output) DOUBLE PRECISION */
/*          norm(L*U - A) / ( N * norm(A) * EPS ) */

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

/*     Quick exit if M = 0 or N = 0. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    afac_dim1 = *ldafac;
    afac_offset = 1 + afac_dim1;
    afac -= afac_offset;
    --ipiv;
    --rwork;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	*resid = 0.;
	return 0;
    }

/*     Determine EPS and the norm of A. */

    eps = dlamch_("Epsilon");
    anorm = dlange_("1", m, n, &a[a_offset], lda, &rwork[1]);

/*     Compute the product L*U and overwrite AFAC with the result. */
/*     A column at a time of the product is obtained, starting with */
/*     column N. */

    for (k = *n; k >= 1; --k) {
	if (k > *m) {
	    dtrmv_("Lower", "No transpose", "Unit", m, &afac[afac_offset], 
		    ldafac, &afac[k * afac_dim1 + 1], &c__1);
	} else {

/*           Compute elements (K+1:M,K) */

	    t = afac[k + k * afac_dim1];
	    if (k + 1 <= *m) {
		i__1 = *m - k;
		dscal_(&i__1, &t, &afac[k + 1 + k * afac_dim1], &c__1);
		i__1 = *m - k;
		i__2 = k - 1;
		dgemv_("No transpose", &i__1, &i__2, &c_b11, &afac[k + 1 + 
			afac_dim1], ldafac, &afac[k * afac_dim1 + 1], &c__1, &
			c_b11, &afac[k + 1 + k * afac_dim1], &c__1);
	    }

/*           Compute the (K,K) element */

	    i__1 = k - 1;
	    afac[k + k * afac_dim1] = t + ddot_(&i__1, &afac[k + afac_dim1], 
		    ldafac, &afac[k * afac_dim1 + 1], &c__1);

/*           Compute elements (1:K-1,K) */

	    i__1 = k - 1;
	    dtrmv_("Lower", "No transpose", "Unit", &i__1, &afac[afac_offset], 
		     ldafac, &afac[k * afac_dim1 + 1], &c__1);
	}
/* L10: */
    }
    i__1 = min(*m,*n);
    dlaswp_(n, &afac[afac_offset], ldafac, &c__1, &i__1, &ipiv[1], &c_n1);

/*     Compute the difference  L*U - A  and store in AFAC. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    afac[i__ + j * afac_dim1] -= a[i__ + j * a_dim1];
/* L20: */
	}
/* L30: */
    }

/*     Compute norm( L*U - A ) / ( N * norm(A) * EPS ) */

    *resid = dlange_("1", m, n, &afac[afac_offset], ldafac, &rwork[1]);

    if (anorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	*resid = *resid / (doublereal) (*n) / anorm / eps;
    }

    return 0;

/*     End of DGET01 */

} /* dget01_ */
