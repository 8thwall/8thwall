#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__5 = 5;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__4 = 4;
static doublereal c_b93 = 2.;

/* Subroutine */ int zlattp_(integer *imat, char *uplo, char *trans, char *
	diag, integer *iseed, integer *n, doublecomplex *ap, doublecomplex *b, 
	 doublecomplex *work, doublereal *rwork, integer *info)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2;
    doublecomplex z__1, z__2, z__3, z__4, z__5;

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
    doublereal t, x, y, z__;
    integer jc;
    doublecomplex ra;
    integer jj;
    doublecomplex rb;
    integer jl, kl, jr, ku, iy, jx;
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
    doublereal tscal;
    doublecomplex ctemp;
    doublereal anorm, bnorm, tleft;
    logical upper;
    extern /* Subroutine */ int zrotg_(doublecomplex *, doublecomplex *, 
	    doublereal *, doublecomplex *), zlatb4_(char *, integer *, 
	    integer *, integer *, char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, char *), dlabad_(
	    doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    char packit[1];
    extern /* Subroutine */ int zdscal_(integer *, doublereal *, 
	    doublecomplex *, integer *);
    doublereal bignum, cndnum;
    extern /* Subroutine */ int dlarnv_(integer *, integer *, integer *, 
	    doublereal *);
    extern integer izamax_(integer *, doublecomplex *, integer *);
    extern /* Double Complex */ VOID zlarnd_(doublecomplex *, integer *, 
	    integer *);
    integer jcnext, jcount;
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

/*  ZLATTP generates a triangular test matrix in packed storage. */
/*  IMAT and UPLO uniquely specify the properties of the test matrix, */
/*  which is returned in the array AP. */

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

/*  AP      (output) COMPLEX*16 array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP((j-1)*j/2 + i) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', */
/*             AP((j-1)*(n-j) + j*(j+1)/2 + i-j) = A(i,j) for j<=i<=n. */

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
    --rwork;
    --work;
    --b;
    --ap;
    --iseed;

    /* Function Body */
    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
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

/*     Call ZLATB4 to set parameters for CLATMS. */

    upper = lsame_(uplo, "U");
    if (upper) {
	zlatb4_(path, imat, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
	*(unsigned char *)packit = 'C';
    } else {
	i__1 = -(*imat);
	zlatb4_(path, &i__1, n, n, type__, &kl, &ku, &anorm, &mode, &cndnum, 
		dist);
	*(unsigned char *)packit = 'R';
    }

/*     IMAT <= 6:  Non-unit triangular matrix */

    if (*imat <= 6) {
	zlatms_(n, n, dist, &iseed[1], type__, &rwork[1], &mode, &cndnum, &
		anorm, &kl, &ku, packit, &ap[1], n, &work[1], info);

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
		    i__3 = jc + i__ - 1;
		    ap[i__3].r = 0., ap[i__3].i = 0.;
/* L10: */
		}
		i__2 = jc + j - 1;
		ap[i__2].r = (doublereal) j, ap[i__2].i = 0.;
		jc += j;
/* L20: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = jc;
		ap[i__2].r = (doublereal) j, ap[i__2].i = 0.;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    i__3 = jc + i__ - j;
		    ap[i__3].r = 0., ap[i__3].i = 0.;
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
		    i__3 = jc + i__;
		    ap[i__3].r = 0., ap[i__3].i = 0.;
/* L50: */
		}
		i__2 = jc + j;
		ap[i__2].r = (doublereal) j, ap[i__2].i = 0.;
		jc += j;
/* L60: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = jc;
		ap[i__2].r = (doublereal) j, ap[i__2].i = 0.;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    i__3 = jc + i__ - j;
		    ap[i__3].r = 0., ap[i__3].i = 0.;
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
		zlarnd_(&z__1, &c__2, &iseed[1]);
		rexp = z__1.r;
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
		i__2 = jc + 1;
		ap[i__2].r = y, ap[i__2].i = 0.;
		if (j > 2) {
		    i__2 = jc + j - 1;
		    i__3 = j - 2;
		    ap[i__2].r = work[i__3].r, ap[i__2].i = work[i__3].i;
		}
		if (j > 3) {
		    i__2 = jc + j - 2;
		    i__3 = *n + j - 3;
		    ap[i__2].r = work[i__3].r, ap[i__2].i = work[i__3].i;
		}
		jc += j;
/* L100: */
	    }
	    jc -= *n;
	    i__1 = jc + 1;
	    ap[i__1].r = z__, ap[i__1].i = 0.;
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = jc + j;
		ap[i__2].r = y, ap[i__2].i = 0.;
