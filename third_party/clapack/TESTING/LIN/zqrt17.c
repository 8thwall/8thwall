#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__13 = 13;
static doublecomplex c_b13 = {-1.,0.};
static doublecomplex c_b14 = {1.,0.};
static integer c__0 = 0;
static doublereal c_b19 = 1.;
static doublecomplex c_b22 = {0.,0.};

doublereal zqrt17_(char *trans, integer *iresid, integer *m, integer *n, 
	integer *nrhs, doublecomplex *a, integer *lda, doublecomplex *x, 
	integer *ldx, doublecomplex *b, integer *ldb, doublecomplex *c__, 
	doublecomplex *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, x_dim1, 
	    x_offset, i__1;
    doublereal ret_val;

    /* Local variables */
    doublereal err;
    integer iscl, info;
    extern logical lsame_(char *, char *);
    doublereal norma, normb;
    integer ncols;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);
    doublereal normx, rwork[1];
    integer nrows;
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    doublereal bignum;
    extern /* Subroutine */ int zlascl_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublecomplex *, 
	     integer *, integer *), zlacpy_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *);
    doublereal smlnum, normrs;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZQRT17 computes the ratio */

/*     || R'*op(A) ||/(||A||*alpha*max(M,N,NRHS)*eps) */

/*  where R = op(A)*X - B, op(A) is A or A', and */

/*     alpha = ||B|| if IRESID = 1 (zero-residual problem) */
/*     alpha = ||R|| if IRESID = 2 (otherwise). */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies whether or not the transpose of A is used. */
/*          = 'N':  No transpose, op(A) = A. */
/*          = 'C':  Conjugate transpose, op(A) = A'. */

/*  IRESID  (input) INTEGER */
/*          IRESID = 1 indicates zero-residual problem. */
/*          IRESID = 2 indicates non-zero residual. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */
/*          If TRANS = 'N', the number of rows of the matrix B. */
/*          If TRANS = 'C', the number of rows of the matrix X. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix  A. */
/*          If TRANS = 'N', the number of rows of the matrix X. */
/*          If TRANS = 'C', the number of rows of the matrix B. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of the matrices X and B. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= M. */

/*  X       (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          If TRANS = 'N', the n-by-nrhs matrix X. */
/*          If TRANS = 'C', the m-by-nrhs matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X. */
/*          If TRANS = 'N', LDX >= N. */
/*          If TRANS = 'C', LDX >= M. */

/*  B       (input) COMPLEX*16 array, dimension (LDB,NRHS) */
/*          If TRANS = 'N', the m-by-nrhs matrix B. */
/*          If TRANS = 'C', the n-by-nrhs matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. */
/*          If TRANS = 'N', LDB >= M. */
/*          If TRANS = 'C', LDB >= N. */

/*  C       (workspace) COMPLEX*16 array, dimension (LDB,NRHS) */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= NRHS*(M+N). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
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
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    c_dim1 = *ldb;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;

    /* Function Body */
    ret_val = 0.;

    if (lsame_(trans, "N")) {
	nrows = *m;
	ncols = *n;
    } else if (lsame_(trans, "C")) {
	nrows = *n;
	ncols = *m;
    } else {
	xerbla_("ZQRT17", &c__1);
	return ret_val;
    }

    if (*lwork < ncols * *nrhs) {
	xerbla_("ZQRT17", &c__13);
	return ret_val;
    }

    if (*m <= 0 || *n <= 0 || *nrhs <= 0) {
	return ret_val;
    }

    norma = zlange_("One-norm", m, n, &a[a_offset], lda, rwork);
    smlnum = dlamch_("Safe minimum") / dlamch_("Precision");
    bignum = 1. / smlnum;
    iscl = 0;

/*     compute residual and scale it */

    zlacpy_("All", &nrows, nrhs, &b[b_offset], ldb, &c__[c_offset], ldb);
    zgemm_(trans, "No transpose", &nrows, nrhs, &ncols, &c_b13, &a[a_offset], 
	    lda, &x[x_offset], ldx, &c_b14, &c__[c_offset], ldb);
    normrs = zlange_("Max", &nrows, nrhs, &c__[c_offset], ldb, rwork);
    if (normrs > smlnum) {
	iscl = 1;
	zlascl_("General", &c__0, &c__0, &normrs, &c_b19, &nrows, nrhs, &c__[
		c_offset], ldb, &info);
    }

/*     compute R'*A */

    zgemm_("Conjugate transpose", trans, nrhs, &ncols, &nrows, &c_b14, &c__[
	    c_offset], ldb, &a[a_offset], lda, &c_b22, &work[1], nrhs);

/*     compute and properly scale error */

    err = zlange_("One-norm", nrhs, &ncols, &work[1], nrhs, rwork);
    if (norma != 0.) {
	err /= norma;
    }

    if (iscl == 1) {
	err *= normrs;
    }

    if (*iresid == 1) {
	normb = zlange_("One-norm", &nrows, nrhs, &b[b_offset], ldb, rwork);
	if (normb != 0.) {
	    err /= normb;
	}
    } else {
	normx = zlange_("One-norm", &ncols, nrhs, &x[x_offset], ldx, rwork);
	if (normx != 0.) {
	    err /= normx;
	}
    }

/* Computing MAX */
    i__1 = max(*m,*n);
    ret_val = err / (dlamch_("Epsilon") * (doublereal) max(i__1,*
	    nrhs));
    return ret_val;

/*     End of ZQRT17 */

} /* zqrt17_ */
