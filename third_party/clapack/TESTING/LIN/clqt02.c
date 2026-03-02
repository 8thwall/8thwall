#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static complex c_b1 = {-1e10f,-1e10f};
static complex c_b8 = {0.f,0.f};
static complex c_b13 = {-1.f,0.f};
static complex c_b14 = {1.f,0.f};
static real c_b22 = -1.f;
static real c_b23 = 1.f;

/* Subroutine */ int clqt02_(integer *m, integer *n, integer *k, complex *a, 
	complex *af, complex *q, complex *l, integer *lda, complex *tau, 
	complex *work, integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, l_dim1, l_offset, q_dim1, 
	    q_offset, i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real eps;
    integer info;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *), cherk_(char *, 
	    char *, integer *, integer *, real *, complex *, integer *, real *
, complex *, integer *);
    real resid, anorm;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *);
    extern doublereal clansy_(char *, char *, integer *, complex *, integer *, 
	     real *);
    extern /* Subroutine */ int cunglq_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLQT02 tests CUNGLQ, which generates an m-by-n matrix Q with */
/*  orthonornmal rows that is defined as the product of k elementary */
/*  reflectors. */

/*  Given the LQ factorization of an m-by-n matrix A, CLQT02 generates */
/*  the orthogonal matrix Q defined by the factorization of the first k */
/*  rows of A; it compares L(1:k,1:m) with A(1:k,1:n)*Q(1:m,1:n)', and */
/*  checks that the rows of Q are orthonormal. */

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

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The m-by-n matrix A which was factorized by CLQT01. */

/*  AF      (input) COMPLEX array, dimension (LDA,N) */
/*          Details of the LQ factorization of A, as returned by CGELQF. */
/*          See CGELQF for further details. */

/*  Q       (workspace) COMPLEX array, dimension (LDA,N) */

/*  L       (workspace) COMPLEX array, dimension (LDA,M) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and L. LDA >= N. */

/*  TAU     (input) COMPLEX array, dimension (M) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the LQ factorization in AF. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESULT  (output) REAL array, dimension (2) */
/*          The test ratios: */
/*          RESULT(1) = norm( L - A*Q' ) / ( N * norm(A) * EPS ) */
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

    /* Parameter adjustments */
    l_dim1 = *lda;
    l_offset = 1 + l_dim1;
    l -= l_offset;
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
    eps = slamch_("Epsilon");

/*     Copy the first k rows of the factorization to the array Q */

    claset_("Full", m, n, &c_b1, &c_b1, &q[q_offset], lda);
    i__1 = *n - 1;
    clacpy_("Upper", k, &i__1, &af[(af_dim1 << 1) + 1], lda, &q[(q_dim1 << 1) 
	    + 1], lda);

/*     Generate the first n columns of the matrix Q */

    s_copy(srnamc_1.srnamt, "CUNGLQ", (ftnlen)6, (ftnlen)6);
    cunglq_(m, n, k, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy L(1:k,1:m) */

    claset_("Full", k, m, &c_b8, &c_b8, &l[l_offset], lda);
    clacpy_("Lower", k, m, &af[af_offset], lda, &l[l_offset], lda);

/*     Compute L(1:k,1:m) - A(1:k,1:n) * Q(1:m,1:n)' */

    cgemm_("No transpose", "Conjugate transpose", k, m, n, &c_b13, &a[
	    a_offset], lda, &q[q_offset], lda, &c_b14, &l[l_offset], lda);

/*     Compute norm( L - A*Q' ) / ( N * norm(A) * EPS ) . */

    anorm = clange_("1", k, n, &a[a_offset], lda, &rwork[1]);
    resid = clange_("1", k, m, &l[l_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*n) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q*Q' */

    claset_("Full", m, m, &c_b8, &c_b14, &l[l_offset], lda);
    cherk_("Upper", "No transpose", m, n, &c_b22, &q[q_offset], lda, &c_b23, &
	    l[l_offset], lda);

/*     Compute norm( I - Q*Q' ) / ( N * EPS ) . */

    resid = clansy_("1", "Upper", m, &l[l_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*n) / eps;

    return 0;

/*     End of CLQT02 */

} /* clqt02_ */
