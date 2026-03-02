#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__5 = 5;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__4 = 4;
static real c_b91 = 2.f;
static integer c_n1 = -1;

/* Subroutine */ int clattb_(integer *imat, char *uplo, char *trans, char *
	diag, integer *iseed, integer *n, integer *kd, complex *ab, integer *
	ldab, complex *b, complex *work, real *rwork, integer *info)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4, i__5;
    real r__1;
    doublereal d__1, d__2;
    complex q__1, q__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    void c_div(complex *, complex *, complex *);
    double pow_dd(doublereal *, doublereal *), c_abs(complex *);

    /* Local variables */
    integer i__, j, kl, ku, iy;
    real ulp, sfac;
    integer ioff, mode, lenj;
    char path[3], dist[1];
    real unfl, rexp;
    char type__[1];
    real texp;
    complex star1, plus1, plus2;
    real bscal;
    extern logical lsame_(char *, char *);
    real tscal, anorm, bnorm, tleft;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), cswap_(integer *, complex *, integer *, 
	    complex *, integer *);
    logical upper;
    real tnorm;
    extern /* Subroutine */ int clatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), slabad_(real *, real *);
    extern integer icamax_(integer *, complex *, integer *);
    extern /* Complex */ VOID clarnd_(complex *, integer *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *);
    char packit[1];
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

/*  CLATTB generates a triangular test matrix in 2-dimensional storage. */
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
/*          = 'C':  Conjugate transpose (= transpose) */

/*  DIAG    (output) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          The seed vector for the random number generator (used in */
/*          CLATMS).  Modified on exit. */

/*  N       (input) INTEGER */
/*          The order of the matrix to be generated. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals or subdiagonals of the banded */
/*          triangular matrix A.  KD >= 0. */

/*  AB      (output) COMPLEX array, dimension (LDAB,N) */
/*          The upper or lower triangular banded matrix A, stored in the */
/*          first KD+1 rows of AB.  Let j be a column of A, 1<=j<=n. */
/*          If UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j. */
/*          If UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  B       (workspace) COMPLEX array, dimension (N) */

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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --b;
    --work;
    --rwork;

    /* Function Body */
    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "TB", (ftnlen)2, (ftnlen)2);
    unfl = slamch_("Safe minimum");
    ulp = slamch_("Epsilon") * slamch_("Base");
    smlnum = unfl;
    bignum = (1.f - ulp) / smlnum;
    slabad_(&smlnum, &bignum);
    if (*imat >= 6 && *imat <= 9 || *imat == 17) {
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
	ku = *kd;
/* Computing MAX */
	i__1 = 0, i__2 = *kd - *n + 1;
	ioff = max(i__1,i__2) + 1;
	kl = 0;
	*(unsigned char *)packit = 'Q';
    } else {
	i__1 = -(*imat);
	clatb4_(path, &i__1, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
	kl = *kd;
	ioff = 1;
	ku = 0;
	*(unsigned char *)packit = 'B';
    }

/*     IMAT <= 5:  Non-unit triangular matrix */

    if (*imat <= 5) {
	clatms_(n, n, dist, &iseed[1], type__, &rwork[1], &mode, &cndnum, &
		anorm, &kl, &ku, packit, &ab[ioff + ab_dim1], ldab, &work[1], 
		info);

/*     IMAT > 5:  Unit triangular matrix */
/*     The diagonal is deliberately set to something other than 1. */

/*     IMAT = 6:  Matrix is the identity */

    } else if (*imat == 6) {
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		i__2 = 1, i__3 = *kd + 2 - j;
		i__4 = *kd;
		for (i__ = max(i__2,i__3); i__ <= i__4; ++i__) {
		    i__2 = i__ + j * ab_dim1;
		    ab[i__2].r = 0.f, ab[i__2].i = 0.f;
/* L10: */
		}
		i__4 = *kd + 1 + j * ab_dim1;
		ab[i__4].r = (real) j, ab[i__4].i = 0.f;
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__4 = j * ab_dim1 + 1;
		ab[i__4].r = (real) j, ab[i__4].i = 0.f;
/* Computing MIN */
		i__2 = *kd + 1, i__3 = *n - j + 1;
		i__4 = min(i__2,i__3);
		for (i__ = 2; i__ <= i__4; ++i__) {
		    i__2 = i__ + j * ab_dim1;
		    ab[i__2].r = 0.f, ab[i__2].i = 0.f;
/* L30: */
		}
/* L40: */
	    }
	}

