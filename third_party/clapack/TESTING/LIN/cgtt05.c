#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int cgtt05_(char *trans, integer *n, integer *nrhs, complex *
	dl, complex *d__, complex *du, complex *b, integer *ldb, complex *x, 
	integer *ldx, complex *xact, integer *ldxact, real *ferr, real *berr, 
	real *reslts)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, xact_dim1, xact_offset, i__1, 
	    i__2, i__3, i__4, i__5, i__6, i__7, i__8, i__9;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7, r__8, r__9, r__10, r__11, 
	    r__12, r__13, r__14;
    complex q__1, q__2;

    /* Builtin functions */
    double r_imag(complex *);

    /* Local variables */
    integer i__, j, k, nz;
    real eps, tmp, diff, axbi;
    integer imax;
    real unfl, ovfl;
    extern logical lsame_(char *, char *);
    real xnorm;
    extern integer icamax_(integer *, complex *, integer *);
    extern doublereal slamch_(char *);
    real errbnd;
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

/*  CGTT05 tests the error bounds from iterative refinement for the */
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

/*  DL      (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of A. */

/*  D       (input) COMPLEX array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of A. */

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
	reslts[1] = 0.f;
	reslts[2] = 0.f;
	return 0;
    }

    eps = slamch_("Epsilon");
    unfl = slamch_("Safe minimum");
    ovfl = 1.f / unfl;
    notran = lsame_(trans, "N");
    nz = 4;

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

