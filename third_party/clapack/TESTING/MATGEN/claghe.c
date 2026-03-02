#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__3 = 3;
static integer c__1 = 1;

/* Subroutine */ int claghe_(integer *n, integer *k, real *d__, complex *a, 
	integer *lda, integer *iseed, complex *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real r__1;
    complex q__1, q__2, q__3, q__4;

    /* Builtin functions */
    double c_abs(complex *);
    void c_div(complex *, complex *, complex *), r_cnjg(complex *, complex *);

    /* Local variables */
    integer i__, j;
    complex wa, wb;
    real wn;
    complex tau;
    extern /* Subroutine */ int cher2_(char *, integer *, complex *, complex *
, integer *, complex *, integer *, complex *, integer *), 
	    cgerc_(integer *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, complex *, integer *);
    complex alpha;
    extern /* Subroutine */ int cscal_(integer *, complex *, complex *, 
	    integer *);
    extern /* Complex */ VOID cdotc_(complex *, integer *, complex *, integer 
	    *, complex *, integer *);
    extern /* Subroutine */ int cgemv_(char *, integer *, integer *, complex *
, complex *, integer *, complex *, integer *, complex *, complex *
, integer *), chemv_(char *, integer *, complex *, 
	    complex *, integer *, complex *, integer *, complex *, complex *, 
	    integer *), caxpy_(integer *, complex *, complex *, 
	    integer *, complex *, integer *);
    extern doublereal scnrm2_(integer *, complex *, integer *);
    extern /* Subroutine */ int xerbla_(char *, integer *), clarnv_(
	    integer *, integer *, integer *, complex *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLAGHE generates a complex hermitian matrix A, by pre- and post- */
/*  multiplying a real diagonal matrix D with a random unitary matrix: */
/*  A = U*D*U'. The semi-bandwidth may then be reduced to k by additional */
/*  unitary transformations. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  K       (input) INTEGER */
/*          The number of nonzero subdiagonals within the band of A. */
/*          0 <= K <= N-1. */

/*  D       (input) REAL array, dimension (N) */
/*          The diagonal elements of the diagonal matrix D. */

/*  A       (output) COMPLEX array, dimension (LDA,N) */
/*          The generated n by n hermitian matrix A (the full matrix is */
/*          stored). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= N. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  WORK    (workspace) COMPLEX array, dimension (2*N) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments */

    /* Parameter adjustments */
    --d__;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --iseed;
    --work;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*k < 0 || *k > *n - 1) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -5;
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("CLAGHE", &i__1);
	return 0;
    }

/*     initialize lower triangle of A to diagonal matrix */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * a_dim1;
	    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * a_dim1;
	i__3 = i__;
	a[i__2].r = d__[i__3], a[i__2].i = 0.f;
/* L30: */
    }

/*     Generate lower triangle of hermitian matrix */

    for (i__ = *n - 1; i__ >= 1; --i__) {

/*        generate random reflection */

	i__1 = *n - i__ + 1;
	clarnv_(&c__3, &iseed[1], &i__1, &work[1]);
	i__1 = *n - i__ + 1;
	wn = scnrm2_(&i__1, &work[1], &c__1);
	r__1 = wn / c_abs(&work[1]);
	q__1.r = r__1 * work[1].r, q__1.i = r__1 * work[1].i;
	wa.r = q__1.r, wa.i = q__1.i;
	if (wn == 0.f) {
	    tau.r = 0.f, tau.i = 0.f;
	} else {
	    q__1.r = work[1].r + wa.r, q__1.i = work[1].i + wa.i;
	    wb.r = q__1.r, wb.i = q__1.i;
	    i__1 = *n - i__;
	    c_div(&q__1, &c_b2, &wb);
	    cscal_(&i__1, &q__1, &work[2], &c__1);
	    work[1].r = 1.f, work[1].i = 0.f;
	    c_div(&q__1, &wb, &wa);
	    r__1 = q__1.r;
	    tau.r = r__1, tau.i = 0.f;
	}

/*        apply random reflection to A(i:n,i:n) from the left */
/*        and the right */

/*        compute  y := tau * A * u */

	i__1 = *n - i__ + 1;
	chemv_("Lower", &i__1, &tau, &a[i__ + i__ * a_dim1], lda, &work[1], &
		c__1, &c_b1, &work[*n + 1], &c__1);

/*        compute  v := y - 1/2 * tau * ( y, u ) * u */

	q__3.r = -.5f, q__3.i = -0.f;
	q__2.r = q__3.r * tau.r - q__3.i * tau.i, q__2.i = q__3.r * tau.i + 
		q__3.i * tau.r;
	i__1 = *n - i__ + 1;
	cdotc_(&q__4, &i__1, &work[*n + 1], &c__1, &work[1], &c__1);
	q__1.r = q__2.r * q__4.r - q__2.i * q__4.i, q__1.i = q__2.r * q__4.i 
		+ q__2.i * q__4.r;
	alpha.r = q__1.r, alpha.i = q__1.i;
	i__1 = *n - i__ + 1;
	caxpy_(&i__1, &alpha, &work[1], &c__1, &work[*n + 1], &c__1);

/*        apply the transformation as a rank-2 update to A(i:n,i:n) */

	i__1 = *n - i__ + 1;
	q__1.r = -1.f, q__1.i = -0.f;
	cher2_("Lower", &i__1, &q__1, &work[1], &c__1, &work[*n + 1], &c__1, &
		a[i__ + i__ * a_dim1], lda);
/* L40: */
    }

