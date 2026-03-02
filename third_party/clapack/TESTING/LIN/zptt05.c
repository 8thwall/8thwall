#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int zptt05_(integer *n, integer *nrhs, doublereal *d__, 
	doublecomplex *e, doublecomplex *b, integer *ldb, doublecomplex *x, 
	integer *ldx, doublecomplex *xact, integer *ldxact, doublereal *ferr, 
	doublereal *berr, doublereal *reslts)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, xact_dim1, xact_offset, i__1, 
	    i__2, i__3, i__4, i__5, i__6, i__7, i__8, i__9;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, d__10, 
	    d__11, d__12;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    double d_imag(doublecomplex *);

    /* Local variables */
    integer i__, j, k, nz;
    doublereal eps, tmp, diff, axbi;
    integer imax;
    doublereal unfl, ovfl, xnorm;
    extern doublereal dlamch_(char *);
    doublereal errbnd;
    extern integer izamax_(integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZPTT05 tests the error bounds from iterative refinement for the */
/*  computed solution to a system of equations A*X = B, where A is a */
/*  Hermitian tridiagonal matrix of order n. */

/*  RESLTS(1) = test of the error bound */
/*            = norm(X - XACT) / ( norm(X) * FERR ) */

/*  A large value is returned if this ratio is not less than one. */

/*  RESLTS(2) = residual from the iterative refinement routine */
/*            = the maximum of BERR / ( NZ*EPS + (*) ), where */
/*              (*) = NZ*UNFL / (min_i (abs(A)*abs(X) +abs(b))_i ) */
/*              and NZ = max. number of nonzeros in any row of A, plus 1 */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices X, B, and XACT, and the */
/*          order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of the matrices X, B, and XACT. */
/*          NRHS >= 0. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix A. */

/*  E       (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the tridiagonal matrix A. */

/*  B       (input) COMPLEX*16 array, dimension (LDB,NRHS) */
/*          The right hand side vectors for the system of linear */
/*          equations. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          The computed solution vectors.  Each vector is stored as a */
/*          column of the matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  XACT    (input) COMPLEX*16 array, dimension (LDX,NRHS) */
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
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick exit if N = 0 or NRHS = 0. */

    /* Parameter adjustments */
    --d__;
    --e;
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
    nz = 4;

/*     Test 1:  Compute the maximum of */
/*        norm(X - XACT) / ( norm(X) * FERR ) */
/*     over all the vectors X and XACT using the infinity-norm. */

    errbnd = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	imax = izamax_(n, &x[j * x_dim1 + 1], &c__1);
/* Computing MAX */
	i__2 = imax + j * x_dim1;
	d__3 = (d__1 = x[i__2].r, abs(d__1)) + (d__2 = d_imag(&x[imax + j * 
		x_dim1]), abs(d__2));
	xnorm = max(d__3,unfl);
	diff = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * x_dim1;
	    i__4 = i__ + j * xact_dim1;
	    z__2.r = x[i__3].r - xact[i__4].r, z__2.i = x[i__3].i - xact[i__4]
		    .i;
	    z__1.r = z__2.r, z__1.i = z__2.i;
/* Computing MAX */
	    d__3 = diff, d__4 = (d__1 = z__1.r, abs(d__1)) + (d__2 = d_imag(&
		    z__1), abs(d__2));
	    diff = max(d__3,d__4);
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

    i__1 = *nrhs;
    for (k = 1; k <= i__1; ++k) {
	if (*n == 1) {
	    i__2 = k * x_dim1 + 1;
	    z__2.r = d__[1] * x[i__2].r, z__2.i = d__[1] * x[i__2].i;
	    z__1.r = z__2.r, z__1.i = z__2.i;
	    i__3 = k * b_dim1 + 1;
	    axbi = (d__1 = b[i__3].r, abs(d__1)) + (d__2 = d_imag(&b[k * 
		    b_dim1 + 1]), abs(d__2)) + ((d__3 = z__1.r, abs(d__3)) + (
		    d__4 = d_imag(&z__1), abs(d__4)));
	} else {
	    i__2 = k * x_dim1 + 1;
	    z__2.r = d__[1] * x[i__2].r, z__2.i = d__[1] * x[i__2].i;
	    z__1.r = z__2.r, z__1.i = z__2.i;
	    i__3 = k * b_dim1 + 1;
	    i__4 = k * x_dim1 + 2;
	    axbi = (d__1 = b[i__3].r, abs(d__1)) + (d__2 = d_imag(&b[k * 
		    b_dim1 + 1]), abs(d__2)) + ((d__3 = z__1.r, abs(d__3)) + (
		    d__4 = d_imag(&z__1), abs(d__4))) + ((d__5 = e[1].r, abs(
		    d__5)) + (d__6 = d_imag(&e[1]), abs(d__6))) * ((d__7 = x[
		    i__4].r, abs(d__7)) + (d__8 = d_imag(&x[k * x_dim1 + 2]), 
		    abs(d__8)));
	    i__2 = *n - 1;
	    for (i__ = 2; i__ <= i__2; ++i__) {
		i__3 = i__;
		i__4 = i__ + k * x_dim1;
		z__2.r = d__[i__3] * x[i__4].r, z__2.i = d__[i__3] * x[i__4]
			.i;
		z__1.r = z__2.r, z__1.i = z__2.i;
		i__5 = i__ + k * b_dim1;
		i__6 = i__ - 1;
		i__7 = i__ - 1 + k * x_dim1;
		i__8 = i__;
		i__9 = i__ + 1 + k * x_dim1;
		tmp = (d__1 = b[i__5].r, abs(d__1)) + (d__2 = d_imag(&b[i__ + 
			k * b_dim1]), abs(d__2)) + ((d__3 = e[i__6].r, abs(
			d__3)) + (d__4 = d_imag(&e[i__ - 1]), abs(d__4))) * ((
			d__5 = x[i__7].r, abs(d__5)) + (d__6 = d_imag(&x[i__ 
			- 1 + k * x_dim1]), abs(d__6))) + ((d__7 = z__1.r, 
			abs(d__7)) + (d__8 = d_imag(&z__1), abs(d__8))) + ((
			d__9 = e[i__8].r, abs(d__9)) + (d__10 = d_imag(&e[i__]
			), abs(d__10))) * ((d__11 = x[i__9].r, abs(d__11)) + (
			d__12 = d_imag(&x[i__ + 1 + k * x_dim1]), abs(d__12)))
			;
		axbi = min(axbi,tmp);
/* L40: */
	    }
	    i__2 = *n;
	    i__3 = *n + k * x_dim1;
	    z__2.r = d__[i__2] * x[i__3].r, z__2.i = d__[i__2] * x[i__3].i;
	    z__1.r = z__2.r, z__1.i = z__2.i;
	    i__4 = *n + k * b_dim1;
	    i__5 = *n - 1;
	    i__6 = *n - 1 + k * x_dim1;
	    tmp = (d__1 = b[i__4].r, abs(d__1)) + (d__2 = d_imag(&b[*n + k * 
		    b_dim1]), abs(d__2)) + ((d__3 = e[i__5].r, abs(d__3)) + (
		    d__4 = d_imag(&e[*n - 1]), abs(d__4))) * ((d__5 = x[i__6]
		    .r, abs(d__5)) + (d__6 = d_imag(&x[*n - 1 + k * x_dim1]), 
		    abs(d__6))) + ((d__7 = z__1.r, abs(d__7)) + (d__8 = 
		    d_imag(&z__1), abs(d__8)));
	    axbi = min(axbi,tmp);
	}
/* Computing MAX */
	d__1 = axbi, d__2 = nz * unfl;
	tmp = berr[k] / (nz * eps + nz * unfl / max(d__1,d__2));
	if (k == 1) {
	    reslts[2] = tmp;
	} else {
	    reslts[2] = max(reslts[2],tmp);
	}
/* L50: */
    }

    return 0;

/*     End of ZPTT05 */

} /* zptt05_ */