/* L110: */
	    }
	} else {

/*           Set the lower triangle of A with a unit triangular matrix */
/*           of known condition number. */

	    i__1 = *n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		i__2 = i__;
		ap[i__2].r = y, ap[i__2].i = 0.;
/* L120: */
	    }
	    i__1 = *n;
	    ap[i__1].r = z__, ap[i__1].i = 0.;
	    jc = *n + 1;
	    i__1 = *n - 1;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = jc + 1;
		i__3 = j - 1;
		ap[i__2].r = work[i__3].r, ap[i__2].i = work[i__3].i;
		if (j < *n - 1) {
		    i__2 = jc + 2;
		    i__3 = *n + j - 1;
		    ap[i__2].r = work[i__3].r, ap[i__2].i = work[i__3].i;
		}
		i__2 = jc + *n - j;
		ap[i__2].r = y, ap[i__2].i = 0.;
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
		i__2 = jcnext + j - 1;
		ra.r = ap[i__2].r, ra.i = ap[i__2].i;
		rb.r = 2., rb.i = 0.;
		zrotg_(&ra, &rb, &c__, &s);

/*              Multiply by [ c  s; -conjg(s)  c] on the left. */

		if (*n > j + 1) {
		    jx = jcnext + j;
		    i__2 = *n;
		    for (i__ = j + 2; i__ <= i__2; ++i__) {
			i__3 = jx + j;
			z__2.r = c__ * ap[i__3].r, z__2.i = c__ * ap[i__3].i;
			i__4 = jx + j + 1;
			z__3.r = s.r * ap[i__4].r - s.i * ap[i__4].i, z__3.i =
				 s.r * ap[i__4].i + s.i * ap[i__4].r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			ctemp.r = z__1.r, ctemp.i = z__1.i;
			i__3 = jx + j + 1;
			d_cnjg(&z__4, &s);
			z__3.r = -z__4.r, z__3.i = -z__4.i;
			i__4 = jx + j;
			z__2.r = z__3.r * ap[i__4].r - z__3.i * ap[i__4].i, 
				z__2.i = z__3.r * ap[i__4].i + z__3.i * ap[
				i__4].r;
			i__5 = jx + j + 1;
			z__5.r = c__ * ap[i__5].r, z__5.i = c__ * ap[i__5].i;
			z__1.r = z__2.r + z__5.r, z__1.i = z__2.i + z__5.i;
			ap[i__3].r = z__1.r, ap[i__3].i = z__1.i;
			i__3 = jx + j;
			ap[i__3].r = ctemp.r, ap[i__3].i = ctemp.i;
			jx += i__;
/* L140: */
		    }
		}

/*              Multiply by [-c -s;  conjg(s) -c] on the right. */

		if (j > 1) {
		    i__2 = j - 1;
		    d__1 = -c__;
		    z__1.r = -s.r, z__1.i = -s.i;
		    zrot_(&i__2, &ap[jcnext], &c__1, &ap[jc], &c__1, &d__1, &
			    z__1);
		}

/*              Negate A(J,J+1). */

		i__2 = jcnext + j - 1;
		i__3 = jcnext + j - 1;
		z__1.r = -ap[i__3].r, z__1.i = -ap[i__3].i;
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc = jcnext;
/* L150: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		jcnext = jc + *n - j + 1;
		i__2 = jc + 1;
		ra.r = ap[i__2].r, ra.i = ap[i__2].i;
		rb.r = 2., rb.i = 0.;
		zrotg_(&ra, &rb, &c__, &s);
		d_cnjg(&z__1, &s);
		s.r = z__1.r, s.i = z__1.i;

/*              Multiply by [ c -s;  conjg(s) c] on the right. */

		if (*n > j + 1) {
		    i__2 = *n - j - 1;
		    z__1.r = -s.r, z__1.i = -s.i;
		    zrot_(&i__2, &ap[jcnext + 1], &c__1, &ap[jc + 2], &c__1, &
			    c__, &z__1);
		}

