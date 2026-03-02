#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer infot, nunit;
    logical ok, lerr;
} infoc_;

#define infoc_1 infoc_

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static integer c__2 = 2;
static integer c__0 = 0;
static integer c_n1 = -1;
static integer c__1 = 1;
static integer c__8 = 8;

/* Subroutine */ int dchkpo_(logical *dotype, integer *nn, integer *nval, 
	integer *nnb, integer *nbval, integer *nns, integer *nsval, 
	doublereal *thresh, logical *tsterr, integer *nmax, doublereal *a, 
	doublereal *afac, doublereal *ainv, doublereal *b, doublereal *x, 
	doublereal *xact, doublereal *work, doublereal *rwork, integer *iwork, 
	 integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char uplos[1*2] = "U" "L";

    /* Format strings */
    static char fmt_9999[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002, "
	    "NB =\002,i4,\002, type \002,i2,\002, test \002,i2,\002, ratio "
	    "=\002,g12.5)";
    static char fmt_9998[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002, "
	    "NRHS=\002,i3,\002, type \002,i2,\002, test(\002,i2,\002) =\002,g"
	    "12.5)";
    static char fmt_9997[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002"
	    ",\002,10x,\002 type \002,i2,\002, test(\002,i2,\002) =\002,g12.5)"
	    ;

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, k, n, nb, in, kl, ku, lda, inb, ioff, mode, imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char uplo[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), dget04_(
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    doublereal rcond;
    extern /* Subroutine */ int dpot01_(char *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *);
    integer nimat;
    extern /* Subroutine */ int dpot02_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *), dpot03_(char *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *), dpot05_(char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *);
    doublereal anorm;
    integer iuplo, izero, nerrs;
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), alaerh_(char *, 
	    char *, integer *, integer *, char *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *);
    doublereal rcondc;
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlarhs_(char *, char *, char *, char *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *), alasum_(char *, 
	    integer *, integer *, integer *, integer *);
    doublereal cndnum;
    extern /* Subroutine */ int dlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublereal *, integer *, doublereal 
	    *, integer *), dpocon_(char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *);
    extern doublereal dlansy_(char *, char *, integer *, doublereal *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int derrpo_(char *, integer *), dporfs_(
	    char *, integer *, integer *, doublereal *, integer *, doublereal 
	    *, integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), dpotrf_(char *, integer *, doublereal *, integer *, 
	    integer *), xlaenv_(integer *, integer *), dpotri_(char *, 
	     integer *, doublereal *, integer *, integer *), dpotrs_(
	    char *, integer *, integer *, doublereal *, integer *, doublereal 
	    *, integer *, integer *);
    doublereal result[8];

    /* Fortran I/O blocks */
    static cilist io___33 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKPO tests DPOTRF, -TRI, -TRS, -RFS, and -CON */

/*  Arguments */
/*  ========= */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          The matrix types to be used for testing.  Matrices of type j */
/*          (for 1 <= j <= NTYPES) are used for testing if DOTYPE(j) = */
/*          .TRUE.; if DOTYPE(j) = .FALSE., then type j is not used. */

/*  NN      (input) INTEGER */
/*          The number of values of N contained in the vector NVAL. */

/*  NVAL    (input) INTEGER array, dimension (NN) */
/*          The values of the matrix dimension N. */

/*  NNB     (input) INTEGER */
/*          The number of values of NB contained in the vector NBVAL. */

/*  NBVAL   (input) INTEGER array, dimension (NBVAL) */
/*          The values of the blocksize NB. */

/*  NNS     (input) INTEGER */
/*          The number of values of NRHS contained in the vector NSVAL. */

/*  NSVAL   (input) INTEGER array, dimension (NNS) */
/*          The values of the number of right hand sides NRHS. */

/*  THRESH  (input) DOUBLE PRECISION */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  NMAX    (input) INTEGER */
/*          The maximum value permitted for N, used in dimensioning the */
/*          work arrays. */

/*  A       (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX) */

/*  AFAC    (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX) */

/*  AINV    (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX) */

/*  B       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension */
/*                      (max(NMAX,2*NSMAX)) */

/*  IWORK   (workspace) INTEGER array, dimension (NMAX) */

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
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    --iwork;
    --rwork;
    --work;
    --xact;
    --x;
    --b;
    --ainv;
    --afac;
    --a;
    --nsval;
    --nbval;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "PO", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	derrpo_(path, nout);
    }
    infoc_1.infot = 0;
    xlaenv_(&c__2, &c__2);

/*     Do for each value of N in NVAL */

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {
	n = nval[in];
	lda = max(n,1);
	*(unsigned char *)xtype = 'N';
	nimat = 9;
	if (n <= 0) {
	    nimat = 1;
	}

	izero = 0;
	i__2 = nimat;
	for (imat = 1; imat <= i__2; ++imat) {

/*           Do the tests only if DOTYPE( IMAT ) is true. */

	    if (! dotype[imat]) {
		goto L110;
	    }

/*           Skip types 3, 4, or 5 if the matrix size is too small. */

	    zerot = imat >= 3 && imat <= 5;
	    if (zerot && n < imat - 2) {
		goto L110;
	    }

/*           Do first for UPLO = 'U', then for UPLO = 'L' */

	    for (iuplo = 1; iuplo <= 2; ++iuplo) {
		*(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 1];

/*              Set up parameters with DLATB4 and generate a test matrix */
/*              with DLATMS. */

		dlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, 
			&cndnum, dist);

		s_copy(srnamc_1.srnamt, "DLATMS", (ftnlen)6, (ftnlen)6);
		dlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &
			cndnum, &anorm, &kl, &ku, uplo, &a[1], &lda, &work[1], 
			 &info);

/*              Check error code from DLATMS. */

		if (info != 0) {
		    alaerh_(path, "DLATMS", &info, &c__0, uplo, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L100;
		}

/*              For types 3-5, zero one row and column of the matrix to */
/*              test that INFO is returned correctly. */

		if (zerot) {
		    if (imat == 3) {
			izero = 1;
		    } else if (imat == 4) {
			izero = n;
		    } else {
			izero = n / 2 + 1;
		    }
		    ioff = (izero - 1) * lda;

/*                 Set row and column IZERO of A to 0. */

		    if (iuplo == 1) {
			i__3 = izero - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    a[ioff + i__] = 0.;
/* L20: */
			}
			ioff += izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    a[ioff] = 0.;
			    ioff += lda;
/* L30: */
			}
		    } else {
			ioff = izero;
			i__3 = izero - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    a[ioff] = 0.;
			    ioff += lda;
/* L40: */
			}
			ioff -= izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    a[ioff + i__] = 0.;
/* L50: */
			}
		    }
		} else {
		    izero = 0;
		}