/*     Reduce number of subdiagonals to K */

    i__1 = *n - 1 - *k;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        generate reflection to annihilate A(k+i+1:n,i) */

	i__2 = *n - *k - i__ + 1;
	wn = scnrm2_(&i__2, &a[*k + i__ + i__ * a_dim1], &c__1);
	r__1 = wn / c_abs(&a[*k + i__ + i__ * a_dim1]);
	i__2 = *k + i__ + i__ * a_dim1;
	q__1.r = r__1 * a[i__2].r, q__1.i = r__1 * a[i__2].i;
	wa.r = q__1.r, wa.i = q__1.i;
	if (wn == 0.f) {
	    tau.r = 0.f, tau.i = 0.f;
	} else {
	    i__2 = *k + i__ + i__ * a_dim1;
	    q__1.r = a[i__2].r + wa.r, q__1.i = a[i__2].i + wa.i;
	    wb.r = q__1.r, wb.i = q__1.i;
	    i__2 = *n - *k - i__;
	    c_div(&q__1, &c_b2, &wb);
	    cscal_(&i__2, &q__1, &a[*k + i__ + 1 + i__ * a_dim1], &c__1);
	    i__2 = *k + i__ + i__ * a_dim1;
	    a[i__2].r = 1.f, a[i__2].i = 0.f;
	    c_div(&q__1, &wb, &wa);
	    r__1 = q__1.r;
	    tau.r = r__1, tau.i = 0.f;
	}

/*        apply reflection to A(k+i:n,i+1:k+i-1) from the left */

	i__2 = *n - *k - i__ + 1;
	i__3 = *k - 1;
	cgemv_("Conjugate transpose", &i__2, &i__3, &c_b2, &a[*k + i__ + (i__ 
		+ 1) * a_dim1], lda, &a[*k + i__ + i__ * a_dim1], &c__1, &
		c_b1, &work[1], &c__1);
	i__2 = *n - *k - i__ + 1;
	i__3 = *k - 1;
	q__1.r = -tau.r, q__1.i = -tau.i;
	cgerc_(&i__2, &i__3, &q__1, &a[*k + i__ + i__ * a_dim1], &c__1, &work[
		1], &c__1, &a[*k + i__ + (i__ + 1) * a_dim1], lda);

/*        apply reflection to A(k+i:n,k+i:n) from the left and the right */

/*        compute  y := tau * A * u */

	i__2 = *n - *k - i__ + 1;
	chemv_("Lower", &i__2, &tau, &a[*k + i__ + (*k + i__) * a_dim1], lda, 
		&a[*k + i__ + i__ * a_dim1], &c__1, &c_b1, &work[1], &c__1);

/*        compute  v := y - 1/2 * tau * ( y, u ) * u */

	q__3.r = -.5f, q__3.i = -0.f;
	q__2.r = q__3.r * tau.r - q__3.i * tau.i, q__2.i = q__3.r * tau.i + 
		q__3.i * tau.r;
	i__2 = *n - *k - i__ + 1;
	cdotc_(&q__4, &i__2, &work[1], &c__1, &a[*k + i__ + i__ * a_dim1], &
		c__1);
	q__1.r = q__2.r * q__4.r - q__2.i * q__4.i, q__1.i = q__2.r * q__4.i 
		+ q__2.i * q__4.r;
	alpha.r = q__1.r, alpha.i = q__1.i;
	i__2 = *n - *k - i__ + 1;
	caxpy_(&i__2, &alpha, &a[*k + i__ + i__ * a_dim1], &c__1, &work[1], &
		c__1);

/*        apply hermitian rank-2 update to A(k+i:n,k+i:n) */

	i__2 = *n - *k - i__ + 1;
	q__1.r = -1.f, q__1.i = -0.f;
	cher2_("Lower", &i__2, &q__1, &a[*k + i__ + i__ * a_dim1], &c__1, &
		work[1], &c__1, &a[*k + i__ + (*k + i__) * a_dim1], lda);

	i__2 = *k + i__ + i__ * a_dim1;
	q__1.r = -wa.r, q__1.i = -wa.i;
	a[i__2].r = q__1.r, a[i__2].i = q__1.i;
	i__2 = *n;
	for (j = *k + i__ + 1; j <= i__2; ++j) {
	    i__3 = j + i__ * a_dim1;
	    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L50: */
	}
/* L60: */
    }

/*     Store full hermitian matrix */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    i__3 = j + i__ * a_dim1;
	    r_cnjg(&q__1, &a[i__ + j * a_dim1]);
	    a[i__3].r = q__1.r, a[i__3].i = q__1.i;
/* L70: */
	}
/* L80: */
    }
    return 0;

/*     End of CLAGHE */

} /* claghe_ */
