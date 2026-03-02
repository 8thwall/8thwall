#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__5 = 5;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__4 = 4;
static doublereal c_b91 = 2.;
static integer c_n1 = -1;

/* Subroutine */ int zlattb_(integer *imat, char *uplo, char *trans, char *
	diag, integer *iseed, integer *n, integer *kd, doublecomplex *ab, 
	integer *ldab, doublecomplex *b, doublecomplex *work, doublereal *
	rwork, integer *info)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);
    double pow_dd(doublereal *, doublereal *), z_abs(doublecomplex *);

    /* Local variables */
    integer i__, j, kl, ku, iy;
    doublereal ulp, sfac;
    integer ioff, mode, lenj;
    char path[3], dist[1];
    doublereal unfl, rexp;
    char type__[1];
    doublereal texp;
    doublecomplex star1, plus1, plus2;
    doublereal bscal;
    extern logical lsame_(char *, char *);
    doublereal tscal, anorm, bnorm, tleft;
    logical upper;
    doublereal tnorm;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zswap_(integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *), zlatb4_(char *, integer *, 
	     integer *, integer *, char *, integer *, integer *, doublereal *, 
	     integer *, doublereal *, char *), 
	    dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlarnd_(integer *, integer *);
    char packit[1];
    extern /* Subroutine */ int zdscal_(integer *, doublereal *, 
	    doublecomplex *, integer *);
    doublereal bignum, cndnum;
    extern /* Subroutine */ int dlarnv_(integer *, integer *, integer *, 
	    doublereal *);
    extern integer izamax_(integer *, doublecomplex *, integer *);
    extern /* Double Complex */ VOID zlarnd_(doublecomplex *, integer *, 
	    integer *);
    integer jcount;
    extern /* Subroutine */ int zlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    doublereal smlnum;
    extern /* Subroutine */ int zlarnv_(integer *, integer *, integer *, 
	    doublecomplex *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLATTB generates a triangular test matrix in 2-dimensional storage. */
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
/*          ZLATMS).  Modified on exit. */

/*  N       (input) INTEGER */
/*          The order of the matrix to be generated. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals or subdiagonals of the banded */
/*          triangular matrix A.  KD >= 0. */

/*  AB      (output) COMPLEX*16 array, dimension (LDAB,N) */
/*          The upper or lower triangular banded matrix A, stored in the */
/*          first KD+1 rows of AB.  Let j be a column of A, 1<=j<=n. */
/*          If UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j. */
/*          If UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  B       (workspace) COMPLEX*16 array, dimension (N) */

/*  WORK    (workspace) COMPLEX*16 array, dimension (2*N) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (N) */

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
    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "TB", (ftnlen)2, (ftnlen)2);
    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Epsilon") * dlamch_("Base");
    smlnum = unfl;
    bignum = (1. - ulp) / smlnum;
    dlabad_(&smlnum, &bignum);
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

/*     Call ZLATB4 to set parameters for CLATMS. */

    upper = lsame_(uplo, "U");
    if (upper) {
	zlatb4_(path, imat, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
	ku = *kd;
/* Computing MAX */
	i__1 = 0, i__2 = *kd - *n + 1;
	ioff = max(i__1,i__2) + 1;
	kl = 0;
	*(unsigned char *)packit = 'Q';
    } else {
	i__1 = -(*imat);
	zlatb4_(path, &i__1, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
	kl = *kd;
	ioff = 1;
	ku = 0;
	*(unsigned char *)packit = 'B';
    }

/*     IMAT <= 5:  Non-unit triangular matrix */

    if (*imat <= 5) {
	zlatms_(n, n, dist, &iseed[1], type__, &rwork[1], &mode, &cndnum, &
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
		    ab[i__2].r = 0., ab[i__2].i = 0.;
/* L10: */
		}
		i__4 = *kd + 1 + j * ab_dim1;
		ab[i__4].r = (doublereal) j, ab[i__4].i = 0.;
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__4 = j * ab_dim1 + 1;
		ab[i__4].r = (doublereal) j, ab[i__4].i = 0.;
/* Computing MIN */
		i__2 = *kd + 1, i__3 = *n - j + 1;
		i__4 = min(i__2,i__3);
		for (i__ = 2; i__ <= i__4; ++i__) {
		    i__2 = i__ + j * ab_dim1;
		    ab[i__2].r = 0., ab[i__2].i = 0.;
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
		    ab[i__4].r = 0., ab[i__4].i = 0.;
/* L50: */
		}
		i__3 = *kd + 1 + j * ab_dim1;
		d__1 = (doublereal) j;
		ab[i__3].r = d__1, ab[i__3].i = 0.;
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
		    ab[i__4].r = 0., ab[i__4].i = 0.;
/* L70: */
		}
		i__3 = j * ab_dim1 + 1;
		d__1 = (doublereal) j;
		ab[i__3].r = d__1, ab[i__3].i = 0.;
/* L80: */
	    }
	}

