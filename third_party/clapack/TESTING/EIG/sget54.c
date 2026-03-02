#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b11 = 1.f;
static real c_b12 = 0.f;
static real c_b15 = -1.f;

/* Subroutine */ int sget54_(integer *n, real *a, integer *lda, real *b, 
	integer *ldb, real *s, integer *lds, real *t, integer *ldt, real *u, 
	integer *ldu, real *v, integer *ldv, real *work, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, s_dim1, s_offset, t_dim1, 
	    t_offset, u_dim1, u_offset, v_dim1, v_offset, i__1;
    real r__1, r__2;

    /* Local variables */
    real dum[1], ulp, unfl;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real wnorm;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    real abnorm;
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

/*  SGET54 checks a generalized decomposition of the form */

/*           A = U*S*V'  and B = U*T* V' */

/*  where ' means transpose and U and V are orthogonal. */

/*  Specifically, */

/*   RESULT = ||( A - U*S*V', B - U*T*V' )|| / (||( A, B )||*n*ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SGET54 does nothing. */
/*          It must be at least zero. */

/*  A       (input) REAL array, dimension (LDA, N) */
/*          The original (unfactored) matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) REAL array, dimension (LDB, N) */
/*          The original (unfactored) matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  S       (input) REAL array, dimension (LDS, N) */
/*          The factored matrix S. */

/*  LDS     (input) INTEGER */
/*          The leading dimension of S.  It must be at least 1 */
/*          and at least N. */

/*  T       (input) REAL array, dimension (LDT, N) */
/*          The factored matrix T. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of T.  It must be at least 1 */
/*          and at least N. */

/*  U       (input) REAL array, dimension (LDU, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  V       (input) REAL array, dimension (LDV, N) */
/*          The orthogonal matrix on the left-hand side in the */
/*          decomposition. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */

/*  WORK    (workspace) REAL array, dimension (3*N**2) */

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

    slacpy_("Full", n, n, &a[a_offset], lda, &work[1], n);
    slacpy_("Full", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n)
	    ;
/* Computing MAX */
    i__1 = *n << 1;
    r__1 = slange_("1", n, &i__1, &work[1], n, dum);
    abnorm = dmax(r__1,unfl);

/*     Compute W1 = A - U*S*V', and put in the array WORK(1:N*N) */

    slacpy_(" ", n, n, &a[a_offset], lda, &work[1], n);
    sgemm_("N", "N", n, n, n, &c_b11, &u[u_offset], ldu, &s[s_offset], lds, &
	    c_b12, &work[*n * *n + 1], n);

    sgemm_("N", "C", n, n, n, &c_b15, &work[*n * *n + 1], n, &v[v_offset], 
	    ldv, &c_b11, &work[1], n);

/*     Compute W2 = B - U*T*V', and put in the workarray W(N*N+1:2*N*N) */

    slacpy_(" ", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n);
    sgemm_("N", "N", n, n, n, &c_b11, &u[u_offset], ldu, &t[t_offset], ldt, &
	    c_b12, &work[(*n << 1) * *n + 1], n);

    sgemm_("N", "C", n, n, n, &c_b15, &work[(*n << 1) * *n + 1], n, &v[
	    v_offset], ldv, &c_b11, &work[*n * *n + 1], n);

/*     Compute norm(W)/ ( ulp*norm((A,B)) ) */

    i__1 = *n << 1;
    wnorm = slange_("1", n, &i__1, &work[1], n, dum);

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

/*     End of SGET54 */

} /* sget54_ */
