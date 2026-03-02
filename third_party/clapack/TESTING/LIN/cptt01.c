#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int cptt01_(integer *n, real *d__, complex *e, real *df, 
	complex *ef, complex *work, real *resid)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2;
    complex q__1, q__2, q__3, q__4;

    /* Builtin functions */
    void r_cnjg(complex *, complex *);
    double c_abs(complex *);

    /* Local variables */
    integer i__;
    complex de;
    real eps, anorm;
    extern doublereal slamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CPTT01 reconstructs a tridiagonal matrix A from its L*D*L' */
/*  factorization and computes the residual */
/*     norm(L*D*L' - A) / ( n * norm(A) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGTER */
/*          The order of the matrix A. */

/*  D       (input) REAL array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix A. */

/*  E       (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the tridiagonal matrix A. */

/*  DF      (input) REAL array, dimension (N) */
/*          The n diagonal elements of the factor L from the L*D*L' */
/*          factorization of A. */

/*  EF      (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the factor L from the */
/*          L*D*L' factorization of A. */

/*  WORK    (workspace) COMPLEX array, dimension (2*N) */

/*  RESID   (output) REAL */
/*          norm(L*D*L' - A) / (n * norm(A) * EPS) */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    --work;
    --ef;
    --df;
    --e;
    --d__;

    /* Function Body */
    if (*n <= 0) {
	*resid = 0.f;
	return 0;
    }

    eps = slamch_("Epsilon");

/*     Construct the difference L*D*L' - A. */

    r__1 = df[1] - d__[1];
    work[1].r = r__1, work[1].i = 0.f;
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = i__;
	q__1.r = df[i__2] * ef[i__3].r, q__1.i = df[i__2] * ef[i__3].i;
	de.r = q__1.r, de.i = q__1.i;
	i__2 = *n + i__;
	i__3 = i__;
	q__1.r = de.r - e[i__3].r, q__1.i = de.i - e[i__3].i;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
	i__2 = i__ + 1;
	r_cnjg(&q__4, &ef[i__]);
	q__3.r = de.r * q__4.r - de.i * q__4.i, q__3.i = de.r * q__4.i + de.i 
		* q__4.r;
	i__3 = i__ + 1;
	q__2.r = q__3.r + df[i__3], q__2.i = q__3.i;
	i__4 = i__ + 1;
	q__1.r = q__2.r - d__[i__4], q__1.i = q__2.i;
	work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L10: */
    }

/*     Compute the 1-norms of the tridiagonal matrices A and WORK. */

    if (*n == 1) {
	anorm = d__[1];
	*resid = c_abs(&work[1]);
    } else {
/* Computing MAX */
	r__1 = d__[1] + c_abs(&e[1]), r__2 = d__[*n] + c_abs(&e[*n - 1]);
	anorm = dmax(r__1,r__2);
/* Computing MAX */
	r__1 = c_abs(&work[1]) + c_abs(&work[*n + 1]), r__2 = c_abs(&work[*n])
		 + c_abs(&work[(*n << 1) - 1]);
	*resid = dmax(r__1,r__2);
	i__1 = *n - 1;
	for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
	    r__1 = anorm, r__2 = d__[i__] + c_abs(&e[i__]) + c_abs(&e[i__ - 1]
		    );
	    anorm = dmax(r__1,r__2);
/* Computing MAX */
	    r__1 = *resid, r__2 = c_abs(&work[i__]) + c_abs(&work[*n + i__ - 
		    1]) + c_abs(&work[*n + i__]);
	    *resid = dmax(r__1,r__2);
/* L20: */
	}
    }

/*     Compute norm(L*D*L' - A) / (n * norm(A) * EPS) */

    if (anorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	*resid = *resid / (real) (*n) / anorm / eps;
    }

    return 0;

/*     End of CPTT01 */

} /* cptt01_ */
