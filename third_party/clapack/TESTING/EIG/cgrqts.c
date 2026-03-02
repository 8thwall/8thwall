#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static complex c_b3 = {-1e10f,0.f};
static real c_b34 = -1.f;
static real c_b35 = 1.f;

/* Subroutine */ int cgrqts_(integer *m, integer *p, integer *n, complex *a, 
	complex *af, complex *q, complex *r__, integer *lda, complex *taua, 
	complex *b, complex *bf, complex *z__, complex *t, complex *bwk, 
	integer *ldb, complex *taub, complex *work, integer *lwork, real *
	rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, r_dim1, r_offset, q_dim1, 
	    q_offset, b_dim1, b_offset, bf_dim1, bf_offset, t_dim1, t_offset, 
	    z_dim1, z_offset, bwk_dim1, bwk_offset, i__1, i__2;
    real r__1;
    complex q__1;

    /* Local variables */
    real ulp;
    integer info;
    real unfl;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *), cherk_(char *, 
	    char *, integer *, integer *, real *, complex *, integer *, real *
, complex *, integer *);
    real resid, anorm, bnorm;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), clanhe_(char *, char *, integer *, 
	    complex *, integer *, real *), slamch_(char *);
    extern /* Subroutine */ int cggrqf_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, complex *, 
	    complex *, integer *, integer *), clacpy_(char *, integer *, 
	    integer *, complex *, integer *, complex *, integer *), 
	    claset_(char *, integer *, integer *, complex *, complex *, 
	    complex *, integer *), cungqr_(integer *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    integer *), cungrq_(integer *, integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGRQTS tests CGGRQF, which computes the GRQ factorization of an */
/*  M-by-N matrix A and a P-by-N matrix B: A = R*Q and B = Z*T*Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  AF      (output) COMPLEX array, dimension (LDA,N) */
/*          Details of the GRQ factorization of A and B, as returned */
/*          by CGGRQF, see CGGRQF for further details. */

/*  Q       (output) COMPLEX array, dimension (LDA,N) */
/*          The N-by-N unitary matrix Q. */

/*  R       (workspace) COMPLEX array, dimension (LDA,MAX(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, R and Q. */
/*          LDA >= max(M,N). */

/*  TAUA    (output) COMPLEX array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGGQRC. */

/*  B       (input) COMPLEX array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix A. */

/*  BF      (output) COMPLEX array, dimension (LDB,N) */
/*          Details of the GQR factorization of A and B, as returned */
/*          by CGGRQF, see CGGRQF for further details. */

/*  Z       (output) REAL array, dimension (LDB,P) */
/*          The P-by-P unitary matrix Z. */

/*  T       (workspace) COMPLEX array, dimension (LDB,max(P,N)) */

/*  BWK     (workspace) COMPLEX array, dimension (LDB,N) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF, Z and T. */
/*          LDB >= max(P,N). */

/*  TAUB    (output) COMPLEX array, dimension (min(P,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGGRQF. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

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

    clacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);
    clacpy_("Full", p, n, &b[b_offset], ldb, &bf[bf_offset], ldb);

/* Computing MAX */
    r__1 = clange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);
/* Computing MAX */
    r__1 = clange_("1", p, n, &b[b_offset], ldb, &rwork[1]);
    bnorm = dmax(r__1,unfl);

/*     Factorize the matrices A and B in the arrays AF and BF. */

    cggrqf_(m, p, n, &af[af_offset], lda, &taua[1], &bf[bf_offset], ldb, &
	    taub[1], &work[1], lwork, &info);

/*     Generate the N-by-N matrix Q */

    claset_("Full", n, n, &c_b3, &c_b3, &q[q_offset], lda);
    if (*m <= *n) {
	if (*m > 0 && *m < *n) {
	    i__1 = *n - *m;
	    clacpy_("Full", m, &i__1, &af[af_offset], lda, &q[*n - *m + 1 + 
		    q_dim1], lda);
	}
	if (*m > 1) {
	    i__1 = *m - 1;
	    i__2 = *m - 1;
	    clacpy_("Lower", &i__1, &i__2, &af[(*n - *m + 1) * af_dim1 + 2], 
		    lda, &q[*n - *m + 2 + (*n - *m + 1) * q_dim1], lda);
	}
    } else {
	if (*n > 1) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    clacpy_("Lower", &i__1, &i__2, &af[*m - *n + 2 + af_dim1], lda, &
		    q[q_dim1 + 2], lda);
	}
    }
    i__1 = min(*m,*n);
    cungrq_(n, n, &i__1, &q[q_offset], lda, &taua[1], &work[1], lwork, &info);

