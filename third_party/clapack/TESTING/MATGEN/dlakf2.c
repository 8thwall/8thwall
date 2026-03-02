#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b3 = 0.;

/* Subroutine */ int dlakf2_(integer *m, integer *n, doublereal *a, integer *
	lda, doublereal *b, doublereal *d__, doublereal *e, doublereal *z__, 
	integer *ldz)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, d_dim1, d_offset, e_dim1, 
	    e_offset, z_dim1, z_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, j, l, ik, jk, mn, mn2;
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Form the 2*M*N by 2*M*N matrix */

/*         Z = [ kron(In, A)  -kron(B', Im) ] */
/*             [ kron(In, D)  -kron(E', Im) ], */

/*  where In is the identity matrix of size n and X' is the transpose */
/*  of X. kron(X, Y) is the Kronecker product between the matrices X */
/*  and Y. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          Size of matrix, must be >= 1. */

/*  N       (input) INTEGER */
/*          Size of matrix, must be >= 1. */

/*  A       (input) DOUBLE PRECISION, dimension ( LDA, M ) */
/*          The matrix A in the output matrix Z. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, B, D, and E. ( LDA >= M+N ) */

/*  B       (input) DOUBLE PRECISION, dimension ( LDA, N ) */
/*  D       (input) DOUBLE PRECISION, dimension ( LDA, M ) */
/*  E       (input) DOUBLE PRECISION, dimension ( LDA, N ) */
/*          The matrices used in forming the output matrix Z. */

/*  Z       (output) DOUBLE PRECISION, dimension ( LDZ, 2*M*N ) */
/*          The resultant Kronecker M*N*2 by M*N*2 matrix (see above.) */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of Z. ( LDZ >= 2*M*N ) */

/*  ==================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize Z */

    /* Parameter adjustments */
    e_dim1 = *lda;
    e_offset = 1 + e_dim1;
    e -= e_offset;
    d_dim1 = *lda;
    d_offset = 1 + d_dim1;
    d__ -= d_offset;
    b_dim1 = *lda;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;

    /* Function Body */
    mn = *m * *n;
    mn2 = mn << 1;
    dlaset_("Full", &mn2, &mn2, &c_b3, &c_b3, &z__[z_offset], ldz);

    ik = 1;
    i__1 = *n;
    for (l = 1; l <= i__1; ++l) {

/*        form kron(In, A) */

	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = *m;
	    for (j = 1; j <= i__3; ++j) {
		z__[ik + i__ - 1 + (ik + j - 1) * z_dim1] = a[i__ + j * 
			a_dim1];
/* L10: */
	    }
/* L20: */
	}

/*        form kron(In, D) */

	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = *m;
	    for (j = 1; j <= i__3; ++j) {
		z__[ik + mn + i__ - 1 + (ik + j - 1) * z_dim1] = d__[i__ + j *
			 d_dim1];
/* L30: */
	    }
/* L40: */
	}

	ik += *m;
/* L50: */
    }

    ik = 1;
    i__1 = *n;
    for (l = 1; l <= i__1; ++l) {
	jk = mn + 1;

	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {

/*           form -kron(B', Im) */

	    i__3 = *m;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		z__[ik + i__ - 1 + (jk + i__ - 1) * z_dim1] = -b[j + l * 
			b_dim1];
/* L60: */
	    }

/*           form -kron(E', Im) */

	    i__3 = *m;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		z__[ik + mn + i__ - 1 + (jk + i__ - 1) * z_dim1] = -e[j + l * 
			e_dim1];
/* L70: */
	    }

	    jk += *m;
/* L80: */
	}

	ik += *m;
/* L90: */
    }

    return 0;

/*     End of DLAKF2 */

} /* dlakf2_ */
