#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {1.f,0.f};

/* Subroutine */ int cgeqrs_(integer *m, integer *n, integer *nrhs, complex *
	a, integer *lda, complex *tau, complex *b, integer *ldb, complex *
	work, integer *lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    extern /* Subroutine */ int ctrsm_(char *, char *, char *, char *, 
	    integer *, integer *, complex *, complex *, integer *, complex *, 
	    integer *), xerbla_(char *, 
	    integer *), cunmqr_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *);


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Solve the least squares problem */
/*      min || A*X - B || */
/*  using the QR factorization */
/*      A = Q*R */
/*  computed by CGEQRF. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  M >= N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of B.  NRHS >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          Details of the QR factorization of the original matrix A as */
/*          returned by CGEQRF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= M. */

/*  TAU     (input) COMPLEX array, dimension (N) */
/*          Details of the orthogonal matrix Q. */

/*  B       (input/output) COMPLEX array, dimension (LDB,NRHS) */
/*          On entry, the m-by-nrhs right hand side matrix B. */
/*          On exit, the n-by-nrhs solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= M. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK must be at least NRHS, */
/*          and should be at least NRHS*NB, where NB is the block size */
/*          for this environment. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    } else if (*ldb < max(1,*m)) {
	*info = -8;
    } else if (*lwork < 1 || *lwork < *nrhs && *m > 0 && *n > 0) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CGEQRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0 || *m == 0) {
	return 0;
    }

/*     B := Q' * B */

    cunmqr_("Left", "Conjugate transpose", m, nrhs, n, &a[a_offset], lda, &
	    tau[1], &b[b_offset], ldb, &work[1], lwork, info);

/*     Solve R*X = B(1:n,:) */

    ctrsm_("Left", "Upper", "No transpose", "Non-unit", n, nrhs, &c_b1, &a[
	    a_offset], lda, &b[b_offset], ldb);

    return 0;

/*     End of CGEQRS */

} /* cgeqrs_ */
