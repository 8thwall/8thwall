#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__3 = 3;
static integer c__1 = 1;

/* Subroutine */ int clagge_(integer *m, integer *n, integer *kl, integer *ku, 
	 real *d__, complex *a, integer *lda, integer *iseed, complex *work, 
	integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real r__1;
    complex q__1;

    /* Builtin functions */
    double c_abs(complex *);
    void c_div(complex *, complex *, complex *);

    /* Local variables */
    integer i__, j;
    complex wa, wb;
    real wn;
    complex tau;
    extern /* Subroutine */ int cgerc_(integer *, integer *, complex *, 
	    complex *, integer *, complex *, integer *, complex *, integer *),
	     cscal_(integer *, complex *, complex *, integer *), cgemv_(char *
, integer *, integer *, complex *, complex *, integer *, complex *
, integer *, complex *, complex *, integer *);
    extern doublereal scnrm2_(integer *, complex *, integer *);
    extern /* Subroutine */ int clacgv_(integer *, complex *, integer *), 
	    xerbla_(char *, integer *), clarnv_(integer *, integer *, 
	    integer *, complex *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLAGGE generates a complex general m by n matrix A, by pre- and post- */
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

/*  D       (input) REAL array, dimension (min(M,N)) */
/*          The diagonal elements of the diagonal matrix D. */

/*  A       (output) COMPLEX array, dimension (LDA,N) */
/*          The generated m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= M. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  WORK    (workspace) COMPLEX array, dimension (M+N) */

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
	xerbla_("CLAGGE", &i__1);
	return 0;
    }

/*     initialize A to diagonal matrix */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * a_dim1;
	    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    i__1 = min(*m,*n);
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__ + i__ * a_dim1;
	i__3 = i__;
	a[i__2].r = d__[i__3], a[i__2].i = 0.f;
/* L30: */
    }

