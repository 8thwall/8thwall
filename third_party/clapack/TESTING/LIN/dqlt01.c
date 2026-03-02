#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublereal c_b6 = -1e10;
static doublereal c_b13 = 0.;
static doublereal c_b20 = -1.;
static doublereal c_b21 = 1.;

/* Subroutine */ int dqlt01_(integer *m, integer *n, doublereal *a, 
	doublereal *af, doublereal *q, doublereal *l, integer *lda, 
	doublereal *tau, doublereal *work, integer *lwork, doublereal *rwork, 
	doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, l_dim1, l_offset, q_dim1, 
	    q_offset, i__1, i__2;

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
    extern /* Subroutine */ int dgeqlf_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, integer *), 
	    dlacpy_(char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *), dlaset_(char *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *), dorgql_(integer *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, integer *);
    extern doublereal dlansy_(char *, char *, integer *, doublereal *, 
	    integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DQLT01 tests DGEQLF, which computes the QL factorization of an m-by-n */
/*  matrix A, and partially tests DORGQL which forms the m-by-m */
/*  orthogonal matrix Q. */

/*  DQLT01 compares L with Q'*A, and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          Details of the QL factorization of A, as returned by DGEQLF. */
/*          See DGEQLF for further details. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDA,M) */
/*          The m-by-m orthogonal matrix Q. */

/*  L       (workspace) DOUBLE PRECISION array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by DGEQLF. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
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
    eps = dlamch_("Epsilon");

/*     Copy the matrix A to the array AF. */

    dlacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "DGEQLF", (ftnlen)6, (ftnlen)6);
    dgeqlf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    dlaset_("Full", m, m, &c_b6, &c_b6, &q[q_offset], lda);
    if (*m >= *n) {
	if (*n < *m && *n > 0) {
	    i__1 = *m - *n;
	    dlacpy_("Full", &i__1, n, &af[af_offset], lda, &q[(*m - *n + 1) * 
		    q_dim1 + 1], lda);
	}
	if (*n > 1) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    dlacpy_("Upper", &i__1, &i__2, &af[*m - *n + 1 + (af_dim1 << 1)], 
		    lda, &q[*m - *n + 1 + (*m - *n + 2) * q_dim1], lda);
	}
    } else {
	if (*m > 1) {
	    i__1 = *m - 1;
	    i__2 = *m - 1;
	    dlacpy_("Upper", &i__1, &i__2, &af[(*n - *m + 2) * af_dim1 + 1], 
		    lda, &q[(q_dim1 << 1) + 1], lda);
	}
    }

/*     Generate the m-by-m matrix Q */

    s_copy(srnamc_1.srnamt, "DORGQL", (ftnlen)6, (ftnlen)6);
    dorgql_(m, m, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy L */

    dlaset_("Full", m, n, &c_b13, &c_b13, &l[l_offset], lda);
    if (*m >= *n) {
	if (*n > 0) {
	    dlacpy_("Lower", n, n, &af[*m - *n + 1 + af_dim1], lda, &l[*m - *
		    n + 1 + l_dim1], lda);
	}
    } else {
	if (*n > *m && *m > 0) {
	    i__1 = *n - *m;
	    dlacpy_("Full", m, &i__1, &af[af_offset], lda, &l[l_offset], lda);
	}
	if (*m > 0) {
	    dlacpy_("Lower", m, m, &af[(*n - *m + 1) * af_dim1 + 1], lda, &l[(
		    *n - *m + 1) * l_dim1 + 1], lda);
	}
    }

/*     Compute L - Q'*A */

    dgemm_("Transpose", "No transpose", m, n, m, &c_b20, &q[q_offset], lda, &
	    a[a_offset], lda, &c_b21, &l[l_offset], lda);

/*     Compute norm( L - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = dlange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = dlange_("1", m, n, &l[l_offset], lda, &rwork[1]);
    if (anorm > 0.) {
	result[1] = resid / (doublereal) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.;
    }

/*     Compute I - Q'*Q */

    dlaset_("Full", m, m, &c_b13, &c_b21, &l[l_offset], lda);
    dsyrk_("Upper", "Transpose", m, m, &c_b20, &q[q_offset], lda, &c_b21, &l[
	    l_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = dlansy_("1", "Upper", m, &l[l_offset], lda, &rwork[1]);

    result[2] = resid / (doublereal) max(1,*m) / eps;

    return 0;

/*     End of DQLT01 */

} /* dqlt01_ */
