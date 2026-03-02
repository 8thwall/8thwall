#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b6 = 1.;
static doublereal c_b7 = 0.;

/* Subroutine */ int dsyt22_(integer *itype, char *uplo, integer *n, integer *
	m, integer *kband, doublereal *a, integer *lda, doublereal *d__, 
	doublereal *e, doublereal *u, integer *ldu, doublereal *v, integer *
	ldv, doublereal *tau, doublereal *work, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, u_dim1, u_offset, v_dim1, v_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j, jj, nn, jj1, jj2;
    doublereal ulp;
    integer nnp1;
    doublereal unfl;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *),
	     dort01_(char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *);
    doublereal anorm;
    extern /* Subroutine */ int dsymm_(char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    doublereal wnorm;
    extern doublereal dlamch_(char *), dlansy_(char *, char *, 
	    integer *, doublereal *, integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*       DSYT22  generally checks a decomposition of the form */

/*               A U = U S */

/*       where A is symmetric, the columns of U are orthonormal, and S */
/*       is diagonal (if KBAND=0) or symmetric tridiagonal (if */
/*       KBAND=1).  If ITYPE=1, then U is represented as a dense matrix, */
/*       otherwise the U is expressed as a product of Householder */
/*       transformations, whose vectors are stored in the array "V" and */
/*       whose scaling constants are in "TAU"; we shall use the letter */
/*       "V" to refer to the product of Householder transformations */
/*       (which should be equal to U). */

/*       Specifically, if ITYPE=1, then: */

/*               RESULT(1) = | U' A U - S | / ( |A| m ulp ) *and* */
/*               RESULT(2) = | I - U'U | / ( m ulp ) */

/*  Arguments */
/*  ========= */

/*  ITYPE   INTEGER */
/*          Specifies the type of tests to be performed. */
/*          1: U expressed as a dense orthogonal matrix: */
/*             RESULT(1) = | A - U S U' | / ( |A| n ulp )   *and* */
/*             RESULT(2) = | I - UU' | / ( n ulp ) */

/*  UPLO    CHARACTER */
/*          If UPLO='U', the upper triangle of A will be used and the */
/*          (strictly) lower triangle will not be referenced.  If */
/*          UPLO='L', the lower triangle of A will be used and the */
/*          (strictly) upper triangle will not be referenced. */
/*          Not modified. */

/*  N       INTEGER */
/*          The size of the matrix.  If it is zero, DSYT22 does nothing. */
/*          It must be at least zero. */
/*          Not modified. */

/*  M       INTEGER */
/*          The number of columns of U.  If it is zero, DSYT22 does */
/*          nothing.  It must be at least zero. */
/*          Not modified. */

/*  KBAND   INTEGER */
/*          The bandwidth of the matrix.  It may only be zero or one. */
/*          If zero, then S is diagonal, and E is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */
/*          Not modified. */

/*  A       DOUBLE PRECISION array, dimension (LDA , N) */
/*          The original (unfactored) matrix.  It is assumed to be */
/*          symmetric, and only the upper (UPLO='U') or only the lower */
/*          (UPLO='L') will be referenced. */
/*          Not modified. */

/*  LDA     INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */
/*          Not modified. */

/*  D       DOUBLE PRECISION array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix. */
/*          Not modified. */

/*  E       DOUBLE PRECISION array, dimension (N) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix. */
/*          E(1) is ignored, E(2) is the (1,2) and (2,1) element, etc. */
/*          Not referenced if KBAND=0. */
/*          Not modified. */

/*  U       DOUBLE PRECISION array, dimension (LDU, N) */
/*          If ITYPE=1 or 3, this contains the orthogonal matrix in */
/*          the decomposition, expressed as a dense matrix.  If ITYPE=2, */
/*          then it is not referenced. */
/*          Not modified. */

/*  LDU     INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */
/*          Not modified. */

/*  V       DOUBLE PRECISION array, dimension (LDV, N) */
/*          If ITYPE=2 or 3, the lower triangle of this array contains */
/*          the Householder vectors used to describe the orthogonal */
/*          matrix in the decomposition.  If ITYPE=1, then it is not */
/*          referenced. */
/*          Not modified. */

/*  LDV     INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */
/*          Not modified. */

/*  TAU     DOUBLE PRECISION array, dimension (N) */
/*          If ITYPE >= 2, then TAU(j) is the scalar factor of */
/*          v(j) v(j)' in the Householder transformation H(j) of */
/*          the product  U = H(1)...H(n-2) */
/*          If ITYPE < 2, then TAU is not referenced. */
/*          Not modified. */

/*  WORK    DOUBLE PRECISION array, dimension (2*N**2) */
/*          Workspace. */
/*          Modified. */

/*  RESULT  DOUBLE PRECISION array, dimension (2) */
/*          The values computed by the two tests described above.  The */
/*          values are currently limited to 1/ulp, to avoid overflow. */
/*          RESULT(1) is always modified.  RESULT(2) is modified only */
/*          if LDU is at least N. */
/*          Modified. */

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

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --tau;
    --work;
    --result;

    /* Function Body */
    result[1] = 0.;
    result[2] = 0.;
    if (*n <= 0 || *m <= 0) {
	return 0;
    }

    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Precision");

/*     Do Test 1 */

/*     Norm of A: */

/* Computing MAX */
    d__1 = dlansy_("1", uplo, n, &a[a_offset], lda, &work[1]);
    anorm = max(d__1,unfl);

/*     Compute error matrix: */

/*     ITYPE=1: error = U' A U - S */

    dsymm_("L", uplo, n, m, &c_b6, &a[a_offset], lda, &u[u_offset], ldu, &
	    c_b7, &work[1], n);
    nn = *n * *n;
    nnp1 = nn + 1;
    dgemm_("T", "N", m, m, n, &c_b6, &u[u_offset], ldu, &work[1], n, &c_b7, &
	    work[nnp1], n);
    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	jj = nn + (j - 1) * *n + j;
	work[jj] -= d__[j];
/* L10: */
    }
    if (*kband == 1 && *n > 1) {
	i__1 = *m;
	for (j = 2; j <= i__1; ++j) {
	    jj1 = nn + (j - 1) * *n + j - 1;
	    jj2 = nn + (j - 2) * *n + j;
	    work[jj1] -= e[j - 1];
	    work[jj2] -= e[j - 1];
/* L20: */
	}
    }
    wnorm = dlansy_("1", uplo, m, &work[nnp1], n, &work[1]);

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*m * ulp);
    } else {
	if (anorm < 1.) {
/* Computing MIN */
	    d__1 = wnorm, d__2 = *m * anorm;
	    result[1] = min(d__1,d__2) / anorm / (*m * ulp);
	} else {
/* Computing MIN */
	    d__1 = wnorm / anorm, d__2 = (doublereal) (*m);
	    result[1] = min(d__1,d__2) / (*m * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  U'U - I */

    if (*itype == 1) {
	i__1 = (*n << 1) * *n;
	dort01_("Columns", n, m, &u[u_offset], ldu, &work[1], &i__1, &result[
		2]);
    }

    return 0;

/*     End of DSYT22 */

} /* dsyt22_ */
