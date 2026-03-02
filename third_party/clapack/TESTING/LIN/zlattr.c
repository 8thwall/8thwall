#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__5 = 5;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__4 = 4;
static doublereal c_b92 = 2.;
static integer c_n1 = -1;

/* Subroutine */ int zlattr_(integer *imat, char *uplo, char *trans, char *
	diag, integer *iseed, integer *n, doublecomplex *a, integer *lda, 
	doublecomplex *b, doublecomplex *work, doublereal *rwork, integer *
	info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal);
    void d_cnjg(doublecomplex *, doublecomplex *);
    double z_abs(doublecomplex *);

    /* Local variables */
    doublereal c__;
    integer i__, j;
    doublecomplex s;
    doublereal x, y, z__;
    doublecomplex ra, rb;
    integer kl, ku, iy;
    doublereal ulp, sfac;
    integer mode;
    char path[3], dist[1];
    doublereal unfl, rexp;
    char type__[1];
    doublereal texp;
    extern /* Subroutine */ int zrot_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *);
    doublecomplex star1, plus1, plus2;
    doublereal bscal;
    extern logical lsame_(char *, char *);
    doublereal tscal, anorm, bnorm, tleft;
    logical upper;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zrotg_(doublecomplex *, 
	    doublecomplex *, doublereal *, doublecomplex *), zswap_(integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *), zlatb4_(
	    char *, integer *, integer *, integer *, char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, char *), dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlarnd_(integer *, integer *);
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

/*  ZLATTR generates a triangular test matrix in 2-dimensional storage. */
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
/*          ZLATMS).  Modified on exit. */

/*  N       (input) INTEGER */
/*          The order of the matrix to be generated. */

/*  A       (output) COMPLEX*16 array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading N x N */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading N x N lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix and the strictly upper triangular part of A is not */
/*          referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (output) COMPLEX*16 array, dimension (N) */
/*          The right hand side vector, if IMAT > 10. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --b;
    --work;
    --rwork;

    /* Function Body */
    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "TR", (ftnlen)2, (ftnlen)2);
    unfl = dlamch_("Safe minimum");
    ulp = dlamch_("Epsilon") * dlamch_("Base");
    smlnum = unfl;
    bignum = (1. - ulp) / smlnum;
    dlabad_(&smlnum, &bignum);
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

