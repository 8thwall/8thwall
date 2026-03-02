#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__5 = 5;
static integer c__2 = 2;

/* Subroutine */ int zlatsp_(char *uplo, integer *n, doublecomplex *x, 
	integer *iseed)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublecomplex z__1, z__2, z__3;

    /* Builtin functions */
    double sqrt(doublereal), z_abs(doublecomplex *);

    /* Local variables */
    doublecomplex a, b, c__;
    integer j;
    doublecomplex r__;
    integer n5, jj;
    doublereal beta, alpha, alpha3;
    extern /* Double Complex */ VOID zlarnd_(doublecomplex *, integer *, 
	    integer *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLATSP generates a special test matrix for the complex symmetric */
/*  (indefinite) factorization for packed matrices.  The pivot blocks of */
/*  the generated matrix will be in the following order: */
/*     2x2 pivot block, non diagonalizable */
/*     1x1 pivot block */
/*     2x2 pivot block, diagonalizable */
/*     (cycle repeats) */
/*  A row interchange is required for each non-diagonalizable 2x2 block. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER */
/*          Specifies whether the generated matrix is to be upper or */
/*          lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The dimension of the matrix to be generated. */

/*  X       (output) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The generated matrix in packed storage format.  The matrix */
/*          consists of 3x3 and 2x2 diagonal blocks which result in the */
/*          pivot sequence given above.  The matrix outside these */
/*          diagonal blocks is zero. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed for the random number generator.  The last */
/*          of the four integers must be odd.  (modified on exit) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants */

    /* Parameter adjustments */
    --iseed;
    --x;

    /* Function Body */
    alpha = (sqrt(17.) + 1.) / 8.;
    beta = alpha - .001;
    alpha3 = alpha * alpha * alpha;

/*     Fill the matrix with zeros. */

    i__1 = *n * (*n + 1) / 2;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j;
	x[i__2].r = 0., x[i__2].i = 0.;
/* L10: */
    }

