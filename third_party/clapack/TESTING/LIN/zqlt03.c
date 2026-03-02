#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublecomplex c_b1 = {-1e10,-1e10};
static integer c__2 = 2;
static doublecomplex c_b21 = {-1.,0.};
static doublecomplex c_b22 = {1.,0.};

/* Subroutine */ int zqlt03_(integer *m, integer *n, integer *k, 
	doublecomplex *af, doublecomplex *c__, doublecomplex *cc, 
	doublecomplex *q, integer *lda, doublecomplex *tau, doublecomplex *
	work, integer *lwork, doublereal *rwork, doublereal *result)
{
    /* Initialized data */

    static integer iseed[4] = { 1988,1989,1990,1991 };

    /* System generated locals */
    integer af_dim1, af_offset, c_dim1, c_offset, cc_dim1, cc_offset, q_dim1, 
	    q_offset, i__1, i__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    integer j, mc, nc;
    doublereal eps;
    char side[1];
    integer info, iside;
    extern logical lsame_(char *, char *);
    doublereal resid;
    integer minmn;
    doublereal cnorm;
    extern /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);
    char trans[1];
    extern doublereal dlamch_(char *), zlange_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *);
    integer itrans;
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), 
	    zlaset_(char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, integer *), zlarnv_(
	    integer *, integer *, integer *, doublecomplex *), zungql_(
	    integer *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, integer *), zunmql_(
	    char *, char *, integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZQLT03 tests ZUNMQL, which computes Q*C, Q'*C, C*Q or C*Q'. */

/*  ZQLT03 compares the results of a call to ZUNMQL with the results of */
/*  forming Q explicitly by a call to ZUNGQL and then performing matrix */
/*  multiplication by a call to ZGEMM. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The order of the orthogonal matrix Q.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of rows or columns of the matrix C; C is m-by-n if */
/*          Q is applied from the left, or n-by-m if Q is applied from */
/*          the right.  N >= 0. */

/*  K       (input) INTEGER */
/*          The number of elementary reflectors whose product defines the */
/*          orthogonal matrix Q.  M >= K >= 0. */

/*  AF      (input) COMPLEX*16 array, dimension (LDA,N) */
/*          Details of the QL factorization of an m-by-n matrix, as */
/*          returned by ZGEQLF. See CGEQLF for further details. */

/*  C       (workspace) COMPLEX*16 array, dimension (LDA,N) */

/*  CC      (workspace) COMPLEX*16 array, dimension (LDA,N) */

/*  Q       (workspace) COMPLEX*16 array, dimension (LDA,M) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays AF, C, CC, and Q. */

/*  TAU     (input) COMPLEX*16 array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the QL factorization in AF. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of WORK.  LWORK must be at least M, and should be */
/*          M*NB, where NB is the blocksize for this environment. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (M) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (4) */
/*          The test ratios compare two techniques for multiplying a */
/*          random matrix C by an m-by-m orthogonal matrix Q. */
/*          RESULT(1) = norm( Q*C - Q*C )  / ( M * norm(C) * EPS ) */
/*          RESULT(2) = norm( C*Q - C*Q )  / ( M * norm(C) * EPS ) */
/*          RESULT(3) = norm( Q'*C - Q'*C )/ ( M * norm(C) * EPS ) */
/*          RESULT(4) = norm( C*Q' - C*Q' )/ ( M * norm(C) * EPS ) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    q_dim1 = *lda;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    cc_dim1 = *lda;
    cc_offset = 1 + cc_dim1;
    cc -= cc_offset;
    c_dim1 = *lda;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    af_dim1 = *lda;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    --tau;
    --work;
    --rwork;
    --result;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    eps = dlamch_("Epsilon");
    minmn = min(*m,*n);

/*     Quick return if possible */

    if (minmn == 0) {
	result[1] = 0.;
	result[2] = 0.;
	result[3] = 0.;
	result[4] = 0.;
	return 0;
    }

/*     Copy the last k columns of the factorization to the array Q */

    zlaset_("Full", m, m, &c_b1, &c_b1, &q[q_offset], lda);
    if (*k > 0 && *m > *k) {
	i__1 = *m - *k;
	zlacpy_("Full", &i__1, k, &af[(*n - *k + 1) * af_dim1 + 1], lda, &q[(*
		m - *k + 1) * q_dim1 + 1], lda);
    }
    if (*k > 1) {
	i__1 = *k - 1;
	i__2 = *k - 1;
	zlacpy_("Upper", &i__1, &i__2, &af[*m - *k + 1 + (*n - *k + 2) * 
		af_dim1], lda, &q[*m - *k + 1 + (*m - *k + 2) * q_dim1], lda);
    }

/*     Generate the m-by-m matrix Q */

    s_copy(srnamc_1.srnamt, "ZUNGQL", (ftnlen)6, (ftnlen)6);
    zungql_(m, m, k, &q[q_offset], lda, &tau[minmn - *k + 1], &work[1], lwork, 
	     &info);

    for (iside = 1; iside <= 2; ++iside) {
	if (iside == 1) {
	    *(unsigned char *)side = 'L';
	    mc = *m;
	    nc = *n;
	} else {
	    *(unsigned char *)side = 'R';
	    mc = *n;
	    nc = *m;
	}

/*        Generate MC by NC matrix C */

	i__1 = nc;
	for (j = 1; j <= i__1; ++j) {
	    zlarnv_(&c__2, iseed, &mc, &c__[j * c_dim1 + 1]);
/* L10: */
	}
	cnorm = zlange_("1", &mc, &nc, &c__[c_offset], lda, &rwork[1]);
	if (cnorm == 0.) {
	    cnorm = 1.;
	}

	for (itrans = 1; itrans <= 2; ++itrans) {
	    if (itrans == 1) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'C';
	    }

/*           Copy C */

	    zlacpy_("Full", &mc, &nc, &c__[c_offset], lda, &cc[cc_offset], 
		    lda);

/*           Apply Q or Q' to C */

	    s_copy(srnamc_1.srnamt, "ZUNMQL", (ftnlen)6, (ftnlen)6);
	    if (*k > 0) {
		zunmql_(side, trans, &mc, &nc, k, &af[(*n - *k + 1) * af_dim1 
			+ 1], lda, &tau[minmn - *k + 1], &cc[cc_offset], lda, 
			&work[1], lwork, &info);
	    }

/*           Form explicit product and subtract */

	    if (lsame_(side, "L")) {
		zgemm_(trans, "No transpose", &mc, &nc, &mc, &c_b21, &q[
			q_offset], lda, &c__[c_offset], lda, &c_b22, &cc[
			cc_offset], lda);
	    } else {
		zgemm_("No transpose", trans, &mc, &nc, &nc, &c_b21, &c__[
			c_offset], lda, &q[q_offset], lda, &c_b22, &cc[
			cc_offset], lda);
	    }

/*           Compute error in the difference */

	    resid = zlange_("1", &mc, &nc, &cc[cc_offset], lda, &rwork[1]);
	    result[(iside - 1 << 1) + itrans] = resid / ((doublereal) max(1,*
		    m) * cnorm * eps);

/* L20: */
	}
/* L30: */
    }

    return 0;

/*     End of ZQLT03 */

} /* zqlt03_ */