/*              Multiply by [-c  s; -conjg(s) -c] on the left. */

		if (j > 1) {
		    jx = 1;
		    i__2 = j - 1;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			d__1 = -c__;
			i__3 = jx + j - i__;
			z__2.r = d__1 * ap[i__3].r, z__2.i = d__1 * ap[i__3]
				.i;
			i__4 = jx + j - i__ + 1;
			z__3.r = s.r * ap[i__4].r - s.i * ap[i__4].i, z__3.i =
				 s.r * ap[i__4].i + s.i * ap[i__4].r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			ctemp.r = z__1.r, ctemp.i = z__1.i;
			i__3 = jx + j - i__ + 1;
			d_cnjg(&z__4, &s);
			z__3.r = -z__4.r, z__3.i = -z__4.i;
			i__4 = jx + j - i__;
			z__2.r = z__3.r * ap[i__4].r - z__3.i * ap[i__4].i, 
				z__2.i = z__3.r * ap[i__4].i + z__3.i * ap[
				i__4].r;
			i__5 = jx + j - i__ + 1;
			z__5.r = c__ * ap[i__5].r, z__5.i = c__ * ap[i__5].i;
			z__1.r = z__2.r - z__5.r, z__1.i = z__2.i - z__5.i;
			ap[i__3].r = z__1.r, ap[i__3].i = z__1.i;
			i__3 = jx + j - i__;
			ap[i__3].r = ctemp.r, ap[i__3].i = ctemp.i;
			jx = jx + *n - i__ + 1;
/* L160: */
		    }
		}

/*              Negate A(J+1,J). */

		i__2 = jc + 1;
		i__3 = jc + 1;
		z__1.r = -ap[i__3].r, z__1.i = -ap[i__3].i;
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
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
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc]);
		i__2 = jc + j - 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc += j;
/* L180: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc + 1]);
		}
		i__2 = jc;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc = jc + *n - j + 1;
/* L190: */
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
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc]);
		i__2 = j - 1;
		zdscal_(&i__2, &tscal, &ap[jc], &c__1);
		i__2 = jc + j - 1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc += j;
/* L200: */
	    }
	    i__1 = *n * (*n + 1) / 2;
	    i__2 = *n * (*n + 1) / 2;
	    z__1.r = smlnum * ap[i__2].r, z__1.i = smlnum * ap[i__2].i;
	    ap[i__1].r = z__1.r, ap[i__1].i = z__1.i;
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		zlarnv_(&c__2, &iseed[1], &i__2, &ap[jc + 1]);
		i__2 = *n - j;
		zdscal_(&i__2, &tscal, &ap[jc + 1], &c__1);
		i__2 = jc;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc = jc + *n - j + 1;
/* L210: */
	    }
	    z__1.r = smlnum * ap[1].r, z__1.i = smlnum * ap[1].i;
	    ap[1].r = z__1.r, ap[1].i = z__1.i;
	}

    } else if (*imat == 13) {

/*        Type 13:  Make the first diagonal element in the solve small to */
/*        cause immediate overflow when dividing by T(j,j). */
/*        In type 13, the offdiagonal elements are O(1) (CNORM(j) > 1). */

	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc]);
		i__2 = jc + j - 1;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc += j;
/* L220: */
	    }
	    i__1 = *n * (*n + 1) / 2;
	    i__2 = *n * (*n + 1) / 2;
	    z__1.r = smlnum * ap[i__2].r, z__1.i = smlnum * ap[i__2].i;
	    ap[i__1].r = z__1.r, ap[i__1].i = z__1.i;
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc + 1]);
		i__2 = jc;
		zlarnd_(&z__1, &c__5, &iseed[1]);
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc = jc + *n - j + 1;
/* L230: */
	    }
	    z__1.r = smlnum * ap[1].r, z__1.i = smlnum * ap[1].i;
	    ap[1].r = z__1.r, ap[1].i = z__1.i;
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
		    i__2 = jc + i__ - 1;
		    ap[i__2].r = 0., ap[i__2].i = 0.;