/*     UPLO = 'U':  Upper triangular storage */

    if (*(unsigned char *)uplo == 'U') {
	n5 = *n / 5;
	n5 = *n - n5 * 5 + 1;

	jj = *n * (*n + 1) / 2;
	i__1 = n5;
	for (j = *n; j >= i__1; j += -5) {
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = alpha3 * z__2.r, z__1.i = alpha3 * z__2.i;
	    a.r = z__1.r, a.i = z__1.i;
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = z__2.r / alpha, z__1.i = z__2.i / alpha;
	    b.r = z__1.r, b.i = z__1.i;
	    z__3.r = b.r * 2., z__3.i = b.i * 2.;
	    z__2.r = z__3.r * 0. - z__3.i * 1., z__2.i = z__3.r * 1. + z__3.i 
		    * 0.;
	    z__1.r = a.r - z__2.r, z__1.i = a.i - z__2.i;
	    c__.r = z__1.r, c__.i = z__1.i;
	    z__1.r = c__.r / beta, z__1.i = c__.i / beta;
	    r__.r = z__1.r, r__.i = z__1.i;
	    i__2 = jj;
	    x[i__2].r = a.r, x[i__2].i = a.i;
	    i__2 = jj - 2;
	    x[i__2].r = b.r, x[i__2].i = b.i;
	    jj -= j;
	    i__2 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    i__2 = jj - 1;
	    x[i__2].r = r__.r, x[i__2].i = r__.i;
	    jj -= j - 1;
	    i__2 = jj;
	    x[i__2].r = c__.r, x[i__2].i = c__.i;
	    jj -= j - 2;
	    i__2 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    jj -= j - 3;
	    i__2 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    if (z_abs(&x[jj + (j - 3)]) > z_abs(&x[jj])) {
		i__2 = jj + (j - 4);
		i__3 = jj + (j - 3);
		z__1.r = x[i__3].r * 2., z__1.i = x[i__3].i * 2.;
		x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    } else {
		i__2 = jj + (j - 4);
		i__3 = jj;
		z__1.r = x[i__3].r * 2., z__1.i = x[i__3].i * 2.;
		x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    }
	    jj -= j - 4;
/* L20: */
	}

/*        Clean-up for N not a multiple of 5. */

	j = n5 - 1;
	if (j > 2) {
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = alpha3 * z__2.r, z__1.i = alpha3 * z__2.i;
	    a.r = z__1.r, a.i = z__1.i;
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = z__2.r / alpha, z__1.i = z__2.i / alpha;
	    b.r = z__1.r, b.i = z__1.i;
	    z__3.r = b.r * 2., z__3.i = b.i * 2.;
	    z__2.r = z__3.r * 0. - z__3.i * 1., z__2.i = z__3.r * 1. + z__3.i 
		    * 0.;
	    z__1.r = a.r - z__2.r, z__1.i = a.i - z__2.i;
	    c__.r = z__1.r, c__.i = z__1.i;
	    z__1.r = c__.r / beta, z__1.i = c__.i / beta;
	    r__.r = z__1.r, r__.i = z__1.i;
	    i__1 = jj;
	    x[i__1].r = a.r, x[i__1].i = a.i;
	    i__1 = jj - 2;
	    x[i__1].r = b.r, x[i__1].i = b.i;
	    jj -= j;
	    i__1 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    i__1 = jj - 1;
	    x[i__1].r = r__.r, x[i__1].i = r__.i;
	    jj -= j - 1;
	    i__1 = jj;
	    x[i__1].r = c__.r, x[i__1].i = c__.i;
	    jj -= j - 2;
	    j += -3;
	}
	if (j > 1) {
	    i__1 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    i__1 = jj - j;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    if (z_abs(&x[jj]) > z_abs(&x[jj - j])) {
		i__1 = jj - 1;
		i__2 = jj;
		z__1.r = x[i__2].r * 2., z__1.i = x[i__2].i * 2.;
		x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    } else {
		i__1 = jj - 1;
		i__2 = jj - j;
		z__1.r = x[i__2].r * 2., z__1.i = x[i__2].i * 2.;
		x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    }
	    jj = jj - j - (j - 1);
	    j += -2;
	} else if (j == 1) {
	    i__1 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    --j;
	}

/*     UPLO = 'L':  Lower triangular storage */

    } else {
	n5 = *n / 5;
	n5 *= 5;

	jj = 1;
	i__1 = n5;
	for (j = 1; j <= i__1; j += 5) {
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = alpha3 * z__2.r, z__1.i = alpha3 * z__2.i;
	    a.r = z__1.r, a.i = z__1.i;
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = z__2.r / alpha, z__1.i = z__2.i / alpha;
	    b.r = z__1.r, b.i = z__1.i;
	    z__3.r = b.r * 2., z__3.i = b.i * 2.;
	    z__2.r = z__3.r * 0. - z__3.i * 1., z__2.i = z__3.r * 1. + z__3.i 
		    * 0.;
	    z__1.r = a.r - z__2.r, z__1.i = a.i - z__2.i;
	    c__.r = z__1.r, c__.i = z__1.i;
	    z__1.r = c__.r / beta, z__1.i = c__.i / beta;
	    r__.r = z__1.r, r__.i = z__1.i;
	    i__2 = jj;
	    x[i__2].r = a.r, x[i__2].i = a.i;
	    i__2 = jj + 2;
	    x[i__2].r = b.r, x[i__2].i = b.i;
	    jj += *n - j + 1;
	    i__2 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    i__2 = jj + 1;
	    x[i__2].r = r__.r, x[i__2].i = r__.i;
	    jj += *n - j;
	    i__2 = jj;
	    x[i__2].r = c__.r, x[i__2].i = c__.i;
	    jj += *n - j - 1;
	    i__2 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    jj += *n - j - 2;
	    i__2 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    if (z_abs(&x[jj - (*n - j - 2)]) > z_abs(&x[jj])) {
		i__2 = jj - (*n - j - 2) + 1;
		i__3 = jj - (*n - j - 2);
		z__1.r = x[i__3].r * 2., z__1.i = x[i__3].i * 2.;
		x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    } else {
		i__2 = jj - (*n - j - 2) + 1;
		i__3 = jj;
		z__1.r = x[i__3].r * 2., z__1.i = x[i__3].i * 2.;
		x[i__2].r = z__1.r, x[i__2].i = z__1.i;
	    }
	    jj += *n - j - 3;
/* L30: */
	}

/*        Clean-up for N not a multiple of 5. */

	j = n5 + 1;
	if (j < *n - 1) {
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = alpha3 * z__2.r, z__1.i = alpha3 * z__2.i;
	    a.r = z__1.r, a.i = z__1.i;
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = z__2.r / alpha, z__1.i = z__2.i / alpha;
	    b.r = z__1.r, b.i = z__1.i;
	    z__3.r = b.r * 2., z__3.i = b.i * 2.;
	    z__2.r = z__3.r * 0. - z__3.i * 1., z__2.i = z__3.r * 1. + z__3.i 
		    * 0.;
	    z__1.r = a.r - z__2.r, z__1.i = a.i - z__2.i;
	    c__.r = z__1.r, c__.i = z__1.i;
	    z__1.r = c__.r / beta, z__1.i = c__.i / beta;
	    r__.r = z__1.r, r__.i = z__1.i;
	    i__1 = jj;
	    x[i__1].r = a.r, x[i__1].i = a.i;
	    i__1 = jj + 2;
	    x[i__1].r = b.r, x[i__1].i = b.i;
	    jj += *n - j + 1;
	    i__1 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    i__1 = jj + 1;
	    x[i__1].r = r__.r, x[i__1].i = r__.i;
	    jj += *n - j;
	    i__1 = jj;
	    x[i__1].r = c__.r, x[i__1].i = c__.i;
	    jj += *n - j - 1;
	    j += 3;
	}
	if (j < *n) {
	    i__1 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    i__1 = jj + (*n - j + 1);
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    if (z_abs(&x[jj]) > z_abs(&x[jj + (*n - j + 1)])) {
		i__1 = jj + 1;
		i__2 = jj;
		z__1.r = x[i__2].r * 2., z__1.i = x[i__2].i * 2.;
		x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    } else {
		i__1 = jj + 1;
		i__2 = jj + (*n - j + 1);
		z__1.r = x[i__2].r * 2., z__1.i = x[i__2].i * 2.;
		x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    }
	    jj = jj + (*n - j + 1) + (*n - j);
	    j += 2;
	} else if (j == *n) {
	    i__1 = jj;
	    zlarnd_(&z__1, &c__2, &iseed[1]);
	    x[i__1].r = z__1.r, x[i__1].i = z__1.i;
	    jj += *n - j + 1;
	    ++j;
	}
    }

    return 0;

/*     End of ZLATSP */

} /* zlatsp_ */
