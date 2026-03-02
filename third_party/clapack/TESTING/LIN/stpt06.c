#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int stpt06_(real *rcond, real *rcondc, char *uplo, char *
	diag, integer *n, real *ap, real *work, real *rat)
{
    /* System generated locals */
    real r__1, r__2;

    /* Local variables */
    real eps, rmin, rmax, anorm;
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal slamch_(char *);
    real bignum;
    extern doublereal slantp_(char *, char *, char *, integer *, real *, real 
	    *);
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

/*  STPT06 computes a test ratio comparing RCOND (the reciprocal */
/*  condition number of a triangular matrix A) and RCONDC, the estimate */
/*  computed by STPCON.  Information about the triangular matrix A is */
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
/*          STPCON. */

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

/*  AP      (input) REAL array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP((j-1)*j/2 + i) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', */
/*             AP((j-1)*(n-j) + j*(j+1)/2 + i-j) = A(i,j) for j<=i<=n. */

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
    --work;
    --ap;

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
	anorm = slantp_("M", uplo, diag, n, &ap[1], &work[1]);

/* Computing MIN */
	r__1 = bignum / dmax(1.f,anorm), r__2 = 1.f / eps;
	*rat = rmax * dmin(r__1,r__2);
    }

    return 0;

/*     End of STPT06 */

} /* stpt06_ */
