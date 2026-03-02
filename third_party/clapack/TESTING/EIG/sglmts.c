#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static real c_b13 = -1.f;
static real c_b15 = 1.f;

/* Subroutine */ int sglmts_(integer *n, integer *m, integer *p, real *a, 
	real *af, integer *lda, real *b, real *bf, integer *ldb, real *d__, 
	real *df, real *x, real *u, real *work, integer *lwork, real *rwork, 
	real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset;
    real r__1;

    /* Local variables */
    real eps;
    integer info;
    real unfl, anorm, bnorm, dnorm;
    extern /* Subroutine */ int sgemv_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *, integer *);
    extern doublereal sasum_(integer *, real *, integer *);
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    real xnorm, ynorm;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int sggglm_(integer *, integer *, integer *, real 
	    *, integer *, real *, integer *, real *, real *, real *, real *, 
	    integer *, integer *), slacpy_(char *, integer *, integer *, real 
	    *, integer *, real *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */

/*  Purpose */
/*  ======= */

/*  SGLMTS tests SGGGLM - a subroutine for solving the generalized */
/*  linear model problem. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices A and B.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of columns of the matrix B.  P >= 0. */

/*  A       (input) REAL array, dimension (LDA,M) */
/*          The N-by-M matrix A. */

/*  AF      (workspace) REAL array, dimension (LDA,M) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF. LDA >= max(M,N). */

/*  B       (input) REAL array, dimension (LDB,P) */
/*          The N-by-P matrix A. */

/*  BF      (workspace) REAL array, dimension (LDB,P) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF. LDB >= max(P,N). */

/*  D       (input) REAL array, dimension( N ) */
/*          On input, the left hand side of the GLM. */

/*  DF      (workspace) REAL array, dimension( N ) */

/*  X       (output) REAL array, dimension( M ) */
/*          solution vector X in the GLM problem. */

/*  U       (output) REAL array, dimension( P ) */
/*          solution vector U in the GLM problem. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESULT   (output) REAL */
/*          The test ratio: */
/*                           norm( d - A*x - B*u ) */
/*            RESULT = ----------------------------------------- */
/*                     (norm(A)+norm(B))*(norm(x)+norm(u))*EPS */

/*  ==================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */

/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

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
    --d__;
    --df;
    --x;
    --u;
    --work;
    --rwork;

    /* Function Body */
    eps = slamch_("Epsilon");
    unfl = slamch_("Safe minimum");
/* Computing MAX */
    r__1 = slange_("1", n, m, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);
/* Computing MAX */
    r__1 = slange_("1", n, p, &b[b_offset], ldb, &rwork[1]);
    bnorm = dmax(r__1,unfl);

/*     Copy the matrices A and B to the arrays AF and BF, */
/*     and the vector D the array DF. */

    slacpy_("Full", n, m, &a[a_offset], lda, &af[af_offset], lda);
    slacpy_("Full", n, p, &b[b_offset], ldb, &bf[bf_offset], ldb);
    scopy_(n, &d__[1], &c__1, &df[1], &c__1);

/*     Solve GLM problem */

    sggglm_(n, m, p, &af[af_offset], lda, &bf[bf_offset], ldb, &df[1], &x[1], 
	    &u[1], &work[1], lwork, &info);

/*     Test the residual for the solution of LSE */

/*                       norm( d - A*x - B*u ) */
/*       RESULT = ----------------------------------------- */
/*                (norm(A)+norm(B))*(norm(x)+norm(u))*EPS */

    scopy_(n, &d__[1], &c__1, &df[1], &c__1);
    sgemv_("No transpose", n, m, &c_b13, &a[a_offset], lda, &x[1], &c__1, &
	    c_b15, &df[1], &c__1);

    sgemv_("No transpose", n, p, &c_b13, &b[b_offset], ldb, &u[1], &c__1, &
	    c_b15, &df[1], &c__1);

    dnorm = sasum_(n, &df[1], &c__1);
    xnorm = sasum_(m, &x[1], &c__1) + sasum_(p, &u[1], &c__1);
    ynorm = anorm + bnorm;

    if (xnorm <= 0.f) {
	*result = 0.f;
    } else {
	*result = dnorm / ynorm / xnorm / eps;
    }

    return 0;

/*     End of SGLMTS */

} /* sglmts_ */
