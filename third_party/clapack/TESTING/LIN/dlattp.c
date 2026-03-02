#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__1 = 1;
static doublereal c_b36 = 2.;
static doublereal c_b48 = 1.;

/* Subroutine */ int dlattp_(integer *imat, char *uplo, char *trans, char *
	diag, integer *iseed, integer *n, doublereal *a, doublereal *b, 
	doublereal *work, integer *info)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal), d_sign(
	    doublereal *, doublereal *);

    /* Local variables */
    doublereal c__;
    integer i__, j;
    doublereal s, t, x, y, z__;
    integer jc;
    doublereal ra;
    integer jj;
    doublereal rb;
    integer jl, kl, jr, ku, iy, jx;
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
    extern /* Subroutine */ int drotg_(doublereal *, doublereal *, doublereal 
	    *, doublereal *);
    doublereal stemp;
    logical upper;
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), dlabad_(doublereal 
	    *, doublereal *);
    extern doublereal dlamch_(char *);
    extern integer idamax_(integer *, doublereal *, integer *);
    extern doublereal dlarnd_(integer *, integer *);
    char packit[1];
    doublereal bignum, cndnum;
    extern /* Subroutine */ int dlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublereal *, integer *, doublereal 
	    *, integer *), dlarnv_(integer *, integer 
	    *, integer *, doublereal *);
    integer jcnext, jcount;
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

/*  DLATTP generates a triangular test matrix in packed storage. */
/*  IMAT and UPLO uniquely specify the properties of the test */
/*  matrix, which is returned in the array AP. */

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

/*  A       (output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP((j-1)*j/2 + i) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', */
/*             AP((j-1)*(n-j) + j*(j+1)/2 + i-j) = A(i,j) for j<=i<=n. */

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
    --work;
    --b;
    --a;
    --iseed;

    /* Function Body */
    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "TP", (ftnlen)2, (ftnlen)2);
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
	*(unsigned char *)packit = 'C';
    } else {
	i__1 = -(*imat);
	dlatb4_(path, &i__1, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
	*(unsigned char *)packit = 'R';
    }

/*     IMAT <= 6:  Non-unit triangular matrix */

    if (*imat <= 6) {
	dlatms_(n, n, dist, &iseed[1], type__, &b[1], &mode, &cndnum, &anorm, 
		&kl, &ku, packit, &a[1], n, &work[1], info);

/*     IMAT > 6:  Unit triangular matrix */
/*     The diagonal is deliberately set to something other than 1. */

/*     IMAT = 7:  Matrix is the identity */

    } else if (*imat == 7) {
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[jc + i__ - 1] = 0.;
/* L10: */
		}
		a[jc + j - 1] = (doublereal) j;
		jc += j;
/* L20: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		a[jc] = (doublereal) j;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    a[jc + i__ - j] = 0.;
/* L30: */
		}
		jc = jc + *n - j + 1;
/* L40: */
	    }
	}

/*     IMAT > 7:  Non-trivial unit triangular matrix */

/*     Generate a unit triangular matrix T with condition CNDNUM by */
/*     forming a triangular matrix with known singular values and */
/*     filling in the zero entries with Givens rotations. */

    } else if (*imat <= 10) {
	if (upper) {
	    jc = 0;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[jc + i__] = 0.;
/* L50: */
		}
		a[jc + j] = (doublereal) j;
		jc += j;
/* L60: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		a[jc] = (doublereal) j;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    a[jc + i__ - j] = 0.;
/* L70: */
		}
		jc = jc + *n - j + 1;
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

	x = sqrt(cndnum) - 1. / sqrt(cndnum);
	if (*n > 2) {
	    y = sqrt(2. / (doublereal) (*n - 2)) * x;
	} else {
	    y = 0.;
	}
	z__ = x * x;

	if (upper) {

/*           Set the upper triangle of A with a unit triangular matrix */
/*           of known condition number. */

	    jc = 1;
	    i__1 = *n;
	    for (j = 2; j <= i__1; ++j) {
		a[jc + 1] = y;
		if (j > 2) {
		    a[jc + j - 1] = work[j - 2];
		}
		if (j > 3) {
		    a[jc + j - 2] = work[*n + j - 3];
		}
		jc += j;
/* L100: */
	    }
	    jc -= *n;
	    a[jc + 1] = z__;
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		a[jc + j] = y;
/* L110: */
	    }
	} else {

/*           Set the lower triangle of A with a unit triangular matrix */
/*           of known condition number. */

	    i__1 = *n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		a[i__] = y;
/* L120: */
	    }
	    a[*n] = z__;
	    jc = *n + 1;
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		a[jc + 1] = work[j - 1];
		if (j < *n - 1) {
		    a[jc + 2] = work[*n + j - 1];
		}
		a[jc + *n - j] = y;
		jc = jc + *n - j + 1;