/*        Special case:  T is tridiagonal.  Set every other offdiagonal */
/*        so that the matrix has norm TNORM+1. */

	if (*kd == 1) {
	    if (upper) {
		i__1 = (ab_dim1 << 1) + 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tnorm * z__2.r, z__1.i = tnorm * z__2.i;
		ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
		lenj = (*n - 3) / 2;
		zlarnv_(&c__2, &iseed[1], &lenj, &work[1]);
		i__1 = lenj;
		for (j = 1; j <= i__1; ++j) {
		    i__3 = (j + 1 << 1) * ab_dim1 + 1;
		    i__4 = j;
		    z__1.r = tnorm * work[i__4].r, z__1.i = tnorm * work[i__4]
			    .i;
		    ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
/* L90: */
		}
	    } else {
		i__1 = ab_dim1 + 2;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tnorm * z__2.r, z__1.i = tnorm * z__2.i;
		ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
		lenj = (*n - 3) / 2;
		zlarnv_(&c__2, &iseed[1], &lenj, &work[1]);
		i__1 = lenj;
		for (j = 1; j <= i__1; ++j) {
		    i__3 = ((j << 1) + 1) * ab_dim1 + 2;
		    i__4 = j;
		    z__1.r = tnorm * work[i__4].r, z__1.i = tnorm * work[i__4]
			    .i;
		    ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
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

	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = tnorm * z__2.r, z__1.i = tnorm * z__2.i;
	    star1.r = z__1.r, star1.i = z__1.i;
	    sfac = sqrt(tnorm);
	    zlarnd_(&z__2, &c__5, &iseed[1]);
	    z__1.r = sfac * z__2.r, z__1.i = sfac * z__2.i;
	    plus1.r = z__1.r, plus1.i = z__1.i;
	    i__1 = *n;
	    for (j = 1; j <= i__1; j += 2) {
		z_div(&z__1, &star1, &plus1);
		plus2.r = z__1.r, plus2.i = z__1.i;
		i__3 = j;
		work[i__3].r = plus1.r, work[i__3].i = plus1.i;
		i__3 = *n + j;
		work[i__3].r = star1.r, work[i__3].i = star1.i;
		if (j + 1 <= *n) {
		    i__3 = j + 1;
		    work[i__3].r = plus2.r, work[i__3].i = plus2.i;
		    i__3 = *n + j + 1;
		    work[i__3].r = 0., work[i__3].i = 0.;
		    z_div(&z__1, &star1, &plus2);
		    plus1.r = z__1.r, plus1.i = z__1.i;

/*                 Generate a new *-value with norm between sqrt(TNORM) */
/*                 and TNORM. */

		    rexp = dlarnd_(&c__2, &iseed[1]);
		    if (rexp < 0.) {
			d__2 = 1. - rexp;
			d__1 = -pow_dd(&sfac, &d__2);
			zlarnd_(&z__2, &c__5, &iseed[1]);
			z__1.r = d__1 * z__2.r, z__1.i = d__1 * z__2.i;
			star1.r = z__1.r, star1.i = z__1.i;
		    } else {
			d__2 = rexp + 1.;
			d__1 = pow_dd(&sfac, &d__2);
			zlarnd_(&z__2, &c__5, &iseed[1]);
			z__1.r = d__1 * z__2.r, z__1.i = d__1 * z__2.i;
			star1.r = z__1.r, star1.i = z__1.i;
		    }
		}
/* L110: */
	    }

/*           Copy the tridiagonal T to AB. */

	    if (upper) {
		i__1 = *n - 1;
		zcopy_(&i__1, &work[1], &c__1, &ab[*kd + (ab_dim1 << 1)], 
			ldab);
		i__1 = *n - 2;
		zcopy_(&i__1, &work[*n + 1], &c__1, &ab[*kd - 1 + ab_dim1 * 3]
, ldab);
	    } else {
		i__1 = *n - 1;
		zcopy_(&i__1, &work[1], &c__1, &ab[ab_dim1 + 2], ldab);
		i__1 = *n - 2;
		zcopy_(&i__1, &work[*n + 1], &c__1, &ab[ab_dim1 + 3], ldab);
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
		zlarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 1 - lenj + j * 
			ab_dim1]);
		i__3 = *kd + 1 + j * ab_dim1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
/* L120: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = *n - j;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    zlarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		}
		i__3 = j * ab_dim1 + 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
/* L130: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = izamax_(n, &b[1], &c__1);
	bnorm = z_abs(&b[iy]);
	bscal = bignum / max(1.,bnorm);
	zdscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 11) {

/*        Type 11:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 11, the offdiagonal elements are small (CNORM(j) < 1). */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	tscal = 1. / (doublereal) (*kd + 1);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = j - 1;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    zlarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			    ab_dim1]);
		    zdscal_(&lenj, &tscal, &ab[*kd + 2 - lenj + j * ab_dim1], 
			    &c__1);
		}
		i__3 = *kd + 1 + j * ab_dim1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
