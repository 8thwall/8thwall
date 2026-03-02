#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__3 = 3;
static integer c__1 = 1;

/* Subroutine */ int zlagge_(integer *m, integer *n, integer *kl, integer *ku, 
	 doublereal *d__, doublecomplex *a, integer *lda, integer *iseed, 
	doublecomplex *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    doublereal d__1;
    doublecomplex z__1;

    /* Builtin functions */
    double z_abs(doublecomplex *);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__, j;
    doublecomplex wa, wb;
    doublereal wn;
    doublecomplex tau;
    extern /* Subroutine */ int zgerc_(integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zscal_(integer *, doublecomplex *, 
	    doublecomplex *, integer *), zgemv_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *);
    extern doublereal dznrm2_(integer *, doublecomplex *, integer *);
    extern /* Subroutine */ int xerbla_(char *, integer *), zlacgv_(
	    integer *, doublecomplex *, integer *), zlarnv_(integer *, 
	    integer *, integer *, doublecomplex *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLAGGE generates a complex general m by n matrix A, by pre- and post- */
/*  multiplying a real diagonal matrix D with random unitary matrices: */
/*  A = U*D*V. The lower and upper bandwidths may then be reduced to */
/*  kl and ku by additional unitary transformations. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of nonzero subdiagonals within the band of A. */
/*          0 <= KL <= M-1. */

/*  KU      (input) INTEGER */
/*          The number of nonzero superdiagonals within the band of A. */
/*          0 <= KU <= N-1. */

/*  D       (input) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The diagonal elements of the diagonal matrix D. */

/*  A       (output) COMPLEX*16 array, dimension (LDA,N) */
/*          The generated m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= M. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (M+N) */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
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
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0 || *kl > *m - 1) {
	*info = -3;
    } else if (*ku < 0 || *ku > *n - 1) {
	*info = -4;
    } else if (*lda < max(1,*m)) {
	*info = -7;
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("ZLAGGE", &i__1);
	return 0;
    }

/*     initialize A to diagonal matrix */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * a_dim1;
	    a[i__3].r = 0., a[i__3].i = 0.;
/* L10: */
	}
/* L20: */
    }
    i__1 = min(*m,*n);
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * a_dim1;
	i__3 = i__;
	a[i__2].r = d__[i__3], a[i__2].i = 0.;
/* L30: */
    }

