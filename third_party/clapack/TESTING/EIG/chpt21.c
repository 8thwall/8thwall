#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__1 = 1;

/* Subroutine */ int chpt21_(integer *itype, char *uplo, integer *n, integer *
	kband, complex *ap, real *d__, real *e, complex *u, integer *ldu, 
	complex *vp, complex *tau, complex *work, real *rwork, real *result)
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1, i__2, i__3, i__4, i__5, i__6;
    real r__1, r__2;
    complex q__1, q__2, q__3;

    /* Local variables */
    integer j, jp, jr, jp1, lap;
    real ulp;
    extern /* Subroutine */ int chpr_(char *, integer *, real *, complex *, 
	    integer *, complex *);
    real unfl;
    complex temp;
    extern /* Subroutine */ int chpr2_(char *, integer *, complex *, complex *
, integer *, complex *, integer *, complex *), cgemm_(
	    char *, char *, integer *, integer *, integer *, complex *, 
	    complex *, integer *, complex *, integer *, complex *, complex *, 
	    integer *);
    extern /* Complex */ VOID cdotc_(complex *, integer *, complex *, integer 
	    *, complex *, integer *);
    extern logical lsame_(char *, char *);
    integer iinfo;
    real anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), chpmv_(char *, integer *, complex *, 
	    complex *, complex *, integer *, complex *, complex *, integer *);
    char cuplo[1];
    complex vsave;
    extern /* Subroutine */ int caxpy_(integer *, complex *, complex *, 
	    integer *, complex *, integer *);
    logical lower;
    real wnorm;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), clanhp_(char *, char *, integer *, 
	    complex *, real *), slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *), cupmtr_(char *, char *, char *, integer *, integer *, 
	    complex *, complex *, complex *, integer *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CHPT21  generally checks a decomposition of the form */

/*          A = U S U* */

/*  where * means conjugate transpose, A is hermitian, U is */
/*  unitary, and S is diagonal (if KBAND=0) or (real) symmetric */
/*  tridiagonal (if KBAND=1).  If ITYPE=1, then U is represented as */
/*  a dense matrix, otherwise the U is expressed as a product of */
/*  Householder transformations, whose vectors are stored in the */
/*  array "V" and whose scaling constants are in "TAU"; we shall */
/*  use the letter "V" to refer to the product of Householder */
/*  transformations (which should be equal to U). */

/*  Specifically, if ITYPE=1, then: */

/*          RESULT(1) = | A - U S U* | / ( |A| n ulp ) *and* */
/*          RESULT(2) = | I - UU* | / ( n ulp ) */

/*  If ITYPE=2, then: */

/*          RESULT(1) = | A - V S V* | / ( |A| n ulp ) */

/*  If ITYPE=3, then: */

/*          RESULT(1) = | I - UV* | / ( n ulp ) */

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

/*         H(j) = I  -  tau(j) v(j) v(j)* */

/*     and the first j-1 elements of v(j) are stored in V(1:j-1,j+1), */
/*     (i.e., VP( j*(j+1)/2 + 1 : j*(j+1)/2 + j-1 ) ), */
/*     the j-th element is 1, and the last n-j elements are 0. */

/*     If UPLO='L', then  V = H(1)...H(n-1),  where */

/*         H(j) = I  -  tau(j) v(j) v(j)* */

/*     and the first j elements of v(j) are 0, the (j+1)-st is 1, and the */
/*     (j+2)-nd through n-th elements are stored in V(j+2:n,j) (i.e., */
/*     in VP( (2*n-j)*(j-1)/2 + j+2 : (2*n-j)*(j-1)/2 + n ) .) */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the type of tests to be performed. */
/*          1: U expressed as a dense unitary matrix: */
/*             RESULT(1) = | A - U S U* | / ( |A| n ulp )   *and* */
/*             RESULT(2) = | I - UU* | / ( n ulp ) */

/*          2: U expressed as a product V of Housholder transformations: */
/*             RESULT(1) = | A - V S V* | / ( |A| n ulp ) */

/*          3: U expressed both as a dense unitary matrix and */
/*             as a product of Housholder transformations: */
/*             RESULT(1) = | I - UV* | / ( n ulp ) */

/*  UPLO    (input) CHARACTER */
/*          If UPLO='U', the upper triangle of A and V will be used and */
/*          the (strictly) lower triangle will not be referenced. */
/*          If UPLO='L', the lower triangle of A and V will be used and */
/*          the (strictly) upper triangle will not be referenced. */

/*  N       (input) INTEGER */
/*          The size of the matrix.  If it is zero, CHPT21 does nothing. */
/*          It must be at least zero. */

/*  KBAND   (input) INTEGER */
/*          The bandwidth of the matrix.  It may only be zero or one. */
/*          If zero, then S is diagonal, and E is not referenced.  If */
/*          one, then S is symmetric tri-diagonal. */

/*  AP      (input) COMPLEX array, dimension (N*(N+1)/2) */
/*          The original (unfactored) matrix.  It is assumed to be */
/*          hermitian, and contains the columns of just the upper */
/*          triangle (UPLO='U') or only the lower triangle (UPLO='L'), */
/*          packed one after another. */

/*  D       (input) REAL array, dimension (N) */
/*          The diagonal of the (symmetric tri-) diagonal matrix. */

/*  E       (input) REAL array, dimension (N) */
/*          The off-diagonal of the (symmetric tri-) diagonal matrix. */
/*          E(1) is the (1,2) and (2,1) element, E(2) is the (2,3) and */
/*          (3,2) element, etc. */
/*          Not referenced if KBAND=0. */

/*  U       (input) COMPLEX array, dimension (LDU, N) */
/*          If ITYPE=1 or 3, this contains the unitary matrix in */
/*          the decomposition, expressed as a dense matrix.  If ITYPE=2, */
/*          then it is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  LDU must be at least N and */
/*          at least 1. */

/*  VP      (input) REAL array, dimension (N*(N+1)/2) */
/*          If ITYPE=2 or 3, the columns of this array contain the */
/*          Householder vectors used to describe the unitary matrix */
/*          in the decomposition, as described in purpose. */
/*          *NOTE* If ITYPE=2 or 3, V is modified and restored.  The */
/*          subdiagonal (if UPLO='L') or the superdiagonal (if UPLO='U') */
/*          is set to one, and later reset to its original value, during */
/*          the course of the calculation. */
/*          If ITYPE=1, then it is neither referenced nor modified. */

/*  TAU     (input) COMPLEX array, dimension (N) */
/*          If ITYPE >= 2, then TAU(j) is the scalar factor of */
/*          v(j) v(j)* in the Householder transformation H(j) of */
/*          the product  U = H(1)...H(n-2) */
/*          If ITYPE < 2, then TAU is not referenced. */

/*  WORK    (workspace) COMPLEX array, dimension (N**2) */
/*          Workspace. */

/*  RWORK   (workspace) REAL array, dimension (N) */
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

/*     Constants */

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
    --rwork;
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
	r__1 = clanhp_("1", cuplo, n, &ap[1], &rwork[1])
		;
	anorm = dmax(r__1,unfl);
    }

