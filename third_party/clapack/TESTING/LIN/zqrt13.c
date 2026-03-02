#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__1 = 1;
static integer c__0 = 0;

/* Subroutine */ int zqrt13_(integer *scale, integer *m, integer *n, 
	doublecomplex *a, integer *lda, doublereal *norma, integer *iseed)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *);

    /* Local variables */
    integer j, info;
    doublereal dummy[1];
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    doublereal bignum;
    extern /* Subroutine */ int zlascl_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublecomplex *, 
	     integer *, integer *);
    extern doublereal dzasum_(integer *, doublecomplex *, integer *);
    doublereal smlnum;
    extern /* Subroutine */ int zlarnv_(integer *, integer *, integer *, 
	    doublecomplex *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZQRT13 generates a full-rank matrix that may be scaled to have large */
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

/*  A       (output) COMPLEX*16 array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  NORMA   (output) DOUBLE PRECISION */
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
	zlarnv_(&c__2, &iseed[1], m, &a[j * a_dim1 + 1]);
	if (j <= *m) {
	    i__2 = j + j * a_dim1;
	    i__3 = j + j * a_dim1;
	    d__2 = dzasum_(m, &a[j * a_dim1 + 1], &c__1);
	    i__4 = j + j * a_dim1;
	    d__3 = a[i__4].r;
	    d__1 = d_sign(&d__2, &d__3);
	    z__2.r = d__1, z__2.i = 0.;
	    z__1.r = a[i__3].r + z__2.r, z__1.i = a[i__3].i + z__2.i;
	    a[i__2].r = z__1.r, a[i__2].i = z__1.i;
	}
/* L10: */
    }

/*     scaled versions */

    if (*scale != 1) {
	*norma = zlange_("Max", m, n, &a[a_offset], lda, dummy);
	smlnum = dlamch_("Safe minimum");
	bignum = 1. / smlnum;
	dlabad_(&smlnum, &bignum);
	smlnum /= dlamch_("Epsilon");
	bignum = 1. / smlnum;

	if (*scale == 2) {

/*           matrix scaled up */

	    zlascl_("General", &c__0, &c__0, norma, &bignum, m, n, &a[
		    a_offset], lda, &info);
	} else if (*scale == 3) {

/*           matrix scaled down */

	    zlascl_("General", &c__0, &c__0, norma, &smlnum, m, n, &a[
		    a_offset], lda, &info);
	}
    }

    *norma = zlange_("One-norm", m, n, &a[a_offset], lda, dummy);
    return 0;

/*     End of ZQRT13 */

} /* zqrt13_ */
