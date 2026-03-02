#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int cunt03_(char *rc, integer *mu, integer *mv, integer *n, 
	integer *k, complex *u, integer *ldu, complex *v, integer *ldv, 
	complex *work, integer *lwork, real *rwork, real *result, integer *
	info)
{
    /* System generated locals */
    integer u_dim1, u_offset, v_dim1, v_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2;
    complex q__1, q__2;

    /* Builtin functions */
    double c_abs(complex *);
    void c_div(complex *, complex *, complex *);

    /* Local variables */
    integer i__, j;
    complex s, su, sv;
    integer irc, lmx;
    real ulp, res1, res2;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int cunt01_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *, real *, real *);
    extern integer icamax_(integer *, complex *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CUNT03 compares two unitary matrices U and V to see if their */
/*  corresponding rows or columns span the same spaces.  The rows are */
/*  checked if RC = 'R', and the columns are checked if RC = 'C'. */

/*  RESULT is the maximum of */

/*     | V*V' - I | / ( MV ulp ), if RC = 'R', or */

/*     | V'*V - I | / ( MV ulp ), if RC = 'C', */

/*  and the maximum over rows (or columns) 1 to K of */

/*     | U(i) - S*V(i) |/ ( N ulp ) */

/*  where abs(S) = 1 (chosen to minimize the expression), U(i) is the */
/*  i-th row (column) of U, and V(i) is the i-th row (column) of V. */

/*  Arguments */
/*  ========== */

/*  RC      (input) CHARACTER*1 */
/*          If RC = 'R' the rows of U and V are to be compared. */
/*          If RC = 'C' the columns of U and V are to be compared. */

/*  MU      (input) INTEGER */
/*          The number of rows of U if RC = 'R', and the number of */
/*          columns if RC = 'C'.  If MU = 0 CUNT03 does nothing. */
/*          MU must be at least zero. */

/*  MV      (input) INTEGER */
/*          The number of rows of V if RC = 'R', and the number of */
/*          columns if RC = 'C'.  If MV = 0 CUNT03 does nothing. */
/*          MV must be at least zero. */

/*  N       (input) INTEGER */
/*          If RC = 'R', the number of columns in the matrices U and V, */
/*          and if RC = 'C', the number of rows in U and V.  If N = 0 */
/*          CUNT03 does nothing.  N must be at least zero. */

/*  K       (input) INTEGER */
/*          The number of rows or columns of U and V to compare. */
/*          0 <= K <= max(MU,MV). */

/*  U       (input) COMPLEX array, dimension (LDU,N) */
/*          The first matrix to compare.  If RC = 'R', U is MU by N, and */
/*          if RC = 'C', U is N by MU. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  If RC = 'R', LDU >= max(1,MU), */
/*          and if RC = 'C', LDU >= max(1,N). */

/*  V       (input) COMPLEX array, dimension (LDV,N) */
/*          The second matrix to compare.  If RC = 'R', V is MV by N, and */
/*          if RC = 'C', V is N by MV. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  If RC = 'R', LDV >= max(1,MV), */
/*          and if RC = 'C', LDV >= max(1,N). */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  For best performance, LWORK */
/*          should be at least N*N if RC = 'C' or M*M if RC = 'R', but */
/*          the tests will be done even if LWORK is 0. */

/*  RWORK   (workspace) REAL array, dimension (max(MV,N)) */

/*  RESULT  (output) REAL */
/*          The value computed by the test described above.  RESULT is */
/*          limited to 1/ulp to avoid overflow. */

/*  INFO    (output) INTEGER */
/*          0  indicates a successful exit */
/*          -k indicates the k-th parameter had an illegal value */

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

/*     Check inputs */

    /* Parameter adjustments */
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --work;
    --rwork;

    /* Function Body */
    *info = 0;
    if (lsame_(rc, "R")) {
	irc = 0;
    } else if (lsame_(rc, "C")) {
	irc = 1;
    } else {
	irc = -1;
    }
    if (irc == -1) {
	*info = -1;
    } else if (*mu < 0) {
	*info = -2;
    } else if (*mv < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > max(*mu,*mv)) {
	*info = -5;
    } else if (irc == 0 && *ldu < max(1,*mu) || irc == 1 && *ldu < max(1,*n)) 
	    {
	*info = -7;
    } else if (irc == 0 && *ldv < max(1,*mv) || irc == 1 && *ldv < max(1,*n)) 
	    {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CUNT03", &i__1);
	return 0;
    }

/*     Initialize result */

    *result = 0.f;
    if (*mu == 0 || *mv == 0 || *n == 0) {
	return 0;
    }

/*     Machine constants */

    ulp = slamch_("Precision");

    if (irc == 0) {

/*        Compare rows */

	res1 = 0.f;
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lmx = icamax_(n, &u[i__ + u_dim1], ldu);
	    i__2 = i__ + lmx * v_dim1;
	    if (v[i__2].r == 0.f && v[i__2].i == 0.f) {
		sv.r = 1.f, sv.i = 0.f;
	    } else {
		r__1 = c_abs(&v[i__ + lmx * v_dim1]);
		q__2.r = r__1, q__2.i = 0.f;
		c_div(&q__1, &q__2, &v[i__ + lmx * v_dim1]);
		sv.r = q__1.r, sv.i = q__1.i;
	    }
	    i__2 = i__ + lmx * u_dim1;
	    if (u[i__2].r == 0.f && u[i__2].i == 0.f) {
		su.r = 1.f, su.i = 0.f;
	    } else {
		r__1 = c_abs(&u[i__ + lmx * u_dim1]);
		q__2.r = r__1, q__2.i = 0.f;
		c_div(&q__1, &q__2, &u[i__ + lmx * u_dim1]);
		su.r = q__1.r, su.i = q__1.i;
	    }
	    c_div(&q__1, &sv, &su);
	    s.r = q__1.r, s.i = q__1.i;
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		i__3 = i__ + j * u_dim1;
		i__4 = i__ + j * v_dim1;
		q__2.r = s.r * v[i__4].r - s.i * v[i__4].i, q__2.i = s.r * v[
			i__4].i + s.i * v[i__4].r;
		q__1.r = u[i__3].r - q__2.r, q__1.i = u[i__3].i - q__2.i;
		r__1 = res1, r__2 = c_abs(&q__1);
		res1 = dmax(r__1,r__2);
/* L10: */
	    }
/* L20: */
	}
	res1 /= (real) (*n) * ulp;

