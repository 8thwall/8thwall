#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer infot, iounit;
    logical ok, lerr;
} infoc_;

#define infoc_1 infoc_

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static complex c_b10 = {0.f,0.f};
static real c_b15 = 1.f;
static integer c__1 = 1;

/* Subroutine */ int cchktz_(logical *dotype, integer *nm, integer *mval, 
	integer *nn, integer *nval, real *thresh, logical *tsterr, complex *a, 
	 complex *copya, real *s, real *copys, complex *tau, complex *work, 
	real *rwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };

    /* Format strings */
    static char fmt_9999[] = "(\002 M =\002,i5,\002, N =\002,i5,\002, type"
	    " \002,i2,\002, test \002,i2,\002, ratio =\002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, k, m, n, im, in, lda;
    real eps;
    integer mode, info;
    char path[3];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer nfail, iseed[4], imode;
    extern doublereal cqrt12_(integer *, integer *, complex *, integer *, 
	    real *, complex *, integer *, real *);
    integer mnmin;
    extern doublereal crzt01_(integer *, integer *, complex *, complex *, 
	    integer *, complex *, complex *, integer *), crzt02_(integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *),
	     ctzt01_(integer *, integer *, complex *, complex *, integer *, 
	    complex *, complex *, integer *), ctzt02_(integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *);
    integer nerrs, lwork;
    extern /* Subroutine */ int cgeqr2_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *), alasum_(char *, integer *, integer *, integer *, integer 
	    *), clatms_(integer *, integer *, char *, integer *, char 
	    *, real *, integer *, real *, real *, integer *, integer *, char *
, complex *, integer *, complex *, integer *), slaord_(char *, integer *, real *, integer *), 
	    cerrtz_(char *, integer *), ctzrqf_(integer *, integer *, 
	    complex *, integer *, complex *, integer *);
    real result[6];
    extern /* Subroutine */ int ctzrzf_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___21 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CCHKTZ tests CTZRQF and CTZRZF. */

/*  Arguments */
/*  ========= */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          The matrix types to be used for testing.  Matrices of type j */
/*          (for 1 <= j <= NTYPES) are used for testing if DOTYPE(j) = */
/*          .TRUE.; if DOTYPE(j) = .FALSE., then type j is not used. */

/*  NM      (input) INTEGER */
/*          The number of values of M contained in the vector MVAL. */

/*  MVAL    (input) INTEGER array, dimension (NM) */
/*          The values of the matrix row dimension M. */

/*  NN      (input) INTEGER */
/*          The number of values of N contained in the vector NVAL. */

/*  NVAL    (input) INTEGER array, dimension (NN) */
/*          The values of the matrix column dimension N. */

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  A       (workspace) COMPLEX array, dimension (MMAX*NMAX) */
/*          where MMAX is the maximum value of M in MVAL and NMAX is the */
/*          maximum value of N in NVAL. */

/*  COPYA   (workspace) COMPLEX array, dimension (MMAX*NMAX) */

/*  S       (workspace) REAL array, dimension */
/*                      (min(MMAX,NMAX)) */

/*  COPYS   (workspace) REAL array, dimension */
/*                      (min(MMAX,NMAX)) */

/*  TAU     (workspace) COMPLEX array, dimension (MMAX) */

/*  WORK    (workspace) COMPLEX array, dimension */
/*                      (MMAX*NMAX + 4*NMAX + MMAX) */

/*  RWORK   (workspace) REAL array, dimension (2*NMAX) */

/*  NOUT    (input) INTEGER */
/*          The unit number for output. */

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
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    --rwork;
    --work;
    --tau;
    --copys;
    --s;
    --copya;
    --a;
    --nval;
    --mval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "TZ", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }
    eps = slamch_("Epsilon");

