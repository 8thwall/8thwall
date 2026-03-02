#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int stbt06_(real *rcond, real *rcondc, char *uplo, char *
	diag, integer *n, integer *kd, real *ab, integer *ldab, real *work, 
	real *rat)
{
    /* System generated locals */
    integer ab_dim1, ab_offset;
    real r__1, r__2;

    /* Local variables */
    real eps, rmin, rmax, anorm;
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal slamch_(char *);
    real bignum;
    extern doublereal slantb_(char *, char *, char *, integer *, integer *, 
	    real *, integer *, real *);
    real smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  STBT06 computes a test ratio comparing RCOND (the reciprocal */
/*  condition number of a triangular matrix A) and RCONDC, the estimate */
/*  computed by STBCON.  Information about the triangular matrix A is */
/*  used if one estimate is zero and the other is non-zero to decide if */
/*  underflow in the estimate is justified. */

/*  Arguments */
/*  ========= */

/*  RCOND   (input) REAL */
/*          The estimate of the reciprocal condition number obtained by */
/*          forming the explicit inverse of the matrix A and computing */
/*          RCOND = 1/( norm(A) * norm(inv(A)) ). */

/*  RCONDC  (input) REAL */
/*          The estimate of the reciprocal condition number computed by */
/*          STBCON. */

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

/*  KD      (input) INTEGER */
/*          The number of superdiagonals or subdiagonals of the */
/*          triangular band matrix A.  KD >= 0. */

/*  AB      (input) REAL array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first kd+1 rows of the array. The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  WORK    (workspace) REAL array, dimension (N) */

/*  RAT     (output) REAL */
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
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --work;

    /* Function Body */
    eps = slamch_("Epsilon");
    rmax = dmax(*rcond,*rcondc);
    rmin = dmin(*rcond,*rcondc);

/*     Do the easy cases first. */

    if (rmin < 0.f) {

/*        Invalid value for RCOND or RCONDC, return 1/EPS. */

	*rat = 1.f / eps;

    } else if (rmin > 0.f) {

/*        Both estimates are positive, return RMAX/RMIN - 1. */

	*rat = rmax / rmin - 1.f;

    } else if (rmax == 0.f) {

/*        Both estimates zero. */

	*rat = 0.f;

    } else {

/*        One estimate is zero, the other is non-zero.  If the matrix is */
/*        ill-conditioned, return the nonzero estimate multiplied by */
/*        1/EPS; if the matrix is badly scaled, return the nonzero */
/*        estimate multiplied by BIGNUM/TMAX, where TMAX is the maximum */
/*        element in absolute value in A. */

	smlnum = slamch_("Safe minimum");
	bignum = 1.f / smlnum;
	slabad_(&smlnum, &bignum);
	anorm = slantb_("M", uplo, diag, n, kd, &ab[ab_offset], ldab, &work[1]
);

/* Computing MIN */
	r__1 = bignum / dmax(1.f,anorm), r__2 = 1.f / eps;
	*rat = rmax * dmin(r__1,r__2);
    }

    return 0;

/*     End of STBT06 */

} /* stbt06_ */
