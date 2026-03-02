#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};

/* Subroutine */ int cget22_(char *transa, char *transe, char *transw, 
	integer *n, complex *a, integer *lda, complex *e, integer *lde, 
	complex *w, complex *work, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, e_dim1, e_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2, r__3, r__4;
    complex q__1, q__2;

    /* Builtin functions */
    double r_imag(complex *);
    void r_cnjg(complex *, complex *);

    /* Local variables */
    integer j;
    real ulp;
    integer joff, jcol, jvec;
    real unfl;
    integer jrow;
    real temp1;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    extern logical lsame_(char *, char *);
    char norma[1];
    real anorm;
    char norme[1];
    real enorm;
    complex wtemp;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);
    real enrmin, enrmax;
    integer itrnse;
    real errnrm;
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

/*  CGET22 does an eigenvector check. */

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

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The matrix whose eigenvectors are in E. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  E       (input) COMPLEX array, dimension (LDE,N) */
/*          The matrix of eigenvectors. If TRANSE = 'N', the eigenvectors */
/*          are stored in the columns of E, if TRANSE = 'T' or 'C', the */
/*          eigenvectors are stored in the rows of E. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of the array E.  LDE >= max(1,N). */

/*  W       (input) COMPLEX array, dimension (N) */
/*          The eigenvalues of A. */

/*  WORK    (workspace) COMPLEX array, dimension (N*N) */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESULT  (output) REAL array, dimension (2) */
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
    result[1] = 0.f;
    result[2] = 0.f;
    if (*n <= 0) {
	return 0;
    }

    unfl = slamch_("Safe minimum");
    ulp = slamch_("Precision");

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

    enrmin = 1.f / ulp;
    enrmax = 0.f;
    if (itrnse == 0) {
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
/* L10: */
	    }
	    enrmin = dmin(enrmin,temp1);
	    enrmax = dmax(enrmax,temp1);
/* L20: */
	}
    } else {
	i__1 = *n;
	for (jvec = 1; jvec <= i__1; ++jvec) {
	    rwork[jvec] = 0.f;
/* L30: */
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (jvec = 1; jvec <= i__2; ++jvec) {
/* Computing MAX */
		i__3 = jvec + j * e_dim1;
		r__3 = rwork[jvec], r__4 = (r__1 = e[i__3].r, dabs(r__1)) + (
			r__2 = r_imag(&e[jvec + j * e_dim1]), dabs(r__2));
		rwork[jvec] = dmax(r__3,r__4);
/* L40: */
	    }
/* L50: */
	}

	i__1 = *n;
	for (jvec = 1; jvec <= i__1; ++jvec) {
/* Computing MIN */
	    r__1 = enrmin, r__2 = rwork[jvec];
	    enrmin = dmin(r__1,r__2);
/* Computing MAX */
	    r__1 = enrmax, r__2 = rwork[jvec];
	    enrmax = dmax(r__1,r__2);
/* L60: */
	}
    }

/*     Norm of A: */

/* Computing MAX */
    r__1 = clange_(norma, n, n, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);

/*     Norm of E: */

/* Computing MAX */
    r__1 = clange_(norme, n, n, &e[e_offset], lde, &rwork[1]);
    enorm = dmax(r__1,ulp);

/*     Norm of error: */

/*     Error =  AE - EW */

    claset_("Full", n, n, &c_b1, &c_b1, &work[1], n);

    joff = 0;
    i__1 = *n;
    for (jcol = 1; jcol <= i__1; ++jcol) {
	if (itrnsw == 0) {
	    i__2 = jcol;
	    wtemp.r = w[i__2].r, wtemp.i = w[i__2].i;
	} else {
	    r_cnjg(&q__1, &w[jcol]);
	    wtemp.r = q__1.r, wtemp.i = q__1.i;
	}

	if (itrnse == 0) {
	    i__2 = *n;
	    for (jrow = 1; jrow <= i__2; ++jrow) {
		i__3 = joff + jrow;
		i__4 = jrow + jcol * e_dim1;
		q__1.r = e[i__4].r * wtemp.r - e[i__4].i * wtemp.i, q__1.i = 
			e[i__4].r * wtemp.i + e[i__4].i * wtemp.r;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L70: */
	    }
	} else if (itrnse == 1) {
	    i__2 = *n;
	    for (jrow = 1; jrow <= i__2; ++jrow) {
		i__3 = joff + jrow;
		i__4 = jcol + jrow * e_dim1;
		q__1.r = e[i__4].r * wtemp.r - e[i__4].i * wtemp.i, q__1.i = 
			e[i__4].r * wtemp.i + e[i__4].i * wtemp.r;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L80: */
	    }
	} else {
	    i__2 = *n;
	    for (jrow = 1; jrow <= i__2; ++jrow) {
		i__3 = joff + jrow;
		r_cnjg(&q__2, &e[jcol + jrow * e_dim1]);
		q__1.r = q__2.r * wtemp.r - q__2.i * wtemp.i, q__1.i = q__2.r 
			* wtemp.i + q__2.i * wtemp.r;
		work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L90: */
	    }
	}
	joff += *n;
/* L100: */
    }

    q__1.r = -1.f, q__1.i = -0.f;
    cgemm_(transa, transe, n, n, n, &c_b2, &a[a_offset], lda, &e[e_offset], 
	    lde, &q__1, &work[1], n);

    errnrm = clange_("One", n, n, &work[1], n, &rwork[1]) / enorm;

/*     Compute RESULT(1) (avoiding under/overflow) */

    if (anorm > errnrm) {
	result[1] = errnrm / anorm / ulp;
    } else {
	if (anorm < 1.f) {
	    result[1] = dmin(errnrm,anorm) / anorm / ulp;
	} else {
/* Computing MIN */
	    r__1 = errnrm / anorm;
	    result[1] = dmin(r__1,1.f) / ulp;
	}
    }

/*     Compute RESULT(2) : the normalization error in E. */

/* Computing MAX */
    r__3 = (r__1 = enrmax - 1.f, dabs(r__1)), r__4 = (r__2 = enrmin - 1.f, 
	    dabs(r__2));
    result[2] = dmax(r__3,r__4) / ((real) (*n) * ulp);

    return 0;

/*     End of CGET22 */

} /* cget22_ */
