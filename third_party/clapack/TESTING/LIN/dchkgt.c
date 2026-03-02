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

static integer c__3 = 3;
static integer c__0 = 0;
static integer c_n1 = -1;
static integer c__1 = 1;
static integer c__2 = 2;
static integer c__7 = 7;
static doublereal c_b63 = 1.;
static doublereal c_b64 = 0.;

/* Subroutine */ int dchkgt_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, doublereal *thresh, logical *tsterr, 
	doublereal *a, doublereal *af, doublereal *b, doublereal *x, 
	doublereal *xact, doublereal *work, doublereal *rwork, integer *iwork, 
	 integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 0,0,0,1 };
    static char transs[1*3] = "N" "T" "C";

    /* Format strings */
    static char fmt_9999[] = "(12x,\002N =\002,i5,\002,\002,10x,\002 type"
	    " \002,i2,\002, test(\002,i2,\002) = \002,g12.5)";
    static char fmt_9997[] = "(\002 NORM ='\002,a1,\002', N =\002,i5,\002"
	    ",\002,10x,\002 type \002,i2,\002, test(\002,i2,\002) = \002,g12."
	    "5)";
    static char fmt_9998[] = "(\002 TRANS='\002,a1,\002', N =\002,i5,\002, N"
	    "RHS=\002,i3,\002, type \002,i2,\002, test(\002,i2,\002) = \002,g"
	    "12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublereal d__1, d__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, m, n;
    doublereal z__[3];
    integer in, kl, ku, ix, lda;
    doublereal cond;
    integer mode, koff, imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char norm[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), dscal_(
	    integer *, doublereal *, doublereal *, integer *), dget04_(
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    extern /* Subroutine */ int dgtt01_(integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *), dgtt02_(char *, integer *, integer *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, integer *, doublereal *
, integer *, doublereal *, doublereal *);
    doublereal rcond;
    extern /* Subroutine */ int dgtt05_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *);
    integer nimat;
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm;
    integer itran;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    char trans[1];
    integer izero, nerrs;
    logical zerot;
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), alaerh_(char *, 
	    char *, integer *, integer *, char *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *);
    doublereal rcondc;
    extern doublereal dlangt_(char *, integer *, doublereal *, doublereal *, 
	    doublereal *);
    extern /* Subroutine */ int derrge_(char *, integer *), dlagtm_(
	    char *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     doublereal *, integer *), dlacpy_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *);
    doublereal rcondi;
    extern /* Subroutine */ int dgtcon_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     doublereal *, doublereal *, integer *, integer *), 
	    alasum_(char *, integer *, integer *, integer *, integer *);
    doublereal rcondo;
    extern /* Subroutine */ int dlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublereal *, integer *, doublereal 
	    *, integer *), dlarnv_(integer *, integer 
	    *, integer *, doublereal *);
    doublereal ainvnm;
    extern /* Subroutine */ int dgtrfs_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), dgttrf_(integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *);
    logical trfcon;
    extern /* Subroutine */ int dgttrs_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, integer *);
    doublereal result[7];

    /* Fortran I/O blocks */
    static cilist io___29 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKGT tests DGTTRF, -TRS, -RFS, and -CON */

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

/*  THRESH  (input) DOUBLE PRECISION */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  A       (workspace) DOUBLE PRECISION array, dimension (NMAX*4) */

/*  AF      (workspace) DOUBLE PRECISION array, dimension (NMAX*4) */