/*     Call ZLATB4 to set parameters for CLATMS. */

    upper = lsame_(uplo, "U");
    if (upper) {
	zlatb4_(path, imat, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
    } else {
	i__1 = -(*imat);
	zlatb4_(path, &i__1, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
    }

/*     IMAT <= 6:  Non-unit triangular matrix */

    if (*imat <= 6) {
	zlatms_(n, n, dist, &iseed[1], type__, &rwork[1], &mode, &cndnum, &
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
		    a[i__3].r = 0., a[i__3].i = 0.;
/* L10: */
		}
		i__2 = j + j * a_dim1;
		a[i__2].r = (doublereal) j, a[i__2].i = 0.;
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + j * a_dim1;
		a[i__2].r = (doublereal) j, a[i__2].i = 0.;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0., a[i__3].i = 0.;
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
		    a[i__3].r = 0., a[i__3].i = 0.;
/* L50: */
		}
		i__2 = j + j * a_dim1;
		a[i__2].r = (doublereal) j, a[i__2].i = 0.;
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + j * a_dim1;
		a[i__2].r = (doublereal) j, a[i__2].i = 0.;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0., a[i__3].i = 0.;
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

	zlarnd_(&z__2, &c__5, &iseed[1]);
	z__1.r = z__2.r * .25, z__1.i = z__2.i * .25;
	star1.r = z__1.r, star1.i = z__1.i;
	sfac = .5;
	zlarnd_(&z__2, &c__5, &iseed[1]);
	z__1.r = sfac * z__2.r, z__1.i = sfac * z__2.i;
	plus1.r = z__1.r, plus1.i = z__1.i;
	i__1 = *n;
	for (j = 1; j <= i__1; j += 2) {
	    z_div(&z__1, &star1, &plus1);
	    plus2.r = z__1.r, plus2.i = z__1.i;
	    i__2 = j;
	    work[i__2].r = plus1.r, work[i__2].i = plus1.i;
	    i__2 = *n + j;
	    work[i__2].r = star1.r, work[i__2].i = star1.i;
	    if (j + 1 <= *n) {
		i__2 = j + 1;
		work[i__2].r = plus2.r, work[i__2].i = plus2.i;
		i__2 = *n + j + 1;
		work[i__2].r = 0., work[i__2].i = 0.;
		z_div(&z__1, &star1, &plus2);
		plus1.r = z__1.r, plus1.i = z__1.i;
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
/* L90: */
	}

	x = sqrt(cndnum) - 1 / sqrt(cndnum);
	if (*n > 2) {
	    y = sqrt(2. / (*n - 2)) * x;
	} else {
	    y = 0.;
	}
	z__ = x * x;

	if (upper) {
	    if (*n > 3) {
		i__1 = *n - 3;
		i__2 = *lda + 1;
		zcopy_(&i__1, &work[1], &c__1, &a[a_dim1 * 3 + 2], &i__2);
		if (*n > 4) {
		    i__1 = *n - 4;
		    i__2 = *lda + 1;
		    zcopy_(&i__1, &work[*n + 1], &c__1, &a[(a_dim1 << 2) + 2], 
			     &i__2);
		}
	    }
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j * a_dim1 + 1;
		a[i__2].r = y, a[i__2].i = 0.;
		i__2 = j + *n * a_dim1;
		a[i__2].r = y, a[i__2].i = 0.;
/* L100: */
	    }
	    i__1 = *n * a_dim1 + 1;
	    a[i__1].r = z__, a[i__1].i = 0.;
	} else {
	    if (*n > 3) {
		i__1 = *n - 3;
		i__2 = *lda + 1;
		zcopy_(&i__1, &work[1], &c__1, &a[(a_dim1 << 1) + 3], &i__2);
		if (*n > 4) {
		    i__1 = *n - 4;
		    i__2 = *lda + 1;
		    zcopy_(&i__1, &work[*n + 1], &c__1, &a[(a_dim1 << 1) + 4], 
			     &i__2);
		}
	    }
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j + a_dim1;
		a[i__2].r = y, a[i__2].i = 0.;
		i__2 = *n + j * a_dim1;
		a[i__2].r = y, a[i__2].i = 0.;
/* L110: */
	    }
	    i__1 = *n + a_dim1;
	    a[i__1].r = z__, a[i__1].i = 0.;
	}

/*        Fill in the zeros using Givens rotations. */

	if (upper) {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + (j + 1) * a_dim1;
		ra.r = a[i__2].r, ra.i = a[i__2].i;
		rb.r = 2., rb.i = 0.;
		zrotg_(&ra, &rb, &c__, &s);

/*              Multiply by [ c  s; -conjg(s)  c] on the left. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    zrot_(&i__2, &a[j + (j + 2) * a_dim1], lda, &a[j + 1 + (j 
			    + 2) * a_dim1], lda, &c__, &s);
		}

/*              Multiply by [-c -s;  conjg(s) -c] on the right. */

		if (j > 1) {
		    i__2 = j - 1;
		    d__1 = -c__;
		    z__1.r = -s.r, z__1.i = -s.i;
		    zrot_(&i__2, &a[(j + 1) * a_dim1 + 1], &c__1, &a[j * 
			    a_dim1 + 1], &c__1, &d__1, &z__1);
		}

/*              Negate A(J,J+1). */

		i__2 = j + (j + 1) * a_dim1;
		i__3 = j + (j + 1) * a_dim1;
		z__1.r = -a[i__3].r, z__1.i = -a[i__3].i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L120: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j + 1 + j * a_dim1;
		ra.r = a[i__2].r, ra.i = a[i__2].i;
		rb.r = 2., rb.i = 0.;
		zrotg_(&ra, &rb, &c__, &s);
		d_cnjg(&z__1, &s);
		s.r = z__1.r, s.i = z__1.i;

/*              Multiply by [ c -s;  conjg(s) c] on the right. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    z__1.r = -s.r, z__1.i = -s.i;
		    zrot_(&i__2, &a[j + 2 + (j + 1) * a_dim1], &c__1, &a[j + 
			    2 + j * a_dim1], &c__1, &c__, &z__1);
		}

/*              Multiply by [-c  s; -conjg(s) -c] on the left. */

		if (j > 1) {
		    i__2 = j - 1;
		    d__1 = -c__;
		    zrot_(&i__2, &a[j + a_dim1], lda, &a[j + 1 + a_dim1], lda, 
			     &d__1, &s);
		}

/*              Negate A(J+1,J). */

		i__2 = j + 1 + j * a_dim1;
		i__3 = j + 1 + j * a_dim1;
		z__1.r = -a[i__3].r, z__1.i = -a[i__3].i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
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
		zlarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j + j * a_dim1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L140: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    zlarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		i__2 = j + j * a_dim1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L150: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = izamax_(n, &b[1], &c__1);
	bnorm = z_abs(&b[iy]);
	bscal = bignum / max(1.,bnorm);
	zdscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 12) {

/*        Type 12:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 12, the offdiagonal elements are small (CNORM(j) < 1). */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n - 1);
	tscal = 1. / max(d__1,d__2);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j - 1;
		zdscal_(&i__2, &tscal, &a[j * a_dim1 + 1], &c__1);
		i__2 = j + j * a_dim1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L160: */
	    }
	    i__1 = *n + *n * a_dim1;
	    i__2 = *n + *n * a_dim1;
	    z__1.r = smlnum * a[i__2].r, z__1.i = smlnum * a[i__2].i;
	    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    zlarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		    i__2 = *n - j;
		    zdscal_(&i__2, &tscal, &a[j + 1 + j * a_dim1], &c__1);
		}
		i__2 = j + j * a_dim1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L170: */
	    }
	    i__1 = a_dim1 + 1;
	    i__2 = a_dim1 + 1;
	    z__1.r = smlnum * a[i__2].r, z__1.i = smlnum * a[i__2].i;
	    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
	}

    } else if (*imat == 13) {

/*        Type 13:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 13, the offdiagonal elements are O(1) (CNORM(j) > 1). */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j + j * a_dim1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L180: */
	    }
	    i__1 = *n + *n * a_dim1;
	    i__2 = *n + *n * a_dim1;
	    z__1.r = smlnum * a[i__2].r, z__1.i = smlnum * a[i__2].i;
	    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    zlarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		i__2 = j + j * a_dim1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L190: */
	    }
	    i__1 = a_dim1 + 1;
	    i__2 = a_dim1 + 1;
	    z__1.r = smlnum * a[i__2].r, z__1.i = smlnum * a[i__2].i;
	    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
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
		    a[i__2].r = 0., a[i__2].i = 0.;
