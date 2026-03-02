#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static doublecomplex c_b3 = {-1e10,0.};
static doublereal c_b34 = -1.;
static doublereal c_b35 = 1.;

/* Subroutine */ int zgqrts_(integer *n, integer *m, integer *p, 
	doublecomplex *a, doublecomplex *af, doublecomplex *q, doublecomplex *
	r__, integer *lda, doublecomplex *taua, doublecomplex *b, 
	doublecomplex *bf, doublecomplex *z__, doublecomplex *t, 
	doublecomplex *bwk, integer *ldb, doublecomplex *taub, doublecomplex *
	work, integer *lwork, doublereal *rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset, bwk_dim1, bwk_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, t_dim1, t_offset, z_dim1, z_offset, i__1, i__2;
    doublereal d__1;
    doublecomplex z__1;

    /* Local variables */
    doublereal ulp;
    integer info;
    doublereal unfl, resid, anorm, bnorm;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), zherk_(char *, char *, integer *, 
	    integer *, doublereal *, doublecomplex *, integer *, doublereal *, 
	     doublecomplex *, integer *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *), 
	    zlanhe_(char *, char *, integer *, doublecomplex *, integer *, 
	    doublereal *);
    extern /* Subroutine */ int zggqrf_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, integer *)
	    , zlacpy_(char *, integer *, integer *, doublecomplex *, integer *
, doublecomplex *, integer *), zlaset_(char *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *), zungqr_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, integer *), zungrq_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGQRTS tests ZGGQRF, which computes the GQR factorization of an */
/*  N-by-M matrix A and a N-by-P matrix B: A = Q*R and B = Q*T*Z. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices A and B.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of columns of the matrix B.  P >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,M) */
/*          The N-by-M matrix A. */

/*  AF      (output) COMPLEX*16 array, dimension (LDA,N) */
/*          Details of the GQR factorization of A and B, as returned */
/*          by ZGGQRF, see CGGQRF for further details. */

/*  Q       (output) COMPLEX*16 array, dimension (LDA,N) */
/*          The M-by-M unitary matrix Q. */

/*  R       (workspace) COMPLEX*16 array, dimension (LDA,MAX(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, R and Q. */
/*          LDA >= max(M,N). */

/*  TAUA    (output) COMPLEX*16 array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by ZGGQRF. */

/*  B       (input) COMPLEX*16 array, dimension (LDB,P) */
/*          On entry, the N-by-P matrix A. */

/*  BF      (output) COMPLEX*16 array, dimension (LDB,N) */
/*          Details of the GQR factorization of A and B, as returned */
/*          by ZGGQRF, see CGGQRF for further details. */

/*  Z       (output) COMPLEX*16 array, dimension (LDB,P) */
/*          The P-by-P unitary matrix Z. */

/*  T       (workspace) COMPLEX*16 array, dimension (LDB,max(P,N)) */

/*  BWK     (workspace) COMPLEX*16 array, dimension (LDB,N) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF, Z and T. */
/*          LDB >= max(P,N). */

/*  TAUB    (output) COMPLEX*16 array, dimension (min(P,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by DGGRQF. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK, LWORK >= max(N,M,P)**2. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (max(N,M,P)) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (4) */
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
    ulp = dlamch_("Precision");
    unfl = dlamch_("Safe minimum");

/*     Copy the matrix A to the array AF. */

    zlacpy_("Full", n, m, &a[a_offset], lda, &af[af_offset], lda);
    zlacpy_("Full", n, p, &b[b_offset], ldb, &bf[bf_offset], ldb);

/* Computing MAX */
    d__1 = zlange_("1", n, m, &a[a_offset], lda, &rwork[1]);
    anorm = max(d__1,unfl);
/* Computing MAX */
    d__1 = zlange_("1", n, p, &b[b_offset], ldb, &rwork[1]);
    bnorm = max(d__1,unfl);

/*     Factorize the matrices A and B in the arrays AF and BF. */

    zggqrf_(n, m, p, &af[af_offset], lda, &taua[1], &bf[bf_offset], ldb, &
	    taub[1], &work[1], lwork, &info);

/*     Generate the N-by-N matrix Q */

    zlaset_("Full", n, n, &c_b3, &c_b3, &q[q_offset], lda);
    i__1 = *n - 1;
    zlacpy_("Lower", &i__1, m, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);
    i__1 = min(*n,*m);
    zungqr_(n, n, &i__1, &q[q_offset], lda, &taua[1], &work[1], lwork, &info);

