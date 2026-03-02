#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int zlsets_(integer *m, integer *p, integer *n, 
	doublecomplex *a, doublecomplex *af, integer *lda, doublecomplex *b, 
	doublecomplex *bf, integer *ldb, doublecomplex *c__, doublecomplex *
	cf, doublecomplex *d__, doublecomplex *df, doublecomplex *x, 
	doublecomplex *work, integer *lwork, doublereal *rwork, doublereal *
	result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset;

    /* Local variables */
    integer info;
    extern /* Subroutine */ int zget02_(char *, integer *, integer *, integer 
	    *, doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *), 
	    zcopy_(integer *, doublecomplex *, integer *, doublecomplex *, 
	    integer *), zgglse_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex *
, integer *, integer *), zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */

/*  Purpose */
/*  ======= */

/*  ZLSETS tests ZGGLSE - a subroutine for solving linear equality */
/*  constrained least square problem (LSE). */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  AF      (workspace) COMPLEX*16 array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  B       (input) COMPLEX*16 array, dimension (LDB,N) */
/*          The P-by-N matrix A. */

/*  BF      (workspace) COMPLEX*16 array, dimension (LDB,N) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF, V and S. */
/*          LDB >= max(P,N). */

/*  C       (input) COMPLEX*16 array, dimension( M ) */
/*          the vector C in the LSE problem. */

/*  CF      (workspace) COMPLEX*16 array, dimension( M ) */

/*  D       (input) COMPLEX*16 array, dimension( P ) */
/*          the vector D in the LSE problem. */

/*  DF      (workspace) COMPLEX*16 array, dimension( P ) */

/*  X       (output) COMPLEX*16 array, dimension( N ) */
/*          solution vector X in the LSE problem. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (2) */
/*          The test ratios: */
/*            RESULT(1) = norm( A*x - c )/ norm(A)*norm(X)*EPS */
/*            RESULT(2) = norm( B*x - d )/ norm(B)*norm(X)*EPS */

/*  ==================================================================== */

/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Copy the matrices A and B to the arrays AF and BF, */
/*     and the vectors C and D to the arrays CF and DF, */

    /* Parameter adjustments */
    af_dim1 = *lda;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    bf_dim1 = *ldb;
    bf_offset = 1 + bf_dim1;
    bf -= bf_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --c__;
    --cf;
    --d__;
    --df;
    --x;
    --work;
    --rwork;
    --result;

    /* Function Body */
    zlacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);
    zlacpy_("Full", p, n, &b[b_offset], ldb, &bf[bf_offset], ldb);
    zcopy_(m, &c__[1], &c__1, &cf[1], &c__1);
    zcopy_(p, &d__[1], &c__1, &df[1], &c__1);

/*     Solve LSE problem */

    zgglse_(m, n, p, &af[af_offset], lda, &bf[bf_offset], ldb, &cf[1], &df[1], 
	     &x[1], &work[1], lwork, &info);

/*     Test the residual for the solution of LSE */

/*     Compute RESULT(1) = norm( A*x - c ) / norm(A)*norm(X)*EPS */

    zcopy_(m, &c__[1], &c__1, &cf[1], &c__1);
    zcopy_(p, &d__[1], &c__1, &df[1], &c__1);
    zget02_("No transpose", m, n, &c__1, &a[a_offset], lda, &x[1], n, &cf[1], 
	    m, &rwork[1], &result[1]);

/*     Compute result(2) = norm( B*x - d ) / norm(B)*norm(X)*EPS */

    zget02_("No transpose", p, n, &c__1, &b[b_offset], ldb, &x[1], n, &df[1], 
	    p, &rwork[1], &result[2]);

    return 0;

/*     End of ZLSETS */

} /* zlsets_ */
