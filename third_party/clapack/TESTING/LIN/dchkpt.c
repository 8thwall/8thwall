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
static doublereal c_b46 = 1.;
static doublereal c_b47 = 0.;
static integer c__7 = 7;

/* Subroutine */ int dchkpt_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, doublereal *thresh, logical *tsterr, 
	doublereal *a, doublereal *d__, doublereal *e, doublereal *b, 
	doublereal *x, doublereal *xact, doublereal *work, doublereal *rwork, 
	integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 0,0,0,1 };

    /* Format strings */
    static char fmt_9999[] = "(\002 N =\002,i5,\002, type \002,i2,\002, te"
	    "st \002,i2,\002, ratio = \002,g12.5)";
    static char fmt_9998[] = "(\002 N =\002,i5,\002, NRHS=\002,i3,\002, ty"
	    "pe \002,i2,\002, test(\002,i2,\002) = \002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n;
    doublereal z__[3];
    integer ia, in, kl, ku, ix, lda;
    doublereal cond;
    integer mode;
    doublereal dmax__;
    integer imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), dscal_(
	    integer *, doublereal *, doublereal *, integer *), dget04_(
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    doublereal rcond;
    integer nimat;
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal anorm;
    extern /* Subroutine */ int dptt01_(integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *), dcopy_(
	    integer *, doublereal *, integer *, doublereal *, integer *), 
	    dptt02_(integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *), 
	    dptt05_(integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *);
    integer izero, nerrs;
    logical zerot;
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), alaerh_(char *, 
	    char *, integer *, integer *, char *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *);
    extern integer idamax_(integer *, doublereal *, integer *);
    doublereal rcondc;
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlaptm_(integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *), alasum_(char *, integer *, integer *, integer *, 
	    integer *), dlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublereal *, integer *, doublereal 
	    *, integer *);
    extern doublereal dlanst_(char *, integer *, doublereal *, doublereal *);
    extern /* Subroutine */ int dlarnv_(integer *, integer *, integer *, 
	    doublereal *), derrgt_(char *, integer *);
    doublereal ainvnm;
    extern /* Subroutine */ int dptcon_(integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, integer *), dptrfs_(
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), dpttrf_(
	    integer *, doublereal *, doublereal *, integer *);
    doublereal result[7];
    extern /* Subroutine */ int dpttrs_(integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___29 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKPT tests DPTTRF, -TRS, -RFS, and -CON */

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

/*  A       (workspace) DOUBLE PRECISION array, dimension (NMAX*2) */

/*  D       (workspace) DOUBLE PRECISION array, dimension (NMAX*2) */

/*  E       (workspace) DOUBLE PRECISION array, dimension (NMAX*2) */

/*  B       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension */
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
    --xact;
    --x;
    --b;
    --e;
    --d__;
    --a;
    --nsval;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "PT", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	derrgt_(path, nout);
    }
    infoc_1.infot = 0;

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {

/*        Do for each value of N in NVAL. */

	n = nval[in];
	lda = max(1,n);
	nimat = 12;
	if (n <= 0) {
	    nimat = 1;
	}

	i__2 = nimat;
	for (imat = 1; imat <= i__2; ++imat) {

/*           Do the tests only if DOTYPE( IMAT ) is true. */

	    if (n > 0 && ! dotype[imat]) {
		goto L100;
	    }

/*           Set up parameters with DLATB4. */

	    dlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cond, dist);

	    zerot = imat >= 8 && imat <= 10;
	    if (imat <= 6) {

/*              Type 1-6:  generate a symmetric tridiagonal matrix of */
/*              known condition number in lower triangular band storage. */

		s_copy(srnamc_1.srnamt, "DLATMS", (ftnlen)6, (ftnlen)6);
		dlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cond, 
			&anorm, &kl, &ku, "B", &a[1], &c__2, &work[1], &info);

/*              Check the error code from DLATMS. */

		if (info != 0) {
		    alaerh_(path, "DLATMS", &info, &c__0, " ", &n, &n, &kl, &
			    ku, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L100;
		}
		izero = 0;

/*              Copy the matrix to D and E. */

		ia = 1;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d__[i__] = a[ia];
		    e[i__] = a[ia + 1];
		    ia += 2;
/* L20: */
		}
		if (n > 0) {
		    d__[n] = a[ia];
		}
	    } else {

/*              Type 7-12:  generate a diagonally dominant matrix with */
/*              unknown condition number in the vectors D and E. */

		if (! zerot || ! dotype[7]) {

/*                 Let D and E have values from [-1,1]. */

		    dlarnv_(&c__2, iseed, &n, &d__[1]);
		    i__3 = n - 1;
		    dlarnv_(&c__2, iseed, &i__3, &e[1]);

/*                 Make the tridiagonal matrix diagonally dominant. */

		    if (n == 1) {
			d__[1] = abs(d__[1]);
		    } else {
			d__[1] = abs(d__[1]) + abs(e[1]);
			d__[n] = (d__1 = d__[n], abs(d__1)) + (d__2 = e[n - 1]
				, abs(d__2));
			i__3 = n - 1;
			for (i__ = 2; i__ <= i__3; ++i__) {
			    d__[i__] = (d__1 = d__[i__], abs(d__1)) + (d__2 = 
				    e[i__], abs(d__2)) + (d__3 = e[i__ - 1], 
				    abs(d__3));
/* L30: */
			}
		    }

/*                 Scale D and E so the maximum element is ANORM. */

		    ix = idamax_(&n, &d__[1], &c__1);
		    dmax__ = d__[ix];
		    d__1 = anorm / dmax__;
		    dscal_(&n, &d__1, &d__[1], &c__1);
		    i__3 = n - 1;
		    d__1 = anorm / dmax__;
		    dscal_(&i__3, &d__1, &e[1], &c__1);

		} else if (izero > 0) {

/*                 Reuse the last matrix by copying back the zeroed out */
/*                 elements. */

		    if (izero == 1) {
			d__[1] = z__[1];
			if (n > 1) {
			    e[1] = z__[2];
			}
		    } else if (izero == n) {
			e[n - 1] = z__[0];
			d__[n] = z__[1];
		    } else {
			e[izero - 1] = z__[0];
			d__[izero] = z__[1];
			e[izero] = z__[2];
		    }
		}

/*              For types 8-10, set one row and column of the matrix to */
/*              zero. */

		izero = 0;
		if (imat == 8) {
		    izero = 1;
		    z__[1] = d__[1];
		    d__[1] = 0.;
		    if (n > 1) {
			z__[2] = e[1];
			e[1] = 0.;
		    }
		} else if (imat == 9) {
		    izero = n;
		    if (n > 1) {
			z__[0] = e[n - 1];
			e[n - 1] = 0.;
		    }
		    z__[1] = d__[n];
		    d__[n] = 0.;
		} else if (imat == 10) {
		    izero = (n + 1) / 2;
		    if (izero > 1) {
			z__[0] = e[izero - 1];
			e[izero - 1] = 0.;
			z__[2] = e[izero];
			e[izero] = 0.;
		    }
		    z__[1] = d__[izero];
		    d__[izero] = 0.;
		}
	    }

	    dcopy_(&n, &d__[1], &c__1, &d__[n + 1], &c__1);
	    if (n > 1) {
		i__3 = n - 1;
		dcopy_(&i__3, &e[1], &c__1, &e[n + 1], &c__1);
	    }

