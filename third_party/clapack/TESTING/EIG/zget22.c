#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};

/* Subroutine */ int zget22_(char *transa, char *transe, char *transw, 
	integer *n, doublecomplex *a, integer *lda, doublecomplex *e, integer 
	*lde, doublecomplex *w, doublecomplex *work, doublereal *rwork, 
	doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, e_dim1, e_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3, d__4;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    double d_imag(doublecomplex *);
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    integer j;
    doublereal ulp;
    integer joff, jcol, jvec;
    doublereal unfl;
    integer jrow;
    doublereal temp1;
    extern logical lsame_(char *, char *);
    char norma[1];
    doublereal anorm;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);
    char norme[1];
    doublereal enorm;
    doublecomplex wtemp;
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    doublereal enrmin, enrmax;
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);
    integer itrnse;
    doublereal errnrm;
    integer itrnsw;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGET22 does an eigenvector check. */

/*  The basic test is: */

/*     RESULT(1) = | A E  -  E W | / ( |A| |E| ulp ) */

/*  using the 1-norm.  It also tests the normalization of E: */

/*     RESULT(2) = max | m-norm(E(j)) - 1 | / ( n ulp ) */
/*                  j */

/*  where E(j) is the j-th eigenvector, and m-norm is the max-norm of a */
/*  vector.  The max-norm of a complex n-vector x in this case is the */
/*  maximum of |re(x(i)| + |im(x(i)| over i = 1, ..., n. */

/*  Arguments */
/*  ========== */

/*  TRANSA  (input) CHARACTER*1 */
/*          Specifies whether or not A is transposed. */
/*          = 'N':  No transpose */
/*          = 'T':  Transpose */
/*          = 'C':  Conjugate transpose */

/*  TRANSE  (input) CHARACTER*1 */
/*          Specifies whether or not E is transposed. */
/*          = 'N':  No transpose, eigenvectors are in columns of E */
/*          = 'T':  Transpose, eigenvectors are in rows of E */
/*          = 'C':  Conjugate transpose, eigenvectors are in rows of E */

/*  TRANSW  (input) CHARACTER*1 */
/*          Specifies whether or not W is transposed. */
/*          = 'N':  No transpose */
/*          = 'T':  Transpose, same as TRANSW = 'N' */
/*          = 'C':  Conjugate transpose, use -WI(j) instead of WI(j) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The matrix whose eigenvectors are in E. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  E       (input) COMPLEX*16 array, dimension (LDE,N) */
/*          The matrix of eigenvectors. If TRANSE = 'N', the eigenvectors */
/*          are stored in the columns of E, if TRANSE = 'T' or 'C', the */
/*          eigenvectors are stored in the rows of E. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of the array E.  LDE >= max(1,N). */

/*  W       (input) COMPLEX*16 array, dimension (N) */
/*          The eigenvalues of A. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (N*N) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
/*          RESULT(1) = | A E  -  E W | / ( |A| |E| ulp ) */
/*          RESULT(2) = max | m-norm(E(j)) - 1 | / ( n ulp ) */
/*                       j */
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

/*     Initialize RESULT (in case N=0) */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    e_dim1 = *lde;
    e_offset = 1 + e_dim1;
    e -= e_offset;
    --w;
    --work;
    --rwork;
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
    itrnsw = 0;
    *(unsigned char *)norma = 'O';
    *(unsigned char *)norme = 'O';

    if (lsame_(transa, "T") || lsame_(transa, "C")) {
	*(unsigned char *)norma = 'I';
    }

    if (lsame_(transe, "T")) {
	itrnse = 1;
	*(unsigned char *)norme = 'I';
    } else if (lsame_(transe, "C")) {
	itrnse = 2;
	*(unsigned char *)norme = 'I';
    }

    if (lsame_(transw, "C")) {
	itrnsw = 1;
    }

