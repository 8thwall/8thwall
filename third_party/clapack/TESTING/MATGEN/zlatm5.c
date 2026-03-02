#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {1.,0.};
static doublecomplex c_b3 = {0.,0.};
static doublecomplex c_b5 = {20.,0.};

/* Subroutine */ int zlatm5_(integer *prtype, integer *m, integer *n, 
	doublecomplex *a, integer *lda, doublecomplex *b, integer *ldb, 
	doublecomplex *c__, integer *ldc, doublecomplex *d__, integer *ldd, 
	doublecomplex *e, integer *lde, doublecomplex *f, integer *ldf, 
	doublecomplex *r__, integer *ldr, doublecomplex *l, integer *ldl, 
	doublereal *alpha, integer *qblcka, integer *qblckb)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, d_dim1, 
	    d_offset, e_dim1, e_offset, f_dim1, f_offset, l_dim1, l_offset, 
	    r_dim1, r_offset, i__1, i__2, i__3, i__4;
    doublereal d__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5;

    /* Builtin functions */
    void z_sin(doublecomplex *, doublecomplex *), z_div(doublecomplex *, 
	    doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__, j, k;
    doublecomplex imeps, reeps;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLATM5 generates matrices involved in the Generalized Sylvester */
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

/*  A       (output) COMPLEX*16 array, dimension (LDA, M). */
/*          On exit A M-by-M is initialized according to PRTYPE. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A. */

/*  B       (output) COMPLEX*16 array, dimension (LDB, N). */
/*          On exit B N-by-N is initialized according to PRTYPE. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B. */

/*  C       (output) COMPLEX*16 array, dimension (LDC, N). */
/*          On exit C M-by-N is initialized according to PRTYPE. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of C. */

/*  D       (output) COMPLEX*16 array, dimension (LDD, M). */
/*          On exit D M-by-M is initialized according to PRTYPE. */

/*  LDD     (input) INTEGER */
/*          The leading dimension of D. */

/*  E       (output) COMPLEX*16 array, dimension (LDE, N). */
/*          On exit E N-by-N is initialized according to PRTYPE. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of E. */

/*  F       (output) COMPLEX*16 array, dimension (LDF, N). */
/*          On exit F M-by-N is initialized according to PRTYPE. */

/*  LDF     (input) INTEGER */
/*          The leading dimension of F. */

/*  R       (output) COMPLEX*16 array, dimension (LDR, N). */
/*          On exit R M-by-N is initialized according to PRTYPE. */

/*  LDR     (input) INTEGER */
/*          The leading dimension of R. */

/*  L       (output) COMPLEX*16 array, dimension (LDL, N). */
/*          On exit L M-by-N is initialized according to PRTYPE. */

/*  LDL     (input) INTEGER */
/*          The leading dimension of L. */

/*  ALPHA   (input) DOUBLE PRECISION */
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
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 1., a[i__3].i = 0.;
		    i__3 = i__ + j * d_dim1;
		    d__[i__3].r = 1., d__[i__3].i = 0.;
		} else if (i__ == j - 1) {
		    i__3 = i__ + j * a_dim1;
		    z__1.r = -1., z__1.i = -0.;
		    a[i__3].r = z__1.r, a[i__3].i = z__1.i;
		    i__3 = i__ + j * d_dim1;
		    d__[i__3].r = 0., d__[i__3].i = 0.;
		} else {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0., a[i__3].i = 0.;
		    i__3 = i__ + j * d_dim1;
		    d__[i__3].r = 0., d__[i__3].i = 0.;
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
		    i__3 = i__ + j * b_dim1;
		    z__1.r = 1. - *alpha, z__1.i = 0.;
		    b[i__3].r = z__1.r, b[i__3].i = z__1.i;
		    i__3 = i__ + j * e_dim1;
		    e[i__3].r = 1., e[i__3].i = 0.;
		} else if (i__ == j - 1) {
		    i__3 = i__ + j * b_dim1;
		    b[i__3].r = 1., b[i__3].i = 0.;
		    i__3 = i__ + j * e_dim1;
		    e[i__3].r = 0., e[i__3].i = 0.;
		} else {
		    i__3 = i__ + j * b_dim1;
		    b[i__3].r = 0., b[i__3].i = 0.;
		    i__3 = i__ + j * e_dim1;
		    e[i__3].r = 0., e[i__3].i = 0.;
		}
