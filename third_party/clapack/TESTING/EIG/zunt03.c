#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int zunt03_(char *rc, integer *mu, integer *mv, integer *n, 
	integer *k, doublecomplex *u, integer *ldu, doublecomplex *v, integer 
	*ldv, doublecomplex *work, integer *lwork, doublereal *rwork, 
	doublereal *result, integer *info)
{
    /* System generated locals */
    integer u_dim1, u_offset, v_dim1, v_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    double z_abs(doublecomplex *);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__, j;
    doublecomplex s, su, sv;
    integer irc, lmx;
    doublereal ulp, res1, res2;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int zunt01_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern integer izamax_(integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZUNT03 compares two unitary matrices U and V to see if their */
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
/*          columns if RC = 'C'.  If MU = 0 ZUNT03 does nothing. */
/*          MU must be at least zero. */

/*  MV      (input) INTEGER */
/*          The number of rows of V if RC = 'R', and the number of */
/*          columns if RC = 'C'.  If MV = 0 ZUNT03 does nothing. */
/*          MV must be at least zero. */

/*  N       (input) INTEGER */
/*          If RC = 'R', the number of columns in the matrices U and V, */
/*          and if RC = 'C', the number of rows in U and V.  If N = 0 */
/*          ZUNT03 does nothing.  N must be at least zero. */

/*  K       (input) INTEGER */
/*          The number of rows or columns of U and V to compare. */
/*          0 <= K <= max(MU,MV). */

/*  U       (input) COMPLEX*16 array, dimension (LDU,N) */
/*          The first matrix to compare.  If RC = 'R', U is MU by N, and */
/*          if RC = 'C', U is N by MU. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  If RC = 'R', LDU >= max(1,MU), */
/*          and if RC = 'C', LDU >= max(1,N). */

/*  V       (input) COMPLEX*16 array, dimension (LDV,N) */
/*          The second matrix to compare.  If RC = 'R', V is MV by N, and */
/*          if RC = 'C', V is N by MV. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  If RC = 'R', LDV >= max(1,MV), */
/*          and if RC = 'C', LDV >= max(1,N). */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  For best performance, LWORK */
/*          should be at least N*N if RC = 'C' or M*M if RC = 'R', but */
/*          the tests will be done even if LWORK is 0. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (max(MV,N)) */

/*  RESULT  (output) DOUBLE PRECISION */
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
	xerbla_("ZUNT03", &i__1);
	return 0;
    }

/*     Initialize result */

    *result = 0.;
    if (*mu == 0 || *mv == 0 || *n == 0) {
	return 0;
    }

/*     Machine constants */

    ulp = dlamch_("Precision");

    if (irc == 0) {

/*        Compare rows */

	res1 = 0.;
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lmx = izamax_(n, &u[i__ + u_dim1], ldu);
	    i__2 = i__ + lmx * v_dim1;
	    if (v[i__2].r == 0. && v[i__2].i == 0.) {
		sv.r = 1., sv.i = 0.;
	    } else {
		d__1 = z_abs(&v[i__ + lmx * v_dim1]);
		z__2.r = d__1, z__2.i = 0.;
		z_div(&z__1, &z__2, &v[i__ + lmx * v_dim1]);
		sv.r = z__1.r, sv.i = z__1.i;
	    }
	    i__2 = i__ + lmx * u_dim1;
	    if (u[i__2].r == 0. && u[i__2].i == 0.) {
		su.r = 1., su.i = 0.;
	    } else {
		d__1 = z_abs(&u[i__ + lmx * u_dim1]);
		z__2.r = d__1, z__2.i = 0.;
		z_div(&z__1, &z__2, &u[i__ + lmx * u_dim1]);
		su.r = z__1.r, su.i = z__1.i;
	    }
	    z_div(&z__1, &sv, &su);
	    s.r = z__1.r, s.i = z__1.i;
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		i__3 = i__ + j * u_dim1;
		i__4 = i__ + j * v_dim1;
		z__2.r = s.r * v[i__4].r - s.i * v[i__4].i, z__2.i = s.r * v[
			i__4].i + s.i * v[i__4].r;
		z__1.r = u[i__3].r - z__2.r, z__1.i = u[i__3].i - z__2.i;
		d__1 = res1, d__2 = z_abs(&z__1);
		res1 = max(d__1,d__2);
/* L10: */
	    }
/* L20: */
	}
	res1 /= (doublereal) (*n) * ulp;

/*        Compute orthogonality of rows of V. */

	zunt01_("Rows", mv, n, &v[v_offset], ldv, &work[1], lwork, &rwork[1], 
		&res2);

    } else {

/*        Compare columns */

	res1 = 0.;
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lmx = izamax_(n, &u[i__ * u_dim1 + 1], &c__1);
	    i__2 = lmx + i__ * v_dim1;
	    if (v[i__2].r == 0. && v[i__2].i == 0.) {
		sv.r = 1., sv.i = 0.;
	    } else {
		d__1 = z_abs(&v[lmx + i__ * v_dim1]);
		z__2.r = d__1, z__2.i = 0.;
		z_div(&z__1, &z__2, &v[lmx + i__ * v_dim1]);
		sv.r = z__1.r, sv.i = z__1.i;
	    }
	    i__2 = lmx + i__ * u_dim1;
	    if (u[i__2].r == 0. && u[i__2].i == 0.) {
		su.r = 1., su.i = 0.;
	    } else {
		d__1 = z_abs(&u[lmx + i__ * u_dim1]);
		z__2.r = d__1, z__2.i = 0.;
		z_div(&z__1, &z__2, &u[lmx + i__ * u_dim1]);
		su.r = z__1.r, su.i = z__1.i;
	    }
	    z_div(&z__1, &sv, &su);
	    s.r = z__1.r, s.i = z__1.i;
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		i__3 = j + i__ * u_dim1;
		i__4 = j + i__ * v_dim1;
		z__2.r = s.r * v[i__4].r - s.i * v[i__4].i, z__2.i = s.r * v[
			i__4].i + s.i * v[i__4].r;
		z__1.r = u[i__3].r - z__2.r, z__1.i = u[i__3].i - z__2.i;
		d__1 = res1, d__2 = z_abs(&z__1);
		res1 = max(d__1,d__2);
/* L30: */
	    }
/* L40: */
	}
	res1 /= (doublereal) (*n) * ulp;

/*        Compute orthogonality of columns of V. */

	zunt01_("Columns", n, mv, &v[v_offset], ldv, &work[1], lwork, &rwork[
		1], &res2);
    }

/* Computing MIN */
    d__1 = max(res1,res2), d__2 = 1. / ulp;
    *result = min(d__1,d__2);
    return 0;

/*     End of ZUNT03 */

} /* zunt03_ */
