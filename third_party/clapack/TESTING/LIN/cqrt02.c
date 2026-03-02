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

/* Subroutine */ int cqrt02_(integer *m, integer *n, integer *k, complex *a, 
	complex *af, complex *q, complex *r__, integer *lda, complex *tau, 
	complex *work, integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, i__1;

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
    extern /* Subroutine */ int cungqr_(integer *, integer *, integer *, 
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

/*  CQRT02 tests CUNGQR, which generates an m-by-n matrix Q with */
/*  orthonornmal columns that is defined as the product of k elementary */
/*  reflectors. */

/*  Given the QR factorization of an m-by-n matrix A, CQRT02 generates */
/*  the orthogonal matrix Q defined by the factorization of the first k */
/*  columns of A; it compares R(1:n,1:k) with Q(1:m,1:n)'*A(1:m,1:k), */
/*  and checks that the columns of Q are orthonormal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix Q to be generated.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix Q to be generated. */
/*          M >= N >= 0. */

/*  K       (input) INTEGER */
/*          The number of elementary reflectors whose product defines the */
/*          matrix Q. N >= K >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The m-by-n matrix A which was factorized by CQRT01. */

/*  AF      (input) COMPLEX array, dimension (LDA,N) */
/*          Details of the QR factorization of A, as returned by CGEQRF. */
/*          See CGEQRF for further details. */

/*  Q       (workspace) COMPLEX array, dimension (LDA,N) */

/*  R       (workspace) COMPLEX array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. LDA >= M. */

/*  TAU     (input) COMPLEX array, dimension (N) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the QR factorization in AF. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESULT  (output) REAL array, dimension (2) */
/*          The test ratios: */
/*          RESULT(1) = norm( R - Q'*A ) / ( M * norm(A) * EPS ) */
/*          RESULT(2) = norm( I - Q'*Q ) / ( M * EPS ) */

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
    eps = slamch_("Epsilon");

/*     Copy the first k columns of the factorization to the array Q */

    claset_("Full", m, n, &c_b1, &c_b1, &q[q_offset], lda);
    i__1 = *m - 1;
    clacpy_("Lower", &i__1, k, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);

/*     Generate the first n columns of the matrix Q */

    s_copy(srnamc_1.srnamt, "CUNGQR", (ftnlen)6, (ftnlen)6);
    cungqr_(m, n, k, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R(1:n,1:k) */

    claset_("Full", n, k, &c_b8, &c_b8, &r__[r_offset], lda);
    clacpy_("Upper", n, k, &af[af_offset], lda, &r__[r_offset], lda);

/*     Compute R(1:n,1:k) - Q(1:m,1:n)' * A(1:m,1:k) */

    cgemm_("Conjugate transpose", "No transpose", n, k, m, &c_b13, &q[
	    q_offset], lda, &a[a_offset], lda, &c_b14, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = clange_("1", m, k, &a[a_offset], lda, &rwork[1]);
    resid = clange_("1", n, k, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q'*Q */

    claset_("Full", n, n, &c_b8, &c_b14, &r__[r_offset], lda);
    cherk_("Upper", "Conjugate transpose", n, m, &c_b22, &q[q_offset], lda, &
	    c_b23, &r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = clansy_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*m) / eps;

    return 0;

/*     End of CQRT02 */

} /* cqrt02_ */
