#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {1.f,0.f};
static complex c_b2 = {0.f,0.f};
static integer c__1 = 1;

/* Subroutine */ int clarfy_(char *uplo, integer *n, complex *v, integer *
	incv, complex *tau, complex *c__, integer *ldc, complex *work)
{
    /* System generated locals */
    integer c_dim1, c_offset;
    complex q__1, q__2, q__3, q__4;

    /* Local variables */
    extern /* Subroutine */ int cher2_(char *, integer *, complex *, complex *
, integer *, complex *, integer *, complex *, integer *);
    complex alpha;
    extern /* Complex */ VOID cdotc_(complex *, integer *, complex *, integer 
	    *, complex *, integer *);
    extern /* Subroutine */ int chemv_(char *, integer *, complex *, complex *
, integer *, complex *, integer *, complex *, complex *, integer *
), caxpy_(integer *, complex *, complex *, integer *, 
	    complex *, integer *);


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLARFY applies an elementary reflector, or Householder matrix, H, */
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

/*  V       (input) COMPLEX array, dimension */
/*                  (1 + (N-1)*abs(INCV)) */
/*          The vector v as described above. */

/*  INCV    (input) INTEGER */
/*          The increment between successive elements of v.  INCV must */
/*          not be zero. */

/*  TAU     (input) COMPLEX */
/*          The value tau as described above. */

/*  C       (input/output) COMPLEX array, dimension (LDC, N) */
/*          On entry, the matrix C. */
/*          On exit, C is overwritten by H * C * H'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C.  LDC >= max( 1, N ). */

/*  WORK    (workspace) COMPLEX array, dimension (N) */

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
    if (tau->r == 0.f && tau->i == 0.f) {
	return 0;
    }

/*     Form  w:= C * v */

    chemv_(uplo, n, &c_b1, &c__[c_offset], ldc, &v[1], incv, &c_b2, &work[1], 
	    &c__1);

    q__3.r = -.5f, q__3.i = -0.f;
    q__2.r = q__3.r * tau->r - q__3.i * tau->i, q__2.i = q__3.r * tau->i + 
	    q__3.i * tau->r;
    cdotc_(&q__4, n, &work[1], &c__1, &v[1], incv);
    q__1.r = q__2.r * q__4.r - q__2.i * q__4.i, q__1.i = q__2.r * q__4.i + 
	    q__2.i * q__4.r;
    alpha.r = q__1.r, alpha.i = q__1.i;
    caxpy_(n, &alpha, &v[1], incv, &work[1], &c__1);

/*     C := C - v * w' - w * v' */

    q__1.r = -tau->r, q__1.i = -tau->i;
    cher2_(uplo, n, &q__1, &v[1], incv, &work[1], &c__1, &c__[c_offset], ldc);

    return 0;

/*     End of CLARFY */

} /* clarfy_ */
