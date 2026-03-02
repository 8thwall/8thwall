#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__7 = 7;
static real c_b5 = 0.f;
static real c_b6 = 1.f;

doublereal sqrt11_(integer *m, integer *k, real *a, integer *lda, real *tau, 
	real *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    real ret_val;

    /* Local variables */
    integer j, info;
    extern /* Subroutine */ int sorm2r_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int xerbla_(char *, integer *), slaset_(
	    char *, integer *, integer *, real *, real *, real *, integer *);
    real rdummy[1];


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SQRT11 computes the test ratio */

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

/*  A       (input) REAL array, dimension (LDA,K) */
/*          The (possibly partial) output of a QR reduction routine. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  TAU     (input) REAL array, dimension (K) */
/*          The scaling factors tau for the elementary transformations as */
/*          computed by the QR factorization routine. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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
    ret_val = 0.f;

/*     Test for sufficient workspace */

    if (*lwork < *m * *m + *m) {
	xerbla_("SQRT11", &c__7);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0) {
	return ret_val;
    }

    slaset_("Full", m, m, &c_b5, &c_b6, &work[1], m);

/*     Form Q */

    sorm2r_("Left", "No transpose", m, m, k, &a[a_offset], lda, &tau[1], &
	    work[1], m, &work[*m * *m + 1], &info);

/*     Form Q'*Q */

    sorm2r_("Left", "Transpose", m, m, k, &a[a_offset], lda, &tau[1], &work[1]
, m, &work[*m * *m + 1], &info);

    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	work[(j - 1) * *m + j] += -1.f;
/* L10: */
    }

    ret_val = slange_("One-norm", m, m, &work[1], m, rdummy) / ((
	    real) (*m) * slamch_("Epsilon"));

    return ret_val;

/*     End of SQRT11 */

} /* sqrt11_ */
