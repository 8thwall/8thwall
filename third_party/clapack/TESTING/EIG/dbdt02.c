#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b7 = -1.;
static doublereal c_b9 = 1.;

/* Subroutine */ int dbdt02_(integer *m, integer *n, doublereal *b, integer *
	ldb, doublereal *c__, integer *ldc, doublereal *u, integer *ldu, 
	doublereal *work, doublereal *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, c_dim1, c_offset, u_dim1, u_offset, i__1;
    doublereal d__1, d__2;

    /* Local variables */
    integer j;
    doublereal eps;
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal bnorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    doublereal realmn;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DBDT02 tests the change of basis C = U' * B by computing the residual */

/*     RESID = norm( B - U * C ) / ( max(m,n) * norm(B) * EPS ), */

/*  where B and C are M by N matrices, U is an M by M orthogonal matrix, */
/*  and EPS is the machine precision. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrices B and C and the order of */
/*          the matrix Q. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices B and C. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,N) */
/*          The m by n matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,M). */

/*  C       (input) DOUBLE PRECISION array, dimension (LDC,N) */
/*          The m by n matrix C, assumed to contain U' * B. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C.  LDC >= max(1,M). */

/*  U       (input) DOUBLE PRECISION array, dimension (LDU,M) */
/*          The m by m orthogonal matrix U. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= max(1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESID   (output) DOUBLE PRECISION */
/*          RESID = norm( B - U * C ) / ( max(m,n) * norm(B) * EPS ), */

/* ====================================================================== */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --work;

    /* Function Body */
    *resid = 0.;
    if (*m <= 0 || *n <= 0) {
	return 0;
    }
    realmn = (doublereal) max(*m,*n);
    eps = dlamch_("Precision");

/*     Compute norm( B - U * C ) */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	dcopy_(m, &b[j * b_dim1 + 1], &c__1, &work[1], &c__1);
	dgemv_("No transpose", m, m, &c_b7, &u[u_offset], ldu, &c__[j * 
		c_dim1 + 1], &c__1, &c_b9, &work[1], &c__1);
/* Computing MAX */
	d__1 = *resid, d__2 = dasum_(m, &work[1], &c__1);
	*resid = max(d__1,d__2);
/* L10: */
    }

/*     Compute norm of B. */

    bnorm = dlange_("1", m, n, &b[b_offset], ldb, &work[1]);

    if (bnorm <= 0.) {
	if (*resid != 0.) {
	    *resid = 1. / eps;
	}
    } else {
	if (bnorm >= *resid) {
	    *resid = *resid / bnorm / (realmn * eps);
	} else {
	    if (bnorm < 1.) {
/* Computing MIN */
		d__1 = *resid, d__2 = realmn * bnorm;
		*resid = min(d__1,d__2) / bnorm / (realmn * eps);
	    } else {
/* Computing MIN */
		d__1 = *resid / bnorm;
		*resid = min(d__1,realmn) / (realmn * eps);
	    }
	}
    }
    return 0;

/*     End of DBDT02 */

} /* dbdt02_ */