/*     IMAT > 6:  Non-trivial unit triangular matrix */

/*     A unit triangular matrix T with condition CNDNUM is formed. */
/*     In this version, T only has bandwidth 2, the rest of it is zero. */

    } else if (*imat <= 9) {
	tnorm = sqrt(cndnum);

/*        Initialize AB to zero. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		i__4 = 1, i__2 = *kd + 2 - j;
		i__3 = *kd;
		for (i__ = max(i__4,i__2); i__ <= i__3; ++i__) {
		    i__4 = i__ + j * ab_dim1;
		    ab[i__4].r = 0.f, ab[i__4].i = 0.f;
/* L50: */
		}
		i__3 = *kd + 1 + j * ab_dim1;
		r__1 = (real) j;
		ab[i__3].r = r__1, ab[i__3].i = 0.f;
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__4 = *kd + 1, i__2 = *n - j + 1;
		i__3 = min(i__4,i__2);
		for (i__ = 2; i__ <= i__3; ++i__) {
		    i__4 = i__ + j * ab_dim1;
		    ab[i__4].r = 0.f, ab[i__4].i = 0.f;
/* L70: */
		}
		i__3 = j * ab_dim1 + 1;
		r__1 = (real) j;
		ab[i__3].r = r__1, ab[i__3].i = 0.f;
/* L80: */
	    }
	}

