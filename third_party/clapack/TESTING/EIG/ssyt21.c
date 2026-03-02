#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b10 = 0.f;
static integer c__1 = 1;
static real c_b42 = 1.f;

/* Subroutine */ int ssyt21_(integer *itype, char *uplo, integer *n, integer *
	kband, real *a, integer *lda, real *d__, real *e, real *u, integer *
	ldu, real *v, integer *ldv, real *tau, real *work, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, u_dim1, u_offset, v_dim1, v_offset, i__1, i__2, 
	    i__3;
    real r__1, r__2;

    /* Local variables */
    integer j, jr;
    real ulp;
    integer jcol;
    real unfl;
    integer jrow;
    extern /* Subroutine */ int ssyr_(char *, integer *, real *, real *, 
	    integer *, real *, integer *), ssyr2_(char *, integer *, 
	    real *, real *, integer *, real *, integer *, real *, integer *);
    extern logical lsame_(char *, char *);
    integer iinfo;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm;
    char cuplo[1];
    real vsave;
    logical lower;
    real wnorm;
    extern /* Subroutine */ int sorm2l_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *), sorm2r_(char *, char *, integer *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *), slarfy_(
	    char *, integer *, real *, integer *, real *, real *, integer *, 
	    real *);
    extern doublereal slansy_(char *, char *, integer *, real *, integer *, 
	    real *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSYT21 generally checks a decomposition of the form */

/*     A = U S U' */

/*  where ' means transpose, A is symmetric, U is orthogonal, and S is */
/*  diagonal (if KBAND=0) or symmetric tridiagonal (if KBAND=1). */

/*  If ITYPE=1, then U is represented as a dense matrix; otherwise U is */
/*  expressed as a product of Householder transformations, whose vectors */
/*  are stored in the array "V" and whose scaling constants are in "TAU". */
/*  We shall use the letter "V" to refer to the product of Householder */
/*  transformations (which should be equal to U). */

/*  Specifically, if ITYPE=1, then: */

/*     RESULT(1) = | A - U S U' | / ( |A| n ulp ) *and* */
/*     RESULT(2) = | I - UU' | / ( n ulp ) */

/*  If ITYPE=2, then: */

/*     RESULT(1) = | A - V S V' | / ( |A| n ulp ) */

/*  If ITYPE=3, then: */

/*     RESULT(1) = | I - VU' | / ( n ulp ) */

/*  For ITYPE > 1, the transformation U is expressed as a product */
/*  V = H(1)...H(n-2),  where H(j) = I  -  tau(j) v(j) v(j)' and each */
/*  vector v(j) has its first j elements 0 and the remaining n-j elements */
/*  stored in V(j+1:n,j). */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the type of tests to be performed. */
/*          1: U expressed as a dense orthogonal matrix: */
/*             RESULT(1) = | A - U S U' | / ( |A| n ulp )   *and* */
/*             RESULT(2) = | I - UU' | / ( n ulp ) */

/*          2: U expressed as a product V of Housholder transformations: */
/*             RESULT(1) = | A - V S V' | / ( |A| n ulp ) */

/*          3: U expressed both as a dense orthogonal matrix and */
/*             as a product of Housholder transformations: */
/*             RESULT(1) = | I - VU' | / ( n ulp ) */

/*  UPLO    (input) CHARACTER */
/*          If UPLO='U', the upper triangle of A and V will be used and */
/*          the (strictly) lower triangle will not be referenced. */
/*          If UPLO='L', the lower triangle of A and V will be used and */
/*          the (strictly) upper triangle will not be referenced. */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SSYT21 does nothing. */
/*          It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix.  It may only be zero or one. */
/*          If zero, then S is diagonal, and E is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  A       (input) REAL array, dimension (LDA, N) */
/*          The original (unfactored) matrix.  It is assumed to be */
/*          symmetric, and only the upper (UPLO='U') or only the lower */
/*          (UPLO='L') will be referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 1 */
/*          and at least N. */

/*  D       (input) REAL array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix. */

/*  E       (input) REAL array, dimension (N-1) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix. */
/*          E(1) is the (1,2) and (2,1) element, E(2) is the (2,3) and */
/*          (3,2) element, etc. */
/*          Not referenced if KBAND=0. */

/*  U       (input) REAL array, dimension (LDU, N) */
/*          If ITYPE=1 or 3, this contains the orthogonal matrix in */
/*          the decomposition, expressed as a dense matrix.  If ITYPE=2, */
/*          then it is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  V       (input) REAL array, dimension (LDV, N) */
/*          If ITYPE=2 or 3, the columns of this array contain the */
/*          Householder vectors used to describe the orthogonal matrix */
/*          in the decomposition.  If UPLO='L', then the vectors are in */
/*          the lower triangle, if UPLO='U', then in the upper */
/*          triangle. */
/*          *NOTE* If ITYPE=2 or 3, V is modified and restored.  The */
/*          subdiagonal (if UPLO='L') or the superdiagonal (if UPLO='U') */
/*          is set to one, and later reset to its original value, during */
/*          the course of the calculation. */
/*          If ITYPE=1, then it is neither referenced nor modified. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of V.  LDV must be at least N and */
/*          at least 1. */

/*  TAU     (input) REAL array, dimension (N) */
/*          If ITYPE >= 2, then TAU(j) is the scalar factor of */
/*          v(j) v(j)' in the Householder transformation H(j) of */
/*          the product  U = H(1)...H(n-2) */
/*          If ITYPE < 2, then TAU is not referenced. */

/*  WORK    (workspace) REAL array, dimension (2*N**2) */

/*  RESULT  (output) REAL array, dimension (2) */
/*          The values computed by the two tests described above.  The */
/*          values are currently limited to 1/ulp, to avoid overflow. */
/*          RESULT(1) is always modified.  RESULT(2) is modified only */
/*          if ITYPE=1. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --tau;
    --work;
    --result;

    /* Function Body */
    result[1] = 0.f;
    if (*itype == 1) {
	result[2] = 0.f;
    }
    if (*n <= 0) {
	return 0;
    }

    if (lsame_(uplo, "U")) {
	lower = FALSE_;
	*(unsigned char *)cuplo = 'U';
    } else {
	lower = TRUE_;
	*(unsigned char *)cuplo = 'L';
    }

    unfl = slamch_("Safe minimum");
    ulp = slamch_("Epsilon") * slamch_("Base");

/*     Some Error Checks */

    if (*itype < 1 || *itype > 3) {
	result[1] = 10.f / ulp;
	return 0;
    }

/*     Do Test 1 */

/*     Norm of A: */

    if (*itype == 3) {
	anorm = 1.f;
    } else {
/* Computing MAX */
	r__1 = slansy_("1", cuplo, n, &a[a_offset], lda, &work[1]);
	anorm = dmax(r__1,unfl);
    }

/*     Compute error matrix: */

    if (*itype == 1) {

/*        ITYPE=1: error = A - U S U' */

	slaset_("Full", n, n, &c_b10, &c_b10, &work[1], n);
	slacpy_(cuplo, n, n, &a[a_offset], lda, &work[1], n);

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    r__1 = -d__[j];
	    ssyr_(cuplo, n, &r__1, &u[j * u_dim1 + 1], &c__1, &work[1], n);
/* L10: */
	}

	if (*n > 1 && *kband == 1) {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		r__1 = -e[j];
		ssyr2_(cuplo, n, &r__1, &u[j * u_dim1 + 1], &c__1, &u[(j + 1) 
			* u_dim1 + 1], &c__1, &work[1], n);
/* L20: */
	    }
	}
