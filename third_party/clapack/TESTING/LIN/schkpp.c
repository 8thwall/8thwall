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

static integer c__0 = 0;
static integer c_n1 = -1;
static integer c__1 = 1;
static integer c__8 = 8;

/* Subroutine */ int schkpp_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, real *thresh, logical *tsterr, integer *
	nmax, real *a, real *afac, real *ainv, real *b, real *x, real *xact, 
	real *work, real *rwork, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char uplos[1*2] = "U" "L";
    static char packs[1*2] = "C" "R";

    /* Format strings */
    static char fmt_9999[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002, "
	    "type \002,i2,\002, test \002,i2,\002, ratio =\002,g12.5)";
    static char fmt_9998[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002, "
	    "NRHS=\002,i3,\002, type \002,i2,\002, test(\002,i2,\002) =\002,g"
	    "12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, k, n, in, kl, ku, lda, npp, ioff, mode, imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char uplo[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer nfail, iseed[4];
    real rcond;
    extern /* Subroutine */ int sget04_(integer *, integer *, real *, integer 
	    *, real *, integer *, real *, real *);
    integer nimat;
    extern doublereal sget06_(real *, real *);
    real anorm;
    extern /* Subroutine */ int sppt01_(char *, integer *, real *, real *, 
	    real *, real *);
    integer iuplo, izero, nerrs;
    extern /* Subroutine */ int sppt02_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *), 
	    scopy_(integer *, real *, integer *, real *, integer *), sppt03_(
	    char *, integer *, real *, real *, real *, integer *, real *, 
	    real *, real *), sppt05_(char *, integer *, integer *, 
	    real *, real *, integer *, real *, integer *, real *, integer *, 
	    real *, real *, real *);
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int slatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), alaerh_(char *, char *, integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    real rcondc;
    char packit[1];
    extern /* Subroutine */ int alasum_(char *, integer *, integer *, integer 
	    *, integer *);
    real cndnum;
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slarhs_(char *, char *, 
	    char *, char *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, integer *, integer *);
    extern doublereal slansp_(char *, char *, integer *, real *, real *);
    extern /* Subroutine */ int sppcon_(char *, integer *, real *, real *, 
	    real *, real *, integer *, integer *), slatms_(integer *, 
	    integer *, char *, integer *, char *, real *, integer *, real *, 
	    real *, integer *, integer *, char *, real *, integer *, real *, 
	    integer *), serrpo_(char *, integer *), spprfs_(char *, integer *, integer *, real *, real *, 
	    real *, integer *, real *, integer *, real *, real *, real *, 
	    integer *, integer *);
    real result[8];
    extern /* Subroutine */ int spptrf_(char *, integer *, real *, integer *), spptri_(char *, integer *, real *, integer *), 
	    spptrs_(char *, integer *, integer *, real *, real *, integer *, 
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___34 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SCHKPP tests SPPTRF, -TRI, -TRS, -RFS, and -CON */

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

/*  NNS     (input) INTEGER */
/*          The number of values of NRHS contained in the vector NSVAL. */

/*  NSVAL   (input) INTEGER array, dimension (NNS) */
/*          The values of the number of right hand sides NRHS. */

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  NMAX    (input) INTEGER */
/*          The maximum value permitted for N, used in dimensioning the */
/*          work arrays. */

/*  A       (workspace) REAL array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  AFAC    (workspace) REAL array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  AINV    (workspace) REAL array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  B       (workspace) REAL array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) REAL array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) REAL array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) REAL array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) REAL array, dimension */
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
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Single precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "PP", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	serrpo_(path, nout);
    }
    infoc_1.infot = 0;

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

	i__2 = nimat;
	for (imat = 1; imat <= i__2; ++imat) {

/*           Do the tests only if DOTYPE( IMAT ) is true. */

	    if (! dotype[imat]) {
		goto L100;
	    }

/*           Skip types 3, 4, or 5 if the matrix size is too small. */

	    zerot = imat >= 3 && imat <= 5;
	    if (zerot && n < imat - 2) {
		goto L100;
	    }

/*           Do first for UPLO = 'U', then for UPLO = 'L' */

	    for (iuplo = 1; iuplo <= 2; ++iuplo) {
		*(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 1];
		*(unsigned char *)packit = *(unsigned char *)&packs[iuplo - 1]
			;

/*              Set up parameters with SLATB4 and generate a test matrix */
/*              with SLATMS. */

		slatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, 
			&cndnum, dist);

		s_copy(srnamc_1.srnamt, "SLATMS", (ftnlen)6, (ftnlen)6);
		slatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &
			cndnum, &anorm, &kl, &ku, packit, &a[1], &lda, &work[
			1], &info);

/*              Check error code from SLATMS. */

		if (info != 0) {
		    alaerh_(path, "SLATMS", &info, &c__0, uplo, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L90;
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

/*                 Set row and column IZERO of A to 0. */

		    if (iuplo == 1) {
			ioff = (izero - 1) * izero / 2;
			i__3 = izero - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    a[ioff + i__] = 0.f;
/* L20: */
			}
			ioff += izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    a[ioff] = 0.f;
			    ioff += i__;
/* L30: */
			}
		    } else {
			ioff = izero;
			i__3 = izero - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    a[ioff] = 0.f;
			    ioff = ioff + n - i__;
/* L40: */
			}
			ioff -= izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    a[ioff + i__] = 0.f;
/* L50: */
			}
		    }
		} else {
		    izero = 0;
		}