/*     Test 2:  Compute the maximum of BERR / ( NZ*EPS + (*) ), where */
/*     (*) = NZ*UNFL / (min_i (abs(op(A))*abs(X) +abs(b))_i ) */

    i__1 = *nrhs;
    for (k = 1; k <= i__1; ++k) {
	if (notran) {
	    if (*n == 1) {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		axbi = (r__1 = b[i__2].r, dabs(r__1)) + (r__2 = r_imag(&b[k * 
			b_dim1 + 1]), dabs(r__2)) + ((r__3 = d__[1].r, dabs(
			r__3)) + (r__4 = r_imag(&d__[1]), dabs(r__4))) * ((
			r__5 = x[i__3].r, dabs(r__5)) + (r__6 = r_imag(&x[k * 
			x_dim1 + 1]), dabs(r__6)));
	    } else {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		i__4 = k * x_dim1 + 2;
		axbi = (r__1 = b[i__2].r, dabs(r__1)) + (r__2 = r_imag(&b[k * 
			b_dim1 + 1]), dabs(r__2)) + ((r__3 = d__[1].r, dabs(
			r__3)) + (r__4 = r_imag(&d__[1]), dabs(r__4))) * ((
			r__5 = x[i__3].r, dabs(r__5)) + (r__6 = r_imag(&x[k * 
			x_dim1 + 1]), dabs(r__6))) + ((r__7 = du[1].r, dabs(
			r__7)) + (r__8 = r_imag(&du[1]), dabs(r__8))) * ((
			r__9 = x[i__4].r, dabs(r__9)) + (r__10 = r_imag(&x[k *
			 x_dim1 + 2]), dabs(r__10)));
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    i__3 = i__ + k * b_dim1;
		    i__4 = i__ - 1;
		    i__5 = i__ - 1 + k * x_dim1;
		    i__6 = i__;
		    i__7 = i__ + k * x_dim1;
		    i__8 = i__;
		    i__9 = i__ + 1 + k * x_dim1;
		    tmp = (r__1 = b[i__3].r, dabs(r__1)) + (r__2 = r_imag(&b[
			    i__ + k * b_dim1]), dabs(r__2)) + ((r__3 = dl[
			    i__4].r, dabs(r__3)) + (r__4 = r_imag(&dl[i__ - 1]
			    ), dabs(r__4))) * ((r__5 = x[i__5].r, dabs(r__5)) 
			    + (r__6 = r_imag(&x[i__ - 1 + k * x_dim1]), dabs(
			    r__6))) + ((r__7 = d__[i__6].r, dabs(r__7)) + (
			    r__8 = r_imag(&d__[i__]), dabs(r__8))) * ((r__9 = 
			    x[i__7].r, dabs(r__9)) + (r__10 = r_imag(&x[i__ + 
			    k * x_dim1]), dabs(r__10))) + ((r__11 = du[i__8]
			    .r, dabs(r__11)) + (r__12 = r_imag(&du[i__]), 
			    dabs(r__12))) * ((r__13 = x[i__9].r, dabs(r__13)) 
			    + (r__14 = r_imag(&x[i__ + 1 + k * x_dim1]), dabs(
			    r__14)));
		    axbi = dmin(axbi,tmp);
/* L40: */
		}
		i__2 = *n + k * b_dim1;
		i__3 = *n - 1;
		i__4 = *n - 1 + k * x_dim1;
		i__5 = *n;
		i__6 = *n + k * x_dim1;
		tmp = (r__1 = b[i__2].r, dabs(r__1)) + (r__2 = r_imag(&b[*n + 
			k * b_dim1]), dabs(r__2)) + ((r__3 = dl[i__3].r, dabs(
			r__3)) + (r__4 = r_imag(&dl[*n - 1]), dabs(r__4))) * (
			(r__5 = x[i__4].r, dabs(r__5)) + (r__6 = r_imag(&x[*n 
			- 1 + k * x_dim1]), dabs(r__6))) + ((r__7 = d__[i__5]
			.r, dabs(r__7)) + (r__8 = r_imag(&d__[*n]), dabs(r__8)
			)) * ((r__9 = x[i__6].r, dabs(r__9)) + (r__10 = 
			r_imag(&x[*n + k * x_dim1]), dabs(r__10)));
		axbi = dmin(axbi,tmp);
	    }
	} else {
	    if (*n == 1) {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		axbi = (r__1 = b[i__2].r, dabs(r__1)) + (r__2 = r_imag(&b[k * 
			b_dim1 + 1]), dabs(r__2)) + ((r__3 = d__[1].r, dabs(
			r__3)) + (r__4 = r_imag(&d__[1]), dabs(r__4))) * ((
			r__5 = x[i__3].r, dabs(r__5)) + (r__6 = r_imag(&x[k * 
			x_dim1 + 1]), dabs(r__6)));
	    } else {
		i__2 = k * b_dim1 + 1;
		i__3 = k * x_dim1 + 1;
		i__4 = k * x_dim1 + 2;
		axbi = (r__1 = b[i__2].r, dabs(r__1)) + (r__2 = r_imag(&b[k * 
			b_dim1 + 1]), dabs(r__2)) + ((r__3 = d__[1].r, dabs(
			r__3)) + (r__4 = r_imag(&d__[1]), dabs(r__4))) * ((
			r__5 = x[i__3].r, dabs(r__5)) + (r__6 = r_imag(&x[k * 
			x_dim1 + 1]), dabs(r__6))) + ((r__7 = dl[1].r, dabs(
			r__7)) + (r__8 = r_imag(&dl[1]), dabs(r__8))) * ((
			r__9 = x[i__4].r, dabs(r__9)) + (r__10 = r_imag(&x[k *
			 x_dim1 + 2]), dabs(r__10)));
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    i__3 = i__ + k * b_dim1;
		    i__4 = i__ - 1;
		    i__5 = i__ - 1 + k * x_dim1;
		    i__6 = i__;
		    i__7 = i__ + k * x_dim1;
		    i__8 = i__;
		    i__9 = i__ + 1 + k * x_dim1;
		    tmp = (r__1 = b[i__3].r, dabs(r__1)) + (r__2 = r_imag(&b[
			    i__ + k * b_dim1]), dabs(r__2)) + ((r__3 = du[
			    i__4].r, dabs(r__3)) + (r__4 = r_imag(&du[i__ - 1]
			    ), dabs(r__4))) * ((r__5 = x[i__5].r, dabs(r__5)) 
			    + (r__6 = r_imag(&x[i__ - 1 + k * x_dim1]), dabs(
			    r__6))) + ((r__7 = d__[i__6].r, dabs(r__7)) + (
			    r__8 = r_imag(&d__[i__]), dabs(r__8))) * ((r__9 = 
			    x[i__7].r, dabs(r__9)) + (r__10 = r_imag(&x[i__ + 
			    k * x_dim1]), dabs(r__10))) + ((r__11 = dl[i__8]
			    .r, dabs(r__11)) + (r__12 = r_imag(&dl[i__]), 
			    dabs(r__12))) * ((r__13 = x[i__9].r, dabs(r__13)) 
			    + (r__14 = r_imag(&x[i__ + 1 + k * x_dim1]), dabs(
			    r__14)));
		    axbi = dmin(axbi,tmp);
/* L50: */
		}
		i__2 = *n + k * b_dim1;
		i__3 = *n - 1;
		i__4 = *n - 1 + k * x_dim1;
		i__5 = *n;
		i__6 = *n + k * x_dim1;
		tmp = (r__1 = b[i__2].r, dabs(r__1)) + (r__2 = r_imag(&b[*n + 
			k * b_dim1]), dabs(r__2)) + ((r__3 = du[i__3].r, dabs(
			r__3)) + (r__4 = r_imag(&du[*n - 1]), dabs(r__4))) * (
			(r__5 = x[i__4].r, dabs(r__5)) + (r__6 = r_imag(&x[*n 
			- 1 + k * x_dim1]), dabs(r__6))) + ((r__7 = d__[i__5]
			.r, dabs(r__7)) + (r__8 = r_imag(&d__[*n]), dabs(r__8)
			)) * ((r__9 = x[i__6].r, dabs(r__9)) + (r__10 = 
			r_imag(&x[*n + k * x_dim1]), dabs(r__10)));
		axbi = dmin(axbi,tmp);
	    }
	}
/* Computing MAX */
	r__1 = axbi, r__2 = nz * unfl;
	tmp = berr[k] / (nz * eps + nz * unfl / dmax(r__1,r__2));
	if (k == 1) {
	    reslts[2] = tmp;
	} else {
	    reslts[2] = dmax(reslts[2],tmp);
	}
/* L60: */
    }

    return 0;

/*     End of CGTT05 */

} /* cgtt05_ */
