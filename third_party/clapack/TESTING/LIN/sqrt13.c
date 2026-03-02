#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__1 = 1;
static integer c__0 = 0;

/* Subroutine */ int sqrt13_(integer *scale, integer *m, integer *n, real *a, 
	integer *lda, real *norma, integer *iseed)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    real r__1;

    /* Builtin functions */
    double r_sign(real *, real *);

    /* Local variables */
    integer j, info;
    extern doublereal sasum_(integer *, real *, integer *);
    real dummy[1];
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    real bignum;
    extern /* Subroutine */ int slascl_(char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *), slarnv_(integer *, integer *, integer *, real *);
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

/*  SQRT13 generates a full-rank matrix that may be scaled to have large */
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

/*  A       (output) REAL array, dimension (LDA,N) */
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
	slarnv_(&c__2, &iseed[1], m, &a[j * a_dim1 + 1]);
	if (j <= *m) {
	    r__1 = sasum_(m, &a[j * a_dim1 + 1], &c__1);
	    a[j + j * a_dim1] += r_sign(&r__1, &a[j + j * a_dim1]);
	}
/* L10: */
    }

/*     scaled versions */

    if (*scale != 1) {
	*norma = slange_("Max", m, n, &a[a_offset], lda, dummy);
	smlnum = slamch_("Safe minimum");
	bignum = 1.f / smlnum;
	slabad_(&smlnum, &bignum);
	smlnum /= slamch_("Epsilon");
	bignum = 1.f / smlnum;

	if (*scale == 2) {

/*           matrix scaled up */

	    slascl_("General", &c__0, &c__0, norma, &bignum, m, n, &a[
		    a_offset], lda, &info);
	} else if (*scale == 3) {

/*           matrix scaled down */

	    slascl_("General", &c__0, &c__0, norma, &smlnum, m, n, &a[
		    a_offset], lda, &info);
	}
    }

    *norma = slange_("One-norm", m, n, &a[a_offset], lda, dummy);
    return 0;

/*     End of SQRT13 */

} /* sqrt13_ */
