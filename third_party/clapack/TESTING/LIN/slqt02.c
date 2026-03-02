#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static real c_b4 = -1e10f;
static real c_b9 = 0.f;
static real c_b14 = -1.f;
static real c_b15 = 1.f;

/* Subroutine */ int slqt02_(integer *m, integer *n, integer *k, real *a, 
	real *af, real *q, real *l, integer *lda, real *tau, real *work, 
	integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, l_dim1, l_offset, q_dim1, 
	    q_offset, i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real eps;
    integer info;
    real resid;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm;
    extern /* Subroutine */ int ssyrk_(char *, char *, integer *, integer *, 
	    real *, real *, integer *, real *, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *), sorglq_(
	    integer *, integer *, integer *, real *, integer *, real *, real *
, integer *, integer *);
    extern doublereal slansy_(char *, char *, integer *, real *, integer *, 
	    real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLQT02 tests SORGLQ, which generates an m-by-n matrix Q with */
/*  orthonornmal rows that is defined as the product of k elementary */
/*  reflectors. */

/*  Given the LQ factorization of an m-by-n matrix A, SLQT02 generates */
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

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The m-by-n matrix A which was factorized by SLQT01. */

/*  AF      (input) REAL array, dimension (LDA,N) */
/*          Details of the LQ factorization of A, as returned by SGELQF. */
/*          See SGELQF for further details. */

/*  Q       (workspace) REAL array, dimension (LDA,N) */

/*  L       (workspace) REAL array, dimension (LDA,M) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and L. LDA >= N. */

/*  TAU     (input) REAL array, dimension (M) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the LQ factorization in AF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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

    slaset_("Full", m, n, &c_b4, &c_b4, &q[q_offset], lda);
    i__1 = *n - 1;
    slacpy_("Upper", k, &i__1, &af[(af_dim1 << 1) + 1], lda, &q[(q_dim1 << 1) 
	    + 1], lda);

/*     Generate the first n columns of the matrix Q */

    s_copy(srnamc_1.srnamt, "SORGLQ", (ftnlen)6, (ftnlen)6);
    sorglq_(m, n, k, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy L(1:k,1:m) */

    slaset_("Full", k, m, &c_b9, &c_b9, &l[l_offset], lda);
    slacpy_("Lower", k, m, &af[af_offset], lda, &l[l_offset], lda);

/*     Compute L(1:k,1:m) - A(1:k,1:n) * Q(1:m,1:n)' */

    sgemm_("No transpose", "Transpose", k, m, n, &c_b14, &a[a_offset], lda, &
	    q[q_offset], lda, &c_b15, &l[l_offset], lda);

/*     Compute norm( L - A*Q' ) / ( N * norm(A) * EPS ) . */

    anorm = slange_("1", k, n, &a[a_offset], lda, &rwork[1]);
    resid = slange_("1", k, m, &l[l_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*n) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q*Q' */

    slaset_("Full", m, m, &c_b9, &c_b15, &l[l_offset], lda);
    ssyrk_("Upper", "No transpose", m, n, &c_b14, &q[q_offset], lda, &c_b15, &
	    l[l_offset], lda);

/*     Compute norm( I - Q*Q' ) / ( N * EPS ) . */

    resid = slansy_("1", "Upper", m, &l[l_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*n) / eps;

    return 0;

/*     End of SLQT02 */

} /* slqt02_ */
