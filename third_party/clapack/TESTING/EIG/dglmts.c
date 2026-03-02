#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b13 = -1.;
static doublereal c_b15 = 1.;

/* Subroutine */ int dglmts_(integer *n, integer *m, integer *p, doublereal *
	a, doublereal *af, integer *lda, doublereal *b, doublereal *bf, 
	integer *ldb, doublereal *d__, doublereal *df, doublereal *x, 
	doublereal *u, doublereal *work, integer *lwork, doublereal *rwork, 
	doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset;
    doublereal d__1;

    /* Local variables */
    doublereal eps;
    integer info;
    doublereal unfl;
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm, bnorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    doublereal dnorm, xnorm, ynorm;
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dggglm_(integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    dlacpy_(char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */

/*  Purpose */
/*  ======= */

/*  DGLMTS tests DGGGLM - a subroutine for solving the generalized */
/*  linear model problem. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices A and B.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of columns of the matrix B.  P >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,M) */
/*          The N-by-M matrix A. */

/*  AF      (workspace) DOUBLE PRECISION array, dimension (LDA,M) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF. LDA >= max(M,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,P) */
/*          The N-by-P matrix A. */

/*  BF      (workspace) DOUBLE PRECISION array, dimension (LDB,P) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF. LDB >= max(P,N). */

/*  D       (input) DOUBLE PRECISION array, dimension( N ) */
/*          On input, the left hand side of the GLM. */

/*  DF      (workspace) DOUBLE PRECISION array, dimension( N ) */

/*  X       (output) DOUBLE PRECISION array, dimension( M ) */
/*          solution vector X in the GLM problem. */

/*  U       (output) DOUBLE PRECISION array, dimension( P ) */
/*          solution vector U in the GLM problem. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESULT   (output) DOUBLE PRECISION */
/*          The test ratio: */
/*                           norm( d - A*x - B*u ) */
/*            RESULT = ----------------------------------------- */
/*                     (norm(A)+norm(B))*(norm(x)+norm(u))*EPS */

/*  ==================================================================== */

/*     .. */
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
    eps = dlamch_("Epsilon");
    unfl = dlamch_("Safe minimum");
/* Computing MAX */
    d__1 = dlange_("1", n, m, &a[a_offset], lda, &rwork[1]);
    anorm = max(d__1,unfl);
/* Computing MAX */
    d__1 = dlange_("1", n, p, &b[b_offset], ldb, &rwork[1]);
    bnorm = max(d__1,unfl);

/*     Copy the matrices A and B to the arrays AF and BF, */
/*     and the vector D the array DF. */

    dlacpy_("Full", n, m, &a[a_offset], lda, &af[af_offset], lda);
    dlacpy_("Full", n, p, &b[b_offset], ldb, &bf[bf_offset], ldb);
    dcopy_(n, &d__[1], &c__1, &df[1], &c__1);

/*     Solve GLM problem */

    dggglm_(n, m, p, &af[af_offset], lda, &bf[bf_offset], ldb, &df[1], &x[1], 
	    &u[1], &work[1], lwork, &info);

/*     Test the residual for the solution of LSE */

/*                       norm( d - A*x - B*u ) */
/*       RESULT = ----------------------------------------- */
/*                (norm(A)+norm(B))*(norm(x)+norm(u))*EPS */

    dcopy_(n, &d__[1], &c__1, &df[1], &c__1);
    dgemv_("No transpose", n, m, &c_b13, &a[a_offset], lda, &x[1], &c__1, &
	    c_b15, &df[1], &c__1);

    dgemv_("No transpose", n, p, &c_b13, &b[b_offset], ldb, &u[1], &c__1, &
	    c_b15, &df[1], &c__1);

    dnorm = dasum_(n, &df[1], &c__1);
    xnorm = dasum_(m, &x[1], &c__1) + dasum_(p, &u[1], &c__1);
    ynorm = anorm + bnorm;

    if (xnorm <= 0.) {
	*result = 0.;
    } else {
	*result = dnorm / ynorm / xnorm / eps;
    }

    return 0;

/*     End of DGLMTS */

} /* dglmts_ */