/* L200: */
		}
		if (jcount <= 2) {
		    i__1 = j + j * a_dim1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
		} else {
		    i__1 = j + j * a_dim1;
		    zlarnd_(&z__1, &c__5, &iseed[1]);
		    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
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
		    a[i__3].r = 0., a[i__3].i = 0.;
/* L220: */
		}
		if (jcount <= 2) {
		    i__2 = j + j * a_dim1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		} else {
		    i__2 = j + j * a_dim1;
		    zlarnd_(&z__1, &c__5, &iseed[1]);
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
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
	    b[1].r = 0., b[1].i = 0.;
	    for (i__ = *n; i__ >= 2; i__ += -2) {
		i__1 = i__;
		b[i__1].r = 0., b[i__1].i = 0.;
		i__1 = i__ - 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		b[i__1].r = z__1.r, b[i__1].i = z__1.i;
/* L240: */
	    }
	} else {
	    i__1 = *n;
	    b[i__1].r = 0., b[i__1].i = 0.;
	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; i__ += 2) {
		i__2 = i__;
		b[i__2].r = 0., b[i__2].i = 0.;
		i__2 = i__ + 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		b[i__2].r = z__1.r, b[i__2].i = z__1.i;
/* L250: */
	    }
	}

    } else if (*imat == 15) {

/*        Type 15:  Make the diagonal elements small to cause gradual */
/*        overflow when dividing by T(j,j).  To control the amount of */
/*        scaling needed, the matrix is bidiagonal. */

/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n - 1);
	texp = 1. / max(d__1,d__2);
	tscal = pow_dd(&smlnum, &texp);
	zlarnv_(&c__4, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 2;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0., a[i__3].i = 0.;
/* L260: */
		}
		if (j > 1) {
		    i__2 = j - 1 + j * a_dim1;
		    a[i__2].r = -1., a[i__2].i = -1.;
		}
		i__2 = j + j * a_dim1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tscal * z__2.r, z__1.i = tscal * z__2.i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L270: */
	    }
	    i__1 = *n;
	    b[i__1].r = 1., b[i__1].i = 1.;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 2; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0., a[i__3].i = 0.;
