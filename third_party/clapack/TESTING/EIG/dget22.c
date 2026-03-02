#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b20 = 0.;
static integer c__2 = 2;
static doublereal c_b25 = 1.;
static integer c__1 = 1;
static doublereal c_b30 = -1.;

/* Subroutine */ int dget22_(char *transa, char *transe, char *transw, 
	integer *n, doublereal *a, integer *lda, doublereal *e, integer *lde, 
	doublereal *wr, doublereal *wi, doublereal *work, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, e_dim1, e_offset, i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Local variables */
    integer j;
    doublereal ulp;
    integer ince, jcol, jvec;
    doublereal unfl, wmat[4]	/* was [2][2] */, temp1;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    integer iecol;
    extern logical lsame_(char *, char *);
    integer ipair;
    char norma[1];
    doublereal anorm;
    char norme[1];
    doublereal enorm;
    integer ierow;
    extern /* Subroutine */ int daxpy_(integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *);
    doublereal enrmin, enrmax;
    integer itrnse;
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

/*  DGET22 does an eigenvector check. */

/*  The basic test is: */

/*     RESULT(1) = | A E  -  E W | / ( |A| |E| ulp ) */

/*  using the 1-norm.  It also tests the normalization of E: */

/*     RESULT(2) = max | m-norm(E(j)) - 1 | / ( n ulp ) */
/*                  j */

/*  where E(j) is the j-th eigenvector, and m-norm is the max-norm of a */
/*  vector.  If an eigenvector is complex, as determined from WI(j) */
/*  nonzero, then the max-norm of the vector ( er + i*ei ) is the maximum */
/*  of */
/*     |er(1)| + |ei(1)|, ... , |er(n)| + |ei(n)| */

/*  W is a block diagonal matrix, with a 1 by 1 block for each real */
/*  eigenvalue and a 2 by 2 block for each complex conjugate pair. */
/*  If eigenvalues j and j+1 are a complex conjugate pair, so that */
/*  WR(j) = WR(j+1) = wr and WI(j) = - WI(j+1) = wi, then the 2 by 2 */
/*  block corresponding to the pair will be: */

/*     (  wr  wi  ) */
/*     ( -wi  wr  ) */

/*  Such a block multiplying an n by 2 matrix ( ur ui ) on the right */
/*  will be the same as multiplying  ur + i*ui  by  wr + i*wi. */

/*  To handle various schemes for storage of left eigenvectors, there are */
/*  options to use A-transpose instead of A, E-transpose instead of E, */
/*  and/or W-transpose instead of W. */

/*  Arguments */
/*  ========== */

/*  TRANSA  (input) CHARACTER*1 */
/*          Specifies whether or not A is transposed. */
/*          = 'N':  No transpose */
/*          = 'T':  Transpose */
/*          = 'C':  Conjugate transpose (= Transpose) */

/*  TRANSE  (input) CHARACTER*1 */
/*          Specifies whether or not E is transposed. */
/*          = 'N':  No transpose, eigenvectors are in columns of E */
/*          = 'T':  Transpose, eigenvectors are in rows of E */
/*          = 'C':  Conjugate transpose (= Transpose) */

/*  TRANSW  (input) CHARACTER*1 */
/*          Specifies whether or not W is transposed. */
/*          = 'N':  No transpose */
/*          = 'T':  Transpose, use -WI(j) instead of WI(j) */
/*          = 'C':  Conjugate transpose, use -WI(j) instead of WI(j) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The matrix whose eigenvectors are in E. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  E       (input) DOUBLE PRECISION array, dimension (LDE,N) */
/*          The matrix of eigenvectors. If TRANSE = 'N', the eigenvectors */
/*          are stored in the columns of E, if TRANSE = 'T' or 'C', the */
/*          eigenvectors are stored in the rows of E. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of the array E.  LDE >= max(1,N). */

/*  WR      (input) DOUBLE PRECISION array, dimension (N) */
/*  WI      (input) DOUBLE PRECISION array, dimension (N) */
/*          The real and imaginary parts of the eigenvalues of A. */
/*          Purely real eigenvalues are indicated by WI(j) = 0. */
/*          Complex conjugate pairs are indicated by WR(j)=WR(j+1) and */
/*          WI(j) = - WI(j+1) non-zero; the real part is assumed to be */
/*          stored in the j-th row/column and the imaginary part in */
/*          the (j+1)-th row/column. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N*(N+1)) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
/*          RESULT(1) = | A E  -  E W | / ( |A| |E| ulp ) */
/*          RESULT(2) = max | m-norm(E(j)) - 1 | / ( n ulp ) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize RESULT (in case N=0) */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    e_dim1 = *lde;
    e_offset = 1 + e_dim1;
    e -= e_offset;
    --wr;
    --wi;
    --work;
    --result;

    /* Function Body */
    result[1] = 0.;
    result[2] = 0.;
    if (*n <= 0) {
	return 0;
    }

    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Precision");

    itrnse = 0;
    ince = 1;
    *(unsigned char *)norma = 'O';
    *(unsigned char *)norme = 'O';

    if (lsame_(transa, "T") || lsame_(transa, "C")) {
	*(unsigned char *)norma = 'I';
    }
    if (lsame_(transe, "T") || lsame_(transe, "C")) {
	*(unsigned char *)norme = 'I';
	itrnse = 1;
	ince = *lde;
    }

/*     Check normalization of E */

    enrmin = 1. / ulp;
    enrmax = 0.;
    if (itrnse == 0) {

/*        Eigenvectors are column vectors. */

	ipair = 0;
	i__1 = *n;
	for (jvec = 1; jvec <= i__1; ++jvec) {
	    temp1 = 0.;
	    if (ipair == 0 && jvec < *n && wi[jvec] != 0.) {
		ipair = 1;
	    }
	    if (ipair == 1) {

/*              Complex eigenvector */

		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = e[j + jvec * e_dim1], abs(
			    d__1)) + (d__2 = e[j + (jvec + 1) * e_dim1], abs(
			    d__2));
		    temp1 = max(d__3,d__4);
/* L10: */
		}
		enrmin = min(enrmin,temp1);
		enrmax = max(enrmax,temp1);
		ipair = 2;
	    } else if (ipair == 2) {
		ipair = 0;
	    } else {

/*              Real eigenvector */

		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* Computing MAX */
		    d__2 = temp1, d__3 = (d__1 = e[j + jvec * e_dim1], abs(
			    d__1));
		    temp1 = max(d__2,d__3);
/* L20: */
		}
		enrmin = min(enrmin,temp1);
		enrmax = max(enrmax,temp1);
		ipair = 0;
	    }
