#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};

/* Subroutine */ int cget54_(integer *n, complex *a, integer *lda, complex *b, 
	 integer *ldb, complex *s, integer *lds, complex *t, integer *ldt, 
	complex *u, integer *ldu, complex *v, integer *ldv, complex *work, 
	real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, s_dim1, s_offset, t_dim1, 
	    t_offset, u_dim1, u_offset, v_dim1, v_offset, i__1;
    real r__1, r__2;
    complex q__1;

    /* Local variables */
    real dum[1], ulp, unfl;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    real wnorm;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *);
    real abnorm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGET54 checks a generalized decomposition of the form */

/*           A = U*S*V'  and B = U*T* V' */

/*  where ' means conjugate transpose and U and V are unitary. */

/*  Specifically, */

/*    RESULT = ||( A - U*S*V', B - U*T*V' )|| / (||( A, B )||*n*ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SGET54 does nothing. */
/*          It must be at least zero. */

/*  A       (input) COMPLEX array, dimension (LDA, N) */
/*          The original (unfactored) matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) COMPLEX array, dimension (LDB, N) */
/*          The original (unfactored) matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  S       (input) COMPLEX array, dimension (LDS, N) */
/*          The factored matrix S. */

/*  LDS     (input) INTEGER */
/*          The leading dimension of S.  It must be at least 1 */
/*          and at least N. */

/*  T       (input) COMPLEX array, dimension (LDT, N) */
/*          The factored matrix T. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of T.  It must be at least 1 */
/*          and at least N. */

/*  U       (input) COMPLEX array, dimension (LDU, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  V       (input) COMPLEX array, dimension (LDV, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */

/*  WORK    (workspace) COMPLEX array, dimension (3*N**2) */

/*  RESULT  (output) REAL */
/*          The value RESULT, It is currently limited to 1/ulp, to */
/*          avoid overflow. Errors are flagged by RESULT=10/ulp. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
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
    s_dim1 = *lds;
    s_offset = 1 + s_dim1;
    s -= s_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
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

/*     compute the norm of (A,B) */

    clacpy_("Full", n, n, &a[a_offset], lda, &work[1], n);
    clacpy_("Full", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n)
	    ;
/* Computing MAX */
    i__1 = *n << 1;
    r__1 = clange_("1", n, &i__1, &work[1], n, dum);
    abnorm = dmax(r__1,unfl);

/*     Compute W1 = A - U*S*V', and put in the array WORK(1:N*N) */

    clacpy_(" ", n, n, &a[a_offset], lda, &work[1], n);
    cgemm_("N", "N", n, n, n, &c_b2, &u[u_offset], ldu, &s[s_offset], lds, &
	    c_b1, &work[*n * *n + 1], n);

    q__1.r = -1.f, q__1.i = -0.f;
    cgemm_("N", "C", n, n, n, &q__1, &work[*n * *n + 1], n, &v[v_offset], ldv, 
	     &c_b2, &work[1], n);

/*     Compute W2 = B - U*T*V', and put in the workarray W(N*N+1:2*N*N) */

    clacpy_(" ", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n);
    cgemm_("N", "N", n, n, n, &c_b2, &u[u_offset], ldu, &t[t_offset], ldt, &
	    c_b1, &work[(*n << 1) * *n + 1], n);

    q__1.r = -1.f, q__1.i = -0.f;
    cgemm_("N", "C", n, n, n, &q__1, &work[(*n << 1) * *n + 1], n, &v[
	    v_offset], ldv, &c_b2, &work[*n * *n + 1], n);

/*     Compute norm(W)/ ( ulp*norm((A,B)) ) */

    i__1 = *n << 1;
    wnorm = clange_("1", n, &i__1, &work[1], n, dum);

    if (abnorm > wnorm) {
	*result = wnorm / abnorm / ((*n << 1) * ulp);
    } else {
	if (abnorm < 1.f) {
/* Computing MIN */
	    r__1 = wnorm, r__2 = (*n << 1) * abnorm;
	    *result = dmin(r__1,r__2) / abnorm / ((*n << 1) * ulp);
	} else {
/* Computing MIN */
	    r__1 = wnorm / abnorm, r__2 = (real) (*n << 1);
	    *result = dmin(r__1,r__2) / ((*n << 1) * ulp);
	}
    }

    return 0;

/*     End of CGET54 */

} /* cget54_ */