/* L280: */
		}
		if (j < *n) {
		    i__2 = j + 1 + j * a_dim1;
		    a[i__2].r = -1., a[i__2].i = -1.;
		}
		i__2 = j + j * a_dim1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tscal * z__2.r, z__1.i = tscal * z__2.i;
		a[i__2].r = z__1.r, a[i__2].i = z__1.i;
/* L290: */
	    }
	    b[1].r = 1., b[1].i = 1.;
	}

    } else if (*imat == 16) {

/*        Type 16:  One zero diagonal element. */

	iy = *n / 2 + 1;
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		if (j != iy) {
		    i__2 = j + j * a_dim1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		} else {
		    i__2 = j + j * a_dim1;
		    a[i__2].r = 0., a[i__2].i = 0.;
		}
/* L300: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    zlarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		if (j != iy) {
		    i__2 = j + j * a_dim1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		} else {
		    i__2 = j + j * a_dim1;
		    a[i__2].r = 0., a[i__2].i = 0.;
		}
/* L310: */
	    }
	}
	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	zdscal_(n, &c_b92, &b[1], &c__1);

    } else if (*imat == 17) {

/*        Type 17:  Make the offdiagonal elements large to cause overflow */
/*        when adding a column of T.  In the non-transposed case, the */
/*        matrix is constructed to cause overflow when adding a column in */
/*        every other step. */

	tscal = unfl / ulp;
	tscal = (1. - ulp) / tscal;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * a_dim1;
		a[i__3].r = 0., a[i__3].i = 0.;
/* L320: */
	    }