/*  B       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension */
/*                      (max(NMAX,2*NSMAX)) */

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
    --xact;
    --x;
    --b;
    --af;
    --a;
    --nsval;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "GT", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	derrge_(path, nout);
    }
    infoc_1.infot = 0;

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {

/*        Do for each value of N in NVAL. */

	n = nval[in];
/* Computing MAX */
	i__2 = n - 1;
	m = max(i__2,0);
	lda = max(1,n);
	nimat = 12;
	if (n <= 0) {
	    nimat = 1;
	}

	i__2 = nimat;
	for (imat = 1; imat <= i__2; ++imat) {

/*           Do the tests only if DOTYPE( IMAT ) is true. */

	    if (! dotype[imat]) {
		goto L100;
	    }

/*           Set up parameters with DLATB4. */

	    dlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cond, dist);

	    zerot = imat >= 8 && imat <= 10;
	    if (imat <= 6) {

/*              Types 1-6:  generate matrices of known condition number. */

/* Computing MAX */
		i__3 = 2 - ku, i__4 = 3 - max(1,n);
		koff = max(i__3,i__4);
		s_copy(srnamc_1.srnamt, "DLATMS", (ftnlen)6, (ftnlen)6);
		dlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cond, 
			&anorm, &kl, &ku, "Z", &af[koff], &c__3, &work[1], &
			info);

/*              Check the error code from DLATMS. */

		if (info != 0) {
		    alaerh_(path, "DLATMS", &info, &c__0, " ", &n, &n, &kl, &
			    ku, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L100;
		}
		izero = 0;

		if (n > 1) {
		    i__3 = n - 1;
		    dcopy_(&i__3, &af[4], &c__3, &a[1], &c__1);
		    i__3 = n - 1;
		    dcopy_(&i__3, &af[3], &c__3, &a[n + m + 1], &c__1);
		}
		dcopy_(&n, &af[2], &c__3, &a[m + 1], &c__1);
	    } else {

/*              Types 7-12:  generate tridiagonal matrices with */
/*              unknown condition numbers. */

		if (! zerot || ! dotype[7]) {

/*                 Generate a matrix with elements from [-1,1]. */

		    i__3 = n + (m << 1);
		    dlarnv_(&c__2, iseed, &i__3, &a[1]);
		    if (anorm != 1.) {
			i__3 = n + (m << 1);
			dscal_(&i__3, &anorm, &a[1], &c__1);
		    }
		} else if (izero > 0) {

/*                 Reuse the last matrix by copying back the zeroed out */
/*                 elements. */

		    if (izero == 1) {
			a[n] = z__[1];
			if (n > 1) {
			    a[1] = z__[2];
			}
		    } else if (izero == n) {
			a[n * 3 - 2] = z__[0];
			a[(n << 1) - 1] = z__[1];
		    } else {
			a[(n << 1) - 2 + izero] = z__[0];
			a[n - 1 + izero] = z__[1];
			a[izero] = z__[2];
		    }
		}

/*              If IMAT > 7, set one column of the matrix to 0. */

		if (! zerot) {
		    izero = 0;
		} else if (imat == 8) {
		    izero = 1;
		    z__[1] = a[n];
		    a[n] = 0.;
		    if (n > 1) {
			z__[2] = a[1];
			a[1] = 0.;
		    }
		} else if (imat == 9) {
		    izero = n;
		    z__[0] = a[n * 3 - 2];
		    z__[1] = a[(n << 1) - 1];
		    a[n * 3 - 2] = 0.;
		    a[(n << 1) - 1] = 0.;
		} else {
		    izero = (n + 1) / 2;
		    i__3 = n - 1;
		    for (i__ = izero; i__ <= i__3; ++i__) {
			a[(n << 1) - 2 + i__] = 0.;
			a[n - 1 + i__] = 0.;
			a[i__] = 0.;
/* L20: */
		    }
		    a[n * 3 - 2] = 0.;
		    a[(n << 1) - 1] = 0.;
		}
	    }

/* +    TEST 1 */
/*           Factor A as L*U and compute the ratio */
/*              norm(L*U - A) / (n * norm(A) * EPS ) */

	    i__3 = n + (m << 1);
	    dcopy_(&i__3, &a[1], &c__1, &af[1], &c__1);
	    s_copy(srnamc_1.srnamt, "DGTTRF", (ftnlen)6, (ftnlen)6);
	    dgttrf_(&n, &af[1], &af[m + 1], &af[n + m + 1], &af[n + (m << 1) 
		    + 1], &iwork[1], &info);

/*           Check error code from DGTTRF. */

	    if (info != izero) {
		alaerh_(path, "DGTTRF", &info, &izero, " ", &n, &n, &c__1, &
			c__1, &c_n1, &imat, &nfail, &nerrs, nout);
	    }
	    trfcon = info != 0;

	    dgtt01_(&n, &a[1], &a[m + 1], &a[n + m + 1], &af[1], &af[m + 1], &
		    af[n + m + 1], &af[n + (m << 1) + 1], &iwork[1], &work[1], 
		     &lda, &rwork[1], result);

/*           Print the test ratio if it is .GE. THRESH. */

	    if (result[0] >= *thresh) {
		if (nfail == 0 && nerrs == 0) {
		    alahd_(nout, path);
		}
		io___29.ciunit = *nout;
		s_wsfe(&io___29);
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&result[0], (ftnlen)sizeof(doublereal));
		e_wsfe();
		++nfail;
	    }
	    ++nrun;

	    for (itran = 1; itran <= 2; ++itran) {
		*(unsigned char *)trans = *(unsigned char *)&transs[itran - 1]
			;
		if (itran == 1) {
		    *(unsigned char *)norm = 'O';
		} else {
		    *(unsigned char *)norm = 'I';
		}
		anorm = dlangt_(norm, &n, &a[1], &a[m + 1], &a[n + m + 1]);

		if (! trfcon) {

/*                 Use DGTTRS to solve for one column at a time of inv(A) */
/*                 or inv(A^T), computing the maximum column sum as we */
/*                 go. */

		    ainvnm = 0.;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			i__4 = n;
			for (j = 1; j <= i__4; ++j) {
			    x[j] = 0.;
/* L30: */
			}
			x[i__] = 1.;
			dgttrs_(trans, &n, &c__1, &af[1], &af[m + 1], &af[n + 
				m + 1], &af[n + (m << 1) + 1], &iwork[1], &x[
				1], &lda, &info);
/* Computing MAX */
			d__1 = ainvnm, d__2 = dasum_(&n, &x[1], &c__1);
			ainvnm = max(d__1,d__2);
/* L40: */
		    }

/*                 Compute RCONDC = 1 / (norm(A) * norm(inv(A)) */

		    if (anorm <= 0. || ainvnm <= 0.) {
			rcondc = 1.;
		    } else {
			rcondc = 1. / anorm / ainvnm;
		    }
		    if (itran == 1) {
			rcondo = rcondc;
		    } else {
			rcondi = rcondc;
		    }
		} else {
		    rcondc = 0.;
		}

