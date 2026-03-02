#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static real c_b11 = 1.f;
static real c_b13 = 0.f;

/* Subroutine */ int slagge_(integer *m, integer *n, integer *kl, integer *ku, 
	 real *d__, real *a, integer *lda, integer *iseed, real *work, 
	integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real r__1;

    /* Builtin functions */
    double r_sign(real *, real *);

    /* Local variables */
    integer i__, j;
    real wa, wb, wn, tau;
    extern /* Subroutine */ int sger_(integer *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *);
    extern doublereal snrm2_(integer *, real *, integer *);
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    sgemv_(char *, integer *, integer *, real *, real *, integer *, 
	    real *, integer *, real *, real *, integer *), xerbla_(
	    char *, integer *), slarnv_(integer *, integer *, integer 
	    *, real *);


/*  -- LAPACK auxiliary test routine (version 3.1) */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLAGGE generates a real general m by n matrix A, by pre- and post- */
/*  multiplying a real diagonal matrix D with random orthogonal matrices: */
/*  A = U*D*V. The lower and upper bandwidths may then be reduced to */
/*  kl and ku by additional orthogonal transformations. */

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

/*  A       (output) REAL array, dimension (LDA,N) */
/*          The generated m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= M. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  WORK    (workspace) REAL array, dimension (M+N) */

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
	xerbla_("SLAGGE", &i__1);
	return 0;
    }

/*     initialize A to diagonal matrix */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] = 0.f;
/* L10: */
	}
/* L20: */
    }
    i__1 = min(*m,*n);
    for (i__ = 1; i__ <= i__1; ++i__) {
	a[i__ + i__ * a_dim1] = d__[i__];
/* L30: */
    }

