#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__5 = 5;
static integer c__2 = 2;

/* Subroutine */ int clatsp_(char *uplo, integer *n, complex *x, integer *
	iseed)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    complex q__1, q__2, q__3;

    /* Builtin functions */
    double sqrt(doublereal), c_abs(complex *);

    /* Local variables */
    complex a, b, c__;
    integer j;
    complex r__;
    integer n5, jj;
    real beta, alpha, alpha3;
    extern /* Complex */ VOID clarnd_(complex *, integer *, integer *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLATSP generates a special test matrix for the complex symmetric */
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

/*  X       (output) COMPLEX array, dimension (N*(N+1)/2) */
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
    alpha = (sqrt(17.f) + 1.f) / 8.f;
    beta = alpha - .001f;
    alpha3 = alpha * alpha * alpha;

/*     Fill the matrix with zeros. */

    i__1 = *n * (*n + 1) / 2;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j;
	x[i__2].r = 0.f, x[i__2].i = 0.f;
/* L10: */
    }

/*     UPLO = 'U':  Upper triangular storage */

    if (*(unsigned char *)uplo == 'U') {
	n5 = *n / 5;
	n5 = *n - n5 * 5 + 1;

	jj = *n * (*n + 1) / 2;
	i__1 = n5;
	for (j = *n; j >= i__1; j += -5) {
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = alpha3 * q__2.r, q__1.i = alpha3 * q__2.i;
	    a.r = q__1.r, a.i = q__1.i;
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = q__2.r / alpha, q__1.i = q__2.i / alpha;
	    b.r = q__1.r, b.i = q__1.i;
	    q__3.r = b.r * 2.f, q__3.i = b.i * 2.f;
	    q__2.r = q__3.r * 0.f - q__3.i * 1.f, q__2.i = q__3.r * 1.f + 
		    q__3.i * 0.f;
	    q__1.r = a.r - q__2.r, q__1.i = a.i - q__2.i;
	    c__.r = q__1.r, c__.i = q__1.i;
	    q__1.r = c__.r / beta, q__1.i = c__.i / beta;
	    r__.r = q__1.r, r__.i = q__1.i;
	    i__2 = jj;
	    x[i__2].r = a.r, x[i__2].i = a.i;
	    i__2 = jj - 2;
	    x[i__2].r = b.r, x[i__2].i = b.i;
	    jj -= j;
	    i__2 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    i__2 = jj - 1;
	    x[i__2].r = r__.r, x[i__2].i = r__.i;
	    jj -= j - 1;
	    i__2 = jj;
	    x[i__2].r = c__.r, x[i__2].i = c__.i;
	    jj -= j - 2;
	    i__2 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    jj -= j - 3;
	    i__2 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    if (c_abs(&x[jj + (j - 3)]) > c_abs(&x[jj])) {
		i__2 = jj + (j - 4);
		i__3 = jj + (j - 3);
		q__1.r = x[i__3].r * 2.f, q__1.i = x[i__3].i * 2.f;
		x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    } else {
		i__2 = jj + (j - 4);
		i__3 = jj;
		q__1.r = x[i__3].r * 2.f, q__1.i = x[i__3].i * 2.f;
		x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    }
	    jj -= j - 4;
/* L20: */
	}

/*        Clean-up for N not a multiple of 5. */

	j = n5 - 1;
	if (j > 2) {
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = alpha3 * q__2.r, q__1.i = alpha3 * q__2.i;
	    a.r = q__1.r, a.i = q__1.i;
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = q__2.r / alpha, q__1.i = q__2.i / alpha;
	    b.r = q__1.r, b.i = q__1.i;
	    q__3.r = b.r * 2.f, q__3.i = b.i * 2.f;
	    q__2.r = q__3.r * 0.f - q__3.i * 1.f, q__2.i = q__3.r * 1.f + 
		    q__3.i * 0.f;
	    q__1.r = a.r - q__2.r, q__1.i = a.i - q__2.i;
	    c__.r = q__1.r, c__.i = q__1.i;
	    q__1.r = c__.r / beta, q__1.i = c__.i / beta;
	    r__.r = q__1.r, r__.i = q__1.i;
	    i__1 = jj;
	    x[i__1].r = a.r, x[i__1].i = a.i;
	    i__1 = jj - 2;
	    x[i__1].r = b.r, x[i__1].i = b.i;
	    jj -= j;
	    i__1 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
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
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    i__1 = jj - j;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    if (c_abs(&x[jj]) > c_abs(&x[jj - j])) {
		i__1 = jj - 1;
		i__2 = jj;
		q__1.r = x[i__2].r * 2.f, q__1.i = x[i__2].i * 2.f;
		x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    } else {
		i__1 = jj - 1;
		i__2 = jj - j;
		q__1.r = x[i__2].r * 2.f, q__1.i = x[i__2].i * 2.f;
		x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    }
	    jj = jj - j - (j - 1);
	    j += -2;
	} else if (j == 1) {
	    i__1 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    --j;
	}

/*     UPLO = 'L':  Lower triangular storage */

    } else {
	n5 = *n / 5;
	n5 *= 5;

	jj = 1;
	i__1 = n5;
	for (j = 1; j <= i__1; j += 5) {
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = alpha3 * q__2.r, q__1.i = alpha3 * q__2.i;
	    a.r = q__1.r, a.i = q__1.i;
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = q__2.r / alpha, q__1.i = q__2.i / alpha;
	    b.r = q__1.r, b.i = q__1.i;
	    q__3.r = b.r * 2.f, q__3.i = b.i * 2.f;
	    q__2.r = q__3.r * 0.f - q__3.i * 1.f, q__2.i = q__3.r * 1.f + 
		    q__3.i * 0.f;
	    q__1.r = a.r - q__2.r, q__1.i = a.i - q__2.i;
	    c__.r = q__1.r, c__.i = q__1.i;
	    q__1.r = c__.r / beta, q__1.i = c__.i / beta;
	    r__.r = q__1.r, r__.i = q__1.i;
	    i__2 = jj;
	    x[i__2].r = a.r, x[i__2].i = a.i;
	    i__2 = jj + 2;
	    x[i__2].r = b.r, x[i__2].i = b.i;
	    jj += *n - j + 1;
	    i__2 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    i__2 = jj + 1;
	    x[i__2].r = r__.r, x[i__2].i = r__.i;
	    jj += *n - j;
	    i__2 = jj;
	    x[i__2].r = c__.r, x[i__2].i = c__.i;
	    jj += *n - j - 1;
	    i__2 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    jj += *n - j - 2;
	    i__2 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    if (c_abs(&x[jj - (*n - j - 2)]) > c_abs(&x[jj])) {
		i__2 = jj - (*n - j - 2) + 1;
		i__3 = jj - (*n - j - 2);
		q__1.r = x[i__3].r * 2.f, q__1.i = x[i__3].i * 2.f;
		x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    } else {
		i__2 = jj - (*n - j - 2) + 1;
		i__3 = jj;
		q__1.r = x[i__3].r * 2.f, q__1.i = x[i__3].i * 2.f;
		x[i__2].r = q__1.r, x[i__2].i = q__1.i;
	    }
	    jj += *n - j - 3;
/* L30: */
	}

/*        Clean-up for N not a multiple of 5. */

	j = n5 + 1;
	if (j < *n - 1) {
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = alpha3 * q__2.r, q__1.i = alpha3 * q__2.i;
	    a.r = q__1.r, a.i = q__1.i;
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = q__2.r / alpha, q__1.i = q__2.i / alpha;
	    b.r = q__1.r, b.i = q__1.i;
	    q__3.r = b.r * 2.f, q__3.i = b.i * 2.f;
	    q__2.r = q__3.r * 0.f - q__3.i * 1.f, q__2.i = q__3.r * 1.f + 
		    q__3.i * 0.f;
	    q__1.r = a.r - q__2.r, q__1.i = a.i - q__2.i;
	    c__.r = q__1.r, c__.i = q__1.i;
	    q__1.r = c__.r / beta, q__1.i = c__.i / beta;
	    r__.r = q__1.r, r__.i = q__1.i;
	    i__1 = jj;
	    x[i__1].r = a.r, x[i__1].i = a.i;
	    i__1 = jj + 2;
	    x[i__1].r = b.r, x[i__1].i = b.i;
	    jj += *n - j + 1;
	    i__1 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
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
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    i__1 = jj + (*n - j + 1);
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    if (c_abs(&x[jj]) > c_abs(&x[jj + (*n - j + 1)])) {
		i__1 = jj + 1;
		i__2 = jj;
		q__1.r = x[i__2].r * 2.f, q__1.i = x[i__2].i * 2.f;
		x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    } else {
		i__1 = jj + 1;
		i__2 = jj + (*n - j + 1);
		q__1.r = x[i__2].r * 2.f, q__1.i = x[i__2].i * 2.f;
		x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    }
	    jj = jj + (*n - j + 1) + (*n - j);
	    j += 2;
	} else if (j == *n) {
	    i__1 = jj;
	    clarnd_(&q__1, &c__2, &iseed[1]);
	    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
	    jj += *n - j + 1;
	    ++j;
	}
    }

    return 0;

/*     End of CLATSP */

} /* clatsp_ */