/*     Generate the P-by-P matrix Z */

    claset_("Full", p, p, &c_b3, &c_b3, &z__[z_offset], ldb);
    if (*p > 1) {
	i__1 = *p - 1;
	clacpy_("Lower", &i__1, n, &bf[bf_dim1 + 2], ldb, &z__[z_dim1 + 2], 
		ldb);
    }
    i__1 = min(*p,*n);
    cungqr_(p, p, &i__1, &z__[z_offset], ldb, &taub[1], &work[1], lwork, &
	    info);

/*     Copy R */

    claset_("Full", m, n, &c_b1, &c_b1, &r__[r_offset], lda);
    if (*m <= *n) {
	clacpy_("Upper", m, m, &af[(*n - *m + 1) * af_dim1 + 1], lda, &r__[(*
		n - *m + 1) * r_dim1 + 1], lda);
    } else {
	i__1 = *m - *n;
	clacpy_("Full", &i__1, n, &af[af_offset], lda, &r__[r_offset], lda);
	clacpy_("Upper", n, n, &af[*m - *n + 1 + af_dim1], lda, &r__[*m - *n 
		+ 1 + r_dim1], lda);
    }

/*     Copy T */

    claset_("Full", p, n, &c_b1, &c_b1, &t[t_offset], ldb);
    clacpy_("Upper", p, n, &bf[bf_offset], ldb, &t[t_offset], ldb);

/*     Compute R - A*Q' */

    q__1.r = -1.f, q__1.i = -0.f;
    cgemm_("No transpose", "Conjugate transpose", m, n, n, &q__1, &a[a_offset]
, lda, &q[q_offset], lda, &c_b2, &r__[r_offset], lda);

/*     Compute norm( R - A*Q' ) / ( MAX(M,N)*norm(A)*ULP ) . */

    resid = clange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*m);
	result[1] = resid / (real) max(i__1,*n) / anorm / ulp;
    } else {
	result[1] = 0.f;
    }

/*     Compute T*Q - Z'*B */

    cgemm_("Conjugate transpose", "No transpose", p, n, p, &c_b2, &z__[
	    z_offset], ldb, &b[b_offset], ldb, &c_b1, &bwk[bwk_offset], ldb);
    q__1.r = -1.f, q__1.i = -0.f;
    cgemm_("No transpose", "No transpose", p, n, n, &c_b2, &t[t_offset], ldb, 
	    &q[q_offset], lda, &q__1, &bwk[bwk_offset], ldb);

/*     Compute norm( T*Q - Z'*B ) / ( MAX(P,N)*norm(A)*ULP ) . */

    resid = clange_("1", p, n, &bwk[bwk_offset], ldb, &rwork[1]);
    if (bnorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*p);
	result[2] = resid / (real) max(i__1,*m) / bnorm / ulp;
    } else {
	result[2] = 0.f;
    }

/*     Compute I - Q*Q' */

    claset_("Full", n, n, &c_b1, &c_b2, &r__[r_offset], lda);
    cherk_("Upper", "No Transpose", n, n, &c_b34, &q[q_offset], lda, &c_b35, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( N * ULP ) . */

    resid = clanhe_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);
    result[3] = resid / (real) max(1,*n) / ulp;

/*     Compute I - Z'*Z */

    claset_("Full", p, p, &c_b1, &c_b2, &t[t_offset], ldb);
    cherk_("Upper", "Conjugate transpose", p, p, &c_b34, &z__[z_offset], ldb, 
	    &c_b35, &t[t_offset], ldb);

/*     Compute norm( I - Z'*Z ) / ( P*ULP ) . */

    resid = clanhe_("1", "Upper", p, &t[t_offset], ldb, &rwork[1]);
    result[4] = resid / (real) max(1,*p) / ulp;

    return 0;

/*     End of CGRQTS */

} /* cgrqts_ */
