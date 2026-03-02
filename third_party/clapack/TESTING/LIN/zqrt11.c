#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__7 = 7;
static doublecomplex c_b5 = {0.,0.};
static doublecomplex c_b6 = {1.,0.};

doublereal zqrt11_(integer *m, integer *k, doublecomplex *a, integer *lda, 
	doublecomplex *tau, doublecomplex *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    doublereal ret_val;
    doublecomplex z__1;

    /* Local variables */
    integer j, info;
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int zunm2r_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), xerbla_(char *, integer *);
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);
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

/*  ZQRT11 computes the test ratio */

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

/*  A       (input) COMPLEX*16 array, dimension (LDA,K) */
/*          The (possibly partial) output of a QR reduction routine. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  TAU     (input) COMPLEX*16 array, dimension (K) */
/*          The scaling factors tau for the elementary transformations as */
/*          computed by the QR factorization routine. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

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
	xerbla_("ZQRT11", &c__7);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	return ret_val;
    }

    zlaset_("Full", m, m, &c_b5, &c_b6, &work[1], m);

/*     Form Q */

    zunm2r_("Left", "No transpose", m, m, k, &a[a_offset], lda, &tau[1], &
	    work[1], m, &work[*m * *m + 1], &info);

/*     Form Q'*Q */

    zunm2r_("Left", "Conjugate transpose", m, m, k, &a[a_offset], lda, &tau[1]
, &work[1], m, &work[*m * *m + 1], &info);

    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = (j - 1) * *m + j;
	i__3 = (j - 1) * *m + j;
	z__1.r = work[i__3].r - 1., z__1.i = work[i__3].i;
	work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L10: */
    }

    ret_val = zlange_("One-norm", m, m, &work[1], m, rdummy) / ((
	    doublereal) (*m) * dlamch_("Epsilon"));

    return ret_val;

/*     End of ZQRT11 */

} /* zqrt11_ */
