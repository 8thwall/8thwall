#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b9 = 1.f;
static real c_b10 = 0.f;
static real c_b13 = -1.f;

/* Subroutine */ int sget51_(integer *itype, integer *n, real *a, integer *
	lda, real *b, integer *ldb, real *u, integer *ldu, real *v, integer *
	ldv, real *work, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, u_dim1, u_offset, v_dim1, 
	    v_offset, i__1, i__2;
    real r__1, r__2;

    /* Local variables */
    real ulp;
    integer jcol;
    real unfl;
    integer jrow, jdiag;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm, wnorm;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*       SGET51  generally checks a decomposition of the form */

/*               A = U B V' */

/*       where ' means transpose and U and V are orthogonal. */

/*       Specifically, if ITYPE=1 */

/*               RESULT = | A - U B V' | / ( |A| n ulp ) */

/*       If ITYPE=2, then: */

/*               RESULT = | A - B | / ( |A| n ulp ) */

/*       If ITYPE=3, then: */

/*               RESULT = | I - UU' | / ( n ulp ) */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the type of tests to be performed. */
/*          =1: RESULT = | A - U B V' | / ( |A| n ulp ) */
/*          =2: RESULT = | A - B | / ( |A| n ulp ) */
/*          =3: RESULT = | I - UU' | / ( n ulp ) */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SGET51 does nothing. */
/*          It must be at least zero. */

/*  A       (input) REAL array, dimension (LDA, N) */
/*          The original (unfactored) matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) REAL array, dimension (LDB, N) */
/*          The factored matrix. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  U       (input) REAL array, dimension (LDU, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */
/*          Not referenced if ITYPE=2 */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  V       (input) REAL array, dimension (LDV, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */
/*          Not referenced if ITYPE=2 */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */

/*  WORK    (workspace) REAL array, dimension (2*N**2) */

/*  RESULT  (output) REAL */
/*          The values computed by the test specified by ITYPE.  The */
/*          value is currently limited to 1/ulp, to avoid overflow. */
/*          Errors are flagged by RESULT=10/ulp. */

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
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --work;

    /* Function Body */
    *result = 0.f;
    if (*n <= 0) {
	return 0;
    }

/*     Constants */

    unfl = slamch_("Safe minimum");
    ulp = slamch_("Epsilon") * slamch_("Base");

/*     Some Error Checks */

    if (*itype < 1 || *itype > 3) {
	*result = 10.f / ulp;
	return 0;
    }

    if (*itype <= 2) {

/*        Tests scaled by the norm(A) */

/* Computing MAX */
	r__1 = slange_("1", n, n, &a[a_offset], lda, &work[1]);
	anorm = dmax(r__1,unfl);

	if (*itype == 1) {

/*           ITYPE=1: Compute W = A - UBV' */

	    slacpy_(" ", n, n, &a[a_offset], lda, &work[1], n);
/* Computing 2nd power */
	    i__1 = *n;
	    sgemm_("N", "N", n, n, n, &c_b9, &u[u_offset], ldu, &b[b_offset], 
		    ldb, &c_b10, &work[i__1 * i__1 + 1], n);

/* Computing 2nd power */
	    i__1 = *n;
	    sgemm_("N", "C", n, n, n, &c_b13, &work[i__1 * i__1 + 1], n, &v[
		    v_offset], ldv, &c_b9, &work[1], n);

	} else {

/*           ITYPE=2: Compute W = A - B */

	    slacpy_(" ", n, n, &b[b_offset], ldb, &work[1], n);

	    i__1 = *n;
	    for (jcol = 1; jcol <= i__1; ++jcol) {
		i__2 = *n;
		for (jrow = 1; jrow <= i__2; ++jrow) {
		    work[jrow + *n * (jcol - 1)] -= a[jrow + jcol * a_dim1];
/* L10: */
		}
/* L20: */
	    }
	}

/*        Compute norm(W)/ ( ulp*norm(A) ) */

/* Computing 2nd power */
	i__1 = *n;
	wnorm = slange_("1", n, n, &work[1], n, &work[i__1 * i__1 + 1]);

	if (anorm > wnorm) {
	    *result = wnorm / anorm / (*n * ulp);
	} else {
	    if (anorm < 1.f) {
/* Computing MIN */
		r__1 = wnorm, r__2 = *n * anorm;
		*result = dmin(r__1,r__2) / anorm / (*n * ulp);
	    } else {
/* Computing MIN */
		r__1 = wnorm / anorm, r__2 = (real) (*n);
		*result = dmin(r__1,r__2) / (*n * ulp);
	    }
	}

    } else {

/*        Tests not scaled by norm(A) */

/*        ITYPE=3: Compute  UU' - I */

	sgemm_("N", "C", n, n, n, &c_b9, &u[u_offset], ldu, &u[u_offset], ldu, 
		 &c_b10, &work[1], n);

	i__1 = *n;
	for (jdiag = 1; jdiag <= i__1; ++jdiag) {
	    work[(*n + 1) * (jdiag - 1) + 1] += -1.f;
/* L30: */
	}

/* Computing MIN */
/* Computing 2nd power */
	i__1 = *n;
	r__1 = slange_("1", n, n, &work[1], n, &work[i__1 * i__1 + 1]), r__2 = (real) (*n);
	*result = dmin(r__1,r__2) / (*n * ulp);
    }

    return 0;

/*     End of SGET51 */

} /* sget51_ */
