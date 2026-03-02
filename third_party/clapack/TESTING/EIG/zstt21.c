#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__1 = 1;

/* Subroutine */ int zstt21_(integer *n, integer *kband, doublereal *ad, 
	doublereal *ae, doublereal *sd, doublereal *se, doublecomplex *u, 
	integer *ldu, doublecomplex *work, doublereal *rwork, doublereal *
	result)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1, i__2, i__3;
    doublereal d__1, d__2, d__3;
    doublecomplex z__1, z__2;

    /* Local variables */
    integer j;
    doublereal ulp, unfl;
    extern /* Subroutine */ int zher_(char *, integer *, doublereal *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    doublereal temp1, temp2;
    extern /* Subroutine */ int zher2_(char *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    doublereal anorm;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);
    doublereal wnorm;
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *), 
	    zlanhe_(char *, char *, integer *, doublecomplex *, integer *, 
	    doublereal *);
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZSTT21  checks a decomposition of the form */

/*     A = U S U* */

/*  where * means conjugate transpose, A is real symmetric tridiagonal, */
/*  U is unitary, and S is real and diagonal (if KBAND=0) or symmetric */
/*  tridiagonal (if KBAND=1).  Two tests are performed: */

/*     RESULT(1) = | A - U S U* | / ( |A| n ulp ) */

/*     RESULT(2) = | I - UU* | / ( n ulp ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, ZSTT21 does nothing. */
/*          It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix S.  It may only be zero or one. */
/*          If zero, then S is diagonal, and SE is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AD      (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal of the original (unfactored) matrix A.  A is */
/*          assumed to be real symmetric tridiagonal. */

/*  AE      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal of the original (unfactored) matrix A.  A */
/*          is assumed to be symmetric tridiagonal.  AE(1) is the (1,2) */
/*          and (2,1) element, AE(2) is the (2,3) and (3,2) element, etc. */

/*  SD      (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal of the real (symmetric tri-) diagonal matrix S. */

/*  SE      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix S. */
/*          Not referenced if KBSND=0.  If KBAND=1, then AE(1) is the */
/*          (1,2) and (2,1) element, SE(2) is the (2,3) and (3,2) */
/*          element, etc. */

/*  U       (input) COMPLEX*16 array, dimension (LDU, N) */
/*          The unitary matrix in the decomposition. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (N**2) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
/*          The values computed by the two tests described above.  The */
/*          values are currently limited to 1/ulp, to avoid overflow. */
/*          RESULT(1) is always modified. */

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

/*     1)      Constants */

    /* Parameter adjustments */
    --ad;
    --ae;
    --sd;
    --se;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --work;
    --rwork;
    --result;

    /* Function Body */
    result[1] = 0.;
    result[2] = 0.;
    if (*n <= 0) {
	return 0;
    }

    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Precision");

/*     Do Test 1 */

/*     Copy A & Compute its 1-Norm: */

    zlaset_("Full", n, n, &c_b1, &c_b1, &work[1], n);

    anorm = 0.;
    temp1 = 0.;

    i__1 = *n - 1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = (*n + 1) * (j - 1) + 1;
	i__3 = j;
	work[i__2].r = ad[i__3], work[i__2].i = 0.;
	i__2 = (*n + 1) * (j - 1) + 2;
	i__3 = j;
	work[i__2].r = ae[i__3], work[i__2].i = 0.;
	temp2 = (d__1 = ae[j], abs(d__1));
/* Computing MAX */
	d__2 = anorm, d__3 = (d__1 = ad[j], abs(d__1)) + temp1 + temp2;
	anorm = max(d__2,d__3);
	temp1 = temp2;
/* L10: */
    }

/* Computing 2nd power */
    i__2 = *n;
    i__1 = i__2 * i__2;
    i__3 = *n;
    work[i__1].r = ad[i__3], work[i__1].i = 0.;
/* Computing MAX */
    d__2 = anorm, d__3 = (d__1 = ad[*n], abs(d__1)) + temp1, d__2 = max(d__2,
	    d__3);
    anorm = max(d__2,unfl);

/*     Norm of A - USU* */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	d__1 = -sd[j];
	zher_("L", n, &d__1, &u[j * u_dim1 + 1], &c__1, &work[1], n);
/* L20: */
    }

    if (*n > 1 && *kband == 1) {
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    z__2.r = se[i__2], z__2.i = 0.;
	    z__1.r = -z__2.r, z__1.i = -z__2.i;
	    zher2_("L", n, &z__1, &u[j * u_dim1 + 1], &c__1, &u[(j + 1) * 
		    u_dim1 + 1], &c__1, &work[1], n);
/* L30: */
	}
    }

    wnorm = zlanhe_("1", "L", n, &work[1], n, &rwork[1])
	    ;

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*n * ulp);
    } else {
	if (anorm < 1.) {
/* Computing MIN */
	    d__1 = wnorm, d__2 = *n * anorm;
	    result[1] = min(d__1,d__2) / anorm / (*n * ulp);
	} else {
/* Computing MIN */
	    d__1 = wnorm / anorm, d__2 = (doublereal) (*n);
	    result[1] = min(d__1,d__2) / (*n * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  UU* - I */

    zgemm_("N", "C", n, n, n, &c_b2, &u[u_offset], ldu, &u[u_offset], ldu, &
	    c_b1, &work[1], n);

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = (*n + 1) * (j - 1) + 1;
	i__3 = (*n + 1) * (j - 1) + 1;
	z__1.r = work[i__3].r - 1., z__1.i = work[i__3].i - 0.;
	work[i__2].r = z__1.r, work[i__2].i = z__1.i;
/* L40: */
    }

/* Computing MIN */
    d__1 = (doublereal) (*n), d__2 = zlange_("1", n, n, &work[1], n, &rwork[1]
);
    result[2] = min(d__1,d__2) / (*n * ulp);

    return 0;

/*     End of ZSTT21 */

} /* zstt21_ */
