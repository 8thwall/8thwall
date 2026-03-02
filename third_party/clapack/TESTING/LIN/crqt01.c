#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static complex c_b1 = {-1e10f,-1e10f};
static complex c_b12 = {0.f,0.f};
static complex c_b19 = {-1.f,0.f};
static complex c_b20 = {1.f,0.f};
static real c_b28 = -1.f;
static real c_b29 = 1.f;

/* Subroutine */ int crqt01_(integer *m, integer *n, complex *a, complex *af, 
	complex *q, complex *r__, integer *lda, complex *tau, complex *work, 
	integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, q_dim1, q_offset, r_dim1, 
	    r_offset, i__1, i__2;

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
    extern /* Subroutine */ int cgerqf_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, integer *), clacpy_(
	    char *, integer *, integer *, complex *, integer *, complex *, 
	    integer *), claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);
    extern doublereal clansy_(char *, char *, integer *, complex *, integer *, 
	     real *);
    extern /* Subroutine */ int cungrq_(integer *, integer *, integer *, 
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

/*  CRQT01 tests CGERQF, which computes the RQ factorization of an m-by-n */
/*  matrix A, and partially tests CUNGRQ which forms the n-by-n */
/*  orthogonal matrix Q. */

/*  CRQT01 compares R with A*Q', and checks that Q is orthogonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  AF      (output) COMPLEX array, dimension (LDA,N) */
/*          Details of the RQ factorization of A, as returned by CGERQF. */
/*          See CGERQF for further details. */

/*  Q       (output) COMPLEX array, dimension (LDA,N) */
/*          The n-by-n orthogonal matrix Q. */

/*  R       (workspace) COMPLEX array, dimension (LDA,max(M,N)) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and L. */
/*          LDA >= max(M,N). */

/*  TAU     (output) COMPLEX array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors, as returned */
/*          by CGERQF. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

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

    clacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);

/*     Factorize the matrix A in the array AF. */

    s_copy(srnamc_1.srnamt, "CGERQF", (ftnlen)6, (ftnlen)6);
    cgerqf_(m, n, &af[af_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy details of Q */

    claset_("Full", n, n, &c_b1, &c_b1, &q[q_offset], lda);
    if (*m <= *n) {
	if (*m > 0 && *m < *n) {
	    i__1 = *n - *m;
	    clacpy_("Full", m, &i__1, &af[af_offset], lda, &q[*n - *m + 1 + 
		    q_dim1], lda);
	}
	if (*m > 1) {
	    i__1 = *m - 1;
	    i__2 = *m - 1;
	    clacpy_("Lower", &i__1, &i__2, &af[(*n - *m + 1) * af_dim1 + 2], 
		    lda, &q[*n - *m + 2 + (*n - *m + 1) * q_dim1], lda);
	}
    } else {
	if (*n > 1) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    clacpy_("Lower", &i__1, &i__2, &af[*m - *n + 2 + af_dim1], lda, &
		    q[q_dim1 + 2], lda);
	}
    }

/*     Generate the n-by-n matrix Q */

    s_copy(srnamc_1.srnamt, "CUNGRQ", (ftnlen)6, (ftnlen)6);
    cungrq_(n, n, &minmn, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

/*     Copy R */

    claset_("Full", m, n, &c_b12, &c_b12, &r__[r_offset], lda);
    if (*m <= *n) {
	if (*m > 0) {
	    clacpy_("Upper", m, m, &af[(*n - *m + 1) * af_dim1 + 1], lda, &
		    r__[(*n - *m + 1) * r_dim1 + 1], lda);
	}
    } else {
	if (*m > *n && *n > 0) {
	    i__1 = *m - *n;
	    clacpy_("Full", &i__1, n, &af[af_offset], lda, &r__[r_offset], 
		    lda);
	}
	if (*n > 0) {
	    clacpy_("Upper", n, n, &af[*m - *n + 1 + af_dim1], lda, &r__[*m - 
		    *n + 1 + r_dim1], lda);
	}
    }

/*     Compute R - A*Q' */

    cgemm_("No transpose", "Conjugate transpose", m, n, n, &c_b19, &a[
	    a_offset], lda, &q[q_offset], lda, &c_b20, &r__[r_offset], lda);

/*     Compute norm( R - Q'*A ) / ( N * norm(A) * EPS ) . */

    anorm = clange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    resid = clange_("1", m, n, &r__[r_offset], lda, &rwork[1]);
    if (anorm > 0.f) {
	result[1] = resid / (real) max(1,*n) / anorm / eps;
    } else {
	result[1] = 0.f;
    }

/*     Compute I - Q*Q' */

    claset_("Full", n, n, &c_b12, &c_b20, &r__[r_offset], lda);
    cherk_("Upper", "No transpose", n, n, &c_b28, &q[q_offset], lda, &c_b29, &
	    r__[r_offset], lda);

/*     Compute norm( I - Q*Q' ) / ( N * EPS ) . */

    resid = clansy_("1", "Upper", n, &r__[r_offset], lda, &rwork[1]);

    result[2] = resid / (real) max(1,*n) / eps;

    return 0;

/*     End of CRQT01 */

} /* crqt01_ */
