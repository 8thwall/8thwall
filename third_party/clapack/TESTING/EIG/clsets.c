#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int clsets_(integer *m, integer *p, integer *n, complex *a, 
	complex *af, integer *lda, complex *b, complex *bf, integer *ldb, 
	complex *c__, complex *cf, complex *d__, complex *df, complex *x, 
	complex *work, integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset;

    /* Local variables */
    integer info;
    extern /* Subroutine */ int cget02_(char *, integer *, integer *, integer 
	    *, complex *, integer *, complex *, integer *, complex *, integer 
	    *, real *, real *), ccopy_(integer *, complex *, integer *
, complex *, integer *), cgglse_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, complex *, 
	    complex *, complex *, integer *, integer *), clacpy_(char *, 
	    integer *, integer *, complex *, integer *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */

/*  Purpose */
/*  ======= */

/*  CLSETS tests CGGLSE - a subroutine for solving linear equality */
/*  constrained least square problem (LSE). */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  AF      (workspace) COMPLEX array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF, Q and R. */
/*          LDA >= max(M,N). */

/*  B       (input) COMPLEX array, dimension (LDB,N) */
/*          The P-by-N matrix A. */

/*  BF      (workspace) COMPLEX array, dimension (LDB,N) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF, V and S. */
/*          LDB >= max(P,N). */

/*  C       (input) COMPLEX array, dimension( M ) */
/*          the vector C in the LSE problem. */

/*  CF      (workspace) COMPLEX array, dimension( M ) */

/*  D       (input) COMPLEX array, dimension( P ) */
/*          the vector D in the LSE problem. */

/*  DF      (workspace) COMPLEX array, dimension( P ) */

/*  X       (output) COMPLEX array, dimension( N ) */
/*          solution vector X in the LSE problem. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESULT  (output) REAL array, dimension (2) */
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
    clacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);
    clacpy_("Full", p, n, &b[b_offset], ldb, &bf[bf_offset], ldb);
    ccopy_(m, &c__[1], &c__1, &cf[1], &c__1);
    ccopy_(p, &d__[1], &c__1, &df[1], &c__1);

/*     Solve LSE problem */

    cgglse_(m, n, p, &af[af_offset], lda, &bf[bf_offset], ldb, &cf[1], &df[1], 
	     &x[1], &work[1], lwork, &info);

/*     Test the residual for the solution of LSE */

/*     Compute RESULT(1) = norm( A*x - c ) / norm(A)*norm(X)*EPS */

    ccopy_(m, &c__[1], &c__1, &cf[1], &c__1);
    ccopy_(p, &d__[1], &c__1, &df[1], &c__1);
    cget02_("No transpose", m, n, &c__1, &a[a_offset], lda, &x[1], n, &cf[1], 
	    m, &rwork[1], &result[1]);

/*     Compute result(2) = norm( B*x - d ) / norm(B)*norm(X)*EPS */

    cget02_("No transpose", p, n, &c__1, &b[b_offset], ldb, &x[1], n, &df[1], 
	    p, &rwork[1], &result[2]);

    return 0;

/*     End of CLSETS */

} /* clsets_ */
