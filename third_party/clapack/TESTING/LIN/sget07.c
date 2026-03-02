#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int sget07_(char *trans, integer *n, integer *nrhs, real *a, 
	integer *lda, real *b, integer *ldb, real *x, integer *ldx, real *
	xact, integer *ldxact, real *ferr, real *berr, real *reslts)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, xact_dim1, 
	    xact_offset, i__1, i__2, i__3;
    real r__1, r__2, r__3;

    /* Local variables */
    integer i__, j, k;
    real eps, tmp, diff, axbi;
    integer imax;
    real unfl, ovfl;
    extern logical lsame_(char *, char *);
    real xnorm;
    extern doublereal slamch_(char *);
    real errbnd;
    extern integer isamax_(integer *, real *, integer *);
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

/*  SGET07 tests the error bounds from iterative refinement for the */
/*  computed solution to a system of equations op(A)*X = B, where A is a */
/*  general n by n matrix and op(A) = A or A**T, depending on TRANS. */

/*  RESLTS(1) = test of the error bound */
/*            = norm(X - XACT) / ( norm(X) * FERR ) */

/*  A large value is returned if this ratio is not less than one. */

/*  RESLTS(2) = residual from the iterative refinement routine */
/*            = the maximum of BERR / ( (n+1)*EPS + (*) ), where */
/*              (*) = (n+1)*UNFL / (min_i (abs(op(A))*abs(X) +abs(b))_i ) */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations. */
/*          = 'N':  A * X = B     (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices X and XACT.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of the matrices X and XACT.  NRHS >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The original n by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input) REAL array, dimension (LDB,NRHS) */
/*          The right hand side vectors for the system of linear */
/*          equations. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input) REAL array, dimension (LDX,NRHS) */
/*          The computed solution vectors.  Each vector is stored as a */
/*          column of the matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  XACT    (input) REAL array, dimension (LDX,NRHS) */
/*          The exact solution vectors.  Each vector is stored as a */
/*          column of the matrix XACT. */

/*  LDXACT  (input) INTEGER */
/*          The leading dimension of the array XACT.  LDXACT >= max(1,N). */

/*  FERR    (input) REAL array, dimension (NRHS) */
/*          The estimated forward error bounds for each solution vector */
/*          X.  If XTRUE is the true solution, FERR bounds the magnitude */
/*          of the largest entry in (X - XTRUE) divided by the magnitude */
/*          of the largest entry in X. */

/*  BERR    (input) REAL array, dimension (NRHS) */
/*          The componentwise relative backward error of each solution */
/*          vector (i.e., the smallest relative change in any entry of A */
/*          or B that makes X an exact solution). */

/*  RESLTS  (output) REAL array, dimension (2) */
/*          The maximum over the NRHS solution vectors of the ratios: */
/*          RESLTS(1) = norm(X - XACT) / ( norm(X) * FERR ) */
/*          RESLTS(2) = BERR / ( (n+1)*EPS + (*) ) */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
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
	reslts[1] = 0.f;
	reslts[2] = 0.f;
	return 0;
    }

    eps = slamch_("Epsilon");
    unfl = slamch_("Safe minimum");
    ovfl = 1.f / unfl;
    notran = lsame_(trans, "N");

/*     Test 1:  Compute the maximum of */
/*        norm(X - XACT) / ( norm(X) * FERR ) */
/*     over all the vectors X and XACT using the infinity-norm. */

    errbnd = 0.f;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	imax = isamax_(n, &x[j * x_dim1 + 1], &c__1);
/* Computing MAX */
	r__2 = (r__1 = x[imax + j * x_dim1], dabs(r__1));
	xnorm = dmax(r__2,unfl);
	diff = 0.f;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    r__2 = diff, r__3 = (r__1 = x[i__ + j * x_dim1] - xact[i__ + j * 
		    xact_dim1], dabs(r__1));
	    diff = dmax(r__2,r__3);
/* L10: */
	}

	if (xnorm > 1.f) {
	    goto L20;
	} else if (diff <= ovfl * xnorm) {
	    goto L20;
	} else {
	    errbnd = 1.f / eps;
	    goto L30;
	}

L20:
	if (diff / xnorm <= ferr[j]) {
/* Computing MAX */
	    r__1 = errbnd, r__2 = diff / xnorm / ferr[j];
	    errbnd = dmax(r__1,r__2);
	} else {
	    errbnd = 1.f / eps;
	}
L30:
	;
    }
    reslts[1] = errbnd;

/*     Test 2:  Compute the maximum of BERR / ( (n+1)*EPS + (*) ), where */
/*     (*) = (n+1)*UNFL / (min_i (abs(op(A))*abs(X) +abs(b))_i ) */

    i__1 = *nrhs;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    tmp = (r__1 = b[i__ + k * b_dim1], dabs(r__1));
	    if (notran) {
		i__3 = *n;
		for (j = 1; j <= i__3; ++j) {
		    tmp += (r__1 = a[i__ + j * a_dim1], dabs(r__1)) * (r__2 = 
			    x[j + k * x_dim1], dabs(r__2));
/* L40: */
		}
	    } else {
		i__3 = *n;
		for (j = 1; j <= i__3; ++j) {
		    tmp += (r__1 = a[j + i__ * a_dim1], dabs(r__1)) * (r__2 = 
			    x[j + k * x_dim1], dabs(r__2));
/* L50: */
		}
	    }
	    if (i__ == 1) {
		axbi = tmp;
	    } else {
		axbi = dmin(axbi,tmp);
	    }
/* L60: */
	}
/* Computing MAX */
	r__1 = axbi, r__2 = (*n + 1) * unfl;
	tmp = berr[k] / ((*n + 1) * eps + (*n + 1) * unfl / dmax(r__1,r__2));
	if (k == 1) {
	    reslts[2] = tmp;
	} else {
	    reslts[2] = dmax(reslts[2],tmp);
	}
/* L70: */
    }

    return 0;

/*     End of SGET07 */

} /* sget07_ */
