#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int zgtt05_(char *trans, integer *n, integer *nrhs, 
	doublecomplex *dl, doublecomplex *d__, doublecomplex *du, 
	doublecomplex *b, integer *ldb, doublecomplex *x, integer *ldx, 
	doublecomplex *xact, integer *ldxact, doublereal *ferr, doublereal *
	berr, doublereal *reslts)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, xact_dim1, xact_offset, i__1, 
	    i__2, i__3, i__4, i__5, i__6, i__7, i__8, i__9;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, d__10, 
	    d__11, d__12, d__13, d__14;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    double d_imag(doublecomplex *);

    /* Local variables */
    integer i__, j, k, nz;
    doublereal eps, tmp, diff, axbi;
    integer imax;
    doublereal unfl, ovfl;
    extern logical lsame_(char *, char *);
    doublereal xnorm;
    extern doublereal dlamch_(char *);
    doublereal errbnd;
    extern integer izamax_(integer *, doublecomplex *, integer *);
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

/*  ZGTT05 tests the error bounds from iterative refinement for the */
/*  computed solution to a system of equations A*X = B, where A is a */
/*  general tridiagonal matrix of order n and op(A) = A or A**T, */
/*  depending on TRANS. */

/*  RESLTS(1) = test of the error bound */
/*            = norm(X - XACT) / ( norm(X) * FERR ) */

/*  A large value is returned if this ratio is not less than one. */

/*  RESLTS(2) = residual from the iterative refinement routine */
/*            = the maximum of BERR / ( NZ*EPS + (*) ), where */
/*              (*) = NZ*UNFL / (min_i (abs(op(A))*abs(X) +abs(b))_i ) */
/*              and NZ = max. number of nonzeros in any row of A, plus 1 */

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

/*  DL      (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of A. */

