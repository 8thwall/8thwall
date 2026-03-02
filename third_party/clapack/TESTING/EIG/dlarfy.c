#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b2 = 1.;
static doublereal c_b3 = 0.;
static integer c__1 = 1;

/* Subroutine */ int dlarfy_(char *uplo, integer *n, doublereal *v, integer *
	incv, doublereal *tau, doublereal *c__, integer *ldc, doublereal *
	work)
{
    /* System generated locals */
    integer c_dim1, c_offset;
    doublereal d__1;

    /* Local variables */
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    extern /* Subroutine */ int dsyr2_(char *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    doublereal alpha;
    extern /* Subroutine */ int daxpy_(integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *), dsymv_(char *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARFY applies an elementary reflector, or Householder matrix, H, */
/*  to an n x n symmetric matrix C, from both the left and the right. */

/*  H is represented in the form */

/*     H = I - tau * v * v' */

/*  where  tau  is a scalar and  v  is a vector. */

/*  If  tau  is  zero, then  H  is taken to be the unit matrix. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix C is stored. */
/*          = 'U':  Upper triangle */
/*          = 'L':  Lower triangle */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix C.  N >= 0. */

/*  V       (input) DOUBLE PRECISION array, dimension */
/*                  (1 + (N-1)*abs(INCV)) */
/*          The vector v as described above. */

/*  INCV    (input) INTEGER */
/*          The increment between successive elements of v.  INCV must */
/*          not be zero. */

/*  TAU     (input) DOUBLE PRECISION */
/*          The value tau as described above. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC, N) */
/*          On entry, the matrix C. */
/*          On exit, C is overwritten by H * C * H'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C.  LDC >= max( 1, N ). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    if (*tau == 0.) {
	return 0;
    }

/*     Form  w:= C * v */

    dsymv_(uplo, n, &c_b2, &c__[c_offset], ldc, &v[1], incv, &c_b3, &work[1], 
	    &c__1);

    alpha = *tau * -.5 * ddot_(n, &work[1], &c__1, &v[1], incv);
    daxpy_(n, &alpha, &v[1], incv, &work[1], &c__1);

/*     C := C - v * w' - w * v' */

    d__1 = -(*tau);
    dsyr2_(uplo, n, &d__1, &v[1], incv, &work[1], &c__1, &c__[c_offset], ldc);

    return 0;

/*     End of DLARFY */

} /* dlarfy_ */