/*     pre- and post-multiply A by random unitary matrices */

    for (i__ = min(*m,*n); i__ >= 1; --i__) {
	if (i__ < *m) {

/*           generate random reflection */

	    i__1 = *m - i__ + 1;
	    clarnv_(&c__3, &iseed[1], &i__1, &work[1]);
	    i__1 = *m - i__ + 1;
	    wn = scnrm2_(&i__1, &work[1], &c__1);
	    r__1 = wn / c_abs(&work[1]);
	    q__1.r = r__1 * work[1].r, q__1.i = r__1 * work[1].i;
	    wa.r = q__1.r, wa.i = q__1.i;
	    if (wn == 0.f) {
		tau.r = 0.f, tau.i = 0.f;
	    } else {
		q__1.r = work[1].r + wa.r, q__1.i = work[1].i + wa.i;
		wb.r = q__1.r, wb.i = q__1.i;
		i__1 = *m - i__;
		c_div(&q__1, &c_b2, &wb);
		cscal_(&i__1, &q__1, &work[2], &c__1);
		work[1].r = 1.f, work[1].i = 0.f;
		c_div(&q__1, &wb, &wa);
		r__1 = q__1.r;
		tau.r = r__1, tau.i = 0.f;
	    }

/*           multiply A(i:m,i:n) by random reflection from the left */

	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    cgemv_("Conjugate transpose", &i__1, &i__2, &c_b2, &a[i__ + i__ * 
		    a_dim1], lda, &work[1], &c__1, &c_b1, &work[*m + 1], &
		    c__1);
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    q__1.r = -tau.r, q__1.i = -tau.i;
	    cgerc_(&i__1, &i__2, &q__1, &work[1], &c__1, &work[*m + 1], &c__1, 
		     &a[i__ + i__ * a_dim1], lda);
	}
	if (i__ < *n) {

/*           generate random reflection */

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

/*           multiply A(i:m,i:n) by random reflection from the right */

	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    cgemv_("No transpose", &i__1, &i__2, &c_b2, &a[i__ + i__ * a_dim1]
, lda, &work[1], &c__1, &c_b1, &work[*n + 1], &c__1);
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    q__1.r = -tau.r, q__1.i = -tau.i;
	    cgerc_(&i__1, &i__2, &q__1, &work[*n + 1], &c__1, &work[1], &c__1, 
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
		wn = scnrm2_(&i__2, &a[*kl + i__ + i__ * a_dim1], &c__1);
		r__1 = wn / c_abs(&a[*kl + i__ + i__ * a_dim1]);
		i__2 = *kl + i__ + i__ * a_dim1;
		q__1.r = r__1 * a[i__2].r, q__1.i = r__1 * a[i__2].i;
		wa.r = q__1.r, wa.i = q__1.i;
		if (wn == 0.f) {
		    tau.r = 0.f, tau.i = 0.f;
		} else {
		    i__2 = *kl + i__ + i__ * a_dim1;
		    q__1.r = a[i__2].r + wa.r, q__1.i = a[i__2].i + wa.i;
		    wb.r = q__1.r, wb.i = q__1.i;
		    i__2 = *m - *kl - i__;
		    c_div(&q__1, &c_b2, &wb);
		    cscal_(&i__2, &q__1, &a[*kl + i__ + 1 + i__ * a_dim1], &
			    c__1);
		    i__2 = *kl + i__ + i__ * a_dim1;
		    a[i__2].r = 1.f, a[i__2].i = 0.f;
		    c_div(&q__1, &wb, &wa);
		    r__1 = q__1.r;
		    tau.r = r__1, tau.i = 0.f;
		}

/*              apply reflection to A(kl+i:m,i+1:n) from the left */

		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		cgemv_("Conjugate transpose", &i__2, &i__3, &c_b2, &a[*kl + 
			i__ + (i__ + 1) * a_dim1], lda, &a[*kl + i__ + i__ * 
			a_dim1], &c__1, &c_b1, &work[1], &c__1);
		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		q__1.r = -tau.r, q__1.i = -tau.i;
		cgerc_(&i__2, &i__3, &q__1, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &work[1], &c__1, &a[*kl + i__ + (i__ + 1) * 
			a_dim1], lda);
		i__2 = *kl + i__ + i__ * a_dim1;
		q__1.r = -wa.r, q__1.i = -wa.i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
	    }

/* Computing MIN */
	    i__2 = *n - 1 - *ku;
	    if (i__ <= min(i__2,*m)) {

/*              generate reflection to annihilate A(i,ku+i+1:n) */

		i__2 = *n - *ku - i__ + 1;
		wn = scnrm2_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		r__1 = wn / c_abs(&a[i__ + (*ku + i__) * a_dim1]);
		i__2 = i__ + (*ku + i__) * a_dim1;
		q__1.r = r__1 * a[i__2].r, q__1.i = r__1 * a[i__2].i;
		wa.r = q__1.r, wa.i = q__1.i;
		if (wn == 0.f) {
		    tau.r = 0.f, tau.i = 0.f;
		} else {
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    q__1.r = a[i__2].r + wa.r, q__1.i = a[i__2].i + wa.i;
		    wb.r = q__1.r, wb.i = q__1.i;
		    i__2 = *n - *ku - i__;
		    c_div(&q__1, &c_b2, &wb);
		    cscal_(&i__2, &q__1, &a[i__ + (*ku + i__ + 1) * a_dim1], 
			    lda);
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    a[i__2].r = 1.f, a[i__2].i = 0.f;
		    c_div(&q__1, &wb, &wa);
		    r__1 = q__1.r;
		    tau.r = r__1, tau.i = 0.f;
		}

/*              apply reflection to A(i+1:m,ku+i:n) from the right */

		i__2 = *n - *ku - i__ + 1;
		clacgv_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		cgemv_("No transpose", &i__2, &i__3, &c_b2, &a[i__ + 1 + (*ku 
			+ i__) * a_dim1], lda, &a[i__ + (*ku + i__) * a_dim1], 
			 lda, &c_b1, &work[1], &c__1);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		q__1.r = -tau.r, q__1.i = -tau.i;
		cgerc_(&i__2, &i__3, &q__1, &work[1], &c__1, &a[i__ + (*ku + 
			i__) * a_dim1], lda, &a[i__ + 1 + (*ku + i__) * 
			a_dim1], lda);
		i__2 = i__ + (*ku + i__) * a_dim1;
		q__1.r = -wa.r, q__1.i = -wa.i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
	    }
	} else {

/*           annihilate superdiagonal elements first (necessary if */
/*           KU = 0) */

/* Computing MIN */
	    i__2 = *n - 1 - *ku;
	    if (i__ <= min(i__2,*m)) {

/*              generate reflection to annihilate A(i,ku+i+1:n) */

		i__2 = *n - *ku - i__ + 1;
		wn = scnrm2_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		r__1 = wn / c_abs(&a[i__ + (*ku + i__) * a_dim1]);
		i__2 = i__ + (*ku + i__) * a_dim1;
		q__1.r = r__1 * a[i__2].r, q__1.i = r__1 * a[i__2].i;
		wa.r = q__1.r, wa.i = q__1.i;
		if (wn == 0.f) {
		    tau.r = 0.f, tau.i = 0.f;
		} else {
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    q__1.r = a[i__2].r + wa.r, q__1.i = a[i__2].i + wa.i;
		    wb.r = q__1.r, wb.i = q__1.i;
		    i__2 = *n - *ku - i__;
		    c_div(&q__1, &c_b2, &wb);
		    cscal_(&i__2, &q__1, &a[i__ + (*ku + i__ + 1) * a_dim1], 
			    lda);
		    i__2 = i__ + (*ku + i__) * a_dim1;
		    a[i__2].r = 1.f, a[i__2].i = 0.f;
		    c_div(&q__1, &wb, &wa);
		    r__1 = q__1.r;
		    tau.r = r__1, tau.i = 0.f;
		}

/*              apply reflection to A(i+1:m,ku+i:n) from the right */

		i__2 = *n - *ku - i__ + 1;
		clacgv_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		cgemv_("No transpose", &i__2, &i__3, &c_b2, &a[i__ + 1 + (*ku 
			+ i__) * a_dim1], lda, &a[i__ + (*ku + i__) * a_dim1], 
			 lda, &c_b1, &work[1], &c__1);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		q__1.r = -tau.r, q__1.i = -tau.i;
		cgerc_(&i__2, &i__3, &q__1, &work[1], &c__1, &a[i__ + (*ku + 
			i__) * a_dim1], lda, &a[i__ + 1 + (*ku + i__) * 
			a_dim1], lda);
		i__2 = i__ + (*ku + i__) * a_dim1;
		q__1.r = -wa.r, q__1.i = -wa.i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
	    }

/* Computing MIN */
	    i__2 = *m - 1 - *kl;
	    if (i__ <= min(i__2,*n)) {

/*              generate reflection to annihilate A(kl+i+1:m,i) */

		i__2 = *m - *kl - i__ + 1;
		wn = scnrm2_(&i__2, &a[*kl + i__ + i__ * a_dim1], &c__1);
		r__1 = wn / c_abs(&a[*kl + i__ + i__ * a_dim1]);
		i__2 = *kl + i__ + i__ * a_dim1;
		q__1.r = r__1 * a[i__2].r, q__1.i = r__1 * a[i__2].i;
		wa.r = q__1.r, wa.i = q__1.i;
		if (wn == 0.f) {
		    tau.r = 0.f, tau.i = 0.f;
		} else {
		    i__2 = *kl + i__ + i__ * a_dim1;
		    q__1.r = a[i__2].r + wa.r, q__1.i = a[i__2].i + wa.i;
		    wb.r = q__1.r, wb.i = q__1.i;
		    i__2 = *m - *kl - i__;
		    c_div(&q__1, &c_b2, &wb);
		    cscal_(&i__2, &q__1, &a[*kl + i__ + 1 + i__ * a_dim1], &
			    c__1);
		    i__2 = *kl + i__ + i__ * a_dim1;
		    a[i__2].r = 1.f, a[i__2].i = 0.f;
		    c_div(&q__1, &wb, &wa);
		    r__1 = q__1.r;
		    tau.r = r__1, tau.i = 0.f;
		}

/*              apply reflection to A(kl+i:m,i+1:n) from the left */

		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		cgemv_("Conjugate transpose", &i__2, &i__3, &c_b2, &a[*kl + 
			i__ + (i__ + 1) * a_dim1], lda, &a[*kl + i__ + i__ * 
			a_dim1], &c__1, &c_b1, &work[1], &c__1);
		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		q__1.r = -tau.r, q__1.i = -tau.i;
		cgerc_(&i__2, &i__3, &q__1, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &work[1], &c__1, &a[*kl + i__ + (i__ + 1) * 
			a_dim1], lda);
		i__2 = *kl + i__ + i__ * a_dim1;
		q__1.r = -wa.r, q__1.i = -wa.i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
	    }
	}

	i__2 = *m;
	for (j = *kl + i__ + 1; j <= i__2; ++j) {
	    i__3 = j + i__ * a_dim1;
	    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L50: */
	}

	i__2 = *n;
	for (j = *ku + i__ + 1; j <= i__2; ++j) {
	    i__3 = i__ + j * a_dim1;
	    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L60: */
	}
/* L70: */
    }
    return 0;

/*     End of CLAGGE */

} /* clagge_ */
