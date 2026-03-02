#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static complex c_b7 = {-1.f,-0.f};
static complex c_b10 = {1.f,0.f};

/* Subroutine */ int cbdt02_(integer *m, integer *n, complex *b, integer *ldb, 
	 complex *c__, integer *ldc, complex *u, integer *ldu, complex *work, 
	real *rwork, real *resid)
{
    /* System generated locals */
    integer b_dim1, b_offset, c_dim1, c_offset, u_dim1, u_offset, i__1;
    real r__1, r__2;

    /* Local variables */
    integer j;
    real eps;
    extern /* Subroutine */ int cgemv_(char *, integer *, integer *, complex *
, complex *, integer *, complex *, integer *, complex *, complex *
, integer *);
    real bnorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    real realmn;
    extern doublereal scasum_(integer *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CBDT02 tests the change of basis C = U' * B by computing the residual */

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

/*  B       (input) COMPLEX array, dimension (LDB,N) */
/*          The m by n matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,M). */

/*  C       (input) COMPLEX array, dimension (LDC,N) */
/*          The m by n matrix C, assumed to contain U' * B. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C.  LDC >= max(1,M). */

/*  U       (input) COMPLEX array, dimension (LDU,M) */
/*          The m by m orthogonal matrix U. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= max(1,M). */

/*  WORK    (workspace) COMPLEX array, dimension (M) */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESID   (output) REAL */
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
    --rwork;

    /* Function Body */
    *resid = 0.f;
    if (*m <= 0 || *n <= 0) {
	return 0;
    }
    realmn = (real) max(*m,*n);
    eps = slamch_("Precision");

/*     Compute norm( B - U * C ) */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	ccopy_(m, &b[j * b_dim1 + 1], &c__1, &work[1], &c__1);
	cgemv_("No transpose", m, m, &c_b7, &u[u_offset], ldu, &c__[j * 
		c_dim1 + 1], &c__1, &c_b10, &work[1], &c__1);
/* Computing MAX */
	r__1 = *resid, r__2 = scasum_(m, &work[1], &c__1);
	*resid = dmax(r__1,r__2);
/* L10: */
    }

/*     Compute norm of B. */

    bnorm = clange_("1", m, n, &b[b_offset], ldb, &rwork[1]);

    if (bnorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	if (bnorm >= *resid) {
	    *resid = *resid / bnorm / (realmn * eps);
	} else {
	    if (bnorm < 1.f) {
/* Computing MIN */
		r__1 = *resid, r__2 = realmn * bnorm;
		*resid = dmin(r__1,r__2) / bnorm / (realmn * eps);
	    } else {
/* Computing MIN */
		r__1 = *resid / bnorm;
		*resid = dmin(r__1,realmn) / (realmn * eps);
	    }
	}
    }
    return 0;

/*     End of CBDT02 */

} /* cbdt02_ */