/*     Generate the P-by-P matrix Z */

    zlaset_("Full", p, p, &c_b3, &c_b3, &z__[z_offset], ldb);
    if (*n <= *p) {
	if (*n > 0 && *n < *p) {
	    i__1 = *p - *n;
	    zlacpy_("Full", n, &i__1, &bf[bf_offset], ldb, &z__[*p - *n + 1 + 
		    z_dim1], ldb);
	}
	if (*n > 1) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    zlacpy_("Lower", &i__1, &i__2, &bf[(*p - *n + 1) * bf_dim1 + 2], 
		    ldb, &z__[*p - *n + 2 + (*p - *n + 1) * z_dim1], ldb);
	}
    } else {
	if (*p > 1) {
	    i__1 = *p - 1;
	    i__2 = *p - 1;
	    zlacpy_("Lower", &i__1, &i__2, &bf[*n - *p + 2 + bf_dim1], ldb, &
		    z__[z_dim1 + 2], ldb);
	}
    }
    i__1 = min(*n,*p);
    zungrq_(p, p, &i__1, &z__[z_offset], ldb, &taub[1], &work[1], lwork, &
	    info);

/*     Copy R */

    zlaset_("Full", n, m, &c_b1, &c_b1, &r__[r_offset], lda);
    zlacpy_("Upper", n, m, &af[af_offset], lda, &r__[r_offset], lda);

/*     Copy T */

    zlaset_("Full", n, p, &c_b1, &c_b1, &t[t_offset], ldb);
    if (*n <= *p) {
	zlacpy_("Upper", n, n, &bf[(*p - *n + 1) * bf_dim1 + 1], ldb, &t[(*p 
		- *n + 1) * t_dim1 + 1], ldb);
    } else {
	i__1 = *n - *p;
	zlacpy_("Full", &i__1, p, &bf[bf_offset], ldb, &t[t_offset], ldb);
	zlacpy_("Upper", p, p, &bf[*n - *p + 1 + bf_dim1], ldb, &t[*n - *p + 
		1 + t_dim1], ldb);
    }

/*     Compute R - Q'*A */

    z__1.r = -1., z__1.i = -0.;
    zgemm_("Conjugate transpose", "No transpose", n, m, n, &z__1, &q[q_offset]
, lda, &a[a_offset], lda, &c_b2, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( MAX(M,N)*norm(A)*ULP ) . */

    resid = zlange_("1", n, m, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.) {
/* Computing MAX */
	i__1 = max(1,*m);
	result[1] = resid / (doublereal) max(i__1,*n) / anorm / ulp;
    } else {
	result[1] = 0.;
    }

/*     Compute T*Z - Q'*B */

    zgemm_("No Transpose", "No transpose", n, p, p, &c_b2, &t[t_offset], ldb, 
	    &z__[z_offset], ldb, &c_b1, &bwk[bwk_offset], ldb);
    z__1.r = -1., z__1.i = -0.;
    zgemm_("Conjugate transpose", "No transpose", n, p, n, &z__1, &q[q_offset]
, lda, &b[b_offset], ldb, &c_b2, &bwk[bwk_offset], ldb);

/*     Compute norm( T*Z - Q'*B ) / ( MAX(P,N)*norm(A)*ULP ) . */

    resid = zlange_("1", n, p, &bwk[bwk_offset], ldb, &rwork[1]);
    if (bnorm > 0.) {
/* Computing MAX */
	i__1 = max(1,*p);
	result[2] = resid / (doublereal) max(i__1,*n) / bnorm / ulp;
    } else {
	result[2] = 0.;
    }

/*     Compute I - Q'*Q */

    zlaset_("Full", n, n, &c_b1, &c_b2, &r__[r_offset], lda);
    zherk_("Upper", "Conjugate transpose", n, n, &c_b34, &q[q_offset], lda, &
	    c_b35, &r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( N * ULP ) . */

    resid = zlanhe_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);
    result[3] = resid / (doublereal) max(1,*n) / ulp;

/*     Compute I - Z'*Z */

    zlaset_("Full", p, p, &c_b1, &c_b2, &t[t_offset], ldb);
    zherk_("Upper", "Conjugate transpose", p, p, &c_b34, &z__[z_offset], ldb, 
	    &c_b35, &t[t_offset], ldb);

/*     Compute norm( I - Z'*Z ) / ( P*ULP ) . */

    resid = zlanhe_("1", "Upper", p, &t[t_offset], ldb, &rwork[1]);
    result[4] = resid / (doublereal) max(1,*p) / ulp;

    return 0;

/*     End of ZGQRTS */

} /* zgqrts_ */
