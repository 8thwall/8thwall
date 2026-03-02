#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b29 = 1.f;
static real c_b30 = 0.f;
static real c_b33 = -1.f;

/* Subroutine */ int slatm5_(integer *prtype, integer *m, integer *n, real *a, 
	 integer *lda, real *b, integer *ldb, real *c__, integer *ldc, real *
	d__, integer *ldd, real *e, integer *lde, real *f, integer *ldf, real 
	*r__, integer *ldr, real *l, integer *ldl, real *alpha, integer *
	qblcka, integer *qblckb)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, d_dim1, 
	    d_offset, e_dim1, e_offset, f_dim1, f_offset, l_dim1, l_offset, 
	    r_dim1, r_offset, i__1, i__2;

    /* Builtin functions */
    double sin(doublereal);

    /* Local variables */
    integer i__, j, k;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real imeps, reeps;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLATM5 generates matrices involved in the Generalized Sylvester */
/*  equation: */

/*      A * R - L * B = C */
/*      D * R - L * E = F */

/*  They also satisfy (the diagonalization condition) */

/*   [ I -L ] ( [ A  -C ], [ D -F ] ) [ I  R ] = ( [ A    ], [ D    ] ) */
/*   [    I ] ( [     B ]  [    E ] ) [    I ]   ( [    B ]  [    E ] ) */


/*  Arguments */
/*  ========= */

/*  PRTYPE  (input) INTEGER */
/*          "Points" to a certian type of the matrices to generate */
/*          (see futher details). */

/*  M       (input) INTEGER */
/*          Specifies the order of A and D and the number of rows in */
/*          C, F,  R and L. */

/*  N       (input) INTEGER */
/*          Specifies the order of B and E and the number of columns in */
/*          C, F, R and L. */

/*  A       (output) REAL array, dimension (LDA, M). */
/*          On exit A M-by-M is initialized according to PRTYPE. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A. */

/*  B       (output) REAL array, dimension (LDB, N). */
/*          On exit B N-by-N is initialized according to PRTYPE. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B. */

/*  C       (output) REAL array, dimension (LDC, N). */
/*          On exit C M-by-N is initialized according to PRTYPE. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of C. */

/*  D       (output) REAL array, dimension (LDD, M). */
/*          On exit D M-by-M is initialized according to PRTYPE. */

/*  LDD     (input) INTEGER */
/*          The leading dimension of D. */

/*  E       (output) REAL array, dimension (LDE, N). */
/*          On exit E N-by-N is initialized according to PRTYPE. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of E. */

/*  F       (output) REAL array, dimension (LDF, N). */
/*          On exit F M-by-N is initialized according to PRTYPE. */

/*  LDF     (input) INTEGER */
/*          The leading dimension of F. */

/*  R       (output) REAL array, dimension (LDR, N). */
/*          On exit R M-by-N is initialized according to PRTYPE. */

/*  LDR     (input) INTEGER */
/*          The leading dimension of R. */

/*  L       (output) REAL array, dimension (LDL, N). */
/*          On exit L M-by-N is initialized according to PRTYPE. */

/*  LDL     (input) INTEGER */
/*          The leading dimension of L. */

/*  ALPHA   (input) REAL */
/*          Parameter used in generating PRTYPE = 1 and 5 matrices. */

/*  QBLCKA  (input) INTEGER */
/*          When PRTYPE = 3, specifies the distance between 2-by-2 */
/*          blocks on the diagonal in A. Otherwise, QBLCKA is not */
/*          referenced. QBLCKA > 1. */

/*  QBLCKB  (input) INTEGER */
/*          When PRTYPE = 3, specifies the distance between 2-by-2 */
/*          blocks on the diagonal in B. Otherwise, QBLCKB is not */
/*          referenced. QBLCKB > 1. */


/*  Further Details */
/*  =============== */

/*  PRTYPE = 1: A and B are Jordan blocks, D and E are identity matrices */

