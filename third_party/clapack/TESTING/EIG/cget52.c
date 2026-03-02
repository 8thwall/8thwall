#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__1 = 1;

/* Subroutine */ int cget52_(logical *left, integer *n, complex *a, integer *
	lda, complex *b, integer *ldb, complex *e, integer *lde, complex *
	alpha, complex *beta, complex *work, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, e_dim1, e_offset, i__1, i__2, 
	    i__3;
    real r__1, r__2, r__3, r__4, r__5, r__6;
    complex q__1;

    /* Builtin functions */
    double r_imag(complex *);
    void r_cnjg(complex *, complex *);

    /* Local variables */
    integer j;
    real ulp;
    integer jvec;
    real temp1;
    complex betai;
    real scale, abmax;
    extern /* Subroutine */ int cgemv_(char *, integer *, integer *, complex *
, complex *, integer *, complex *, integer *, complex *, complex *
, integer *);
    real anorm, bnorm, enorm;
    char trans[1];
    complex acoeff, bcoeff;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *);
    complex alphai;
    extern doublereal slamch_(char *);
    real alfmax, safmin;
    char normab[1];
    real safmax, betmax, enrmer, errnrm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGET52  does an eigenvector check for the generalized eigenvalue */
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

/*  CGET52 also tests the normalization of E.  Each eigenvector is */
/*  supposed to be normalized so that the maximum "absolute value" */
/*  of its elements is 1, where in this case, "absolute value" */
/*  of a complex value x is  |Re(x)| + |Im(x)| ; let us call this */
/*  maximum "absolute value" norm of a vector v  M(v). */
/*  if a(i)=b(i)=0, then the eigenvector is set to be the jth coordinate */
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
/*          The size of the matrices.  If it is zero, CGET52 does */
/*          nothing.  It must be at least zero. */

/*  A       (input) COMPLEX array, dimension (LDA, N) */
/*          The matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) COMPLEX array, dimension (LDB, N) */
/*          The matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  E       (input) COMPLEX array, dimension (LDE, N) */
/*          The matrix of eigenvectors.  It must be O( 1 ). */

/*  LDE     (input) INTEGER */
/*          The leading dimension of E.  It must be at least 1 and at */
/*          least N. */

/*  ALPHA   (input) COMPLEX array, dimension (N) */
/*          The values a(i) as described above, which, along with b(i), */
/*          define the generalized eigenvalues. */

/*  BETA    (input) COMPLEX array, dimension (N) */
/*          The values b(i) as described above, which, along with a(i), */
/*          define the generalized eigenvalues. */

/*  WORK    (workspace) COMPLEX array, dimension (N**2) */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESULT  (output) REAL array, dimension (2) */
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
    result[1] = 0.f;
    result[2] = 0.f;
    if (*n <= 0) {
	return 0;
    }

    safmin = slamch_("Safe minimum");
    safmax = 1.f / safmin;
    ulp = slamch_("Epsilon") * slamch_("Base");

    if (*left) {
	*(unsigned char *)trans = 'C';
	*(unsigned char *)normab = 'I';
    } else {
	*(unsigned char *)trans = 'N';
	*(unsigned char *)normab = 'O';
    }

/*     Norm of A, B, and E: */

/* Computing MAX */
    r__1 = clange_(normab, n, n, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,safmin);
/* Computing MAX */
    r__1 = clange_(normab, n, n, &b[b_offset], ldb, &rwork[1]);
    bnorm = dmax(r__1,safmin);
/* Computing MAX */
    r__1 = clange_("O", n, n, &e[e_offset], lde, &rwork[1]);
    enorm = dmax(r__1,ulp);
    alfmax = safmax / dmax(1.f,bnorm);
    betmax = safmax / dmax(1.f,anorm);