/*     Compute error matrix: */

    if (*itype == 1) {

/*        ITYPE=1: error = A - U S U* */

	claset_("Full", n, n, &c_b1, &c_b1, &work[1], n);
	ccopy_(&lap, &ap[1], &c__1, &work[1], &c__1);

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    r__1 = -d__[j];
	    chpr_(cuplo, n, &r__1, &u[j * u_dim1 + 1], &c__1, &work[1]);
/* L10: */
	}

	if (*n > 1 && *kband == 1) {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j;
		q__2.r = e[i__2], q__2.i = 0.f;
		q__1.r = -q__2.r, q__1.i = -q__2.i;
		chpr2_(cuplo, n, &q__1, &u[j * u_dim1 + 1], &c__1, &u[(j - 1) 
			* u_dim1 + 1], &c__1, &work[1]);
/* L20: */
	    }
	}
	wnorm = clanhp_("1", cuplo, n, &work[1], &rwork[1]);

    } else if (*itype == 2) {

/*        ITYPE=2: error = V S V* - A */

	claset_("Full", n, n, &c_b1, &c_b1, &work[1], n);

	if (lower) {
	    i__1 = lap;
	    i__2 = *n;
	    work[i__1].r = d__[i__2], work[i__1].i = 0.f;
	    for (j = *n - 1; j >= 1; --j) {
		jp = ((*n << 1) - j) * (j - 1) / 2;
		jp1 = jp + *n - j;
		if (*kband == 1) {
		    i__1 = jp + j + 1;
		    i__2 = j;
		    q__2.r = 1.f - tau[i__2].r, q__2.i = 0.f - tau[i__2].i;
		    i__3 = j;
		    q__1.r = e[i__3] * q__2.r, q__1.i = e[i__3] * q__2.i;
		    work[i__1].r = q__1.r, work[i__1].i = q__1.i;
		    i__1 = *n;
		    for (jr = j + 2; jr <= i__1; ++jr) {
			i__2 = jp + jr;
			i__3 = j;
			q__3.r = -tau[i__3].r, q__3.i = -tau[i__3].i;
			i__4 = j;
			q__2.r = e[i__4] * q__3.r, q__2.i = e[i__4] * q__3.i;
			i__5 = jp + jr;
			q__1.r = q__2.r * vp[i__5].r - q__2.i * vp[i__5].i, 
				q__1.i = q__2.r * vp[i__5].i + q__2.i * vp[
				i__5].r;
			work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L30: */
		    }
		}

		i__1 = j;
		if (tau[i__1].r != 0.f || tau[i__1].i != 0.f) {
		    i__1 = jp + j + 1;
		    vsave.r = vp[i__1].r, vsave.i = vp[i__1].i;
		    i__1 = jp + j + 1;
		    vp[i__1].r = 1.f, vp[i__1].i = 0.f;
		    i__1 = *n - j;
		    chpmv_("L", &i__1, &c_b2, &work[jp1 + j + 1], &vp[jp + j 
			    + 1], &c__1, &c_b1, &work[lap + 1], &c__1);
		    i__1 = j;
		    q__2.r = tau[i__1].r * -.5f, q__2.i = tau[i__1].i * -.5f;
		    i__2 = *n - j;
		    cdotc_(&q__3, &i__2, &work[lap + 1], &c__1, &vp[jp + j + 
			    1], &c__1);
		    q__1.r = q__2.r * q__3.r - q__2.i * q__3.i, q__1.i = 
			    q__2.r * q__3.i + q__2.i * q__3.r;
		    temp.r = q__1.r, temp.i = q__1.i;
		    i__1 = *n - j;
		    caxpy_(&i__1, &temp, &vp[jp + j + 1], &c__1, &work[lap + 
			    1], &c__1);
		    i__1 = *n - j;
		    i__2 = j;
		    q__1.r = -tau[i__2].r, q__1.i = -tau[i__2].i;
		    chpr2_("L", &i__1, &q__1, &vp[jp + j + 1], &c__1, &work[
			    lap + 1], &c__1, &work[jp1 + j + 1]);

		    i__1 = jp + j + 1;
		    vp[i__1].r = vsave.r, vp[i__1].i = vsave.i;
		}
		i__1 = jp + j;
		i__2 = j;
		work[i__1].r = d__[i__2], work[i__1].i = 0.f;
/* L40: */
	    }
	} else {
	    work[1].r = d__[1], work[1].i = 0.f;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		jp = j * (j - 1) / 2;
		jp1 = jp + j;
		if (*kband == 1) {
		    i__2 = jp1 + j;
		    i__3 = j;
		    q__2.r = 1.f - tau[i__3].r, q__2.i = 0.f - tau[i__3].i;
		    i__4 = j;
		    q__1.r = e[i__4] * q__2.r, q__1.i = e[i__4] * q__2.i;
		    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
		    i__2 = j - 1;
		    for (jr = 1; jr <= i__2; ++jr) {
			i__3 = jp1 + jr;
			i__4 = j;
			q__3.r = -tau[i__4].r, q__3.i = -tau[i__4].i;
			i__5 = j;
			q__2.r = e[i__5] * q__3.r, q__2.i = e[i__5] * q__3.i;
			i__6 = jp1 + jr;
			q__1.r = q__2.r * vp[i__6].r - q__2.i * vp[i__6].i, 
				q__1.i = q__2.r * vp[i__6].i + q__2.i * vp[
				i__6].r;
			work[i__3].r = q__1.r, work[i__3].i = q__1.i;
/* L50: */
		    }
		}

		i__2 = j;
		if (tau[i__2].r != 0.f || tau[i__2].i != 0.f) {
		    i__2 = jp1 + j;
		    vsave.r = vp[i__2].r, vsave.i = vp[i__2].i;
		    i__2 = jp1 + j;
		    vp[i__2].r = 1.f, vp[i__2].i = 0.f;
		    chpmv_("U", &j, &c_b2, &work[1], &vp[jp1 + 1], &c__1, &
			    c_b1, &work[lap + 1], &c__1);
		    i__2 = j;
		    q__2.r = tau[i__2].r * -.5f, q__2.i = tau[i__2].i * -.5f;
		    cdotc_(&q__3, &j, &work[lap + 1], &c__1, &vp[jp1 + 1], &
			    c__1);
		    q__1.r = q__2.r * q__3.r - q__2.i * q__3.i, q__1.i = 
			    q__2.r * q__3.i + q__2.i * q__3.r;
		    temp.r = q__1.r, temp.i = q__1.i;
		    caxpy_(&j, &temp, &vp[jp1 + 1], &c__1, &work[lap + 1], &
			    c__1);
		    i__2 = j;
		    q__1.r = -tau[i__2].r, q__1.i = -tau[i__2].i;
		    chpr2_("U", &j, &q__1, &vp[jp1 + 1], &c__1, &work[lap + 1]
, &c__1, &work[1]);
		    i__2 = jp1 + j;
		    vp[i__2].r = vsave.r, vp[i__2].i = vsave.i;
		}
		i__2 = jp1 + j + 1;
		i__3 = j + 1;
		work[i__2].r = d__[i__3], work[i__2].i = 0.f;
/* L60: */
	    }
	}

	i__1 = lap;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    i__3 = j;
	    i__4 = j;
	    q__1.r = work[i__3].r - ap[i__4].r, q__1.i = work[i__3].i - ap[
		    i__4].i;
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L70: */
	}
	wnorm = clanhp_("1", cuplo, n, &work[1], &rwork[1]);

    } else if (*itype == 3) {

/*        ITYPE=3: error = U V* - I */

	if (*n < 2) {
	    return 0;
	}
	clacpy_(" ", n, n, &u[u_offset], ldu, &work[1], n);
/* Computing 2nd power */
	i__1 = *n;
	cupmtr_("R", cuplo, "C", n, n, &vp[1], &tau[1], &work[1], n, &work[
		i__1 * i__1 + 1], &iinfo);
	if (iinfo != 0) {
	    result[1] = 10.f / ulp;
	    return 0;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = (*n + 1) * (j - 1) + 1;
	    i__3 = (*n + 1) * (j - 1) + 1;
	    q__1.r = work[i__3].r - 1.f, q__1.i = work[i__3].i - 0.f;
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L80: */
	}

	wnorm = clange_("1", n, n, &work[1], n, &rwork[1]);
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

/*     Compute  UU* - I */

    if (*itype == 1) {
	cgemm_("N", "C", n, n, n, &c_b2, &u[u_offset], ldu, &u[u_offset], ldu, 
		 &c_b1, &work[1], n);

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = (*n + 1) * (j - 1) + 1;
	    i__3 = (*n + 1) * (j - 1) + 1;
	    q__1.r = work[i__3].r - 1.f, q__1.i = work[i__3].i - 0.f;
	    work[i__2].r = q__1.r, work[i__2].i = q__1.i;
/* L90: */
	}

/* Computing MIN */
	r__1 = clange_("1", n, n, &work[1], n, &rwork[1]), r__2 = (
		real) (*n);
	result[2] = dmin(r__1,r__2) / (*n * ulp);
    }

    return 0;

/*     End of CHPT21 */

} /* chpt21_ */