/*             A : if (i == j) then A(i, j) = 1.0 */
/*                 if (j == i + 1) then A(i, j) = -1.0 */
/*                 else A(i, j) = 0.0,            i, j = 1...M */

/*             B : if (i == j) then B(i, j) = 1.0 - ALPHA */
/*                 if (j == i + 1) then B(i, j) = 1.0 */
/*                 else B(i, j) = 0.0,            i, j = 1...N */

/*             D : if (i == j) then D(i, j) = 1.0 */
/*                 else D(i, j) = 0.0,            i, j = 1...M */

/*             E : if (i == j) then E(i, j) = 1.0 */
/*                 else E(i, j) = 0.0,            i, j = 1...N */

/*             L =  R are chosen from [-10...10], */
/*                  which specifies the right hand sides (C, F). */

/*  PRTYPE = 2 or 3: Triangular and/or quasi- triangular. */

/*             A : if (i <= j) then A(i, j) = [-1...1] */
/*                 else A(i, j) = 0.0,             i, j = 1...M */

/*                 if (PRTYPE = 3) then */
/*                    A(k + 1, k + 1) = A(k, k) */
/*                    A(k + 1, k) = [-1...1] */
/*                    sign(A(k, k + 1) = -(sin(A(k + 1, k)) */
/*                        k = 1, M - 1, QBLCKA */

/*             B : if (i <= j) then B(i, j) = [-1...1] */
/*                 else B(i, j) = 0.0,            i, j = 1...N */

/*                 if (PRTYPE = 3) then */
/*                    B(k + 1, k + 1) = B(k, k) */
/*                    B(k + 1, k) = [-1...1] */
/*                    sign(B(k, k + 1) = -(sign(B(k + 1, k)) */
/*                        k = 1, N - 1, QBLCKB */

/*             D : if (i <= j) then D(i, j) = [-1...1]. */
/*                 else D(i, j) = 0.0,            i, j = 1...M */


/*             E : if (i <= j) then D(i, j) = [-1...1] */
/*                 else E(i, j) = 0.0,            i, j = 1...N */

/*                 L, R are chosen from [-10...10], */
/*                 which specifies the right hand sides (C, F). */

/*  PRTYPE = 4 Full */
/*             A(i, j) = [-10...10] */
/*             D(i, j) = [-1...1]    i,j = 1...M */
/*             B(i, j) = [-10...10] */
/*             E(i, j) = [-1...1]    i,j = 1...N */
/*             R(i, j) = [-10...10] */
/*             L(i, j) = [-1...1]    i = 1..M ,j = 1...N */

/*             L, R specifies the right hand sides (C, F). */

/*  PRTYPE = 5 special case common and/or close eigs. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    d_dim1 = *ldd;
    d_offset = 1 + d_dim1;
    d__ -= d_offset;
    e_dim1 = *lde;
    e_offset = 1 + e_dim1;
    e -= e_offset;
    f_dim1 = *ldf;
    f_offset = 1 + f_dim1;
    f -= f_offset;
    r_dim1 = *ldr;
    r_offset = 1 + r_dim1;
    r__ -= r_offset;
    l_dim1 = *ldl;
    l_offset = 1 + l_dim1;
    l -= l_offset;

    /* Function Body */
    if (*prtype == 1) {
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *m;
	    for (j = 1; j <= i__2; ++j) {
		if (i__ == j) {
		    a[i__ + j * a_dim1] = 1.f;
		    d__[i__ + j * d_dim1] = 1.f;
		} else if (i__ == j - 1) {
		    a[i__ + j * a_dim1] = -1.f;
		    d__[i__ + j * d_dim1] = 0.f;
		} else {
		    a[i__ + j * a_dim1] = 0.f;
		    d__[i__ + j * d_dim1] = 0.f;
		}
/* L10: */
	    }
/* L20: */
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (i__ == j) {
		    b[i__ + j * b_dim1] = 1.f - *alpha;
		    e[i__ + j * e_dim1] = 1.f;
		} else if (i__ == j - 1) {
		    b[i__ + j * b_dim1] = 1.f;
		    e[i__ + j * e_dim1] = 0.f;
		} else {
		    b[i__ + j * b_dim1] = 0.f;
		    e[i__ + j * e_dim1] = 0.f;
		}
/* L30: */
	    }
