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
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__8 = 8;

/* Subroutine */ int cchkpp_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, real *thresh, logical *tsterr, integer *
	nmax, complex *a, complex *afac, complex *ainv, complex *b, complex *
	x, complex *xact, complex *work, real *rwork, integer *nout)
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
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, k, n, in, kl, ku, lda, npp, ioff, mode, imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char uplo[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), cget04_(
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    real *, real *);
    integer nfail, iseed[4];
    real rcond;
    extern /* Subroutine */ int cppt01_(char *, integer *, complex *, complex 
	    *, real *, real *);
    integer nimat;
    extern doublereal sget06_(real *, real *);
    extern /* Subroutine */ int cppt02_(char *, integer *, integer *, complex 
	    *, complex *, integer *, complex *, integer *, real *, real *), cppt03_(char *, integer *, complex *, complex *, complex 
	    *, integer *, real *, real *, real *);
    real anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), cppt05_(char *, integer *, integer *, 
	    complex *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, real *, real *);
    integer iuplo, izero, nerrs;
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int clatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), alaerh_(char *, char *, integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *), claipd_(integer *, complex *, integer *, integer 
	    *);
    extern doublereal clanhp_(char *, char *, integer *, complex *, real *);
    real rcondc;
    char packit[1];
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), clarhs_(char *, char 
	    *, char *, char *, integer *, integer *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, integer *, integer *), 
	    alasum_(char *, integer *, integer *, integer *, integer *);
    real cndnum;
    extern /* Subroutine */ int clatms_(integer *, integer *, char *, integer 
	    *, char *, real *, integer *, real *, real *, integer *, integer *
, char *, complex *, integer *, complex *, integer *), cppcon_(char *, integer *, complex *, real *, 
	    real *, complex *, real *, integer *), cerrpo_(char *, 
	    integer *), cpprfs_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *, complex *, integer *, real *, 
	    real *, complex *, real *, integer *), cpptrf_(char *, 
	    integer *, complex *, integer *), cpptri_(char *, integer 
	    *, complex *, integer *), cpptrs_(char *, integer *, 
	    integer *, complex *, complex *, integer *, integer *);
    real result[8];

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

/*  CCHKPP tests CPPTRF, -TRI, -TRS, -RFS, and -CON */

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

/*  A       (workspace) COMPLEX array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  AFAC    (workspace) COMPLEX array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  AINV    (workspace) COMPLEX array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  B       (workspace) COMPLEX array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) COMPLEX array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) COMPLEX array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) COMPLEX array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) REAL array, dimension */
/*                      (max(NMAX,2*NSMAX)) */

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

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
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
	cerrpo_(path, nout);
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

/*              Set up parameters with CLATB4 and generate a test matrix */
/*              with CLATMS. */

		clatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, 
			&cndnum, dist);

		s_copy(srnamc_1.srnamt, "CLATMS", (ftnlen)6, (ftnlen)6);
		clatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &
			cndnum, &anorm, &kl, &ku, packit, &a[1], &lda, &work[
			1], &info);

/*              Check error code from CLATMS. */

		if (info != 0) {
		    alaerh_(path, "CLATMS", &info, &c__0, uplo, &n, &n, &c_n1, 
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
			    i__4 = ioff + i__;
			    a[i__4].r = 0.f, a[i__4].i = 0.f;
/* L20: */
			}
			ioff += izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    i__4 = ioff;
			    a[i__4].r = 0.f, a[i__4].i = 0.f;
			    ioff += i__;
/* L30: */
			}
		    } else {
			ioff = izero;
			i__3 = izero - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    i__4 = ioff;
			    a[i__4].r = 0.f, a[i__4].i = 0.f;
			    ioff = ioff + n - i__;
/* L40: */
			}
			ioff -= izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    i__4 = ioff + i__;
			    a[i__4].r = 0.f, a[i__4].i = 0.f;
/* L50: */
			}
		    }
		} else {
		    izero = 0;
		}

