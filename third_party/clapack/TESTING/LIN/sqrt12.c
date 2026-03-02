#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__7 = 7;
static integer c__1 = 1;
static real c_b6 = 0.f;
static integer c__0 = 0;
static real c_b33 = -1.f;

doublereal sqrt12_(integer *m, integer *n, real *a, integer *lda, real *s, 
	real *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    real ret_val;

    /* Local variables */
    integer i__, j, mn, iscl, info;
    real anrm;
    extern doublereal snrm2_(integer *, real *, integer *), sasum_(integer *, 
	    real *, integer *);
    real dummy[1];
    extern /* Subroutine */ int saxpy_(integer *, real *, real *, integer *, 
	    real *, integer *), sgebd2_(integer *, integer *, real *, integer 
	    *, real *, real *, real *, real *, real *, integer *), slabad_(
	    real *, real *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    real bignum;
    extern /* Subroutine */ int slascl_(char *, integer *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *), slaset_(char *, integer *, integer *, real *, real *, 
	    real *, integer *), sbdsqr_(char *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *);
    real smlnum, nrmsvl;


/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SQRT12 computes the singular values `svlues' of the upper trapezoid */
/*  of A(1:M,1:N) and returns the ratio */

/*       || s - svlues||/(||svlues||*eps*max(M,N)) */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. */

/*  A       (input) REAL array, dimension (LDA,N) */
/*          The M-by-N matrix A. Only the upper trapezoid is referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  S       (input) REAL array, dimension (min(M,N)) */
/*          The singular values of the matrix A. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

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
    ret_val = 0.f;

/*     Test that enough workspace is supplied */

/* Computing MAX */
    i__1 = *m * *n + (min(*m,*n) << 2) + max(*m,*n), i__2 = *m * *n + (min(*m,
	    *n) << 1) + (*n << 2);
    if (*lwork < max(i__1,i__2)) {
	xerbla_("SQRT12", &c__7);
	return ret_val;
    }

/*     Quick return if possible */

    mn = min(*m,*n);
    if ((real) mn <= 0.f) {
	return ret_val;
    }

    nrmsvl = snrm2_(&mn, &s[1], &c__1);

/*     Copy upper triangle of A into work */

    slaset_("Full", m, n, &c_b6, &c_b6, &work[1], m);
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

    smlnum = slamch_("S") / slamch_("P");
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);

/*     Scale work if max entry outside range [SMLNUM,BIGNUM] */

    anrm = slange_("M", m, n, &work[1], m, dummy);
    iscl = 0;
    if (anrm > 0.f && anrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	slascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &work[1], m, &info);
	iscl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	slascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &work[1], m, &info);
	iscl = 1;
    }

    if (anrm != 0.f) {

/*        Compute SVD of work */

	sgebd2_(m, n, &work[1], m, &work[*m * *n + 1], &work[*m * *n + mn + 1]
, &work[*m * *n + (mn << 1) + 1], &work[*m * *n + mn * 3 + 1], 
		 &work[*m * *n + (mn << 2) + 1], &info);
	sbdsqr_("Upper", &mn, &c__0, &c__0, &c__0, &work[*m * *n + 1], &work[*
		m * *n + mn + 1], dummy, &mn, dummy, &c__1, dummy, &mn, &work[
		*m * *n + (mn << 1) + 1], &info);

	if (iscl == 1) {
	    if (anrm > bignum) {
		slascl_("G", &c__0, &c__0, &bignum, &anrm, &mn, &c__1, &work[*
			m * *n + 1], &mn, &info);
	    }
	    if (anrm < smlnum) {
		slascl_("G", &c__0, &c__0, &smlnum, &anrm, &mn, &c__1, &work[*
			m * *n + 1], &mn, &info);
	    }
	}

    } else {

	i__1 = mn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[*m * *n + i__] = 0.f;
/* L30: */
	}
    }

/*     Compare s and singular values of work */

    saxpy_(&mn, &c_b33, &s[1], &c__1, &work[*m * *n + 1], &c__1);
    ret_val = sasum_(&mn, &work[*m * *n + 1], &c__1) / (slamch_("Epsilon") * (real) max(*m,*n));
    if (nrmsvl != 0.f) {
	ret_val /= nrmsvl;
    }

    return ret_val;

/*     End of SQRT12 */

} /* sqrt12_ */