/* L130: */
	    }
	}

/*        Fill in the zeros using Givens rotations */

	if (upper) {
	    jc = 1;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		jcnext = jc + j;
		ra = a[jcnext + j - 1];
		rb = 2.;
		drotg_(&ra, &rb, &c__, &s);

/*              Multiply by [ c  s; -s  c] on the left. */

		if (*n > j + 1) {
		    jx = jcnext + j;
		    i__2 = *n;
		    for (i__ = j + 2; i__ <= i__2; ++i__) {
			stemp = c__ * a[jx + j] + s * a[jx + j + 1];
			a[jx + j + 1] = -s * a[jx + j] + c__ * a[jx + j + 1];
			a[jx + j] = stemp;
			jx += i__;
/* L140: */
		    }
		}

/*              Multiply by [-c -s;  s -c] on the right. */

		if (j > 1) {
		    i__2 = j - 1;
		    d__1 = -c__;
		    d__2 = -s;
		    drot_(&i__2, &a[jcnext], &c__1, &a[jc], &c__1, &d__1, &
			    d__2);
		}

/*              Negate A(J,J+1). */

		a[jcnext + j - 1] = -a[jcnext + j - 1];
		jc = jcnext;
/* L150: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		jcnext = jc + *n - j + 1;
		ra = a[jc + 1];
		rb = 2.;
		drotg_(&ra, &rb, &c__, &s);

/*              Multiply by [ c -s;  s  c] on the right. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    d__1 = -s;
		    drot_(&i__2, &a[jcnext + 1], &c__1, &a[jc + 2], &c__1, &
			    c__, &d__1);
		}

/*              Multiply by [-c  s; -s -c] on the left. */

		if (j > 1) {
		    jx = 1;
		    i__2 = j - 1;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			stemp = -c__ * a[jx + j - i__] + s * a[jx + j - i__ + 
				1];
			a[jx + j - i__ + 1] = -s * a[jx + j - i__] - c__ * a[
				jx + j - i__ + 1];
			a[jx + j - i__] = stemp;
			jx = jx + *n - i__ + 1;
/* L160: */
		    }
		}

/*              Negate A(J+1,J). */

		a[jc + 1] = -a[jc + 1];
		jc = jcnext;
/* L170: */
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
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dlarnv_(&c__2, &iseed[1], &j, &a[jc]);
		a[jc + j - 1] = d_sign(&c_b36, &a[jc + j - 1]);
		jc += j;
/* L180: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc]);
		a[jc] = d_sign(&c_b36, &a[jc]);
		jc = jc + *n - j + 1;
/* L190: */
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
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc]);
		i__2 = j - 1;
		dscal_(&i__2, &tscal, &a[jc], &c__1);
		d__1 = dlarnd_(&c__2, &iseed[1]);
		a[jc + j - 1] = d_sign(&c_b48, &d__1);
		jc += j;
/* L200: */
	    }
	    a[*n * (*n + 1) / 2] = smlnum;
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc + 1]);
		i__2 = *n - j;
		dscal_(&i__2, &tscal, &a[jc + 1], &c__1);
		d__1 = dlarnd_(&c__2, &iseed[1]);
		a[jc] = d_sign(&c_b48, &d__1);
		jc = jc + *n - j + 1;