/*     Test the error exits */

    if (*tsterr) {
	cerrtz_(path, nout);
    }
    infoc_1.infot = 0;

    i__1 = *nm;
    for (im = 1; im <= i__1; ++im) {

/*        Do for each value of M in MVAL. */

	m = mval[im];
	lda = max(1,m);

	i__2 = *nn;
	for (in = 1; in <= i__2; ++in) {

/*           Do for each value of N in NVAL for which M .LE. N. */

	    n = nval[in];
	    mnmin = min(m,n);
/* Computing MAX */
	    i__3 = 1, i__4 = n * n + (m << 2) + n;
	    lwork = max(i__3,i__4);

	    if (m <= n) {
		for (imode = 1; imode <= 3; ++imode) {

/*                 Do for each type of singular value distribution. */
/*                    0:  zero matrix */
/*                    1:  one small singular value */
/*                    2:  exponential distribution */

		    mode = imode - 1;

/*                 Test CTZRQF */

/*                 Generate test matrix of size m by n using */
/*                 singular value distribution indicated by `mode'. */

		    if (mode == 0) {
			claset_("Full", &m, &n, &c_b10, &c_b10, &a[1], &lda);
			i__3 = mnmin;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    copys[i__] = 0.f;
/* L20: */
			}
		    } else {
			r__1 = 1.f / eps;
			clatms_(&m, &n, "Uniform", iseed, "Nonsymmetric", &
				copys[1], &imode, &r__1, &c_b15, &m, &n, 
				"No packing", &a[1], &lda, &work[1], &info);
			cgeqr2_(&m, &n, &a[1], &lda, &work[1], &work[mnmin + 
				1], &info);
			i__3 = m - 1;
			claset_("Lower", &i__3, &n, &c_b10, &c_b10, &a[2], &
				lda);
			slaord_("Decreasing", &mnmin, &copys[1], &c__1);
		    }

/*                 Save A and its singular values */

		    clacpy_("All", &m, &n, &a[1], &lda, &copya[1], &lda);

/*                 Call CTZRQF to reduce the upper trapezoidal matrix to */
/*                 upper triangular form. */

		    s_copy(srnamc_1.srnamt, "CTZRQF", (ftnlen)6, (ftnlen)6);
		    ctzrqf_(&m, &n, &a[1], &lda, &tau[1], &info);

/*                 Compute norm(svd(a) - svd(r)) */

		    result[0] = cqrt12_(&m, &m, &a[1], &lda, &copys[1], &work[
			    1], &lwork, &rwork[1]);

/*                 Compute norm( A - R*Q ) */

		    result[1] = ctzt01_(&m, &n, &copya[1], &a[1], &lda, &tau[
			    1], &work[1], &lwork);

/*                 Compute norm(Q'*Q - I). */

		    result[2] = ctzt02_(&m, &n, &a[1], &lda, &tau[1], &work[1]
, &lwork);

/*                 Test CTZRZF */

/*                 Generate test matrix of size m by n using */
/*                 singular value distribution indicated by `mode'. */

		    if (mode == 0) {
			claset_("Full", &m, &n, &c_b10, &c_b10, &a[1], &lda);
			i__3 = mnmin;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    copys[i__] = 0.f;
/* L30: */
			}
		    } else {
			r__1 = 1.f / eps;
			clatms_(&m, &n, "Uniform", iseed, "Nonsymmetric", &
				copys[1], &imode, &r__1, &c_b15, &m, &n, 
				"No packing", &a[1], &lda, &work[1], &info);
			cgeqr2_(&m, &n, &a[1], &lda, &work[1], &work[mnmin + 
				1], &info);
			i__3 = m - 1;
			claset_("Lower", &i__3, &n, &c_b10, &c_b10, &a[2], &
				lda);
			slaord_("Decreasing", &mnmin, &copys[1], &c__1);
		    }

/*                 Save A and its singular values */

		    clacpy_("All", &m, &n, &a[1], &lda, &copya[1], &lda);

/*                 Call CTZRZF to reduce the upper trapezoidal matrix to */
/*                 upper triangular form. */

		    s_copy(srnamc_1.srnamt, "CTZRZF", (ftnlen)6, (ftnlen)6);
		    ctzrzf_(&m, &n, &a[1], &lda, &tau[1], &work[1], &lwork, &
			    info);

/*                 Compute norm(svd(a) - svd(r)) */

		    result[3] = cqrt12_(&m, &m, &a[1], &lda, &copys[1], &work[
			    1], &lwork, &rwork[1]);

/*                 Compute norm( A - R*Q ) */

		    result[4] = crzt01_(&m, &n, &copya[1], &a[1], &lda, &tau[
			    1], &work[1], &lwork);

/*                 Compute norm(Q'*Q - I). */

		    result[5] = crzt02_(&m, &n, &a[1], &lda, &tau[1], &work[1]
, &lwork);

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    for (k = 1; k <= 6; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___21.ciunit = *nout;
			    s_wsfe(&io___21);
			    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&imode, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				    sizeof(real));
			    e_wsfe();
			    ++nfail;
			}
/* L40: */
		    }
		    nrun += 6;
/* L50: */
		}
	    }
/* L60: */
	}
/* L70: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);


/*     End if CCHKTZ */

    return 0;
} /* cchktz_ */