/* +    TEST 1 */
/*           Factor A as L*D*L' and compute the ratio */
/*              norm(L*D*L' - A) / (n * norm(A) * EPS ) */

	    dpttrf_(&n, &d__[n + 1], &e[n + 1], &info);

/*           Check error code from DPTTRF. */

	    if (info != izero) {
		alaerh_(path, "DPTTRF", &info, &izero, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		goto L100;
	    }

	    if (info > 0) {
		rcondc = 0.;
		goto L90;
	    }

	    dptt01_(&n, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &work[1], 
		    result);

/*           Print the test ratio if greater than or equal to THRESH. */

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

/*           Compute RCONDC = 1 / (norm(A) * norm(inv(A)) */

/*           Compute norm(A). */

	    anorm = dlanst_("1", &n, &d__[1], &e[1]);

/*           Use DPTTRS to solve for one column at a time of inv(A), */
/*           computing the maximum column sum as we go. */

	    ainvnm = 0.;
	    i__3 = n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		i__4 = n;
		for (j = 1; j <= i__4; ++j) {
		    x[j] = 0.;
/* L40: */
		}
		x[i__] = 1.;
		dpttrs_(&n, &c__1, &d__[n + 1], &e[n + 1], &x[1], &lda, &info)
			;
/* Computing MAX */
		d__1 = ainvnm, d__2 = dasum_(&n, &x[1], &c__1);
		ainvnm = max(d__1,d__2);
/* L50: */
	    }