/*     Compute error matrix. */
/*     Column i = ( b(i) A - a(i) B ) E(i) / max( |a(i) B| |b(i) A| ) */

    i__1 = *n;
    for (jvec = 1; jvec <= i__1; ++jvec) {
	i__2 = jvec;
	alphai.r = alpha[i__2].r, alphai.i = alpha[i__2].i;
	i__2 = jvec;
	betai.r = beta[i__2].r, betai.i = beta[i__2].i;
/* Computing MAX */
	r__5 = (r__1 = alphai.r, dabs(r__1)) + (r__2 = r_imag(&alphai), dabs(
		r__2)), r__6 = (r__3 = betai.r, dabs(r__3)) + (r__4 = r_imag(&
		betai), dabs(r__4));
	abmax = dmax(r__5,r__6);
	if ((r__1 = alphai.r, dabs(r__1)) + (r__2 = r_imag(&alphai), dabs(
		r__2)) > alfmax || (r__3 = betai.r, dabs(r__3)) + (r__4 = 
		r_imag(&betai), dabs(r__4)) > betmax || abmax < 1.f) {
	    scale = 1.f / dmax(abmax,safmin);
	    q__1.r = scale * alphai.r, q__1.i = scale * alphai.i;
	    alphai.r = q__1.r, alphai.i = q__1.i;
	    q__1.r = scale * betai.r, q__1.i = scale * betai.i;
	    betai.r = q__1.r, betai.i = q__1.i;
	}
/* Computing MAX */
	r__5 = ((r__1 = alphai.r, dabs(r__1)) + (r__2 = r_imag(&alphai), dabs(
		r__2))) * bnorm, r__6 = ((r__3 = betai.r, dabs(r__3)) + (r__4 
		= r_imag(&betai), dabs(r__4))) * anorm, r__5 = max(r__5,r__6);
	scale = 1.f / dmax(r__5,safmin);
	q__1.r = scale * betai.r, q__1.i = scale * betai.i;
	acoeff.r = q__1.r, acoeff.i = q__1.i;
	q__1.r = scale * alphai.r, q__1.i = scale * alphai.i;
	bcoeff.r = q__1.r, bcoeff.i = q__1.i;
	if (*left) {
	    r_cnjg(&q__1, &acoeff);
	    acoeff.r = q__1.r, acoeff.i = q__1.i;
	    r_cnjg(&q__1, &bcoeff);
	    bcoeff.r = q__1.r, bcoeff.i = q__1.i;
	}
	cgemv_(trans, n, n, &acoeff, &a[a_offset], lda, &e[jvec * e_dim1 + 1], 
		 &c__1, &c_b1, &work[*n * (jvec - 1) + 1], &c__1);
	q__1.r = -bcoeff.r, q__1.i = -bcoeff.i;
	cgemv_(trans, n, n, &q__1, &b[b_offset], lda, &e[jvec * e_dim1 + 1], &
		c__1, &c_b2, &work[*n * (jvec - 1) + 1], &c__1);
/* L10: */
    }

    errnrm = clange_("One", n, n, &work[1], n, &rwork[1]) / enorm;

/*     Compute RESULT(1) */

    result[1] = errnrm / ulp;

/*     Normalization of E: */

    enrmer = 0.f;
    i__1 = *n;
    for (jvec = 1; jvec <= i__1; ++jvec) {
	temp1 = 0.f;
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
	    i__3 = j + jvec * e_dim1;
	    r__3 = temp1, r__4 = (r__1 = e[i__3].r, dabs(r__1)) + (r__2 = 
		    r_imag(&e[j + jvec * e_dim1]), dabs(r__2));
	    temp1 = dmax(r__3,r__4);
/* L20: */
	}
/* Computing MAX */
	r__1 = enrmer, r__2 = temp1 - 1.f;
	enrmer = dmax(r__1,r__2);
/* L30: */
    }

/*     Compute RESULT(2) : the normalization error in E. */

    result[2] = enrmer / ((real) (*n) * ulp);

    return 0;

/*     End of CGET52 */

} /* cget52_ */
