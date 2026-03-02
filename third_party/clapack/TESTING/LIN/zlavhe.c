#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {1.,0.};
static integer c__1 = 1;

/* Subroutine */ int zlavhe_(char *uplo, char *trans, char *diag, integer *n, 
	integer *nrhs, doublecomplex *a, integer *lda, integer *ipiv, 
	doublecomplex *b, integer *ldb, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    doublecomplex z__1, z__2, z__3;

    /* Builtin functions */
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    integer j, k;
    doublecomplex t1, t2, d11, d12, d21, d22;
    integer kp;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int zscal_(integer *, doublecomplex *, 
	    doublecomplex *, integer *), zgemv_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *), 
	    zgeru_(integer *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *)
	    , zswap_(integer *, doublecomplex *, integer *, doublecomplex *, 
	    integer *), xerbla_(char *, integer *), zlacgv_(integer *, 
	     doublecomplex *, integer *);
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

/*     ZLAVHE  performs one of the matrix-vector operations */
/*        x := A*x  or  x := A^H*x, */
/*     where x is an N element vector and  A is one of the factors */
/*     from the symmetric factorization computed by ZHETRF. */
/*     ZHETRF produces a factorization of the form */
/*          U * D * U^H     or     L * D * L^H, */
/*     where U (or L) is a product of permutation and unit upper (lower) */
/*     triangular matrices, U^H (or L^H) is the conjugate transpose of */
/*     U (or L), and D is Hermitian and block diagonal with 1 x 1 and */
/*     2 x 2 diagonal blocks.  The multipliers for the transformations */
/*     and the upper or lower triangular parts of the diagonal blocks */
/*     are stored in the leading upper or lower triangle of the 2-D */
/*     array A. */

/*     If TRANS = 'N' or 'n', ZLAVHE multiplies either by U or U * D */
/*     (or L or L * D). */
/*     If TRANS = 'C' or 'c', ZLAVHE multiplies either by U^H or D * U^H */
/*     (or L^H or D * L^H ). */

/*  Arguments */
/*  ========== */

/*  UPLO   - CHARACTER*1 */
/*           On entry, UPLO specifies whether the triangular matrix */
/*           stored in A is upper or lower triangular. */
/*              UPLO = 'U' or 'u'   The matrix is upper triangular. */
/*              UPLO = 'L' or 'l'   The matrix is lower triangular. */
/*           Unchanged on exit. */

/*  TRANS  - CHARACTER*1 */
/*           On entry, TRANS specifies the operation to be performed as */
/*           follows: */
/*              TRANS = 'N' or 'n'   x := A*x. */
/*              TRANS = 'C' or 'c'   x := A^H*x. */
/*           Unchanged on exit. */

/*  DIAG   - CHARACTER*1 */
/*           On entry, DIAG specifies whether the diagonal blocks are */
/*           assumed to be unit matrices: */
/*              DIAG = 'U' or 'u'   Diagonal blocks are unit matrices. */
/*              DIAG = 'N' or 'n'   Diagonal blocks are non-unit. */
/*           Unchanged on exit. */

/*  N      - INTEGER */
/*           On entry, N specifies the order of the matrix A. */
/*           N must be at least zero. */
/*           Unchanged on exit. */

/*  NRHS   - INTEGER */
/*           On entry, NRHS specifies the number of right hand sides, */
/*           i.e., the number of vectors x to be multiplied by A. */
/*           NRHS must be at least zero. */
/*           Unchanged on exit. */

/*  A      - COMPLEX*16 array, dimension( LDA, N ) */
/*           On entry, A contains a block diagonal matrix and the */
/*           multipliers of the transformations used to obtain it, */
/*           stored as a 2-D triangular matrix. */
/*           Unchanged on exit. */

/*  LDA    - INTEGER */
/*           On entry, LDA specifies the first dimension of A as declared */
/*           in the calling ( sub ) program. LDA must be at least */
/*           max( 1, N ). */
/*           Unchanged on exit. */

/*  IPIV   - INTEGER array, dimension( N ) */
/*           On entry, IPIV contains the vector of pivot indices as */
/*           determined by ZSYTRF or ZHETRF. */
/*           If IPIV( K ) = K, no interchange was done. */
/*           If IPIV( K ) <> K but IPIV( K ) > 0, then row K was inter- */
/*           changed with row IPIV( K ) and a 1 x 1 pivot block was used. */
/*           If IPIV( K ) < 0 and UPLO = 'U', then row K-1 was exchanged */
/*           with row | IPIV( K ) | and a 2 x 2 pivot block was used. */
/*           If IPIV( K ) < 0 and UPLO = 'L', then row K+1 was exchanged */
/*           with row | IPIV( K ) | and a 2 x 2 pivot block was used. */

