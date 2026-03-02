#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__3 = 3;
static integer c__1 = 1;

/* Subroutine */ int zlarge_(integer *n, doublecomplex *a, integer *lda, 
	integer *iseed, doublecomplex *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    doublereal d__1;
    doublecomplex z__1;

    /* Builtin functions */
    double z_abs(doublecomplex *);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__;
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
    extern /* Subroutine */ int xerbla_(char *, integer *), zlarnv_(
	    integer *, integer *, integer *, doublecomplex *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLARGE pre- and post-multiplies a complex general n by n matrix A */
/*  with a random unitary matrix: A = U*D*U'. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) COMPLEX*16 array, dimension (LDA,N) */
/*          On entry, the original n by n matrix A. */
/*          On exit, A is overwritten by U*A*U' for some random */
/*          unitary matrix U. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= N. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (2*N) */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --iseed;
    --work;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*lda < max(1,*n)) {
	*info = -3;
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("ZLARGE", &i__1);
	return 0;
    }

/*     pre- and post-multiply A by random unitary matrix */

    for (i__ = *n; i__ >= 1; --i__) {

/*        generate random reflection */

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

/*        multiply A(i:n,1:n) by random reflection from the left */

	i__1 = *n - i__ + 1;
	zgemv_("Conjugate transpose", &i__1, n, &c_b2, &a[i__ + a_dim1], lda, 
		&work[1], &c__1, &c_b1, &work[*n + 1], &c__1);
	i__1 = *n - i__ + 1;
	z__1.r = -tau.r, z__1.i = -tau.i;
	zgerc_(&i__1, n, &z__1, &work[1], &c__1, &work[*n + 1], &c__1, &a[i__ 
		+ a_dim1], lda);

/*        multiply A(1:n,i:n) by random reflection from the right */

	i__1 = *n - i__ + 1;
	zgemv_("No transpose", n, &i__1, &c_b2, &a[i__ * a_dim1 + 1], lda, &
		work[1], &c__1, &c_b1, &work[*n + 1], &c__1);
	i__1 = *n - i__ + 1;
	z__1.r = -tau.r, z__1.i = -tau.i;
	zgerc_(n, &i__1, &z__1, &work[*n + 1], &c__1, &work[1], &c__1, &a[i__ 
		* a_dim1 + 1], lda);
/* L10: */
    }
    return 0;

/*     End of ZLARGE */

} /* zlarge_ */