/*        Compute orthogonality of rows of V. */

	cunt01_("Rows", mv, n, &v[v_offset], ldv, &work[1], lwork, &rwork[1], 
		&res2);

    } else {

/*        Compare columns */

	res1 = 0.f;
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lmx = icamax_(n, &u[i__ * u_dim1 + 1], &c__1);
	    i__2 = lmx + i__ * v_dim1;
	    if (v[i__2].r == 0.f && v[i__2].i == 0.f) {
		sv.r = 1.f, sv.i = 0.f;
	    } else {
		r__1 = c_abs(&v[lmx + i__ * v_dim1]);
		q__2.r = r__1, q__2.i = 0.f;
		c_div(&q__1, &q__2, &v[lmx + i__ * v_dim1]);
		sv.r = q__1.r, sv.i = q__1.i;
	    }
	    i__2 = lmx + i__ * u_dim1;
	    if (u[i__2].r == 0.f && u[i__2].i == 0.f) {
		su.r = 1.f, su.i = 0.f;
	    } else {
		r__1 = c_abs(&u[lmx + i__ * u_dim1]);
		q__2.r = r__1, q__2.i = 0.f;
		c_div(&q__1, &q__2, &u[lmx + i__ * u_dim1]);
		su.r = q__1.r, su.i = q__1.i;
	    }
	    c_div(&q__1, &sv, &su);
	    s.r = q__1.r, s.i = q__1.i;
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		i__3 = j + i__ * u_dim1;
		i__4 = j + i__ * v_dim1;
		q__2.r = s.r * v[i__4].r - s.i * v[i__4].i, q__2.i = s.r * v[
			i__4].i + s.i * v[i__4].r;
		q__1.r = u[i__3].r - q__2.r, q__1.i = u[i__3].i - q__2.i;
		r__1 = res1, r__2 = c_abs(&q__1);
		res1 = dmax(r__1,r__2);
/* L30: */
	    }
/* L40: */
	}
	res1 /= (real) (*n) * ulp;

/*        Compute orthogonality of columns of V. */

	cunt01_("Columns", n, mv, &v[v_offset], ldv, &work[1], lwork, &rwork[
		1], &res2);
    }

/* Computing MIN */
    r__1 = dmax(res1,res2), r__2 = 1.f / ulp;
    *result = dmin(r__1,r__2);
    return 0;

/*     End of CUNT03 */

} /* cunt03_ */
