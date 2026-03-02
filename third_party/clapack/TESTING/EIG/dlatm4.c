#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b3 = 0.;

/* Subroutine */ int dlatm4_(integer *itype, integer *n, integer *nz1, 
	integer *nz2, integer *isign, doublereal *amagn, doublereal *rcond, 
	doublereal *triang, integer *idist, integer *iseed, doublereal *a, 
	integer *lda)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *), log(doublereal), exp(
	    doublereal), sqrt(doublereal);

    /* Local variables */
    integer i__, k, jc, jd;
    doublereal cl, cr;
    integer jr;
    doublereal sl, sr, sv1, sv2;
    integer kbeg, isdb, kend, ioff, isde, klen;
    doublereal temp, alpha;
    extern doublereal dlamch_(char *), dlaran_(integer *), dlarnd_(
	    integer *, integer *);
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *);
    doublereal safmin;


/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATM4 generates basic square matrices, which may later be */
/*  multiplied by others in order to produce test matrices.  It is */
/*  intended mainly to be used to test the generalized eigenvalue */
/*  routines. */

/*  It first generates the diagonal and (possibly) subdiagonal, */
/*  according to the value of ITYPE, NZ1, NZ2, ISIGN, AMAGN, and RCOND. */
/*  It then fills in the upper triangle with random numbers, if TRIANG is */
/*  non-zero. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          The "type" of matrix on the diagonal and sub-diagonal. */
/*          If ITYPE < 0, then type abs(ITYPE) is generated and then */
/*             swapped end for end (A(I,J) := A'(N-J,N-I).)  See also */
/*             the description of AMAGN and ISIGN. */

/*          Special types: */
/*          = 0:  the zero matrix. */
/*          = 1:  the identity. */
/*          = 2:  a transposed Jordan block. */
/*          = 3:  If N is odd, then a k+1 x k+1 transposed Jordan block */
/*                followed by a k x k identity block, where k=(N-1)/2. */
/*                If N is even, then k=(N-2)/2, and a zero diagonal entry */
/*                is tacked onto the end. */

/*          Diagonal types.  The diagonal consists of NZ1 zeros, then */
/*             k=N-NZ1-NZ2 nonzeros.  The subdiagonal is zero.  ITYPE */
/*             specifies the nonzero diagonal entries as follows: */
/*          = 4:  1, ..., k */
/*          = 5:  1, RCOND, ..., RCOND */
/*          = 6:  1, ..., 1, RCOND */
/*          = 7:  1, a, a^2, ..., a^(k-1)=RCOND */
/*          = 8:  1, 1-d, 1-2*d, ..., 1-(k-1)*d=RCOND */
/*          = 9:  random numbers chosen from (RCOND,1) */
/*          = 10: random numbers with distribution IDIST (see DLARND.) */

/*  N       (input) INTEGER */
/*          The order of the matrix. */

/*  NZ1     (input) INTEGER */
/*          If abs(ITYPE) > 3, then the first NZ1 diagonal entries will */
/*          be zero. */

/*  NZ2     (input) INTEGER */
/*          If abs(ITYPE) > 3, then the last NZ2 diagonal entries will */
/*          be zero. */

/*  ISIGN   (input) INTEGER */
/*          = 0: The sign of the diagonal and subdiagonal entries will */
/*               be left unchanged. */
/*          = 1: The diagonal and subdiagonal entries will have their */
/*               sign changed at random. */
/*          = 2: If ITYPE is 2 or 3, then the same as ISIGN=1. */
/*               Otherwise, with probability 0.5, odd-even pairs of */
/*               diagonal entries A(2*j-1,2*j-1), A(2*j,2*j) will be */
/*               converted to a 2x2 block by pre- and post-multiplying */
/*               by distinct random orthogonal rotations.  The remaining */
/*               diagonal entries will have their sign changed at random. */

/*  AMAGN   (input) DOUBLE PRECISION */
/*          The diagonal and subdiagonal entries will be multiplied by */
/*          AMAGN. */

/*  RCOND   (input) DOUBLE PRECISION */
/*          If abs(ITYPE) > 4, then the smallest diagonal entry will be */
/*          entry will be RCOND.  RCOND must be between 0 and 1. */

/*  TRIANG  (input) DOUBLE PRECISION */
/*          The entries above the diagonal will be random numbers with */
/*          magnitude bounded by TRIANG (i.e., random numbers multiplied */
/*          by TRIANG.) */

/*  IDIST   (input) INTEGER */
/*          Specifies the type of distribution to be used to generate a */
/*          random matrix. */
/*          = 1:  UNIFORM( 0, 1 ) */
/*          = 2:  UNIFORM( -1, 1 ) */
/*          = 3:  NORMAL ( 0, 1 ) */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry ISEED specifies the seed of the random number */
/*          generator.  The values of ISEED are changed on exit, and can */
/*          be used in the next call to DLATM4 to continue the same */
/*          random number sequence. */
/*          Note: ISEED(4) should be odd, for the random number generator */
/*          used at present. */

/*  A       (output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          Array to be computed. */

