#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {1.,0.};
static doublecomplex c_b2 = {0.,0.};
static integer c__1 = 1;

/* Subroutine */ int zlarfy_(char *uplo, integer *n, doublecomplex *v, 
	integer *incv, doublecomplex *tau, doublecomplex *c__, integer *ldc, 
	doublecomplex *work)
{
    /* System generated locals */
    integer c_dim1, c_offset;
    doublecomplex z__1, z__2, z__3, z__4;

    /* Local variables */
    extern /* Subroutine */ int zher2_(char *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    doublecomplex alpha;
    extern /* Double Complex */ VOID zdotc_(doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    extern /* Subroutine */ int zhemv_(char *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *), zaxpy_(
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLARFY applies an elementary reflector, or Householder matrix, H, */
/*  to an n x n Hermitian matrix C, from both the left and the right. */

/*  H is represented in the form */

/*     H = I - tau * v * v' */

/*  where  tau  is a scalar and  v  is a vector. */

/*  If  tau  is  zero, then  H  is taken to be the unit matrix. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          Hermitian matrix C is stored. */
/*          = 'U':  Upper triangle */
/*          = 'L':  Lower triangle */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix C.  N >= 0. */

/*  V       (input) COMPLEX*16 array, dimension */
/*                  (1 + (N-1)*abs(INCV)) */
/*          The vector v as described above. */

/*  INCV    (input) INTEGER */
/*          The increment between successive elements of v.  INCV must */
/*          not be zero. */

/*  TAU     (input) COMPLEX*16 */
/*          The value tau as described above. */

/*  C       (input/output) COMPLEX*16 array, dimension (LDC, N) */
/*          On entry, the matrix C. */
/*          On exit, C is overwritten by H * C * H'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C.  LDC >= max( 1, N ). */

/*  WORK    (workspace) COMPLEX*16 array, dimension (N) */

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
    if (tau->r == 0. && tau->i == 0.) {
	return 0;
    }

/*     Form  w:= C * v */

    zhemv_(uplo, n, &c_b1, &c__[c_offset], ldc, &v[1], incv, &c_b2, &work[1], 
	    &c__1);

    z__3.r = -.5, z__3.i = -0.;
    z__2.r = z__3.r * tau->r - z__3.i * tau->i, z__2.i = z__3.r * tau->i + 
	    z__3.i * tau->r;
    zdotc_(&z__4, n, &work[1], &c__1, &v[1], incv);
    z__1.r = z__2.r * z__4.r - z__2.i * z__4.i, z__1.i = z__2.r * z__4.i + 
	    z__2.i * z__4.r;
    alpha.r = z__1.r, alpha.i = z__1.i;
    zaxpy_(n, &alpha, &v[1], incv, &work[1], &c__1);

/*     C := C - v * w' - w * v' */

    z__1.r = -tau->r, z__1.i = -tau->i;
    zher2_(uplo, n, &z__1, &v[1], incv, &work[1], &c__1, &c__[c_offset], ldc);

    return 0;

/*     End of ZLARFY */

} /* zlarfy_ */
