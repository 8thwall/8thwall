#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublecomplex c_b1 = {-1e10,-1e10};
static doublecomplex c_b9 = {0.,0.};
static doublecomplex c_b14 = {-1.,0.};
static doublecomplex c_b15 = {1.,0.};
static doublereal c_b23 = -1.;
static doublereal c_b24 = 1.;

/* Subroutine */ int zrqt02_(integer *m, integer *n, integer *k, 
	doublecomplex *a, doublecomplex *af, doublecomplex *q, doublecomplex *
	r__, integer *lda, doublecomplex *tau, doublecomplex *work, integer *
	lwork, doublereal *rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, i__1, i__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal eps;
    integer info;
    doublereal resid, anorm;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), zherk_(char *, char *, integer *, 
	    integer *, doublereal *, doublecomplex *, integer *, doublereal *, 
	     doublecomplex *, integer *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), 
	    zlaset_(char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, integer *);
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int zungrq_(integer *, integer *, integer *, 
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

/*  ZRQT02 tests ZUNGRQ, which generates an m-by-n matrix Q with */
/*  orthonornmal rows that is defined as the product of k elementary */
/*  reflectors. */

/*  Given the RQ factorization of an m-by-n matrix A, ZRQT02 generates */
/*  the orthogonal matrix Q defined by the factorization of the last k */
/*  rows of A; it compares R(m-k+1:m,n-m+1:n) with */
/*  A(m-k+1:m,1:n)*Q(n-m+1:n,1:n)', and checks that the rows of Q are */
/*  orthonormal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix Q to be generated.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix Q to be generated. */
/*          N >= M >= 0. */

/*  K       (input) INTEGER */
/*          The number of elementary reflectors whose product defines the */
/*          matrix Q. M >= K >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The m-by-n matrix A which was factorized by ZRQT01. */

/*  AF      (input) COMPLEX*16 array, dimension (LDA,N) */
/*          Details of the RQ factorization of A, as returned by ZGERQF. */
/*          See ZGERQF for further details. */

/*  Q       (workspace) COMPLEX*16 array, dimension (LDA,N) */

/*  R       (workspace) COMPLEX*16 array, dimension (LDA,M) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and L. LDA >= N. */

/*  TAU     (input) COMPLEX*16 array, dimension (M) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the RQ factorization in AF. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
/*          The test ratios: */
/*          RESULT(1) = norm( R - A*Q' ) / ( N * norm(A) * EPS ) */
/*          RESULT(2) = norm( I - Q*Q' ) / ( N * EPS ) */

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
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

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
    --tau;
    --work;
    --rwork;
    --result;

    /* Function Body */
    if (*m == 0 || *n == 0 || *k == 0) {
	result[1] = 0.;
	result[2] = 0.;
	return 0;
    }

    eps = dlamch_("Epsilon");

/*     Copy the last k rows of the factorization to the array Q */

    zlaset_("Full", m, n, &c_b1, &c_b1, &q[q_offset], lda);
    if (*k < *n) {
	i__1 = *n - *k;
	zlacpy_("Full", k, &i__1, &af[*m - *k + 1 + af_dim1], lda, &q[*m - *k 
		+ 1 + q_dim1], lda);
    }
    if (*k > 1) {
	i__1 = *k - 1;
	i__2 = *k - 1;
	zlacpy_("Lower", &i__1, &i__2, &af[*m - *k + 2 + (*n - *k + 1) * 
		af_dim1], lda, &q[*m - *k + 2 + (*n - *k + 1) * q_dim1], lda);
    }

/*     Generate the last n rows of the matrix Q */

    s_copy(srnamc_1.srnamt, "ZUNGRQ", (ftnlen)6, (ftnlen)6);
    zungrq_(m, n, k, &q[q_offset], lda, &tau[*m - *k + 1], &work[1], lwork, &
	    info);

/*     Copy R(m-k+1:m,n-m+1:n) */

    zlaset_("Full", k, m, &c_b9, &c_b9, &r__[*m - *k + 1 + (*n - *m + 1) * 
	    r_dim1], lda);
    zlacpy_("Upper", k, k, &af[*m - *k + 1 + (*n - *k + 1) * af_dim1], lda, &
	    r__[*m - *k + 1 + (*n - *k + 1) * r_dim1], lda);

/*     Compute R(m-k+1:m,n-m+1:n) - A(m-k+1:m,1:n) * Q(n-m+1:n,1:n)' */

    zgemm_("No transpose", "Conjugate transpose", k, m, n, &c_b14, &a[*m - *k 
	    + 1 + a_dim1], lda, &q[q_offset], lda, &c_b15, &r__[*m - *k + 1 + 
	    (*n - *m + 1) * r_dim1], lda);

/*     Compute norm( R - A*Q' ) / ( N * norm(A) * EPS ) . */

    anorm = zlange_("1", k, n, &a[*m - *k + 1 + a_dim1], lda, &rwork[1]);
    resid = zlange_("1", k, m, &r__[*m - *k + 1 + (*n - *m + 1) * r_dim1], 
	    lda, &rwork[1]);
    if (anorm > 0.) {
	result[1] = resid / (doublereal) max(1,*n) / anorm / eps;
    } else {
	result[1] = 0.;
    }

/*     Compute I - Q*Q' */

    zlaset_("Full", m, m, &c_b9, &c_b15, &r__[r_offset], lda);
    zherk_("Upper", "No transpose", m, n, &c_b23, &q[q_offset], lda, &c_b24, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q*Q' ) / ( N * EPS ) . */

    resid = zlansy_("1", "Upper", m, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (doublereal) max(1,*n) / eps;

    return 0;

/*     End of ZRQT02 */

} /* zrqt02_ */
