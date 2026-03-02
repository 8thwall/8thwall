#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__10 = 10;
static integer c__1 = 1;
static integer c__0 = 0;
static doublereal c_b15 = 1.;

doublereal zqrt14_(char *trans, integer *m, integer *n, integer *nrhs, 
	doublecomplex *a, integer *lda, doublecomplex *x, integer *ldx, 
	doublecomplex *work, integer *lwork)
{
    /* System generated locals */
    integer a_dim1, a_offset, x_dim1, x_offset, i__1, i__2, i__3;
    doublereal ret_val, d__1, d__2;
    doublecomplex z__1;

    /* Builtin functions */
    double z_abs(doublecomplex *);
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__, j;
    doublereal err;
    integer info;
    doublereal anrm;
    logical tpsd;
    doublereal xnrm;
    extern logical lsame_(char *, char *);
    doublereal rwork[1];
    extern /* Subroutine */ int zgelq2_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, doublecomplex *, integer *), zgeqr2_(
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     doublecomplex *, integer *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, integer *);
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int zlascl_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublecomplex *, 
	     integer *, integer *);
    integer ldwork;
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZQRT14 checks whether X is in the row space of A or A'.  It does so */
/*  by scaling both X and A such that their norms are in the range */
/*  [sqrt(eps), 1/sqrt(eps)], then computing a QR factorization of [A,X] */
/*  (if TRANS = 'C') or an LQ factorization of [A',X]' (if TRANS = 'N'), */
/*  and returning the norm of the trailing triangle, scaled by */
/*  MAX(M,N,NRHS)*eps. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          = 'N':  No transpose, check for X in the row space of A */
/*          = 'C':  Conjugate transpose, check for X in row space of A'. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of X. */

/*  A       (input) COMPLEX*16 array, dimension (LDA,N) */
/*          The M-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  X       (input) COMPLEX*16 array, dimension (LDX,NRHS) */
/*          If TRANS = 'N', the N-by-NRHS matrix X. */
/*          IF TRANS = 'C', the M-by-NRHS matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X. */

/*  WORK    (workspace) COMPLEX*16 array dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          length of workspace array required */
/*          If TRANS = 'N', LWORK >= (M+NRHS)*(N+2); */
/*          if TRANS = 'C', LWORK >= (N+NRHS)*(M+2). */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --work;

    /* Function Body */
    ret_val = 0.;
    if (lsame_(trans, "N")) {
	ldwork = *m + *nrhs;
	tpsd = FALSE_;
	if (*lwork < (*m + *nrhs) * (*n + 2)) {
	    xerbla_("ZQRT14", &c__10);
	    return ret_val;
	} else if (*n <= 0 || *nrhs <= 0) {
	    return ret_val;
	}
    } else if (lsame_(trans, "C")) {
	ldwork = *m;
	tpsd = TRUE_;
	if (*lwork < (*n + *nrhs) * (*m + 2)) {
	    xerbla_("ZQRT14", &c__10);
	    return ret_val;
	} else if (*m <= 0 || *nrhs <= 0) {
	    return ret_val;
	}
    } else {
	xerbla_("ZQRT14", &c__1);
	return ret_val;
    }

/*     Copy and scale A */

    zlacpy_("All", m, n, &a[a_offset], lda, &work[1], &ldwork);
    anrm = zlange_("M", m, n, &work[1], &ldwork, rwork);
    if (anrm != 0.) {
	zlascl_("G", &c__0, &c__0, &anrm, &c_b15, m, n, &work[1], &ldwork, &
		info);
    }

/*     Copy X or X' into the right place and scale it */

    if (tpsd) {

/*        Copy X into columns n+1:n+nrhs of work */

	zlacpy_("All", m, nrhs, &x[x_offset], ldx, &work[*n * ldwork + 1], &
		ldwork);
	xnrm = zlange_("M", m, nrhs, &work[*n * ldwork + 1], &ldwork, rwork);
	if (xnrm != 0.) {
	    zlascl_("G", &c__0, &c__0, &xnrm, &c_b15, m, nrhs, &work[*n * 
		    ldwork + 1], &ldwork, &info);
	}
	i__1 = *n + *nrhs;
	anrm = zlange_("One-norm", m, &i__1, &work[1], &ldwork, rwork);

/*        Compute QR factorization of X */

	i__1 = *n + *nrhs;
/* Computing MIN */
	i__2 = *m, i__3 = *n + *nrhs;
	zgeqr2_(m, &i__1, &work[1], &ldwork, &work[ldwork * (*n + *nrhs) + 1], 
		 &work[ldwork * (*n + *nrhs) + min(i__2, i__3)+ 1], &info);

/*        Compute largest entry in upper triangle of */
/*        work(n+1:m,n+1:n+nrhs) */

	err = 0.;
	i__1 = *n + *nrhs;
	for (j = *n + 1; j <= i__1; ++j) {
	    i__2 = min(*m,j);
	    for (i__ = *n + 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__1 = err, d__2 = z_abs(&work[i__ + (j - 1) * *m]);
		err = max(d__1,d__2);
/* L10: */
	    }
/* L20: */
	}

    } else {

/*        Copy X' into rows m+1:m+nrhs of work */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *nrhs;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = *m + j + (i__ - 1) * ldwork;
		d_cnjg(&z__1, &x[i__ + j * x_dim1]);
		work[i__3].r = z__1.r, work[i__3].i = z__1.i;
/* L30: */
	    }
/* L40: */
	}

	xnrm = zlange_("M", nrhs, n, &work[*m + 1], &ldwork, rwork)
		;
	if (xnrm != 0.) {
	    zlascl_("G", &c__0, &c__0, &xnrm, &c_b15, nrhs, n, &work[*m + 1], 
		    &ldwork, &info);
	}

/*        Compute LQ factorization of work */

	zgelq2_(&ldwork, n, &work[1], &ldwork, &work[ldwork * *n + 1], &work[
		ldwork * (*n + 1) + 1], &info);

/*        Compute largest entry in lower triangle in */
/*        work(m+1:m+nrhs,m+1:n) */

	err = 0.;
	i__1 = *n;
	for (j = *m + 1; j <= i__1; ++j) {
	    i__2 = ldwork;
	    for (i__ = j; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__1 = err, d__2 = z_abs(&work[i__ + (j - 1) * ldwork]);
		err = max(d__1,d__2);
/* L50: */
	    }
/* L60: */
	}

    }

/* Computing MAX */
    i__1 = max(*m,*n);
    ret_val = err / ((doublereal) max(i__1,*nrhs) * dlamch_("Epsilon"));

    return ret_val;

/*     End of ZQRT14 */

} /* zqrt14_ */
