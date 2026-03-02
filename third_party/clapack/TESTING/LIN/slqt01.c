#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static real c_b6 = -1e10f;
static real c_b11 = 0.f;
static real c_b16 = -1.f;
static real c_b17 = 1.f;

/* Subroutine */ int slqt01_(integer *m, integer *n, real *a, real *af, real *
	q, real *l, integer *lda, real *tau, real *work, integer *lwork, real 
	*rwork, real *result)
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
    integer minmn;
    extern /* Subroutine */ int ssyrk_(char *, char *, integer *, integer *, 
	    real *, real *, integer *, real *, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int sgelqf_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *, integer *), slacpy_(char *, integer 
	    *, integer *, real *, integer *, real *, integer *), 
	    slaset_(char *, integer *, integer *, real *, real *, real *, 
	    integer *), sorglq_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *);
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

/*  SLQT01 tests SGELQF, which computes the LQ factorization of an m-by-n */
/*  matrix A, and partially tests SORGLQ which forms the n-by-n */
/*  orthogonal matrix Q. */

/*  SLQT01 compares L with A*Q', and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) REAL array, dimension (LDA,N) */
/*          Details of the LQ factorization of A, as returned by SGELQF. */
/*          See SGELQF for further details. */

/*  Q       (output) REAL array, dimension (LDA,N) */
/*          The n-by-n orthogonal matrix Q. */

/*  L       (workspace) REAL array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and L. */
/*          LDA >= max(M,N). */

/*  TAU     (output) REAL array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGELQF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) REAL array, dimension (max(M,N)) */

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
    minmn = min(*m,*n);
    eps = slamch_("Epsilon");

/*     Copy the matrix A to the array AF. */

    slacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "SGELQF", (ftnlen)6, (ftnlen)6);
    sgelqf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    slaset_("Full", n, n, &c_b6, &c_b6, &q[q_offset], lda);
    if (*n > 1) {
	i__1 = *n - 1;
	slacpy_("Upper", m, &i__1, &af[(af_dim1 << 1) + 1], lda, &q[(q_dim1 <<
		 1) + 1], lda);
    }

/*     Generate the n-by-n matrix Q */

    s_copy(srnamc_1.srnamt, "SORGLQ", (ftnlen)6, (ftnlen)6);
    sorglq_(n, n, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy L */

    slaset_("Full", m, n, &c_b11, &c_b11, &l[l_offset], lda);
    slacpy_("Lower", m, n, &af[af_offset], lda, &l[l_offset], lda);

/*     Compute L - A*Q' */

    sgemm_("No transpose", "Transpose", m, n, n, &c_b16, &a[a_offset], lda, &
	    q[q_offset], lda, &c_b17, &l[l_offset], lda);

/*     Compute norm( L - Q'*A ) / ( N * norm(A) * EPS ) . */

    anorm = slange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = slange_("1", m, n, &l[l_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*n) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q*Q' */

    slaset_("Full", n, n, &c_b11, &c_b17, &l[l_offset], lda);
    ssyrk_("Upper", "No transpose", n, n, &c_b16, &q[q_offset], lda, &c_b17, &
	    l[l_offset], lda);

/*     Compute norm( I - Q*Q' ) / ( N * EPS ) . */

    resid = slansy_("1", "Upper", n, &l[l_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*n) / eps;

    return 0;

/*     End of SLQT01 */

} /* slqt01_ */