/* L40: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		r__[i__ + j * r_dim1] = (.5f - sin((real) (i__ / j))) * 20.f;
		l[i__ + j * l_dim1] = r__[i__ + j * r_dim1];
/* L50: */
	    }
/* L60: */
	}

    } else if (*prtype == 2 || *prtype == 3) {
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *m;
	    for (j = 1; j <= i__2; ++j) {
		if (i__ <= j) {
		    a[i__ + j * a_dim1] = (.5f - sin((real) i__)) * 2.f;
		    d__[i__ + j * d_dim1] = (.5f - sin((real) (i__ * j))) * 
			    2.f;
		} else {
		    a[i__ + j * a_dim1] = 0.f;
		    d__[i__ + j * d_dim1] = 0.f;
		}
/* L70: */
	    }
/* L80: */
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (i__ <= j) {
		    b[i__ + j * b_dim1] = (.5f - sin((real) (i__ + j))) * 2.f;
		    e[i__ + j * e_dim1] = (.5f - sin((real) j)) * 2.f;
		} else {
		    b[i__ + j * b_dim1] = 0.f;
		    e[i__ + j * e_dim1] = 0.f;
		}
/* L90: */
	    }
/* L100: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		r__[i__ + j * r_dim1] = (.5f - sin((real) (i__ * j))) * 20.f;
		l[i__ + j * l_dim1] = (.5f - sin((real) (i__ + j))) * 20.f;
/* L110: */
	    }
/* L120: */
	}

	if (*prtype == 3) {
	    if (*qblcka <= 1) {
		*qblcka = 2;
	    }
	    i__1 = *m - 1;
	    i__2 = *qblcka;
	    for (k = 1; i__2 < 0 ? k >= i__1 : k <= i__1; k += i__2) {
		a[k + 1 + (k + 1) * a_dim1] = a[k + k * a_dim1];
		a[k + 1 + k * a_dim1] = -sin(a[k + (k + 1) * a_dim1]);
/* L130: */
	    }

	    if (*qblckb <= 1) {
		*qblckb = 2;
	    }
	    i__2 = *n - 1;
	    i__1 = *qblckb;
	    for (k = 1; i__1 < 0 ? k >= i__2 : k <= i__2; k += i__1) {
		b[k + 1 + (k + 1) * b_dim1] = b[k + k * b_dim1];
		b[k + 1 + k * b_dim1] = -sin(b[k + (k + 1) * b_dim1]);
/* L140: */
	    }
	}

    } else if (*prtype == 4) {
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *m;
	    for (j = 1; j <= i__2; ++j) {
		a[i__ + j * a_dim1] = (.5f - sin((real) (i__ * j))) * 20.f;
		d__[i__ + j * d_dim1] = (.5f - sin((real) (i__ + j))) * 2.f;
/* L150: */
	    }
/* L160: */
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		b[i__ + j * b_dim1] = (.5f - sin((real) (i__ + j))) * 20.f;
		e[i__ + j * e_dim1] = (.5f - sin((real) (i__ * j))) * 2.f;
/* L170: */
	    }
/* L180: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		r__[i__ + j * r_dim1] = (.5f - sin((real) (j / i__))) * 20.f;
		l[i__ + j * l_dim1] = (.5f - sin((real) (i__ * j))) * 2.f;
/* L190: */
	    }