/*  D       (input) COMPLEX*16 array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of A. */

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
    --dl;
    --d__;
    --du;
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
    notran = lsame_(trans, "N");
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
/*     (*) = NZ*UNFL / (min_i (abs(op(A))*abs(X) +abs(b))_i ) */

    i__1 = *nrhs;
    for (k = 1; k <= i__1; ++k) {
	if (notran) {
	    if (*n == 1) {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		axbi = (d__1 = b[i__2].r, abs(d__1)) + (d__2 = d_imag(&b[k * 
			b_dim1 + 1]), abs(d__2)) + ((d__3 = d__[1].r, abs(
			d__3)) + (d__4 = d_imag(&d__[1]), abs(d__4))) * ((
			d__5 = x[i__3].r, abs(d__5)) + (d__6 = d_imag(&x[k * 
			x_dim1 + 1]), abs(d__6)));
	    } else {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		i__4 = k * x_dim1 + 2;
		axbi = (d__1 = b[i__2].r, abs(d__1)) + (d__2 = d_imag(&b[k * 
			b_dim1 + 1]), abs(d__2)) + ((d__3 = d__[1].r, abs(
			d__3)) + (d__4 = d_imag(&d__[1]), abs(d__4))) * ((
			d__5 = x[i__3].r, abs(d__5)) + (d__6 = d_imag(&x[k * 
			x_dim1 + 1]), abs(d__6))) + ((d__7 = du[1].r, abs(
			d__7)) + (d__8 = d_imag(&du[1]), abs(d__8))) * ((d__9 
			= x[i__4].r, abs(d__9)) + (d__10 = d_imag(&x[k * 
			x_dim1 + 2]), abs(d__10)));
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    i__3 = i__ + k * b_dim1;
		    i__4 = i__ - 1;
		    i__5 = i__ - 1 + k * x_dim1;
		    i__6 = i__;
		    i__7 = i__ + k * x_dim1;
		    i__8 = i__;
		    i__9 = i__ + 1 + k * x_dim1;
		    tmp = (d__1 = b[i__3].r, abs(d__1)) + (d__2 = d_imag(&b[
			    i__ + k * b_dim1]), abs(d__2)) + ((d__3 = dl[i__4]
			    .r, abs(d__3)) + (d__4 = d_imag(&dl[i__ - 1]), 
			    abs(d__4))) * ((d__5 = x[i__5].r, abs(d__5)) + (
			    d__6 = d_imag(&x[i__ - 1 + k * x_dim1]), abs(d__6)
			    )) + ((d__7 = d__[i__6].r, abs(d__7)) + (d__8 = 
			    d_imag(&d__[i__]), abs(d__8))) * ((d__9 = x[i__7]
			    .r, abs(d__9)) + (d__10 = d_imag(&x[i__ + k * 
			    x_dim1]), abs(d__10))) + ((d__11 = du[i__8].r, 
			    abs(d__11)) + (d__12 = d_imag(&du[i__]), abs(
			    d__12))) * ((d__13 = x[i__9].r, abs(d__13)) + (
			    d__14 = d_imag(&x[i__ + 1 + k * x_dim1]), abs(
			    d__14)));
		    axbi = min(axbi,tmp);
/* L40: */
		}
		i__2 = *n + k * b_dim1;
		i__3 = *n - 1;
		i__4 = *n - 1 + k * x_dim1;
		i__5 = *n;
		i__6 = *n + k * x_dim1;
		tmp = (d__1 = b[i__2].r, abs(d__1)) + (d__2 = d_imag(&b[*n + 
			k * b_dim1]), abs(d__2)) + ((d__3 = dl[i__3].r, abs(
			d__3)) + (d__4 = d_imag(&dl[*n - 1]), abs(d__4))) * ((
			d__5 = x[i__4].r, abs(d__5)) + (d__6 = d_imag(&x[*n - 
			1 + k * x_dim1]), abs(d__6))) + ((d__7 = d__[i__5].r, 
			abs(d__7)) + (d__8 = d_imag(&d__[*n]), abs(d__8))) * (
			(d__9 = x[i__6].r, abs(d__9)) + (d__10 = d_imag(&x[*n 
			+ k * x_dim1]), abs(d__10)));
		axbi = min(axbi,tmp);
	    }
	} else {
	    if (*n == 1) {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		axbi = (d__1 = b[i__2].r, abs(d__1)) + (d__2 = d_imag(&b[k * 
			b_dim1 + 1]), abs(d__2)) + ((d__3 = d__[1].r, abs(
			d__3)) + (d__4 = d_imag(&d__[1]), abs(d__4))) * ((
			d__5 = x[i__3].r, abs(d__5)) + (d__6 = d_imag(&x[k * 
			x_dim1 + 1]), abs(d__6)));
	    } else {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		i__4 = k * x_dim1 + 2;
		axbi = (d__1 = b[i__2].r, abs(d__1)) + (d__2 = d_imag(&b[k * 
			b_dim1 + 1]), abs(d__2)) + ((d__3 = d__[1].r, abs(
			d__3)) + (d__4 = d_imag(&d__[1]), abs(d__4))) * ((
			d__5 = x[i__3].r, abs(d__5)) + (d__6 = d_imag(&x[k * 
			x_dim1 + 1]), abs(d__6))) + ((d__7 = dl[1].r, abs(
			d__7)) + (d__8 = d_imag(&dl[1]), abs(d__8))) * ((d__9 
			= x[i__4].r, abs(d__9)) + (d__10 = d_imag(&x[k * 
			x_dim1 + 2]), abs(d__10)));
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    i__3 = i__ + k * b_dim1;
		    i__4 = i__ - 1;
		    i__5 = i__ - 1 + k * x_dim1;
		    i__6 = i__;
		    i__7 = i__ + k * x_dim1;
		    i__8 = i__;
		    i__9 = i__ + 1 + k * x_dim1;
		    tmp = (d__1 = b[i__3].r, abs(d__1)) + (d__2 = d_imag(&b[
			    i__ + k * b_dim1]), abs(d__2)) + ((d__3 = du[i__4]
			    .r, abs(d__3)) + (d__4 = d_imag(&du[i__ - 1]), 
			    abs(d__4))) * ((d__5 = x[i__5].r, abs(d__5)) + (
			    d__6 = d_imag(&x[i__ - 1 + k * x_dim1]), abs(d__6)
			    )) + ((d__7 = d__[i__6].r, abs(d__7)) + (d__8 = 
			    d_imag(&d__[i__]), abs(d__8))) * ((d__9 = x[i__7]
			    .r, abs(d__9)) + (d__10 = d_imag(&x[i__ + k * 
			    x_dim1]), abs(d__10))) + ((d__11 = dl[i__8].r, 
			    abs(d__11)) + (d__12 = d_imag(&dl[i__]), abs(
			    d__12))) * ((d__13 = x[i__9].r, abs(d__13)) + (
			    d__14 = d_imag(&x[i__ + 1 + k * x_dim1]), abs(
			    d__14)));
		    axbi = min(axbi,tmp);
/* L50: */
		}
		i__2 = *n + k * b_dim1;
		i__3 = *n - 1;
		i__4 = *n - 1 + k * x_dim1;
		i__5 = *n;
		i__6 = *n + k * x_dim1;
		tmp = (d__1 = b[i__2].r, abs(d__1)) + (d__2 = d_imag(&b[*n + 
			k * b_dim1]), abs(d__2)) + ((d__3 = du[i__3].r, abs(
			d__3)) + (d__4 = d_imag(&du[*n - 1]), abs(d__4))) * ((
			d__5 = x[i__4].r, abs(d__5)) + (d__6 = d_imag(&x[*n - 
			1 + k * x_dim1]), abs(d__6))) + ((d__7 = d__[i__5].r, 
			abs(d__7)) + (d__8 = d_imag(&d__[*n]), abs(d__8))) * (
			(d__9 = x[i__6].r, abs(d__9)) + (d__10 = d_imag(&x[*n 
			+ k * x_dim1]), abs(d__10)));
		axbi = min(axbi,tmp);
	    }
	}
/* Computing MAX */
	d__1 = axbi, d__2 = nz * unfl;
	tmp = berr[k] / (nz * eps + nz * unfl / max(d__1,d__2));
	if (k == 1) {
	    reslts[2] = tmp;
	} else {
	    reslts[2] = max(reslts[2],tmp);
	}
/* L60: */
    }

    return 0;

/*     End of ZGTT05 */

} /* zgtt05_ */