/*     Normalization of E: */

    enrmin = 1. / ulp;
    enrmax = 0.;
    if (itrnse == 0) {
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
/* L10: */
	    }
	    enrmin = min(enrmin,temp1);
	    enrmax = max(enrmax,temp1);
/* L20: */
	}
    } else {
	i__1 = *n;
	for (jvec = 1; jvec <= i__1; ++jvec) {
	    rwork[jvec] = 0.;
/* L30: */
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (jvec = 1; jvec <= i__2; ++jvec) {
/* Computing MAX */
		i__3 = jvec + j * e_dim1;
		d__3 = rwork[jvec], d__4 = (d__1 = e[i__3].r, abs(d__1)) + (
			d__2 = d_imag(&e[jvec + j * e_dim1]), abs(d__2));
		rwork[jvec] = max(d__3,d__4);
/* L40: */
	    }
/* L50: */
	}

	i__1 = *n;
	for (jvec = 1; jvec <= i__1; ++jvec) {
/* Computing MIN */
	    d__1 = enrmin, d__2 = rwork[jvec];
	    enrmin = min(d__1,d__2);
/* Computing MAX */
	    d__1 = enrmax, d__2 = rwork[jvec];
	    enrmax = max(d__1,d__2);
/* L60: */
	}
    }

/*     Norm of A: */

/* Computing MAX */
    d__1 = zlange_(norma, n, n, &a[a_offset], lda, &rwork[1]);
    anorm = max(d__1,unfl);

/*     Norm of E: */

/* Computing MAX */
    d__1 = zlange_(norme, n, n, &e[e_offset], lde, &rwork[1]);
    enorm = max(d__1,ulp);

/*     Norm of error: */

/*     Error =  AE - EW */

    zlaset_("Full", n, n, &c_b1, &c_b1, &work[1], n);

    joff = 0;
    i__1 = *n;
    for (jcol = 1; jcol <= i__1; ++jcol) {
	if (itrnsw == 0) {
	    i__2 = jcol;
	    wtemp.r = w[i__2].r, wtemp.i = w[i__2].i;
	} else {
	    d_cnjg(&z__1, &w[jcol]);
	    wtemp.r = z__1.r, wtemp.i = z__1.i;
	}

	if (itrnse == 0) {
	    i__2 = *n;
	    for (jrow = 1; jrow <= i__2; ++jrow) {
		i__3 = joff + jrow;
		i__4 = jrow + jcol * e_dim1;
		z__1.r = e[i__4].r * wtemp.r - e[i__4].i * wtemp.i, z__1.i = 
			e[i__4].r * wtemp.i + e[i__4].i * wtemp.r;
		work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L70: */
	    }
	} else if (itrnse == 1) {
	    i__2 = *n;
	    for (jrow = 1; jrow <= i__2; ++jrow) {
		i__3 = joff + jrow;
		i__4 = jcol + jrow * e_dim1;
		z__1.r = e[i__4].r * wtemp.r - e[i__4].i * wtemp.i, z__1.i = 
			e[i__4].r * wtemp.i + e[i__4].i * wtemp.r;
		work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L80: */
	    }
	} else {
	    i__2 = *n;
	    for (jrow = 1; jrow <= i__2; ++jrow) {
		i__3 = joff + jrow;
		d_cnjg(&z__2, &e[jcol + jrow * e_dim1]);
		z__1.r = z__2.r * wtemp.r - z__2.i * wtemp.i, z__1.i = z__2.r 
			* wtemp.i + z__2.i * wtemp.r;
		work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L90: */
	    }
	}
	joff += *n;
/* L100: */
    }

    z__1.r = -1., z__1.i = -0.;
    zgemm_(transa, transe, n, n, n, &c_b2, &a[a_offset], lda, &e[e_offset], 
	    lde, &z__1, &work[1], n);

    errnrm = zlange_("One", n, n, &work[1], n, &rwork[1]) / enorm;

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

/*     End of ZGET22 */

} /* zget22_ */
