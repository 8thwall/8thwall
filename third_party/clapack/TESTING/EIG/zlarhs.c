#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {1.,0.};
static doublecomplex c_b2 = {0.,0.};
static integer c__2 = 2;
static integer c__1 = 1;

/* Subroutine */ int zlarhs_(char *path, char *xtype, char *uplo, char *trans, 
	 integer *m, integer *n, integer *kl, integer *ku, integer *nrhs, 
	doublecomplex *a, integer *lda, doublecomplex *x, integer *ldx, 
	doublecomplex *b, integer *ldb, integer *iseed, integer *info)
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
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), zhemm_(char *, char *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), zgbmv_(char *, integer *, integer *, 
	    integer *, integer *, doublecomplex *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), zhbmv_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *), 
	    zsbmv_(char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *), ztbmv_(char 
	    *, char *, char *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, integer *), zhpmv_(
	    char *, integer *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), ztrmm_(char *, char *, char *, char *, 
	    integer *, integer *, doublecomplex *, doublecomplex *, integer *, 
	     doublecomplex *, integer *), 
	    zspmv_(char *, integer *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *), zsymm_(char *, char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *), ztpmv_(char *, char *, char *, integer *, doublecomplex *
, doublecomplex *, integer *), xerbla_(
	    char *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    logical notran;
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), 
	    zlarnv_(integer *, integer *, integer *, doublecomplex *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLARHS chooses a set of NRHS random solution vectors and sets */
/*  up the right hand sides for the linear system */
/*     op( A ) * X = B, */
/*  where op( A ) may be A, A**T (transpose of A), or A**H (conjugate */
/*  transpose of A). */

/*  Arguments */
/*  ========= */

/*  PATH    (input) CHARACTER*3 */
/*          The type of the complex matrix A.  PATH may be given in any */
/*          combination of upper and lower case.  Valid paths include */
/*             xGE:  General m x n matrix */
/*             xGB:  General banded matrix */
/*             xPO:  Hermitian positive definite, 2-D storage */
/*             xPP:  Hermitian positive definite packed */
/*             xPB:  Hermitian positive definite banded */
/*             xHE:  Hermitian indefinite, 2-D storage */
/*             xHP:  Hermitian indefinite packed */
/*             xHB:  Hermitian indefinite banded */
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
/*          Used only if A is symmetric or triangular; specifies whether */
/*          the upper or lower triangular part of the matrix A is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Used only if A is nonsymmetric; specifies the operation */
/*          applied to the matrix A. */
/*          = 'N':  B := A    * X */
/*          = 'T':  B := A**T * X */
/*          = 'C':  B := A**H * X */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

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

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The test matrix whose type is given by PATH. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */
/*          If PATH = xGB, LDA >= KL+KU+1. */
/*          If PATH = xPB, xSB, xHB, or xTB, LDA >= KL+1. */
/*          Otherwise, LDA >= max(1,M). */

/*  X       (input or output) COMPLEX*16  array, dimension (LDX,NRHS) */
/*          On entry, if XTYPE = 'C' (for 'Computed'), then X contains */
/*          the exact solution to the system of linear equations. */
/*          On exit, if XTYPE = 'N' (for 'New'), then X is initialized */
/*          with random values. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  If TRANS = 'N', */
/*          LDX >= max(1,N); if TRANS = 'T', LDX >= max(1,M). */

/*  B       (output) COMPLEX*16  array, dimension (LDB,NRHS) */
/*          The right hand side vector(s) for the system of equations, */
/*          computed from B = op(A) * X, where op(A) is determined by */
/*          TRANS. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  If TRANS = 'N', */
/*          LDB >= max(1,M); if TRANS = 'T', LDB >= max(1,N). */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          The seed vector for the random number generator (used in */
/*          ZLATMS).  Modified on exit. */

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
	    "S") || lsame_(path + 1, "H");
    tri = lsame_(path + 1, "T");
    band = lsame_(path + 2, "B");
    if (! lsame_(c1, "Zomplex precision")) {
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
	xerbla_("ZLARHS", &i__1);
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
	    zlarnv_(&c__2, &iseed[1], n, &x[j * x_dim1 + 1]);
/* L10: */
	}
    }