/* L240: */
		}
		if (jcount <= 2) {
		    i__1 = jc + j - 1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		    ap[i__1].r = z__1.r, ap[i__1].i = z__1.i;
		} else {
		    i__1 = jc + j - 1;
		    zlarnd_(&z__1, &c__5, &iseed[1]);
		    ap[i__1].r = z__1.r, ap[i__1].i = z__1.i;
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
		    i__3 = jc + i__ - j;
		    ap[i__3].r = 0., ap[i__3].i = 0.;
/* L260: */
		}
		if (jcount <= 2) {
		    i__2 = jc;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		    ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		} else {
		    i__2 = jc;
		    zlarnd_(&z__1, &c__5, &iseed[1]);
		    ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
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
	    b[1].r = 0., b[1].i = 0.;
	    for (i__ = *n; i__ >= 2; i__ += -2) {
		i__1 = i__;
		b[i__1].r = 0., b[i__1].i = 0.;
		i__1 = i__ - 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = smlnum * z__2.r, z__1.i = smlnum * z__2.i;
		b[i__1].r = z__1.r, b[i__1].i = z__1.i;
/* L280: */
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
	zlarnv_(&c__4, &iseed[1], n, &b[1]);
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 2;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = jc + i__ - 1;
		    ap[i__3].r = 0., ap[i__3].i = 0.;
/* L300: */
		}
		if (j > 1) {
		    i__2 = jc + j - 2;
		    ap[i__2].r = -1., ap[i__2].i = -1.;
		}
		i__2 = jc + j - 1;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tscal * z__2.r, z__1.i = tscal * z__2.i;
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc += j;
/* L310: */
	    }
	    i__1 = *n;
	    b[i__1].r = 1., b[i__1].i = 1.;
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j + 2; i__ <= i__2; ++i__) {
		    i__3 = jc + i__ - j;
		    ap[i__3].r = 0., ap[i__3].i = 0.;
/* L320: */
		}
		if (j < *n) {
		    i__2 = jc + 1;
		    ap[i__2].r = -1., ap[i__2].i = -1.;
		}
		i__2 = jc;
		zlarnd_(&z__2, &c__5, &iseed[1]);
		z__1.r = tscal * z__2.r, z__1.i = tscal * z__2.i;
		ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		jc = jc + *n - j + 1;
/* L330: */
	    }
	    b[1].r = 1., b[1].i = 1.;
	}

    } else if (*imat == 16) {

/*        Type 16:  One zero diagonal element. */

	iy = *n / 2 + 1;
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		zlarnv_(&c__4, &iseed[1], &j, &ap[jc]);
		if (j != iy) {
		    i__2 = jc + j - 1;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		    ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		} else {
		    i__2 = jc + j - 1;
		    ap[i__2].r = 0., ap[i__2].i = 0.;
		}
		jc += j;
/* L340: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc]);
		if (j != iy) {
		    i__2 = jc;
		    zlarnd_(&z__2, &c__5, &iseed[1]);
		    z__1.r = z__2.r * 2., z__1.i = z__2.i * 2.;
		    ap[i__2].r = z__1.r, ap[i__2].i = z__1.i;
		} else {
		    i__2 = jc;
		    ap[i__2].r = 0., ap[i__2].i = 0.;
		}
		jc = jc + *n - j + 1;
/* L350: */
	    }
	}
	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	zdscal_(n, &c_b93, &b[1], &c__1);

    } else if (*imat == 17) {

/*        Type 17:  Make the offdiagonal elements large to cause overflow */
/*        when adding a column of T.  In the non-transposed case, the */
/*        matrix is constructed to cause overflow when adding a column in */
/*        every other step. */

	tscal = unfl / ulp;
	tscal = (1. - ulp) / tscal;
	i__1 = *n * (*n + 1) / 2;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    ap[i__2].r = 0., ap[i__2].i = 0.;
/* L360: */
	}
	texp = 1.;
	if (upper) {
	    jc = (*n - 1) * *n / 2 + 1;
	    for (j = *n; j >= 2; j += -2) {
		i__1 = jc;
		d__1 = -tscal / (doublereal) (*n + 1);
		ap[i__1].r = d__1, ap[i__1].i = 0.;
		i__1 = jc + j - 1;
		ap[i__1].r = 1., ap[i__1].i = 0.;
		i__1 = j;
		d__1 = texp * (1. - ulp);
		b[i__1].r = d__1, b[i__1].i = 0.;
		jc = jc - j + 1;
		i__1 = jc;
		d__1 = -(tscal / (doublereal) (*n + 1)) / (doublereal) (*n + 
			2);
		ap[i__1].r = d__1, ap[i__1].i = 0.;
		i__1 = jc + j - 2;
		ap[i__1].r = 1., ap[i__1].i = 0.;
		i__1 = j - 1;
		d__1 = texp * (doublereal) (*n * *n + *n - 1);
		b[i__1].r = d__1, b[i__1].i = 0.;
		texp *= 2.;
		jc = jc - j + 2;
/* L370: */
	    }
	    d__1 = (doublereal) (*n + 1) / (doublereal) (*n + 2) * tscal;
	    b[1].r = d__1, b[1].i = 0.;
	} else {
	    jc = 1;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; j += 2) {
		i__2 = jc + *n - j;
		d__1 = -tscal / (doublereal) (*n + 1);
		ap[i__2].r = d__1, ap[i__2].i = 0.;
		i__2 = jc;
		ap[i__2].r = 1., ap[i__2].i = 0.;
		i__2 = j;
		d__1 = texp * (1. - ulp);
		b[i__2].r = d__1, b[i__2].i = 0.;
		jc = jc + *n - j + 1;
		i__2 = jc + *n - j - 1;
		d__1 = -(tscal / (doublereal) (*n + 1)) / (doublereal) (*n + 
			2);
		ap[i__2].r = d__1, ap[i__2].i = 0.;
		i__2 = jc;
		ap[i__2].r = 1., ap[i__2].i = 0.;
		i__2 = j + 1;
		d__1 = texp * (doublereal) (*n * *n + *n - 1);
		b[i__2].r = d__1, b[i__2].i = 0.;
		texp *= 2.;
		jc = jc + *n - j;
/* L380: */
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
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc]);
		i__2 = jc + j - 1;
		ap[i__2].r = 0., ap[i__2].i = 0.;
		jc += j;
