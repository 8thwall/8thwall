#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__7 = 7;
static integer c__1 = 1;
static doublereal c_b6 = 0.;
static integer c__0 = 0;
static doublereal c_b33 = -1.;

doublereal dqrt12_(integer *m, integer *n, doublereal *a, integer *lda, 
	doublereal *s, doublereal *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    doublereal ret_val;

    /* Local variables */
    integer i__, j, mn, iscl, info;
    doublereal anrm;
    extern doublereal dnrm2_(integer *, doublereal *, integer *), dasum_(
	    integer *, doublereal *, integer *);
    extern /* Subroutine */ int daxpy_(integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *), dgebd2_(integer *, integer *, 
	     doublereal *, integer *, doublereal *, doublereal *, doublereal *
, doublereal *, doublereal *, integer *);
    doublereal dummy[1];
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlascl_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *), dlaset_(char *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *, integer *), 
	    xerbla_(char *, integer *), dbdsqr_(char *, integer *, 
	    integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *);
    doublereal bignum, smlnum, nrmsvl;


/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DQRT12 computes the singular values `svlues' of the upper trapezoid */
/*  of A(1:M,1:N) and returns the ratio */

/*       || s - svlues||/(||svlues||*eps*max(M,N)) */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The M-by-N matrix A. Only the upper trapezoid is referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  S       (input) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The singular values of the matrix A. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK. LWORK >= max(M*N + 4*min(M,N) + */
/*          max(M,N), M*N+2*MIN( M, N )+4*N). */

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
    --s;
    --work;

    /* Function Body */
    ret_val = 0.;

/*     Test that enough workspace is supplied */

/* Computing MAX */
    i__1 = *m * *n + (min(*m,*n) << 2) + max(*m,*n), i__2 = *m * *n + (min(*m,
	    *n) << 1) + (*n << 2);
    if (*lwork < max(i__1,i__2)) {
	xerbla_("DQRT12", &c__7);
	return ret_val;
    }

/*     Quick return if possible */

    mn = min(*m,*n);
    if ((doublereal) mn <= 0.) {
	return ret_val;
    }

    nrmsvl = dnrm2_(&mn, &s[1], &c__1);

/*     Copy upper triangle of A into work */

    dlaset_("Full", m, n, &c_b6, &c_b6, &work[1], m);
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = min(j,*m);
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[(j - 1) * *m + i__] = a[i__ + j * a_dim1];
/* L10: */
	}
/* L20: */
    }

/*     Get machine parameters */

    smlnum = dlamch_("S") / dlamch_("P");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Scale work if max entry outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &work[1], m, dummy);
    iscl = 0;
    if (anrm > 0. && anrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &work[1], m, &info);
	iscl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &work[1], m, &info);
	iscl = 1;
    }

    if (anrm != 0.) {

/*        Compute SVD of work */

	dgebd2_(m, n, &work[1], m, &work[*m * *n + 1], &work[*m * *n + mn + 1]
, &work[*m * *n + (mn << 1) + 1], &work[*m * *n + mn * 3 + 1], 
		 &work[*m * *n + (mn << 2) + 1], &info);
	dbdsqr_("Upper", &mn, &c__0, &c__0, &c__0, &work[*m * *n + 1], &work[*
		m * *n + mn + 1], dummy, &mn, dummy, &c__1, dummy, &mn, &work[
		*m * *n + (mn << 1) + 1], &info);

	if (iscl == 1) {
	    if (anrm > bignum) {
		dlascl_("G", &c__0, &c__0, &bignum, &anrm, &mn, &c__1, &work[*
			m * *n + 1], &mn, &info);
	    }
	    if (anrm < smlnum) {
		dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &mn, &c__1, &work[*
			m * *n + 1], &mn, &info);
	    }
	}

    } else {

	i__1 = mn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[*m * *n + i__] = 0.;
/* L30: */
	}
    }

/*     Compare s and singular values of work */

    daxpy_(&mn, &c_b33, &s[1], &c__1, &work[*m * *n + 1], &c__1);
    ret_val = dasum_(&mn, &work[*m * *n + 1], &c__1) / (dlamch_("Epsilon") * (doublereal) max(*m,*n));
    if (nrmsvl != 0.) {
	ret_val /= nrmsvl;
    }

    return ret_val;

/*     End of DQRT12 */

} /* dqrt12_ */
