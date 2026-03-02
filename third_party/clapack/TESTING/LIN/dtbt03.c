#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int dtbt03_(char *uplo, char *trans, char *diag, integer *n, 
	integer *kd, integer *nrhs, doublereal *ab, integer *ldab, doublereal 
	*scale, doublereal *cnorm, doublereal *tscal, doublereal *x, integer *
	ldx, doublereal *b, integer *ldb, doublereal *work, doublereal *resid)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, b_dim1, b_offset, x_dim1, x_offset, i__1;
    doublereal d__1, d__2, d__3;

    /* Local variables */
    integer j, ix;
    doublereal eps, err;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *);
    extern logical lsame_(char *, char *);
    doublereal xscal;
    extern /* Subroutine */ int dtbmv_(char *, char *, char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *), dcopy_(integer *, doublereal *, integer *
, doublereal *, integer *), daxpy_(integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal tnorm, xnorm;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    extern integer idamax_(integer *, doublereal *, integer *);
    doublereal bignum, smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTBT03 computes the residual for the solution to a scaled triangular */
/*  system of equations  A*x = s*b  or  A'*x = s*b  when A is a */
/*  triangular band matrix. Here A' is the transpose of A, s is a scalar, */
/*  and x and b are N by NRHS matrices.  The test ratio is the maximum */
/*  over the number of right hand sides of */
/*     norm(s*b - op(A)*x) / ( norm(op(A)) * norm(x) * EPS ), */
/*  where op(A) denotes A or A' and EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to A. */
/*          = 'N':  A *x = b  (No transpose) */
/*          = 'T':  A'*x = b  (Transpose) */
/*          = 'C':  A'*x = b  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals or subdiagonals of the */
/*          triangular band matrix A.  KD >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices X and B.  NRHS >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first kd+1 rows of the array. The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  SCALE   (input) DOUBLE PRECISION */
/*          The scaling factor s used in solving the triangular system. */

/*  CNORM   (input) DOUBLE PRECISION array, dimension (N) */
/*          The 1-norms of the columns of A, not counting the diagonal. */

/*  TSCAL   (input) DOUBLE PRECISION */
/*          The scaling factor used in computing the 1-norms in CNORM. */
/*          CNORM actually contains the column norms of TSCAL*A. */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The computed solution vectors for the system of linear */
/*          equations. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side vectors for the system of linear */
/*          equations. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESID   (output) DOUBLE PRECISION */
/*          The maximum over the number of right hand sides of */
/*          norm(op(A)*x - s*b) / ( norm(op(A)) * norm(x) * EPS ). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick exit if N = 0 */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --cnorm;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;

    /* Function Body */
    if (*n <= 0 || *nrhs <= 0) {
	*resid = 0.;
	return 0;
    }
    eps = dlamch_("Epsilon");
    smlnum = dlamch_("Safe minimum");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Compute the norm of the triangular matrix A using the column */
/*     norms already computed by DLATBS. */

    tnorm = 0.;
    if (lsame_(diag, "N")) {
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		d__2 = tnorm, d__3 = *tscal * (d__1 = ab[*kd + 1 + j * 
			ab_dim1], abs(d__1)) + cnorm[j];
		tnorm = max(d__2,d__3);
/* L10: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		d__2 = tnorm, d__3 = *tscal * (d__1 = ab[j * ab_dim1 + 1], 
			abs(d__1)) + cnorm[j];
		tnorm = max(d__2,d__3);
/* L20: */
	    }
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    d__1 = tnorm, d__2 = *tscal + cnorm[j];
	    tnorm = max(d__1,d__2);
/* L30: */
	}
    }

/*     Compute the maximum over the number of right hand sides of */
/*        norm(op(A)*x - s*b) / ( norm(op(A)) * norm(x) * EPS ). */

    *resid = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	dcopy_(n, &x[j * x_dim1 + 1], &c__1, &work[1], &c__1);
	ix = idamax_(n, &work[1], &c__1);
/* Computing MAX */
	d__2 = 1., d__3 = (d__1 = x[ix + j * x_dim1], abs(d__1));
	xnorm = max(d__2,d__3);
	xscal = 1. / xnorm / (doublereal) (*kd + 1);
	dscal_(n, &xscal, &work[1], &c__1);
	dtbmv_(uplo, trans, diag, n, kd, &ab[ab_offset], ldab, &work[1], &
		c__1);
	d__1 = -(*scale) * xscal;
	daxpy_(n, &d__1, &b[j * b_dim1 + 1], &c__1, &work[1], &c__1);
	ix = idamax_(n, &work[1], &c__1);
	err = *tscal * (d__1 = work[ix], abs(d__1));
	ix = idamax_(n, &x[j * x_dim1 + 1], &c__1);
	xnorm = (d__1 = x[ix + j * x_dim1], abs(d__1));
	if (err * smlnum <= xnorm) {
	    if (xnorm > 0.) {
		err /= xnorm;
	    }
	} else {
	    if (err > 0.) {
		err = 1. / eps;
	    }
	}
	if (err * smlnum <= tnorm) {
	    if (tnorm > 0.) {
		err /= tnorm;
	    }
	} else {
	    if (err > 0.) {
		err = 1. / eps;
	    }
	}
	*resid = max(*resid,err);
/* L40: */
    }

    return 0;

/*     End of DTBT03 */

} /* dtbt03_ */
