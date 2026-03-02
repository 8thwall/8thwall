#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int dtbt05_(char *uplo, char *trans, char *diag, integer *n, 
	integer *kd, integer *nrhs, doublereal *ab, integer *ldab, doublereal 
	*b, integer *ldb, doublereal *x, integer *ldx, doublereal *xact, 
	integer *ldxact, doublereal *ferr, doublereal *berr, doublereal *
	reslts)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, b_dim1, b_offset, x_dim1, x_offset, xact_dim1,
	     xact_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k, nz, ifu;
    doublereal eps, tmp, diff, axbi;
    integer imax;
    doublereal unfl, ovfl;
    logical unit;
    extern logical lsame_(char *, char *);
    logical upper;
    doublereal xnorm;
    extern doublereal dlamch_(char *);
    extern integer idamax_(integer *, doublereal *, integer *);
    doublereal errbnd;
    logical notran;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTBT05 tests the error bounds from iterative refinement for the */
/*  computed solution to a system of equations A*X = B, where A is a */
/*  triangular band matrix. */

/*  RESLTS(1) = test of the error bound */
/*            = norm(X - XACT) / ( norm(X) * FERR ) */

/*  A large value is returned if this ratio is not less than one. */

/*  RESLTS(2) = residual from the iterative refinement routine */
/*            = the maximum of BERR / ( NZ*EPS + (*) ), where */
/*              (*) = NZ*UNFL / (min_i (abs(A)*abs(X) +abs(b))_i ) */
/*              and NZ = max. number of nonzeros in any row of A, plus 1 */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations. */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A'* X = B  (Transpose) */
/*          = 'C':  A'* X = B  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices X, B, and XACT, and the */
/*          order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of super-diagonals of the matrix A if UPLO = 'U', */
/*          or the number of sub-diagonals if UPLO = 'L'.  KD >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of the matrices X, B, and XACT. */
/*          NRHS >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first kd+1 rows of the array. The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */
/*          If DIAG = 'U', the diagonal elements of A are not referenced */
/*          and are assumed to be 1. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side vectors for the system of linear */
/*          equations. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The computed solution vectors.  Each vector is stored as a */
/*          column of the matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  XACT    (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The exact solution vectors.  Each vector is stored as a */
/*          column of the matrix XACT. */

/*  LDXACT  (input) INTEGER */
/*          The leading dimension of the array XACT.  LDXACT >= max(1,N). */

/*  FERR    (input) DOUBLE PRECISION array, dimension (NRHS) */
/*          The estimated forward error bounds for each solution vector */
/*          X.  If XTRUE is the true solution, FERR bounds the magnitude */
/*          of the largest entry in (X - XTRUE) divided by the magnitude */
/*          of the largest entry in X. */

/*  BERR    (input) DOUBLE PRECISION array, dimension (NRHS) */
/*          The componentwise relative backward error of each solution */
/*          vector (i.e., the smallest relative change in any entry of A */
/*          or B that makes X an exact solution). */

/*  RESLTS  (output) DOUBLE PRECISION array, dimension (2) */
/*          The maximum over the NRHS solution vectors of the ratios: */
/*          RESLTS(1) = norm(X - XACT) / ( norm(X) * FERR ) */
/*          RESLTS(2) = BERR / ( NZ*EPS + (*) ) */

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

/*     Quick exit if N = 0 or NRHS = 0. */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    xact_dim1 = *ldxact;
    xact_offset = 1 + xact_dim1;
    xact -= xact_offset;
    --ferr;
    --berr;
    --reslts;

    /* Function Body */
    if (*n <= 0 || *nrhs <= 0) {
	reslts[1] = 0.;
	reslts[2] = 0.;
	return 0;
    }

    eps = dlamch_("Epsilon");
    unfl = dlamch_("Safe minimum");
    ovfl = 1. / unfl;
    upper = lsame_(uplo, "U");
    notran = lsame_(trans, "N");
    unit = lsame_(diag, "U");
/* Computing MIN */
    i__1 = *kd, i__2 = *n - 1;
    nz = min(i__1,i__2) + 1;