/*     pre- and post-multiply A by random orthogonal matrices */

    for (i__ = min(*m,*n); i__ >= 1; --i__) {
	if (i__ < *m) {

/*           generate random reflection */

	    i__1 = *m - i__ + 1;
	    slarnv_(&c__3, &iseed[1], &i__1, &work[1]);
	    i__1 = *m - i__ + 1;
	    wn = snrm2_(&i__1, &work[1], &c__1);
	    wa = r_sign(&wn, &work[1]);
	    if (wn == 0.f) {
		tau = 0.f;
	    } else {
		wb = work[1] + wa;
		i__1 = *m - i__;
		r__1 = 1.f / wb;
		sscal_(&i__1, &r__1, &work[2], &c__1);
		work[1] = 1.f;
		tau = wb / wa;
	    }

/*           multiply A(i:m,i:n) by random reflection from the left */

	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    sgemv_("Transpose", &i__1, &i__2, &c_b11, &a[i__ + i__ * a_dim1], 
		    lda, &work[1], &c__1, &c_b13, &work[*m + 1], &c__1);
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    r__1 = -tau;
	    sger_(&i__1, &i__2, &r__1, &work[1], &c__1, &work[*m + 1], &c__1, 
		    &a[i__ + i__ * a_dim1], lda);
	}
	if (i__ < *n) {

/*           generate random reflection */

	    i__1 = *n - i__ + 1;
	    slarnv_(&c__3, &iseed[1], &i__1, &work[1]);
	    i__1 = *n - i__ + 1;
	    wn = snrm2_(&i__1, &work[1], &c__1);
	    wa = r_sign(&wn, &work[1]);
	    if (wn == 0.f) {
		tau = 0.f;
	    } else {
		wb = work[1] + wa;
		i__1 = *n - i__;
		r__1 = 1.f / wb;
		sscal_(&i__1, &r__1, &work[2], &c__1);
		work[1] = 1.f;
		tau = wb / wa;
	    }

/*           multiply A(i:m,i:n) by random reflection from the right */

	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    sgemv_("No transpose", &i__1, &i__2, &c_b11, &a[i__ + i__ * 
		    a_dim1], lda, &work[1], &c__1, &c_b13, &work[*n + 1], &
		    c__1);
	    i__1 = *m - i__ + 1;
	    i__2 = *n - i__ + 1;
	    r__1 = -tau;
	    sger_(&i__1, &i__2, &r__1, &work[*n + 1], &c__1, &work[1], &c__1, 
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
		wn = snrm2_(&i__2, &a[*kl + i__ + i__ * a_dim1], &c__1);
		wa = r_sign(&wn, &a[*kl + i__ + i__ * a_dim1]);
		if (wn == 0.f) {
		    tau = 0.f;
		} else {
		    wb = a[*kl + i__ + i__ * a_dim1] + wa;
		    i__2 = *m - *kl - i__;
		    r__1 = 1.f / wb;
		    sscal_(&i__2, &r__1, &a[*kl + i__ + 1 + i__ * a_dim1], &
			    c__1);
		    a[*kl + i__ + i__ * a_dim1] = 1.f;
		    tau = wb / wa;
		}

/*              apply reflection to A(kl+i:m,i+1:n) from the left */

		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		sgemv_("Transpose", &i__2, &i__3, &c_b11, &a[*kl + i__ + (i__ 
			+ 1) * a_dim1], lda, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &c_b13, &work[1], &c__1);
		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		r__1 = -tau;
		sger_(&i__2, &i__3, &r__1, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &work[1], &c__1, &a[*kl + i__ + (i__ + 1) * 
			a_dim1], lda);
		a[*kl + i__ + i__ * a_dim1] = -wa;
	    }

/* Computing MIN */
	    i__2 = *n - 1 - *ku;
	    if (i__ <= min(i__2,*m)) {

/*              generate reflection to annihilate A(i,ku+i+1:n) */

		i__2 = *n - *ku - i__ + 1;
		wn = snrm2_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		wa = r_sign(&wn, &a[i__ + (*ku + i__) * a_dim1]);
		if (wn == 0.f) {
		    tau = 0.f;
		} else {
		    wb = a[i__ + (*ku + i__) * a_dim1] + wa;
		    i__2 = *n - *ku - i__;
		    r__1 = 1.f / wb;
		    sscal_(&i__2, &r__1, &a[i__ + (*ku + i__ + 1) * a_dim1], 
			    lda);
		    a[i__ + (*ku + i__) * a_dim1] = 1.f;
		    tau = wb / wa;
		}

/*              apply reflection to A(i+1:m,ku+i:n) from the right */

		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b11, &a[i__ + 1 + (*
			ku + i__) * a_dim1], lda, &a[i__ + (*ku + i__) * 
			a_dim1], lda, &c_b13, &work[1], &c__1);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		r__1 = -tau;
		sger_(&i__2, &i__3, &r__1, &work[1], &c__1, &a[i__ + (*ku + 
			i__) * a_dim1], lda, &a[i__ + 1 + (*ku + i__) * 
			a_dim1], lda);
		a[i__ + (*ku + i__) * a_dim1] = -wa;
	    }
	} else {

/*           annihilate superdiagonal elements first (necessary if */
/*           KU = 0) */

/* Computing MIN */
	    i__2 = *n - 1 - *ku;
	    if (i__ <= min(i__2,*m)) {

/*              generate reflection to annihilate A(i,ku+i+1:n) */

		i__2 = *n - *ku - i__ + 1;
		wn = snrm2_(&i__2, &a[i__ + (*ku + i__) * a_dim1], lda);
		wa = r_sign(&wn, &a[i__ + (*ku + i__) * a_dim1]);
		if (wn == 0.f) {
		    tau = 0.f;
		} else {
		    wb = a[i__ + (*ku + i__) * a_dim1] + wa;
		    i__2 = *n - *ku - i__;
		    r__1 = 1.f / wb;
		    sscal_(&i__2, &r__1, &a[i__ + (*ku + i__ + 1) * a_dim1], 
			    lda);
		    a[i__ + (*ku + i__) * a_dim1] = 1.f;
		    tau = wb / wa;
		}

/*              apply reflection to A(i+1:m,ku+i:n) from the right */

		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		sgemv_("No transpose", &i__2, &i__3, &c_b11, &a[i__ + 1 + (*
			ku + i__) * a_dim1], lda, &a[i__ + (*ku + i__) * 
			a_dim1], lda, &c_b13, &work[1], &c__1);
		i__2 = *m - i__;
		i__3 = *n - *ku - i__ + 1;
		r__1 = -tau;
		sger_(&i__2, &i__3, &r__1, &work[1], &c__1, &a[i__ + (*ku + 
			i__) * a_dim1], lda, &a[i__ + 1 + (*ku + i__) * 
			a_dim1], lda);
		a[i__ + (*ku + i__) * a_dim1] = -wa;
	    }

/* Computing MIN */
	    i__2 = *m - 1 - *kl;
	    if (i__ <= min(i__2,*n)) {

/*              generate reflection to annihilate A(kl+i+1:m,i) */

		i__2 = *m - *kl - i__ + 1;
		wn = snrm2_(&i__2, &a[*kl + i__ + i__ * a_dim1], &c__1);
		wa = r_sign(&wn, &a[*kl + i__ + i__ * a_dim1]);
		if (wn == 0.f) {
		    tau = 0.f;
		} else {
		    wb = a[*kl + i__ + i__ * a_dim1] + wa;
		    i__2 = *m - *kl - i__;
		    r__1 = 1.f / wb;
		    sscal_(&i__2, &r__1, &a[*kl + i__ + 1 + i__ * a_dim1], &
			    c__1);
		    a[*kl + i__ + i__ * a_dim1] = 1.f;
		    tau = wb / wa;
		}

/*              apply reflection to A(kl+i:m,i+1:n) from the left */

		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		sgemv_("Transpose", &i__2, &i__3, &c_b11, &a[*kl + i__ + (i__ 
			+ 1) * a_dim1], lda, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &c_b13, &work[1], &c__1);
		i__2 = *m - *kl - i__ + 1;
		i__3 = *n - i__;
		r__1 = -tau;
		sger_(&i__2, &i__3, &r__1, &a[*kl + i__ + i__ * a_dim1], &
			c__1, &work[1], &c__1, &a[*kl + i__ + (i__ + 1) * 
			a_dim1], lda);
		a[*kl + i__ + i__ * a_dim1] = -wa;
	    }
	}

	i__2 = *m;
	for (j = *kl + i__ + 1; j <= i__2; ++j) {
	    a[j + i__ * a_dim1] = 0.f;
/* L50: */
	}

	i__2 = *n;
	for (j = *ku + i__ + 1; j <= i__2; ++j) {
	    a[i__ + j * a_dim1] = 0.f;
/* L60: */
	}
/* L70: */
    }
    return 0;

/*     End of SLAGGE */

} /* slagge_ */