/* Computing 2nd power */
	i__1 = *n;
	wnorm = slansy_("1", cuplo, n, &work[1], n, &work[i__1 * i__1 + 1]);

    } else if (*itype == 2) {

/*        ITYPE=2: error = V S V' - A */

	slaset_("Full", n, n, &c_b10, &c_b10, &work[1], n);

	if (lower) {
/* Computing 2nd power */
	    i__1 = *n;
	    work[i__1 * i__1] = d__[*n];
	    for (j = *n - 1; j >= 1; --j) {
		if (*kband == 1) {
		    work[(*n + 1) * (j - 1) + 2] = (1.f - tau[j]) * e[j];
		    i__1 = *n;
		    for (jr = j + 2; jr <= i__1; ++jr) {
			work[(j - 1) * *n + jr] = -tau[j] * e[j] * v[jr + j * 
				v_dim1];
/* L30: */
		    }
		}

		vsave = v[j + 1 + j * v_dim1];
		v[j + 1 + j * v_dim1] = 1.f;
		i__1 = *n - j;
/* Computing 2nd power */
		i__2 = *n;
		slarfy_("L", &i__1, &v[j + 1 + j * v_dim1], &c__1, &tau[j], &
			work[(*n + 1) * j + 1], n, &work[i__2 * i__2 + 1]);
		v[j + 1 + j * v_dim1] = vsave;
		work[(*n + 1) * (j - 1) + 1] = d__[j];
/* L40: */
	    }
	} else {
	    work[1] = d__[1];
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		if (*kband == 1) {
		    work[(*n + 1) * j] = (1.f - tau[j]) * e[j];
		    i__2 = j - 1;
		    for (jr = 1; jr <= i__2; ++jr) {
			work[j * *n + jr] = -tau[j] * e[j] * v[jr + (j + 1) * 
				v_dim1];
/* L50: */
		    }
		}

		vsave = v[j + (j + 1) * v_dim1];
		v[j + (j + 1) * v_dim1] = 1.f;
/* Computing 2nd power */
		i__2 = *n;
		slarfy_("U", &j, &v[(j + 1) * v_dim1 + 1], &c__1, &tau[j], &
			work[1], n, &work[i__2 * i__2 + 1]);
		v[j + (j + 1) * v_dim1] = vsave;
		work[(*n + 1) * j + 1] = d__[j + 1];
/* L60: */
	    }
	}

	i__1 = *n;
	for (jcol = 1; jcol <= i__1; ++jcol) {
	    if (lower) {
		i__2 = *n;
		for (jrow = jcol; jrow <= i__2; ++jrow) {
		    work[jrow + *n * (jcol - 1)] -= a[jrow + jcol * a_dim1];
/* L70: */
		}
	    } else {
		i__2 = jcol;
		for (jrow = 1; jrow <= i__2; ++jrow) {
		    work[jrow + *n * (jcol - 1)] -= a[jrow + jcol * a_dim1];
/* L80: */
		}
	    }
/* L90: */
	}
