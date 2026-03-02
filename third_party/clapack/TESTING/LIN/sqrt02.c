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

/* Subroutine */ int sqrt02_(integer *m, integer *n, integer *k, real *a, 
	real *af, real *q, real *r__, integer *lda, real *tau, real *work, 
	integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, i__1;

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
	    integer *, real *, real *, real *, integer *);
    extern doublereal slansy_(char *, char *, integer *, real *, integer *, 
	    real *);
    extern /* Subroutine */ int sorgqr_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SQRT02 tests SORGQR, which generates an m-by-n matrix Q with */
/*  orthonornmal columns that is defined as the product of k elementary */
/*  reflectors. */

/*  Given the QR factorization of an m-by-n matrix A, SQRT02 generates */
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

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The m-by-n matrix A which was factorized by SQRT01. */

/*  AF      (input) REAL array, dimension (LDA,N) */
/*          Details of the QR factorization of A, as returned by SGEQRF. */
/*          See SGEQRF for further details. */

/*  Q       (workspace) REAL array, dimension (LDA,N) */

/*  R       (workspace) REAL array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. LDA >= M. */

/*  TAU     (input) REAL array, dimension (N) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the QR factorization in AF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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

    slaset_("Full", m, n, &c_b4, &c_b4, &q[q_offset], lda);
    i__1 = *m - 1;
    slacpy_("Lower", &i__1, k, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);

/*     Generate the first n columns of the matrix Q */

    s_copy(srnamc_1.srnamt, "SORGQR", (ftnlen)6, (ftnlen)6);
    sorgqr_(m, n, k, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R(1:n,1:k) */

    slaset_("Full", n, k, &c_b9, &c_b9, &r__[r_offset], lda);
    slacpy_("Upper", n, k, &af[af_offset], lda, &r__[r_offset], lda);

/*     Compute R(1:n,1:k) - Q(1:m,1:n)' * A(1:m,1:k) */

    sgemm_("Transpose", "No transpose", n, k, m, &c_b14, &q[q_offset], lda, &
	    a[a_offset], lda, &c_b15, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = slange_("1", m, k, &a[a_offset], lda, &rwork[1]);
    resid = slange_("1", n, k, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q'*Q */

    slaset_("Full", n, n, &c_b9, &c_b15, &r__[r_offset], lda);
    ssyrk_("Upper", "Transpose", n, m, &c_b14, &q[q_offset], lda, &c_b15, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = slansy_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*m) / eps;

    return 0;

/*     End of SQRT02 */

} /* sqrt02_ */