/* L210: */
	    }
	    a[1] = smlnum;
	}

    } else if (*imat == 13) {

/*        Type 13:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 13, the offdiagonal elements are O(1) (CNORM(j) > 1). */

	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc]);
		d__1 = dlarnd_(&c__2, &iseed[1]);
		a[jc + j - 1] = d_sign(&c_b48, &d__1);
		jc += j;
/* L220: */
	    }
	    a[*n * (*n + 1) / 2] = smlnum;
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc + 1]);
		d__1 = dlarnd_(&c__2, &iseed[1]);
		a[jc] = d_sign(&c_b48, &d__1);
		jc = jc + *n - j + 1;
/* L230: */
	    }
	    a[1] = smlnum;
	}

    } else if (*imat == 14) {

/*        Type 14:  T is diagonal with small numbers on the diagonal to */
/*        make the growth factor underflow, but a small right hand side */
/*        chosen so that the solution does not overflow. */

	if (upper) {
	    jcount = 1;
	    jc = (*n - 1) * *n / 2 + 1;
	    for (j = *n; j >= 1; --j) {
		i__1 = j - 1;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    a[jc + i__ - 1] = 0.;
/* L240: */
		}
		if (jcount <= 2) {
		    a[jc + j - 1] = smlnum;
		} else {
		    a[jc + j - 1] = 1.;
		}
		++jcount;
		if (jcount > 4) {
		    jcount = 1;
		}
		jc = jc - j + 1;
/* L250: */
	    }
	} else {
	    jcount = 1;
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    a[jc + i__ - j] = 0.;
/* L260: */
		}
		if (jcount <= 2) {
		    a[jc] = smlnum;
		} else {
		    a[jc] = 1.;
		}
		++jcount;
		if (jcount > 4) {
		    jcount = 1;
		}
		jc = jc + *n - j + 1;
/* L270: */
	    }
	}

/*        Set the right hand side alternately zero and small. */

	if (upper) {
	    b[1] = 0.;
	    for (i__ = *n; i__ >= 2; i__ += -2) {
		b[i__] = 0.;
		b[i__ - 1] = smlnum;
/* L280: */
	    }
	} else {
	    b[*n] = 0.;
	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; i__ += 2) {
		b[i__] = 0.;
		b[i__ + 1] = smlnum;
/* L290: */
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
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 2;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[jc + i__ - 1] = 0.;
/* L300: */
		}
		if (j > 1) {
		    a[jc + j - 2] = -1.;
		}
		a[jc + j - 1] = tscal;
		jc += j;
/* L310: */
	    }
	    b[*n] = 1.;
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 2; i__ <= i__2; ++i__) {
		    a[jc + i__ - j] = 0.;
/* L320: */
		}
		if (j < *n) {
		    a[jc + 1] = -1.;
		}
		a[jc] = tscal;
		jc = jc + *n - j + 1;
