#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__13 = 13;
static real c_b13 = -1.f;
static real c_b14 = 1.f;
static integer c__0 = 0;
static real c_b22 = 0.f;

doublereal sqrt17_(char *trans, integer *iresid, integer *m, integer *n, 
	integer *nrhs, real *a, integer *lda, real *x, integer *ldx, real *b, 
	integer *ldb, real *c__, real *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, x_dim1, 
	    x_offset, i__1;
    real ret_val;

    /* Local variables */
    real err;
    integer iscl, info;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real norma, normb;
    integer ncols;
    real normx, rwork[1];
    integer nrows;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    real bignum;
    extern /* Subroutine */ int slascl_(char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *), slacpy_(char *, integer *, integer *, real *, integer *, 
	    real *, integer *);
    real smlnum, normrs;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SQRT17 computes the ratio */

/*     || R'*op(A) ||/(||A||*alpha*max(M,N,NRHS)*eps) */

/*  where R = op(A)*X - B, op(A) is A or A', and */

/*     alpha = ||B|| if IRESID = 1 (zero-residual problem) */
/*     alpha = ||R|| if IRESID = 2 (otherwise). */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies whether or not the transpose of A is used. */
/*          = 'N':  No transpose, op(A) = A. */
/*          = 'T':  Transpose, op(A) = A'. */

/*  IRESID  (input) INTEGER */
/*          IRESID = 1 indicates zero-residual problem. */
/*          IRESID = 2 indicates non-zero residual. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */
/*          If TRANS = 'N', the number of rows of the matrix B. */
/*          If TRANS = 'T', the number of rows of the matrix X. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix  A. */
/*          If TRANS = 'N', the number of rows of the matrix X. */
/*          If TRANS = 'T', the number of rows of the matrix B. */

/*  NRHS    (input) INTEGER */
/*          The number of columns of the matrices X and B. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The m-by-n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= M. */

/*  X       (input) REAL array, dimension (LDX,NRHS) */
/*          If TRANS = 'N', the n-by-nrhs matrix X. */
/*          If TRANS = 'T', the m-by-nrhs matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X. */
/*          If TRANS = 'N', LDX >= N. */
/*          If TRANS = 'T', LDX >= M. */

/*  B       (input) REAL array, dimension (LDB,NRHS) */
/*          If TRANS = 'N', the m-by-nrhs matrix B. */
/*          If TRANS = 'T', the n-by-nrhs matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. */
/*          If TRANS = 'N', LDB >= M. */
/*          If TRANS = 'T', LDB >= N. */

/*  C       (workspace) REAL array, dimension (LDB,NRHS) */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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
    ret_val = 0.f;

    if (lsame_(trans, "N")) {
	nrows = *m;
	ncols = *n;
    } else if (lsame_(trans, "T")) {
	nrows = *n;
	ncols = *m;
    } else {
	xerbla_("SQRT17", &c__1);
	return ret_val;
    }

    if (*lwork < ncols * *nrhs) {
	xerbla_("SQRT17", &c__13);
	return ret_val;
    }

    if (*m <= 0 || *n <= 0 || *nrhs <= 0) {
	return ret_val;
    }

    norma = slange_("One-norm", m, n, &a[a_offset], lda, rwork);
    smlnum = slamch_("Safe minimum") / slamch_("Precision");
    bignum = 1.f / smlnum;
    iscl = 0;

/*     compute residual and scale it */

    slacpy_("All", &nrows, nrhs, &b[b_offset], ldb, &c__[c_offset], ldb);
    sgemm_(trans, "No transpose", &nrows, nrhs, &ncols, &c_b13, &a[a_offset], 
	    lda, &x[x_offset], ldx, &c_b14, &c__[c_offset], ldb);
    normrs = slange_("Max", &nrows, nrhs, &c__[c_offset], ldb, rwork);
    if (normrs > smlnum) {
	iscl = 1;
	slascl_("General", &c__0, &c__0, &normrs, &c_b14, &nrows, nrhs, &c__[
		c_offset], ldb, &info);
    }

/*     compute R'*A */

    sgemm_("Transpose", trans, nrhs, &ncols, &nrows, &c_b14, &c__[c_offset], 
	    ldb, &a[a_offset], lda, &c_b22, &work[1], nrhs);

/*     compute and properly scale error */

    err = slange_("One-norm", nrhs, &ncols, &work[1], nrhs, rwork);
    if (norma != 0.f) {
	err /= norma;
    }

    if (iscl == 1) {
	err *= normrs;
    }

    if (*iresid == 1) {
	normb = slange_("One-norm", &nrows, nrhs, &b[b_offset], ldb, rwork);
	if (normb != 0.f) {
	    err /= normb;
	}
    } else {
	normx = slange_("One-norm", &ncols, nrhs, &x[x_offset], ldx, rwork);
	if (normx != 0.f) {
	    err /= normx;
	}
    }

/* Computing MAX */
    i__1 = max(*m,*n);
    ret_val = err / (slamch_("Epsilon") * (real) max(i__1,*nrhs));
    return ret_val;

/*     End of SQRT17 */

} /* sqrt17_ */
