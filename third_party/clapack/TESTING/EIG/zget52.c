#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__1 = 1;

/* Subroutine */ int zget52_(logical *left, integer *n, doublecomplex *a, 
	integer *lda, doublecomplex *b, integer *ldb, doublecomplex *e, 
	integer *lde, doublecomplex *alpha, doublecomplex *beta, 
	doublecomplex *work, doublereal *rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, e_dim1, e_offset, i__1, i__2, 
	    i__3;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6;
    doublecomplex z__1;

    /* Builtin functions */
    double d_imag(doublecomplex *);
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    integer j;
    doublereal ulp;
    integer jvec;
    doublereal temp1;
    doublecomplex betai;
    doublereal scale, abmax, anorm, bnorm, enorm;
    char trans[1];
    extern /* Subroutine */ int zgemv_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *);
    doublecomplex acoeff, bcoeff;
    extern doublereal dlamch_(char *);
    doublecomplex alphai;
    doublereal alfmax, safmin;
    char normab[1];
    doublereal safmax, betmax;
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    doublereal enrmer, errnrm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGET52  does an eigenvector check for the generalized eigenvalue */
/*  problem. */

/*  The basic test for right eigenvectors is: */

/*                            | b(i) A E(i) -  a(i) B E(i) | */
/*          RESULT(1) = max   ------------------------------- */
/*                       i    n ulp max( |b(i) A|, |a(i) B| ) */

/*  using the 1-norm.  Here, a(i)/b(i) = w is the i-th generalized */
/*  eigenvalue of A - w B, or, equivalently, b(i)/a(i) = m is the i-th */
/*  generalized eigenvalue of m A - B. */

/*                          H   H  _      _ */
/*  For left eigenvectors, A , B , a, and b  are used. */

/*  ZGET52 also tests the normalization of E.  Each eigenvector is */
/*  supposed to be normalized so that the maximum "absolute value" */
/*  of its elements is 1, where in this case, "absolute value" */
/*  of a complex value x is  |Re(x)| + |Im(x)| ; let us call this */
/*  maximum "absolute value" norm of a vector v  M(v). */
/*  If a(i)=b(i)=0, then the eigenvector is set to be the jth coordinate */
/*  vector. The normalization test is: */

/*          RESULT(2) =      max       | M(v(i)) - 1 | / ( n ulp ) */
/*                     eigenvectors v(i) */


/*  Arguments */
/*  ========= */

/*  LEFT    (input) LOGICAL */
/*          =.TRUE.:  The eigenvectors in the columns of E are assumed */
/*                    to be *left* eigenvectors. */
/*          =.FALSE.: The eigenvectors in the columns of E are assumed */
/*                    to be *right* eigenvectors. */

/*  N       (input) INTEGER */
/*          The size of the matrices.  If it is zero, ZGET52 does */
/*          nothing.  It must be at least zero. */

/*  A       (input) COMPLEX*16 array, dimension (LDA, N) */
/*          The matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) COMPLEX*16 array, dimension (LDB, N) */
/*          The matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  E       (input) COMPLEX*16 array, dimension (LDE, N) */
/*          The matrix of eigenvectors.  It must be O( 1 ). */

/*  LDE     (input) INTEGER */
/*          The leading dimension of E.  It must be at least 1 and at */
/*          least N. */

/*  ALPHA   (input) COMPLEX*16 array, dimension (N) */
/*          The values a(i) as described above, which, along with b(i), */
/*          define the generalized eigenvalues. */

/*  BETA    (input) COMPLEX*16 array, dimension (N) */
/*          The values b(i) as described above, which, along with a(i), */
/*          define the generalized eigenvalues. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (N**2) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
/*          The values computed by the test described above.  If A E or */
/*          B E is likely to overflow, then RESULT(1:2) is set to */
/*          10 / ulp. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    e_dim1 = *lde;
    e_offset = 1 + e_dim1;
    e -= e_offset;
    --alpha;
    --beta;
    --work;
    --rwork;
    --result;

    /* Function Body */
    result[1] = 0.;
    result[2] = 0.;
    if (*n <= 0) {
	return 0;
    }

    safmin = dlamch_("Safe minimum");
    safmax = 1. / safmin;
    ulp = dlamch_("Epsilon") * dlamch_("Base");

    if (*left) {
	*(unsigned char *)trans = 'C';
	*(unsigned char *)normab = 'I';
    } else {
	*(unsigned char *)trans = 'N';
	*(unsigned char *)normab = 'O';
    }

/*     Norm of A, B, and E: */

