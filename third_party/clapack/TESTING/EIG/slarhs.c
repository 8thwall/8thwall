#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static real c_b32 = 1.f;
static real c_b33 = 0.f;
static integer c__1 = 1;

/* Subroutine */ int slarhs_(char *path, char *xtype, char *uplo, char *trans, 
	 integer *m, integer *n, integer *kl, integer *ku, integer *nrhs, 
	real *a, integer *lda, real *x, integer *ldx, real *b, integer *ldb, 
	integer *iseed, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    integer j;
    char c1[1], c2[2];
    integer mb, nx;
    logical gen, tri, qrs, sym, band;
    char diag[1];
    logical tran;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *), sgbmv_(char *, integer *, 
	    integer *, integer *, integer *, real *, real *, integer *, real *
, integer *, real *, real *, integer *), ssbmv_(char *, 
	    integer *, integer *, real *, real *, integer *, real *, integer *
, real *, real *, integer *), stbmv_(char *, char *, char 
	    *, integer *, integer *, real *, integer *, real *, integer *), strmm_(char *, char *, char *, char *, 
	    integer *, integer *, real *, real *, integer *, real *, integer *
), sspmv_(char *, integer *, real 
	    *, real *, real *, integer *, real *, real *, integer *), 
	    ssymm_(char *, char *, integer *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, real *, integer *), stpmv_(char *, char *, char *, integer *, real *, real *, 
	     integer *), xerbla_(char *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *);
    logical notran;
    extern /* Subroutine */ int slarnv_(integer *, integer *, integer *, real 
	    *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLARHS chooses a set of NRHS random solution vectors and sets */
/*  up the right hand sides for the linear system */
/*     op( A ) * X = B, */
/*  where op( A ) may be A or A' (transpose of A). */

/*  Arguments */
/*  ========= */

/*  PATH    (input) CHARACTER*3 */
/*          The type of the real matrix A.  PATH may be given in any */
/*          combination of upper and lower case.  Valid types include */
/*             xGE:  General m x n matrix */
/*             xGB:  General banded matrix */
/*             xPO:  Symmetric positive definite, 2-D storage */
/*             xPP:  Symmetric positive definite packed */
/*             xPB:  Symmetric positive definite banded */
/*             xSY:  Symmetric indefinite, 2-D storage */
/*             xSP:  Symmetric indefinite packed */
/*             xSB:  Symmetric indefinite banded */
/*             xTR:  Triangular */
/*             xTP:  Triangular packed */
/*             xTB:  Triangular banded */
/*             xQR:  General m x n matrix */
/*             xLQ:  General m x n matrix */
/*             xQL:  General m x n matrix */
/*             xRQ:  General m x n matrix */
/*          where the leading character indicates the precision. */

/*  XTYPE   (input) CHARACTER*1 */
/*          Specifies how the exact solution X will be determined: */
/*          = 'N':  New solution; generate a random X. */
/*          = 'C':  Computed; use value of X on entry. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          matrix A is stored, if A is symmetric. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to the matrix A. */
/*          = 'N':  System is  A * x = b */
/*          = 'T':  System is  A'* x = b */
/*          = 'C':  System is  A'* x = b */

/*  M       (input) INTEGER */
/*          The number or rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          Used only if A is a band matrix; specifies the number of */
/*          subdiagonals of A if A is a general band matrix or if A is */
/*          symmetric or triangular and UPLO = 'L'; specifies the number */
/*          of superdiagonals of A if A is symmetric or triangular and */
/*          UPLO = 'U'.  0 <= KL <= M-1. */

/*  KU      (input) INTEGER */
/*          Used only if A is a general band matrix or if A is */
/*          triangular. */

/*          If PATH = xGB, specifies the number of superdiagonals of A, */
/*          and 0 <= KU <= N-1. */

/*          If PATH = xTR, xTP, or xTB, specifies whether or not the */
/*          matrix has unit diagonal: */
/*          = 1:  matrix has non-unit diagonal (default) */
/*          = 2:  matrix has unit diagonal */

/*  NRHS    (input) INTEGER */
/*          The number of right hand side vectors in the system A*X = B. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The test matrix whose type is given by PATH. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */
/*          If PATH = xGB, LDA >= KL+KU+1. */
/*          If PATH = xPB, xSB, xHB, or xTB, LDA >= KL+1. */
/*          Otherwise, LDA >= max(1,M). */

/*  X       (input or output) REAL array, dimension(LDX,NRHS) */
/*          On entry, if XTYPE = 'C' (for 'Computed'), then X contains */
/*          the exact solution to the system of linear equations. */
/*          On exit, if XTYPE = 'N' (for 'New'), then X is initialized */
/*          with random values. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  If TRANS = 'N', */
/*          LDX >= max(1,N); if TRANS = 'T', LDX >= max(1,M). */

/*  B       (output) REAL array, dimension (LDB,NRHS) */
/*          The right hand side vector(s) for the system of equations, */
/*          computed from B = op(A) * X, where op(A) is determined by */
/*          TRANS. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  If TRANS = 'N', */
/*          LDB >= max(1,M); if TRANS = 'T', LDB >= max(1,N). */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          The seed vector for the random number generator (used in */
/*          SLATMS).  Modified on exit. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

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
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --iseed;

    /* Function Body */
    *info = 0;
    *(unsigned char *)c1 = *(unsigned char *)path;
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);
    tran = lsame_(trans, "T") || lsame_(trans, "C");
    notran = ! tran;
    gen = lsame_(path + 1, "G");
    qrs = lsame_(path + 1, "Q") || lsame_(path + 2, 
	    "Q");
    sym = lsame_(path + 1, "P") || lsame_(path + 1, 
	    "S");
    tri = lsame_(path + 1, "T");
    band = lsame_(path + 2, "B");
    if (! lsame_(c1, "Single precision")) {
	*info = -1;
    } else if (! (lsame_(xtype, "N") || lsame_(xtype, 
	    "C"))) {
	*info = -2;
    } else if ((sym || tri) && ! (lsame_(uplo, "U") || 
	    lsame_(uplo, "L"))) {
	*info = -3;
    } else if ((gen || qrs) && ! (tran || lsame_(trans, "N"))) {
	*info = -4;
    } else if (*m < 0) {
	*info = -5;
    } else if (*n < 0) {
	*info = -6;
    } else if (band && *kl < 0) {
	*info = -7;
    } else if (band && *ku < 0) {
	*info = -8;
    } else if (*nrhs < 0) {
	*info = -9;
    } else if (! band && *lda < max(1,*m) || band && (sym || tri) && *lda < *
	    kl + 1 || band && gen && *lda < *kl + *ku + 1) {
	*info = -11;
    } else if (notran && *ldx < max(1,*n) || tran && *ldx < max(1,*m)) {
	*info = -13;
    } else if (notran && *ldb < max(1,*m) || tran && *ldb < max(1,*n)) {
	*info = -15;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLARHS", &i__1);
	return 0;
    }

/*     Initialize X to NRHS random vectors unless XTYPE = 'C'. */

    if (tran) {
	nx = *m;
	mb = *n;
    } else {
	nx = *n;
	mb = *m;
    }
    if (! lsame_(xtype, "C")) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    slarnv_(&c__2, &iseed[1], n, &x[j * x_dim1 + 1]);
/* L10: */
	}
    }

