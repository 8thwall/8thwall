#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b9 = -1e10f;
static real c_b19 = 0.f;
static real c_b30 = -1.f;
static real c_b31 = 1.f;

/* Subroutine */ int sgqrts_(integer *n, integer *m, integer *p, real *a, 
	real *af, real *q, real *r__, integer *lda, real *taua, real *b, real 
	*bf, real *z__, real *t, real *bwk, integer *ldb, real *taub, real *
	work, integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, r_dim1, r_offset, q_dim1, 
	    q_offset, b_dim1, b_offset, bf_dim1, bf_offset, t_dim1, t_offset, 
	    z_dim1, z_offset, bwk_dim1, bwk_offset, i__1, i__2;
    real r__1;

    /* Local variables */
    real ulp;
    integer info;
    real unfl, resid;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm, bnorm;
    extern /* Subroutine */ int ssyrk_(char *, char *, integer *, integer *, 
	    real *, real *, integer *, real *, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int sggqrf_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, real *, real *, integer *
, integer *), slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *);
    extern doublereal slansy_(char *, char *, integer *, real *, integer *, 
	    real *);
    extern /* Subroutine */ int sorgqr_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *), sorgrq_(
	    integer *, integer *, integer *, real *, integer *, real *, real *
, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGQRTS tests SGGQRF, which computes the GQR factorization of an */
/*  N-by-M matrix A and a N-by-P matrix B: A = Q*R and B = Q*T*Z. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices A and B.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of columns of the matrix B.  P >= 0. */

/*  A       (input) REAL array, dimension (LDA,M) */
/*          The N-by-M matrix A. */

/*  AF      (output) REAL array, dimension (LDA,N) */
/*          Details of the GQR factorization of A and B, as returned */
/*          by SGGQRF, see SGGQRF for further details. */

/*  Q       (output) REAL array, dimension (LDA,N) */
/*          The M-by-M orthogonal matrix Q. */

/*  R       (workspace) REAL array, dimension (LDA,MAX(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, R and Q. */
/*          LDA >= max(M,N). */

/*  TAUA    (output) REAL array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGGQRF. */

/*  B       (input) REAL array, dimension (LDB,P) */
/*          On entry, the N-by-P matrix A. */

/*  BF      (output) REAL array, dimension (LDB,N) */
/*          Details of the GQR factorization of A and B, as returned */
/*          by SGGQRF, see SGGQRF for further details. */

/*  Z       (output) REAL array, dimension (LDB,P) */
/*          The P-by-P orthogonal matrix Z. */

/*  T       (workspace) REAL array, dimension (LDB,max(P,N)) */

/*  BWK     (workspace) REAL array, dimension (LDB,N) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF, Z and T. */
/*          LDB >= max(P,N). */

/*  TAUB    (output) REAL array, dimension (min(P,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGGRQF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK, LWORK >= max(N,M,P)**2. */

/*  RWORK   (workspace) REAL array, dimension (max(N,M,P)) */

/*  RESULT  (output) REAL array, dimension (4) */
/*          The test ratios: */
/*            RESULT(1) = norm( R - Q'*A ) / ( MAX(M,N)*norm(A)*ULP) */
/*            RESULT(2) = norm( T*Z - Q'*B ) / (MAX(P,N)*norm(B)*ULP) */
/*            RESULT(3) = norm( I - Q'*Q ) / ( M*ULP ) */
/*            RESULT(4) = norm( I - Z'*Z ) / ( P*ULP ) */

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
    r_dim1 = *lda;
    r_offset = 1 + r_dim1;
    r__ -= r_offset;
    q_dim1 = *lda;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    af_dim1 = *lda;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --taua;
    bwk_dim1 = *ldb;
    bwk_offset = 1 + bwk_dim1;
    bwk -= bwk_offset;
    t_dim1 = *ldb;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    z_dim1 = *ldb;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    bf_dim1 = *ldb;
    bf_offset = 1 + bf_dim1;
    bf -= bf_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --taub;
    --work;
    --rwork;
    --result;

    /* Function Body */
    ulp = slamch_("Precision");
    unfl = slamch_("Safe minimum");

/*     Copy the matrix A to the array AF. */

    slacpy_("Full", n, m, &a[a_offset], lda, &af[af_offset], lda);
    slacpy_("Full", n, p, &b[b_offset], ldb, &bf[bf_offset], ldb);

/* Computing MAX */
    r__1 = slange_("1", n, m, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);
/* Computing MAX */
    r__1 = slange_("1", n, p, &b[b_offset], ldb, &rwork[1]);
    bnorm = dmax(r__1,unfl);

/*     Factorize the matrices A and B in the arrays AF and BF. */

    sggqrf_(n, m, p, &af[af_offset], lda, &taua[1], &bf[bf_offset], ldb, &
	    taub[1], &work[1], lwork, &info);

/*     Generate the N-by-N matrix Q */

    slaset_("Full", n, n, &c_b9, &c_b9, &q[q_offset], lda);
    i__1 = *n - 1;
    slacpy_("Lower", &i__1, m, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);
    i__1 = min(*n,*m);
    sorgqr_(n, n, &i__1, &q[q_offset], lda, &taua[1], &work[1], lwork, &info);

/*     Generate the P-by-P matrix Z */

    slaset_("Full", p, p, &c_b9, &c_b9, &z__[z_offset], ldb);
    if (*n <= *p) {
	if (*n > 0 && *n < *p) {
	    i__1 = *p - *n;
	    slacpy_("Full", n, &i__1, &bf[bf_offset], ldb, &z__[*p - *n + 1 + 
		    z_dim1], ldb);
	}
	if (*n > 1) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    slacpy_("Lower", &i__1, &i__2, &bf[(*p - *n + 1) * bf_dim1 + 2], 
		    ldb, &z__[*p - *n + 2 + (*p - *n + 1) * z_dim1], ldb);
	}
    } else {
	if (*p > 1) {
	    i__1 = *p - 1;
	    i__2 = *p - 1;
	    slacpy_("Lower", &i__1, &i__2, &bf[*n - *p + 2 + bf_dim1], ldb, &
		    z__[z_dim1 + 2], ldb);
	}
    }
    i__1 = min(*n,*p);
    sorgrq_(p, p, &i__1, &z__[z_offset], ldb, &taub[1], &work[1], lwork, &
	    info);