/* Computing MAX */
	    d__1 = 1., d__2 = anorm * ainvnm;
	    rcondc = 1. / max(d__1,d__2);

	    i__3 = *nns;
	    for (irhs = 1; irhs <= i__3; ++irhs) {
		nrhs = nsval[irhs];

/*           Generate NRHS random solution vectors. */

		ix = 1;
		i__4 = nrhs;
		for (j = 1; j <= i__4; ++j) {
		    dlarnv_(&c__2, iseed, &n, &xact[ix]);
		    ix += lda;
/* L60: */
		}

/*           Set the right hand side. */

		dlaptm_(&n, &nrhs, &c_b46, &d__[1], &e[1], &xact[1], &lda, &
			c_b47, &b[1], &lda);

/* +    TEST 2 */
/*           Solve A*x = b and compute the residual. */

		dlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);
		dpttrs_(&n, &nrhs, &d__[n + 1], &e[n + 1], &x[1], &lda, &info)
			;

/*           Check error code from DPTTRS. */

		if (info != 0) {
		    alaerh_(path, "DPTTRS", &info, &c__0, " ", &n, &n, &c_n1, 
			    &c_n1, &nrhs, &imat, &nfail, &nerrs, nout);
		}

		dlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		dptt02_(&n, &nrhs, &d__[1], &e[1], &x[1], &lda, &work[1], &
			lda, &result[1]);

/* +    TEST 3 */
/*           Check solution from generated exact solution. */

		dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			result[2]);

/* +    TESTS 4, 5, and 6 */
/*           Use iterative refinement to improve the solution. */

		s_copy(srnamc_1.srnamt, "DPTRFS", (ftnlen)6, (ftnlen)6);
		dptrfs_(&n, &nrhs, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &b[
			1], &lda, &x[1], &lda, &rwork[1], &rwork[nrhs + 1], &
			work[1], &info);

/*           Check error code from DPTRFS. */

		if (info != 0) {
		    alaerh_(path, "DPTRFS", &info, &c__0, " ", &n, &n, &c_n1, 
			    &c_n1, &nrhs, &imat, &nfail, &nerrs, nout);
		}

		dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			result[3]);
		dptt05_(&n, &nrhs, &d__[1], &e[1], &b[1], &lda, &x[1], &lda, &
			xact[1], &lda, &rwork[1], &rwork[nrhs + 1], &result[4]
);

/*           Print information about the tests that did not pass the */
/*           threshold. */

		for (k = 2; k <= 6; ++k) {
		    if (result[k - 1] >= *thresh) {
			if (nfail == 0 && nerrs == 0) {
			    alahd_(nout, path);
			}
			io___35.ciunit = *nout;
			s_wsfe(&io___35);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[k - 1], (ftnlen)sizeof(
				doublereal));
			e_wsfe();
			++nfail;
		    }
/* L70: */
		}
		nrun += 5;
/* L80: */
	    }

/* +    TEST 7 */
/*           Estimate the reciprocal of the condition number of the */
/*           matrix. */

L90:
	    s_copy(srnamc_1.srnamt, "DPTCON", (ftnlen)6, (ftnlen)6);
	    dptcon_(&n, &d__[n + 1], &e[n + 1], &anorm, &rcond, &rwork[1], &
		    info);

/*           Check error code from DPTCON. */

	    if (info != 0) {
		alaerh_(path, "DPTCON", &info, &c__0, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
	    }

	    result[6] = dget06_(&rcond, &rcondc);

/*           Print the test ratio if greater than or equal to THRESH. */

	    if (result[6] >= *thresh) {
		if (nfail == 0 && nerrs == 0) {
		    alahd_(nout, path);
		}
		io___37.ciunit = *nout;
		s_wsfe(&io___37);
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&result[6], (ftnlen)sizeof(doublereal));
		e_wsfe();
		++nfail;
	    }
	    ++nrun;
L100:
	    ;
	}
/* L110: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of DCHKPT */

} /* dchkpt_ */
