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

static real c_b10 = 0.f;
static real c_b15 = 1.f;
static integer c__1 = 1;

/* Subroutine */ int schktz_(logical *dotype, integer *nm, integer *mval, 
	integer *nn, integer *nval, real *thresh, logical *tsterr, real *a, 
	real *copya, real *s, real *copys, real *tau, real *work, integer *
	nout)
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
    integer nfail, iseed[4], imode, mnmin, nerrs;
    extern doublereal sqrt12_(integer *, integer *, real *, integer *, real *, 
	     real *, integer *);
    integer lwork;
    extern doublereal srzt01_(integer *, integer *, real *, real *, integer *, 
	     real *, real *, integer *), srzt02_(integer *, integer *, real *, 
	     integer *, real *, real *, integer *), stzt01_(integer *, 
	    integer *, real *, real *, integer *, real *, real *, integer *), 
	    stzt02_(integer *, integer *, real *, integer *, real *, real *, 
	    integer *);
    extern /* Subroutine */ int sgeqr2_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int alasum_(char *, integer *, integer *, integer 
	    *, integer *), slaord_(char *, integer *, real *, integer 
	    *), slacpy_(char *, integer *, integer *, real *, integer 
	    *, real *, integer *), slaset_(char *, integer *, integer 
	    *, real *, real *, real *, integer *), slatms_(integer *, 
	    integer *, char *, integer *, char *, real *, integer *, real *, 
	    real *, integer *, integer *, char *, real *, integer *, real *, 
	    integer *);
    real result[6];
    extern /* Subroutine */ int serrtz_(char *, integer *), stzrqf_(
	    integer *, integer *, real *, integer *, real *, integer *), 
	    stzrzf_(integer *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___21 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SCHKTZ tests STZRQF and STZRZF. */

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

/*  A       (workspace) REAL array, dimension (MMAX*NMAX) */
/*          where MMAX is the maximum value of M in MVAL and NMAX is the */
/*          maximum value of N in NVAL. */

/*  COPYA   (workspace) REAL array, dimension (MMAX*NMAX) */

/*  S       (workspace) REAL array, dimension */
/*                      (min(MMAX,NMAX)) */

/*  COPYS   (workspace) REAL array, dimension */
/*                      (min(MMAX,NMAX)) */

/*  TAU     (workspace) REAL array, dimension (MMAX) */

/*  WORK    (workspace) REAL array, dimension */
/*                      (MMAX*NMAX + 4*NMAX + MMAX) */

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

    s_copy(path, "Single precision", (ftnlen)1, (ftnlen)16);
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
	serrtz_(path, nout);
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
	    i__3 = 1, i__4 = n * n + (m << 2) + n, i__3 = max(i__3,i__4), 
		    i__4 = m * n + (mnmin << 1) + (n << 2);
	    lwork = max(i__3,i__4);

	    if (m <= n) {
		for (imode = 1; imode <= 3; ++imode) {
		    if (! dotype[imode]) {
			goto L50;
		    }

/*                 Do for each type of singular value distribution. */
/*                    0:  zero matrix */
/*                    1:  one small singular value */
/*                    2:  exponential distribution */

		    mode = imode - 1;

/*                 Test STZRQF */

/*                 Generate test matrix of size m by n using */
/*                 singular value distribution indicated by `mode'. */

		    if (mode == 0) {
			slaset_("Full", &m, &n, &c_b10, &c_b10, &a[1], &lda);
			i__3 = mnmin;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    copys[i__] = 0.f;
/* L20: */
			}
		    } else {
			r__1 = 1.f / eps;
			slatms_(&m, &n, "Uniform", iseed, "Nonsymmetric", &
				copys[1], &imode, &r__1, &c_b15, &m, &n, 
				"No packing", &a[1], &lda, &work[1], &info);
			sgeqr2_(&m, &n, &a[1], &lda, &work[1], &work[mnmin + 
				1], &info);
			i__3 = m - 1;
			slaset_("Lower", &i__3, &n, &c_b10, &c_b10, &a[2], &
				lda);
			slaord_("Decreasing", &mnmin, &copys[1], &c__1);
		    }

/*                 Save A and its singular values */

		    slacpy_("All", &m, &n, &a[1], &lda, &copya[1], &lda);

/*                 Call STZRQF to reduce the upper trapezoidal matrix to */
/*                 upper triangular form. */

		    s_copy(srnamc_1.srnamt, "STZRQF", (ftnlen)6, (ftnlen)6);
		    stzrqf_(&m, &n, &a[1], &lda, &tau[1], &info);

/*                 Compute norm(svd(a) - svd(r)) */

		    result[0] = sqrt12_(&m, &m, &a[1], &lda, &copys[1], &work[
			    1], &lwork);

/*                 Compute norm( A - R*Q ) */

		    result[1] = stzt01_(&m, &n, &copya[1], &a[1], &lda, &tau[
			    1], &work[1], &lwork);

/*                 Compute norm(Q'*Q - I). */

		    result[2] = stzt02_(&m, &n, &a[1], &lda, &tau[1], &work[1]
, &lwork);

/*                 Test STZRZF */

/*                 Generate test matrix of size m by n using */
/*                 singular value distribution indicated by `mode'. */

		    if (mode == 0) {
			slaset_("Full", &m, &n, &c_b10, &c_b10, &a[1], &lda);
			i__3 = mnmin;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    copys[i__] = 0.f;
/* L30: */
			}
		    } else {
			r__1 = 1.f / eps;
			slatms_(&m, &n, "Uniform", iseed, "Nonsymmetric", &
				copys[1], &imode, &r__1, &c_b15, &m, &n, 
				"No packing", &a[1], &lda, &work[1], &info);
			sgeqr2_(&m, &n, &a[1], &lda, &work[1], &work[mnmin + 
				1], &info);
			i__3 = m - 1;
			slaset_("Lower", &i__3, &n, &c_b10, &c_b10, &a[2], &
				lda);
			slaord_("Decreasing", &mnmin, &copys[1], &c__1);
		    }

/*                 Save A and its singular values */

		    slacpy_("All", &m, &n, &a[1], &lda, &copya[1], &lda);

/*                 Call STZRZF to reduce the upper trapezoidal matrix to */
/*                 upper triangular form. */

		    s_copy(srnamc_1.srnamt, "STZRZF", (ftnlen)6, (ftnlen)6);
		    stzrzf_(&m, &n, &a[1], &lda, &tau[1], &work[1], &lwork, &
			    info);

/*                 Compute norm(svd(a) - svd(r)) */

		    result[3] = sqrt12_(&m, &m, &a[1], &lda, &copys[1], &work[
			    1], &lwork);

/*                 Compute norm( A - R*Q ) */

		    result[4] = srzt01_(&m, &n, &copya[1], &a[1], &lda, &tau[
			    1], &work[1], &lwork);

/*                 Compute norm(Q'*Q - I). */

		    result[5] = srzt02_(&m, &n, &a[1], &lda, &tau[1], &work[1]
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
L50:
		    ;
		}
	    }
/* L60: */
	}
/* L70: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);


/*     End if SCHKTZ */

    return 0;
} /* schktz_ */