/* L330: */
	}
	texp = 1.;
	if (upper) {
	    for (j = *n; j >= 2; j += -2) {
		i__1 = j * a_dim1 + 1;
		d__1 = -tscal / (doublereal) (*n + 1);
		a[i__1].r = d__1, a[i__1].i = 0.;
		i__1 = j + j * a_dim1;
		a[i__1].r = 1., a[i__1].i = 0.;
		i__1 = j;
		d__1 = texp * (1. - ulp);
		b[i__1].r = d__1, b[i__1].i = 0.;
		i__1 = (j - 1) * a_dim1 + 1;
		d__1 = -(tscal / (doublereal) (*n + 1)) / (doublereal) (*n + 
			2);
		a[i__1].r = d__1, a[i__1].i = 0.;
		i__1 = j - 1 + (j - 1) * a_dim1;
		a[i__1].r = 1., a[i__1].i = 0.;
		i__1 = j - 1;
		d__1 = texp * (doublereal) (*n * *n + *n - 1);
		b[i__1].r = d__1, b[i__1].i = 0.;
		texp *= 2.;
/* L340: */
	    }
	    d__1 = (doublereal) (*n + 1) / (doublereal) (*n + 2) * tscal;
	    b[1].r = d__1, b[1].i = 0.;
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; j += 2) {
		i__2 = *n + j * a_dim1;
		d__1 = -tscal / (doublereal) (*n + 1);
		a[i__2].r = d__1, a[i__2].i = 0.;
		i__2 = j + j * a_dim1;
		a[i__2].r = 1., a[i__2].i = 0.;
		i__2 = j;
		d__1 = texp * (1. - ulp);
		b[i__2].r = d__1, b[i__2].i = 0.;
		i__2 = *n + (j + 1) * a_dim1;
		d__1 = -(tscal / (doublereal) (*n + 1)) / (doublereal) (*n + 
			2);
		a[i__2].r = d__1, a[i__2].i = 0.;
		i__2 = j + 1 + (j + 1) * a_dim1;
		a[i__2].r = 1., a[i__2].i = 0.;
		i__2 = j + 1;
		d__1 = texp * (doublereal) (*n * *n + *n - 1);
		b[i__2].r = d__1, b[i__2].i = 0.;
		texp *= 2.;
/* L350: */
	    }
	    i__1 = *n;
	    d__1 = (doublereal) (*n + 1) / (doublereal) (*n + 2) * tscal;
	    b[i__1].r = d__1, b[i__1].i = 0.;
	}

    } else if (*imat == 18) {

/*        Type 18:  Generate a unit triangular matrix with elements */
/*        between -1 and 1, and make the right hand side large so that it */
/*        requires scaling. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		i__2 = j + j * a_dim1;
		a[i__2].r = 0., a[i__2].i = 0.;
/* L360: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    zlarnv_(&c__4, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		i__2 = j + j * a_dim1;
		a[i__2].r = 0., a[i__2].i = 0.;
/* L370: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = izamax_(n, &b[1], &c__1);
	bnorm = z_abs(&b[iy]);
	bscal = bignum / max(1.,bnorm);
	zdscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 19) {

/*        Type 19:  Generate a triangular matrix with elements between */
/*        BIGNUM/(n-1) and BIGNUM so that at least one of the column */
/*        norms will exceed BIGNUM. */
/*        1/3/91:  ZLATRS no longer can handle this case */

/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n - 1);
	tleft = bignum / max(d__1,d__2);
/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n);
	tscal = bignum * ((doublereal) (*n - 1) / max(d__1,d__2));
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		zlarnv_(&c__5, &iseed[1], &j, &a[j * a_dim1 + 1]);
		dlarnv_(&c__1, &iseed[1], &j, &rwork[1]);
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    i__4 = i__ + j * a_dim1;
		    d__1 = tleft + rwork[i__] * tscal;
		    z__1.r = d__1 * a[i__4].r, z__1.i = d__1 * a[i__4].i;
		    a[i__3].r = z__1.r, a[i__3].i = z__1.i;
/* L380: */
		}
/* L390: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		zlarnv_(&c__5, &iseed[1], &i__2, &a[j + j * a_dim1]);
		i__2 = *n - j + 1;
		dlarnv_(&c__1, &iseed[1], &i__2, &rwork[1]);
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    i__3 = i__ + j * a_dim1;
		    i__4 = i__ + j * a_dim1;
		    d__1 = tleft + rwork[i__ - j + 1] * tscal;
		    z__1.r = d__1 * a[i__4].r, z__1.i = d__1 * a[i__4].i;
		    a[i__3].r = z__1.r, a[i__3].i = z__1.i;
/* L400: */
		}
/* L410: */
	    }
	}
	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	zdscal_(n, &c_b92, &b[1], &c__1);
    }

/*     Flip the matrix if the transpose will be used. */

    if (! lsame_(trans, "N")) {
	if (upper) {
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - (j << 1) + 1;
		zswap_(&i__2, &a[j + j * a_dim1], lda, &a[j + 1 + (*n - j + 1)
			 * a_dim1], &c_n1);
/* L420: */
	    }
	} else {
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - (j << 1) + 1;
		i__3 = -(*lda);
		zswap_(&i__2, &a[j + j * a_dim1], &c__1, &a[*n - j + 1 + (j + 
			1) * a_dim1], &i__3);
/* L430: */
	    }
	}
    }

    return 0;

/*     End of ZLATTR */

} /* zlattr_ */
