#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublereal c_b6 = -1e10;
static doublereal c_b11 = 0.;
static doublereal c_b16 = -1.;
static doublereal c_b17 = 1.;

/* Subroutine */ int dqrt01_(integer *m, integer *n, doublereal *a, 
	doublereal *af, doublereal *q, doublereal *r__, integer *lda, 
	doublereal *tau, doublereal *work, integer *lwork, doublereal *rwork, 
	doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal eps;
    integer info;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    doublereal resid, anorm;
    integer minmn;
    extern /* Subroutine */ int dsyrk_(char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dgeqrf_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, integer *), 
	    dlacpy_(char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *), dlaset_(char *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *);
    extern doublereal dlansy_(char *, char *, integer *, doublereal *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int dorgqr_(integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DQRT01 tests DGEQRF, which computes the QR factorization of an m-by-n */
/*  matrix A, and partially tests DORGQR which forms the m-by-m */
/*  orthogonal matrix Q. */

/*  DQRT01 compares R with Q'*A, and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          Details of the QR factorization of A, as returned by DGEQRF. */
/*          See DGEQRF for further details. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDA,M) */
/*          The m-by-m orthogonal matrix Q. */

/*  R       (workspace) DOUBLE PRECISION array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by DGEQRF. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
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
    eps = dlamch_("Epsilon");

/*     Copy the matrix A to the array AF. */

    dlacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "DGEQRF", (ftnlen)6, (ftnlen)6);
    dgeqrf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    dlaset_("Full", m, m, &c_b6, &c_b6, &q[q_offset], lda);
    i__1 = *m - 1;
    dlacpy_("Lower", &i__1, n, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);

/*     Generate the m-by-m matrix Q */

    s_copy(srnamc_1.srnamt, "DORGQR", (ftnlen)6, (ftnlen)6);
    dorgqr_(m, m, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R */

    dlaset_("Full", m, n, &c_b11, &c_b11, &r__[r_offset], lda);
    dlacpy_("Upper", m, n, &af[af_offset], lda, &r__[r_offset], lda);

/*     Compute R - Q'*A */

    dgemm_("Transpose", "No transpose", m, n, m, &c_b16, &q[q_offset], lda, &
	    a[a_offset], lda, &c_b17, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = dlange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = dlange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.) {
	result[1] = resid / (doublereal) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.;
    }

/*     Compute I - Q'*Q */

    dlaset_("Full", m, m, &c_b11, &c_b17, &r__[r_offset], lda);
    dsyrk_("Upper", "Transpose", m, m, &c_b16, &q[q_offset], lda, &c_b17, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = dlansy_("1", "Upper", m, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (doublereal) max(1,*m) / eps;

    return 0;

/*     End of DQRT01 */

} /* dqrt01_ */
