#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b9 = -1e10f;
static real c_b19 = 0.f;
static real c_b30 = -1.f;
static real c_b31 = 1.f;

/* Subroutine */ int sgrqts_(integer *m, integer *p, integer *n, real *a, 
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
    extern /* Subroutine */ int sggrqf_(integer *, integer *, integer *, real 
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

/*  SGRQTS tests SGGRQF, which computes the GRQ factorization of an */
/*  M-by-N matrix A and a P-by-N matrix B: A = R*Q and B = Z*T*Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  AF      (output) REAL array, dimension (LDA,N) */
/*          Details of the GRQ factorization of A and B, as returned */
/*          by SGGRQF, see SGGRQF for further details. */

/*  Q       (output) REAL array, dimension (LDA,N) */
/*          The N-by-N orthogonal matrix Q. */

/*  R       (workspace) REAL array, dimension (LDA,MAX(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, R and Q. */
/*          LDA >= max(M,N). */

/*  TAUA    (output) REAL array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGGQRC. */

/*  B       (input) REAL array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix A. */

/*  BF      (output) REAL array, dimension (LDB,N) */
/*          Details of the GQR factorization of A and B, as returned */
/*          by SGGRQF, see SGGRQF for further details. */

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
/*          The dimension of the array WORK, LWORK >= max(M,P,N)**2. */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESULT  (output) REAL array, dimension (4) */
/*          The test ratios: */
/*            RESULT(1) = norm( R - A*Q' ) / ( MAX(M,N)*norm(A)*ULP) */
/*            RESULT(2) = norm( T*Q - Z'*B ) / (MAX(P,N)*norm(B)*ULP) */
/*            RESULT(3) = norm( I - Q'*Q ) / ( N*ULP ) */
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

    slacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);
    slacpy_("Full", p, n, &b[b_offset], ldb, &bf[bf_offset], ldb);

/* Computing MAX */
    r__1 = slange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);
/* Computing MAX */
    r__1 = slange_("1", p, n, &b[b_offset], ldb, &rwork[1]);
    bnorm = dmax(r__1,unfl);

/*     Factorize the matrices A and B in the arrays AF and BF. */

    sggrqf_(m, p, n, &af[af_offset], lda, &taua[1], &bf[bf_offset], ldb, &
	    taub[1], &work[1], lwork, &info);

/*     Generate the N-by-N matrix Q */

    slaset_("Full", n, n, &c_b9, &c_b9, &q[q_offset], lda);
    if (*m <= *n) {
	if (*m > 0 && *m < *n) {
	    i__1 = *n - *m;
	    slacpy_("Full", m, &i__1, &af[af_offset], lda, &q[*n - *m + 1 + 
		    q_dim1], lda);
	}
	if (*m > 1) {
	    i__1 = *m - 1;
	    i__2 = *m - 1;
	    slacpy_("Lower", &i__1, &i__2, &af[(*n - *m + 1) * af_dim1 + 2], 
		    lda, &q[*n - *m + 2 + (*n - *m + 1) * q_dim1], lda);
	}
    } else {
	if (*n > 1) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    slacpy_("Lower", &i__1, &i__2, &af[*m - *n + 2 + af_dim1], lda, &
		    q[q_dim1 + 2], lda);
	}
    }
    i__1 = min(*m,*n);
    sorgrq_(n, n, &i__1, &q[q_offset], lda, &taua[1], &work[1], lwork, &info);

/*     Generate the P-by-P matrix Z */

    slaset_("Full", p, p, &c_b9, &c_b9, &z__[z_offset], ldb);
    if (*p > 1) {
	i__1 = *p - 1;
	slacpy_("Lower", &i__1, n, &bf[bf_dim1 + 2], ldb, &z__[z_dim1 + 2], 
		ldb);
    }
    i__1 = min(*p,*n);
    sorgqr_(p, p, &i__1, &z__[z_offset], ldb, &taub[1], &work[1], lwork, &
	    info);

/*     Copy R */

    slaset_("Full", m, n, &c_b19, &c_b19, &r__[r_offset], lda);
    if (*m <= *n) {
	slacpy_("Upper", m, m, &af[(*n - *m + 1) * af_dim1 + 1], lda, &r__[(*
		n - *m + 1) * r_dim1 + 1], lda);
    } else {
	i__1 = *m - *n;
	slacpy_("Full", &i__1, n, &af[af_offset], lda, &r__[r_offset], lda);
	slacpy_("Upper", n, n, &af[*m - *n + 1 + af_dim1], lda, &r__[*m - *n 
		+ 1 + r_dim1], lda);
    }

/*     Copy T */

    slaset_("Full", p, n, &c_b19, &c_b19, &t[t_offset], ldb);
    slacpy_("Upper", p, n, &bf[bf_offset], ldb, &t[t_offset], ldb);

/*     Compute R - A*Q' */

    sgemm_("No transpose", "Transpose", m, n, n, &c_b30, &a[a_offset], lda, &
	    q[q_offset], lda, &c_b31, &r__[r_offset], lda);

/*     Compute norm( R - A*Q' ) / ( MAX(M,N)*norm(A)*ULP ) . */

    resid = slange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*m);
	result[1] = resid / (real) max(i__1,*n) / anorm / ulp;
    } else {
	result[1] = 0.f;
    }

/*     Compute T*Q - Z'*B */

    sgemm_("Transpose", "No transpose", p, n, p, &c_b31, &z__[z_offset], ldb, 
	    &b[b_offset], ldb, &c_b19, &bwk[bwk_offset], ldb);
    sgemm_("No transpose", "No transpose", p, n, n, &c_b31, &t[t_offset], ldb, 
	     &q[q_offset], lda, &c_b30, &bwk[bwk_offset], ldb);

/*     Compute norm( T*Q - Z'*B ) / ( MAX(P,N)*norm(A)*ULP ) . */

    resid = slange_("1", p, n, &bwk[bwk_offset], ldb, &rwork[1]);
    if (bnorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*p);
	result[2] = resid / (real) max(i__1,*m) / bnorm / ulp;
    } else {
	result[2] = 0.f;
    }

/*     Compute I - Q*Q' */

    slaset_("Full", n, n, &c_b19, &c_b31, &r__[r_offset], lda);
    ssyrk_("Upper", "No Transpose", n, n, &c_b30, &q[q_offset], lda, &c_b31, &
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

/*     End of SGRQTS */

} /* sgrqts_ */