/* L390: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < *n) {
		    i__2 = *n - j;
		    zlarnv_(&c__4, &iseed[1], &i__2, &ap[jc + 1]);
		}
		i__2 = jc;
		ap[i__2].r = 0., ap[i__2].i = 0.;
		jc = jc + *n - j + 1;
/* L400: */
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
/*        1/3/91:  ZLATPS no longer can handle this case */

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
		zlarnv_(&c__5, &iseed[1], &j, &ap[jc]);
		dlarnv_(&c__1, &iseed[1], &j, &rwork[1]);
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    i__3 = jc + i__ - 1;
		    i__4 = jc + i__ - 1;
		    d__1 = tleft + rwork[i__] * tscal;
		    z__1.r = d__1 * ap[i__4].r, z__1.i = d__1 * ap[i__4].i;
		    ap[i__3].r = z__1.r, ap[i__3].i = z__1.i;
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
		zlarnv_(&c__5, &iseed[1], &i__2, &ap[jc]);
		i__2 = *n - j + 1;
		dlarnv_(&c__1, &iseed[1], &i__2, &rwork[1]);
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    i__3 = jc + i__ - j;
		    i__4 = jc + i__ - j;
		    d__1 = tleft + rwork[i__ - j + 1] * tscal;
		    z__1.r = d__1 * ap[i__4].r, z__1.i = d__1 * ap[i__4].i;
		    ap[i__3].r = z__1.r, ap[i__3].i = z__1.i;
/* L430: */
		}
		jc = jc + *n - j + 1;
/* L440: */
	    }
	}
	zlarnv_(&c__2, &iseed[1], n, &b[1]);
	zdscal_(n, &c_b93, &b[1], &c__1);
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
		    i__3 = jr - i__ + j;
		    t = ap[i__3].r;
		    i__3 = jr - i__ + j;
		    i__4 = jl;
		    ap[i__3].r = ap[i__4].r, ap[i__3].i = ap[i__4].i;
		    i__3 = jl;
		    ap[i__3].r = t, ap[i__3].i = 0.;
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
		    i__3 = jl + i__ - j;
		    t = ap[i__3].r;
		    i__3 = jl + i__ - j;
		    i__4 = jr;
		    ap[i__3].r = ap[i__4].r, ap[i__3].i = ap[i__4].i;
		    i__3 = jr;
		    ap[i__3].r = t, ap[i__3].i = 0.;
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

/*     End of ZLATTP */

} /* zlattp_ */
