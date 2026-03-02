#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int zptt01_(integer *n, doublereal *d__, doublecomplex *e, 
	doublereal *df, doublecomplex *ef, doublecomplex *work, doublereal *
	resid)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublereal d__1, d__2;
    doublecomplex z__1, z__2, z__3, z__4;

    /* Builtin functions */
    void d_cnjg(doublecomplex *, doublecomplex *);
    double z_abs(doublecomplex *);

    /* Local variables */
    integer i__;
    doublecomplex de;
    doublereal eps, anorm;
    extern doublereal dlamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZPTT01 reconstructs a tridiagonal matrix A from its L*D*L' */
/*  factorization and computes the residual */
/*     norm(L*D*L' - A) / ( n * norm(A) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGTER */
/*          The order of the matrix A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix A. */

/*  E       (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the tridiagonal matrix A. */

/*  DF      (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the factor L from the L*D*L' */
/*          factorization of A. */

/*  EF      (input) COMPLEX*16 array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the factor L from the */
/*          L*D*L' factorization of A. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (2*N) */

/*  RESID   (output) DOUBLE PRECISION */
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
	*resid = 0.;
	return 0;
    }

    eps = dlamch_("Epsilon");

/*     Construct the difference L*D*L' - A. */

    d__1 = df[1] - d__[1];
    work[1].r = d__1, work[1].i = 0.;
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = i__;
	z__1.r = df[i__2] * ef[i__3].r, z__1.i = df[i__2] * ef[i__3].i;
	de.r = z__1.r, de.i = z__1.i;
	i__2 = *n + i__;
	i__3 = i__;
	z__1.r = de.r - e[i__3].r, z__1.i = de.i - e[i__3].i;
	work[i__2].r = z__1.r, work[i__2].i = z__1.i;
	i__2 = i__ + 1;
	d_cnjg(&z__4, &ef[i__]);
	z__3.r = de.r * z__4.r - de.i * z__4.i, z__3.i = de.r * z__4.i + de.i 
		* z__4.r;
	i__3 = i__ + 1;
	z__2.r = z__3.r + df[i__3], z__2.i = z__3.i;
	i__4 = i__ + 1;
	z__1.r = z__2.r - d__[i__4], z__1.i = z__2.i;
	work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L10: */
    }

/*     Compute the 1-norms of the tridiagonal matrices A and WORK. */

    if (*n == 1) {
	anorm = d__[1];
	*resid = z_abs(&work[1]);
    } else {
/* Computing MAX */
	d__1 = d__[1] + z_abs(&e[1]), d__2 = d__[*n] + z_abs(&e[*n - 1]);
	anorm = max(d__1,d__2);
/* Computing MAX */
	d__1 = z_abs(&work[1]) + z_abs(&work[*n + 1]), d__2 = z_abs(&work[*n])
		 + z_abs(&work[(*n << 1) - 1]);
	*resid = max(d__1,d__2);
	i__1 = *n - 1;
	for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = anorm, d__2 = d__[i__] + z_abs(&e[i__]) + z_abs(&e[i__ - 1]
		    );
	    anorm = max(d__1,d__2);
/* Computing MAX */
	    d__1 = *resid, d__2 = z_abs(&work[i__]) + z_abs(&work[*n + i__ - 
		    1]) + z_abs(&work[*n + i__]);
	    *resid = max(d__1,d__2);
/* L20: */
	}
    }

/*     Compute norm(L*D*L' - A) / (n * norm(A) * EPS) */

    if (anorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	*resid = *resid / (doublereal) (*n) / anorm / eps;
    }

    return 0;

/*     End of ZPTT01 */

} /* zptt01_ */
