#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int dget53_(doublereal *a, integer *lda, doublereal *b, 
	integer *ldb, doublereal *scale, doublereal *wr, doublereal *wi, 
	doublereal *result, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    doublereal s1, ci11, ci12, ci22, cr11, cr12, cr21, cr22, ulp, wis, wrs, 
	    deti, absw, detr, temp, anorm, bnorm, cnorm;
    extern doublereal dlamch_(char *);
    doublereal cscale, scales, safmin, sigmin;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET53  checks the generalized eigenvalues computed by DLAG2. */

/*  The basic test for an eigenvalue is: */

/*                               | det( s A - w B ) | */
/*      RESULT =  --------------------------------------------------- */
/*                ulp max( s norm(A), |w| norm(B) )*norm( s A - w B ) */

/*  Two "safety checks" are performed: */

/*  (1)  ulp*max( s*norm(A), |w|*norm(B) )  must be at least */
/*       safe_minimum.  This insures that the test performed is */
/*       not essentially  det(0*A + 0*B)=0. */

/*  (2)  s*norm(A) + |w|*norm(B) must be less than 1/safe_minimum. */
/*       This insures that  s*A - w*B  will not overflow. */

/*  If these tests are not passed, then  s  and  w  are scaled and */
/*  tested anyway, if this is possible. */

/*  Arguments */
/*  ========= */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, 2) */
/*          The 2x2 matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 2. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB, N) */
/*          The 2x2 upper-triangular matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 2. */

/*  SCALE   (input) DOUBLE PRECISION */
/*          The "scale factor" s in the formula  s A - w B .  It is */
/*          assumed to be non-negative. */

/*  WR      (input) DOUBLE PRECISION */
/*          The real part of the eigenvalue  w  in the formula */
/*          s A - w B . */

/*  WI      (input) DOUBLE PRECISION */
/*          The imaginary part of the eigenvalue  w  in the formula */
/*          s A - w B . */

/*  RESULT  (output) DOUBLE PRECISION */
/*          If INFO is 2 or less, the value computed by the test */
/*             described above. */
/*          If INFO=3, this will just be 1/ulp. */

/*  INFO    (output) INTEGER */
/*          =0:  The input data pass the "safety checks". */
/*          =1:  s*norm(A) + |w|*norm(B) > 1/safe_minimum. */
/*          =2:  ulp*max( s*norm(A), |w|*norm(B) ) < safe_minimum */
/*          =3:  same as INFO=2, but  s  and  w  could not be scaled so */
/*               as to compute the test. */

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

/*     Initialize */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    *result = 0.;
    scales = *scale;
    wrs = *wr;
    wis = *wi;

/*     Machine constants and norms */

    safmin = dlamch_("Safe minimum");
    ulp = dlamch_("Epsilon") * dlamch_("Base");
    absw = abs(wrs) + abs(wis);
/* Computing MAX */
    d__5 = (d__1 = a[a_dim1 + 1], abs(d__1)) + (d__2 = a[a_dim1 + 2], abs(
	    d__2)), d__6 = (d__3 = a[(a_dim1 << 1) + 1], abs(d__3)) + (d__4 = 
	    a[(a_dim1 << 1) + 2], abs(d__4)), d__5 = max(d__5,d__6);
    anorm = max(d__5,safmin);
/* Computing MAX */
    d__4 = (d__3 = b[b_dim1 + 1], abs(d__3)), d__5 = (d__1 = b[(b_dim1 << 1) 
	    + 1], abs(d__1)) + (d__2 = b[(b_dim1 << 1) + 2], abs(d__2)), d__4 
	    = max(d__4,d__5);
    bnorm = max(d__4,safmin);

/*     Check for possible overflow. */

    temp = safmin * bnorm * absw + safmin * anorm * scales;
    if (temp >= 1.) {

/*        Scale down to avoid overflow */

	*info = 1;
	temp = 1. / temp;
	scales *= temp;
	wrs *= temp;
	wis *= temp;
	absw = abs(wrs) + abs(wis);
    }
/* Computing MAX */
/* Computing MAX */
    d__3 = scales * anorm, d__4 = absw * bnorm;
    d__1 = ulp * max(d__3,d__4), d__2 = safmin * max(scales,absw);
    s1 = max(d__1,d__2);

/*     Check for W and SCALE essentially zero. */

    if (s1 < safmin) {
	*info = 2;
	if (scales < safmin && absw < safmin) {
	    *info = 3;
	    *result = 1. / ulp;
	    return 0;
	}

/*        Scale up to avoid underflow */

/* Computing MAX */
	d__1 = scales * anorm + absw * bnorm;
	temp = 1. / max(d__1,safmin);
	scales *= temp;
	wrs *= temp;
	wis *= temp;
	absw = abs(wrs) + abs(wis);
/* Computing MAX */
/* Computing MAX */
	d__3 = scales * anorm, d__4 = absw * bnorm;
	d__1 = ulp * max(d__3,d__4), d__2 = safmin * max(scales,absw);
	s1 = max(d__1,d__2);
	if (s1 < safmin) {
	    *info = 3;
	    *result = 1. / ulp;
	    return 0;
	}
    }

/*     Compute C = s A - w B */

    cr11 = scales * a[a_dim1 + 1] - wrs * b[b_dim1 + 1];
    ci11 = -wis * b[b_dim1 + 1];
    cr21 = scales * a[a_dim1 + 2];
    cr12 = scales * a[(a_dim1 << 1) + 1] - wrs * b[(b_dim1 << 1) + 1];
    ci12 = -wis * b[(b_dim1 << 1) + 1];
    cr22 = scales * a[(a_dim1 << 1) + 2] - wrs * b[(b_dim1 << 1) + 2];
    ci22 = -wis * b[(b_dim1 << 1) + 2];

/*     Compute the smallest singular value of s A - w B: */

/*                 |det( s A - w B )| */
/*     sigma_min = ------------------ */
/*                 norm( s A - w B ) */

/* Computing MAX */
    d__1 = abs(cr11) + abs(ci11) + abs(cr21), d__2 = abs(cr12) + abs(ci12) + 
	    abs(cr22) + abs(ci22), d__1 = max(d__1,d__2);
    cnorm = max(d__1,safmin);
    cscale = 1. / sqrt(cnorm);
    detr = cscale * cr11 * (cscale * cr22) - cscale * ci11 * (cscale * ci22) 
	    - cscale * cr12 * (cscale * cr21);
    deti = cscale * cr11 * (cscale * ci22) + cscale * ci11 * (cscale * cr22) 
	    - cscale * ci12 * (cscale * cr21);
    sigmin = abs(detr) + abs(deti);
    *result = sigmin / s1;
    return 0;

/*     End of DGET53 */

} /* dget53_ */