/*              Do for each value of NB in NBVAL */

		i__3 = *nnb;
		for (inb = 1; inb <= i__3; ++inb) {
		    nb = nbval[inb];
		    xlaenv_(&c__1, &nb);

/*                 Compute the L*L' or U'*U factorization of the matrix. */

		    dlacpy_(uplo, &n, &n, &a[1], &lda, &afac[1], &lda);
		    s_copy(srnamc_1.srnamt, "DPOTRF", (ftnlen)6, (ftnlen)6);
		    dpotrf_(uplo, &n, &afac[1], &lda, &info);

/*                 Check error code from DPOTRF. */

		    if (info != izero) {
			alaerh_(path, "DPOTRF", &info, &izero, uplo, &n, &n, &
				c_n1, &c_n1, &nb, &imat, &nfail, &nerrs, nout);
			goto L90;
		    }

/*                 Skip the tests if INFO is not 0. */

		    if (info != 0) {
			goto L90;
		    }

/* +    TEST 1 */
/*                 Reconstruct matrix from factors and compute residual. */

		    dlacpy_(uplo, &n, &n, &afac[1], &lda, &ainv[1], &lda);
		    dpot01_(uplo, &n, &a[1], &lda, &ainv[1], &lda, &rwork[1], 
			    result);

/* +    TEST 2 */
/*                 Form the inverse and compute the residual. */

		    dlacpy_(uplo, &n, &n, &afac[1], &lda, &ainv[1], &lda);
		    s_copy(srnamc_1.srnamt, "DPOTRI", (ftnlen)6, (ftnlen)6);
		    dpotri_(uplo, &n, &ainv[1], &lda, &info);

/*                 Check error code from DPOTRI. */

		    if (info != 0) {
			alaerh_(path, "DPOTRI", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &c_n1, &imat, &nfail, &nerrs, 
				nout);
		    }

		    dpot03_(uplo, &n, &a[1], &lda, &ainv[1], &lda, &work[1], &
			    lda, &rwork[1], &rcondc, &result[1]);

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    for (k = 1; k <= 2; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___33.ciunit = *nout;
			    s_wsfe(&io___33);
			    do_fio(&c__1, uplo, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				    sizeof(doublereal));
			    e_wsfe();
			    ++nfail;
			}
/* L60: */
		    }
		    nrun += 2;