/*     pre- and post-multiply A by random unitary matrices */

    for (i__ = min(*m,*n); i__ >= 1; --i__) {
	if (i__ < *m) {

/*           generate random reflection */

	    i__1 = *m - i__ + 1;
	    zlarnv_(&c__3, &iseed[1], &i__1, &work[1]);
	    i__1 = *m - i__ + 1;
	    wn = dznrm2_(&i__1, &work[1], &c__1);
	    d__1 = wn / z_abs(&work[1]);
	    z__1.r = d__1 * work[1].r, z__1.i = d__1 * work[1].i;
	    wa.r = z__1.r, wa.i = z__1.i;
	    if (wn == 0.) {
		tau.r = 0., tau.i = 0.;
	    } else {
		z__1.r = work[1].r + wa.r, z__1.i = work[1].i + wa.i;
		wb.r = z__1.r, wb.i = z__1.i;
		i__1 = *m - i__;
		z_div(&z__1, &c_b2, &wb);
		zscal_(&i__1, &z__1, &work[2], &c__1);
		work[1].r = 1., work[1].i = 0.;
		z_div(&z__1, &wb, &wa);
		d__1 = z__1.r;
		tau.r = d__1, tau.i = 0.;
	    }

/*           multiply A(i:m,i:n) by random reflection from the left */

	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    zgemv_("Conjugate transpose", &i__1, &i__2, &c_b2, &a[i__ + i__ * 
		    a_dim1], lda, &work[1], &c__1, &c_b1, &work[*m + 1], &
		    c__1);
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    z__1.r = -tau.r, z__1.i = -tau.i;
	    zgerc_(&i__1, &i__2, &z__1, &work[1], &c__1, &work[*m + 1], &c__1, 
		     &a[i__ + i__ * a_dim1], lda);
	}
	if (i__ < *n) {

/*           generate random reflection */

	    i__1 = *n - i__ + 1;
	    zlarnv_(&c__3, &iseed[1], &i__1, &work[1]);
	    i__1 = *n - i__ + 1;
	    wn = dznrm2_(&i__1, &work[1], &c__1);
	    d__1 = wn / z_abs(&work[1]);
	    z__1.r = d__1 * work[1].r, z__1.i = d__1 * work[1].i;
	    wa.r = z__1.r, wa.i = z__1.i;
	    if (wn == 0.) {
		tau.r = 0., tau.i = 0.;
	    } else {
		z__1.r = work[1].r + wa.r, z__1.i = work[1].i + wa.i;
		wb.r = z__1.r, wb.i = z__1.i;
		i__1 = *n - i__;
		z_div(&z__1, &c_b2, &wb);
		zscal_(&i__1, &z__1, &work[2], &c__1);
		work[1].r = 1., work[1].i = 0.;
		z_div(&z__1, &wb, &wa);
		d__1 = z__1.r;
		tau.r = d__1, tau.i = 0.;
	    }

/*           multiply A(i:m,i:n) by random reflection from the right */

	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    zgemv_("No transpose", &i__1, &i__2, &c_b2, &a[i__ + i__ * a_dim1]
, lda, &work[1], &c__1, &c_b1, &work[*n + 1], &c__1);
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    z__1.r = -tau.r, z__1.i = -tau.i;
	    zgerc_(&i__1, &i__2, &z__1, &work[*n + 1], &c__1, &work[1], &c__1, 
		     &a[i__ + i__ * a_dim1], lda);
	}
/* L40: */
    }

/*     Reduce number of subdiagonals to KL and number of superdiagonals */
/*     to KU */

