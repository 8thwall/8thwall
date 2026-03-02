#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static real c_b12 = 0.f;
static real c_b15 = 1.f;

/* Subroutine */ int sget52_(logical *left, integer *n, real *a, integer *lda, 
	 real *b, integer *ldb, real *e, integer *lde, real *alphar, real *
	alphai, real *beta, real *work, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, e_dim1, e_offset, i__1, i__2;
    real r__1, r__2, r__3, r__4;

    /* Local variables */
    integer j;
    real ulp;
    integer jvec;
    real temp1, acoef, scale, abmax, salfi, sbeta, salfr, anorm, bnorm, enorm;
    extern /* Subroutine */ int sgemv_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *);
    char trans[1];
    real bcoefi, bcoefr, alfmax;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    real safmin;
    char normab[1];
    real safmax, betmax, enrmer;
    logical ilcplx;
    real errnrm;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGET52  does an eigenvector check for the generalized eigenvalue */
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

/*  SGET52 also tests the normalization of E.  Each eigenvector is */
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
/*          The size of the matrices.  If it is zero, SGET52 does */
/*          nothing.  It must be at least zero. */

/*  A       (input) REAL array, dimension (LDA, N) */
/*          The matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  B       (input) REAL array, dimension (LDB, N) */
/*          The matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least 1 */
/*          and at least N. */

/*  E       (input) REAL array, dimension (LDE, N) */
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

/*  ALPHAR  (input) REAL array, dimension (N) */
/*          The real parts of the values a(j) as described above, which, */
/*          along with b(j), define the generalized eigenvalues. */
/*          Complex eigenvalues always come in complex conjugate pairs */
/*          a(j)/b(j) and a(j+1)/b(j+1), which are stored in adjacent */
/*          elements in ALPHAR, ALPHAI, and BETA.  Thus, if the j-th */
/*          and (j+1)-st eigenvalues form a pair, ALPHAR(j+1)/BETA(j+1) */
/*          is assumed to be equal to ALPHAR(j)/BETA(j). */

/*  ALPHAI  (input) REAL array, dimension (N) */
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

/*  BETA    (input) REAL array, dimension (N) */
/*          The values b(j) as described above, which, along with a(j), */
/*          define the generalized eigenvalues. */

/*  WORK    (workspace) REAL array, dimension (N**2+N) */

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
    result[1] = 0.f;
    result[2] = 0.f;
    if (*n <= 0) {
	return 0;
    }

    safmin = slamch_("Safe minimum");
    safmax = 1.f / safmin;
    ulp = slamch_("Epsilon") * slamch_("Base");

    if (*left) {
	*(unsigned char *)trans = 'T';
	*(unsigned char *)normab = 'I';
    } else {
	*(unsigned char *)trans = 'N';
	*(unsigned char *)normab = 'O';
    }

/*     Norm of A, B, and E: */

/* Computing MAX */
    r__1 = slange_(normab, n, n, &a[a_offset], lda, &work[1]);
    anorm = dmax(r__1,safmin);
/* Computing MAX */
    r__1 = slange_(normab, n, n, &b[b_offset], ldb, &work[1]);
    bnorm = dmax(r__1,safmin);