/*     Copy R */

    slaset_("Full", n, m, &c_b19, &c_b19, &r__[r_offset], lda);
    slacpy_("Upper", n, m, &af[af_offset], lda, &r__[r_offset], lda);

/*     Copy T */

    slaset_("Full", n, p, &c_b19, &c_b19, &t[t_offset], ldb);
    if (*n <= *p) {
	slacpy_("Upper", n, n, &bf[(*p - *n + 1) * bf_dim1 + 1], ldb, &t[(*p 
		- *n + 1) * t_dim1 + 1], ldb);
    } else {
	i__1 = *n - *p;
	slacpy_("Full", &i__1, p, &bf[bf_offset], ldb, &t[t_offset], ldb);
	slacpy_("Upper", p, p, &bf[*n - *p + 1 + bf_dim1], ldb, &t[*n - *p + 
		1 + t_dim1], ldb);
    }

/*     Compute R - Q'*A */

    sgemm_("Transpose", "No transpose", n, m, n, &c_b30, &q[q_offset], lda, &
	    a[a_offset], lda, &c_b31, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( MAX(M,N)*norm(A)*ULP ) . */

    resid = slange_("1", n, m, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*m);
	result[1] = resid / (real) max(i__1,*n) / anorm / ulp;
    } else {
	result[1] = 0.f;
    }

/*     Compute T*Z - Q'*B */

    sgemm_("No Transpose", "No transpose", n, p, p, &c_b31, &t[t_offset], ldb, 
	     &z__[z_offset], ldb, &c_b19, &bwk[bwk_offset], ldb);
    sgemm_("Transpose", "No transpose", n, p, n, &c_b30, &q[q_offset], lda, &
	    b[b_offset], ldb, &c_b31, &bwk[bwk_offset], ldb);

/*     Compute norm( T*Z - Q'*B ) / ( MAX(P,N)*norm(A)*ULP ) . */

    resid = slange_("1", n, p, &bwk[bwk_offset], ldb, &rwork[1]);
    if (bnorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*p);
	result[2] = resid / (real) max(i__1,*n) / bnorm / ulp;
    } else {
	result[2] = 0.f;
    }

/*     Compute I - Q'*Q */

    slaset_("Full", n, n, &c_b19, &c_b31, &r__[r_offset], lda);
    ssyrk_("Upper", "Transpose", n, n, &c_b30, &q[q_offset], lda, &c_b31, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( N * ULP ) . */

    resid = slansy_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);
    result[3] = resid / (real) max(1,*n) / ulp;

/*     Compute I - Z'*Z */

    slaset_("Full", p, p, &c_b19, &c_b31, &t[t_offset], ldb);
    ssyrk_("Upper", "Transpose", p, p, &c_b30, &z__[z_offset], ldb, &c_b31, &
	    t[t_offset], ldb);

/*     Compute norm( I - Z'*Z ) / ( P*ULP ) . */

    resid = slansy_("1", "Upper", p, &t[t_offset], ldb, &rwork[1]);
    result[4] = resid / (real) max(1,*p) / ulp;

    return 0;

/*     End of SGQRTS */

} /* sgqrts_ */
