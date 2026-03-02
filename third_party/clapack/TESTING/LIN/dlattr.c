#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__1 = 1;
static doublereal c_b35 = 2.;
static doublereal c_b46 = 1.;
static integer c_n1 = -1;

/* Subroutine */ int dlattr_(integer *imat, char *uplo, char *trans, char *
	diag, integer *iseed, integer *n, doublereal *a, integer *lda, 
	doublereal *b, doublereal *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    doublereal d__1, d__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal), d_sign(
	    doublereal *, doublereal *);

    /* Local variables */
    doublereal c__;
    integer i__, j;
    doublereal s, x, y, z__, ra, rb;
    integer kl, ku, iy;
    doublereal ulp, sfac;
    integer mode;
    char path[3], dist[1];
    doublereal unfl;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *);
    doublereal rexp;
    char type__[1];
    doublereal texp, star1, plus1, plus2, bscal;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *);
    extern logical lsame_(char *, char *);
    doublereal tscal, anorm, bnorm, tleft;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), drotg_(doublereal *, doublereal *, 
	    doublereal *, doublereal *), dswap_(integer *, doublereal *, 
	    integer *, doublereal *, integer *);
    logical upper;
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), dlabad_(doublereal 
	    *, doublereal *);
    extern doublereal dlamch_(char *);
    extern integer idamax_(integer *, doublereal *, integer *);
    extern doublereal dlarnd_(integer *, integer *);
    doublereal bignum, cndnum;
    extern /* Subroutine */ int dlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublereal *, integer *, doublereal 
	    *, integer *), dlarnv_(integer *, integer 
	    *, integer *, doublereal *);
    integer jcount;
    doublereal smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATTR generates a triangular test matrix. */
/*  IMAT and UPLO uniquely specify the properties of the test */
/*  matrix, which is returned in the array A. */

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
/*          = 'C':  Conjugate transpose (= Transpose) */

/*  DIAG    (output) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          The seed vector for the random number generator (used in */
/*          DLATMS).  Modified on exit. */

/*  N       (input) INTEGER */
/*          The order of the matrix to be generated. */

/*  A       (output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading n by n */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading n by n lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix, and the strictly upper triangular part of A is not */
/*          referenced.  If DIAG = 'U', the diagonal elements of A are */
/*          set so that A(k,k) = k for 1 <= k <= n. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (output) DOUBLE PRECISION array, dimension (N) */
/*          The right hand side vector, if IMAT > 10. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0: if INFO = -k, the k-th argument had an illegal value */

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

    /* Function Body */
    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
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

