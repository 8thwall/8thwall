#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__5 = 5;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__4 = 4;
static real c_b92 = 2.f;
static integer c_n1 = -1;

/* Subroutine */ int clattr_(integer *imat, char *uplo, char *trans, char *
	diag, integer *iseed, integer *n, complex *a, integer *lda, complex *
	b, complex *work, real *rwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2;
    doublereal d__1, d__2;
    complex q__1, q__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    void c_div(complex *, complex *, complex *);
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal);
    void r_cnjg(complex *, complex *);
    double c_abs(complex *);

    /* Local variables */
    real c__;
    integer i__, j;
    complex s;
    real x, y, z__;
    complex ra, rb;
    integer kl, ku, iy;
    real ulp, sfac;
    integer mode;
    char path[3], dist[1];
    real unfl;
    extern /* Subroutine */ int crot_(integer *, complex *, integer *, 
	    complex *, integer *, real *, complex *);
    real rexp;
    char type__[1];
    real texp;
    complex star1, plus1, plus2;
    real bscal;
    extern logical lsame_(char *, char *);
    real tscal, anorm, bnorm, tleft;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), crotg_(complex *, complex *, real *, 
	    complex *), cswap_(integer *, complex *, integer *, complex *, 
	    integer *);
    logical upper;
    extern /* Subroutine */ int clatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), slabad_(real *, real *);
    extern integer icamax_(integer *, complex *, integer *);
    extern /* Complex */ VOID clarnd_(complex *, integer *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *);
    real bignum;
    extern doublereal slarnd_(integer *, integer *);
    real cndnum;
    extern /* Subroutine */ int clarnv_(integer *, integer *, integer *, 
	    complex *), clatms_(integer *, integer *, char *, integer *, char 
	    *, real *, integer *, real *, real *, integer *, integer *, char *
, complex *, integer *, complex *, integer *);
    integer jcount;
    extern /* Subroutine */ int slarnv_(integer *, integer *, integer *, real 
	    *);
    real smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLATTR generates a triangular test matrix in 2-dimensional storage. */
/*  IMAT and UPLO uniquely specify the properties of the test matrix, */
/*  which is returned in the array A. */

/*  Arguments */
/*  ========= */

/*  IMAT    (input) INTEGER */
/*          An integer key describing which matrix to generate for this */
/*          path. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A will be upper or lower */
/*          triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies whether the matrix or its transpose will be used. */
/*          = 'N':  No transpose */
/*          = 'T':  Transpose */
/*          = 'C':  Conjugate transpose */

/*  DIAG    (output) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          The seed vector for the random number generator (used in */
/*          CLATMS).  Modified on exit. */

/*  N       (input) INTEGER */
/*          The order of the matrix to be generated. */

/*  A       (output) COMPLEX array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading N x N */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading N x N lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix and the strictly upper triangular part of A is not */
/*          referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (output) COMPLEX array, dimension (N) */
/*          The right hand side vector, if IMAT > 10. */

/*  WORK    (workspace) COMPLEX array, dimension (2*N) */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

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
    --iseed;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --b;
    --work;
    --rwork;

    /* Function Body */
    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "TR", (ftnlen)2, (ftnlen)2);
    unfl = slamch_("Safe minimum");
    ulp = slamch_("Epsilon") * slamch_("Base");
    smlnum = unfl;
    bignum = (1.f - ulp) / smlnum;
    slabad_(&smlnum, &bignum);
    if (*imat >= 7 && *imat <= 10 || *imat == 18) {
	*(unsigned char *)diag = 'U';
    } else {
	*(unsigned char *)diag = 'N';
    }
    *info = 0;

/*     Quick return if N.LE.0. */

    if (*n <= 0) {
	return 0;
    }

/*     Call CLATB4 to set parameters for CLATMS. */

    upper = lsame_(uplo, "U");
    if (upper) {
	clatb4_(path, imat, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
    } else {
	i__1 = -(*imat);
	clatb4_(path, &i__1, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
    }

/*     IMAT <= 6:  Non-unit triangular matrix */

    if (*imat <= 6) {
	clatms_(n, n, dist, &iseed[1], type__, &rwork[1], &mode, &cndnum, &
		anorm, &kl, &ku, "No packing", &a[a_offset], lda, &work[1], 
		info);

/*     IMAT > 6:  Unit triangular matrix */
/*     The diagonal is deliberately set to something other than 1. */

/*     IMAT = 7:  Matrix is the identity */

    } else if (*imat == 7) {
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L10: */
		}
		i__2 = j + j * a_dim1;
		a[i__2].r = (real) j, a[i__2].i = 0.f;
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + j * a_dim1;
		a[i__2].r = (real) j, a[i__2].i = 0.f;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L30: */
		}
/* L40: */
	    }
	}

