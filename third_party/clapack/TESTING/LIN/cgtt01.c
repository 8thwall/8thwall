#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int cgtt01_(integer *n, complex *dl, complex *d__, complex *
	du, complex *dlf, complex *df, complex *duf, complex *du2, integer *
	ipiv, complex *work, integer *ldwork, real *rwork, real *resid)
{
    /* System generated locals */
    integer work_dim1, work_offset, i__1, i__2, i__3, i__4;
    complex q__1;

    /* Local variables */
    integer i__, j;
    complex li;
    integer ip;
    real eps, anorm;
    integer lastj;
    extern /* Subroutine */ int cswap_(integer *, complex *, integer *, 
	    complex *, integer *), caxpy_(integer *, complex *, complex *, 
	    integer *, complex *, integer *);
    extern doublereal slamch_(char *), clangt_(char *, integer *, 
	    complex *, complex *, complex *), clanhs_(char *, integer 
	    *, complex *, integer *, real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGTT01 reconstructs a tridiagonal matrix A from its LU factorization */
/*  and computes the residual */
/*     norm(L*U - A) / ( norm(A) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGTER */
/*          The order of the matrix A.  N >= 0. */

/*  DL      (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of A. */

/*  D       (input) COMPLEX array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of A. */

/*  DLF     (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) multipliers that define the matrix L from the */
/*          LU factorization of A. */

/*  DF      (input) COMPLEX array, dimension (N) */
/*          The n diagonal elements of the upper triangular matrix U from */
/*          the LU factorization of A. */

/*  DUF     (input) COMPLEX array, dimension (N-1) */
/*          The (n-1) elements of the first super-diagonal of U. */

/*  DU2     (input) COMPLEX array, dimension (N-2) */
/*          The (n-2) elements of the second super-diagonal of U. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= n, row i of the matrix was */
/*          interchanged with row IPIV(i).  IPIV(i) will always be either */
/*          i or i+1; IPIV(i) = i indicates a row interchange was not */
/*          required. */

/*  WORK    (workspace) COMPLEX array, dimension (LDWORK,N) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK.  LDWORK >= max(1,N). */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESID   (output) REAL */
/*          The scaled residual:  norm(L*U - A) / (norm(A) * EPS) */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    --dl;
    --d__;
    --du;
    --dlf;
    --df;
    --duf;
    --du2;
    --ipiv;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1;
    work -= work_offset;
    --rwork;

    /* Function Body */
    if (*n <= 0) {
	*resid = 0.f;
	return 0;
    }

    eps = slamch_("Epsilon");

/*     Copy the matrix U to WORK. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * work_dim1;
	    work[i__3].r = 0.f, work[i__3].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (i__ == 1) {
	    i__2 = i__ + i__ * work_dim1;
	    i__3 = i__;
	    work[i__2].r = df[i__3].r, work[i__2].i = df[i__3].i;
	    if (*n >= 2) {
		i__2 = i__ + (i__ + 1) * work_dim1;
		i__3 = i__;
		work[i__2].r = duf[i__3].r, work[i__2].i = duf[i__3].i;
	    }
	    if (*n >= 3) {
		i__2 = i__ + (i__ + 2) * work_dim1;
		i__3 = i__;
		work[i__2].r = du2[i__3].r, work[i__2].i = du2[i__3].i;
	    }
	} else if (i__ == *n) {
	    i__2 = i__ + i__ * work_dim1;
	    i__3 = i__;
	    work[i__2].r = df[i__3].r, work[i__2].i = df[i__3].i;
	} else {
	    i__2 = i__ + i__ * work_dim1;
	    i__3 = i__;
	    work[i__2].r = df[i__3].r, work[i__2].i = df[i__3].i;
	    i__2 = i__ + (i__ + 1) * work_dim1;
	    i__3 = i__;
	    work[i__2].r = duf[i__3].r, work[i__2].i = duf[i__3].i;
	    if (i__ < *n - 1) {
		i__2 = i__ + (i__ + 2) * work_dim1;
		i__3 = i__;
		work[i__2].r = du2[i__3].r, work[i__2].i = du2[i__3].i;
	    }
	}
/* L30: */
    }

/*     Multiply on the left by L. */

    lastj = *n;
    for (i__ = *n - 1; i__ >= 1; --i__) {
	i__1 = i__;
	li.r = dlf[i__1].r, li.i = dlf[i__1].i;
	i__1 = lastj - i__ + 1;
	caxpy_(&i__1, &li, &work[i__ + i__ * work_dim1], ldwork, &work[i__ + 
		1 + i__ * work_dim1], ldwork);
	ip = ipiv[i__];
	if (ip == i__) {
/* Computing MIN */
	    i__1 = i__ + 2;
	    lastj = min(i__1,*n);
	} else {
	    i__1 = lastj - i__ + 1;
	    cswap_(&i__1, &work[i__ + i__ * work_dim1], ldwork, &work[i__ + 1 
		    + i__ * work_dim1], ldwork);
	}
/* L40: */
    }

/*     Subtract the matrix A. */

    i__1 = work_dim1 + 1;
    i__2 = work_dim1 + 1;
    q__1.r = work[i__2].r - d__[1].r, q__1.i = work[i__2].i - d__[1].i;
    work[i__1].r = q__1.r, work[i__1].i = q__1.i;
    if (*n > 1) {
	i__1 = (work_dim1 << 1) + 1;
	i__2 = (work_dim1 << 1) + 1;
	q__1.r = work[i__2].r - du[1].r, q__1.i = work[i__2].i - du[1].i;
	work[i__1].r = q__1.r, work[i__1].i = q__1.i;
	i__1 = *n + (*n - 1) * work_dim1;
	i__2 = *n + (*n - 1) * work_dim1;
	i__3 = *n - 1;
	q__1.r = work[i__2].r - dl[i__3].r, q__1.i = work[i__2].i - dl[i__3]
		.i;
	work[i__1].r = q__1.r, work[i__1].i = q__1.i;
	i__1 = *n + *n * work_dim1;
	i__2 = *n + *n * work_dim1;
	i__3 = *n;
	q__1.r = work[i__2].r - d__[i__3].r, q__1.i = work[i__2].i - d__[i__3]
		.i;
	work[i__1].r = q__1.r, work[i__1].i = q__1.i;
	i__1 = *n - 1;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    i__2 = i__ + (i__ - 1) * work_dim1;
	    i__3 = i__ + (i__ - 1) * work_dim1;
	    i__4 = i__ - 1;
	    q__1.r = work[i__3].r - dl[i__4].r, q__1.i = work[i__3].i - dl[
		    i__4].i;
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
	    i__2 = i__ + i__ * work_dim1;
	    i__3 = i__ + i__ * work_dim1;
	    i__4 = i__;
	    q__1.r = work[i__3].r - d__[i__4].r, q__1.i = work[i__3].i - d__[
		    i__4].i;
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
	    i__2 = i__ + (i__ + 1) * work_dim1;
	    i__3 = i__ + (i__ + 1) * work_dim1;
	    i__4 = i__;
	    q__1.r = work[i__3].r - du[i__4].r, q__1.i = work[i__3].i - du[
		    i__4].i;
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L50: */
	}
    }

/*     Compute the 1-norm of the tridiagonal matrix A. */

    anorm = clangt_("1", n, &dl[1], &d__[1], &du[1]);

/*     Compute the 1-norm of WORK, which is only guaranteed to be */
/*     upper Hessenberg. */

    *resid = clanhs_("1", n, &work[work_offset], ldwork, &rwork[1])
	    ;

/*     Compute norm(L*U - A) / (norm(A) * EPS) */

    if (anorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	*resid = *resid / anorm / eps;
    }

    return 0;

/*     End of CGTT01 */

} /* cgtt01_ */
