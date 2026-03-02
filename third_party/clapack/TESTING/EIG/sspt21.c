#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b10 = 0.f;
static integer c__1 = 1;
static real c_b26 = 1.f;

/* Subroutine */ int sspt21_(integer *itype, char *uplo, integer *n, integer *
	kband, real *ap, real *d__, real *e, real *u, integer *ldu, real *vp, 
	real *tau, real *work, real *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1, i__2;
    real r__1, r__2;

    /* Local variables */
    integer j, jp, jr, jp1, lap;
    real ulp, unfl, temp;
    extern doublereal sdot_(integer *, real *, integer *, real *, integer *);
    extern /* Subroutine */ int sspr_(char *, integer *, real *, real *, 
	    integer *, real *), sspr2_(char *, integer *, real *, 
	    real *, integer *, real *, integer *, real *);
    extern logical lsame_(char *, char *);
    integer iinfo;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm;
    char cuplo[1];
    real vsave;
    logical lower;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    real wnorm;
    extern /* Subroutine */ int saxpy_(integer *, real *, real *, integer *, 
	    real *, integer *), sspmv_(char *, integer *, real *, real *, 
	    real *, integer *, real *, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *);
    extern doublereal slansp_(char *, char *, integer *, real *, real *);
    extern /* Subroutine */ int sopmtr_(char *, char *, char *, integer *, 
	    integer *, real *, real *, real *, integer *, real *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SSPT21  generally checks a decomposition of the form */

/*          A = U S U' */

/*  where ' means transpose, A is symmetric (stored in packed format), U */
/*  is orthogonal, and S is diagonal (if KBAND=0) or symmetric */
/*  tridiagonal (if KBAND=1).  If ITYPE=1, then U is represented as a */
/*  dense matrix, otherwise the U is expressed as a product of */
/*  Householder transformations, whose vectors are stored in the array */
/*  "V" and whose scaling constants are in "TAU"; we shall use the */
/*  letter "V" to refer to the product of Householder transformations */
/*  (which should be equal to U). */

/*  Specifically, if ITYPE=1, then: */

/*          RESULT(1) = | A - U S U' | / ( |A| n ulp ) *and* */
/*          RESULT(2) = | I - UU' | / ( n ulp ) */

/*  If ITYPE=2, then: */

/*          RESULT(1) = | A - V S V' | / ( |A| n ulp ) */

/*  If ITYPE=3, then: */

/*          RESULT(1) = | I - VU' | / ( n ulp ) */

/*  Packed storage means that, for example, if UPLO='U', then the columns */
/*  of the upper triangle of A are stored one after another, so that */
/*  A(1,j+1) immediately follows A(j,j) in the array AP.  Similarly, if */
/*  UPLO='L', then the columns of the lower triangle of A are stored one */
/*  after another in AP, so that A(j+1,j+1) immediately follows A(n,j) */
/*  in the array AP.  This means that A(i,j) is stored in: */

/*     AP( i + j*(j-1)/2 )                 if UPLO='U' */

/*     AP( i + (2*n-j)*(j-1)/2 )           if UPLO='L' */

/*  The array VP bears the same relation to the matrix V that A does to */
/*  AP. */

/*  For ITYPE > 1, the transformation U is expressed as a product */
/*  of Householder transformations: */

/*     If UPLO='U', then  V = H(n-1)...H(1),  where */

/*         H(j) = I  -  tau(j) v(j) v(j)' */

/*     and the first j-1 elements of v(j) are stored in V(1:j-1,j+1), */
/*     (i.e., VP( j*(j+1)/2 + 1 : j*(j+1)/2 + j-1 ) ), */
/*     the j-th element is 1, and the last n-j elements are 0. */

/*     If UPLO='L', then  V = H(1)...H(n-1),  where */

/*         H(j) = I  -  tau(j) v(j) v(j)' */

/*     and the first j elements of v(j) are 0, the (j+1)-st is 1, and the */
/*     (j+2)-nd through n-th elements are stored in V(j+2:n,j) (i.e., */
/*     in VP( (2*n-j)*(j-1)/2 + j+2 : (2*n-j)*(j-1)/2 + n ) .) */

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
/*          If UPLO='U', AP and VP are considered to contain the upper */
/*          triangle of A and V. */
/*          If UPLO='L', AP and VP are considered to contain the lower */
/*          triangle of A and V. */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, SSPT21 does nothing. */
/*          It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix.  It may only be zero or one. */
/*          If zero, then S is diagonal, and E is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AP      (input) REAL array, dimension (N*(N+1)/2) */
/*          The original (unfactored) matrix.  It is assumed to be */
/*          symmetric, and contains the columns of just the upper */
/*          triangle (UPLO='U') or only the lower triangle (UPLO='L'), */
/*          packed one after another. */

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

/*  VP      (input) REAL array, dimension (N*(N+1)/2) */
/*          If ITYPE=2 or 3, the columns of this array contain the */
/*          Householder vectors used to describe the orthogonal matrix */
/*          in the decomposition, as described in purpose. */
/*          *NOTE* If ITYPE=2 or 3, V is modified and restored.  The */
/*          subdiagonal (if UPLO='L') or the superdiagonal (if UPLO='U') */
/*          is set to one, and later reset to its original value, during */
/*          the course of the calculation. */
/*          If ITYPE=1, then it is neither referenced nor modified. */

/*  TAU     (input) REAL array, dimension (N) */
/*          If ITYPE >= 2, then TAU(j) is the scalar factor of */
/*          v(j) v(j)' in the Householder transformation H(j) of */
/*          the product  U = H(1)...H(n-2) */
/*          If ITYPE < 2, then TAU is not referenced. */

/*  WORK    (workspace) REAL array, dimension (N**2+N) */
/*          Workspace. */

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

/*     1)      Constants */

    /* Parameter adjustments */
    --ap;
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --vp;
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

    lap = *n * (*n + 1) / 2;

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
	r__1 = slansp_("1", cuplo, n, &ap[1], &work[1]);
	anorm = dmax(r__1,unfl);
    }

/*     Compute error matrix: */

    if (*itype == 1) {

/*        ITYPE=1: error = A - U S U' */

	slaset_("Full", n, n, &c_b10, &c_b10, &work[1], n);
	scopy_(&lap, &ap[1], &c__1, &work[1], &c__1);

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    r__1 = -d__[j];
	    sspr_(cuplo, n, &r__1, &u[j * u_dim1 + 1], &c__1, &work[1]);
/* L10: */
	}

	if (*n > 1 && *kband == 1) {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		r__1 = -e[j];
		sspr2_(cuplo, n, &r__1, &u[j * u_dim1 + 1], &c__1, &u[(j + 1) 
			* u_dim1 + 1], &c__1, &work[1]);
/* L20: */
	    }
	}
