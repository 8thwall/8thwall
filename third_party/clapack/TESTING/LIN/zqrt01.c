#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublecomplex c_b1 = {-1e10,-1e10};
static doublecomplex c_b10 = {0.,0.};
static doublecomplex c_b15 = {-1.,0.};
static doublecomplex c_b16 = {1.,0.};
static doublereal c_b24 = -1.;
static doublereal c_b25 = 1.;

/* Subroutine */ int zqrt01_(integer *m, integer *n, doublecomplex *a, 
	doublecomplex *af, doublecomplex *q, doublecomplex *r__, integer *lda, 
	 doublecomplex *tau, doublecomplex *work, integer *lwork, doublereal *
	rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, i__1;

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
    extern /* Subroutine */ int zgeqrf_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, doublecomplex *, integer *, integer *
), zlacpy_(char *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, integer *), zlaset_(char *, integer *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *);
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int zungqr_(integer *, integer *, integer *, 
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

/*  ZQRT01 tests ZGEQRF, which computes the QR factorization of an m-by-n */
/*  matrix A, and partially tests ZUNGQR which forms the m-by-m */
/*  orthogonal matrix Q. */

/*  ZQRT01 compares R with Q'*A, and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) COMPLEX*16 array, dimension (LDA,N) */
/*          Details of the QR factorization of A, as returned by ZGEQRF. */
/*          See ZGEQRF for further details. */

/*  Q       (output) COMPLEX*16 array, dimension (LDA,M) */
/*          The m-by-m orthogonal matrix Q. */

/*  R       (workspace) COMPLEX*16 array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  TAU     (output) COMPLEX*16 array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by ZGEQRF. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

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

    zlacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "ZGEQRF", (ftnlen)6, (ftnlen)6);
    zgeqrf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    zlaset_("Full", m, m, &c_b1, &c_b1, &q[q_offset], lda);
    i__1 = *m - 1;
    zlacpy_("Lower", &i__1, n, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);

/*     Generate the m-by-m matrix Q */

    s_copy(srnamc_1.srnamt, "ZUNGQR", (ftnlen)6, (ftnlen)6);
    zungqr_(m, m, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R */

    zlaset_("Full", m, n, &c_b10, &c_b10, &r__[r_offset], lda);
    zlacpy_("Upper", m, n, &af[af_offset], lda, &r__[r_offset], lda);

/*     Compute R - Q'*A */

    zgemm_("Conjugate transpose", "No transpose", m, n, m, &c_b15, &q[
	    q_offset], lda, &a[a_offset], lda, &c_b16, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = zlange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = zlange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.) {
	result[1] = resid / (doublereal) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.;
    }

/*     Compute I - Q'*Q */

    zlaset_("Full", m, m, &c_b10, &c_b16, &r__[r_offset], lda);
    zherk_("Upper", "Conjugate transpose", m, m, &c_b24, &q[q_offset], lda, &
	    c_b25, &r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = zlansy_("1", "Upper", m, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (doublereal) max(1,*m) / eps;

    return 0;

/*     End of ZQRT01 */

} /* zqrt01_ */
