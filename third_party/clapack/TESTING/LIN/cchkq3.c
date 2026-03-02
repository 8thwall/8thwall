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

static complex c_b1 = {0.f,0.f};
static real c_b15 = 1.f;
static integer c__1 = 1;
static integer c__3 = 3;

/* Subroutine */ int cchkq3_(logical *dotype, integer *nm, integer *mval, 
	integer *nn, integer *nval, integer *nnb, integer *nbval, integer *
	nxval, real *thresh, complex *a, complex *copya, real *s, real *copys, 
	 complex *tau, complex *work, real *rwork, integer *iwork, integer *
	nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };

    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002 M =\002,i5,\002, N =\002,i5,\002, "
	    "NB =\002,i4,\002, type \002,i2,\002, test \002,i2,\002, ratio "
	    "=\002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, k, m, n, nb, im, in, lw, nx, lda, inb;
    real eps;
    integer mode, info;
    char path[3];
    integer ilow, nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer ihigh, nfail, iseed[4], imode;
    extern doublereal cqpt01_(integer *, integer *, integer *, complex *, 
	    complex *, integer *, complex *, integer *, complex *, integer *),
	     cqrt11_(integer *, integer *, complex *, integer *, complex *, 
	    complex *, integer *), cqrt12_(integer *, integer *, complex *, 
	    integer *, real *, complex *, integer *, real *);
    integer mnmin;
    extern /* Subroutine */ int icopy_(integer *, integer *, integer *, 
	    integer *, integer *);
    integer istep, nerrs, lwork;
    extern /* Subroutine */ int cgeqp3_(integer *, integer *, complex *, 
	    integer *, integer *, complex *, complex *, integer *, real *, 
	    integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *), alasum_(char *, integer *, integer *, integer *, integer 
	    *), clatms_(integer *, integer *, char *, integer *, char 
	    *, real *, integer *, real *, real *, integer *, integer *, char *
, complex *, integer *, complex *, integer *), slaord_(char *, integer *, real *, integer *), 
	    xlaenv_(integer *, integer *);
    real result[3];

    /* Fortran I/O blocks */
    static cilist io___28 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CCHKQ3 tests CGEQP3. */

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

/*  NNB     (input) INTEGER */
/*          The number of values of NB and NX contained in the */
/*          vectors NBVAL and NXVAL.  The blocking parameters are used */
/*          in pairs (NB,NX). */

/*  NBVAL   (input) INTEGER array, dimension (NNB) */
/*          The values of the blocksize NB. */

/*  NXVAL   (input) INTEGER array, dimension (NNB) */
/*          The values of the crossover point NX. */

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

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
/*                      (max(M*max(M,N) + 4*min(M,N) + max(M,N))) */

/*  RWORK   (workspace) REAL array, dimension (4*NMAX) */