/*  B      - COMPLEX*16 array, dimension( LDB, NRHS ) */
/*           On entry, B contains NRHS vectors of length N. */
/*           On exit, B is overwritten with the product A * B. */

/*  LDB    - INTEGER */
/*           On entry, LDB contains the leading dimension of B as */
/*           declared in the calling program.  LDB must be at least */
/*           max( 1, N ). */
/*           Unchanged on exit. */

/*  INFO   - INTEGER */
/*           INFO is the error flag. */
/*           On exit, a value of 0 indicates a successful exit. */
/*           A negative value, say -K, indicates that the K-th argument */
/*           has an illegal value. */

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
	    "C")) {
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
	xerbla_("ZLAVHE ", &i__1);
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
		    zscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}

/*              Multiply by  P(K) * inv(U(K))  if K > 1. */

		if (k > 1) {

/*                 Apply the transformation. */

		    i__1 = k - 1;
		    zgeru_(&i__1, nrhs, &c_b1, &a[k * a_dim1 + 1], &c__1, &b[
			    k + b_dim1], ldb, &b[b_dim1 + 1], ldb);

/*                 Interchange if P(K) != I. */

		    kp = ipiv[k];
		    if (kp != k) {
			zswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }
		}
		++k;
	    } else {

/*              2 x 2 pivot block */

/*              Multiply by the diagonal block if forming U * D. */

		if (nounit) {
		    i__1 = k + k * a_dim1;
		    d11.r = a[i__1].r, d11.i = a[i__1].i;
		    i__1 = k + 1 + (k + 1) * a_dim1;
		    d22.r = a[i__1].r, d22.i = a[i__1].i;
		    i__1 = k + (k + 1) * a_dim1;
		    d12.r = a[i__1].r, d12.i = a[i__1].i;
		    d_cnjg(&z__1, &d12);
		    d21.r = z__1.r, d21.i = z__1.i;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			i__2 = k + j * b_dim1;
			t1.r = b[i__2].r, t1.i = b[i__2].i;
			i__2 = k + 1 + j * b_dim1;
			t2.r = b[i__2].r, t2.i = b[i__2].i;
			i__2 = k + j * b_dim1;
			z__2.r = d11.r * t1.r - d11.i * t1.i, z__2.i = d11.r *
				 t1.i + d11.i * t1.r;
			z__3.r = d12.r * t2.r - d12.i * t2.i, z__3.i = d12.r *
				 t2.i + d12.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
			i__2 = k + 1 + j * b_dim1;
			z__2.r = d21.r * t1.r - d21.i * t1.i, z__2.i = d21.r *
				 t1.i + d21.i * t1.r;
			z__3.r = d22.r * t2.r - d22.i * t2.i, z__3.i = d22.r *
				 t2.i + d22.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
/* L20: */
		    }
		}

/*              Multiply by  P(K) * inv(U(K))  if K > 1. */

		if (k > 1) {

/*                 Apply the transformations. */

		    i__1 = k - 1;
		    zgeru_(&i__1, nrhs, &c_b1, &a[k * a_dim1 + 1], &c__1, &b[
			    k + b_dim1], ldb, &b[b_dim1 + 1], ldb);
		    i__1 = k - 1;
		    zgeru_(&i__1, nrhs, &c_b1, &a[(k + 1) * a_dim1 + 1], &
			    c__1, &b[k + 1 + b_dim1], ldb, &b[b_dim1 + 1], 
			    ldb);

/*                 Interchange if P(K) != I. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k) {
			zswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
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
		    zscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}

/*              Multiply by  P(K) * inv(L(K))  if K < N. */

		if (k != *n) {
		    kp = ipiv[k];

/*                 Apply the transformation. */

		    i__1 = *n - k;
		    zgeru_(&i__1, nrhs, &c_b1, &a[k + 1 + k * a_dim1], &c__1, 
			    &b[k + b_dim1], ldb, &b[k + 1 + b_dim1], ldb);

/*                 Interchange if a permutation was applied at the */
/*                 K-th step of the factorization. */

		    if (kp != k) {
			zswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }
		}
		--k;

	    } else {

/*              2 x 2 pivot block: */

/*              Multiply by the diagonal block if forming L * D. */

		if (nounit) {
		    i__1 = k - 1 + (k - 1) * a_dim1;
		    d11.r = a[i__1].r, d11.i = a[i__1].i;
		    i__1 = k + k * a_dim1;
		    d22.r = a[i__1].r, d22.i = a[i__1].i;
		    i__1 = k + (k - 1) * a_dim1;
		    d21.r = a[i__1].r, d21.i = a[i__1].i;
		    d_cnjg(&z__1, &d21);
		    d12.r = z__1.r, d12.i = z__1.i;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			i__2 = k - 1 + j * b_dim1;
			t1.r = b[i__2].r, t1.i = b[i__2].i;
			i__2 = k + j * b_dim1;
			t2.r = b[i__2].r, t2.i = b[i__2].i;
			i__2 = k - 1 + j * b_dim1;
			z__2.r = d11.r * t1.r - d11.i * t1.i, z__2.i = d11.r *
				 t1.i + d11.i * t1.r;
			z__3.r = d12.r * t2.r - d12.i * t2.i, z__3.i = d12.r *
				 t2.i + d12.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
			i__2 = k + j * b_dim1;
			z__2.r = d21.r * t1.r - d21.i * t1.i, z__2.i = d21.r *
				 t1.i + d21.i * t1.r;
			z__3.r = d22.r * t2.r - d22.i * t2.i, z__3.i = d22.r *
				 t2.i + d22.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
/* L50: */
		    }
		}