/*     Multiply X by op( A ) using an appropriate */
/*     matrix multiply routine. */

    if (lsamen_(&c__2, c2, "GE") || lsamen_(&c__2, c2, 
	    "QR") || lsamen_(&c__2, c2, "LQ") || lsamen_(&c__2, c2, "QL") || 
	    lsamen_(&c__2, c2, "RQ")) {

/*        General matrix */

	sgemm_(trans, "N", &mb, nrhs, &nx, &c_b32, &a[a_offset], lda, &x[
		x_offset], ldx, &c_b33, &b[b_offset], ldb);

    } else if (lsamen_(&c__2, c2, "PO") || lsamen_(&
	    c__2, c2, "SY")) {

/*        Symmetric matrix, 2-D storage */

	ssymm_("Left", uplo, n, nrhs, &c_b32, &a[a_offset], lda, &x[x_offset], 
		 ldx, &c_b33, &b[b_offset], ldb);

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        General matrix, band storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    sgbmv_(trans, &mb, &nx, kl, ku, &c_b32, &a[a_offset], lda, &x[j * 
		    x_dim1 + 1], &c__1, &c_b33, &b[j * b_dim1 + 1], &c__1);
/* L20: */
	}

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        Symmetric matrix, band storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ssbmv_(uplo, n, kl, &c_b32, &a[a_offset], lda, &x[j * x_dim1 + 1], 
		     &c__1, &c_b33, &b[j * b_dim1 + 1], &c__1);
/* L30: */
	}

    } else if (lsamen_(&c__2, c2, "PP") || lsamen_(&
	    c__2, c2, "SP")) {

/*        Symmetric matrix, packed storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    sspmv_(uplo, n, &c_b32, &a[a_offset], &x[j * x_dim1 + 1], &c__1, &
		    c_b33, &b[j * b_dim1 + 1], &c__1);
/* L40: */
	}

    } else if (lsamen_(&c__2, c2, "TR")) {

/*        Triangular matrix.  Note that for triangular matrices, */
/*           KU = 1 => non-unit triangular */
/*           KU = 2 => unit triangular */

	slacpy_("Full", n, nrhs, &x[x_offset], ldx, &b[b_offset], ldb);
	if (*ku == 2) {
	    *(unsigned char *)diag = 'U';
	} else {
	    *(unsigned char *)diag = 'N';
	}
	strmm_("Left", uplo, trans, diag, n, nrhs, &c_b32, &a[a_offset], lda, 
		&b[b_offset], ldb)
		;

    } else if (lsamen_(&c__2, c2, "TP")) {

/*        Triangular matrix, packed storage */

	slacpy_("Full", n, nrhs, &x[x_offset], ldx, &b[b_offset], ldb);
	if (*ku == 2) {
	    *(unsigned char *)diag = 'U';
	} else {
	    *(unsigned char *)diag = 'N';
	}
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    stpmv_(uplo, trans, diag, n, &a[a_offset], &b[j * b_dim1 + 1], &
		    c__1);
/* L50: */
	}

    } else if (lsamen_(&c__2, c2, "TB")) {

/*        Triangular matrix, banded storage */

	slacpy_("Full", n, nrhs, &x[x_offset], ldx, &b[b_offset], ldb);
	if (*ku == 2) {
	    *(unsigned char *)diag = 'U';
	} else {
	    *(unsigned char *)diag = 'N';
	}
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    stbmv_(uplo, trans, diag, n, kl, &a[a_offset], lda, &b[j * b_dim1 
		    + 1], &c__1);
/* L60: */
	}

    } else {

/*        If PATH is none of the above, return with an error code. */

	*info = -1;
	i__1 = -(*info);
	xerbla_("SLARHS", &i__1);
    }

    return 0;

/*     End of SLARHS */

} /* slarhs_ */