/*        Special case:  T is tridiagonal.  Set every other offdiagonal */
/*        so that the matrix has norm TNORM+1. */

	if (*kd == 1) {
	    if (upper) {
		i__1 = (ab_dim1 << 1) + 1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = tnorm * q__2.r, q__1.i = tnorm * q__2.i;
		ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
		lenj = (*n - 3) / 2;
		clarnv_(&c__2, &iseed[1], &lenj, &work[1]);
		i__1 = lenj;
		for (j = 1; j <= i__1; ++j) {
		    i__3 = (j + 1 << 1) * ab_dim1 + 1;
		    i__4 = j;
		    q__1.r = tnorm * work[i__4].r, q__1.i = tnorm * work[i__4]
			    .i;
		    ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L90: */
		}
	    } else {
		i__1 = ab_dim1 + 2;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = tnorm * q__2.r, q__1.i = tnorm * q__2.i;
		ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
		lenj = (*n - 3) / 2;
		clarnv_(&c__2, &iseed[1], &lenj, &work[1]);
		i__1 = lenj;
		for (j = 1; j <= i__1; ++j) {
		    i__3 = ((j << 1) + 1) * ab_dim1 + 2;
		    i__4 = j;
		    q__1.r = tnorm * work[i__4].r, q__1.i = tnorm * work[i__4]
			    .i;
		    ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L100: */
		}
	    }
	} else if (*kd > 1) {

/*           Form a unit triangular matrix T with condition CNDNUM.  T is */
/*           given by */
/*                   | 1   +   *                      | */
/*                   |     1   +                      | */
/*               T = |         1   +   *              | */
/*                   |             1   +              | */
/*                   |                 1   +   *      | */
/*                   |                     1   +      | */
/*                   |                          . . . | */
/*        Each element marked with a '*' is formed by taking the product */
/*        of the adjacent elements marked with '+'.  The '*'s can be */
/*        chosen freely, and the '+'s are chosen so that the inverse of */
/*        T will have elements of the same magnitude as T. */

/*        The two offdiagonals of T are stored in WORK. */

	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = tnorm * q__2.r, q__1.i = tnorm * q__2.i;
	    star1.r = q__1.r, star1.i = q__1.i;
	    sfac = sqrt(tnorm);
	    clarnd_(&q__2, &c__5, &iseed[1]);
	    q__1.r = sfac * q__2.r, q__1.i = sfac * q__2.i;
	    plus1.r = q__1.r, plus1.i = q__1.i;
	    i__1 = *n;
	    for (j = 1; j <= i__1; j += 2) {
		c_div(&q__1, &star1, &plus1);
		plus2.r = q__1.r, plus2.i = q__1.i;
		i__3 = j;
		work[i__3].r = plus1.r, work[i__3].i = plus1.i;
		i__3 = *n + j;
		work[i__3].r = star1.r, work[i__3].i = star1.i;
		if (j + 1 <= *n) {
		    i__3 = j + 1;
		    work[i__3].r = plus2.r, work[i__3].i = plus2.i;
		    i__3 = *n + j + 1;
		    work[i__3].r = 0.f, work[i__3].i = 0.f;
		    c_div(&q__1, &star1, &plus2);
		    plus1.r = q__1.r, plus1.i = q__1.i;

/*                 Generate a new *-value with norm between sqrt(TNORM) */
/*                 and TNORM. */

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
/* L110: */
	    }

/*           Copy the tridiagonal T to AB. */

	    if (upper) {
		i__1 = *n - 1;
		ccopy_(&i__1, &work[1], &c__1, &ab[*kd + (ab_dim1 << 1)], 
			ldab);
		i__1 = *n - 2;
		ccopy_(&i__1, &work[*n + 1], &c__1, &ab[*kd - 1 + ab_dim1 * 3]
, ldab);
	    } else {
		i__1 = *n - 1;
		ccopy_(&i__1, &work[1], &c__1, &ab[ab_dim1 + 2], ldab);
		i__1 = *n - 2;
		ccopy_(&i__1, &work[*n + 1], &c__1, &ab[ab_dim1 + 3], ldab);
	    }
	}

/*     IMAT > 9:  Pathological test cases.  These triangular matrices */
/*     are badly scaled or badly conditioned, so when used in solving a */
/*     triangular system they may cause overflow in the solution vector. */

    } else if (*imat == 10) {

/*        Type 10:  Generate a triangular matrix with elements between */
/*        -1 and 1. Give the diagonal norm 2 to make it well-conditioned. */
/*        Make the right hand side large so that it requires scaling. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = j - 1;
		lenj = min(i__3,*kd);
		clarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 1 - lenj + j * 
			ab_dim1]);
		i__3 = *kd + 1 + j * ab_dim1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L120: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = *n - j;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    clarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		}
		i__3 = j * ab_dim1 + 1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L130: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = icamax_(n, &b[1], &c__1);
	bnorm = c_abs(&b[iy]);
	bscal = bignum / dmax(1.f,bnorm);
	csscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 11) {

/*        Type 11:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 11, the offdiagonal elements are small (CNORM(j) < 1). */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
	tscal = 1.f / (real) (*kd + 1);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = j - 1;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    clarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			    ab_dim1]);
		    csscal_(&lenj, &tscal, &ab[*kd + 2 - lenj + j * ab_dim1], 
			    &c__1);
		}
		i__3 = *kd + 1 + j * ab_dim1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L140: */
	    }
	    i__1 = *kd + 1 + *n * ab_dim1;
	    i__3 = *kd + 1 + *n * ab_dim1;
	    q__1.r = smlnum * ab[i__3].r, q__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = *n - j;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    clarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		    csscal_(&lenj, &tscal, &ab[j * ab_dim1 + 2], &c__1);
		}
		i__3 = j * ab_dim1 + 1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L150: */
	    }
	    i__1 = ab_dim1 + 1;
	    i__3 = ab_dim1 + 1;
	    q__1.r = smlnum * ab[i__3].r, q__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
	}

    } else if (*imat == 12) {

/*        Type 12:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 12, the offdiagonal elements are O(1) (CNORM(j) > 1). */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = j - 1;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    clarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			    ab_dim1]);
		}
		i__3 = *kd + 1 + j * ab_dim1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L160: */
	    }
	    i__1 = *kd + 1 + *n * ab_dim1;
	    i__3 = *kd + 1 + *n * ab_dim1;
	    q__1.r = smlnum * ab[i__3].r, q__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = *n - j;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    clarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		}
		i__3 = j * ab_dim1 + 1;
		clarnd_(&q__1, &c__5, &iseed[1]);
		ab[i__3].r = q__1.r, ab[i__3].i = q__1.i;
