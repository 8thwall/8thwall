#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static real c_b4 = -1e10f;
static real c_b10 = 0.f;
static real c_b15 = -1.f;
static real c_b16 = 1.f;

/* Subroutine */ int sqlt02_(integer *m, integer *n, integer *k, real *a, 
	real *af, real *q, real *l, integer *lda, real *tau, real *work, 
	integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, l_dim1, l_offset, q_dim1, 
	    q_offset, i__1, i__2;

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
	    integer *, real *, real *, real *, integer *), sorgql_(
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

/*  SQLT02 tests SORGQL, which generates an m-by-n matrix Q with */
/*  orthonornmal columns that is defined as the product of k elementary */
/*  reflectors. */

/*  Given the QL factorization of an m-by-n matrix A, SQLT02 generates */
/*  the orthogonal matrix Q defined by the factorization of the last k */
/*  columns of A; it compares L(m-n+1:m,n-k+1:n) with */
/*  Q(1:m,m-n+1:m)'*A(1:m,n-k+1:n), and checks that the columns of Q are */
/*  orthonormal. */

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

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The m-by-n matrix A which was factorized by SQLT01. */

/*  AF      (input) REAL array, dimension (LDA,N) */
/*          Details of the QL factorization of A, as returned by SGEQLF. */
/*          See SGEQLF for further details. */

/*  Q       (workspace) REAL array, dimension (LDA,N) */

/*  L       (workspace) REAL array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and L. LDA >= M. */

/*  TAU     (input) REAL array, dimension (N) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the QL factorization in AF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESULT  (output) REAL array, dimension (2) */
/*          The test ratios: */
/*          RESULT(1) = norm( L - Q'*A ) / ( M * norm(A) * EPS ) */
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

/*     Quick return if possible */

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
    if (*m == 0 || *n == 0 || *k == 0) {
	result[1] = 0.f;
	result[2] = 0.f;
	return 0;
    }

    eps = slamch_("Epsilon");

/*     Copy the last k columns of the factorization to the array Q */

    slaset_("Full", m, n, &c_b4, &c_b4, &q[q_offset], lda);
    if (*k < *m) {
	i__1 = *m - *k;
	slacpy_("Full", &i__1, k, &af[(*n - *k + 1) * af_dim1 + 1], lda, &q[(*
		n - *k + 1) * q_dim1 + 1], lda);
    }
    if (*k > 1) {
	i__1 = *k - 1;
	i__2 = *k - 1;
	slacpy_("Upper", &i__1, &i__2, &af[*m - *k + 1 + (*n - *k + 2) * 
		af_dim1], lda, &q[*m - *k + 1 + (*n - *k + 2) * q_dim1], lda);
    }

/*     Generate the last n columns of the matrix Q */

    s_copy(srnamc_1.srnamt, "SORGQL", (ftnlen)6, (ftnlen)6);
    sorgql_(m, n, k, &q[q_offset], lda, &tau[*n - *k + 1], &work[1], lwork, &
	    info);

/*     Copy L(m-n+1:m,n-k+1:n) */

    slaset_("Full", n, k, &c_b10, &c_b10, &l[*m - *n + 1 + (*n - *k + 1) * 
	    l_dim1], lda);
    slacpy_("Lower", k, k, &af[*m - *k + 1 + (*n - *k + 1) * af_dim1], lda, &
	    l[*m - *k + 1 + (*n - *k + 1) * l_dim1], lda);

/*     Compute L(m-n+1:m,n-k+1:n) - Q(1:m,m-n+1:m)' * A(1:m,n-k+1:n) */

    sgemm_("Transpose", "No transpose", n, k, m, &c_b15, &q[q_offset], lda, &
	    a[(*n - *k + 1) * a_dim1 + 1], lda, &c_b16, &l[*m - *n + 1 + (*n 
	    - *k + 1) * l_dim1], lda);

/*     Compute norm( L - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = slange_("1", m, k, &a[(*n - *k + 1) * a_dim1 + 1], lda, &rwork[1]);
    resid = slange_("1", n, k, &l[*m - *n + 1 + (*n - *k + 1) * l_dim1], lda, 
	    &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q'*Q */

    slaset_("Full", n, n, &c_b10, &c_b16, &l[l_offset], lda);
    ssyrk_("Upper", "Transpose", n, m, &c_b15, &q[q_offset], lda, &c_b16, &l[
	    l_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = slansy_("1", "Upper", n, &l[l_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*m) / eps;

    return 0;

/*     End of SQLT02 */

} /* sqlt02_ */