/* Computing 2nd power */
	i__1 = *n;
	wnorm = slansp_("1", cuplo, n, &work[1], &work[i__1 * i__1 + 1]);

    } else if (*itype == 2) {

/*        ITYPE=2: error = V S V' - A */

	slaset_("Full", n, n, &c_b10, &c_b10, &work[1], n);

	if (lower) {
	    work[lap] = d__[*n];
	    for (j = *n - 1; j >= 1; --j) {
		jp = ((*n << 1) - j) * (j - 1) / 2;
		jp1 = jp + *n - j;
		if (*kband == 1) {
		    work[jp + j + 1] = (1.f - tau[j]) * e[j];
		    i__1 = *n;
		    for (jr = j + 2; jr <= i__1; ++jr) {
			work[jp + jr] = -tau[j] * e[j] * vp[jp + jr];
/* L30: */
		    }
		}

		if (tau[j] != 0.f) {
		    vsave = vp[jp + j + 1];
		    vp[jp + j + 1] = 1.f;
		    i__1 = *n - j;
		    sspmv_("L", &i__1, &c_b26, &work[jp1 + j + 1], &vp[jp + j 
			    + 1], &c__1, &c_b10, &work[lap + 1], &c__1);
		    i__1 = *n - j;
		    temp = tau[j] * -.5f * sdot_(&i__1, &work[lap + 1], &c__1, 
			     &vp[jp + j + 1], &c__1);
		    i__1 = *n - j;
		    saxpy_(&i__1, &temp, &vp[jp + j + 1], &c__1, &work[lap + 
			    1], &c__1);
		    i__1 = *n - j;
		    r__1 = -tau[j];
		    sspr2_("L", &i__1, &r__1, &vp[jp + j + 1], &c__1, &work[
			    lap + 1], &c__1, &work[jp1 + j + 1]);
		    vp[jp + j + 1] = vsave;
		}
		work[jp + j] = d__[j];
/* L40: */
	    }
	} else {
	    work[1] = d__[1];
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		jp = j * (j - 1) / 2;
		jp1 = jp + j;
		if (*kband == 1) {
		    work[jp1 + j] = (1.f - tau[j]) * e[j];
		    i__2 = j - 1;
		    for (jr = 1; jr <= i__2; ++jr) {
			work[jp1 + jr] = -tau[j] * e[j] * vp[jp1 + jr];
/* L50: */
		    }
		}

		if (tau[j] != 0.f) {
		    vsave = vp[jp1 + j];
		    vp[jp1 + j] = 1.f;
		    sspmv_("U", &j, &c_b26, &work[1], &vp[jp1 + 1], &c__1, &
			    c_b10, &work[lap + 1], &c__1);
		    temp = tau[j] * -.5f * sdot_(&j, &work[lap + 1], &c__1, &
			    vp[jp1 + 1], &c__1);
		    saxpy_(&j, &temp, &vp[jp1 + 1], &c__1, &work[lap + 1], &
			    c__1);
		    r__1 = -tau[j];
		    sspr2_("U", &j, &r__1, &vp[jp1 + 1], &c__1, &work[lap + 1]
, &c__1, &work[1]);
		    vp[jp1 + j] = vsave;
		}
		work[jp1 + j + 1] = d__[j + 1];
/* L60: */
	    }
	}

	i__1 = lap;
	for (j = 1; j <= i__1; ++j) {
	    work[j] -= ap[j];
/* L70: */
	}
	wnorm = slansp_("1", cuplo, n, &work[1], &work[lap + 1]);

    } else if (*itype == 3) {

/*        ITYPE=3: error = U V' - I */

	if (*n < 2) {
	    return 0;
	}
	slacpy_(" ", n, n, &u[u_offset], ldu, &work[1], n);
/* Computing 2nd power */
	i__1 = *n;
	sopmtr_("R", cuplo, "T", n, n, &vp[1], &tau[1], &work[1], n, &work[
		i__1 * i__1 + 1], &iinfo);
	if (iinfo != 0) {
	    result[1] = 10.f / ulp;
	    return 0;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[(*n + 1) * (j - 1) + 1] += -1.f;
/* L80: */
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
	sgemm_("N", "C", n, n, n, &c_b26, &u[u_offset], ldu, &u[u_offset], 
		ldu, &c_b10, &work[1], n);

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[(*n + 1) * (j - 1) + 1] += -1.f;
/* L90: */
	}

/* Computing MIN */
/* Computing 2nd power */
	i__1 = *n;
	r__1 = slange_("1", n, n, &work[1], n, &work[i__1 * i__1 + 1]), r__2 = (real) (*n);
	result[2] = dmin(r__1,r__2) / (*n * ulp);
    }

    return 0;

/*     End of SSPT21 */

} /* sspt21_ */
