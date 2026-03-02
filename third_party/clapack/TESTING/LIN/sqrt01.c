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

/* Subroutine */ int sqrt01_(integer *m, integer *n, real *a, real *af, real *
	q, real *r__, integer *lda, real *tau, real *work, integer *lwork, 
	real *rwork, real *result)
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
    integer minmn;
    extern /* Subroutine */ int ssyrk_(char *, char *, integer *, integer *, 
	    real *, real *, integer *, real *, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int sgeqrf_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *, integer *), slacpy_(char *, integer 
	    *, integer *, real *, integer *, real *, integer *), 
	    slaset_(char *, integer *, integer *, real *, real *, real *, 
	    integer *);
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

/*  SQRT01 tests SGEQRF, which computes the QR factorization of an m-by-n */
/*  matrix A, and partially tests SORGQR which forms the m-by-m */
/*  orthogonal matrix Q. */

/*  SQRT01 compares R with Q'*A, and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) REAL array, dimension (LDA,N) */
/*          Details of the QR factorization of A, as returned by SGEQRF. */
/*          See SGEQRF for further details. */

/*  Q       (output) REAL array, dimension (LDA,M) */
/*          The m-by-m orthogonal matrix Q. */

/*  R       (workspace) REAL array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  TAU     (output) REAL array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by SGEQRF. */

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
    minmn = min(*m,*n);
    eps = slamch_("Epsilon");

/*     Copy the matrix A to the array AF. */

    slacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "SGEQRF", (ftnlen)6, (ftnlen)6);
    sgeqrf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    slaset_("Full", m, m, &c_b6, &c_b6, &q[q_offset], lda);
    i__1 = *m - 1;
    slacpy_("Lower", &i__1, n, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);

/*     Generate the m-by-m matrix Q */

    s_copy(srnamc_1.srnamt, "SORGQR", (ftnlen)6, (ftnlen)6);
    sorgqr_(m, m, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R */

    slaset_("Full", m, n, &c_b11, &c_b11, &r__[r_offset], lda);
    slacpy_("Upper", m, n, &af[af_offset], lda, &r__[r_offset], lda);

/*     Compute R - Q'*A */

    sgemm_("Transpose", "No transpose", m, n, m, &c_b16, &q[q_offset], lda, &
	    a[a_offset], lda, &c_b17, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = slange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = slange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q'*Q */

    slaset_("Full", m, m, &c_b11, &c_b17, &r__[r_offset], lda);
    ssyrk_("Upper", "Transpose", m, m, &c_b16, &q[q_offset], lda, &c_b17, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = slansy_("1", "Upper", m, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*m) / eps;

    return 0;

/*     End of SQRT01 */

} /* sqrt01_ */