/*     IMAT > 7:  Non-trivial unit triangular matrix */

/*     Generate a unit triangular matrix T with condition CNDNUM by */
/*     forming a triangular matrix with known singular values and */
/*     filling in the zero entries with Givens rotations. */

    } else if (*imat <= 10) {
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L50: */
		}
		i__2 = j + j * a_dim1;
		a[i__2].r = (real) j, a[i__2].i = 0.f;
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + j * a_dim1;
		a[i__2].r = (real) j, a[i__2].i = 0.f;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L70: */
		}
/* L80: */
	    }
	}

/*        Since the trace of a unit triangular matrix is 1, the product */
/*        of its singular values must be 1.  Let s = sqrt(CNDNUM), */
/*        x = sqrt(s) - 1/sqrt(s), y = sqrt(2/(n-2))*x, and z = x**2. */
/*        The following triangular matrix has singular values s, 1, 1, */
/*        ..., 1, 1/s: */

/*        1  y  y  y  ...  y  y  z */
/*           1  0  0  ...  0  0  y */
/*              1  0  ...  0  0  y */
/*                 .  ...  .  .  . */
/*                     .   .  .  . */
/*                         1  0  y */
/*                            1  y */
/*                               1 */

/*        To fill in the zeros, we first multiply by a matrix with small */
/*        condition number of the form */

/*        1  0  0  0  0  ... */
/*           1  +  *  0  0  ... */
/*              1  +  0  0  0 */
/*                 1  +  *  0  0 */
/*                    1  +  0  0 */
/*                       ... */
/*                          1  +  0 */
/*                             1  0 */
/*                                1 */

/*        Each element marked with a '*' is formed by taking the product */
/*        of the adjacent elements marked with '+'.  The '*'s can be */
/*        chosen freely, and the '+'s are chosen so that the inverse of */
/*        T will have elements of the same magnitude as T.  If the *'s in */
/*        both T and inv(T) have small magnitude, T is well conditioned. */
/*        The two offdiagonals of T are stored in WORK. */

/*        The product of these two matrices has the form */

/*        1  y  y  y  y  y  .  y  y  z */
/*           1  +  *  0  0  .  0  0  y */
/*              1  +  0  0  .  0  0  y */
/*                 1  +  *  .  .  .  . */
/*                    1  +  .  .  .  . */
/*                       .  .  .  .  . */
/*                          .  .  .  . */
/*                             1  +  y */
/*                                1  y */
/*                                   1 */

/*        Now we multiply by Givens rotations, using the fact that */

/*              [  c   s ] [  1   w ] [ -c  -s ] =  [  1  -w ] */
/*              [ -s   c ] [  0   1 ] [  s  -c ]    [  0   1 ] */
/*        and */
/*              [ -c  -s ] [  1   0 ] [  c   s ] =  [  1   0 ] */
/*              [  s  -c ] [  w   1 ] [ -s   c ]    [ -w   1 ] */