/* L140: */
	    }
	    i__1 = *kd + 1 + *n * ab_dim1;
	    i__3 = *kd + 1 + *n * ab_dim1;
	    z__1.r = smlnum * ab[i__3].r, z__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = *n - j;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    zlarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		    zdscal_(&lenj, &tscal, &ab[j * ab_dim1 + 2], &c__1);
		}
		i__3 = j * ab_dim1 + 1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
/* L150: */
	    }
	    i__1 = ab_dim1 + 1;
	    i__3 = ab_dim1 + 1;
	    z__1.r = smlnum * ab[i__3].r, z__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
	}

    } else if (*imat == 12) {

/*        Type 12:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 12, the offdiagonal elements are O(1) (CNORM(j) > 1). */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = j - 1;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    zlarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			    ab_dim1]);
		}
		i__3 = *kd + 1 + j * ab_dim1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
/* L160: */
	    }
	    i__1 = *kd + 1 + *n * ab_dim1;
	    i__3 = *kd + 1 + *n * ab_dim1;
	    z__1.r = smlnum * ab[i__3].r, z__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__3 = *n - j;
		lenj = min(i__3,*kd);
		if (lenj > 0) {
		    zlarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		}
		i__3 = j * ab_dim1 + 1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ab[i__3].r = z__1.r, ab[i__3].i = z__1.i;
/* L170: */
	    }
	    i__1 = ab_dim1 + 1;
	    i__3 = ab_dim1 + 1;
	    z__1.r = smlnum * ab[i__3].r, z__1.i = smlnum * ab[i__3].i;
	    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
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
		    ab[i__1].r = 0., ab[i__1].i = 0.;
