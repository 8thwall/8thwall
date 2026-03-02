#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};

/* Subroutine */ int zget54_(integer *n, doublecomplex *a, integer *lda, 
	doublecomplex *b, integer *ldb, doublecomplex *s, integer *lds, 
	doublecomplex *t, integer *ldt, doublecomplex *u, integer *ldu, 
	doublecomplex *v, integer *ldv, doublecomplex *work, doublereal *
	result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, s_dim1, s_offset, t_dim1, 
	    t_offset, u_dim1, u_offset, v_dim1, v_offset, i__1;
    doublereal d__1, d__2;
    doublecomplex z__1;

    /* Local variables */
    doublereal dum[1], ulp, unfl;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);
    doublereal wnorm;
    extern doublereal dlamch_(char *);
    doublereal abnorm;
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
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

/*  ZGET54 checks a generalized decomposition of the form */

/*           A = U*S*V'  and B = U*T* V' */

/*  where ' means conjugate transpose and U and V are unitary. */

/*  Specifically, */

/*    RESULT = ||( A - U*S*V', B - U*T*V' )|| / (||( A, B )||*n*ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, DGET54 does nothing. */
/*          It must be at least zero. */

/*  A       (input) COMPLEX*16 array, dimension (LDA, N) */
/*          The original (unfactored) matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) COMPLEX*16 array, dimension (LDB, N) */
/*          The original (unfactored) matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  S       (input) COMPLEX*16 array, dimension (LDS, N) */
/*          The factored matrix S. */

/*  LDS     (input) INTEGER */
/*          The leading dimension of S.  It must be at least 1 */
/*          and at least N. */

/*  T       (input) COMPLEX*16 array, dimension (LDT, N) */
/*          The factored matrix T. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of T.  It must be at least 1 */
/*          and at least N. */

/*  U       (input) COMPLEX*16 array, dimension (LDU, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  V       (input) COMPLEX*16 array, dimension (LDV, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (3*N**2) */

/*  RESULT  (output) DOUBLE PRECISION */
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
    *result = 0.;
    if (*n <= 0) {
	return 0;
    }

/*     Constants */

    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Epsilon") * dlamch_("Base");

/*     compute the norm of (A,B) */

    zlacpy_("Full", n, n, &a[a_offset], lda, &work[1], n);
    zlacpy_("Full", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n)
	    ;
/* Computing MAX */
    i__1 = *n << 1;
    d__1 = zlange_("1", n, &i__1, &work[1], n, dum);
    abnorm = max(d__1,unfl);

/*     Compute W1 = A - U*S*V', and put in the array WORK(1:N*N) */

    zlacpy_(" ", n, n, &a[a_offset], lda, &work[1], n);
    zgemm_("N", "N", n, n, n, &c_b2, &u[u_offset], ldu, &s[s_offset], lds, &
	    c_b1, &work[*n * *n + 1], n);

    z__1.r = -1., z__1.i = -0.;
    zgemm_("N", "C", n, n, n, &z__1, &work[*n * *n + 1], n, &v[v_offset], ldv, 
	     &c_b2, &work[1], n);

/*     Compute W2 = B - U*T*V', and put in the workarray W(N*N+1:2*N*N) */

    zlacpy_(" ", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n);
    zgemm_("N", "N", n, n, n, &c_b2, &u[u_offset], ldu, &t[t_offset], ldt, &
	    c_b1, &work[(*n << 1) * *n + 1], n);

    z__1.r = -1., z__1.i = -0.;
    zgemm_("N", "C", n, n, n, &z__1, &work[(*n << 1) * *n + 1], n, &v[
	    v_offset], ldv, &c_b2, &work[*n * *n + 1], n);

/*     Compute norm(W)/ ( ulp*norm((A,B)) ) */

    i__1 = *n << 1;
    wnorm = zlange_("1", n, &i__1, &work[1], n, dum);

    if (abnorm > wnorm) {
	*result = wnorm / abnorm / ((*n << 1) * ulp);
    } else {
	if (abnorm < 1.) {
/* Computing MIN */
	    d__1 = wnorm, d__2 = (*n << 1) * abnorm;
	    *result = min(d__1,d__2) / abnorm / ((*n << 1) * ulp);
	} else {
/* Computing MIN */
	    d__1 = wnorm / abnorm, d__2 = (doublereal) (*n << 1);
	    *result = min(d__1,d__2) / ((*n << 1) * ulp);
	}
    }

    return 0;

/*     End of ZGET54 */

} /* zget54_ */