/* L330: */
	    }
	    b[1] = 1.;
	}

    } else if (*imat == 16) {

/*        Type 16:  One zero diagonal element. */

	iy = *n / 2 + 1;
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dlarnv_(&c__2, &iseed[1], &j, &a[jc]);
		if (j != iy) {
		    a[jc + j - 1] = d_sign(&c_b36, &a[jc + j - 1]);
		} else {
		    a[jc + j - 1] = 0.;
		}
		jc += j;
/* L340: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc]);
		if (j != iy) {
		    a[jc] = d_sign(&c_b36, &a[jc]);
		} else {
		    a[jc] = 0.;
		}
		jc = jc + *n - j + 1;
/* L350: */
	    }
	}
	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	dscal_(n, &c_b36, &b[1], &c__1);

    } else if (*imat == 17) {

/*        Type 17:  Make the offdiagonal elements large to cause overflow */
/*        when adding a column of T.  In the non-transposed case, the */
/*        matrix is constructed to cause overflow when adding a column in */
/*        every other step. */

	tscal = unfl / ulp;
	tscal = (1. - ulp) / tscal;
	i__1 = *n * (*n + 1) / 2;
	for (j = 1; j <= i__1; ++j) {
	    a[j] = 0.;
/* L360: */
	}
	texp = 1.;
	if (upper) {
	    jc = (*n - 1) * *n / 2 + 1;
	    for (j = *n; j >= 2; j += -2) {
		a[jc] = -tscal / (doublereal) (*n + 1);
		a[jc + j - 1] = 1.;
		b[j] = texp * (1. - ulp);
		jc = jc - j + 1;
		a[jc] = -(tscal / (doublereal) (*n + 1)) / (doublereal) (*n + 
			2);
		a[jc + j - 2] = 1.;
		b[j - 1] = texp * (doublereal) (*n * *n + *n - 1);
		texp *= 2.;
		jc = jc - j + 2;
/* L370: */
	    }
	    b[1] = (doublereal) (*n + 1) / (doublereal) (*n + 2) * tscal;
	} else {
	    jc = 1;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; j += 2) {
		a[jc + *n - j] = -tscal / (doublereal) (*n + 1);
		a[jc] = 1.;
		b[j] = texp * (1. - ulp);
		jc = jc + *n - j + 1;
		a[jc + *n - j - 1] = -(tscal / (doublereal) (*n + 1)) / (
			doublereal) (*n + 2);
		a[jc] = 1.;
		b[j + 1] = texp * (doublereal) (*n * *n + *n - 1);
		texp *= 2.;
		jc = jc + *n - j;
/* L380: */
	    }
	    b[*n] = (doublereal) (*n + 1) / (doublereal) (*n + 2) * tscal;
	}

    } else if (*imat == 18) {

/*        Type 18:  Generate a unit triangular matrix with elements */
/*        between -1 and 1, and make the right hand side large so that it */
/*        requires scaling. */

	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc]);
		a[jc + j - 1] = 0.;
		jc += j;
/* L390: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    dlarnv_(&c__2, &iseed[1], &i__2, &a[jc + 1]);
		}
		a[jc] = 0.;
		jc = jc + *n - j + 1;
/* L400: */
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

/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n - 1);
	tleft = bignum / max(d__1,d__2);
/* Computing MAX */
	d__1 = 1., d__2 = (doublereal) (*n);
	tscal = bignum * ((doublereal) (*n - 1) / max(d__1,d__2));
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dlarnv_(&c__2, &iseed[1], &j, &a[jc]);
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[jc + i__ - 1] = d_sign(&tleft, &a[jc + i__ - 1]) + 
			    tscal * a[jc + i__ - 1];
/* L410: */
		}
		jc += j;
/* L420: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dlarnv_(&c__2, &iseed[1], &i__2, &a[jc]);
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    a[jc + i__ - j] = d_sign(&tleft, &a[jc + i__ - j]) + 
			    tscal * a[jc + i__ - j];
/* L430: */
		}
		jc = jc + *n - j + 1;
/* L440: */
	    }
	}
	dlarnv_(&c__2, &iseed[1], n, &b[1]);
	dscal_(n, &c_b36, &b[1], &c__1);
    }

/*     Flip the matrix across its counter-diagonal if the transpose will */
/*     be used. */

    if (! lsame_(trans, "N")) {
	if (upper) {
	    jj = 1;
	    jr = *n * (*n + 1) / 2;
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		jl = jj;
		i__2 = *n - j;
		for (i__ = j; i__ <= i__2; ++i__) {
		    t = a[jr - i__ + j];
		    a[jr - i__ + j] = a[jl];
		    a[jl] = t;
		    jl += i__;
/* L450: */
		}
		jj = jj + j + 1;
		jr -= *n - j + 1;
/* L460: */
	    }
	} else {
	    jl = 1;
	    jj = *n * (*n + 1) / 2;
	    i__1 = *n / 2;
	    for (j = 1; j <= i__1; ++j) {
		jr = jj;
		i__2 = *n - j;
		for (i__ = j; i__ <= i__2; ++i__) {
		    t = a[jl + i__ - j];
		    a[jl + i__ - j] = a[jr];
		    a[jr] = t;
		    jr -= i__;
/* L470: */
		}
		jl = jl + *n - j + 1;
		jj = jj - j - 1;
/* L480: */
	    }
	}
    }

    return 0;

/*     End of DLATTP */

} /* dlattp_ */