/* L180: */
		}
		if (jcount <= 2) {
		    i__4 = *kd + 1 + j * ab_dim1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		    ab[i__4].r = z__1.r, ab[i__4].i = z__1.i;
		} else {
		    i__4 = *kd + 1 + j * ab_dim1;
		    zlarnd_(&z__1, &c__5, &iseed[1]);
		    ab[i__4].r = z__1.r, ab[i__4].i = z__1.i;
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
		    ab[i__3].r = 0., ab[i__3].i = 0.;
/* L200: */
		}
		if (jcount <= 2) {
		    i__1 = j * ab_dim1 + 1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
		} else {
		    i__1 = j * ab_dim1 + 1;
		    zlarnd_(&z__1, &c__5, &iseed[1]);
		    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
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
	    b[1].r = 0., b[1].i = 0.;
	    for (i__ = *n; i__ >= 2; i__ += -2) {
		i__4 = i__;
		b[i__4].r = 0., b[i__4].i = 0.;
		i__4 = i__ - 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		b[i__4].r = z__1.r, b[i__4].i = z__1.i;
/* L220: */
	    }
	} else {
	    i__4 = *n;
	    b[i__4].r = 0., b[i__4].i = 0.;
	    i__4 = *n - 1;
	    for (i__ = 1; i__ <= i__4; i__ += 2) {
		i__1 = i__;
		b[i__1].r = 0., b[i__1].i = 0.;
		i__1 = i__ + 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		b[i__1].r = z__1.r, b[i__1].i = z__1.i;
/* L230: */
	    }
	}

    } else if (*imat == 14) {

/*        Type 14:  Make the diagonal elements small to cause gradual */
/*        overflow when dividing by T(j,j).  To control the amount of */
/*        scaling needed, the matrix is bidiagonal. */

	texp = 1. / (doublereal) (*kd + 1);
	tscal = pow_dd(&smlnum, &texp);
	zlarnv_(&c__4, &iseed[1], n, &b[1]);
	if (upper) {
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MAX */
		i__1 = 1, i__3 = *kd + 2 - j;
		i__2 = *kd;
		for (i__ = max(i__1,i__3); i__ <= i__2; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    ab[i__1].r = 0., ab[i__1].i = 0.;
/* L240: */
		}
		if (j > 1 && *kd > 0) {
		    i__2 = *kd + j * ab_dim1;
		    ab[i__2].r = -1., ab[i__2].i = -1.;
		}
		i__2 = *kd + 1 + j * ab_dim1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tscal * z__2.r, z__1.i = tscal * z__2.i;
		ab[i__2].r = z__1.r, ab[i__2].i = z__1.i;
/* L250: */
	    }
	    i__4 = *n;
	    b[i__4].r = 1., b[i__4].i = 1.;
	} else {
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MIN */
		i__1 = *n - j + 1, i__3 = *kd + 1;
		i__2 = min(i__1,i__3);
		for (i__ = 3; i__ <= i__2; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    ab[i__1].r = 0., ab[i__1].i = 0.;
/* L260: */
		}
		if (j < *n && *kd > 0) {
		    i__2 = j * ab_dim1 + 2;
		    ab[i__2].r = -1., ab[i__2].i = -1.;
		}
		i__2 = j * ab_dim1 + 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tscal * z__2.r, z__1.i = tscal * z__2.i;
		ab[i__2].r = z__1.r, ab[i__2].i = z__1.i;
/* L270: */
	    }
	    b[1].r = 1., b[1].i = 1.;
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
		zlarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			ab_dim1]);
		if (j != iy) {
		    i__2 = *kd + 1 + j * ab_dim1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		    ab[i__2].r = z__1.r, ab[i__2].i = z__1.i;
		} else {
		    i__2 = *kd + 1 + j * ab_dim1;
		    ab[i__2].r = 0., ab[i__2].i = 0.;
		}
/* L280: */
	    }
	} else {
	    i__4 = *n;
	    for (j = 1; j <= i__4; ++j) {
/* Computing MIN */
		i__2 = *n - j + 1, i__1 = *kd + 1;
		lenj = min(i__2,i__1);
		zlarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 1]);
		if (j != iy) {
		    i__2 = j * ab_dim1 + 1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		    ab[i__2].r = z__1.r, ab[i__2].i = z__1.i;
		} else {
		    i__2 = j * ab_dim1 + 1;
		    ab[i__2].r = 0., ab[i__2].i = 0.;
		}