/*        where c = w / sqrt(w**2+4) and s = 2 / sqrt(w**2+4). */

	clarnd_(&q__2, &c__5, &iseed[1]);
	q__1.r = q__2.r * .25f, q__1.i = q__2.i * .25f;
	star1.r = q__1.r, star1.i = q__1.i;
	sfac = .5f;
	clarnd_(&q__2, &c__5, &iseed[1]);
	q__1.r = sfac * q__2.r, q__1.i = sfac * q__2.i;
	plus1.r = q__1.r, plus1.i = q__1.i;
	i__1 = *n;
	for (j = 1; j <= i__1; j += 2) {
	    c_div(&q__1, &star1, &plus1);
	    plus2.r = q__1.r, plus2.i = q__1.i;
	    i__2 = j;
	    work[i__2].r = plus1.r, work[i__2].i = plus1.i;
	    i__2 = *n + j;
	    work[i__2].r = star1.r, work[i__2].i = star1.i;
	    if (j + 1 <= *n) {
		i__2 = j + 1;
		work[i__2].r = plus2.r, work[i__2].i = plus2.i;
		i__2 = *n + j + 1;
		work[i__2].r = 0.f, work[i__2].i = 0.f;
		c_div(&q__1, &star1, &plus2);
		plus1.r = q__1.r, plus1.i = q__1.i;
		rexp = slarnd_(&c__2, &iseed[1]);
		if (rexp < 0.f) {
		    d__1 = (doublereal) sfac;
		    d__2 = (doublereal) (1.f - rexp);
		    r__1 = -pow_dd(&d__1, &d__2);
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = r__1 * q__2.r, q__1.i = r__1 * q__2.i;
		    star1.r = q__1.r, star1.i = q__1.i;
		} else {
		    d__1 = (doublereal) sfac;
		    d__2 = (doublereal) (rexp + 1.f);
		    r__1 = pow_dd(&d__1, &d__2);
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = r__1 * q__2.r, q__1.i = r__1 * q__2.i;
		    star1.r = q__1.r, star1.i = q__1.i;
		}
	    }
/* L90: */
	}

	x = sqrt(cndnum) - 1 / sqrt(cndnum);
	if (*n > 2) {
	    y = sqrt(2.f / (*n - 2)) * x;
	} else {
	    y = 0.f;
	}
	z__ = x * x;

	if (upper) {
	    if (*n > 3) {
		i__1 = *n - 3;
		i__2 = *lda + 1;
		ccopy_(&i__1, &work[1], &c__1, &a[a_dim1 * 3 + 2], &i__2);
		if (*n > 4) {
		    i__1 = *n - 4;
		    i__2 = *lda + 1;
		    ccopy_(&i__1, &work[*n + 1], &c__1, &a[(a_dim1 << 2) + 2], 
			     &i__2);
		}
	    }
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j * a_dim1 + 1;
		a[i__2].r = y, a[i__2].i = 0.f;
		i__2 = j + *n * a_dim1;
		a[i__2].r = y, a[i__2].i = 0.f;
/* L100: */
	    }
	    i__1 = *n * a_dim1 + 1;
	    a[i__1].r = z__, a[i__1].i = 0.f;
	} else {
	    if (*n > 3) {
		i__1 = *n - 3;
		i__2 = *lda + 1;
		ccopy_(&i__1, &work[1], &c__1, &a[(a_dim1 << 1) + 3], &i__2);
		if (*n > 4) {
		    i__1 = *n - 4;
		    i__2 = *lda + 1;
		    ccopy_(&i__1, &work[*n + 1], &c__1, &a[(a_dim1 << 1) + 4], 
			     &i__2);
		}
	    }
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j + a_dim1;
		a[i__2].r = y, a[i__2].i = 0.f;
		i__2 = *n + j * a_dim1;
		a[i__2].r = y, a[i__2].i = 0.f;
/* L110: */
	    }
	    i__1 = *n + a_dim1;
	    a[i__1].r = z__, a[i__1].i = 0.f;
	}

/*        Fill in the zeros using Givens rotations. */

	if (upper) {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + (j + 1) * a_dim1;
		ra.r = a[i__2].r, ra.i = a[i__2].i;
		rb.r = 2.f, rb.i = 0.f;
		crotg_(&ra, &rb, &c__, &s);

/*              Multiply by [ c  s; -conjg(s)  c] on the left. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    crot_(&i__2, &a[j + (j + 2) * a_dim1], lda, &a[j + 1 + (j 
			    + 2) * a_dim1], lda, &c__, &s);
		}

/*              Multiply by [-c -s;  conjg(s) -c] on the right. */

		if (j > 1) {
		    i__2 = j - 1;
		    r__1 = -c__;
		    q__1.r = -s.r, q__1.i = -s.i;
		    crot_(&i__2, &a[(j + 1) * a_dim1 + 1], &c__1, &a[j * 
			    a_dim1 + 1], &c__1, &r__1, &q__1);
		}

/*              Negate A(J,J+1). */

		i__2 = j + (j + 1) * a_dim1;
		i__3 = j + (j + 1) * a_dim1;
		q__1.r = -a[i__3].r, q__1.i = -a[i__3].i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L120: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + 1 + j * a_dim1;
		ra.r = a[i__2].r, ra.i = a[i__2].i;
		rb.r = 2.f, rb.i = 0.f;
		crotg_(&ra, &rb, &c__, &s);
		r_cnjg(&q__1, &s);
		s.r = q__1.r, s.i = q__1.i;

/*              Multiply by [ c -s;  conjg(s) c] on the right. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    q__1.r = -s.r, q__1.i = -s.i;
		    crot_(&i__2, &a[j + 2 + (j + 1) * a_dim1], &c__1, &a[j + 
			    2 + j * a_dim1], &c__1, &c__, &q__1);
		}

/*              Multiply by [-c  s; -conjg(s) -c] on the left. */

		if (j > 1) {
		    i__2 = j - 1;
		    r__1 = -c__;
		    crot_(&i__2, &a[j + a_dim1], lda, &a[j + 1 + a_dim1], lda, 
			     &r__1, &s);
		}

/*              Negate A(J+1,J). */

		i__2 = j + 1 + j * a_dim1;
		i__3 = j + 1 + j * a_dim1;
		q__1.r = -a[i__3].r, q__1.i = -a[i__3].i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L130: */
	    }
	}