/* L170: */
	    }
	    i__1 = ab_dim1 + 1;
	    i__3 = ab_dim1 + 1;
	    q__1.r = smlnum * ab[i__3].r, q__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
	}

    } else if (*imat == 13) {

/*        Type 13:  T is diagonal with small numbers on the diagonal to */
/*        make the growth factor underflow, but a small right hand side */
/*        chosen so that the solution does not overflow. */

	if (upper) {
	    jcount = 1;
	    for (j = *n; j >= 1; --j) {
/* Computing MAX */
		i__1 = 1, i__3 = *kd + 1 - (j - 1);
		i__4 = *kd;
		for (i__ = max(i__1,i__3); i__ <= i__4; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    ab[i__1].r = 0.f, ab[i__1].i = 0.f;
/* L180: */
		}
		if (jcount <= 2) {
		    i__4 = *kd + 1 + j * ab_dim1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		    ab[i__4].r = q__1.r, ab[i__4].i = q__1.i;
		} else {
		    i__4 = *kd + 1 + j * ab_dim1;
		    clarnd_(&q__1, &c__5, &iseed[1]);
		    ab[i__4].r = q__1.r, ab[i__4].i = q__1.i;
		}
		++jcount;
		if (jcount > 4) {
		    jcount = 1;
		}
/* L190: */
	    }
	} else {
	    jcount = 1;
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MIN */
		i__3 = *n - j + 1, i__2 = *kd + 1;
		i__1 = min(i__3,i__2);
		for (i__ = 2; i__ <= i__1; ++i__) {
		    i__3 = i__ + j * ab_dim1;
		    ab[i__3].r = 0.f, ab[i__3].i = 0.f;
/* L200: */
		}
		if (jcount <= 2) {
		    i__1 = j * ab_dim1 + 1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
		} else {
		    i__1 = j * ab_dim1 + 1;
		    clarnd_(&q__1, &c__5, &iseed[1]);
		    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
		}
		++jcount;
		if (jcount > 4) {
		    jcount = 1;
		}
/* L210: */
	    }
	}