/* L290: */
	    }
	}
	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	zdscal_(n, &c_b91, &b[1], &c__1);

    } else if (*imat == 16) {

/*        Type 16:  Make the offdiagonal elements large to cause overflow */
/*        when adding a column of T.  In the non-transposed case, the */
/*        matrix is constructed to cause overflow when adding a column in */
/*        every other step. */

	tscal = unfl / ulp;
	tscal = (1. - ulp) / tscal;
	i__4 = *n;
	for (j = 1; j <= i__4; ++j) {
	    i__2 = *kd + 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__1 = i__ + j * ab_dim1;
		ab[i__1].r = 0., ab[i__1].i = 0.;
/* L300: */
	    }
/* L310: */
	}
	texp = 1.;
	if (*kd > 0) {
	    if (upper) {
		i__4 = -(*kd);
		for (j = *n; i__4 < 0 ? j >= 1 : j <= 1; j += i__4) {
/* Computing MAX */
		    i__1 = 1, i__3 = j - *kd + 1;
		    i__2 = max(i__1,i__3);
		    for (i__ = j; i__ >= i__2; i__ += -2) {
			i__1 = j - i__ + 1 + i__ * ab_dim1;
			d__1 = -tscal / (doublereal) (*kd + 2);
			ab[i__1].r = d__1, ab[i__1].i = 0.;
			i__1 = *kd + 1 + i__ * ab_dim1;
			ab[i__1].r = 1., ab[i__1].i = 0.;
			i__1 = i__;
			d__1 = texp * (1. - ulp);
			b[i__1].r = d__1, b[i__1].i = 0.;
/* Computing MAX */
			i__1 = 1, i__3 = j - *kd + 1;
			if (i__ > max(i__1,i__3)) {
			    i__1 = j - i__ + 2 + (i__ - 1) * ab_dim1;
			    d__1 = -(tscal / (doublereal) (*kd + 2)) / (
				    doublereal) (*kd + 3);
			    ab[i__1].r = d__1, ab[i__1].i = 0.;
			    i__1 = *kd + 1 + (i__ - 1) * ab_dim1;
			    ab[i__1].r = 1., ab[i__1].i = 0.;
			    i__1 = i__ - 1;
			    d__1 = texp * (doublereal) ((*kd + 1) * (*kd + 1) 
				    + *kd);
			    b[i__1].r = d__1, b[i__1].i = 0.;
			}
			texp *= 2.;
/* L320: */
		    }
/* Computing MAX */
		    i__1 = 1, i__3 = j - *kd + 1;
		    i__2 = max(i__1,i__3);
		    d__1 = (doublereal) (*kd + 2) / (doublereal) (*kd + 3) * 
			    tscal;
		    b[i__2].r = d__1, b[i__2].i = 0.;
/* L330: */
		}
	    } else {
		i__4 = *n;
		i__2 = *kd;
		for (j = 1; i__2 < 0 ? j >= i__4 : j <= i__4; j += i__2) {
		    texp = 1.;
/* Computing MIN */
		    i__1 = *kd + 1, i__3 = *n - j + 1;
		    lenj = min(i__1,i__3);
/* Computing MIN */
		    i__3 = *n, i__5 = j + *kd - 1;
		    i__1 = min(i__3,i__5);
		    for (i__ = j; i__ <= i__1; i__ += 2) {
			i__3 = lenj - (i__ - j) + j * ab_dim1;
			d__1 = -tscal / (doublereal) (*kd + 2);
			ab[i__3].r = d__1, ab[i__3].i = 0.;
			i__3 = j * ab_dim1 + 1;
			ab[i__3].r = 1., ab[i__3].i = 0.;
			i__3 = j;
			d__1 = texp * (1. - ulp);
			b[i__3].r = d__1, b[i__3].i = 0.;
/* Computing MIN */
			i__3 = *n, i__5 = j + *kd - 1;
			if (i__ < min(i__3,i__5)) {
			    i__3 = lenj - (i__ - j + 1) + (i__ + 1) * ab_dim1;
			    d__1 = -(tscal / (doublereal) (*kd + 2)) / (
				    doublereal) (*kd + 3);
			    ab[i__3].r = d__1, ab[i__3].i = 0.;
			    i__3 = (i__ + 1) * ab_dim1 + 1;
			    ab[i__3].r = 1., ab[i__3].i = 0.;
			    i__3 = i__ + 1;
			    d__1 = texp * (doublereal) ((*kd + 1) * (*kd + 1) 
				    + *kd);
			    b[i__3].r = d__1, b[i__3].i = 0.;
			}
			texp *= 2.;
/* L340: */
		    }
/* Computing MIN */
		    i__3 = *n, i__5 = j + *kd - 1;
		    i__1 = min(i__3,i__5);
		    d__1 = (doublereal) (*kd + 2) / (doublereal) (*kd + 3) * 
			    tscal;
		    b[i__1].r = d__1, b[i__1].i = 0.;
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
		zlarnv_(&c__4, &iseed[1], &lenj, &ab[*kd + 1 - lenj + j * 
			ab_dim1]);
		i__4 = *kd + 1 + j * ab_dim1;
		d__1 = (doublereal) j;
		ab[i__4].r = d__1, ab[i__4].i = 0.;
/* L360: */
	    }
	} else {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = *n - j;
		lenj = min(i__4,*kd);
		if (lenj > 0) {
		    zlarnv_(&c__4, &iseed[1], &lenj, &ab[j * ab_dim1 + 2]);
		}
		i__4 = j * ab_dim1 + 1;
		d__1 = (doublereal) j;
		ab[i__4].r = d__1, ab[i__4].i = 0.;
/* L370: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = izamax_(n, &b[1], &c__1);
	bnorm = z_abs(&b[iy]);
	bscal = bignum / max(1.,bnorm);
	zdscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 18) {

/*        Type 18:  Generate a triangular matrix with elements between */
/*        BIGNUM/(KD+1) and BIGNUM so that at least one of the column */
/*        norms will exceed BIGNUM. */
/*        1/3/91:  ZLATBS no longer can handle this case */

	tleft = bignum / (doublereal) (*kd + 1);
	tscal = bignum * ((doublereal) (*kd + 1) / (doublereal) (*kd + 2));
	if (upper) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
/* Computing MIN */
		i__4 = j, i__1 = *kd + 1;
		lenj = min(i__4,i__1);
		zlarnv_(&c__5, &iseed[1], &lenj, &ab[*kd + 2 - lenj + j * 
			ab_dim1]);
		dlarnv_(&c__1, &iseed[1], &lenj, &rwork[*kd + 2 - lenj]);
		i__4 = *kd + 1;
		for (i__ = *kd + 2 - lenj; i__ <= i__4; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    i__3 = i__ + j * ab_dim1;
		    d__1 = tleft + rwork[i__] * tscal;
		    z__1.r = d__1 * ab[i__3].r, z__1.i = d__1 * ab[i__3].i;
		    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
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
		zlarnv_(&c__5, &iseed[1], &lenj, &ab[j * ab_dim1 + 1]);
		dlarnv_(&c__1, &iseed[1], &lenj, &rwork[1]);
		i__4 = lenj;
		for (i__ = 1; i__ <= i__4; ++i__) {
		    i__1 = i__ + j * ab_dim1;
		    i__3 = i__ + j * ab_dim1;
		    d__1 = tleft + rwork[i__] * tscal;
		    z__1.r = d__1 * ab[i__3].r, z__1.i = d__1 * ab[i__3].i;
		    ab[i__1].r = z__1.r, ab[i__1].i = z__1.i;
/* L400: */
		}
/* L410: */
	    }
	}
	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	zdscal_(n, &c_b91, &b[1], &c__1);
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
		zswap_(&lenj, &ab[*kd + 1 + j * ab_dim1], &i__4, &ab[*kd + 2 
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
		zswap_(&lenj, &ab[j * ab_dim1 + 1], &c__1, &ab[lenj + (*n - j 
			+ 2 - lenj) * ab_dim1], &i__4);
/* L430: */
	    }
	}
    }

    return 0;

/*     End of ZLATTB */

} /* zlattb_ */