/*     IMAT > 10:  Pathological test cases.  These triangular matrices */
/*     are badly scaled or badly conditioned, so when used in solving a */
/*     triangular system they may cause overflow in the solution vector. */

    } else if (*imat == 11) {

/*        Type 11:  Generate a triangular matrix with elements between */
/*        -1 and 1. Give the diagonal norm 2 to make it well-conditioned. */
/*        Make the right hand side large so that it requires scaling. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		clarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j + j * a_dim1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L140: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    clarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		i__2 = j + j * a_dim1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L150: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = icamax_(n, &b[1], &c__1);
	bnorm = c_abs(&b[iy]);
	bscal = bignum / dmax(1.f,bnorm);
	csscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 12) {

/*        Type 12:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 12, the offdiagonal elements are small (CNORM(j) < 1). */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
/* Computing MAX */
	r__1 = 1.f, r__2 = (real) (*n - 1);
	tscal = 1.f / dmax(r__1,r__2);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		clarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j - 1;
		csscal_(&i__2, &tscal, &a[j * a_dim1 + 1], &c__1);
		i__2 = j + j * a_dim1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L160: */
	    }
	    i__1 = *n + *n * a_dim1;
	    i__2 = *n + *n * a_dim1;
	    q__1.r = smlnum * a[i__2].r, q__1.i = smlnum * a[i__2].i;
	    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    clarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		    i__2 = *n - j;
		    csscal_(&i__2, &tscal, &a[j + 1 + j * a_dim1], &c__1);
		}
		i__2 = j + j * a_dim1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L170: */
	    }
	    i__1 = a_dim1 + 1;
	    i__2 = a_dim1 + 1;
	    q__1.r = smlnum * a[i__2].r, q__1.i = smlnum * a[i__2].i;
	    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	}

    } else if (*imat == 13) {

/*        Type 13:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 13, the offdiagonal elements are O(1) (CNORM(j) > 1). */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		clarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j + j * a_dim1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L180: */
	    }
	    i__1 = *n + *n * a_dim1;
	    i__2 = *n + *n * a_dim1;
	    q__1.r = smlnum * a[i__2].r, q__1.i = smlnum * a[i__2].i;
	    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    clarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		i__2 = j + j * a_dim1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L190: */
	    }
	    i__1 = a_dim1 + 1;
	    i__2 = a_dim1 + 1;
	    q__1.r = smlnum * a[i__2].r, q__1.i = smlnum * a[i__2].i;
	    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	}

    } else if (*imat == 14) {

/*        Type 14:  T is diagonal with small numbers on the diagonal to */
/*        make the growth factor underflow, but a small right hand side */
/*        chosen so that the solution does not overflow. */

	if (upper) {
	    jcount = 1;
	    for (j = *n; j >= 1; --j) {
		i__1 = j - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    i__2 = i__ + j * a_dim1;
		    a[i__2].r = 0.f, a[i__2].i = 0.f;
/* L200: */
		}
		if (jcount <= 2) {
		    i__1 = j + j * a_dim1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
		} else {
		    i__1 = j + j * a_dim1;
		    clarnd_(&q__1, &c__5, &iseed[1]);
		    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
		}
		++jcount;
		if (jcount > 4) {
		    jcount = 1;
		}
/* L210: */
	    }
	} else {
	    jcount = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L220: */
		}
		if (jcount <= 2) {
		    i__2 = j + j * a_dim1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		    a[i__2].r = q__1.r, a[i__2].i = q__1.i;
		} else {
		    i__2 = j + j * a_dim1;
		    clarnd_(&q__1, &c__5, &iseed[1]);
		    a[i__2].r = q__1.r, a[i__2].i = q__1.i;
		}
		++jcount;
		if (jcount > 4) {
		    jcount = 1;
		}
/* L230: */
	    }
	}

