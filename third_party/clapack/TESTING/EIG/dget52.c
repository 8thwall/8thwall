#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b12 = 0.;
static doublereal c_b15 = 1.;

/* Subroutine */ int dget52_(logical *left, integer *n, doublereal *a, 
	integer *lda, doublereal *b, integer *ldb, doublereal *e, integer *
	lde, doublereal *alphar, doublereal *alphai, doublereal *beta, 
	doublereal *work, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, e_dim1, e_offset, i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Local variables */
    integer j;
    doublereal ulp;
    integer jvec;
    doublereal temp1, acoef, scale, abmax, salfi, sbeta;
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    doublereal salfr, anorm, bnorm, enorm;
    char trans[1];
    doublereal bcoefi;
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    doublereal bcoefr, alfmax, safmin;
    char normab[1];
    doublereal safmax, betmax, enrmer;
    logical ilcplx;
    doublereal errnrm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGET52  does an eigenvector check for the generalized eigenvalue */
/*  problem. */

/*  The basic test for right eigenvectors is: */

/*                            | b(j) A E(j) -  a(j) B E(j) | */
/*          RESULT(1) = max   ------------------------------- */
/*                       j    n ulp max( |b(j) A|, |a(j) B| ) */

/*  using the 1-norm.  Here, a(j)/b(j) = w is the j-th generalized */
/*  eigenvalue of A - w B, or, equivalently, b(j)/a(j) = m is the j-th */
/*  generalized eigenvalue of m A - B. */

/*  For real eigenvalues, the test is straightforward.  For complex */
/*  eigenvalues, E(j) and a(j) are complex, represented by */
/*  Er(j) + i*Ei(j) and ar(j) + i*ai(j), resp., so the test for that */
/*  eigenvector becomes */

/*                  max( |Wr|, |Wi| ) */
/*      -------------------------------------------- */
/*      n ulp max( |b(j) A|, (|ar(j)|+|ai(j)|) |B| ) */

/*  where */

/*      Wr = b(j) A Er(j) - ar(j) B Er(j) + ai(j) B Ei(j) */

/*      Wi = b(j) A Ei(j) - ai(j) B Er(j) - ar(j) B Ei(j) */

/*                          T   T  _ */
/*  For left eigenvectors, A , B , a, and b  are used. */

/*  DGET52 also tests the normalization of E.  Each eigenvector is */
/*  supposed to be normalized so that the maximum "absolute value" */
/*  of its elements is 1, where in this case, "absolute value" */
/*  of a complex value x is  |Re(x)| + |Im(x)| ; let us call this */
/*  maximum "absolute value" norm of a vector v  M(v). */
/*  if a(j)=b(j)=0, then the eigenvector is set to be the jth coordinate */
/*  vector.  The normalization test is: */

/*          RESULT(2) =      max       | M(v(j)) - 1 | / ( n ulp ) */
/*                     eigenvectors v(j) */

/*  Arguments */
/*  ========= */

/*  LEFT    (input) LOGICAL */
/*          =.TRUE.:  The eigenvectors in the columns of E are assumed */
/*                    to be *left* eigenvectors. */
/*          =.FALSE.: The eigenvectors in the columns of E are assumed */
/*                    to be *right* eigenvectors. */

/*  N       (input) INTEGER */
/*          The size of the matrices.  If it is zero, DGET52 does */
/*          nothing.  It must be at least zero. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, N) */
/*          The matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB, N) */
/*          The matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  E       (input) DOUBLE PRECISION array, dimension (LDE, N) */
/*          The matrix of eigenvectors.  It must be O( 1 ).  Complex */
/*          eigenvalues and eigenvectors always come in pairs, the */
/*          eigenvalue and its conjugate being stored in adjacent */
/*          elements of ALPHAR, ALPHAI, and BETA.  Thus, if a(j)/b(j) */
/*          and a(j+1)/b(j+1) are a complex conjugate pair of */
/*          generalized eigenvalues, then E(,j) contains the real part */
/*          of the eigenvector and E(,j+1) contains the imaginary part. */
/*          Note that whether E(,j) is a real eigenvector or part of a */
/*          complex one is specified by whether ALPHAI(j) is zero or not. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of E.  It must be at least 1 and at */
/*          least N. */

/*  ALPHAR  (input) DOUBLE PRECISION array, dimension (N) */
/*          The real parts of the values a(j) as described above, which, */
/*          along with b(j), define the generalized eigenvalues. */
/*          Complex eigenvalues always come in complex conjugate pairs */
/*          a(j)/b(j) and a(j+1)/b(j+1), which are stored in adjacent */
/*          elements in ALPHAR, ALPHAI, and BETA.  Thus, if the j-th */
/*          and (j+1)-st eigenvalues form a pair, ALPHAR(j+1)/BETA(j+1) */
/*          is assumed to be equal to ALPHAR(j)/BETA(j). */

/*  ALPHAI  (input) DOUBLE PRECISION array, dimension (N) */
/*          The imaginary parts of the values a(j) as described above, */
/*          which, along with b(j), define the generalized eigenvalues. */
/*          If ALPHAI(j)=0, then the eigenvalue is real, otherwise it */
/*          is part of a complex conjugate pair.  Complex eigenvalues */
/*          always come in complex conjugate pairs a(j)/b(j) and */
/*          a(j+1)/b(j+1), which are stored in adjacent elements in */
/*          ALPHAR, ALPHAI, and BETA.  Thus, if the j-th and (j+1)-st */
/*          eigenvalues form a pair, ALPHAI(j+1)/BETA(j+1) is assumed to */
/*          be equal to  -ALPHAI(j)/BETA(j).  Also, nonzero values in */
/*          ALPHAI are assumed to always come in adjacent pairs. */

/*  BETA    (input) DOUBLE PRECISION array, dimension (N) */
/*          The values b(j) as described above, which, along with a(j), */
/*          define the generalized eigenvalues. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N**2+N) */

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
    --alphar;
    --alphai;
    --beta;
    --work;
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
	*(unsigned char *)trans = 'T';
	*(unsigned char *)normab = 'I';
    } else {
	*(unsigned char *)trans = 'N';
	*(unsigned char *)normab = 'O';
    }

