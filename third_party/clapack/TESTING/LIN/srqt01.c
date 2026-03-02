#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static real c_b6 = -1e10f;
static real c_b13 = 0.f;
static real c_b20 = -1.f;
static real c_b21 = 1.f;

/* Subroutine */ int srqt01_(integer *m, integer *n, real *a, real *af, real *
	q, real *r__, integer *lda, real *tau, real *work, integer *lwork, 
	real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, i__1, i__2;

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
    integer minmn;
    extern /* Subroutine */ int ssyrk_(char *, char *, integer *, integer *, 
	    real *, real *, integer *, real *, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int sgerqf_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *, integer *), slacpy_(char *, integer 
	    *, integer *, real *, integer *, real *, integer *), 
	    slaset_(char *, integer *, integer *, real *, real *, real *, 
	    integer *);
    extern doublereal slansy_(char *, char *, integer *, real *, integer *, 
	    real *);
    extern /* Subroutine */ int sorgrq_(integer *, integer *, integer *, real 
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

/*  SRQT01 tests SGERQF, which computes the RQ factorization of an m-by-n */
/*  matrix A, and partially tests SORGRQ which forms the n-by-n */
/*  orthogonal matrix Q. */

/*  SRQT01 compares R with A*Q', and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) REAL array, dimension (LDA,N) */
/*          Details of the RQ factorization of A, as returned by SGERQF. */
/*          See SGERQF for further details. */

/*  Q       (output) REAL array, dimension (LDA,N) */
/*          The n-by-n orthogonal matrix Q. */

/*  R       (workspace) REAL array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and L. */
/*          LDA >= max(M,N). */

/*  TAU     (output) REAL array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGERQF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) REAL array, dimension (max(M,N)) */

/*  RESULT  (output) REAL array, dimension (2) */
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
    minmn = min(*m,*n);
    eps = slamch_("Epsilon");

/*     Copy the matrix A to the array AF. */

    slacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "SGERQF", (ftnlen)6, (ftnlen)6);
    sgerqf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    slaset_("Full", n, n, &c_b6, &c_b6, &q[q_offset], lda);
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

/*     Generate the n-by-n matrix Q */

    s_copy(srnamc_1.srnamt, "SORGRQ", (ftnlen)6, (ftnlen)6);
    sorgrq_(n, n, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R */

    slaset_("Full", m, n, &c_b13, &c_b13, &r__[r_offset], lda);
    if (*m <= *n) {
	if (*m > 0) {
	    slacpy_("Upper", m, m, &af[(*n - *m + 1) * af_dim1 + 1], lda, &
		    r__[(*n - *m + 1) * r_dim1 + 1], lda);
	}
    } else {
	if (*m > *n && *n > 0) {
	    i__1 = *m - *n;
	    slacpy_("Full", &i__1, n, &af[af_offset], lda, &r__[r_offset], 
		    lda);
	}
	if (*n > 0) {
	    slacpy_("Upper", n, n, &af[*m - *n + 1 + af_dim1], lda, &r__[*m - 
		    *n + 1 + r_dim1], lda);
	}
    }

/*     Compute R - A*Q' */

    sgemm_("No transpose", "Transpose", m, n, n, &c_b20, &a[a_offset], lda, &
	    q[q_offset], lda, &c_b21, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( N * norm(A) * EPS ) . */

    anorm = slange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = slange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*n) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q*Q' */

    slaset_("Full", n, n, &c_b13, &c_b21, &r__[r_offset], lda);
    ssyrk_("Upper", "No transpose", n, n, &c_b20, &q[q_offset], lda, &c_b21, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q*Q' ) / ( N * EPS ) . */

    resid = slansy_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*n) / eps;

    return 0;

/*     End of SRQT01 */

} /* srqt01_ */