/*              Multiply by  P(K) * inv(L(K))  if K < N. */

		if (k != *n) {

/*                 Apply the transformation. */

		    i__1 = *n - k;
		    zgeru_(&i__1, nrhs, &c_b1, &a[k + 1 + k * a_dim1], &c__1, 
			    &b[k + b_dim1], ldb, &b[k + 1 + b_dim1], ldb);
		    i__1 = *n - k;
		    zgeru_(&i__1, nrhs, &c_b1, &a[k + 1 + (k - 1) * a_dim1], &
			    c__1, &b[k - 1 + b_dim1], ldb, &b[k + 1 + b_dim1], 
			     ldb);

/*                 Interchange if a permutation was applied at the */
/*                 K-th step of the factorization. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k) {
			zswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }
		}
		k += -2;
	    }
	    goto L40;
L60:
	    ;
	}
/* -------------------------------------------------- */

/*     Compute  B := A^H * B  (conjugate transpose) */

/* -------------------------------------------------- */
    } else {

/*        Form  B := U^H*B */
/*        where U  = P(m)*inv(U(m))* ... *P(1)*inv(U(1)) */
/*        and   U^H = inv(U^H(1))*P(1)* ... *inv(U^H(m))*P(m) */

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

/*                 Interchange if P(K) != I. */

		    kp = ipiv[k];
		    if (kp != k) {
			zswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }

/*                 Apply the transformation */
/*                    y = y - B' conjg(x), */
/*                 where x is a column of A and y is a row of B. */

		    zlacgv_(nrhs, &b[k + b_dim1], ldb);
		    i__1 = k - 1;
		    zgemv_("Conjugate", &i__1, nrhs, &c_b1, &b[b_offset], ldb, 
			     &a[k * a_dim1 + 1], &c__1, &c_b1, &b[k + b_dim1], 
			     ldb);
		    zlacgv_(nrhs, &b[k + b_dim1], ldb);
		}
		if (nounit) {
		    zscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}
		--k;

/*           2 x 2 pivot block. */

	    } else {
		if (k > 2) {

/*                 Interchange if P(K) != I. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k - 1) {
			zswap_(nrhs, &b[k - 1 + b_dim1], ldb, &b[kp + b_dim1], 
				 ldb);
		    }

/*                 Apply the transformations */
/*                    y = y - B' conjg(x), */
/*                 where x is a block column of A and y is a block */
/*                 row of B. */

		    zlacgv_(nrhs, &b[k + b_dim1], ldb);
		    i__1 = k - 2;
		    zgemv_("Conjugate", &i__1, nrhs, &c_b1, &b[b_offset], ldb, 
			     &a[k * a_dim1 + 1], &c__1, &c_b1, &b[k + b_dim1], 
			     ldb);
		    zlacgv_(nrhs, &b[k + b_dim1], ldb);

		    zlacgv_(nrhs, &b[k - 1 + b_dim1], ldb);
		    i__1 = k - 2;
		    zgemv_("Conjugate", &i__1, nrhs, &c_b1, &b[b_offset], ldb, 
			     &a[(k - 1) * a_dim1 + 1], &c__1, &c_b1, &b[k - 1 
			    + b_dim1], ldb);
		    zlacgv_(nrhs, &b[k - 1 + b_dim1], ldb);
		}

/*              Multiply by the diagonal block if non-unit. */

		if (nounit) {
		    i__1 = k - 1 + (k - 1) * a_dim1;
		    d11.r = a[i__1].r, d11.i = a[i__1].i;
		    i__1 = k + k * a_dim1;
		    d22.r = a[i__1].r, d22.i = a[i__1].i;
		    i__1 = k - 1 + k * a_dim1;
		    d12.r = a[i__1].r, d12.i = a[i__1].i;
		    d_cnjg(&z__1, &d12);
		    d21.r = z__1.r, d21.i = z__1.i;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			i__2 = k - 1 + j * b_dim1;
			t1.r = b[i__2].r, t1.i = b[i__2].i;
			i__2 = k + j * b_dim1;
			t2.r = b[i__2].r, t2.i = b[i__2].i;
			i__2 = k - 1 + j * b_dim1;
			z__2.r = d11.r * t1.r - d11.i * t1.i, z__2.i = d11.r *
				 t1.i + d11.i * t1.r;
			z__3.r = d12.r * t2.r - d12.i * t2.i, z__3.i = d12.r *
				 t2.i + d12.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
			i__2 = k + j * b_dim1;
			z__2.r = d21.r * t1.r - d21.i * t1.i, z__2.i = d21.r *
				 t1.i + d21.i * t1.r;
			z__3.r = d22.r * t2.r - d22.i * t2.i, z__3.i = d22.r *
				 t2.i + d22.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
/* L80: */
		    }
		}
		k += -2;
	    }
	    goto L70;