/*        Set the right hand side alternately zero and small. */

	if (upper) {
	    b[1].r = 0.f, b[1].i = 0.f;
	    for (i__ = *n; i__ >= 2; i__ += -2) {
		i__4 = i__;
		b[i__4].r = 0.f, b[i__4].i = 0.f;
		i__4 = i__ - 1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		b[i__4].r = q__1.r, b[i__4].i = q__1.i;
/* L220: */
	    }
	} else {
	    i__4 = *n;
	    b[i__4].r = 0.f, b[i__4].i = 0.f;
	    i__4 = *n - 1;
	    for (i__ = 1; i__ <= i__4; i__ += 2) {
		i__1 = i__;
		b[i__1].r = 0.f, b[i__1].i = 0.f;
		i__1 = i__ + 1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = smlnum * q__2.r, q__1.i = smlnum * q__2.i;
		b[i__1].r = q__1.r, b[i__1].i = q__1.i;
/* L230: */
	    }
	}

    } else if (*imat == 14) {

/*        Type 14:  Make the diagonal elements small to cause gradual */
/*        overflow when dividing by T(j,j).  To control the amount of */
/*        scaling needed, the matrix is bidiagonal. */

	texp = 1.f / (real) (*kd + 1);
	d__1 = (doublereal) smlnum;
	d__2 = (doublereal) texp;
	tscal = pow_dd(&d__1, &d__2);
	clarnv_(&c__4, &iseed[1], n, &b[1]);
	if (upper) {
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MAX */
		i__1 = 1, i__3 = *kd + 2 - j;
		i__2 = *kd;
		for (i__ = max(i__1,i__3); i__ <= i__2; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    ab[i__1].r = 0.f, ab[i__1].i = 0.f;
/* L240: */
		}
		if (j > 1 && *kd > 0) {
		    i__2 = *kd + j * ab_dim1;
		    ab[i__2].r = -1.f, ab[i__2].i = -1.f;
		}
		i__2 = *kd + 1 + j * ab_dim1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = tscal * q__2.r, q__1.i = tscal * q__2.i;
		ab[i__2].r = q__1.r, ab[i__2].i = q__1.i;
/* L250: */
	    }
	    i__4 = *n;
	    b[i__4].r = 1.f, b[i__4].i = 1.f;
	} else {
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MIN */
		i__1 = *n - j + 1, i__3 = *kd + 1;
		i__2 = min(i__1,i__3);
		for (i__ = 3; i__ <= i__2; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    ab[i__1].r = 0.f, ab[i__1].i = 0.f;
/* L260: */
		}
		if (j < *n && *kd > 0) {
		    i__2 = j * ab_dim1 + 2;
		    ab[i__2].r = -1.f, ab[i__2].i = -1.f;
		}
		i__2 = j * ab_dim1 + 1;
		clarnd_(&q__2, &c__5, &iseed[1]);
		q__1.r = tscal * q__2.r, q__1.i = tscal * q__2.i;
		ab[i__2].r = q__1.r, ab[i__2].i = q__1.i;
/* L270: */
	    }
	    b[1].r = 1.f, b[1].i = 1.f;
	}

    } else if (*imat == 15) {

/*        Type 15:  One zero diagonal element. */

	iy = *n / 2 + 1;
	if (upper) {
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MIN */
		i__2 = j, i__1 = *kd + 1;
		lenj = min(i__2,i__1);
		clarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			ab_dim1]);
		if (j != iy) {
		    i__2 = *kd + 1 + j * ab_dim1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		    ab[i__2].r = q__1.r, ab[i__2].i = q__1.i;
		} else {
		    i__2 = *kd + 1 + j * ab_dim1;
		    ab[i__2].r = 0.f, ab[i__2].i = 0.f;
		}
/* L280: */
	    }
	} else {
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MIN */
		i__2 = *n - j + 1, i__1 = *kd + 1;
		lenj = min(i__2,i__1);
		clarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 1]);
		if (j != iy) {
		    i__2 = j * ab_dim1 + 1;
		    clarnd_(&q__2, &c__5, &iseed[1]);
		    q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
		    ab[i__2].r = q__1.r, ab[i__2].i = q__1.i;
		} else {
		    i__2 = j * ab_dim1 + 1;
		    ab[i__2].r = 0.f, ab[i__2].i = 0.f;
		}
