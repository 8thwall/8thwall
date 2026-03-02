#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b7 = 1.f;
static real c_b9 = 0.f;

/* Subroutine */ int sgerqs_(integer *m, integer *n, integer *nrhs, real *a, 
	integer *lda, real *tau, real *b, integer *ldb, real *work, integer *
	lwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    extern /* Subroutine */ int strsm_(char *, char *, char *, char *, 
	    integer *, integer *, real *, real *, integer *, real *, integer *
), xerbla_(char *, integer *), slaset_(char *, integer *, integer *, real *, real *, 
	    real *, integer *), sormrq_(char *, char *, integer *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, real *, integer *, integer *);


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Compute a minimum-norm solution */
/*      min || A*X - B || */
/*  using the RQ factorization */
/*      A = R*Q */
/*  computed by SGERQF. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= M >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of B.  NRHS >= 0. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          Details of the RQ factorization of the original matrix A as */
/*          returned by SGERQF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= M. */

/*  TAU     (input) REAL array, dimension (M) */
/*          Details of the orthogonal matrix Q. */

/*  B       (input/output) REAL array, dimension (LDB,NRHS) */
/*          On entry, the right hand side vectors for the linear system. */
/*          On exit, the solution vectors X.  Each solution vector */
/*          is contained in rows 1:N of a column of B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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

/*     Test the input parameters. */

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
    } else if (*n < 0 || *m > *n) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    } else if (*ldb < max(1,*n)) {
	*info = -8;
    } else if (*lwork < 1 || *lwork < *nrhs && *m > 0 && *n > 0) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SGERQS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0 || *m == 0) {
	return 0;
    }

/*     Solve R*X = B(n-m+1:n,:) */

    strsm_("Left", "Upper", "No transpose", "Non-unit", m, nrhs, &c_b7, &a[(*
	    n - *m + 1) * a_dim1 + 1], lda, &b[*n - *m + 1 + b_dim1], ldb);

/*     Set B(1:n-m,:) to zero */

    i__1 = *n - *m;
    slaset_("Full", &i__1, nrhs, &c_b9, &c_b9, &b[b_offset], ldb);

/*     B := Q' * B */

    sormrq_("Left", "Transpose", n, nrhs, m, &a[a_offset], lda, &tau[1], &b[
	    b_offset], ldb, &work[1], lwork, info);

    return 0;

/*     End of SGERQS */

} /* sgerqs_ */
