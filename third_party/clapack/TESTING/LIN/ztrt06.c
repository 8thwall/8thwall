#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int ztrt06_(doublereal *rcond, doublereal *rcondc, char *
	uplo, char *diag, integer *n, doublecomplex *a, integer *lda, 
	doublereal *rwork, doublereal *rat)
{
    /* System generated locals */
    integer a_dim1, a_offset;
    doublereal d__1, d__2;

    /* Local variables */
    doublereal eps, rmin, rmax, anorm;
    extern doublereal dlamch_(char *);
    doublereal bignum;
    extern doublereal zlantr_(char *, char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZTRT06 computes a test ratio comparing RCOND (the reciprocal */
/*  condition number of a triangular matrix A) and RCONDC, the estimate */
/*  computed by ZTRCON.  Information about the triangular matrix A is */
/*  used if one estimate is zero and the other is non-zero to decide if */
/*  underflow in the estimate is justified. */

/*  Arguments */
/*  ========= */

/*  RCOND   (input) DOUBLE PRECISION */
/*          The estimate of the reciprocal condition number obtained by */
/*          forming the explicit inverse of the matrix A and computing */
/*          RCOND = 1/( norm(A) * norm(inv(A)) ). */

/*  RCONDC  (input) DOUBLE PRECISION */
/*          The estimate of the reciprocal condition number computed by */
/*          ZTRCON. */

/*  UPLO    (input) CHARACTER */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  DIAG    (input) CHARACTER */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading n by n */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading n by n lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix, and the strictly upper triangular part of A is not */
/*          referenced.  If DIAG = 'U', the diagonal elements of A are */
/*          also not referenced and are assumed to be 1. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RAT     (output) DOUBLE PRECISION */
/*          The test ratio.  If both RCOND and RCONDC are nonzero, */
/*             RAT = MAX( RCOND, RCONDC )/MIN( RCOND, RCONDC ) - 1. */
/*          If RAT = 0, the two estimates are exactly the same. */

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

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --rwork;

    /* Function Body */
    eps = dlamch_("Epsilon");
    rmax = max(*rcond,*rcondc);
    rmin = min(*rcond,*rcondc);

/*     Do the easy cases first. */

    if (rmin < 0.) {

/*        Invalid value for RCOND or RCONDC, return 1/EPS. */

	*rat = 1. / eps;

    } else if (rmin > 0.) {

/*        Both estimates are positive, return RMAX/RMIN - 1. */

	*rat = rmax / rmin - 1.;

    } else if (rmax == 0.) {

/*        Both estimates zero. */

	*rat = 0.;

    } else {

/*        One estimate is zero, the other is non-zero.  If the matrix is */
/*        ill-conditioned, return the nonzero estimate multiplied by */
/*        1/EPS; if the matrix is badly scaled, return the nonzero */
/*        estimate multiplied by BIGNUM/TMAX, where TMAX is the maximum */
/*        element in absolute value in A. */

	bignum = 1. / dlamch_("Safe minimum");
	anorm = zlantr_("M", uplo, diag, n, n, &a[a_offset], lda, &rwork[1]);

/* Computing MIN */
	d__1 = bignum / max(1.,anorm), d__2 = 1. / eps;
	*rat = rmax * min(d__1,d__2);
    }

    return 0;

/*     End of ZTRT06 */

} /* ztrt06_ */