/* Computing MAX */
    d__1 = zlange_(normab, n, n, &a[a_offset], lda, &rwork[1]);
    anorm = max(d__1,safmin);
/* Computing MAX */
    d__1 = zlange_(normab, n, n, &b[b_offset], ldb, &rwork[1]);
    bnorm = max(d__1,safmin);
/* Computing MAX */
    d__1 = zlange_("O", n, n, &e[e_offset], lde, &rwork[1]);
    enorm = max(d__1,ulp);
    alfmax = safmax / max(1.,bnorm);
    betmax = safmax / max(1.,anorm);

/*     Compute error matrix. */
/*     Column i = ( b(i) A - a(i) B ) E(i) / max( |a(i) B| |b(i) A| ) */

    i__1 = *n;
    for (jvec = 1; jvec <= i__1; ++jvec) {
	i__2 = jvec;
	alphai.r = alpha[i__2].r, alphai.i = alpha[i__2].i;
	i__2 = jvec;
	betai.r = beta[i__2].r, betai.i = beta[i__2].i;
/* Computing MAX */
	d__5 = (d__1 = alphai.r, abs(d__1)) + (d__2 = d_imag(&alphai), abs(
		d__2)), d__6 = (d__3 = betai.r, abs(d__3)) + (d__4 = d_imag(&
		betai), abs(d__4));
	abmax = max(d__5,d__6);
	if ((d__1 = alphai.r, abs(d__1)) + (d__2 = d_imag(&alphai), abs(d__2))
		 > alfmax || (d__3 = betai.r, abs(d__3)) + (d__4 = d_imag(&
		betai), abs(d__4)) > betmax || abmax < 1.) {
	    scale = 1. / max(abmax,safmin);
	    z__1.r = scale * alphai.r, z__1.i = scale * alphai.i;
	    alphai.r = z__1.r, alphai.i = z__1.i;
	    z__1.r = scale * betai.r, z__1.i = scale * betai.i;
	    betai.r = z__1.r, betai.i = z__1.i;
	}
/* Computing MAX */
	d__5 = ((d__1 = alphai.r, abs(d__1)) + (d__2 = d_imag(&alphai), abs(
		d__2))) * bnorm, d__6 = ((d__3 = betai.r, abs(d__3)) + (d__4 =
		 d_imag(&betai), abs(d__4))) * anorm, d__5 = max(d__5,d__6);
	scale = 1. / max(d__5,safmin);
	z__1.r = scale * betai.r, z__1.i = scale * betai.i;
	acoeff.r = z__1.r, acoeff.i = z__1.i;
	z__1.r = scale * alphai.r, z__1.i = scale * alphai.i;
	bcoeff.r = z__1.r, bcoeff.i = z__1.i;
	if (*left) {
	    d_cnjg(&z__1, &acoeff);
	    acoeff.r = z__1.r, acoeff.i = z__1.i;
	    d_cnjg(&z__1, &bcoeff);
	    bcoeff.r = z__1.r, bcoeff.i = z__1.i;
	}
	zgemv_(trans, n, n, &acoeff, &a[a_offset], lda, &e[jvec * e_dim1 + 1], 
		 &c__1, &c_b1, &work[*n * (jvec - 1) + 1], &c__1);
	z__1.r = -bcoeff.r, z__1.i = -bcoeff.i;
	zgemv_(trans, n, n, &z__1, &b[b_offset], lda, &e[jvec * e_dim1 + 1], &
		c__1, &c_b2, &work[*n * (jvec - 1) + 1], &c__1);
/* L10: */
    }

    errnrm = zlange_("One", n, n, &work[1], n, &rwork[1]) / enorm;

/*     Compute RESULT(1) */

    result[1] = errnrm / ulp;

/*     Normalization of E: */

    enrmer = 0.;
    i__1 = *n;
    for (jvec = 1; jvec <= i__1; ++jvec) {
	temp1 = 0.;
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
	    i__3 = j + jvec * e_dim1;
	    d__3 = temp1, d__4 = (d__1 = e[i__3].r, abs(d__1)) + (d__2 = 
		    d_imag(&e[j + jvec * e_dim1]), abs(d__2));
	    temp1 = max(d__3,d__4);
/* L20: */
	}
/* Computing MAX */
	d__1 = enrmer, d__2 = temp1 - 1.;
	enrmer = max(d__1,d__2);
/* L30: */
    }

/*     Compute RESULT(2) : the normalization error in E. */

    result[2] = enrmer / ((doublereal) (*n) * ulp);

    return 0;

/*     End of ZGET52 */

} /* zget52_ */