/*     Call DLATB4 to set parameters for SLATMS. */

    upper = lsame_(uplo, "U");
    if (upper) {
	dlatb4_(path, imat, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
    } else {
	i__1 = -(*imat);
	dlatb4_(path, &i__1, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
    }

/*     IMAT <= 6:  Non-unit triangular matrix */

    if (*imat <= 6) {
	dlatms_(n, n, dist, &iseed[1], type__, &b[1], &mode, &cndnum, &anorm, 
		&kl, &ku, "No packing", &a[a_offset], lda, &work[1], info);

/*     IMAT > 6:  Unit triangular matrix */
/*     The diagonal is deliberately set to something other than 1. */

/*     IMAT = 7:  Matrix is the identity */

    } else if (*imat == 7) {
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = 0.;
/* L10: */
		}
		a[j + j * a_dim1] = (doublereal) j;
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		a[j + j * a_dim1] = (doublereal) j;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = 0.;
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
		    a[i__ + j * a_dim1] = 0.;
/* L50: */
		}
		a[j + j * a_dim1] = (doublereal) j;
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		a[j + j * a_dim1] = (doublereal) j;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = 0.;
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

	star1 = .25;
	sfac = .5;
	plus1 = sfac;
	i__1 = *n;
	for (j = 1; j <= i__1; j += 2) {
	    plus2 = star1 / plus1;
	    work[j] = plus1;
	    work[*n + j] = star1;
	    if (j + 1 <= *n) {
		work[j + 1] = plus2;
		work[*n + j + 1] = 0.;
		plus1 = star1 / plus2;
		rexp = dlarnd_(&c__2, &iseed[1]);
		star1 *= pow_dd(&sfac, &rexp);
		if (rexp < 0.) {
		    d__1 = 1. - rexp;
		    star1 = -pow_dd(&sfac, &d__1);
		} else {
		    d__1 = rexp + 1.;
		    star1 = pow_dd(&sfac, &d__1);
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
		dcopy_(&i__1, &work[1], &c__1, &a[a_dim1 * 3 + 2], &i__2);
		if (*n > 4) {
		    i__1 = *n - 4;
		    i__2 = *lda + 1;
		    dcopy_(&i__1, &work[*n + 1], &c__1, &a[(a_dim1 << 2) + 2], 
			     &i__2);
		}
	    }
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		a[j * a_dim1 + 1] = y;
		a[j + *n * a_dim1] = y;
/* L100: */
	    }
	    a[*n * a_dim1 + 1] = z__;
	} else {
	    if (*n > 3) {
		i__1 = *n - 3;
		i__2 = *lda + 1;
		dcopy_(&i__1, &work[1], &c__1, &a[(a_dim1 << 1) + 3], &i__2);
		if (*n > 4) {
		    i__1 = *n - 4;
		    i__2 = *lda + 1;
		    dcopy_(&i__1, &work[*n + 1], &c__1, &a[(a_dim1 << 1) + 4], 
			     &i__2);
		}
	    }
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		a[j + a_dim1] = y;
		a[*n + j * a_dim1] = y;
/* L110: */
	    }
	    a[*n + a_dim1] = z__;
	}

/*        Fill in the zeros using Givens rotations. */

	if (upper) {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		ra = a[j + (j + 1) * a_dim1];
		rb = 2.;
		drotg_(&ra, &rb, &c__, &s);

/*              Multiply by [ c  s; -s  c] on the left. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    drot_(&i__2, &a[j + (j + 2) * a_dim1], lda, &a[j + 1 + (j 
			    + 2) * a_dim1], lda, &c__, &s);
		}

/*              Multiply by [-c -s;  s -c] on the right. */

		if (j > 1) {
		    i__2 = j - 1;
		    d__1 = -c__;
		    d__2 = -s;
		    drot_(&i__2, &a[(j + 1) * a_dim1 + 1], &c__1, &a[j * 
			    a_dim1 + 1], &c__1, &d__1, &d__2);
		}

/*              Negate A(J,J+1). */

		a[j + (j + 1) * a_dim1] = -a[j + (j + 1) * a_dim1];
/* L120: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		ra = a[j + 1 + j * a_dim1];
		rb = 2.;
		drotg_(&ra, &rb, &c__, &s);

/*              Multiply by [ c -s;  s  c] on the right. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    d__1 = -s;
		    drot_(&i__2, &a[j + 2 + (j + 1) * a_dim1], &c__1, &a[j + 
			    2 + j * a_dim1], &c__1, &c__, &d__1);
		}

/*              Multiply by [-c  s; -s -c] on the left. */

		if (j > 1) {
		    i__2 = j - 1;
		    d__1 = -c__;
		    drot_(&i__2, &a[j + a_dim1], lda, &a[j + 1 + a_dim1], lda, 
			     &d__1, &s);
		}

/*              Negate A(J+1,J). */

		a[j + 1 + j * a_dim1] = -a[j + 1 + j * a_dim1];
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
		dlarnv_(&c__2, &iseed[1], &j, &a[j * a_dim1 + 1]);
		a[j + j * a_dim1] = d_sign(&c_b35, &a[j + j * a_dim1]);
/* L140: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[j + j * a_dim1]);
		a[j + j * a_dim1] = d_sign(&c_b35, &a[j + j * a_dim1]);
/* L150: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = idamax_(n, &b[1], &c__1);
	bnorm = (d__1 = b[iy], abs(d__1));
	bscal = bignum / max(1.,bnorm);
	dscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 12) {

/*        Type 12:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 12, the offdiagonal elements are small (CNORM(j) < 1). */

	dlarnv_(&c__2, &iseed[1], n, &b[1]);
/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n - 1);
	tscal = 1. / max(d__1,d__2);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dlarnv_(&c__2, &iseed[1], &j, &a[j * a_dim1 + 1]);
		i__2 = j - 1;
		dscal_(&i__2, &tscal, &a[j * a_dim1 + 1], &c__1);
		a[j + j * a_dim1] = d_sign(&c_b46, &a[j + j * a_dim1]);
