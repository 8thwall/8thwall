#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b7 = {0.f,0.f};
static complex c_b8 = {1.f,0.f};
static real c_b10 = -1.f;
static real c_b11 = 1.f;
static integer c__1 = 1;

/* Subroutine */ int cunt01_(char *rowcol, integer *m, integer *n, complex *u, 
	 integer *ldu, complex *work, integer *lwork, real *rwork, real *
	resid)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1, i__2;
    real r__1, r__2, r__3, r__4;
    complex q__1, q__2;

    /* Builtin functions */
    double r_imag(complex *);

    /* Local variables */
    integer i__, j, k;
    real eps;
    complex tmp;
    extern /* Complex */ VOID cdotc_(complex *, integer *, complex *, integer 
	    *, complex *, integer *);
    extern /* Subroutine */ int cherk_(char *, char *, integer *, integer *, 
	    real *, complex *, integer *, real *, complex *, integer *);
    extern logical lsame_(char *, char *);
    integer mnmin;
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);
    extern doublereal clansy_(char *, char *, integer *, complex *, integer *, 
	     real *);
    integer ldwork;
    char transu[1];


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CUNT01 checks that the matrix U is unitary by computing the ratio */

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

/*  U       (input) COMPLEX array, dimension (LDU,N) */
/*          The unitary matrix U.  U is checked for orthogonal columns */
/*          if m > n or if m = n and ROWCOL = 'C'.  U is checked for */
/*          orthogonal rows if m < n or if m = n and ROWCOL = 'R'. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= max(1,M). */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  For best performance, LWORK */
/*          should be at least N*N if ROWCOL = 'C' or M*M if */
/*          ROWCOL = 'R', but the test will be done even if LWORK is 0. */

/*  RWORK   (workspace) REAL array, dimension (min(M,N)) */
/*          Used only if LWORK is large enough to use the Level 3 BLAS */
/*          code. */

/*  RESID   (output) REAL */
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
    *resid = 0.f;

/*     Quick return if possible */

    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    eps = slamch_("Precision");
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

	claset_("Upper", &mnmin, &mnmin, &c_b7, &c_b8, &work[1], &ldwork);
	cherk_("Upper", transu, &mnmin, &k, &c_b10, &u[u_offset], ldu, &c_b11, 
		 &work[1], &ldwork);

/*        Compute norm( I - U*U' ) / ( K * EPS ) . */

	*resid = clansy_("1", "Upper", &mnmin, &work[1], &ldwork, &rwork[1]);
	*resid = *resid / (real) k / eps;
    } else if (*(unsigned char *)transu == 'C') {

/*        Find the maximum element in abs( I - U'*U ) / ( m * EPS ) */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		if (i__ != j) {
		    tmp.r = 0.f, tmp.i = 0.f;
		} else {
		    tmp.r = 1.f, tmp.i = 0.f;
		}
		cdotc_(&q__2, m, &u[i__ * u_dim1 + 1], &c__1, &u[j * u_dim1 + 
			1], &c__1);
		q__1.r = tmp.r - q__2.r, q__1.i = tmp.i - q__2.i;
		tmp.r = q__1.r, tmp.i = q__1.i;
/* Computing MAX */
		r__3 = *resid, r__4 = (r__1 = tmp.r, dabs(r__1)) + (r__2 = 
			r_imag(&tmp), dabs(r__2));
		*resid = dmax(r__3,r__4);
/* L10: */
	    }
/* L20: */
	}
	*resid = *resid / (real) (*m) / eps;
    } else {

/*        Find the maximum element in abs( I - U*U' ) / ( n * EPS ) */

	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		if (i__ != j) {
		    tmp.r = 0.f, tmp.i = 0.f;
		} else {
		    tmp.r = 1.f, tmp.i = 0.f;
		}
		cdotc_(&q__2, n, &u[j + u_dim1], ldu, &u[i__ + u_dim1], ldu);
		q__1.r = tmp.r - q__2.r, q__1.i = tmp.i - q__2.i;
		tmp.r = q__1.r, tmp.i = q__1.i;
/* Computing MAX */
		r__3 = *resid, r__4 = (r__1 = tmp.r, dabs(r__1)) + (r__2 = 
			r_imag(&tmp), dabs(r__2));
		*resid = dmax(r__3,r__4);
/* L30: */
	    }
/* L40: */
	}
	*resid = *resid / (real) (*n) / eps;
    }
    return 0;

/*     End of CUNT01 */

} /* cunt01_ */