/* +    TEST 7 */
/*              Estimate the reciprocal of the condition number of the */
/*              matrix. */

		s_copy(srnamc_1.srnamt, "DGTCON", (ftnlen)6, (ftnlen)6);
		dgtcon_(norm, &n, &af[1], &af[m + 1], &af[n + m + 1], &af[n + 
			(m << 1) + 1], &iwork[1], &anorm, &rcond, &work[1], &
			iwork[n + 1], &info);

/*              Check error code from DGTCON. */

		if (info != 0) {
		    alaerh_(path, "DGTCON", &info, &c__0, norm, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		}

		result[6] = dget06_(&rcond, &rcondc);

/*              Print the test ratio if it is .GE. THRESH. */

		if (result[6] >= *thresh) {
		    if (nfail == 0 && nerrs == 0) {
			alahd_(nout, path);
		    }
		    io___39.ciunit = *nout;
		    s_wsfe(&io___39);
		    do_fio(&c__1, norm, (ftnlen)1);
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&result[6], (ftnlen)sizeof(
			    doublereal));
		    e_wsfe();
		    ++nfail;
		}
		++nrun;
/* L50: */
	    }

/*           Skip the remaining tests if the matrix is singular. */

	    if (trfcon) {
		goto L100;
	    }

	    i__3 = *nns;
	    for (irhs = 1; irhs <= i__3; ++irhs) {
		nrhs = nsval[irhs];

/*              Generate NRHS random solution vectors. */

		ix = 1;
		i__4 = nrhs;
		for (j = 1; j <= i__4; ++j) {
		    dlarnv_(&c__2, iseed, &n, &xact[ix]);
		    ix += lda;
/* L60: */
		}

		for (itran = 1; itran <= 3; ++itran) {
		    *(unsigned char *)trans = *(unsigned char *)&transs[itran 
			    - 1];
		    if (itran == 1) {
			rcondc = rcondo;
		    } else {
			rcondc = rcondi;
		    }

/*                 Set the right hand side. */

		    dlagtm_(trans, &n, &nrhs, &c_b63, &a[1], &a[m + 1], &a[n 
			    + m + 1], &xact[1], &lda, &c_b64, &b[1], &lda);

/* +    TEST 2 */
/*                 Solve op(A) * X = B and compute the residual. */

		    dlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);
		    s_copy(srnamc_1.srnamt, "DGTTRS", (ftnlen)6, (ftnlen)6);
		    dgttrs_(trans, &n, &nrhs, &af[1], &af[m + 1], &af[n + m + 
			    1], &af[n + (m << 1) + 1], &iwork[1], &x[1], &lda, 
			     &info);

/*                 Check error code from DGTTRS. */

		    if (info != 0) {
			alaerh_(path, "DGTTRS", &info, &c__0, trans, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    dlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		    dgtt02_(trans, &n, &nrhs, &a[1], &a[m + 1], &a[n + m + 1], 
			     &x[1], &lda, &work[1], &lda, &rwork[1], &result[
			    1]);

/* +    TEST 3 */
/*                 Check solution from generated exact solution. */

		    dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[2]);

/* +    TESTS 4, 5, and 6 */
/*                 Use iterative refinement to improve the solution. */

		    s_copy(srnamc_1.srnamt, "DGTRFS", (ftnlen)6, (ftnlen)6);
		    dgtrfs_(trans, &n, &nrhs, &a[1], &a[m + 1], &a[n + m + 1], 
			     &af[1], &af[m + 1], &af[n + m + 1], &af[n + (m <<
			     1) + 1], &iwork[1], &b[1], &lda, &x[1], &lda, &
			    rwork[1], &rwork[nrhs + 1], &work[1], &iwork[n + 
			    1], &info);

/*                 Check error code from DGTRFS. */

		    if (info != 0) {
			alaerh_(path, "DGTRFS", &info, &c__0, trans, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[3]);
		    dgtt05_(trans, &n, &nrhs, &a[1], &a[m + 1], &a[n + m + 1], 
			     &b[1], &lda, &x[1], &lda, &xact[1], &lda, &rwork[
			    1], &rwork[nrhs + 1], &result[4]);

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    for (k = 2; k <= 6; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___44.ciunit = *nout;
			    s_wsfe(&io___44);
			    do_fio(&c__1, trans, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer))
				    ;
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
/* L90: */
	    }

L100:
	    ;
	}
/* L110: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of DCHKGT */

} /* dchkgt_ */
