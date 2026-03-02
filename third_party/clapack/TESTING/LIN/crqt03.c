#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static complex c_b1 = {-1e10f,-1e10f};
static integer c__2 = 2;
static complex c_b21 = {-1.f,0.f};
static complex c_b22 = {1.f,0.f};

/* Subroutine */ int crqt03_(integer *m, integer *n, integer *k, complex *af, 
	complex *c__, complex *cc, complex *q, integer *lda, complex *tau, 
	complex *work, integer *lwork, real *rwork, real *result)
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
    real eps;
    char side[1];
    integer info;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    integer iside;
    extern logical lsame_(char *, char *);
    real resid;
    integer minmn;
    real cnorm;
    char trans[1];
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *), clarnv_(integer *, integer *, integer *, complex *), 
	    cungrq_(integer *, integer *, integer *, complex *, integer *, 
	    complex *, complex *, integer *, integer *);
    integer itrans;
    extern /* Subroutine */ int cunmrq_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CRQT03 tests CUNMRQ, which computes Q*C, Q'*C, C*Q or C*Q'. */

/*  CRQT03 compares the results of a call to CUNMRQ with the results of */
/*  forming Q explicitly by a call to CUNGRQ and then performing matrix */
/*  multiplication by a call to CGEMM. */

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

/*  AF      (input) COMPLEX array, dimension (LDA,N) */
/*          Details of the RQ factorization of an m-by-n matrix, as */
/*          returned by CGERQF. See CGERQF for further details. */

/*  C       (workspace) COMPLEX array, dimension (LDA,N) */

/*  CC      (workspace) COMPLEX array, dimension (LDA,N) */

/*  Q       (workspace) COMPLEX array, dimension (LDA,N) */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays AF, C, CC, and Q. */

/*  TAU     (input) COMPLEX array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors corresponding */
/*          to the RQ factorization in AF. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

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
    minmn = min(*m,*n);

/*     Quick return if possible */

    if (minmn == 0) {
	result[1] = 0.f;
	result[2] = 0.f;
	result[3] = 0.f;
	result[4] = 0.f;
	return 0;
    }

/*     Copy the last k rows of the factorization to the array Q */

    claset_("Full", n, n, &c_b1, &c_b1, &q[q_offset], lda);
    if (*k > 0 && *n > *k) {
	i__1 = *n - *k;
	clacpy_("Full", k, &i__1, &af[*m - *k + 1 + af_dim1], lda, &q[*n - *k 
		+ 1 + q_dim1], lda);
    }
    if (*k > 1) {
	i__1 = *k - 1;
	i__2 = *k - 1;
	clacpy_("Lower", &i__1, &i__2, &af[*m - *k + 2 + (*n - *k + 1) * 
		af_dim1], lda, &q[*n - *k + 2 + (*n - *k + 1) * q_dim1], lda);
    }

/*     Generate the n-by-n matrix Q */

    s_copy(srnamc_1.srnamt, "CUNGRQ", (ftnlen)6, (ftnlen)6);
    cungrq_(n, n, k, &q[q_offset], lda, &tau[minmn - *k + 1], &work[1], lwork, 
	     &info);

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
	    clarnv_(&c__2, iseed, &mc, &c__[j * c_dim1 + 1]);
/* L10: */
	}
	cnorm = clange_("1", &mc, &nc, &c__[c_offset], lda, &rwork[1]);
	if (cnorm == 0.f) {
	    cnorm = 1.f;
	}

	for (itrans = 1; itrans <= 2; ++itrans) {
	    if (itrans == 1) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'C';
	    }

/*           Copy C */

	    clacpy_("Full", &mc, &nc, &c__[c_offset], lda, &cc[cc_offset], 
		    lda);

/*           Apply Q or Q' to C */

	    s_copy(srnamc_1.srnamt, "CUNMRQ", (ftnlen)6, (ftnlen)6);
	    if (*k > 0) {
		cunmrq_(side, trans, &mc, &nc, k, &af[*m - *k + 1 + af_dim1], 
			lda, &tau[minmn - *k + 1], &cc[cc_offset], lda, &work[
			1], lwork, &info);
	    }

/*           Form explicit product and subtract */

	    if (lsame_(side, "L")) {
		cgemm_(trans, "No transpose", &mc, &nc, &mc, &c_b21, &q[
			q_offset], lda, &c__[c_offset], lda, &c_b22, &cc[
			cc_offset], lda);
	    } else {
		cgemm_("No transpose", trans, &mc, &nc, &nc, &c_b21, &c__[
			c_offset], lda, &q[q_offset], lda, &c_b22, &cc[
			cc_offset], lda);
	    }

/*           Compute error in the difference */

	    resid = clange_("1", &mc, &nc, &cc[cc_offset], lda, &rwork[1]);
	    result[(iside - 1 << 1) + itrans] = resid / ((real) max(1,*n) * 
		    cnorm * eps);

/* L20: */
	}
/* L30: */
    }

    return 0;

/*     End of CRQT03 */

} /* crqt03_ */
