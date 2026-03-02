#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static real c_b4 = -1e10f;
static integer c__2 = 2;
static real c_b21 = -1.f;
static real c_b22 = 1.f;

/* Subroutine */ int slqt03_(integer *m, integer *n, integer *k, real *af, 
	real *c__, real *cc, real *q, integer *lda, real *tau, real *work, 
	integer *lwork, real *rwork, real *result)
{
    /* Initialized data */

    static integer iseed[4] = { 1988,1989,1990,1991 };

    /* System generated locals */
    integer af_dim1, af_offset, c_dim1, c_offset, cc_dim1, cc_offset, q_dim1, 
	    q_offset, i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    integer j, mc, nc;
    real eps;
    char side[1];
    integer info, iside;
    extern logical lsame_(char *, char *);
    real resid;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real cnorm;
    char trans[1];
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *);
    integer itrans;
    extern /* Subroutine */ int slarnv_(integer *, integer *, integer *, real 
	    *), sorglq_(integer *, integer *, integer *, real *, integer *, 
	    real *, real *, integer *, integer *), sormlq_(char *, char *, 
	    integer *, integer *, integer *, real *, integer *, real *, real *
, integer *, real *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLQT03 tests SORMLQ, which computes Q*C, Q'*C, C*Q or C*Q'. */

/*  SLQT03 compares the results of a call to SORMLQ with the results of */
/*  forming Q explicitly by a call to SORGLQ and then performing matrix */
/*  multiplication by a call to SGEMM. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows or columns of the matrix C; C is n-by-m if */
/*          Q is applied from the left, or m-by-n if Q is applied from */
/*          the right.  M >= 0. */

/*  N       (input) INTEGER */
/*          The order of the orthogonal matrix Q.  N >= 0. */

/*  K       (input) INTEGER */
/*          The number of elementary reflectors whose product defines the */
/*          orthogonal matrix Q.  N >= K >= 0. */

/*  AF      (input) REAL array, dimension (LDA,N) */
/*          Details of the LQ factorization of an m-by-n matrix, as */
/*          returned by SGELQF. See SGELQF for further details. */

/*  C       (workspace) REAL array, dimension (LDA,N) */

/*  CC      (workspace) REAL array, dimension (LDA,N) */

/*  Q       (workspace) REAL array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays AF, C, CC, and Q. */

/*  TAU     (input) REAL array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the LQ factorization in AF. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The length of WORK.  LWORK must be at least M, and should be */
/*          M*NB, where NB is the blocksize for this environment. */

/*  RWORK   (workspace) REAL array, dimension (M) */

/*  RESULT  (output) REAL array, dimension (4) */
/*          The test ratios compare two techniques for multiplying a */
/*          random matrix C by an n-by-n orthogonal matrix Q. */
/*          RESULT(1) = norm( Q*C - Q*C )  / ( N * norm(C) * EPS ) */
/*          RESULT(2) = norm( C*Q - C*Q )  / ( N * norm(C) * EPS ) */
/*          RESULT(3) = norm( Q'*C - Q'*C )/ ( N * norm(C) * EPS ) */
/*          RESULT(4) = norm( C*Q' - C*Q' )/ ( N * norm(C) * EPS ) */

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

    eps = slamch_("Epsilon");

/*     Copy the first k rows of the factorization to the array Q */

    slaset_("Full", n, n, &c_b4, &c_b4, &q[q_offset], lda);
    i__1 = *n - 1;
    slacpy_("Upper", k, &i__1, &af[(af_dim1 << 1) + 1], lda, &q[(q_dim1 << 1) 
	    + 1], lda);

/*     Generate the n-by-n matrix Q */

    s_copy(srnamc_1.srnamt, "SORGLQ", (ftnlen)6, (ftnlen)6);
    sorglq_(n, n, k, &q[q_offset], lda, &tau[1], &work[1], lwork, &info);

    for (iside = 1; iside <= 2; ++iside) {
	if (iside == 1) {
	    *(unsigned char *)side = 'L';
	    mc = *n;
	    nc = *m;
	} else {
	    *(unsigned char *)side = 'R';
	    mc = *m;
	    nc = *n;
	}

/*        Generate MC by NC matrix C */

	i__1 = nc;
	for (j = 1; j <= i__1; ++j) {
	    slarnv_(&c__2, iseed, &mc, &c__[j * c_dim1 + 1]);
/* L10: */
	}
	cnorm = slange_("1", &mc, &nc, &c__[c_offset], lda, &rwork[1]);
	if (cnorm == 0.f) {
	    cnorm = 1.f;
	}

	for (itrans = 1; itrans <= 2; ++itrans) {
	    if (itrans == 1) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'T';
	    }

/*           Copy C */

	    slacpy_("Full", &mc, &nc, &c__[c_offset], lda, &cc[cc_offset], 
		    lda);

/*           Apply Q or Q' to C */

	    s_copy(srnamc_1.srnamt, "SORMLQ", (ftnlen)6, (ftnlen)6);
	    sormlq_(side, trans, &mc, &nc, k, &af[af_offset], lda, &tau[1], &
		    cc[cc_offset], lda, &work[1], lwork, &info);

/*           Form explicit product and subtract */

	    if (lsame_(side, "L")) {
		sgemm_(trans, "No transpose", &mc, &nc, &mc, &c_b21, &q[
			q_offset], lda, &c__[c_offset], lda, &c_b22, &cc[
			cc_offset], lda);
	    } else {
		sgemm_("No transpose", trans, &mc, &nc, &nc, &c_b21, &c__[
			c_offset], lda, &q[q_offset], lda, &c_b22, &cc[
			cc_offset], lda);
	    }

/*           Compute error in the difference */

	    resid = slange_("1", &mc, &nc, &cc[cc_offset], lda, &rwork[1]);
	    result[(iside - 1 << 1) + itrans] = resid / ((real) max(1,*n) * 
		    cnorm * eps);

/* L20: */
	}
/* L30: */
    }

    return 0;

/*     End of SLQT03 */

} /* slqt03_ */