/* L30: */
	    }
/* L40: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * r_dim1;
		i__4 = i__ / j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 20. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 20.;
		r__[i__3].r = z__1.r, r__[i__3].i = z__1.i;
		i__3 = i__ + j * l_dim1;
		i__4 = i__ + j * r_dim1;
		l[i__3].r = r__[i__4].r, l[i__3].i = r__[i__4].i;
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
		    i__3 = i__ + j * a_dim1;
		    z__4.r = (doublereal) i__, z__4.i = 0.;
		    z_sin(&z__3, &z__4);
		    z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		    z__1.r = z__2.r * 2. - z__2.i * 0., z__1.i = z__2.r * 0. 
			    + z__2.i * 2.;
		    a[i__3].r = z__1.r, a[i__3].i = z__1.i;
		    i__3 = i__ + j * d_dim1;
		    i__4 = i__ * j;
		    z__4.r = (doublereal) i__4, z__4.i = 0.;
		    z_sin(&z__3, &z__4);
		    z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		    z__1.r = z__2.r * 2. - z__2.i * 0., z__1.i = z__2.r * 0. 
			    + z__2.i * 2.;
		    d__[i__3].r = z__1.r, d__[i__3].i = z__1.i;
		} else {
		    i__3 = i__ + j * a_dim1;
		    a[i__3].r = 0., a[i__3].i = 0.;
		    i__3 = i__ + j * d_dim1;
		    d__[i__3].r = 0., d__[i__3].i = 0.;
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
		    i__3 = i__ + j * b_dim1;
		    i__4 = i__ + j;
		    z__4.r = (doublereal) i__4, z__4.i = 0.;
		    z_sin(&z__3, &z__4);
		    z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		    z__1.r = z__2.r * 2. - z__2.i * 0., z__1.i = z__2.r * 0. 
			    + z__2.i * 2.;
		    b[i__3].r = z__1.r, b[i__3].i = z__1.i;
		    i__3 = i__ + j * e_dim1;
		    z__4.r = (doublereal) j, z__4.i = 0.;
		    z_sin(&z__3, &z__4);
		    z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		    z__1.r = z__2.r * 2. - z__2.i * 0., z__1.i = z__2.r * 0. 
			    + z__2.i * 2.;
		    e[i__3].r = z__1.r, e[i__3].i = z__1.i;
		} else {
		    i__3 = i__ + j * b_dim1;
		    b[i__3].r = 0., b[i__3].i = 0.;
		    i__3 = i__ + j * e_dim1;
		    e[i__3].r = 0., e[i__3].i = 0.;
		}
/* L90: */
	    }
/* L100: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * r_dim1;
		i__4 = i__ * j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 20. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 20.;
		r__[i__3].r = z__1.r, r__[i__3].i = z__1.i;
		i__3 = i__ + j * l_dim1;
		i__4 = i__ + j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 20. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 20.;
		l[i__3].r = z__1.r, l[i__3].i = z__1.i;
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
		i__3 = k + 1 + (k + 1) * a_dim1;
		i__4 = k + k * a_dim1;
		a[i__3].r = a[i__4].r, a[i__3].i = a[i__4].i;
		i__3 = k + 1 + k * a_dim1;
		z_sin(&z__2, &a[k + (k + 1) * a_dim1]);
		z__1.r = -z__2.r, z__1.i = -z__2.i;
		a[i__3].r = z__1.r, a[i__3].i = z__1.i;
/* L130: */
	    }

	    if (*qblckb <= 1) {
		*qblckb = 2;
	    }
	    i__2 = *n - 1;
	    i__1 = *qblckb;
	    for (k = 1; i__1 < 0 ? k >= i__2 : k <= i__2; k += i__1) {
		i__3 = k + 1 + (k + 1) * b_dim1;
		i__4 = k + k * b_dim1;
		b[i__3].r = b[i__4].r, b[i__3].i = b[i__4].i;
		i__3 = k + 1 + k * b_dim1;
		z_sin(&z__2, &b[k + (k + 1) * b_dim1]);
		z__1.r = -z__2.r, z__1.i = -z__2.i;
		b[i__3].r = z__1.r, b[i__3].i = z__1.i;
/* L140: */
	    }
	}

    } else if (*prtype == 4) {
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *m;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * a_dim1;
		i__4 = i__ * j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 20. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 20.;
		a[i__3].r = z__1.r, a[i__3].i = z__1.i;
		i__3 = i__ + j * d_dim1;
		i__4 = i__ + j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 2. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 2.;
		d__[i__3].r = z__1.r, d__[i__3].i = z__1.i;
/* L150: */
	    }