/* L160: */
	    }
	    a[*n + *n * a_dim1] = smlnum * a[*n + *n * a_dim1];
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[j + j * a_dim1]);
		if (*n > j) {
		    i__2 = *n - j;
		    dscal_(&i__2, &tscal, &a[j + 1 + j * a_dim1], &c__1);
		}
		a[j + j * a_dim1] = d_sign(&c_b46, &a[j + j * a_dim1]);
/* L170: */
	    }
	    a[a_dim1 + 1] = smlnum * a[a_dim1 + 1];
	}

    } else if (*imat == 13) {

/*        Type 13:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 13, the offdiagonal elements are O(1) (CNORM(j) > 1). */

	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dlarnv_(&c__2, &iseed[1], &j, &a[j * a_dim1 + 1]);
		a[j + j * a_dim1] = d_sign(&c_b46, &a[j + j * a_dim1]);
/* L180: */
	    }
	    a[*n + *n * a_dim1] = smlnum * a[*n + *n * a_dim1];
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[j + j * a_dim1]);
		a[j + j * a_dim1] = d_sign(&c_b46, &a[j + j * a_dim1]);
/* L190: */
	    }
	    a[a_dim1 + 1] = smlnum * a[a_dim1 + 1];
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
		    a[i__ + j * a_dim1] = 0.;
/* L200: */
		}
		if (jcount <= 2) {
		    a[j + j * a_dim1] = smlnum;
		} else {
		    a[j + j * a_dim1] = 1.;
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
		    a[i__ + j * a_dim1] = 0.;
/* L220: */
		}
		if (jcount <= 2) {
		    a[j + j * a_dim1] = smlnum;
		} else {
		    a[j + j * a_dim1] = 1.;
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
	    b[1] = 0.;
	    for (i__ = *n; i__ >= 2; i__ += -2) {
		b[i__] = 0.;
		b[i__ - 1] = smlnum;
/* L240: */
	    }
	} else {
	    b[*n] = 0.;
	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; i__ += 2) {
		b[i__] = 0.;
		b[i__ + 1] = smlnum;
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
	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 2;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = 0.;
/* L260: */
		}
		if (j > 1) {
		    a[j - 1 + j * a_dim1] = -1.;
		}
		a[j + j * a_dim1] = tscal;
/* L270: */
	    }
	    b[*n] = 1.;
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 2; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = 0.;
/* L280: */
		}
		if (j < *n) {
		    a[j + 1 + j * a_dim1] = -1.;
		}
		a[j + j * a_dim1] = tscal;
/* L290: */
	    }
	    b[1] = 1.;
	}

    } else if (*imat == 16) {

/*        Type 16:  One zero diagonal element. */

	iy = *n / 2 + 1;
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dlarnv_(&c__2, &iseed[1], &j, &a[j * a_dim1 + 1]);
		if (j != iy) {
		    a[j + j * a_dim1] = d_sign(&c_b35, &a[j + j * a_dim1]);
		} else {
		    a[j + j * a_dim1] = 0.;
		}
/* L300: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[j + j * a_dim1]);
		if (j != iy) {
		    a[j + j * a_dim1] = d_sign(&c_b35, &a[j + j * a_dim1]);
		} else {
		    a[j + j * a_dim1] = 0.;
		}
/* L310: */
	    }
	}
	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	dscal_(n, &c_b35, &b[1], &c__1);

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
		a[i__ + j * a_dim1] = 0.;
