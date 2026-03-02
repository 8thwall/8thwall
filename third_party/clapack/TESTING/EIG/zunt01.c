#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b7 = {0.,0.};
static doublecomplex c_b8 = {1.,0.};
static doublereal c_b10 = -1.;
static doublereal c_b11 = 1.;
static integer c__1 = 1;

/* Subroutine */ int zunt01_(char *rowcol, integer *m, integer *n, 
	doublecomplex *u, integer *ldu, doublecomplex *work, integer *lwork, 
	doublereal *rwork, doublereal *resid)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    double d_imag(doublecomplex *);

    /* Local variables */
    integer i__, j, k;
    doublereal eps;
    doublecomplex tmp;
    extern logical lsame_(char *, char *);
    integer mnmin;
    extern /* Double Complex */ VOID zdotc_(doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    extern /* Subroutine */ int zherk_(char *, char *, integer *, integer *, 
	    doublereal *, doublecomplex *, integer *, doublereal *, 
	    doublecomplex *, integer *);
    extern doublereal dlamch_(char *);
    integer ldwork;
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);
    char transu[1];
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZUNT01 checks that the matrix U is unitary by computing the ratio */

/*     RESID = norm( I - U*U' ) / ( n * EPS ), if ROWCOL = 'R', */
/*  or */
/*     RESID = norm( I - U'*U ) / ( m * EPS ), if ROWCOL = 'C'. */

/*  Alternatively, if there isn't sufficient workspace to form */
/*  I - U*U' or I - U'*U, the ratio is computed as */

/*     RESID = abs( I - U*U' ) / ( n * EPS ), if ROWCOL = 'R', */
/*  or */
/*     RESID = abs( I - U'*U ) / ( m * EPS ), if ROWCOL = 'C'. */

/*  where EPS is the machine precision.  ROWCOL is used only if m = n; */
/*  if m > n, ROWCOL is assumed to be 'C', and if m < n, ROWCOL is */
/*  assumed to be 'R'. */

/*  Arguments */
/*  ========= */

/*  ROWCOL  (input) CHARACTER */
/*          Specifies whether the rows or columns of U should be checked */
/*          for orthogonality.  Used only if M = N. */
/*          = 'R':  Check for orthogonal rows of U */
/*          = 'C':  Check for orthogonal columns of U */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix U. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix U. */

/*  U       (input) COMPLEX*16 array, dimension (LDU,N) */
/*          The unitary matrix U.  U is checked for orthogonal columns */
/*          if m > n or if m = n and ROWCOL = 'C'.  U is checked for */
/*          orthogonal rows if m < n or if m = n and ROWCOL = 'R'. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= max(1,M). */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  For best performance, LWORK */
/*          should be at least N*N if ROWCOL = 'C' or M*M if */
/*          ROWCOL = 'R', but the test will be done even if LWORK is 0. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          Used only if LWORK is large enough to use the Level 3 BLAS */
/*          code. */

/*  RESID   (output) DOUBLE PRECISION */
/*          RESID = norm( I - U * U' ) / ( n * EPS ), if ROWCOL = 'R', or */
/*          RESID = norm( I - U' * U ) / ( m * EPS ), if ROWCOL = 'C'. */

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
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --work;
    --rwork;

    /* Function Body */
    *resid = 0.;

/*     Quick return if possible */

    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    eps = dlamch_("Precision");
    if (*m < *n || *m == *n && lsame_(rowcol, "R")) {
	*(unsigned char *)transu = 'N';
	k = *n;
    } else {
	*(unsigned char *)transu = 'C';
	k = *m;
    }
    mnmin = min(*m,*n);

    if ((mnmin + 1) * mnmin <= *lwork) {
	ldwork = mnmin;
    } else {
	ldwork = 0;
    }
    if (ldwork > 0) {

/*        Compute I - U*U' or I - U'*U. */

	zlaset_("Upper", &mnmin, &mnmin, &c_b7, &c_b8, &work[1], &ldwork);
	zherk_("Upper", transu, &mnmin, &k, &c_b10, &u[u_offset], ldu, &c_b11, 
		 &work[1], &ldwork);

/*        Compute norm( I - U*U' ) / ( K * EPS ) . */

	*resid = zlansy_("1", "Upper", &mnmin, &work[1], &ldwork, &rwork[1]);
	*resid = *resid / (doublereal) k / eps;
    } else if (*(unsigned char *)transu == 'C') {

/*        Find the maximum element in abs( I - U'*U ) / ( m * EPS ) */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		if (i__ != j) {
		    tmp.r = 0., tmp.i = 0.;
		} else {
		    tmp.r = 1., tmp.i = 0.;
		}
		zdotc_(&z__2, m, &u[i__ * u_dim1 + 1], &c__1, &u[j * u_dim1 + 
			1], &c__1);
		z__1.r = tmp.r - z__2.r, z__1.i = tmp.i - z__2.i;
		tmp.r = z__1.r, tmp.i = z__1.i;
/* Computing MAX */
		d__3 = *resid, d__4 = (d__1 = tmp.r, abs(d__1)) + (d__2 = 
			d_imag(&tmp), abs(d__2));
		*resid = max(d__3,d__4);
/* L10: */
	    }
/* L20: */
	}
	*resid = *resid / (doublereal) (*m) / eps;
    } else {

/*        Find the maximum element in abs( I - U*U' ) / ( n * EPS ) */

	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		if (i__ != j) {
		    tmp.r = 0., tmp.i = 0.;
		} else {
		    tmp.r = 1., tmp.i = 0.;
		}
		zdotc_(&z__2, n, &u[j + u_dim1], ldu, &u[i__ + u_dim1], ldu);
		z__1.r = tmp.r - z__2.r, z__1.i = tmp.i - z__2.i;
		tmp.r = z__1.r, tmp.i = z__1.i;
/* Computing MAX */
		d__3 = *resid, d__4 = (d__1 = tmp.r, abs(d__1)) + (d__2 = 
			d_imag(&tmp), abs(d__2));
		*resid = max(d__3,d__4);
/* L30: */
	    }
/* L40: */
	}
	*resid = *resid / (doublereal) (*n) / eps;
    }
    return 0;

/*     End of ZUNT01 */

} /* zunt01_ */