/* L290: */
	    }
	}
	clarnv_(&c__2, &iseed[1], n, &b[1]);
	csscal_(n, &c_b91, &b[1], &c__1);

    } else if (*imat == 16) {

/*        Type 16:  Make the offdiagonal elements large to cause overflow */
/*        when adding a column of T.  In the non-transposed case, the */
/*        matrix is constructed to cause overflow when adding a column in */
/*        every other step. */

	tscal = unfl / ulp;
	tscal = (1.f - ulp) / tscal;
	i__4 = *n;
	for (j = 1; j <= i__4; ++j) {
	    i__2 = *kd + 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__1 = i__ + j * ab_dim1;
		ab[i__1].r = 0.f, ab[i__1].i = 0.f;
/* L300: */
	    }
/* L310: */
	}
	texp = 1.f;
	if (*kd > 0) {
	    if (upper) {
		i__4 = -(*kd);
		for (j = *n; i__4 < 0 ? j >= 1 : j <= 1; j += i__4) {
/* Computing MAX */
		    i__1 = 1, i__3 = j - *kd + 1;
		    i__2 = max(i__1,i__3);
		    for (i__ = j; i__ >= i__2; i__ += -2) {
			i__1 = j - i__ + 1 + i__ * ab_dim1;
			r__1 = -tscal / (real) (*kd + 2);
			ab[i__1].r = r__1, ab[i__1].i = 0.f;
			i__1 = *kd + 1 + i__ * ab_dim1;
			ab[i__1].r = 1.f, ab[i__1].i = 0.f;
			i__1 = i__;
			r__1 = texp * (1.f - ulp);
			b[i__1].r = r__1, b[i__1].i = 0.f;
/* Computing MAX */
			i__1 = 1, i__3 = j - *kd + 1;
			if (i__ > max(i__1,i__3)) {
			    i__1 = j - i__ + 2 + (i__ - 1) * ab_dim1;
			    r__1 = -(tscal / (real) (*kd + 2)) / (real) (*kd 
				    + 3);
			    ab[i__1].r = r__1, ab[i__1].i = 0.f;
			    i__1 = *kd + 1 + (i__ - 1) * ab_dim1;
			    ab[i__1].r = 1.f, ab[i__1].i = 0.f;
			    i__1 = i__ - 1;
			    r__1 = texp * (real) ((*kd + 1) * (*kd + 1) + *kd)
				    ;
			    b[i__1].r = r__1, b[i__1].i = 0.f;
			}
			texp *= 2.f;
/* L320: */
		    }
/* Computing MAX */
		    i__1 = 1, i__3 = j - *kd + 1;
		    i__2 = max(i__1,i__3);
		    r__1 = (real) (*kd + 2) / (real) (*kd + 3) * tscal;
		    b[i__2].r = r__1, b[i__2].i = 0.f;
/* L330: */
		}
	    } else {
		i__4 = *n;
		i__2 = *kd;
		for (j = 1; i__2 < 0 ? j >= i__4 : j <= i__4; j += i__2) {
		    texp = 1.f;
/* Computing MIN */
		    i__1 = *kd + 1, i__3 = *n - j + 1;
		    lenj = min(i__1,i__3);
/* Computing MIN */
		    i__3 = *n, i__5 = j + *kd - 1;
		    i__1 = min(i__3,i__5);
		    for (i__ = j; i__ <= i__1; i__ += 2) {
			i__3 = lenj - (i__ - j) + j * ab_dim1;
			r__1 = -tscal / (real) (*kd + 2);
			ab[i__3].r = r__1, ab[i__3].i = 0.f;
			i__3 = j * ab_dim1 + 1;
			ab[i__3].r = 1.f, ab[i__3].i = 0.f;
			i__3 = j;
			r__1 = texp * (1.f - ulp);
			b[i__3].r = r__1, b[i__3].i = 0.f;
/* Computing MIN */
			i__3 = *n, i__5 = j + *kd - 1;
			if (i__ < min(i__3,i__5)) {
			    i__3 = lenj - (i__ - j + 1) + (i__ + 1) * ab_dim1;
			    r__1 = -(tscal / (real) (*kd + 2)) / (real) (*kd 
				    + 3);
			    ab[i__3].r = r__1, ab[i__3].i = 0.f;
			    i__3 = (i__ + 1) * ab_dim1 + 1;
			    ab[i__3].r = 1.f, ab[i__3].i = 0.f;
			    i__3 = i__ + 1;
			    r__1 = texp * (real) ((*kd + 1) * (*kd + 1) + *kd)
				    ;
			    b[i__3].r = r__1, b[i__3].i = 0.f;
			}
			texp *= 2.f;
/* L340: */
		    }
/* Computing MIN */
		    i__3 = *n, i__5 = j + *kd - 1;
		    i__1 = min(i__3,i__5);
		    r__1 = (real) (*kd + 2) / (real) (*kd + 3) * tscal;
		    b[i__1].r = r__1, b[i__1].i = 0.f;
/* L350: */
		}
	    }
	}

    } else if (*imat == 17) {

/*        Type 17:  Generate a unit triangular matrix with elements */
/*        between -1 and 1, and make the right hand side large so that it */
/*        requires scaling. */

	if (upper) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = j - 1;
		lenj = min(i__4,*kd);
		clarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 1 - lenj + j * 
			ab_dim1]);
		i__4 = *kd + 1 + j * ab_dim1;
		r__1 = (real) j;
		ab[i__4].r = r__1, ab[i__4].i = 0.f;