/*                 Skip the rest of the tests unless this is the first */
/*                 blocksize. */

		    if (inb != 1) {
			goto L90;
		    }

		    i__4 = *nns;
		    for (irhs = 1; irhs <= i__4; ++irhs) {
			nrhs = nsval[irhs];

/* +    TEST 3 */
/*                 Solve and compute residual for A * X = B . */

			s_copy(srnamc_1.srnamt, "DLARHS", (ftnlen)6, (ftnlen)
				6);
			dlarhs_(path, xtype, uplo, " ", &n, &n, &kl, &ku, &
				nrhs, &a[1], &lda, &xact[1], &lda, &b[1], &
				lda, iseed, &info);
			dlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);

			s_copy(srnamc_1.srnamt, "DPOTRS", (ftnlen)6, (ftnlen)
				6);
			dpotrs_(uplo, &n, &nrhs, &afac[1], &lda, &x[1], &lda, 
				&info);

/*                 Check error code from DPOTRS. */

			if (info != 0) {
			    alaerh_(path, "DPOTRS", &info, &c__0, uplo, &n, &
				    n, &c_n1, &c_n1, &nrhs, &imat, &nfail, &
				    nerrs, nout);
			}

			dlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &
				lda);
			dpot02_(uplo, &n, &nrhs, &a[1], &lda, &x[1], &lda, &
				work[1], &lda, &rwork[1], &result[2]);

/* +    TEST 4 */
/*                 Check solution from generated exact solution. */

			dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[3]);

/* +    TESTS 5, 6, and 7 */
/*                 Use iterative refinement to improve the solution. */

			s_copy(srnamc_1.srnamt, "DPORFS", (ftnlen)6, (ftnlen)
				6);
			dporfs_(uplo, &n, &nrhs, &a[1], &lda, &afac[1], &lda, 
				&b[1], &lda, &x[1], &lda, &rwork[1], &rwork[
				nrhs + 1], &work[1], &iwork[1], &info);

/*                 Check error code from DPORFS. */

			if (info != 0) {
			    alaerh_(path, "DPORFS", &info, &c__0, uplo, &n, &
				    n, &c_n1, &c_n1, &nrhs, &imat, &nfail, &
				    nerrs, nout);
			}

			dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[4]);
			dpot05_(uplo, &n, &nrhs, &a[1], &lda, &b[1], &lda, &x[
				1], &lda, &xact[1], &lda, &rwork[1], &rwork[
				nrhs + 1], &result[5]);

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			for (k = 3; k <= 7; ++k) {
			    if (result[k - 1] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    alahd_(nout, path);
				}
				io___36.ciunit = *nout;
				s_wsfe(&io___36);
				do_fio(&c__1, uplo, (ftnlen)1);
				do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
					sizeof(doublereal));
				e_wsfe();
				++nfail;
			    }
/* L70: */
			}
			nrun += 5;
/* L80: */
		    }

/* +    TEST 8 */
/*                 Get an estimate of RCOND = 1/CNDNUM. */

		    anorm = dlansy_("1", uplo, &n, &a[1], &lda, &rwork[1]);
		    s_copy(srnamc_1.srnamt, "DPOCON", (ftnlen)6, (ftnlen)6);
		    dpocon_(uplo, &n, &afac[1], &lda, &anorm, &rcond, &work[1]
, &iwork[1], &info);

/*                 Check error code from DPOCON. */

		    if (info != 0) {
			alaerh_(path, "DPOCON", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &c_n1, &imat, &nfail, &nerrs, 
				nout);
		    }

		    result[7] = dget06_(&rcond, &rcondc);

/*                 Print the test ratio if it is .GE. THRESH. */

		    if (result[7] >= *thresh) {
			if (nfail == 0 && nerrs == 0) {
			    alahd_(nout, path);
			}
			io___38.ciunit = *nout;
			s_wsfe(&io___38);
			do_fio(&c__1, uplo, (ftnlen)1);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[7], (ftnlen)sizeof(
				doublereal));
			e_wsfe();
			++nfail;
		    }
		    ++nrun;
L90:
		    ;
		}
L100:
		;
	    }
L110:
	    ;
	}
/* L120: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of DCHKPO */

} /* dchkpo_ */