/* L160: */
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * b_dim1;
		i__4 = i__ + j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 20. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 20.;
		b[i__3].r = z__1.r, b[i__3].i = z__1.i;
		i__3 = i__ + j * e_dim1;
		i__4 = i__ * j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 2. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 2.;
		e[i__3].r = z__1.r, e[i__3].i = z__1.i;
/* L170: */
	    }
/* L180: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * r_dim1;
		i__4 = j / i__;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 20. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 20.;
		r__[i__3].r = z__1.r, r__[i__3].i = z__1.i;
		i__3 = i__ + j * l_dim1;
		i__4 = i__ * j;
		z__4.r = (doublereal) i__4, z__4.i = 0.;
		z_sin(&z__3, &z__4);
		z__2.r = .5 - z__3.r, z__2.i = 0. - z__3.i;
		z__1.r = z__2.r * 2. - z__2.i * 0., z__1.i = z__2.r * 0. + 
			z__2.i * 2.;
		l[i__3].r = z__1.r, l[i__3].i = z__1.i;
/* L190: */
	    }
/* L200: */
	}

    } else if (*prtype >= 5) {
	z__3.r = 1., z__3.i = 0.;
	z__2.r = z__3.r * 20. - z__3.i * 0., z__2.i = z__3.r * 0. + z__3.i * 
		20.;
	z__1.r = z__2.r / *alpha, z__1.i = z__2.i / *alpha;
	reeps.r = z__1.r, reeps.i = z__1.i;
	z__2.r = -1.5, z__2.i = 0.;
	z__1.r = z__2.r / *alpha, z__1.i = z__2.i / *alpha;
	imeps.r = z__1.r, imeps.i = z__1.i;
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * r_dim1;
		i__4 = i__ * j;
		z__5.r = (doublereal) i__4, z__5.i = 0.;
		z_sin(&z__4, &z__5);
		z__3.r = .5 - z__4.r, z__3.i = 0. - z__4.i;
		z__2.r = *alpha * z__3.r, z__2.i = *alpha * z__3.i;
		z_div(&z__1, &z__2, &c_b5);
		r__[i__3].r = z__1.r, r__[i__3].i = z__1.i;
		i__3 = i__ + j * l_dim1;
		i__4 = i__ + j;
		z__5.r = (doublereal) i__4, z__5.i = 0.;
		z_sin(&z__4, &z__5);
		z__3.r = .5 - z__4.r, z__3.i = 0. - z__4.i;
		z__2.r = *alpha * z__3.r, z__2.i = *alpha * z__3.i;
		z_div(&z__1, &z__2, &c_b5);
		l[i__3].r = z__1.r, l[i__3].i = z__1.i;
/* L210: */
	    }
/* L220: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__ + i__ * d_dim1;
	    d__[i__2].r = 1., d__[i__2].i = 0.;
/* L230: */
	}

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (i__ <= 4) {
		i__2 = i__ + i__ * a_dim1;
		a[i__2].r = 1., a[i__2].i = 0.;
		if (i__ > 2) {
		    i__2 = i__ + i__ * a_dim1;
		    z__1.r = reeps.r + 1., z__1.i = reeps.i + 0.;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		}
		if (i__ % 2 != 0 && i__ < *m) {
		    i__2 = i__ + (i__ + 1) * a_dim1;
		    a[i__2].r = imeps.r, a[i__2].i = imeps.i;
		} else if (i__ > 1) {
		    i__2 = i__ + (i__ - 1) * a_dim1;
		    z__1.r = -imeps.r, z__1.i = -imeps.i;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		}
	    } else if (i__ <= 8) {
		if (i__ <= 6) {
		    i__2 = i__ + i__ * a_dim1;
		    a[i__2].r = reeps.r, a[i__2].i = reeps.i;
		} else {
		    i__2 = i__ + i__ * a_dim1;
		    z__1.r = -reeps.r, z__1.i = -reeps.i;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		}
		if (i__ % 2 != 0 && i__ < *m) {
		    i__2 = i__ + (i__ + 1) * a_dim1;
		    a[i__2].r = 1., a[i__2].i = 0.;
		} else if (i__ > 1) {
		    i__2 = i__ + (i__ - 1) * a_dim1;
		    z__1.r = -1., z__1.i = -0.;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		}
	    } else {
		i__2 = i__ + i__ * a_dim1;
		a[i__2].r = 1., a[i__2].i = 0.;
		if (i__ % 2 != 0 && i__ < *m) {
		    i__2 = i__ + (i__ + 1) * a_dim1;
		    d__1 = 2.;
		    z__1.r = d__1 * imeps.r, z__1.i = d__1 * imeps.i;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		} else if (i__ > 1) {
		    i__2 = i__ + (i__ - 1) * a_dim1;
		    z__2.r = -imeps.r, z__2.i = -imeps.i;
		    d__1 = 2.;
		    z__1.r = d__1 * z__2.r, z__1.i = d__1 * z__2.i;
		    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
		}
	    }
