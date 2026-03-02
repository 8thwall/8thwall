#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__1 = 1;
static integer c__0 = 0;

/* Subroutine */ int cqrt13_(integer *scale, integer *m, integer *n, complex *
	a, integer *lda, real *norma, integer *iseed)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2, r__3;
    complex q__1, q__2;

    /* Builtin functions */
    double r_sign(real *, real *);

    /* Local variables */
    integer j, info;
    real dummy[1];
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *);
    extern /* Subroutine */ int clascl_(char *, integer *, integer *, real *, 
	    real *, integer *, integer *, complex *, integer *, integer *);
    extern doublereal slamch_(char *);
    real bignum;
    extern /* Subroutine */ int clarnv_(integer *, integer *, integer *, 
	    complex *);
    extern doublereal scasum_(integer *, complex *, integer *);
    real smlnum;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CQRT13 generates a full-rank matrix that may be scaled to have large */
/*  or small norm. */

/*  Arguments */
/*  ========= */

/*  SCALE   (input) INTEGER */
/*          SCALE = 1: normally scaled matrix */
/*          SCALE = 2: matrix scaled up */
/*          SCALE = 3: matrix scaled down */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */

/*  N       (input) INTEGER */
/*          The number of columns of A. */

/*  A       (output) COMPLEX array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  NORMA   (output) REAL */
/*          The one-norm of A. */

/*  ISEED   (input/output) integer array, dimension (4) */
/*          Seed for random number generator */

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
/*     .. Local Arrays .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --iseed;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

/*     benign matrix */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	clarnv_(&c__2, &iseed[1], m, &a[j * a_dim1 + 1]);
	if (j <= *m) {
	    i__2 = j + j * a_dim1;
	    i__3 = j + j * a_dim1;
	    r__2 = scasum_(m, &a[j * a_dim1 + 1], &c__1);
	    i__4 = j + j * a_dim1;
	    r__3 = a[i__4].r;
	    r__1 = r_sign(&r__2, &r__3);
	    q__2.r = r__1, q__2.i = 0.f;
	    q__1.r = a[i__3].r + q__2.r, q__1.i = a[i__3].i + q__2.i;
	    a[i__2].r = q__1.r, a[i__2].i = q__1.i;
	}
/* L10: */
    }

/*     scaled versions */

    if (*scale != 1) {
	*norma = clange_("Max", m, n, &a[a_offset], lda, dummy);
	smlnum = slamch_("Safe minimum");
	bignum = 1.f / smlnum;
	slabad_(&smlnum, &bignum);
	smlnum /= slamch_("Epsilon");
	bignum = 1.f / smlnum;

	if (*scale == 2) {

/*           matrix scaled up */

	    clascl_("General", &c__0, &c__0, norma, &bignum, m, n, &a[
		    a_offset], lda, &info);
	} else if (*scale == 3) {

/*           matrix scaled down */

	    clascl_("General", &c__0, &c__0, norma, &smlnum, m, n, &a[
		    a_offset], lda, &info);
	}
    }

    *norma = clange_("One-norm", m, n, &a[a_offset], lda, dummy);
    return 0;

/*     End of CQRT13 */

} /* cqrt13_ */
