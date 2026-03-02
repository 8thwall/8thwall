#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b2 = 1.f;
static real c_b3 = 0.f;
static integer c__1 = 1;

/* Subroutine */ int slarfy_(char *uplo, integer *n, real *v, integer *incv, 
	real *tau, real *c__, integer *ldc, real *work)
{
    /* System generated locals */
    integer c_dim1, c_offset;
    real r__1;

    /* Local variables */
    extern doublereal sdot_(integer *, real *, integer *, real *, integer *);
    extern /* Subroutine */ int ssyr2_(char *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *);
    real alpha;
    extern /* Subroutine */ int saxpy_(integer *, real *, real *, integer *, 
	    real *, integer *), ssymv_(char *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, real *, integer *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLARFY applies an elementary reflector, or Householder matrix, H, */
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

/*  V       (input) REAL array, dimension */
/*                  (1 + (N-1)*abs(INCV)) */
/*          The vector v as described above. */

/*  INCV    (input) INTEGER */
/*          The increment between successive elements of v.  INCV must */
/*          not be zero. */

/*  TAU     (input) REAL */
/*          The value tau as described above. */

/*  C       (input/output) REAL array, dimension (LDC, N) */
/*          On entry, the matrix C. */
/*          On exit, C is overwritten by H * C * H'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C.  LDC >= max( 1, N ). */

/*  WORK    (workspace) REAL array, dimension (N) */

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
    if (*tau == 0.f) {
	return 0;
    }

/*     Form  w:= C * v */

    ssymv_(uplo, n, &c_b2, &c__[c_offset], ldc, &v[1], incv, &c_b3, &work[1], 
	    &c__1);

    alpha = *tau * -.5f * sdot_(n, &work[1], &c__1, &v[1], incv);
    saxpy_(n, &alpha, &v[1], incv, &work[1], &c__1);

/*     C := C - v * w' - w * v' */

    r__1 = -(*tau);
    ssyr2_(uplo, n, &r__1, &v[1], incv, &work[1], &c__1, &c__[c_offset], ldc);

    return 0;

/*     End of SLARFY */

} /* slarfy_ */
