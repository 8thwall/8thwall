#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;

/* Subroutine */ int dlatb9_(char *path, integer *imat, integer *m, integer *
	p, integer *n, char *type__, integer *kla, integer *kua, integer *klb, 
	 integer *kub, doublereal *anorm, doublereal *bnorm, integer *modea, 
	integer *modeb, doublereal *cndnma, doublereal *cndnmb, char *dista, 
	char *distb)
{
    /* Initialized data */

    static logical first = TRUE_;

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static doublereal eps, badc1, badc2, large, small;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    extern logical lsamen_(integer *, char *, char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATB9 sets parameters for the matrix generator based on the type of */
/*  matrix to be generated. */

/*  Arguments */
/*  ========= */

/*  PATH    (input) CHARACTER*3 */
/*          The LAPACK path name. */

/*  IMAT    (input) INTEGER */
/*          An integer key describing which matrix to generate for this */
/*          path. */

/*  M       (input) INTEGER */
/*          The number of rows in the matrix to be generated. */

/*  N       (input) INTEGER */
/*          The number of columns in the matrix to be generated. */

/*  TYPE    (output) CHARACTER*1 */
/*          The type of the matrix to be generated: */
/*          = 'S':  symmetric matrix; */
/*          = 'P':  symmetric positive (semi)definite matrix; */
/*          = 'N':  nonsymmetric matrix. */

/*  KL      (output) INTEGER */
/*          The lower band width of the matrix to be generated. */

/*  KU      (output) INTEGER */
/*          The upper band width of the matrix to be generated. */

/*  ANORM   (output) DOUBLE PRECISION */
/*          The desired norm of the matrix to be generated.  The diagonal */
/*          matrix of singular values or eigenvalues is scaled by this */
/*          value. */

/*  MODE    (output) INTEGER */
/*          A key indicating how to choose the vector of eigenvalues. */

/*  CNDNUM  (output) DOUBLE PRECISION */
/*          The desired condition number. */

/*  DIST    (output) CHARACTER*1 */
/*          The type of distribution to be used by the random number */
/*          generator. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Save statement .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Set some constants for use in the subroutine. */

    if (first) {
	first = FALSE_;
	eps = dlamch_("Precision");
	badc2 = .1 / eps;
	badc1 = sqrt(badc2);
	small = dlamch_("Safe minimum");
	large = 1. / small;

/*        If it looks like we're on a Cray, take the square root of */
/*        SMALL and LARGE to avoid overflow and underflow problems. */

	dlabad_(&small, &large);
	small = small / eps * .25;
	large = 1. / small;
    }

/*     Set some parameters we don't plan to change. */

    *(unsigned char *)type__ = 'N';
    *(unsigned char *)dista = 'S';
    *(unsigned char *)distb = 'S';
    *modea = 3;
    *modeb = 4;

/*     Set the lower and upper bandwidths. */

    if (lsamen_(&c__3, path, "GRQ") || lsamen_(&c__3, 
	    path, "LSE") || lsamen_(&c__3, path, "GSV")) {

/*        A: M by N, B: P by N */

	if (*imat == 1) {

/*           A: diagonal, B: upper triangular */

	    *kla = 0;
	    *kua = 0;
	    *klb = 0;
/* Computing MAX */
	    i__1 = *n - 1;
	    *kub = max(i__1,0);

	} else if (*imat == 2) {

/*           A: upper triangular, B: upper triangular */

	    *kla = 0;
/* Computing MAX */
	    i__1 = *n - 1;
	    *kua = max(i__1,0);
	    *klb = 0;
/* Computing MAX */
	    i__1 = *n - 1;
	    *kub = max(i__1,0);

	} else if (*imat == 3) {

/*           A: lower triangular, B: upper triangular */

/* Computing MAX */
	    i__1 = *m - 1;
	    *kla = max(i__1,0);
	    *kua = 0;
	    *klb = 0;
/* Computing MAX */
	    i__1 = *n - 1;
	    *kub = max(i__1,0);

	} else {

/*           A: general dense, B: general dense */

/* Computing MAX */
	    i__1 = *m - 1;
	    *kla = max(i__1,0);
/* Computing MAX */
	    i__1 = *n - 1;
	    *kua = max(i__1,0);
/* Computing MAX */
	    i__1 = *p - 1;
	    *klb = max(i__1,0);
/* Computing MAX */
	    i__1 = *n - 1;
	    *kub = max(i__1,0);

	}

    } else if (lsamen_(&c__3, path, "GQR") || lsamen_(&
	    c__3, path, "GLM")) {

/*        A: N by M, B: N by P */

	if (*imat == 1) {

/*           A: diagonal, B: lower triangular */

	    *kla = 0;
	    *kua = 0;
/* Computing MAX */
	    i__1 = *n - 1;
	    *klb = max(i__1,0);
	    *kub = 0;
	} else if (*imat == 2) {

/*           A: lower triangular, B: diagonal */

/* Computing MAX */
	    i__1 = *n - 1;
	    *kla = max(i__1,0);
	    *kua = 0;
	    *klb = 0;
	    *kub = 0;

	} else if (*imat == 3) {

/*           A: lower triangular, B: upper triangular */

/* Computing MAX */
	    i__1 = *n - 1;
	    *kla = max(i__1,0);
	    *kua = 0;
	    *klb = 0;
/* Computing MAX */
	    i__1 = *p - 1;
	    *kub = max(i__1,0);

	} else {

/*           A: general dense, B: general dense */

/* Computing MAX */
	    i__1 = *n - 1;
	    *kla = max(i__1,0);
/* Computing MAX */
	    i__1 = *m - 1;
	    *kua = max(i__1,0);
/* Computing MAX */
	    i__1 = *n - 1;
	    *klb = max(i__1,0);
/* Computing MAX */
	    i__1 = *p - 1;
	    *kub = max(i__1,0);
	}

    }

/*     Set the condition number and norm. */

    *cndnma = 100.;
    *cndnmb = 10.;
    if (lsamen_(&c__3, path, "GQR") || lsamen_(&c__3, 
	    path, "GRQ") || lsamen_(&c__3, path, "GSV")) {
	if (*imat == 5) {
	    *cndnma = badc1;
	    *cndnmb = badc1;
	} else if (*imat == 6) {
	    *cndnma = badc2;
	    *cndnmb = badc2;
	} else if (*imat == 7) {
	    *cndnma = badc1;
	    *cndnmb = badc2;
	} else if (*imat == 8) {
	    *cndnma = badc2;
	    *cndnmb = badc1;
	}
    }

    *anorm = 10.;
    *bnorm = 1e3;
    if (lsamen_(&c__3, path, "GQR") || lsamen_(&c__3, 
	    path, "GRQ")) {
	if (*imat == 7) {
	    *anorm = small;
	    *bnorm = large;
	} else if (*imat == 8) {
	    *anorm = large;
	    *bnorm = small;
	}
    }

    if (*n <= 1) {
	*cndnma = 1.;
	*cndnmb = 1.;
    }

    return 0;

/*     End of DLATB9 */

} /* dlatb9_ */
