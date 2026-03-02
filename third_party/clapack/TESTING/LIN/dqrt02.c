#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublereal c_b4 = -1e10;
static doublereal c_b9 = 0.;
static doublereal c_b14 = -1.;
static doublereal c_b15 = 1.;

/* Subroutine */ int dqrt02_(integer *m, integer *n, integer *k, doublereal *
	a, doublereal *af, doublereal *q, doublereal *r__, integer *lda, 
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
    extern /* Subroutine */ int dsyrk_(char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlaset_(char *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *);
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

/*  DQRT02 tests DORGQR, which generates an m-by-n matrix Q with */
/*  orthonornmal columns that is defined as the product of k elementary */
/*  reflectors. */

/*  Given the QR factorization of an m-by-n matrix A, DQRT02 generates */
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

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m-by-n matrix A which was factorized by DQRT01. */

/*  AF      (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          Details of the QR factorization of A, as returned by DGEQRF. */
/*          See DGEQRF for further details. */

/*  Q       (workspace) DOUBLE PRECISION array, dimension (LDA,N) */

/*  R       (workspace) DOUBLE PRECISION array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. LDA >= M. */

/*  TAU     (input) DOUBLE PRECISION array, dimension (N) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the QR factorization in AF. */

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
    eps = dlamch_("Epsilon");

/*     Copy the first k columns of the factorization to the array Q */

    dlaset_("Full", m, n, &c_b4, &c_b4, &q[q_offset], lda);
    i__1 = *m - 1;
    dlacpy_("Lower", &i__1, k, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);

/*     Generate the first n columns of the matrix Q */

    s_copy(srnamc_1.srnamt, "DORGQR", (ftnlen)6, (ftnlen)6);
    dorgqr_(m, n, k, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R(1:n,1:k) */

    dlaset_("Full", n, k, &c_b9, &c_b9, &r__[r_offset], lda);
    dlacpy_("Upper", n, k, &af[af_offset], lda, &r__[r_offset], lda);

/*     Compute R(1:n,1:k) - Q(1:m,1:n)' * A(1:m,1:k) */

    dgemm_("Transpose", "No transpose", n, k, m, &c_b14, &q[q_offset], lda, &
	    a[a_offset], lda, &c_b15, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = dlange_("1", m, k, &a[a_offset], lda, &rwork[1]);
    resid = dlange_("1", n, k, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.) {
	result[1] = resid / (doublereal) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.;
    }

/*     Compute I - Q'*Q */

    dlaset_("Full", n, n, &c_b9, &c_b15, &r__[r_offset], lda);
    dsyrk_("Upper", "Transpose", n, m, &c_b14, &q[q_offset], lda, &c_b15, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = dlansy_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (doublereal) max(1,*m) / eps;

    return 0;

/*     End of DQRT02 */

} /* dqrt02_ */
