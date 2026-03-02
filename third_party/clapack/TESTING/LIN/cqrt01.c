#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static complex c_b1 = {-1e10f,-1e10f};
static complex c_b10 = {0.f,0.f};
static complex c_b15 = {-1.f,0.f};
static complex c_b16 = {1.f,0.f};
static real c_b24 = -1.f;
static real c_b25 = 1.f;

/* Subroutine */ int cqrt01_(integer *m, integer *n, complex *a, complex *af, 
	complex *q, complex *r__, integer *lda, complex *tau, complex *work, 
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
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *), cherk_(char *, 
	    char *, integer *, integer *, real *, complex *, integer *, real *
, complex *, integer *);
    real resid, anorm;
    integer minmn;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int cgeqrf_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, integer *), clacpy_(
	    char *, integer *, integer *, complex *, integer *, complex *, 
	    integer *), claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);
    extern doublereal clansy_(char *, char *, integer *, complex *, integer *, 
	     real *);
    extern /* Subroutine */ int cungqr_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CQRT01 tests CGEQRF, which computes the QR factorization of an m-by-n */
/*  matrix A, and partially tests CUNGQR which forms the m-by-m */
/*  orthogonal matrix Q. */

/*  CQRT01 compares R with Q'*A, and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) COMPLEX array, dimension (LDA,N) */
/*          Details of the QR factorization of A, as returned by CGEQRF. */
/*          See CGEQRF for further details. */

/*  Q       (output) COMPLEX array, dimension (LDA,M) */
/*          The m-by-m orthogonal matrix Q. */

/*  R       (workspace) COMPLEX array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  TAU     (output) COMPLEX array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by CGEQRF. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

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

    clacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "CGEQRF", (ftnlen)6, (ftnlen)6);
    cgeqrf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    claset_("Full", m, m, &c_b1, &c_b1, &q[q_offset], lda);
    i__1 = *m - 1;
    clacpy_("Lower", &i__1, n, &af[af_dim1 + 2], lda, &q[q_dim1 + 2], lda);

/*     Generate the m-by-m matrix Q */

    s_copy(srnamc_1.srnamt, "CUNGQR", (ftnlen)6, (ftnlen)6);
    cungqr_(m, m, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R */

    claset_("Full", m, n, &c_b10, &c_b10, &r__[r_offset], lda);
    clacpy_("Upper", m, n, &af[af_offset], lda, &r__[r_offset], lda);

/*     Compute R - Q'*A */

    cgemm_("Conjugate transpose", "No transpose", m, n, m, &c_b15, &q[
	    q_offset], lda, &a[a_offset], lda, &c_b16, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( M * norm(A) * EPS ) . */

    anorm = clange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = clange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*m) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q'*Q */

    claset_("Full", m, m, &c_b10, &c_b16, &r__[r_offset], lda);
    cherk_("Upper", "Conjugate transpose", m, m, &c_b24, &q[q_offset], lda, &
	    c_b25, &r__[r_offset], lda);

/*     Compute norm( I - Q'*Q ) / ( M * EPS ) . */

    resid = clansy_("1", "Upper", m, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*m) / eps;

    return 0;

/*     End of CQRT01 */

} /* cqrt01_ */
