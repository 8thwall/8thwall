#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int cpot05_(char *uplo, integer *n, integer *nrhs, complex *
	a, integer *lda, complex *b, integer *ldb, complex *x, integer *ldx, 
	complex *xact, integer *ldxact, real *ferr, real *berr, real *reslts)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, xact_dim1, 
	    xact_offset, i__1, i__2, i__3, i__4, i__5;
    real r__1, r__2, r__3, r__4;
    complex q__1, q__2;

    /* Builtin functions */
    double r_imag(complex *);

    /* Local variables */
    integer i__, j, k;
    real eps, tmp, diff, axbi;
    integer imax;
    real unfl, ovfl;
    extern logical lsame_(char *, char *);
    logical upper;
    real xnorm;
    extern integer icamax_(integer *, complex *, integer *);
    extern doublereal slamch_(char *);
    real errbnd;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CPOT05 tests the error bounds from iterative refinement for the */
/*  computed solution to a system of equations A*X = B, where A is a */
/*  Hermitian n by n matrix. */

/*  RESLTS(1) = test of the error bound */
/*            = norm(X - XACT) / ( norm(X) * FERR ) */

/*  A large value is returned if this ratio is not less than one. */

/*  RESLTS(2) = residual from the iterative refinement routine */
/*            = the maximum of BERR / ( (n+1)*EPS + (*) ), where */
/*              (*) = (n+1)*UNFL / (min_i (abs(A)*abs(X) +abs(b))_i ) */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          Hermitian matrix A is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices X, B, and XACT, and the */
/*          order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of the matrices X, B, and XACT. */
/*          NRHS >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The Hermitian matrix A.  If UPLO = 'U', the leading n by n */
/*          upper triangular part of A contains the upper triangular part */
/*          of the matrix A, and the strictly lower triangular part of A */
/*          is not referenced.  If UPLO = 'L', the leading n by n lower */
/*          triangular part of A contains the lower triangular part of */
/*          the matrix A, and the strictly upper triangular part of A is */
/*          not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input) COMPLEX array, dimension (LDB,NRHS) */
/*          The right hand side vectors for the system of linear */
/*          equations. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input) COMPLEX array, dimension (LDX,NRHS) */
/*          The computed solution vectors.  Each vector is stored as a */
/*          column of the matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  XACT    (input) COMPLEX array, dimension (LDX,NRHS) */
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
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
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
    upper = lsame_(uplo, "U");

/*     Test 1:  Compute the maximum of */
/*        norm(X - XACT) / ( norm(X) * FERR ) */
/*     over all the vectors X and XACT using the infinity-norm. */

    errbnd = 0.f;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	imax = icamax_(n, &x[j * x_dim1 + 1], &c__1);
/* Computing MAX */
	i__2 = imax + j * x_dim1;
	r__3 = (r__1 = x[i__2].r, dabs(r__1)) + (r__2 = r_imag(&x[imax + j * 
		x_dim1]), dabs(r__2));
	xnorm = dmax(r__3,unfl);
	diff = 0.f;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * x_dim1;
	    i__4 = i__ + j * xact_dim1;
	    q__2.r = x[i__3].r - xact[i__4].r, q__2.i = x[i__3].i - xact[i__4]
		    .i;
	    q__1.r = q__2.r, q__1.i = q__2.i;
/* Computing MAX */
	    r__3 = diff, r__4 = (r__1 = q__1.r, dabs(r__1)) + (r__2 = r_imag(&
		    q__1), dabs(r__2));
	    diff = dmax(r__3,r__4);
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
/*     (*) = (n+1)*UNFL / (min_i (abs(A)*abs(X) +abs(b))_i ) */

    i__1 = *nrhs;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + k * b_dim1;
	    tmp = (r__1 = b[i__3].r, dabs(r__1)) + (r__2 = r_imag(&b[i__ + k *
		     b_dim1]), dabs(r__2));
	    if (upper) {
		i__3 = i__ - 1;
		for (j = 1; j <= i__3; ++j) {
		    i__4 = j + i__ * a_dim1;
		    i__5 = j + k * x_dim1;
		    tmp += ((r__1 = a[i__4].r, dabs(r__1)) + (r__2 = r_imag(&
			    a[j + i__ * a_dim1]), dabs(r__2))) * ((r__3 = x[
			    i__5].r, dabs(r__3)) + (r__4 = r_imag(&x[j + k * 
			    x_dim1]), dabs(r__4)));
/* L40: */
		}
		i__3 = i__ + i__ * a_dim1;
		i__4 = i__ + k * x_dim1;
		tmp += (r__1 = a[i__3].r, dabs(r__1)) * ((r__2 = x[i__4].r, 
			dabs(r__2)) + (r__3 = r_imag(&x[i__ + k * x_dim1]), 
			dabs(r__3)));
		i__3 = *n;
		for (j = i__ + 1; j <= i__3; ++j) {
		    i__4 = i__ + j * a_dim1;
		    i__5 = j + k * x_dim1;
		    tmp += ((r__1 = a[i__4].r, dabs(r__1)) + (r__2 = r_imag(&
			    a[i__ + j * a_dim1]), dabs(r__2))) * ((r__3 = x[
			    i__5].r, dabs(r__3)) + (r__4 = r_imag(&x[j + k * 
			    x_dim1]), dabs(r__4)));
/* L50: */
		}
	    } else {
		i__3 = i__ - 1;
		for (j = 1; j <= i__3; ++j) {
		    i__4 = i__ + j * a_dim1;
		    i__5 = j + k * x_dim1;
		    tmp += ((r__1 = a[i__4].r, dabs(r__1)) + (r__2 = r_imag(&
			    a[i__ + j * a_dim1]), dabs(r__2))) * ((r__3 = x[
			    i__5].r, dabs(r__3)) + (r__4 = r_imag(&x[j + k * 
			    x_dim1]), dabs(r__4)));
/* L60: */
		}
		i__3 = i__ + i__ * a_dim1;
		i__4 = i__ + k * x_dim1;
		tmp += (r__1 = a[i__3].r, dabs(r__1)) * ((r__2 = x[i__4].r, 
			dabs(r__2)) + (r__3 = r_imag(&x[i__ + k * x_dim1]), 
			dabs(r__3)));
		i__3 = *n;
		for (j = i__ + 1; j <= i__3; ++j) {
		    i__4 = j + i__ * a_dim1;
		    i__5 = j + k * x_dim1;
		    tmp += ((r__1 = a[i__4].r, dabs(r__1)) + (r__2 = r_imag(&
			    a[j + i__ * a_dim1]), dabs(r__2))) * ((r__3 = x[
			    i__5].r, dabs(r__3)) + (r__4 = r_imag(&x[j + k * 
			    x_dim1]), dabs(r__4)));
/* L70: */
		}
	    }
	    if (i__ == 1) {
		axbi = tmp;
	    } else {
		axbi = dmin(axbi,tmp);
	    }
/* L80: */
	}
/* Computing MAX */
	r__1 = axbi, r__2 = (*n + 1) * unfl;
	tmp = berr[k] / ((*n + 1) * eps + (*n + 1) * unfl / dmax(r__1,r__2));
	if (k == 1) {
	    reslts[2] = tmp;
	} else {
	    reslts[2] = dmax(reslts[2],tmp);
	}
/* L90: */
    }

    return 0;

/*     End of CPOT05 */

} /* cpot05_ */
