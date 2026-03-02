#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};

/* Subroutine */ int zget51_(integer *itype, integer *n, doublecomplex *a, 
	integer *lda, doublecomplex *b, integer *ldb, doublecomplex *u, 
	integer *ldu, doublecomplex *v, integer *ldv, doublecomplex *work, 
	doublereal *rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, u_dim1, u_offset, v_dim1, 
	    v_offset, i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2;
    doublecomplex z__1;

    /* Local variables */
    doublereal ulp;
    integer jcol;
    doublereal unfl;
    integer jrow, jdiag;
    doublereal anorm;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);
    doublereal wnorm;
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*       ZGET51  generally checks a decomposition of the form */

/*               A = U B V* */

/*       where * means conjugate transpose and U and V are unitary. */

/*       Specifically, if ITYPE=1 */

/*               RESULT = | A - U B V* | / ( |A| n ulp ) */

/*       If ITYPE=2, then: */

/*               RESULT = | A - B | / ( |A| n ulp ) */

/*       If ITYPE=3, then: */

/*               RESULT = | I - UU* | / ( n ulp ) */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the type of tests to be performed. */
/*          =1: RESULT = | A - U B V* | / ( |A| n ulp ) */
/*          =2: RESULT = | A - B | / ( |A| n ulp ) */
/*          =3: RESULT = | I - UU* | / ( n ulp ) */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, ZGET51 does nothing. */
/*          It must be at least zero. */

/*  A       (input) COMPLEX*16 array, dimension (LDA, N) */
/*          The original (unfactored) matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) COMPLEX*16 array, dimension (LDB, N) */
/*          The factored matrix. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  U       (input) COMPLEX*16 array, dimension (LDU, N) */
/*          The unitary matrix on the left-hand side in the */
/*          decomposition. */
/*          Not referenced if ITYPE=2 */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  V       (input) COMPLEX*16 array, dimension (LDV, N) */
/*          The unitary matrix on the left-hand side in the */
/*          decomposition. */
/*          Not referenced if ITYPE=2 */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (2*N**2) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESULT  (output) DOUBLE PRECISION */
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
    --rwork;

    /* Function Body */
    *result = 0.;
    if (*n <= 0) {
	return 0;
    }

/*     Constants */

    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Epsilon") * dlamch_("Base");

/*     Some Error Checks */

    if (*itype < 1 || *itype > 3) {
	*result = 10. / ulp;
	return 0;
    }

    if (*itype <= 2) {

/*        Tests scaled by the norm(A) */

/* Computing MAX */
	d__1 = zlange_("1", n, n, &a[a_offset], lda, &rwork[1]);
	anorm = max(d__1,unfl);

	if (*itype == 1) {

/*           ITYPE=1: Compute W = A - UBV' */

	    zlacpy_(" ", n, n, &a[a_offset], lda, &work[1], n);
/* Computing 2nd power */
	    i__1 = *n;
	    zgemm_("N", "N", n, n, n, &c_b2, &u[u_offset], ldu, &b[b_offset], 
		    ldb, &c_b1, &work[i__1 * i__1 + 1], n);

	    z__1.r = -1., z__1.i = -0.;
/* Computing 2nd power */
	    i__1 = *n;
	    zgemm_("N", "C", n, n, n, &z__1, &work[i__1 * i__1 + 1], n, &v[
		    v_offset], ldv, &c_b2, &work[1], n);

	} else {

/*           ITYPE=2: Compute W = A - B */

	    zlacpy_(" ", n, n, &b[b_offset], ldb, &work[1], n);

	    i__1 = *n;
	    for (jcol = 1; jcol <= i__1; ++jcol) {
		i__2 = *n;
		for (jrow = 1; jrow <= i__2; ++jrow) {
		    i__3 = jrow + *n * (jcol - 1);
		    i__4 = jrow + *n * (jcol - 1);
		    i__5 = jrow + jcol * a_dim1;
		    z__1.r = work[i__4].r - a[i__5].r, z__1.i = work[i__4].i 
			    - a[i__5].i;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L10: */
		}
/* L20: */
	    }
	}

/*        Compute norm(W)/ ( ulp*norm(A) ) */

	wnorm = zlange_("1", n, n, &work[1], n, &rwork[1]);

	if (anorm > wnorm) {
	    *result = wnorm / anorm / (*n * ulp);
	} else {
	    if (anorm < 1.) {
/* Computing MIN */
		d__1 = wnorm, d__2 = *n * anorm;
		*result = min(d__1,d__2) / anorm / (*n * ulp);
	    } else {
/* Computing MIN */
		d__1 = wnorm / anorm, d__2 = (doublereal) (*n);
		*result = min(d__1,d__2) / (*n * ulp);
	    }
	}

    } else {

/*        Tests not scaled by norm(A) */

/*        ITYPE=3: Compute  UU' - I */

	zgemm_("N", "C", n, n, n, &c_b2, &u[u_offset], ldu, &u[u_offset], ldu, 
		 &c_b1, &work[1], n);

	i__1 = *n;
	for (jdiag = 1; jdiag <= i__1; ++jdiag) {
	    i__2 = (*n + 1) * (jdiag - 1) + 1;
	    i__3 = (*n + 1) * (jdiag - 1) + 1;
	    z__1.r = work[i__3].r - 1., z__1.i = work[i__3].i - 0.;
	    work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L30: */
	}

/* Computing MIN */
	d__1 = zlange_("1", n, n, &work[1], n, &rwork[1]), d__2 = (
		doublereal) (*n);
	*result = min(d__1,d__2) / (*n * ulp);
    }

    return 0;

/*     End of ZGET51 */

} /* zget51_ */