/*              Compute the L*L' or U'*U factorization of the matrix. */

		npp = n * (n + 1) / 2;
		scopy_(&npp, &a[1], &c__1, &afac[1], &c__1);
		s_copy(srnamc_1.srnamt, "SPPTRF", (ftnlen)6, (ftnlen)6);
		spptrf_(uplo, &n, &afac[1], &info);

/*              Check error code from SPPTRF. */

		if (info != izero) {
		    alaerh_(path, "SPPTRF", &info, &izero, uplo, &n, &n, &
			    c_n1, &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L90;
		}

/*              Skip the tests if INFO is not 0. */

		if (info != 0) {
		    goto L90;
		}

/* +    TEST 1 */
/*              Reconstruct matrix from factors and compute residual. */

		scopy_(&npp, &afac[1], &c__1, &ainv[1], &c__1);
		sppt01_(uplo, &n, &a[1], &ainv[1], &rwork[1], result);

/* +    TEST 2 */
/*              Form the inverse and compute the residual. */

		scopy_(&npp, &afac[1], &c__1, &ainv[1], &c__1);
		s_copy(srnamc_1.srnamt, "SPPTRI", (ftnlen)6, (ftnlen)6);
		spptri_(uplo, &n, &ainv[1], &info);

/*              Check error code from SPPTRI. */

		if (info != 0) {
		    alaerh_(path, "SPPTRI", &info, &c__0, uplo, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		}

		sppt03_(uplo, &n, &a[1], &ainv[1], &work[1], &lda, &rwork[1], 
			&rcondc, &result[1]);

/*              Print information about the tests that did not pass */
/*              the threshold. */

		for (k = 1; k <= 2; ++k) {
		    if (result[k - 1] >= *thresh) {
			if (nfail == 0 && nerrs == 0) {
			    alahd_(nout, path);
			}
			io___34.ciunit = *nout;
			s_wsfe(&io___34);
			do_fio(&c__1, uplo, (ftnlen)1);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[k - 1], (ftnlen)sizeof(
				real));
			e_wsfe();
			++nfail;
		    }
/* L60: */
		}
		nrun += 2;

		i__3 = *nns;
		for (irhs = 1; irhs <= i__3; ++irhs) {
		    nrhs = nsval[irhs];

/* +    TEST 3 */
/*              Solve and compute residual for  A * X = B. */

		    s_copy(srnamc_1.srnamt, "SLARHS", (ftnlen)6, (ftnlen)6);
		    slarhs_(path, xtype, uplo, " ", &n, &n, &kl, &ku, &nrhs, &
			    a[1], &lda, &xact[1], &lda, &b[1], &lda, iseed, &
			    info);
		    slacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);

		    s_copy(srnamc_1.srnamt, "SPPTRS", (ftnlen)6, (ftnlen)6);
		    spptrs_(uplo, &n, &nrhs, &afac[1], &x[1], &lda, &info);

/*              Check error code from SPPTRS. */

		    if (info != 0) {
			alaerh_(path, "SPPTRS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    slacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		    sppt02_(uplo, &n, &nrhs, &a[1], &x[1], &lda, &work[1], &
			    lda, &rwork[1], &result[2]);

/* +    TEST 4 */
/*              Check solution from generated exact solution. */

		    sget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[3]);

/* +    TESTS 5, 6, and 7 */
/*              Use iterative refinement to improve the solution. */

		    s_copy(srnamc_1.srnamt, "SPPRFS", (ftnlen)6, (ftnlen)6);
		    spprfs_(uplo, &n, &nrhs, &a[1], &afac[1], &b[1], &lda, &x[
			    1], &lda, &rwork[1], &rwork[nrhs + 1], &work[1], &
			    iwork[1], &info);

/*              Check error code from SPPRFS. */

		    if (info != 0) {
			alaerh_(path, "SPPRFS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    sget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[4]);
		    sppt05_(uplo, &n, &nrhs, &a[1], &b[1], &lda, &x[1], &lda, 
			    &xact[1], &lda, &rwork[1], &rwork[nrhs + 1], &
			    result[5]);

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    for (k = 3; k <= 7; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___37.ciunit = *nout;
			    s_wsfe(&io___37);
			    do_fio(&c__1, uplo, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				    sizeof(real));
			    e_wsfe();
			    ++nfail;
			}
/* L70: */
		    }
		    nrun += 5;
/* L80: */
		}

/* +    TEST 8 */
/*              Get an estimate of RCOND = 1/CNDNUM. */

		anorm = slansp_("1", uplo, &n, &a[1], &rwork[1]);
		s_copy(srnamc_1.srnamt, "SPPCON", (ftnlen)6, (ftnlen)6);
		sppcon_(uplo, &n, &afac[1], &anorm, &rcond, &work[1], &iwork[
			1], &info);

/*              Check error code from SPPCON. */

		if (info != 0) {
		    alaerh_(path, "SPPCON", &info, &c__0, uplo, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		}

		result[7] = sget06_(&rcond, &rcondc);

/*              Print the test ratio if greater than or equal to THRESH. */

		if (result[7] >= *thresh) {
		    if (nfail == 0 && nerrs == 0) {
			alahd_(nout, path);
		    }
		    io___39.ciunit = *nout;
		    s_wsfe(&io___39);
		    do_fio(&c__1, uplo, (ftnlen)1);
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&result[7], (ftnlen)sizeof(real));
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
/* L110: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of SCHKPP */

} /* schkpp_ */