/*  IWORK   (workspace) INTEGER array, dimension (2*NMAX) */

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
    --iwork;
    --rwork;
    --work;
    --tau;
    --copys;
    --s;
    --copya;
    --a;
    --nxval;
    --nbval;
    --nval;
    --mval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "Q3", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }
    eps = slamch_("Epsilon");
    infoc_1.infot = 0;

    i__1 = *nm;
    for (im = 1; im <= i__1; ++im) {

/*        Do for each value of M in MVAL. */

	m = mval[im];
	lda = max(1,m);

	i__2 = *nn;
	for (in = 1; in <= i__2; ++in) {

/*           Do for each value of N in NVAL. */

	    n = nval[in];
	    mnmin = min(m,n);
/* Computing MAX */
	    i__3 = 1, i__4 = m * max(m,n) + (mnmin << 2) + max(m,n);
	    lwork = max(i__3,i__4);

	    for (imode = 1; imode <= 6; ++imode) {
		if (! dotype[imode]) {
		    goto L70;
		}

/*              Do for each type of matrix */
/*                 1:  zero matrix */
/*                 2:  one small singular value */
/*                 3:  geometric distribution of singular values */
/*                 4:  first n/2 columns fixed */
/*                 5:  last n/2 columns fixed */
/*                 6:  every second column fixed */

		mode = imode;
		if (imode > 3) {
		    mode = 1;
		}

/*              Generate test matrix of size m by n using */
/*              singular value distribution indicated by `mode'. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    iwork[i__] = 0;
/* L20: */
		}
		if (imode == 1) {
		    claset_("Full", &m, &n, &c_b1, &c_b1, &copya[1], &lda);
		    i__3 = mnmin;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			copys[i__] = 0.f;
/* L30: */
		    }
		} else {
		    r__1 = 1.f / eps;
		    clatms_(&m, &n, "Uniform", iseed, "Nonsymm", &copys[1], &
			    mode, &r__1, &c_b15, &m, &n, "No packing", &copya[
			    1], &lda, &work[1], &info);
		    if (imode >= 4) {
			if (imode == 4) {
			    ilow = 1;
			    istep = 1;
/* Computing MAX */
			    i__3 = 1, i__4 = n / 2;
			    ihigh = max(i__3,i__4);
			} else if (imode == 5) {
/* Computing MAX */
			    i__3 = 1, i__4 = n / 2;
			    ilow = max(i__3,i__4);
			    istep = 1;
			    ihigh = n;
			} else if (imode == 6) {
			    ilow = 1;
			    istep = 2;
			    ihigh = n;
			}
			i__3 = ihigh;
			i__4 = istep;
			for (i__ = ilow; i__4 < 0 ? i__ >= i__3 : i__ <= i__3;
				 i__ += i__4) {
			    iwork[i__] = 1;
/* L40: */
			}
		    }
		    slaord_("Decreasing", &mnmin, &copys[1], &c__1);
		}

		i__4 = *nnb;
		for (inb = 1; inb <= i__4; ++inb) {

/*                 Do for each pair of values (NB,NX) in NBVAL and NXVAL. */

		    nb = nbval[inb];
		    xlaenv_(&c__1, &nb);
		    nx = nxval[inb];
		    xlaenv_(&c__3, &nx);

/*                 Save A and its singular values and a copy of */
/*                 vector IWORK. */

		    clacpy_("All", &m, &n, &copya[1], &lda, &a[1], &lda);
		    icopy_(&n, &iwork[1], &c__1, &iwork[n + 1], &c__1);

/*                 Workspace needed. */

		    lw = nb * (n + 1);

		    s_copy(srnamc_1.srnamt, "CGEQP3", (ftnlen)6, (ftnlen)6);
		    cgeqp3_(&m, &n, &a[1], &lda, &iwork[n + 1], &tau[1], &
			    work[1], &lw, &rwork[1], &info);

/*                 Compute norm(svd(a) - svd(r)) */

		    result[0] = cqrt12_(&m, &n, &a[1], &lda, &copys[1], &work[
			    1], &lwork, &rwork[1]);

/*                 Compute norm( A*P - Q*R ) */

		    result[1] = cqpt01_(&m, &n, &mnmin, &copya[1], &a[1], &
			    lda, &tau[1], &iwork[n + 1], &work[1], &lwork);

/*                 Compute Q'*Q */

		    result[2] = cqrt11_(&m, &mnmin, &a[1], &lda, &tau[1], &
			    work[1], &lwork);

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    for (k = 1; k <= 3; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___28.ciunit = *nout;
			    s_wsfe(&io___28);
			    do_fio(&c__1, "CGEQP3", (ftnlen)6);
			    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&imode, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				    sizeof(real));
			    e_wsfe();
			    ++nfail;
			}
/* L50: */
		    }
		    nrun += 3;

/* L60: */
		}
L70:
		;
	    }
/* L80: */
	}
/* L90: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);


/*     End of CCHKQ3 */

    return 0;
} /* cchkq3_ */