/*     Norm of A, B, and E: */

/* Computing MAX */
    d__1 = dlange_(normab, n, n, &a[a_offset], lda, &work[1]);
    anorm = max(d__1,safmin);
/* Computing MAX */
    d__1 = dlange_(normab, n, n, &b[b_offset], ldb, &work[1]);
    bnorm = max(d__1,safmin);
/* Computing MAX */
    d__1 = dlange_("O", n, n, &e[e_offset], lde, &work[1]);
    enorm = max(d__1,ulp);
    alfmax = safmax / max(1.,bnorm);
    betmax = safmax / max(1.,anorm);

/*     Compute error matrix. */
/*     Column i = ( b(i) A - a(i) B ) E(i) / max( |a(i) B| |b(i) A| ) */

    ilcplx = FALSE_;
    i__1 = *n;
    for (jvec = 1; jvec <= i__1; ++jvec) {
	if (ilcplx) {

/*           2nd Eigenvalue/-vector of pair -- do nothing */

	    ilcplx = FALSE_;
	} else {
	    salfr = alphar[jvec];
	    salfi = alphai[jvec];
	    sbeta = beta[jvec];
	    if (salfi == 0.) {

/*              Real eigenvalue and -vector */

/* Computing MAX */
		d__1 = abs(salfr), d__2 = abs(sbeta);
		abmax = max(d__1,d__2);
		if (abs(salfr) > alfmax || abs(sbeta) > betmax || abmax < 1.) 
			{
		    scale = 1. / max(abmax,safmin);
		    salfr = scale * salfr;
		    sbeta = scale * sbeta;
		}
/* Computing MAX */
		d__1 = abs(salfr) * bnorm, d__2 = abs(sbeta) * anorm, d__1 = 
			max(d__1,d__2);
		scale = 1. / max(d__1,safmin);
		acoef = scale * sbeta;
		bcoefr = scale * salfr;
		dgemv_(trans, n, n, &acoef, &a[a_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b12, &work[*n * (jvec - 1) + 1]
, &c__1);
		d__1 = -bcoefr;
		dgemv_(trans, n, n, &d__1, &b[b_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * (jvec - 1) + 1]
, &c__1);
	    } else {

/*              Complex conjugate pair */

		ilcplx = TRUE_;
		if (jvec == *n) {
		    result[1] = 10. / ulp;
		    return 0;
		}
/* Computing MAX */
		d__1 = abs(salfr) + abs(salfi), d__2 = abs(sbeta);
		abmax = max(d__1,d__2);
		if (abs(salfr) + abs(salfi) > alfmax || abs(sbeta) > betmax ||
			 abmax < 1.) {
		    scale = 1. / max(abmax,safmin);
		    salfr = scale * salfr;
		    salfi = scale * salfi;
		    sbeta = scale * sbeta;
		}
/* Computing MAX */
		d__1 = (abs(salfr) + abs(salfi)) * bnorm, d__2 = abs(sbeta) * 
			anorm, d__1 = max(d__1,d__2);
		scale = 1. / max(d__1,safmin);
		acoef = scale * sbeta;
		bcoefr = scale * salfr;
		bcoefi = scale * salfi;
		if (*left) {
		    bcoefi = -bcoefi;
		}

		dgemv_(trans, n, n, &acoef, &a[a_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b12, &work[*n * (jvec - 1) + 1]
, &c__1);
		d__1 = -bcoefr;
		dgemv_(trans, n, n, &d__1, &b[b_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * (jvec - 1) + 1]
, &c__1);
		dgemv_(trans, n, n, &bcoefi, &b[b_offset], lda, &e[(jvec + 1) 
			* e_dim1 + 1], &c__1, &c_b15, &work[*n * (jvec - 1) + 
			1], &c__1);

		dgemv_(trans, n, n, &acoef, &a[a_offset], lda, &e[(jvec + 1) *
			 e_dim1 + 1], &c__1, &c_b12, &work[*n * jvec + 1], &
			c__1);
		d__1 = -bcoefi;
		dgemv_(trans, n, n, &d__1, &b[b_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * jvec + 1], &
			c__1);
		d__1 = -bcoefr;
		dgemv_(trans, n, n, &d__1, &b[b_offset], lda, &e[(jvec + 1) * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * jvec + 1], &
			c__1);
	    }
	}
/* L10: */
    }

/* Computing 2nd power */
    i__1 = *n;
    errnrm = dlange_("One", n, n, &work[1], n, &work[i__1 * i__1 + 1]) / enorm;

/*     Compute RESULT(1) */

    result[1] = errnrm / ulp;

/*     Normalization of E: */

    enrmer = 0.;
    ilcplx = FALSE_;
    i__1 = *n;
    for (jvec = 1; jvec <= i__1; ++jvec) {
	if (ilcplx) {
	    ilcplx = FALSE_;
	} else {
	    temp1 = 0.;
	    if (alphai[jvec] == 0.) {
		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		    d__2 = temp1, d__3 = (d__1 = e[j + jvec * e_dim1], abs(
			    d__1));
		    temp1 = max(d__2,d__3);
/* L20: */
		}
/* Computing MAX */
		d__1 = enrmer, d__2 = temp1 - 1.;
		enrmer = max(d__1,d__2);
	    } else {
		ilcplx = TRUE_;
		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = e[j + jvec * e_dim1], abs(
			    d__1)) + (d__2 = e[j + (jvec + 1) * e_dim1], abs(
			    d__2));
		    temp1 = max(d__3,d__4);
/* L30: */
		}
/* Computing MAX */
		d__1 = enrmer, d__2 = temp1 - 1.;
		enrmer = max(d__1,d__2);
	    }
	}
/* L40: */
    }

/*     Compute RESULT(2) : the normalization error in E. */

    result[2] = enrmer / ((doublereal) (*n) * ulp);

    return 0;

/*     End of DGET52 */

} /* dget52_ */