/* Computing 2nd power */
	i__1 = *n;
	wnorm = slansy_("1", cuplo, n, &work[1], n, &work[i__1 * i__1 + 1]);

    } else if (*itype == 3) {

/*        ITYPE=3: error = U V' - I */

	if (*n < 2) {
	    return 0;
	}
	slacpy_(" ", n, n, &u[u_offset], ldu, &work[1], n);
	if (lower) {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
/* Computing 2nd power */
	    i__3 = *n;
	    sorm2r_("R", "T", n, &i__1, &i__2, &v[v_dim1 + 2], ldv, &tau[1], &
		    work[*n + 1], n, &work[i__3 * i__3 + 1], &iinfo);
	} else {
	    i__1 = *n - 1;
	    i__2 = *n - 1;
/* Computing 2nd power */
	    i__3 = *n;
	    sorm2l_("R", "T", n, &i__1, &i__2, &v[(v_dim1 << 1) + 1], ldv, &
		    tau[1], &work[1], n, &work[i__3 * i__3 + 1], &iinfo);
	}
	if (iinfo != 0) {
	    result[1] = 10.f / ulp;
	    return 0;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[(*n + 1) * (j - 1) + 1] += -1.f;
/* L100: */
	}

/* Computing 2nd power */
	i__1 = *n;
	wnorm = slange_("1", n, n, &work[1], n, &work[i__1 * i__1 + 1]);
    }

    if (anorm > wnorm) {
	result[1] = wnorm / anorm / (*n * ulp);
    } else {
	if (anorm < 1.f) {
/* Computing MIN */
	    r__1 = wnorm, r__2 = *n * anorm;
	    result[1] = dmin(r__1,r__2) / anorm / (*n * ulp);
	} else {
/* Computing MIN */
	    r__1 = wnorm / anorm, r__2 = (real) (*n);
	    result[1] = dmin(r__1,r__2) / (*n * ulp);
	}
    }

/*     Do Test 2 */

/*     Compute  UU' - I */

    if (*itype == 1) {
	sgemm_("N", "C", n, n, n, &c_b42, &u[u_offset], ldu, &u[u_offset], 
		ldu, &c_b10, &work[1], n);

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[(*n + 1) * (j - 1) + 1] += -1.f;
/* L110: */
	}

/* Computing MIN */
/* Computing 2nd power */
	i__1 = *n;
	r__1 = slange_("1", n, n, &work[1], n, &work[i__1 * i__1 + 1]), r__2 = (real) (*n);
	result[2] = dmin(r__1,r__2) / (*n * ulp);
    }

    return 0;

/*     End of SSYT21 */

} /* ssyt21_ */