/* Computing MAX */
    r__1 = slange_("O", n, n, &e[e_offset], lde, &work[1]);
    enorm = dmax(r__1,ulp);
    alfmax = safmax / dmax(1.f,bnorm);
    betmax = safmax / dmax(1.f,anorm);

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
	    if (salfi == 0.f) {

/*              Real eigenvalue and -vector */

/* Computing MAX */
		r__1 = dabs(salfr), r__2 = dabs(sbeta);
		abmax = dmax(r__1,r__2);
		if (dabs(salfr) > alfmax || dabs(sbeta) > betmax || abmax < 
			1.f) {
		    scale = 1.f / dmax(abmax,safmin);
		    salfr = scale * salfr;
		    sbeta = scale * sbeta;
		}
/* Computing MAX */
		r__1 = dabs(salfr) * bnorm, r__2 = dabs(sbeta) * anorm, r__1 =
			 max(r__1,r__2);
		scale = 1.f / dmax(r__1,safmin);
		acoef = scale * sbeta;
		bcoefr = scale * salfr;
		sgemv_(trans, n, n, &acoef, &a[a_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b12, &work[*n * (jvec - 1) + 1]
, &c__1);
		r__1 = -bcoefr;
		sgemv_(trans, n, n, &r__1, &b[b_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * (jvec - 1) + 1]
, &c__1);
	    } else {

/*              Complex conjugate pair */

		ilcplx = TRUE_;
		if (jvec == *n) {
		    result[1] = 10.f / ulp;
		    return 0;
		}
/* Computing MAX */
		r__1 = dabs(salfr) + dabs(salfi), r__2 = dabs(sbeta);
		abmax = dmax(r__1,r__2);
		if (dabs(salfr) + dabs(salfi) > alfmax || dabs(sbeta) > 
			betmax || abmax < 1.f) {
		    scale = 1.f / dmax(abmax,safmin);
		    salfr = scale * salfr;
		    salfi = scale * salfi;
		    sbeta = scale * sbeta;
		}
/* Computing MAX */
		r__1 = (dabs(salfr) + dabs(salfi)) * bnorm, r__2 = dabs(sbeta)
			 * anorm, r__1 = max(r__1,r__2);
		scale = 1.f / dmax(r__1,safmin);
		acoef = scale * sbeta;
		bcoefr = scale * salfr;
		bcoefi = scale * salfi;
		if (*left) {
		    bcoefi = -bcoefi;
		}

		sgemv_(trans, n, n, &acoef, &a[a_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b12, &work[*n * (jvec - 1) + 1]
, &c__1);
		r__1 = -bcoefr;
		sgemv_(trans, n, n, &r__1, &b[b_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * (jvec - 1) + 1]
, &c__1);
		sgemv_(trans, n, n, &bcoefi, &b[b_offset], lda, &e[(jvec + 1) 
			* e_dim1 + 1], &c__1, &c_b15, &work[*n * (jvec - 1) + 
			1], &c__1);

		sgemv_(trans, n, n, &acoef, &a[a_offset], lda, &e[(jvec + 1) *
			 e_dim1 + 1], &c__1, &c_b12, &work[*n * jvec + 1], &
			c__1);
		r__1 = -bcoefi;
		sgemv_(trans, n, n, &r__1, &b[b_offset], lda, &e[jvec * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * jvec + 1], &
			c__1);
		r__1 = -bcoefr;
		sgemv_(trans, n, n, &r__1, &b[b_offset], lda, &e[(jvec + 1) * 
			e_dim1 + 1], &c__1, &c_b15, &work[*n * jvec + 1], &
			c__1);
	    }
	}
/* L10: */
    }

/* Computing 2nd power */
    i__1 = *n;
    errnrm = slange_("One", n, n, &work[1], n, &work[i__1 * i__1 + 1]) / enorm;

/*     Compute RESULT(1) */

    result[1] = errnrm / ulp;

/*     Normalization of E: */

    enrmer = 0.f;
    ilcplx = FALSE_;
    i__1 = *n;
    for (jvec = 1; jvec <= i__1; ++jvec) {
	if (ilcplx) {
	    ilcplx = FALSE_;
	} else {
	    temp1 = 0.f;
	    if (alphai[jvec] == 0.f) {
		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		    r__2 = temp1, r__3 = (r__1 = e[j + jvec * e_dim1], dabs(
			    r__1));
		    temp1 = dmax(r__2,r__3);
/* L20: */
		}
/* Computing MAX */
		r__1 = enrmer, r__2 = temp1 - 1.f;
		enrmer = dmax(r__1,r__2);
	    } else {
		ilcplx = TRUE_;
		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = e[j + jvec * e_dim1], dabs(
			    r__1)) + (r__2 = e[j + (jvec + 1) * e_dim1], dabs(
			    r__2));
		    temp1 = dmax(r__3,r__4);
/* L30: */
		}
/* Computing MAX */
		r__1 = enrmer, r__2 = temp1 - 1.f;
		enrmer = dmax(r__1,r__2);
	    }
	}
/* L40: */
    }

/*     Compute RESULT(2) : the normalization error in E. */

    result[2] = enrmer / ((real) (*n) * ulp);

    return 0;

/*     End of SGET52 */

} /* sget52_ */