/*     Multiply X by op( A ) using an appropriate */
/*     matrix multiply routine. */

    if (lsamen_(&c__2, c2, "GE") || lsamen_(&c__2, c2, 
	    "QR") || lsamen_(&c__2, c2, "LQ") || lsamen_(&c__2, c2, "QL") || 
	    lsamen_(&c__2, c2, "RQ")) {

/*        General matrix */

	zgemm_(trans, "N", &mb, nrhs, &nx, &c_b1, &a[a_offset], lda, &x[
		x_offset], ldx, &c_b2, &b[b_offset], ldb);

    } else if (lsamen_(&c__2, c2, "PO") || lsamen_(&
	    c__2, c2, "HE")) {

/*        Hermitian matrix, 2-D storage */

	zhemm_("Left", uplo, n, nrhs, &c_b1, &a[a_offset], lda, &x[x_offset], 
		ldx, &c_b2, &b[b_offset], ldb);

    } else if (lsamen_(&c__2, c2, "SY")) {

/*        Symmetric matrix, 2-D storage */

	zsymm_("Left", uplo, n, nrhs, &c_b1, &a[a_offset], lda, &x[x_offset], 
		ldx, &c_b2, &b[b_offset], ldb);

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        General matrix, band storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    zgbmv_(trans, m, n, kl, ku, &c_b1, &a[a_offset], lda, &x[j * 
		    x_dim1 + 1], &c__1, &c_b2, &b[j * b_dim1 + 1], &c__1);
/* L20: */
	}

    } else if (lsamen_(&c__2, c2, "PB") || lsamen_(&
	    c__2, c2, "HB")) {

/*        Hermitian matrix, band storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    zhbmv_(uplo, n, kl, &c_b1, &a[a_offset], lda, &x[j * x_dim1 + 1], 
		    &c__1, &c_b2, &b[j * b_dim1 + 1], &c__1);
/* L30: */
	}

    } else if (lsamen_(&c__2, c2, "SB")) {

/*        Symmetric matrix, band storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    zsbmv_(uplo, n, kl, &c_b1, &a[a_offset], lda, &x[j * x_dim1 + 1], 
		    &c__1, &c_b2, &b[j * b_dim1 + 1], &c__1);
/* L40: */
	}

    } else if (lsamen_(&c__2, c2, "PP") || lsamen_(&
	    c__2, c2, "HP")) {

/*        Hermitian matrix, packed storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    zhpmv_(uplo, n, &c_b1, &a[a_offset], &x[j * x_dim1 + 1], &c__1, &
		    c_b2, &b[j * b_dim1 + 1], &c__1);
/* L50: */
	}

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        Symmetric matrix, packed storage */

	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    zspmv_(uplo, n, &c_b1, &a[a_offset], &x[j * x_dim1 + 1], &c__1, &
		    c_b2, &b[j * b_dim1 + 1], &c__1);
/* L60: */
	}

    } else if (lsamen_(&c__2, c2, "TR")) {

/*        Triangular matrix.  Note that for triangular matrices, */
/*           KU = 1 => non-unit triangular */
/*           KU = 2 => unit triangular */

	zlacpy_("Full", n, nrhs, &x[x_offset], ldx, &b[b_offset], ldb);
	if (*ku == 2) {
	    *(unsigned char *)diag = 'U';
	} else {
	    *(unsigned char *)diag = 'N';
	}
	ztrmm_("Left", uplo, trans, diag, n, nrhs, &c_b1, &a[a_offset], lda, &
		b[b_offset], ldb);

    } else if (lsamen_(&c__2, c2, "TP")) {

/*        Triangular matrix, packed storage */

	zlacpy_("Full", n, nrhs, &x[x_offset], ldx, &b[b_offset], ldb);
	if (*ku == 2) {
	    *(unsigned char *)diag = 'U';
	} else {
	    *(unsigned char *)diag = 'N';
	}
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ztpmv_(uplo, trans, diag, n, &a[a_offset], &b[j * b_dim1 + 1], &
		    c__1);
/* L70: */
	}

    } else if (lsamen_(&c__2, c2, "TB")) {

/*        Triangular matrix, banded storage */

	zlacpy_("Full", n, nrhs, &x[x_offset], ldx, &b[b_offset], ldb);
	if (*ku == 2) {
	    *(unsigned char *)diag = 'U';
	} else {
	    *(unsigned char *)diag = 'N';
	}
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ztbmv_(uplo, trans, diag, n, kl, &a[a_offset], lda, &b[j * b_dim1 
		    + 1], &c__1);
/* L80: */
	}

    } else {

/*        If none of the above, set INFO = -1 and return */

	*info = -1;
	i__1 = -(*info);
	xerbla_("ZLARHS", &i__1);
    }

    return 0;

/*     End of ZLARHS */

} /* zlarhs_ */