/*        Set the right hand side alternately zero and small. */

	if (upper) {
	    b[1].r = 0.f, b[1].i = 0.f;
	    for (i__ = *n; i__ >= 2; i__ += -2) {
		i__1 = i__;
		b[i__1].r = 0.f, b[i__1].i = 0.f;
		i__1 = i__ - 1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		b[i__1].r = q__1.r, b[i__1].i = q__1.i;
/* L240: */
	    }
	} else {
	    i__1 = *n;
	    b[i__1].r = 0.f, b[i__1].i = 0.f;
	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; i__ += 2) {
		i__2 = i__;
		b[i__2].r = 0.f, b[i__2].i = 0.f;
		i__2 = i__ + 1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		b[i__2].r = q__1.r, b[i__2].i = q__1.i;
/* L250: */
	    }
	}

    } else if (*imat == 15) {

/*        Type 15:  Make the diagonal elements small to cause gradual */
/*        overflow when dividing by T(j,j).  To control the amount of */
/*        scaling needed, the matrix is bidiagonal. */

/* Computing MAX */
	r__1 = 1.f, r__2 = (real) (*n - 1);
	texp = 1.f / dmax(r__1,r__2);
	d__1 = (doublereal) smlnum;
	d__2 = (doublereal) texp;
	tscal = pow_dd(&d__1, &d__2);
	clarnv_(&c__4, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 2;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L260: */
		}
		if (j > 1) {
		    i__2 = j - 1 + j * a_dim1;
		    a[i__2].r = -1.f, a[i__2].i = -1.f;
		}
		i__2 = j + j * a_dim1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = tscal * q__2.r, q__1.i = tscal * q__2.i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L270: */
	    }
	    i__1 = *n;
	    b[i__1].r = 1.f, b[i__1].i = 1.f;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 2; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L280: */
		}
		if (j < *n) {
		    i__2 = j + 1 + j * a_dim1;
		    a[i__2].r = -1.f, a[i__2].i = -1.f;
		}
		i__2 = j + j * a_dim1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = tscal * q__2.r, q__1.i = tscal * q__2.i;
		a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* L290: */
	    }
	    b[1].r = 1.f, b[1].i = 1.f;
	}

    } else if (*imat == 16) {

/*        Type 16:  One zero diagonal element. */

	iy = *n / 2 + 1;
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		clarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		if (j != iy) {
		    i__2 = j + j * a_dim1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		    a[i__2].r = q__1.r, a[i__2].i = q__1.i;
		} else {
		    i__2 = j + j * a_dim1;
		    a[i__2].r = 0.f, a[i__2].i = 0.f;
		}
/* L300: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    clarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		if (j != iy) {
		    i__2 = j + j * a_dim1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		    a[i__2].r = q__1.r, a[i__2].i = q__1.i;
		} else {
		    i__2 = j + j * a_dim1;
		    a[i__2].r = 0.f, a[i__2].i = 0.f;
		}
/* L310: */
	    }
	}
	clarnv_(&c__2, &iseed[1], n, &b[1]);
	csscal_(n, &c_b92, &b[1], &c__1);

    } else if (*imat == 17) {

/*        Type 17:  Make the offdiagonal elements large to cause overflow */
/*        when adding a column of T.  In the non-transposed case, the */
/*        matrix is constructed to cause overflow when adding a column in */
/*        every other step. */

	tscal = unfl / ulp;
	tscal = (1.f - ulp) / tscal;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * a_dim1;
		a[i__3].r = 0.f, a[i__3].i = 0.f;
/* L320: */
	    }