L90:

/*        Form  B := L^H*B */
/*        where L  = P(1)*inv(L(1))* ... *P(m)*inv(L(m)) */
/*        and   L^H = inv(L^H(m))*P(m)* ... *inv(L^H(1))*P(1) */

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

/*                 Interchange if P(K) != I. */

		    kp = ipiv[k];
		    if (kp != k) {
			zswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], 
				ldb);
		    }

/*                 Apply the transformation */

		    zlacgv_(nrhs, &b[k + b_dim1], ldb);
		    i__1 = *n - k;
		    zgemv_("Conjugate", &i__1, nrhs, &c_b1, &b[k + 1 + b_dim1]
, ldb, &a[k + 1 + k * a_dim1], &c__1, &c_b1, &b[k 
			    + b_dim1], ldb);
		    zlacgv_(nrhs, &b[k + b_dim1], ldb);
		}
		if (nounit) {
		    zscal_(nrhs, &a[k + k * a_dim1], &b[k + b_dim1], ldb);
		}
		++k;

/*           2 x 2 pivot block. */

	    } else {
		if (k < *n - 1) {

/*              Interchange if P(K) != I. */

		    kp = (i__1 = ipiv[k], abs(i__1));
		    if (kp != k + 1) {
			zswap_(nrhs, &b[k + 1 + b_dim1], ldb, &b[kp + b_dim1], 
				 ldb);
		    }

/*                 Apply the transformation */

		    zlacgv_(nrhs, &b[k + 1 + b_dim1], ldb);
		    i__1 = *n - k - 1;
		    zgemv_("Conjugate", &i__1, nrhs, &c_b1, &b[k + 2 + b_dim1]
, ldb, &a[k + 2 + (k + 1) * a_dim1], &c__1, &c_b1, 
			     &b[k + 1 + b_dim1], ldb);
		    zlacgv_(nrhs, &b[k + 1 + b_dim1], ldb);

		    zlacgv_(nrhs, &b[k + b_dim1], ldb);
		    i__1 = *n - k - 1;
		    zgemv_("Conjugate", &i__1, nrhs, &c_b1, &b[k + 2 + b_dim1]
, ldb, &a[k + 2 + k * a_dim1], &c__1, &c_b1, &b[k 
			    + b_dim1], ldb);
		    zlacgv_(nrhs, &b[k + b_dim1], ldb);
		}

/*              Multiply by the diagonal block if non-unit. */

		if (nounit) {
		    i__1 = k + k * a_dim1;
		    d11.r = a[i__1].r, d11.i = a[i__1].i;
		    i__1 = k + 1 + (k + 1) * a_dim1;
		    d22.r = a[i__1].r, d22.i = a[i__1].i;
		    i__1 = k + 1 + k * a_dim1;
		    d21.r = a[i__1].r, d21.i = a[i__1].i;
		    d_cnjg(&z__1, &d21);
		    d12.r = z__1.r, d12.i = z__1.i;
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			i__2 = k + j * b_dim1;
			t1.r = b[i__2].r, t1.i = b[i__2].i;
			i__2 = k + 1 + j * b_dim1;
			t2.r = b[i__2].r, t2.i = b[i__2].i;
			i__2 = k + j * b_dim1;
			z__2.r = d11.r * t1.r - d11.i * t1.i, z__2.i = d11.r *
				 t1.i + d11.i * t1.r;
			z__3.r = d12.r * t2.r - d12.i * t2.i, z__3.i = d12.r *
				 t2.i + d12.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
			i__2 = k + 1 + j * b_dim1;
			z__2.r = d21.r * t1.r - d21.i * t1.i, z__2.i = d21.r *
				 t1.i + d21.i * t1.r;
			z__3.r = d22.r * t2.r - d22.i * t2.i, z__3.i = d22.r *
				 t2.i + d22.i * t2.r;
			z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
			b[i__2].r = z__1.r, b[i__2].i = z__1.i;
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

/*     End of ZLAVHE */

} /* zlavhe_ */