/* L200: */
	}

    } else if (*prtype >= 5) {
	reeps = 20.f / *alpha;
	imeps = -1.5f / *alpha;
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		r__[i__ + j * r_dim1] = (.5f - sin((real) (i__ * j))) * *
			alpha / 20.f;
		l[i__ + j * l_dim1] = (.5f - sin((real) (i__ + j))) * *alpha /
			 20.f;
/* L210: */
	    }
/* L220: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__[i__ + i__ * d_dim1] = 1.f;
/* L230: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (i__ <= 4) {
		a[i__ + i__ * a_dim1] = 1.f;
		if (i__ > 2) {
		    a[i__ + i__ * a_dim1] = reeps + 1.f;
		}
		if (i__ % 2 != 0 && i__ < *m) {
		    a[i__ + (i__ + 1) * a_dim1] = imeps;
		} else if (i__ > 1) {
		    a[i__ + (i__ - 1) * a_dim1] = -imeps;
		}
	    } else if (i__ <= 8) {
		if (i__ <= 6) {
		    a[i__ + i__ * a_dim1] = reeps;
		} else {
		    a[i__ + i__ * a_dim1] = -reeps;
		}
		if (i__ % 2 != 0 && i__ < *m) {
		    a[i__ + (i__ + 1) * a_dim1] = 1.f;
		} else if (i__ > 1) {
		    a[i__ + (i__ - 1) * a_dim1] = -1.f;
		}
	    } else {
		a[i__ + i__ * a_dim1] = 1.f;
		if (i__ % 2 != 0 && i__ < *m) {
		    a[i__ + (i__ + 1) * a_dim1] = imeps * 2;
		} else if (i__ > 1) {
		    a[i__ + (i__ - 1) * a_dim1] = -imeps * 2;
		}
	    }
/* L240: */
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    e[i__ + i__ * e_dim1] = 1.f;
	    if (i__ <= 4) {
		b[i__ + i__ * b_dim1] = -1.f;
		if (i__ > 2) {
		    b[i__ + i__ * b_dim1] = 1.f - reeps;
		}
		if (i__ % 2 != 0 && i__ < *n) {
		    b[i__ + (i__ + 1) * b_dim1] = imeps;
		} else if (i__ > 1) {
		    b[i__ + (i__ - 1) * b_dim1] = -imeps;
		}
	    } else if (i__ <= 8) {
		if (i__ <= 6) {
		    b[i__ + i__ * b_dim1] = reeps;
		} else {
		    b[i__ + i__ * b_dim1] = -reeps;
		}
		if (i__ % 2 != 0 && i__ < *n) {
		    b[i__ + (i__ + 1) * b_dim1] = imeps + 1.f;
		} else if (i__ > 1) {
		    b[i__ + (i__ - 1) * b_dim1] = -1.f - imeps;
		}
	    } else {
		b[i__ + i__ * b_dim1] = 1.f - reeps;
		if (i__ % 2 != 0 && i__ < *n) {
		    b[i__ + (i__ + 1) * b_dim1] = imeps * 2;
		} else if (i__ > 1) {
		    b[i__ + (i__ - 1) * b_dim1] = -imeps * 2;
		}
	    }
/* L250: */
	}
    }

/*     Compute rhs (C, F) */

    sgemm_("N", "N", m, n, m, &c_b29, &a[a_offset], lda, &r__[r_offset], ldr, 
	    &c_b30, &c__[c_offset], ldc);
    sgemm_("N", "N", m, n, n, &c_b33, &l[l_offset], ldl, &b[b_offset], ldb, &
	    c_b29, &c__[c_offset], ldc);
    sgemm_("N", "N", m, n, m, &c_b29, &d__[d_offset], ldd, &r__[r_offset], 
	    ldr, &c_b30, &f[f_offset], ldf);
    sgemm_("N", "N", m, n, n, &c_b33, &l[l_offset], ldl, &e[e_offset], lde, &
	    c_b29, &f[f_offset], ldf);

/*     End of SLATM5 */

    return 0;
} /* slatm5_ */
