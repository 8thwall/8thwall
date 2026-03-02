#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__7 = 7;
static doublereal c_b5 = 0.;
static doublereal c_b6 = 1.;

doublereal dqrt11_(integer *m, integer *k, doublereal *a, integer *lda, 
	doublereal *tau, doublereal *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    doublereal ret_val;

    /* Local variables */
    integer j, info;
    extern /* Subroutine */ int dorm2r_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    xerbla_(char *, integer *);
    doublereal rdummy[1];


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DQRT11 computes the test ratio */

/*        || Q'*Q - I || / (eps * m) */

/*  where the orthogonal matrix Q is represented as a product of */
/*  elementary transformations.  Each transformation has the form */

/*     H(k) = I - tau(k) v(k) v(k)' */

/*  where tau(k) is stored in TAU(k) and v(k) is an m-vector of the form */
/*  [ 0 ... 0 1 x(k) ]', where x(k) is a vector of length m-k stored */
/*  in A(k+1:m,k). */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */

/*  K       (input) INTEGER */
/*          The number of columns of A whose subdiagonal entries */
/*          contain information about orthogonal transformations. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,K) */
/*          The (possibly partial) output of a QR reduction routine. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  TAU     (input) DOUBLE PRECISION array, dimension (K) */
/*          The scaling factors tau for the elementary transformations as */
/*          computed by the QR factorization routine. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= M*M + M. */

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
/*     .. Local Arrays .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    ret_val = 0.;

/*     Test for sufficient workspace */

    if (*lwork < *m * *m + *m) {
	xerbla_("DQRT11", &c__7);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	return ret_val;
    }

    dlaset_("Full", m, m, &c_b5, &c_b6, &work[1], m);

/*     Form Q */

    dorm2r_("Left", "No transpose", m, m, k, &a[a_offset], lda, &tau[1], &
	    work[1], m, &work[*m * *m + 1], &info);

/*     Form Q'*Q */

    dorm2r_("Left", "Transpose", m, m, k, &a[a_offset], lda, &tau[1], &work[1]
, m, &work[*m * *m + 1], &info);

    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	work[(j - 1) * *m + j] += -1.;
/* L10: */
    }

    ret_val = dlange_("One-norm", m, m, &work[1], m, rdummy) / ((
	    doublereal) (*m) * dlamch_("Epsilon"));

    return ret_val;

/*     End of DQRT11 */

} /* dqrt11_ */
