#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__8 = 8;
static real c_b6 = 0.f;
static real c_b13 = -1.f;
static integer c__1 = 1;

doublereal srzt01_(integer *m, integer *n, real *a, real *af, integer *lda, 
	real *tau, real *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, i__1, i__2;
    real ret_val;

    /* Local variables */
    integer i__, j, info;
    real norma, rwork[1];
    extern /* Subroutine */ int saxpy_(integer *, real *, real *, integer *, 
	    real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int xerbla_(char *, integer *), slaset_(
	    char *, integer *, integer *, real *, real *, real *, integer *), sormrz_(char *, char *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SRZT01 returns */
/*       || A - R*Q || / ( M * eps * ||A|| ) */
/*  for an upper trapezoidal A that was factored with STZRZF. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrices A and AF. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and AF. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The original upper trapezoidal M by N matrix A. */

/*  AF      (input) REAL array, dimension (LDA,N) */
/*          The output of STZRZF for input matrix A. */
/*          The lower triangle is not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A and AF. */

/*  TAU     (input) REAL array, dimension (M) */
/*          Details of the Householder transformations as returned by */
/*          STZRZF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= m*n + m*nb. */

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
    af_dim1 = *lda;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    ret_val = 0.f;

    if (*lwork < *m * *n + *m) {
	xerbla_("SRZT01", &c__8);
	return ret_val;
    }

/*     Quick return if possible */

    if (*m <= 0 || *n <= 0) {
	return ret_val;
    }

    norma = slange_("One-norm", m, n, &a[a_offset], lda, rwork);

/*     Copy upper triangle R */

    slaset_("Full", m, n, &c_b6, &c_b6, &work[1], m);
    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[(j - 1) * *m + i__] = af[i__ + j * af_dim1];
/* L10: */
	}
/* L20: */
    }

/*     R = R * P(1) * ... *P(m) */

    i__1 = *n - *m;
    i__2 = *lwork - *m * *n;
    sormrz_("Right", "No tranpose", m, n, m, &i__1, &af[af_offset], lda, &tau[
	    1], &work[1], m, &work[*m * *n + 1], &i__2, &info);

/*     R = R - A */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	saxpy_(m, &c_b13, &a[i__ * a_dim1 + 1], &c__1, &work[(i__ - 1) * *m + 
		1], &c__1);
/* L30: */
    }

    ret_val = slange_("One-norm", m, n, &work[1], m, rwork);

    ret_val /= slamch_("Epsilon") * (real) max(*m,*n);
    if (norma != 0.f) {
	ret_val /= norma;
    }

    return ret_val;

/*     End of SRZT01 */

} /* srzt01_ */
