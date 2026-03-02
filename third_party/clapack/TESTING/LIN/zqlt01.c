#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublecomplex c_b1 = {-1e10,-1e10};
static doublecomplex c_b12 = {0.,0.};
static doublecomplex c_b19 = {-1.,0.};
static doublecomplex c_b20 = {1.,0.};
static doublereal c_b28 = -1.;
static doublereal c_b29 = 1.;

/* Subroutine */ int zqlt01_(integer *m, integer *n, doublecomplex *a, 
	doublecomplex *af, doublecomplex *q, doublecomplex *l, integer *lda, 
	doublecomplex *tau, doublecomplex *work, integer *lwork, doublereal *
	rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, l_dim1, l_offset, q_dim1, 
	    q_offset, i__1, i__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal eps;
    integer info;
    doublereal resid, anorm;
    integer minmn;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), zherk_(char *, char *, integer *, 
	    integer *, doublereal *, doublecomplex *, integer *, doublereal *, 
	     doublecomplex *, integer *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    extern /* Subroutine */ int zgeqlf_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, doublecomplex *, integer *, integer *
), zlacpy_(char *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, integer *), zlaset_(char *, integer *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *);
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int zungql_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZQLT01 tests ZGEQLF, which computes the QL factorization of an m-by-n */
/*  matrix A, and partially tests ZUNGQL which forms the m-by-m */
/*  orthogonal matrix Q. */

/*  ZQLT01 compares L with Q'*A, and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) COMPLEX*16 array, dimension (LDA,N) */
/*          Details of the QL factorization of A, as returned by ZGEQLF. */
/*          See ZGEQLF for further details. */

/*  Q       (output) COMPLEX*16 array, dimension (LDA,M) */
/*          The m-by-m orthogonal matrix Q. */

/*  L       (workspace) COMPLEX*16 array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  TAU     (output) COMPLEX*16 array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by ZGEQLF. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

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

    zlacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "ZGEQLF", (ftnlen)6, (ftnlen)6);
    zgeqlf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    zlaset_("Full", m, m, &c_b1, &c_b1, &q[q_offset], lda);
    if (*m >= *n) {
	if (*n < *m && *n > 0) {
	    i__1 = *m - *n;
	    zlacpy_("Full", &i__1, n, &af[af_offset], lda, &q[(*m - *n + 1) * 
		    q_dim1 + 1], lda);
	}
	if (*n > 1) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    zlacpy_("Upper", &i__1, &i__2, &af[*m - *n + 1 + (af_dim1 << 1)], 
		    lda, &q[*m - *n + 1 + (*m - *n + 2) * q_dim1], lda);
	}
    } else {
	if (*m > 1) {
	    i__1 = *m - 1;
	    i__2 = *m - 1;
	    zlacpy_("Upper", &i__1, &i__2, &af[(*n - *m + 2) * af_dim1 + 1], 
		    lda, &q[(q_dim1 << 1) + 1], lda);
	}
    }

/*     Generate the m-by-m matrix Q */

    s_copy(srnamc_1.srnamt, "ZUNGQL", (ftnlen)6, (ftnlen)6);
    zungql_(m, m, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy L */

    zlaset_("Full", m, n, &c_b12, &c_b12, &l[l_offset], lda);
    if (*m >= *n) {
	if (*n > 0) {
	    zlacpy_("Lower", n, n, &af[*m - *n + 1 + af_dim1], lda, &l[*m - *
		    n + 1 + l_dim1], lda);
	}
    } else {
	if (*n > *m && *m > 0) {
	    i__1 = *n - *m;
	    zlacpy_("Full", m, &i__1, &af[af_offset], lda, &l[l_offset], lda);
	}
	if (*m > 0) {
	    zlacpy_("Lower", m, m, &af[(*n - *m + 1) * af_dim1 + 1], lda, &l[(
		    *n - *m + 1) * l_dim1 + 1], lda);
	}
    }

/*     Compute L - Q'*A */

    zgemm_("Conjugate transpose", "No transpose", m, n, m, &c_b19, &q[
	    q_offset], lda, &a[a_offset], lda, &c_b20, &l[l_offset], lda);

/*     Compute norm( L - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = zlange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = zlange_("1", m, n, &l[l_offset], lda, &rwork[1]);
    if (anorm > 0.) {
	result[1] = resid / (doublereal) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.;
    }

/*     Compute I - Q'*Q */

    zlaset_("Full", m, m, &c_b12, &c_b20, &l[l_offset], lda);
    zherk_("Upper", "Conjugate transpose", m, m, &c_b28, &q[q_offset], lda, &
	    c_b29, &l[l_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = zlansy_("1", "Upper", m, &l[l_offset], lda, &rwork[1]);

    result[2] = resid / (doublereal) max(1,*m) / eps;

    return 0;

/*     End of ZQLT01 */

} /* zqlt01_ */
