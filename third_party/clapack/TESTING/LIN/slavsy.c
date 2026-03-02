#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b15 = 1.f;
static integer c__1 = 1;

/* Subroutine */ int slavsy_(char *uplo, char *trans, char *diag, integer *n, 
	integer *nrhs, real *a, integer *lda, integer *ipiv, real *b, integer 
	*ldb, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    integer j, k;
    real t1, t2, d11, d12, d21, d22;
    integer kp;
    extern /* Subroutine */ int sger_(integer *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *), 
	    sgemv_(char *, integer *, integer *, real *, real *, integer *, 
	    real *, integer *, real *, real *, integer *), sswap_(
	    integer *, real *, integer *, real *, integer *), xerbla_(char *, 
	    integer *);
    logical nounit;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLAVSY  performs one of the matrix-vector operations */
/*     x := A*x  or  x := A'*x, */
/*  where x is an N element vector and A is one of the factors */
/*  from the block U*D*U' or L*D*L' factorization computed by SSYTRF. */

/*  If TRANS = 'N', multiplies by U  or U * D  (or L  or L * D) */
/*  If TRANS = 'T', multiplies by U' or D * U' (or L' or D * L') */
/*  If TRANS = 'C', multiplies by U' or D * U' (or L' or D * L') */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the factor stored in A is upper or lower */
/*          triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation to be performed: */
/*          = 'N':  x := A*x */
/*          = 'T':  x := A'*x */
/*          = 'C':  x := A'*x */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the diagonal blocks are unit */
/*          matrices.  If the diagonal blocks are assumed to be unit, */
/*          then A = U or A = L, otherwise A = U*D or A = L*D. */
/*          = 'U':  Diagonal blocks are assumed to be unit matrices. */
/*          = 'N':  Diagonal blocks are assumed to be non-unit matrices. */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of vectors */
/*          x to be multiplied by A.  NRHS >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The block diagonal matrix D and the multipliers used to */
/*          obtain the factor U or L as computed by SSYTRF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from SSYTRF. */

/*  B       (input/output) REAL array, dimension (LDB,NRHS) */
/*          On entry, B contains NRHS vectors of length N. */
/*          On exit, B is overwritten with the product A * B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
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

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! lsame_(trans, "N") && ! lsame_(trans, 
	    "T") && ! lsame_(trans, "C")) {
	*info = -2;
    } else if (! lsame_(diag, "U") && ! lsame_(diag, 
	    "N")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < max(1,*n)) {
	*info = -6;
    } else if (*ldb < max(1,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAVSY ", &i__1);
	return 0;
    }

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    }

    nounit = lsame_(diag, "N");
/* ------------------------------------------ */

/*     Compute  B := A * B  (No transpose) */

/* ------------------------------------------ */
    if (lsame_(trans, "N")) {

/*        Compute  B := U*B */
/*        where U = P(m)*inv(U(m))* ... *P(1)*inv(U(1)) */

	if (lsame_(uplo, "U")) {

/*        Loop forward applying the transformations. */

	    k = 1;
L10:
	    if (k > *n) {
		goto L30;
	    }
	    if (ipiv[k] > 0) {

/*              1 x 1 pivot block */

/*              Multiply by the diagonal element if forming U * D. */

		if (nounit) {
		    sscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}

/*              Multiply by  P(K) * inv(U(K))  if K > 1. */

		if (k > 1) {

/*                 Apply the transformation. */

		    i__1 = k - 1;
		    sger_(&i__1, nrhs, &c_b15, &a[k * a_dim1 + 1], &c__1, &b[
			    k + b_dim1], ldb, &b[b_dim1 + 1], ldb);

/*                 Interchange if P(K) .ne. I. */

		    kp = ipiv[k];
		    if (kp != k) {
			sswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }
		}
		++k;
	    } else {

/*              2 x 2 pivot block */

/*              Multiply by the diagonal block if forming U * D. */

		if (nounit) {
		    d11 = a[k + k * a_dim1];
		    d22 = a[k + 1 + (k + 1) * a_dim1];
		    d12 = a[k + (k + 1) * a_dim1];
		    d21 = d12;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			t1 = b[k + j * b_dim1];
			t2 = b[k + 1 + j * b_dim1];
			b[k + j * b_dim1] = d11 * t1 + d12 * t2;
			b[k + 1 + j * b_dim1] = d21 * t1 + d22 * t2;
/* L20: */
		    }
		}

/*              Multiply by  P(K) * inv(U(K))  if K > 1. */

		if (k > 1) {

/*                 Apply the transformations. */

		    i__1 = k - 1;
		    sger_(&i__1, nrhs, &c_b15, &a[k * a_dim1 + 1], &c__1, &b[
			    k + b_dim1], ldb, &b[b_dim1 + 1], ldb);
		    i__1 = k - 1;
		    sger_(&i__1, nrhs, &c_b15, &a[(k + 1) * a_dim1 + 1], &
			    c__1, &b[k + 1 + b_dim1], ldb, &b[b_dim1 + 1], 
			    ldb);

/*                 Interchange if P(K) .ne. I. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k) {
			sswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }
		}
		k += 2;
	    }
	    goto L10;
L30:

/*        Compute  B := L*B */
/*        where L = P(1)*inv(L(1))* ... *P(m)*inv(L(m)) . */

	    ;
	} else {

/*           Loop backward applying the transformations to B. */

	    k = *n;
L40:
	    if (k < 1) {
		goto L60;
	    }

/*           Test the pivot index.  If greater than zero, a 1 x 1 */
/*           pivot was used, otherwise a 2 x 2 pivot was used. */

	    if (ipiv[k] > 0) {

/*              1 x 1 pivot block: */

/*              Multiply by the diagonal element if forming L * D. */

		if (nounit) {
		    sscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}

/*              Multiply by  P(K) * inv(L(K))  if K < N. */

		if (k != *n) {
		    kp = ipiv[k];

/*                 Apply the transformation. */

		    i__1 = *n - k;
		    sger_(&i__1, nrhs, &c_b15, &a[k + 1 + k * a_dim1], &c__1, 
			    &b[k + b_dim1], ldb, &b[k + 1 + b_dim1], ldb);

/*                 Interchange if a permutation was applied at the */
/*                 K-th step of the factorization. */

		    if (kp != k) {
			sswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }
		}
		--k;

	    } else {

/*              2 x 2 pivot block: */

/*              Multiply by the diagonal block if forming L * D. */

		if (nounit) {
		    d11 = a[k - 1 + (k - 1) * a_dim1];
		    d22 = a[k + k * a_dim1];
		    d21 = a[k + (k - 1) * a_dim1];
		    d12 = d21;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			t1 = b[k - 1 + j * b_dim1];
			t2 = b[k + j * b_dim1];
			b[k - 1 + j * b_dim1] = d11 * t1 + d12 * t2;
			b[k + j * b_dim1] = d21 * t1 + d22 * t2;
/* L50: */
		    }
		}

/*              Multiply by  P(K) * inv(L(K))  if K < N. */

		if (k != *n) {

/*                 Apply the transformation. */

		    i__1 = *n - k;
		    sger_(&i__1, nrhs, &c_b15, &a[k + 1 + k * a_dim1], &c__1, 
			    &b[k + b_dim1], ldb, &b[k + 1 + b_dim1], ldb);
		    i__1 = *n - k;
		    sger_(&i__1, nrhs, &c_b15, &a[k + 1 + (k - 1) * a_dim1], &
			    c__1, &b[k - 1 + b_dim1], ldb, &b[k + 1 + b_dim1], 
			     ldb);

/*                 Interchange if a permutation was applied at the */
/*                 K-th step of the factorization. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k) {
			sswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }
		}
		k += -2;
	    }
	    goto L40;
L60:
	    ;
	}
/* ---------------------------------------- */

/*     Compute  B := A' * B  (transpose) */

/* ---------------------------------------- */
    } else {

/*        Form  B := U'*B */
/*        where U  = P(m)*inv(U(m))* ... *P(1)*inv(U(1)) */
/*        and   U' = inv(U'(1))*P(1)* ... *inv(U'(m))*P(m) */

	if (lsame_(uplo, "U")) {

/*           Loop backward applying the transformations. */

	    k = *n;
L70:
	    if (k < 1) {
		goto L90;
	    }

/*           1 x 1 pivot block. */

	    if (ipiv[k] > 0) {
		if (k > 1) {

/*                 Interchange if P(K) .ne. I. */

		    kp = ipiv[k];
		    if (kp != k) {
			sswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }

/*                 Apply the transformation */

		    i__1 = k - 1;
		    sgemv_("Transpose", &i__1, nrhs, &c_b15, &b[b_offset], 
			    ldb, &a[k * a_dim1 + 1], &c__1, &c_b15, &b[k + 
			    b_dim1], ldb);
		}
		if (nounit) {
		    sscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}
		--k;

/*           2 x 2 pivot block. */

	    } else {
		if (k > 2) {

/*                 Interchange if P(K) .ne. I. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k - 1) {
			sswap_(nrhs, &b[k - 1 + b_dim1], ldb, &b[kp + b_dim1], 
				 ldb);
		    }

/*                 Apply the transformations */

		    i__1 = k - 2;
		    sgemv_("Transpose", &i__1, nrhs, &c_b15, &b[b_offset], 
			    ldb, &a[k * a_dim1 + 1], &c__1, &c_b15, &b[k + 
			    b_dim1], ldb);
		    i__1 = k - 2;
		    sgemv_("Transpose", &i__1, nrhs, &c_b15, &b[b_offset], 
			    ldb, &a[(k - 1) * a_dim1 + 1], &c__1, &c_b15, &b[
			    k - 1 + b_dim1], ldb);
		}

/*              Multiply by the diagonal block if non-unit. */

		if (nounit) {
		    d11 = a[k - 1 + (k - 1) * a_dim1];
		    d22 = a[k + k * a_dim1];
		    d12 = a[k - 1 + k * a_dim1];
		    d21 = d12;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			t1 = b[k - 1 + j * b_dim1];
			t2 = b[k + j * b_dim1];
			b[k - 1 + j * b_dim1] = d11 * t1 + d12 * t2;
			b[k + j * b_dim1] = d21 * t1 + d22 * t2;
/* L80: */
		    }
		}
		k += -2;
	    }
	    goto L70;
L90:

/*        Form  B := L'*B */
/*        where L  = P(1)*inv(L(1))* ... *P(m)*inv(L(m)) */
/*        and   L' = inv(L'(m))*P(m)* ... *inv(L'(1))*P(1) */

	    ;
	} else {

/*           Loop forward applying the L-transformations. */

	    k = 1;
L100:
	    if (k > *n) {
		goto L120;
	    }

/*           1 x 1 pivot block */

	    if (ipiv[k] > 0) {
		if (k < *n) {

/*                 Interchange if P(K) .ne. I. */

		    kp = ipiv[k];
		    if (kp != k) {
			sswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }

/*                 Apply the transformation */

		    i__1 = *n - k;
		    sgemv_("Transpose", &i__1, nrhs, &c_b15, &b[k + 1 + 
			    b_dim1], ldb, &a[k + 1 + k * a_dim1], &c__1, &
			    c_b15, &b[k + b_dim1], ldb);
		}
		if (nounit) {
		    sscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}
		++k;

/*           2 x 2 pivot block. */

	    } else {
		if (k < *n - 1) {

/*              Interchange if P(K) .ne. I. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k + 1) {
			sswap_(nrhs, &b[k + 1 + b_dim1], ldb, &b[kp + b_dim1], 
				 ldb);
		    }

/*                 Apply the transformation */

		    i__1 = *n - k - 1;
		    sgemv_("Transpose", &i__1, nrhs, &c_b15, &b[k + 2 + 
			    b_dim1], ldb, &a[k + 2 + (k + 1) * a_dim1], &c__1, 
			     &c_b15, &b[k + 1 + b_dim1], ldb);
		    i__1 = *n - k - 1;
		    sgemv_("Transpose", &i__1, nrhs, &c_b15, &b[k + 2 + 
			    b_dim1], ldb, &a[k + 2 + k * a_dim1], &c__1, &
			    c_b15, &b[k + b_dim1], ldb);
		}

/*              Multiply by the diagonal block if non-unit. */

		if (nounit) {
		    d11 = a[k + k * a_dim1];
		    d22 = a[k + 1 + (k + 1) * a_dim1];
		    d21 = a[k + 1 + k * a_dim1];
		    d12 = d21;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			t1 = b[k + j * b_dim1];
			t2 = b[k + 1 + j * b_dim1];
			b[k + j * b_dim1] = d11 * t1 + d12 * t2;
			b[k + 1 + j * b_dim1] = d21 * t1 + d22 * t2;
/* L110: */
		    }
		}
		k += 2;
	    }
	    goto L100;
L120:
	    ;
	}

    }
    return 0;

/*     End of SLAVSY */

} /* slavsy_ */