/* L320: */
	    }
/* L330: */
	}
	texp = 1.;
	if (upper) {
	    for (j = *n; j >= 2; j += -2) {
		a[j * a_dim1 + 1] = -tscal / (doublereal) (*n + 1);
		a[j + j * a_dim1] = 1.;
		b[j] = texp * (1. - ulp);
		a[(j - 1) * a_dim1 + 1] = -(tscal / (doublereal) (*n + 1)) / (
			doublereal) (*n + 2);
		a[j - 1 + (j - 1) * a_dim1] = 1.;
		b[j - 1] = texp * (doublereal) (*n * *n + *n - 1);
		texp *= 2.;
/* L340: */
	    }
	    b[1] = (doublereal) (*n + 1) / (doublereal) (*n + 2) * tscal;
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; j += 2) {
		a[*n + j * a_dim1] = -tscal / (doublereal) (*n + 1);
		a[j + j * a_dim1] = 1.;
		b[j] = texp * (1. - ulp);
		a[*n + (j + 1) * a_dim1] = -(tscal / (doublereal) (*n + 1)) / 
			(doublereal) (*n + 2);
		a[j + 1 + (j + 1) * a_dim1] = 1.;
		b[j + 1] = texp * (doublereal) (*n * *n + *n - 1);
		texp *= 2.;
/* L350: */
	    }
	    b[*n] = (doublereal) (*n + 1) / (doublereal) (*n + 2) * tscal;
	}

    } else if (*imat == 18) {

/*        Type 18:  Generate a unit triangular matrix with elements */
/*        between -1 and 1, and make the right hand side large so that it */
/*        requires scaling. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[j * a_dim1 + 1]);
		a[j + j * a_dim1] = 0.;
/* L360: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    dlarnv_(&c__2, &iseed[1], &i__2, &a[j + 1 + j * a_dim1]);
		}
		a[j + j * a_dim1] = 0.;
/* L370: */
	    }
	}

/*        Set the right hand side so that the largest value is BIGNUM. */

	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	iy = idamax_(n, &b[1], &c__1);
	bnorm = (d__1 = b[iy], abs(d__1));
	bscal = bignum / max(1.,bnorm);
	dscal_(n, &bscal, &b[1], &c__1);

    } else if (*imat == 19) {

/*        Type 19:  Generate a triangular matrix with elements between */
/*        BIGNUM/(n-1) and BIGNUM so that at least one of the column */
/*        norms will exceed BIGNUM. */
/*        1/3/91:  DLATRS no longer can handle this case */

/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n - 1);
	tleft = bignum / max(d__1,d__2);
/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n);
	tscal = bignum * ((doublereal) (*n - 1) / max(d__1,d__2));
	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dlarnv_(&c__2, &iseed[1], &j, &a[j * a_dim1 + 1]);
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = d_sign(&tleft, &a[i__ + j * a_dim1])
			     + tscal * a[i__ + j * a_dim1];
/* L380: */
		}
/* L390: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[j + j * a_dim1]);
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = d_sign(&tleft, &a[i__ + j * a_dim1])
			     + tscal * a[i__ + j * a_dim1];
/* L400: */
		}
/* L410: */
	    }
	}
	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	dscal_(n, &c_b35, &b[1], &c__1);
    }

/*     Flip the matrix if the transpose will be used. */

    if (! lsame_(trans, "N")) {
	if (upper) {
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - (j << 1) + 1;
		dswap_(&i__2, &a[j + j * a_dim1], lda, &a[j + 1 + (*n - j + 1)
			 * a_dim1], &c_n1);
/* L420: */
	    }
	} else {
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - (j << 1) + 1;
		i__3 = -(*lda);
		dswap_(&i__2, &a[j + j * a_dim1], &c__1, &a[*n - j + 1 + (j + 
			1) * a_dim1], &i__3);
/* L430: */
	    }
	}
    }

    return 0;

/*     End of DLATTR */

} /* dlattr_ */