/* Computing MAX */
    i__2 = *m - 1 - *kl, i__3 = *n - 1 - *ku;
    i__1 = max(i__2,i__3);
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (*kl <= *ku) {

/*           annihilate subdiagonal elements first (necessary if KL = 0) */

/* Computing MIN */
	    i__2 = *m - 1 - *kl;
	    if (i__ <= min(i__2,*n)) {

/*              generate reflection to annihilate A(kl+i+1:m,i) */

		i__2 = *m - *kl - i__ + 1;
		wn = dznrm2_(&i__2, &a[*kl + i__ + i__ * a_dim1], &c__1);
		d__1 = wn / z_abs(&a[*kl + i__ + i__ * a_dim1]);
		i__2 = *kl + i__ + i__ * a_dim1;
		z__1.r = d__1 * a[i__2].r, z__1.i = d__1 * a[i__2].i;
		wa.r = z__1.r, wa.i = z__1.i;
		if (wn == 0.) {
		    tau.r = 0., tau.i = 0.;
		} else {
		    i__2 = *kl + i__ + i__ * a_dim1;
		    z__1.r = a[i__2].r + wa.r, z__1.i = a[i__2].i + wa.i;
		    wb.r = z__1.r, wb.i = z__1.i;
		    i__2 = *m - *kl - i__;
		    z_div(&z__1, &c_b2, &wb);
		    zscal_(&i__2, &z__1, &a[*kl + i__ + 1 + i__ * a_dim1], &
			    c__1);
		    i__2 = *kl + i__ + i__ * a_dim1;
		    a[i__2].r = 1., a[i__2].i = 0.;
		    z_div(&z__1, &wb, &wa);
		    d__1 = z__1.r;
		    tau.r = d__1, tau.i = 0.;
		}

/*              apply reflection to A(kl+i:m,i+1:n) from the left */

		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		zgemv_("Conjugate transpose", &i__2, &i__3, &c_b2, &a[*kl + 
			i__ + (i__ + 1) * a_dim1], lda, &a[*kl + i__ + i__ * 
			a_dim1], &c__1, &c_b1, &work[1], &c__1);
		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		z__1.r = -tau.r, z__1.i = -tau.i;
		zgerc_(&i__2, &i__3, &z__1, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &work[1], &c__1, &a[*kl + i__ + (i__ + 1) * 
			a_dim1], lda);
		i__2 = *kl + i__ + i__ * a_dim1;
		z__1.r = -wa.r, z__1.i = -wa.i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
	    }

/* Computing MIN */
	    i__2 = *n - 1 - *ku;
	    if (i__ <= min(i__2,*m)) {

/*              generate reflection to annihilate A(i,ku+i+1:n) */

		i__2 = *n - *ku - i__ + 1;
		wn = dznrm2_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		d__1 = wn / z_abs(&a[i__ + (*ku + i__) * a_dim1]);
		i__2 = i__ + (*ku + i__) * a_dim1;
		z__1.r = d__1 * a[i__2].r, z__1.i = d__1 * a[i__2].i;
		wa.r = z__1.r, wa.i = z__1.i;
		if (wn == 0.) {
		    tau.r = 0., tau.i = 0.;
		} else {
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    z__1.r = a[i__2].r + wa.r, z__1.i = a[i__2].i + wa.i;
		    wb.r = z__1.r, wb.i = z__1.i;
		    i__2 = *n - *ku - i__;
		    z_div(&z__1, &c_b2, &wb);
		    zscal_(&i__2, &z__1, &a[i__ + (*ku + i__ + 1) * a_dim1], 
			    lda);
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    a[i__2].r = 1., a[i__2].i = 0.;
		    z_div(&z__1, &wb, &wa);
		    d__1 = z__1.r;
		    tau.r = d__1, tau.i = 0.;
		}

/*              apply reflection to A(i+1:m,ku+i:n) from the right */

		i__2 = *n - *ku - i__ + 1;
		zlacgv_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		zgemv_("No transpose", &i__2, &i__3, &c_b2, &a[i__ + 1 + (*ku 
			+ i__) * a_dim1], lda, &a[i__ + (*ku + i__) * a_dim1], 
			 lda, &c_b1, &work[1], &c__1);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		z__1.r = -tau.r, z__1.i = -tau.i;
		zgerc_(&i__2, &i__3, &z__1, &work[1], &c__1, &a[i__ + (*ku + 
			i__) * a_dim1], lda, &a[i__ + 1 + (*ku + i__) * 
			a_dim1], lda);
		i__2 = i__ + (*ku + i__) * a_dim1;
		z__1.r = -wa.r, z__1.i = -wa.i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
	    }
	} else {

/*           annihilate superdiagonal elements first (necessary if */
/*           KU = 0) */

/* Computing MIN */
	    i__2 = *n - 1 - *ku;
	    if (i__ <= min(i__2,*m)) {

/*              generate reflection to annihilate A(i,ku+i+1:n) */

		i__2 = *n - *ku - i__ + 1;
		wn = dznrm2_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		d__1 = wn / z_abs(&a[i__ + (*ku + i__) * a_dim1]);
		i__2 = i__ + (*ku + i__) * a_dim1;
		z__1.r = d__1 * a[i__2].r, z__1.i = d__1 * a[i__2].i;
		wa.r = z__1.r, wa.i = z__1.i;
		if (wn == 0.) {
		    tau.r = 0., tau.i = 0.;
		} else {
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    z__1.r = a[i__2].r + wa.r, z__1.i = a[i__2].i + wa.i;
		    wb.r = z__1.r, wb.i = z__1.i;
		    i__2 = *n - *ku - i__;
		    z_div(&z__1, &c_b2, &wb);
		    zscal_(&i__2, &z__1, &a[i__ + (*ku + i__ + 1) * a_dim1], 
			    lda);
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    a[i__2].r = 1., a[i__2].i = 0.;
		    z_div(&z__1, &wb, &wa);
		    d__1 = z__1.r;
		    tau.r = d__1, tau.i = 0.;
		}

/*              apply reflection to A(i+1:m,ku+i:n) from the right */

		i__2 = *n - *ku - i__ + 1;
		zlacgv_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		zgemv_("No transpose", &i__2, &i__3, &c_b2, &a[i__ + 1 + (*ku 
			+ i__) * a_dim1], lda, &a[i__ + (*ku + i__) * a_dim1], 
			 lda, &c_b1, &work[1], &c__1);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		z__1.r = -tau.r, z__1.i = -tau.i;
		zgerc_(&i__2, &i__3, &z__1, &work[1], &c__1, &a[i__ + (*ku + 
			i__) * a_dim1], lda, &a[i__ + 1 + (*ku + i__) * 
			a_dim1], lda);
		i__2 = i__ + (*ku + i__) * a_dim1;
		z__1.r = -wa.r, z__1.i = -wa.i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
	    }

/* Computing MIN */
	    i__2 = *m - 1 - *kl;
	    if (i__ <= min(i__2,*n)) {

/*              generate reflection to annihilate A(kl+i+1:m,i) */

		i__2 = *m - *kl - i__ + 1;
		wn = dznrm2_(&i__2, &a[*kl + i__ + i__ * a_dim1], &c__1);
		d__1 = wn / z_abs(&a[*kl + i__ + i__ * a_dim1]);
		i__2 = *kl + i__ + i__ * a_dim1;
		z__1.r = d__1 * a[i__2].r, z__1.i = d__1 * a[i__2].i;
		wa.r = z__1.r, wa.i = z__1.i;
		if (wn == 0.) {
		    tau.r = 0., tau.i = 0.;
		} else {
		    i__2 = *kl + i__ + i__ * a_dim1;
		    z__1.r = a[i__2].r + wa.r, z__1.i = a[i__2].i + wa.i;
		    wb.r = z__1.r, wb.i = z__1.i;
		    i__2 = *m - *kl - i__;
		    z_div(&z__1, &c_b2, &wb);
		    zscal_(&i__2, &z__1, &a[*kl + i__ + 1 + i__ * a_dim1], &
			    c__1);
		    i__2 = *kl + i__ + i__ * a_dim1;
		    a[i__2].r = 1., a[i__2].i = 0.;
		    z_div(&z__1, &wb, &wa);
		    d__1 = z__1.r;
		    tau.r = d__1, tau.i = 0.;
		}

/*              apply reflection to A(kl+i:m,i+1:n) from the left */

		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		zgemv_("Conjugate transpose", &i__2, &i__3, &c_b2, &a[*kl + 
			i__ + (i__ + 1) * a_dim1], lda, &a[*kl + i__ + i__ * 
			a_dim1], &c__1, &c_b1, &work[1], &c__1);
		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		z__1.r = -tau.r, z__1.i = -tau.i;
		zgerc_(&i__2, &i__3, &z__1, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &work[1], &c__1, &a[*kl + i__ + (i__ + 1) * 
			a_dim1], lda);
		i__2 = *kl + i__ + i__ * a_dim1;
		z__1.r = -wa.r, z__1.i = -wa.i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
	    }
	}

	i__2 = *m;
	for (j = *kl + i__ + 1; j <= i__2; ++j) {
	    i__3 = j + i__ * a_dim1;
	    a[i__3].r = 0., a[i__3].i = 0.;
/* L50: */
	}

	i__2 = *n;
	for (j = *ku + i__ + 1; j <= i__2; ++j) {
	    i__3 = i__ + j * a_dim1;
	    a[i__3].r = 0., a[i__3].i = 0.;
/* L60: */
	}
/* L70: */
    }
    return 0;

/*     End of ZLAGGE */

} /* zlagge_ */
