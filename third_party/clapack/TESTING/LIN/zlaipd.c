#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int zlaipd_(integer *n, doublecomplex *a, integer *inda, 
	integer *vinda)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1;
    doublecomplex z__1;

    /* Local variables */
    integer i__, ia, ixa;
    extern doublereal dlamch_(char *);
    doublereal bignum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLAIPD sets the imaginary part of the diagonal elements of a complex */
/*  matrix A to a large value.  This is used to test LAPACK routines for */
/*  complex Hermitian matrices, which are not supposed to access or use */
/*  the imaginary parts of the diagonals. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         The number of diagonal elements of A. */

/*  A      (input/output) COMPLEX*16 array, dimension */
/*                        (1+(N-1)*INDA+(N-2)*VINDA) */
/*         On entry, the complex (Hermitian) matrix A. */
/*         On exit, the imaginary parts of the diagonal elements are set */
/*         to BIGNUM = EPS / SAFMIN, where EPS is the machine epsilon and */
/*         SAFMIN is the safe minimum. */

/*  INDA   (input) INTEGER */
/*         The increment between A(1) and the next diagonal element of A. */
/*         Typical values are */
/*         = LDA+1:  square matrices with leading dimension LDA */
/*         = 2:  packed upper triangular matrix, starting at A(1,1) */
/*         = N:  packed lower triangular matrix, starting at A(1,1) */

/*  VINDA  (input) INTEGER */
/*         The change in the diagonal increment between columns of A. */
/*         Typical values are */
/*         = 0:  no change, the row and column increments in A are fixed */
/*         = 1:  packed upper triangular matrix */
/*         = -1:  packed lower triangular matrix */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --a;

    /* Function Body */
    bignum = dlamch_("Epsilon") / dlamch_("Safe minimum");
    ia = 1;
    ixa = *inda;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = ia;
	i__3 = ia;
	d__1 = a[i__3].r;
	z__1.r = d__1, z__1.i = bignum;
	a[i__2].r = z__1.r, a[i__2].i = z__1.i;
	ia += ixa;
	ixa += *vinda;
/* L10: */
    }
    return 0;
} /* zlaipd_ */