/*  LDA     (input) INTEGER */
/*          Leading dimension of A.  Must be at least 1 and at least N. */

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

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    dlaset_("Full", n, n, &c_b3, &c_b3, &a[a_offset], lda);

/*     Insure a correct ISEED */

    if (iseed[4] % 2 != 1) {
	++iseed[4];
    }

/*     Compute diagonal and subdiagonal according to ITYPE, NZ1, NZ2, */
/*     and RCOND */

    if (*itype != 0) {
	if (abs(*itype) >= 4) {
/* Computing MAX */
/* Computing MIN */
	    i__3 = *n, i__4 = *nz1 + 1;
	    i__1 = 1, i__2 = min(i__3,i__4);
	    kbeg = max(i__1,i__2);
/* Computing MAX */
/* Computing MIN */
	    i__3 = *n, i__4 = *n - *nz2;
	    i__1 = kbeg, i__2 = min(i__3,i__4);
	    kend = max(i__1,i__2);
	    klen = kend + 1 - kbeg;
	} else {
	    kbeg = 1;
	    kend = *n;
	    klen = *n;
	}
	isdb = 1;
	isde = 0;
	switch (abs(*itype)) {
	    case 1:  goto L10;
	    case 2:  goto L30;
	    case 3:  goto L50;
	    case 4:  goto L80;
	    case 5:  goto L100;
	    case 6:  goto L120;
	    case 7:  goto L140;
	    case 8:  goto L160;
	    case 9:  goto L180;
	    case 10:  goto L200;
	}

/*        abs(ITYPE) = 1: Identity */

L10:
	i__1 = *n;
	for (jd = 1; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = 1.;
/* L20: */
	}
	goto L220;

/*        abs(ITYPE) = 2: Transposed Jordan block */

L30:
	i__1 = *n - 1;
	for (jd = 1; jd <= i__1; ++jd) {
	    a[jd + 1 + jd * a_dim1] = 1.;
/* L40: */
	}
	isdb = 1;
	isde = *n - 1;
	goto L220;

/*        abs(ITYPE) = 3: Transposed Jordan block, followed by the */
/*                        identity. */

L50:
	k = (*n - 1) / 2;
	i__1 = k;
	for (jd = 1; jd <= i__1; ++jd) {
	    a[jd + 1 + jd * a_dim1] = 1.;
/* L60: */
	}
	isdb = 1;
	isde = k;
	i__1 = (k << 1) + 1;
	for (jd = k + 2; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = 1.;
/* L70: */
	}
	goto L220;

/*        abs(ITYPE) = 4: 1,...,k */

L80:
	i__1 = kend;
	for (jd = kbeg; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = (doublereal) (jd - *nz1);
/* L90: */
	}
	goto L220;

/*        abs(ITYPE) = 5: One large D value: */

L100:
	i__1 = kend;
	for (jd = kbeg + 1; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = *rcond;
/* L110: */
	}
	a[kbeg + kbeg * a_dim1] = 1.;
	goto L220;

/*        abs(ITYPE) = 6: One small D value: */

L120:
	i__1 = kend - 1;
	for (jd = kbeg; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = 1.;
/* L130: */
	}
	a[kend + kend * a_dim1] = *rcond;
	goto L220;

/*        abs(ITYPE) = 7: Exponentially distributed D values: */

L140:
	a[kbeg + kbeg * a_dim1] = 1.;
	if (klen > 1) {
	    d__1 = 1. / (doublereal) (klen - 1);
	    alpha = pow_dd(rcond, &d__1);
	    i__1 = klen;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		d__1 = (doublereal) (i__ - 1);
		a[*nz1 + i__ + (*nz1 + i__) * a_dim1] = pow_dd(&alpha, &d__1);
/* L150: */
	    }
	}
	goto L220;

/*        abs(ITYPE) = 8: Arithmetically distributed D values: */

L160:
	a[kbeg + kbeg * a_dim1] = 1.;
	if (klen > 1) {
	    alpha = (1. - *rcond) / (doublereal) (klen - 1);
	    i__1 = klen;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		a[*nz1 + i__ + (*nz1 + i__) * a_dim1] = (doublereal) (klen - 
			i__) * alpha + *rcond;
/* L170: */
	    }
	}
	goto L220;

/*        abs(ITYPE) = 9: Randomly distributed D values on ( RCOND, 1): */

L180:
	alpha = log(*rcond);
	i__1 = kend;
	for (jd = kbeg; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = exp(alpha * dlaran_(&iseed[1]));
/* L190: */
	}
	goto L220;

/*        abs(ITYPE) = 10: Randomly distributed D values from DIST */

L200:
	i__1 = kend;
	for (jd = kbeg; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = dlarnd_(idist, &iseed[1]);
/* L210: */
	}

L220:

/*        Scale by AMAGN */

	i__1 = kend;
	for (jd = kbeg; jd <= i__1; ++jd) {
	    a[jd + jd * a_dim1] = *amagn * a[jd + jd * a_dim1];
/* L230: */
	}
	i__1 = isde;
	for (jd = isdb; jd <= i__1; ++jd) {
	    a[jd + 1 + jd * a_dim1] = *amagn * a[jd + 1 + jd * a_dim1];
/* L240: */
	}