/* L360: */
	    }
	} else {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = *n - j;
		lenj = min(i__4,*kd);
		if (lenj > 0) {
		    clarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		}
		i__4 = j * ab_dim1 + 1;
		r__1 = (real) j;
		ab[i__4].r = r__1, ab[i__4].i = 0.f;
/* L370: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	clarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = icamax_(n, &b[1], &c__1);
	bnorm = c_abs(&b[iy]);
	bscal = bignum / dmax(1.f,bnorm);
	csscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 18) {

/*        Type 18:  Generate a triangular matrix with elements between */
/*        BIGNUM/(KD+1) and BIGNUM so that at least one of the column */
/*        norms will exceed BIGNUM. */
/*        1/3/91:  CLATBS no longer can handle this case */

	tleft = bignum / (real) (*kd + 1);
	tscal = bignum * ((real) (*kd + 1) / (real) (*kd + 2));
	if (upper) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = j, i__1 = *kd + 1;
		lenj = min(i__4,i__1);
		clarnv_(&c__5, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			ab_dim1]);
		slarnv_(&c__1, &iseed[1], &lenj, &rwork[*kd + 2 - lenj]);
		i__4 = *kd + 1;
		for (i__ = *kd + 2 - lenj; i__ <= i__4; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    i__3 = i__ + j * ab_dim1;
		    r__1 = tleft + rwork[i__] * tscal;
		    q__1.r = r__1 * ab[i__3].r, q__1.i = r__1 * ab[i__3].i;
		    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
/* L380: */
		}
/* L390: */
	    }
	} else {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = *n - j + 1, i__1 = *kd + 1;
		lenj = min(i__4,i__1);
		clarnv_(&c__5, &iseed[1], &lenj, &ab[j * ab_dim1 + 1]);
		slarnv_(&c__1, &iseed[1], &lenj, &rwork[1]);
		i__4 = lenj;
		for (i__ = 1; i__ <= i__4; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    i__3 = i__ + j * ab_dim1;
		    r__1 = tleft + rwork[i__] * tscal;
		    q__1.r = r__1 * ab[i__3].r, q__1.i = r__1 * ab[i__3].i;
		    ab[i__1].r = q__1.r, ab[i__1].i = q__1.i;
/* L400: */
		}
/* L410: */
	    }
	}
	clarnv_(&c__2, &iseed[1], n, &b[1]);
	csscal_(n, &c_b91, &b[1], &c__1);
    }

/*     Flip the matrix if the transpose will be used. */

    if (! lsame_(trans, "N")) {
	if (upper) {
	    i__2 = *n / 2;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = *n - (j << 1) + 1, i__1 = *kd + 1;
		lenj = min(i__4,i__1);
		i__4 = *ldab - 1;
		cswap_(&lenj, &ab[*kd + 1 + j * ab_dim1], &i__4, &ab[*kd + 2 
			- lenj + (*n - j + 1) * ab_dim1], &c_n1);
/* L420: */
	    }
	} else {
	    i__2 = *n / 2;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = *n - (j << 1) + 1, i__1 = *kd + 1;
		lenj = min(i__4,i__1);
		i__4 = -(*ldab) + 1;
		cswap_(&lenj, &ab[j * ab_dim1 + 1], &c__1, &ab[lenj + (*n - j 
			+ 2 - lenj) * ab_dim1], &i__4);
/* L430: */
	    }
	}
    }

    return 0;

/*     End of CLATTB */

} /* clattb_ */
