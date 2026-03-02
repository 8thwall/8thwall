#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static real c_b22 = 1.f;
static real c_b23 = 0.f;

/* Subroutine */ int ssbt21_(char *uplo, integer *n, integer *ka, integer *ks, 
	 real *a, integer *lda, real *d__, real *e, real *u, integer *ldu, 
	real *work, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, u_dim1, u_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2;

    /* Local variables */
    integer j, jc, jr, lw, ika;
    real ulp, unfl;
    extern /* Subroutine */ int sspr_(char *, integer *, real *, real *, 
	    integer *, real *), sspr2_(char *, integer *, real *, 
	    real *, integer *, real *, integer *, real *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm;
    char cuplo[1];
    logical lower;
    real wnorm;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *), slansb_(char *, 
	    char *, integer *, integer *, real *, integer *, real *), slansp_(char *, char *, integer *, real *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSBT21  generally checks a decomposition of the form */

/*          A = U S U' */

/*  where ' means transpose, A is symmetric banded, U is */
/*  orthogonal, and S is diagonal (if KS=0) or symmetric */
/*  tridiagonal (if KS=1). */

/*  Specifically: */

/*          RESULT(1) = | A - U S U' | / ( |A| n ulp ) *and* */
/*          RESULT(2) = | I - UU' | / ( n ulp ) */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER */
/*          If UPLO='U', the upper triangle of A and V will be used and */
/*          the (strictly) lower triangle will not be referenced. */
/*          If UPLO='L', the lower triangle of A and V will be used and */
/*          the (strictly) upper triangle will not be referenced. */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SSBT21 does nothing. */
/*          It must be at least zero. */

/*  KA      (input) INTEGER */
/*          The bandwidth of the matrix A.  It must be at least zero.  If */
/*          it is larger than N-1, then max( 0, N-1 ) will be used. */

/*  KS      (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and E is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  A       (input) REAL array, dimension (LDA, N) */
/*          The original (unfactored) matrix.  It is assumed to be */
/*          symmetric, and only the upper (UPLO='U') or only the lower */
/*          (UPLO='L') will be referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least min( KA, N-1 ). */

/*  D       (input) REAL array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix S. */

/*  E       (input) REAL array, dimension (N-1) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix S. */
/*          E(1) is the (1,2) and (2,1) element, E(2) is the (2,3) and */
/*          (3,2) element, etc. */
/*          Not referenced if KS=0. */

/*  U       (input) REAL array, dimension (LDU, N) */
/*          The orthogonal matrix in the decomposition, expressed as a */
/*          dense matrix (i.e., not as a product of Householder */
/*          transformations, Givens transformations, etc.) */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  WORK    (workspace) REAL array, dimension (N**2+N) */

/*  RESULT  (output) REAL array, dimension (2) */
/*          The values computed by the two tests described above.  The */
/*          values are currently limited to 1/ulp, to avoid overflow. */

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

/*     Constants */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --work;
    --result;

    /* Function Body */
    result[1] = 0.f;
    result[2] = 0.f;
    if (*n <= 0) {
	return 0;
    }

/* Computing MAX */
/* Computing MIN */
    i__3 = *n - 1;
    i__1 = 0, i__2 = min(i__3,*ka);
    ika = max(i__1,i__2);
    lw = *n * (*n + 1) / 2;

    if (lsame_(uplo, "U")) {
	lower = FALSE_;
	*(unsigned char *)cuplo = 'U';
    } else {
	lower = TRUE_;
	*(unsigned char *)cuplo = 'L';
    }

    unfl = slamch_("Safe minimum");
    ulp = slamch_("Epsilon") * slamch_("Base");

/*     Some Error Checks */

/*     Do Test 1 */

/*     Norm of A: */

/* Computing MAX */
    r__1 = slansb_("1", cuplo, n, &ika, &a[a_offset], lda, &work[1]);
    anorm = dmax(r__1,unfl);

/*     Compute error matrix:    Error = A - U S U' */

/*     Copy A from SB to SP storage format. */

    j = 0;
    i__1 = *n;
    for (jc = 1; jc <= i__1; ++jc) {
	if (lower) {
/* Computing MIN */
	    i__3 = ika + 1, i__4 = *n + 1 - jc;
	    i__2 = min(i__3,i__4);
	    for (jr = 1; jr <= i__2; ++jr) {
		++j;
		work[j] = a[jr + jc * a_dim1];
/* L10: */
	    }
	    i__2 = *n + 1 - jc;
	    for (jr = ika + 2; jr <= i__2; ++jr) {
		++j;
		work[j] = 0.f;
/* L20: */
	    }
	} else {
	    i__2 = jc;
	    for (jr = ika + 2; jr <= i__2; ++jr) {
		++j;
		work[j] = 0.f;
/* L30: */
	    }
/* Computing MIN */
	    i__2 = ika, i__3 = jc - 1;
	    for (jr = min(i__2,i__3); jr >= 0; --jr) {
		++j;
		work[j] = a[ika + 1 - jr + jc * a_dim1];
/* L40: */
	    }
	}
/* L50: */
    }

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	r__1 = -d__[j];
	sspr_(cuplo, n, &r__1, &u[j * u_dim1 + 1], &c__1, &work[1])
		;
/* L60: */
    }

    if (*n > 1 && *ks == 1) {
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    r__1 = -e[j];
	    sspr2_(cuplo, n, &r__1, &u[j * u_dim1 + 1], &c__1, &u[(j + 1) * 
		    u_dim1 + 1], &c__1, &work[1]);
/* L70: */
	}
    }
    wnorm = slansp_("1", cuplo, n, &work[1], &work[lw + 1]);

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*n * ulp);
    } else {
	if (anorm < 1.f) {
/* Computing MIN */
	    r__1 = wnorm, r__2 = *n * anorm;
	    result[1] = dmin(r__1,r__2) / anorm / (*n * ulp);
	} else {
/* Computing MIN */
	    r__1 = wnorm / anorm, r__2 = (real) (*n);
	    result[1] = dmin(r__1,r__2) / (*n * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  UU' - I */

    sgemm_("N", "C", n, n, n, &c_b22, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b23, &work[1], n);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	work[(*n + 1) * (j - 1) + 1] += -1.f;
/* L80: */
    }

/* Computing MIN */
/* Computing 2nd power */
    i__1 = *n;
    r__1 = slange_("1", n, n, &work[1], n, &work[i__1 * i__1 + 1]),
	     r__2 = (real) (*n);
    result[2] = dmin(r__1,r__2) / (*n * ulp);

    return 0;

/*     End of SSBT21 */

} /* ssbt21_ */
