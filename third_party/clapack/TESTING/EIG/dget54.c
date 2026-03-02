#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b11 = 1.;
static doublereal c_b12 = 0.;
static doublereal c_b15 = -1.;

/* Subroutine */ int dget54_(integer *n, doublereal *a, integer *lda, 
	doublereal *b, integer *ldb, doublereal *s, integer *lds, doublereal *
	t, integer *ldt, doublereal *u, integer *ldu, doublereal *v, integer *
	ldv, doublereal *work, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, s_dim1, s_offset, t_dim1, 
	    t_offset, u_dim1, u_offset, v_dim1, v_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    doublereal dum[1], ulp, unfl;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    doublereal wnorm;
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal abnorm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET54 checks a generalized decomposition of the form */

/*           A = U*S*V'  and B = U*T* V' */

/*  where ' means transpose and U and V are orthogonal. */

/*  Specifically, */

/*   RESULT = ||( A - U*S*V', B - U*T*V' )|| / (||( A, B )||*n*ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, DGET54 does nothing. */
/*          It must be at least zero. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, N) */
/*          The original (unfactored) matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB, N) */
/*          The original (unfactored) matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  S       (input) DOUBLE PRECISION array, dimension (LDS, N) */
/*          The factored matrix S. */

/*  LDS     (input) INTEGER */
/*          The leading dimension of S.  It must be at least 1 */
/*          and at least N. */

/*  T       (input) DOUBLE PRECISION array, dimension (LDT, N) */
/*          The factored matrix T. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of T.  It must be at least 1 */
/*          and at least N. */

/*  U       (input) DOUBLE PRECISION array, dimension (LDU, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  V       (input) DOUBLE PRECISION array, dimension (LDV, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N**2) */

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

    dlacpy_("Full", n, n, &a[a_offset], lda, &work[1], n);
    dlacpy_("Full", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n)
	    ;
/* Computing MAX */
    i__1 = *n << 1;
    d__1 = dlange_("1", n, &i__1, &work[1], n, dum);
    abnorm = max(d__1,unfl);

/*     Compute W1 = A - U*S*V', and put in the array WORK(1:N*N) */

    dlacpy_(" ", n, n, &a[a_offset], lda, &work[1], n);
    dgemm_("N", "N", n, n, n, &c_b11, &u[u_offset], ldu, &s[s_offset], lds, &
	    c_b12, &work[*n * *n + 1], n);

    dgemm_("N", "C", n, n, n, &c_b15, &work[*n * *n + 1], n, &v[v_offset], 
	    ldv, &c_b11, &work[1], n);

/*     Compute W2 = B - U*T*V', and put in the workarray W(N*N+1:2*N*N) */

    dlacpy_(" ", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n);
    dgemm_("N", "N", n, n, n, &c_b11, &u[u_offset], ldu, &t[t_offset], ldt, &
	    c_b12, &work[(*n << 1) * *n + 1], n);

    dgemm_("N", "C", n, n, n, &c_b15, &work[(*n << 1) * *n + 1], n, &v[
	    v_offset], ldv, &c_b11, &work[*n * *n + 1], n);

/*     Compute norm(W)/ ( ulp*norm((A,B)) ) */

    i__1 = *n << 1;
    wnorm = dlange_("1", n, &i__1, &work[1], n, dum);

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

/*     End of DGET54 */

} /* dget54_ */