/*     Test 1:  Compute the maximum of */
/*        norm(X - XACT) / ( norm(X) * FERR ) */
/*     over all the vectors X and XACT using the infinity-norm. */

    errbnd = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	imax = idamax_(n, &x[j * x_dim1 + 1], &c__1);
/* Computing MAX */
	d__2 = (d__1 = x[imax + j * x_dim1], abs(d__1));
	xnorm = max(d__2,unfl);
	diff = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = diff, d__3 = (d__1 = x[i__ + j * x_dim1] - xact[i__ + j * 
		    xact_dim1], abs(d__1));
	    diff = max(d__2,d__3);
/* L10: */
	}

	if (xnorm > 1.) {
	    goto L20;
	} else if (diff <= ovfl * xnorm) {
	    goto L20;
	} else {
	    errbnd = 1. / eps;
	    goto L30;
	}

L20:
	if (diff / xnorm <= ferr[j]) {
/* Computing MAX */
	    d__1 = errbnd, d__2 = diff / xnorm / ferr[j];
	    errbnd = max(d__1,d__2);
	} else {
	    errbnd = 1. / eps;
	}
L30:
	;
    }
    reslts[1] = errbnd;

/*     Test 2:  Compute the maximum of BERR / ( NZ*EPS + (*) ), where */
/*     (*) = NZ*UNFL / (min_i (abs(A)*abs(X) +abs(b))_i ) */

    ifu = 0;
    if (unit) {
	ifu = 1;
    }
    i__1 = *nrhs;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    tmp = (d__1 = b[i__ + k * b_dim1], abs(d__1));
	    if (upper) {
		if (! notran) {
/* Computing MAX */
		    i__3 = i__ - *kd;
		    i__4 = i__ - ifu;
		    for (j = max(i__3,1); j <= i__4; ++j) {
			tmp += (d__1 = ab[*kd + 1 - i__ + j + i__ * ab_dim1], 
				abs(d__1)) * (d__2 = x[j + k * x_dim1], abs(
				d__2));
/* L40: */
		    }
		    if (unit) {
			tmp += (d__1 = x[i__ + k * x_dim1], abs(d__1));
		    }
		} else {
		    if (unit) {
			tmp += (d__1 = x[i__ + k * x_dim1], abs(d__1));
		    }
/* Computing MIN */
		    i__3 = i__ + *kd;
		    i__4 = min(i__3,*n);
		    for (j = i__ + ifu; j <= i__4; ++j) {
			tmp += (d__1 = ab[*kd + 1 + i__ - j + j * ab_dim1], 
				abs(d__1)) * (d__2 = x[j + k * x_dim1], abs(
				d__2));
/* L50: */
		    }
		}
	    } else {
		if (notran) {
/* Computing MAX */
		    i__4 = i__ - *kd;
		    i__3 = i__ - ifu;
		    for (j = max(i__4,1); j <= i__3; ++j) {
			tmp += (d__1 = ab[i__ + 1 - j + j * ab_dim1], abs(
				d__1)) * (d__2 = x[j + k * x_dim1], abs(d__2))
				;
/* L60: */
		    }
		    if (unit) {
			tmp += (d__1 = x[i__ + k * x_dim1], abs(d__1));
		    }
		} else {
		    if (unit) {
			tmp += (d__1 = x[i__ + k * x_dim1], abs(d__1));
		    }
/* Computing MIN */
		    i__4 = i__ + *kd;
		    i__3 = min(i__4,*n);
		    for (j = i__ + ifu; j <= i__3; ++j) {
			tmp += (d__1 = ab[j + 1 - i__ + i__ * ab_dim1], abs(
				d__1)) * (d__2 = x[j + k * x_dim1], abs(d__2))
				;
/* L70: */
		    }
		}
	    }
	    if (i__ == 1) {
		axbi = tmp;
	    } else {
		axbi = min(axbi,tmp);
	    }
/* L80: */
	}
/* Computing MAX */
	d__1 = axbi, d__2 = nz * unfl;
	tmp = berr[k] / (nz * eps + nz * unfl / max(d__1,d__2));
	if (k == 1) {
	    reslts[2] = tmp;
	} else {
	    reslts[2] = max(reslts[2],tmp);
	}
/* L90: */
    }

    return 0;

/*     End of DTBT05 */

} /* dtbt05_ */
