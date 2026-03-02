#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static doublecomplex c_b13 = {-1.,-0.};
static doublecomplex c_b15 = {1.,0.};

/* Subroutine */ int zglmts_(integer *n, integer *m, integer *p, 
	doublecomplex *a, doublecomplex *af, integer *lda, doublecomplex *b, 
	doublecomplex *bf, integer *ldb, doublecomplex *d__, doublecomplex *
	df, doublecomplex *x, doublecomplex *u, doublecomplex *work, integer *
	lwork, doublereal *rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset;
    doublereal d__1;

    /* Local variables */
    doublereal eps;
    integer info;
    doublereal unfl, anorm, bnorm, dnorm;
    extern /* Subroutine */ int zgemv_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *);
    doublereal xnorm, ynorm;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    extern /* Subroutine */ int zggglm_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex *
, integer *, integer *), zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    extern doublereal dzasum_(integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */

/*  Purpose */
/*  ======= */

/*  ZGLMTS tests ZGGGLM - a subroutine for solving the generalized */
/*  linear model problem. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices A and B.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of columns of the matrix B.  P >= 0. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,M) */
/*          The N-by-M matrix A. */

/*  AF      (workspace) COMPLEX*16 array, dimension (LDA,M) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A, AF. LDA >= max(M,N). */

/*  B       (input) COMPLEX*16 array, dimension (LDB,P) */
/*          The N-by-P matrix A. */

/*  BF      (workspace) COMPLEX*16 array, dimension (LDB,P) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B, BF. LDB >= max(P,N). */

/*  D       (input) COMPLEX*16 array, dimension( N ) */
/*          On input, the left hand side of the GLM. */

/*  DF      (workspace) COMPLEX*16 array, dimension( N ) */

/*  X       (output) COMPLEX*16 array, dimension( M ) */
/*          solution vector X in the GLM problem. */

/*  U       (output) COMPLEX*16 array, dimension( P ) */
/*          solution vector U in the GLM problem. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

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
    d__1 = zlange_("1", n, m, &a[a_offset], lda, &rwork[1]);
    anorm = max(d__1,unfl);
/* Computing MAX */
    d__1 = zlange_("1", n, p, &b[b_offset], ldb, &rwork[1]);
    bnorm = max(d__1,unfl);

/*     Copy the matrices A and B to the arrays AF and BF, */
/*     and the vector D the array DF. */

    zlacpy_("Full", n, m, &a[a_offset], lda, &af[af_offset], lda);
    zlacpy_("Full", n, p, &b[b_offset], ldb, &bf[bf_offset], ldb);
    zcopy_(n, &d__[1], &c__1, &df[1], &c__1);

/*     Solve GLM problem */

    zggglm_(n, m, p, &af[af_offset], lda, &bf[bf_offset], ldb, &df[1], &x[1], 
	    &u[1], &work[1], lwork, &info);

/*     Test the residual for the solution of LSE */

/*                       norm( d - A*x - B*u ) */
/*       RESULT = ----------------------------------------- */
/*                (norm(A)+norm(B))*(norm(x)+norm(u))*EPS */

    zcopy_(n, &d__[1], &c__1, &df[1], &c__1);
    zgemv_("No transpose", n, m, &c_b13, &a[a_offset], lda, &x[1], &c__1, &
	    c_b15, &df[1], &c__1);

    zgemv_("No transpose", n, p, &c_b13, &b[b_offset], ldb, &u[1], &c__1, &
	    c_b15, &df[1], &c__1);

    dnorm = dzasum_(n, &df[1], &c__1);
    xnorm = dzasum_(m, &x[1], &c__1) + dzasum_(p, &u[1], &c__1);
    ynorm = anorm + bnorm;

    if (xnorm <= 0.) {
	*result = 0.;
    } else {
	*result = dnorm / ynorm / xnorm / eps;
    }

    return 0;

/*     End of ZGLMTS */

} /* zglmts_ */