/* L30: */
	}

    } else {

/*        Eigenvectors are row vectors. */

	i__1 = *n;
	for (jvec = 1; jvec <= i__1; ++jvec) {
	    work[jvec] = 0.;
/* L40: */
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    ipair = 0;
	    i__2 = *n;
	    for (jvec = 1; jvec <= i__2; ++jvec) {
		if (ipair == 0 && jvec < *n && wi[jvec] != 0.) {
		    ipair = 1;
		}
		if (ipair == 1) {
/* Computing MAX */
		    d__3 = work[jvec], d__4 = (d__1 = e[j + jvec * e_dim1], 
			    abs(d__1)) + (d__2 = e[j + (jvec + 1) * e_dim1], 
			    abs(d__2));
		    work[jvec] = max(d__3,d__4);
		    work[jvec + 1] = work[jvec];
		} else if (ipair == 2) {
		    ipair = 0;
		} else {
/* Computing MAX */
		    d__2 = work[jvec], d__3 = (d__1 = e[j + jvec * e_dim1], 
			    abs(d__1));
		    work[jvec] = max(d__2,d__3);
		    ipair = 0;
		}
/* L50: */
	    }
/* L60: */
	}

	i__1 = *n;
	for (jvec = 1; jvec <= i__1; ++jvec) {
/* Computing MIN */
	    d__1 = enrmin, d__2 = work[jvec];
	    enrmin = min(d__1,d__2);
/* Computing MAX */
	    d__1 = enrmax, d__2 = work[jvec];
	    enrmax = max(d__1,d__2);
/* L70: */
	}
    }

/*     Norm of A: */

/* Computing MAX */
    d__1 = dlange_(norma, n, n, &a[a_offset], lda, &work[1]);
    anorm = max(d__1,unfl);

/*     Norm of E: */

/* Computing MAX */
    d__1 = dlange_(norme, n, n, &e[e_offset], lde, &work[1]);
    enorm = max(d__1,ulp);

/*     Norm of error: */

/*     Error =  AE - EW */

    dlaset_("Full", n, n, &c_b20, &c_b20, &work[1], n);

    ipair = 0;
    ierow = 1;
    iecol = 1;

    i__1 = *n;
    for (jcol = 1; jcol <= i__1; ++jcol) {
	if (itrnse == 1) {
	    ierow = jcol;
	} else {
	    iecol = jcol;
	}

	if (ipair == 0 && wi[jcol] != 0.) {
	    ipair = 1;
	}

	if (ipair == 1) {
	    wmat[0] = wr[jcol];
	    wmat[1] = -wi[jcol];
	    wmat[2] = wi[jcol];
	    wmat[3] = wr[jcol];
	    dgemm_(transe, transw, n, &c__2, &c__2, &c_b25, &e[ierow + iecol *
		     e_dim1], lde, wmat, &c__2, &c_b20, &work[*n * (jcol - 1) 
		    + 1], n);
	    ipair = 2;
	} else if (ipair == 2) {
	    ipair = 0;

	} else {

	    daxpy_(n, &wr[jcol], &e[ierow + iecol * e_dim1], &ince, &work[*n *
		     (jcol - 1) + 1], &c__1);
	    ipair = 0;
	}

/* L80: */
    }

    dgemm_(transa, transe, n, n, n, &c_b25, &a[a_offset], lda, &e[e_offset], 
	    lde, &c_b30, &work[1], n);

    errnrm = dlange_("One", n, n, &work[1], n, &work[*n * *n + 1]) 
	    / enorm;

/*     Compute RESULT(1) (avoiding under/overflow) */

    if (anorm > errnrm) {
	result[1] = errnrm / anorm / ulp;
    } else {
	if (anorm < 1.) {
	    result[1] = min(errnrm,anorm) / anorm / ulp;
	} else {
/* Computing MIN */
	    d__1 = errnrm / anorm;
	    result[1] = min(d__1,1.) / ulp;
	}
    }

/*     Compute RESULT(2) : the normalization error in E. */

/* Computing MAX */
    d__3 = (d__1 = enrmax - 1., abs(d__1)), d__4 = (d__2 = enrmin - 1., abs(
	    d__2));
    result[2] = max(d__3,d__4) / ((doublereal) (*n) * ulp);

    return 0;

/*     End of DGET22 */

} /* dget22_ */