/*              Set the imaginary part of the diagonals. */

		if (iuplo == 1) {
		    claipd_(&n, &a[1], &c__2, &c__1);
		} else {
		    claipd_(&n, &a[1], &n, &c_n1);
		}

/*              Compute the L*L' or U'*U factorization of the matrix. */

		npp = n * (n + 1) / 2;
		ccopy_(&npp, &a[1], &c__1, &afac[1], &c__1);
		s_copy(srnamc_1.srnamt, "CPPTRF", (ftnlen)6, (ftnlen)6);
		cpptrf_(uplo, &n, &afac[1], &info);

/*              Check error code from CPPTRF. */

		if (info != izero) {
		    alaerh_(path, "CPPTRF", &info, &izero, uplo, &n, &n, &
			    c_n1, &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L90;
		}

/*              Skip the tests if INFO is not 0. */

		if (info != 0) {
		    goto L90;
		}

/* +    TEST 1 */
/*              Reconstruct matrix from factors and compute residual. */

		ccopy_(&npp, &afac[1], &c__1, &ainv[1], &c__1);
		cppt01_(uplo, &n, &a[1], &ainv[1], &rwork[1], result);

/* +    TEST 2 */
/*              Form the inverse and compute the residual. */

		ccopy_(&npp, &afac[1], &c__1, &ainv[1], &c__1);
		s_copy(srnamc_1.srnamt, "CPPTRI", (ftnlen)6, (ftnlen)6);
		cpptri_(uplo, &n, &ainv[1], &info);

/*              Check error code from CPPTRI. */

		if (info != 0) {
		    alaerh_(path, "CPPTRI", &info, &c__0, uplo, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		}

		cppt03_(uplo, &n, &a[1], &ainv[1], &work[1], &lda, &rwork[1], 
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

		    s_copy(srnamc_1.srnamt, "CLARHS", (ftnlen)6, (ftnlen)6);
		    clarhs_(path, xtype, uplo, " ", &n, &n, &kl, &ku, &nrhs, &
			    a[1], &lda, &xact[1], &lda, &b[1], &lda, iseed, &
			    info);
		    clacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);

		    s_copy(srnamc_1.srnamt, "CPPTRS", (ftnlen)6, (ftnlen)6);
		    cpptrs_(uplo, &n, &nrhs, &afac[1], &x[1], &lda, &info);

/*              Check error code from CPPTRS. */

		    if (info != 0) {
			alaerh_(path, "CPPTRS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    clacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		    cppt02_(uplo, &n, &nrhs, &a[1], &x[1], &lda, &work[1], &
			    lda, &rwork[1], &result[2]);

/* +    TEST 4 */
/*              Check solution from generated exact solution. */

		    cget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[3]);

/* +    TESTS 5, 6, and 7 */
/*              Use iterative refinement to improve the solution. */

		    s_copy(srnamc_1.srnamt, "CPPRFS", (ftnlen)6, (ftnlen)6);
		    cpprfs_(uplo, &n, &nrhs, &a[1], &afac[1], &b[1], &lda, &x[
			    1], &lda, &rwork[1], &rwork[nrhs + 1], &work[1], &
			    rwork[(nrhs << 1) + 1], &info);

/*              Check error code from CPPRFS. */

		    if (info != 0) {
			alaerh_(path, "CPPRFS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    cget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[4]);
		    cppt05_(uplo, &n, &nrhs, &a[1], &b[1], &lda, &x[1], &lda, 
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

		anorm = clanhp_("1", uplo, &n, &a[1], &rwork[1]);
		s_copy(srnamc_1.srnamt, "CPPCON", (ftnlen)6, (ftnlen)6);
		cppcon_(uplo, &n, &afac[1], &anorm, &rcond, &work[1], &rwork[
			1], &info);

/*              Check error code from CPPCON. */

		if (info != 0) {
		    alaerh_(path, "CPPCON", &info, &c__0, uplo, &n, &n, &c_n1, 
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

/*     End of CCHKPP */

} /* cchkpp_ */