/*        If ISIGN = 1 or 2, assign random signs to diagonal and */
/*        subdiagonal */

	if (*isign > 0) {
	    i__1 = kend;
	    for (jd = kbeg; jd <= i__1; ++jd) {
		if (a[jd + jd * a_dim1] != 0.) {
		    if (dlaran_(&iseed[1]) > .5) {
			a[jd + jd * a_dim1] = -a[jd + jd * a_dim1];
		    }
		}
/* L250: */
	    }
	    i__1 = isde;
	    for (jd = isdb; jd <= i__1; ++jd) {
		if (a[jd + 1 + jd * a_dim1] != 0.) {
		    if (dlaran_(&iseed[1]) > .5) {
			a[jd + 1 + jd * a_dim1] = -a[jd + 1 + jd * a_dim1];
		    }
		}
/* L260: */
	    }
	}

/*        Reverse if ITYPE < 0 */

	if (*itype < 0) {
	    i__1 = (kbeg + kend - 1) / 2;
	    for (jd = kbeg; jd <= i__1; ++jd) {
		temp = a[jd + jd * a_dim1];
		a[jd + jd * a_dim1] = a[kbeg + kend - jd + (kbeg + kend - jd) 
			* a_dim1];
		a[kbeg + kend - jd + (kbeg + kend - jd) * a_dim1] = temp;
/* L270: */
	    }
	    i__1 = (*n - 1) / 2;
	    for (jd = 1; jd <= i__1; ++jd) {
		temp = a[jd + 1 + jd * a_dim1];
		a[jd + 1 + jd * a_dim1] = a[*n + 1 - jd + (*n - jd) * a_dim1];
		a[*n + 1 - jd + (*n - jd) * a_dim1] = temp;
/* L280: */
	    }
	}

/*        If ISIGN = 2, and no subdiagonals already, then apply */
/*        random rotations to make 2x2 blocks. */

	if (*isign == 2 && *itype != 2 && *itype != 3) {
	    safmin = dlamch_("S");
	    i__1 = kend - 1;
	    for (jd = kbeg; jd <= i__1; jd += 2) {
		if (dlaran_(&iseed[1]) > .5) {

/*                 Rotation on left. */

		    cl = dlaran_(&iseed[1]) * 2. - 1.;
		    sl = dlaran_(&iseed[1]) * 2. - 1.;
/* Computing MAX */
/* Computing 2nd power */
		    d__3 = cl;
/* Computing 2nd power */
		    d__4 = sl;
		    d__1 = safmin, d__2 = sqrt(d__3 * d__3 + d__4 * d__4);
		    temp = 1. / max(d__1,d__2);
		    cl *= temp;
		    sl *= temp;

/*                 Rotation on right. */

		    cr = dlaran_(&iseed[1]) * 2. - 1.;
		    sr = dlaran_(&iseed[1]) * 2. - 1.;
/* Computing MAX */
/* Computing 2nd power */
		    d__3 = cr;
/* Computing 2nd power */
		    d__4 = sr;
		    d__1 = safmin, d__2 = sqrt(d__3 * d__3 + d__4 * d__4);
		    temp = 1. / max(d__1,d__2);
		    cr *= temp;
		    sr *= temp;

/*                 Apply */

		    sv1 = a[jd + jd * a_dim1];
		    sv2 = a[jd + 1 + (jd + 1) * a_dim1];
		    a[jd + jd * a_dim1] = cl * cr * sv1 + sl * sr * sv2;
		    a[jd + 1 + jd * a_dim1] = -sl * cr * sv1 + cl * sr * sv2;
		    a[jd + (jd + 1) * a_dim1] = -cl * sr * sv1 + sl * cr * 
			    sv2;
		    a[jd + 1 + (jd + 1) * a_dim1] = sl * sr * sv1 + cl * cr * 
			    sv2;
		}
/* L290: */
	    }
	}

    }

/*     Fill in upper triangle (except for 2x2 blocks) */

    if (*triang != 0.) {
	if (*isign != 2 || *itype == 2 || *itype == 3) {
	    ioff = 1;
	} else {
	    ioff = 2;
	    i__1 = *n - 1;
	    for (jr = 1; jr <= i__1; ++jr) {
		if (a[jr + 1 + jr * a_dim1] == 0.) {
		    a[jr + (jr + 1) * a_dim1] = *triang * dlarnd_(idist, &
			    iseed[1]);
		}
/* L300: */
	    }
	}

	i__1 = *n;
	for (jc = 2; jc <= i__1; ++jc) {
	    i__2 = jc - ioff;
	    for (jr = 1; jr <= i__2; ++jr) {
		a[jr + jc * a_dim1] = *triang * dlarnd_(idist, &iseed[1]);
/* L310: */
	    }
/* L320: */
	}
    }

    return 0;

/*     End of DLATM4 */

} /* dlatm4_ */