/* L330: */
	}
	texp = 1.f;
	if (upper) {
	    for (j = *n; j >= 2; j += -2) {
		i__1 = j * a_dim1 + 1;
		r__1 = -tscal / (real) (*n + 1);
		a[i__1].r = r__1, a[i__1].i = 0.f;
		i__1 = j + j * a_dim1;
		a[i__1].r = 1.f, a[i__1].i = 0.f;
		i__1 = j;
		r__1 = texp * (1.f - ulp);
		b[i__1].r = r__1, b[i__1].i = 0.f;
		i__1 = (j - 1) * a_dim1 + 1;
		r__1 = -(tscal / (real) (*n + 1)) / (real) (*n + 2);
		a[i__1].r = r__1, a[i__1].i = 0.f;
		i__1 = j - 1 + (j - 1) * a_dim1;
		a[i__1].r = 1.f, a[i__1].i = 0.f;
		i__1 = j - 1;
		r__1 = texp * (real) (*n * *n + *n - 1);
		b[i__1].r = r__1, b[i__1].i = 0.f;
		texp *= 2.f;
/* L340: */
	    }
	    r__1 = (real) (*n + 1) / (real) (*n + 2) * tscal;
	    b[1].r = r__1, b[1].i = 0.f;
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; j += 2) {
		i__2 = *n + j * a_dim1;
		r__1 = -tscal / (real) (*n + 1);
		a[i__2].r = r__1, a[i__2].i = 0.f;
		i__2 = j + j * a_dim1;
		a[i__2].r = 1.f, a[i__2].i = 0.f;
		i__2 = j;
		r__1 = texp * (1.f - ulp);
		b[i__2].r = r__1, b[i__2].i = 0.f;
		i__2 = *n + (j + 1) * a_dim1;
		r__1 = -(tscal / (real) (*n + 1)) / (real) (*n + 2);
		a[i__2].r = r__1, a[i__2].i = 0.f;
		i__2 = j + 1 + (j + 1) * a_dim1;
		a[i__2].r = 1.f, a[i__2].i = 0.f;
		i__2 = j + 1;
		r__1 = texp * (real) (*n * *n + *n - 1);
		b[i__2].r = r__1, b[i__2].i = 0.f;
		texp *= 2.f;
/* L350: */
	    }
	    i__1 = *n;
	    r__1 = (real) (*n + 1) / (real) (*n + 2) * tscal;
	    b[i__1].r = r__1, b[i__1].i = 0.f;
	}

    } else if (*imat == 18) {

/*        Type 18:  Generate a unit triangular matrix with elements */
/*        between -1 and 1, and make the right hand side large so that it */
/*        requires scaling. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		clarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j + j * a_dim1;
		a[i__2].r = 0.f, a[i__2].i = 0.f;
/* L360: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    clarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		i__2 = j + j * a_dim1;
		a[i__2].r = 0.f, a[i__2].i = 0.f;
/* L370: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = icamax_(n, &b[1], &c__1);
	bnorm = c_abs(&b[iy]);
	bscal = bignum / dmax(1.f,bnorm);
	csscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 19) {

/*        Type 19:  Generate a triangular matrix with elements between */
/*        BIGNUM/(n-1) and BIGNUM so that at least one of the column */
/*        norms will exceed BIGNUM. */
/*        1/3/91:  CLATRS no longer can handle this case */

/* Computing MAX */
	r__1 = 1.f, r__2 = (real) (*n - 1);
	tleft = bignum / dmax(r__1,r__2);
/* Computing MAX */
	r__1 = 1.f, r__2 = (real) (*n);
	tscal = bignum * ((real) (*n - 1) / dmax(r__1,r__2));
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		clarnv_(&c__5, &iseed[1], &j, &a[j * a_dim1 + 1]);
		slarnv_(&c__1, &iseed[1], &j, &rwork[1]);
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    i__4 = i__ + j * a_dim1;
		    r__1 = tleft + rwork[i__] * tscal;
		    q__1.r = r__1 * a[i__4].r, q__1.i = r__1 * a[i__4].i;
		    a[i__3].r = q__1.r, a[i__3].i = q__1.i;
/* L380: */
		}
/* L390: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		clarnv_(&c__5, &iseed[1], &i__2, &a[j + j * a_dim1]);
		i__2 = *n - j + 1;
		slarnv_(&c__1, &iseed[1], &i__2, &rwork[1]);
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    i__4 = i__ + j * a_dim1;
		    r__1 = tleft + rwork[i__ - j + 1] * tscal;
		    q__1.r = r__1 * a[i__4].r, q__1.i = r__1 * a[i__4].i;
		    a[i__3].r = q__1.r, a[i__3].i = q__1.i;
/* L400: */
		}
/* L410: */
	    }
	}
	clarnv_(&c__2, &iseed[1], n, &b[1]);
	csscal_(n, &c_b92, &b[1], &c__1);
    }

/*     Flip the matrix if the transpose will be used. */

    if (! lsame_(trans, "N")) {
	if (upper) {
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - (j << 1) + 1;
		cswap_(&i__2, &a[j + j * a_dim1], lda, &a[j + 1 + (*n - j + 1)
			 * a_dim1], &c_n1);
/* L420: */
	    }
	} else {
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - (j << 1) + 1;
		i__3 = -(*lda);
		cswap_(&i__2, &a[j + j * a_dim1], &c__1, &a[*n - j + 1 + (j + 
			1) * a_dim1], &i__3);
/* L430: */
	    }
	}
    }

    return 0;

/*     End of CLATTR */

} /* clattr_ */