/* L240: */
	}

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__ + i__ * e_dim1;
	    e[i__2].r = 1., e[i__2].i = 0.;
	    if (i__ <= 4) {
		i__2 = i__ + i__ * b_dim1;
		z__1.r = -1., z__1.i = -0.;
		b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		if (i__ > 2) {
		    i__2 = i__ + i__ * b_dim1;
		    z__1.r = 1. - reeps.r, z__1.i = 0. - reeps.i;
		    b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		}
		if (i__ % 2 != 0 && i__ < *n) {
		    i__2 = i__ + (i__ + 1) * b_dim1;
		    b[i__2].r = imeps.r, b[i__2].i = imeps.i;
		} else if (i__ > 1) {
		    i__2 = i__ + (i__ - 1) * b_dim1;
		    z__1.r = -imeps.r, z__1.i = -imeps.i;
		    b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		}
	    } else if (i__ <= 8) {
		if (i__ <= 6) {
		    i__2 = i__ + i__ * b_dim1;
		    b[i__2].r = reeps.r, b[i__2].i = reeps.i;
		} else {
		    i__2 = i__ + i__ * b_dim1;
		    z__1.r = -reeps.r, z__1.i = -reeps.i;
		    b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		}
		if (i__ % 2 != 0 && i__ < *n) {
		    i__2 = i__ + (i__ + 1) * b_dim1;
		    z__1.r = imeps.r + 1., z__1.i = imeps.i + 0.;
		    b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		} else if (i__ > 1) {
		    i__2 = i__ + (i__ - 1) * b_dim1;
		    z__2.r = -1., z__2.i = -0.;
		    z__1.r = z__2.r - imeps.r, z__1.i = z__2.i - imeps.i;
		    b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		}
	    } else {
		i__2 = i__ + i__ * b_dim1;
		z__1.r = 1. - reeps.r, z__1.i = 0. - reeps.i;
		b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		if (i__ % 2 != 0 && i__ < *n) {
		    i__2 = i__ + (i__ + 1) * b_dim1;
		    d__1 = 2.;
		    z__1.r = d__1 * imeps.r, z__1.i = d__1 * imeps.i;
		    b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		} else if (i__ > 1) {
		    i__2 = i__ + (i__ - 1) * b_dim1;
		    z__2.r = -imeps.r, z__2.i = -imeps.i;
		    d__1 = 2.;
		    z__1.r = d__1 * z__2.r, z__1.i = d__1 * z__2.i;
		    b[i__2].r = z__1.r, b[i__2].i = z__1.i;
		}
	    }
/* L250: */
	}
    }

/*     Compute rhs (C, F) */

    zgemm_("N", "N", m, n, m, &c_b1, &a[a_offset], lda, &r__[r_offset], ldr, &
	    c_b3, &c__[c_offset], ldc);
    z__1.r = -1., z__1.i = -0.;
    zgemm_("N", "N", m, n, n, &z__1, &l[l_offset], ldl, &b[b_offset], ldb, &
	    c_b1, &c__[c_offset], ldc);
    zgemm_("N", "N", m, n, m, &c_b1, &d__[d_offset], ldd, &r__[r_offset], ldr, 
	     &c_b3, &f[f_offset], ldf);
    z__1.r = -1., z__1.i = -0.;
    zgemm_("N", "N", m, n, n, &z__1, &l[l_offset], ldl, &e[e_offset], lde, &
	    c_b1, &f[f_offset], ldf);

/*     End of ZLATM5 */

    return 0;
} /* zlatm5_ */
