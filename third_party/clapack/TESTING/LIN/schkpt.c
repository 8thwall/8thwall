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
static real c_b46 = 1.f;
static real c_b47 = 0.f;
static integer c__7 = 7;

/* Subroutine */ int schkpt_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, real *thresh, logical *tsterr, real *a, 
	real *d__, real *e, real *b, real *x, real *xact, real *work, real *
	rwork, integer *nout)
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
    real r__1, r__2, r__3;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n;
    real z__[3];
    integer ia, in, kl, ku, ix, lda;
    real cond;
    integer mode;
    real dmax__;
    integer imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer nfail, iseed[4];
    real rcond;
    extern /* Subroutine */ int sget04_(integer *, integer *, real *, integer 
	    *, real *, integer *, real *, real *), sscal_(integer *, real *, 
	    real *, integer *);
    integer nimat;
    extern doublereal sget06_(real *, real *);
    real anorm;
    integer izero, nerrs;
    extern doublereal sasum_(integer *, real *, integer *);
    extern /* Subroutine */ int sptt01_(integer *, real *, real *, real *, 
	    real *, real *, real *), sptt02_(integer *, integer *, real *, 
	    real *, real *, integer *, real *, integer *, real *), scopy_(
	    integer *, real *, integer *, real *, integer *), sptt05_(integer 
	    *, integer *, real *, real *, real *, integer *, real *, integer *
, real *, integer *, real *, real *, real *);
    logical zerot;
    extern /* Subroutine */ int slatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), alaerh_(char *, char *, integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    real rcondc;
    extern integer isamax_(integer *, real *, integer *);
    extern /* Subroutine */ int alasum_(char *, integer *, integer *, integer 
	    *, integer *);
    real ainvnm;
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaptm_(integer *, integer 
	    *, real *, real *, real *, real *, integer *, real *, real *, 
	    integer *), slatms_(integer *, integer *, char *, integer *, char 
	    *, real *, integer *, real *, real *, integer *, integer *, char *
, real *, integer *, real *, integer *);
    extern doublereal slanst_(char *, integer *, real *, real *);
    extern /* Subroutine */ int serrgt_(char *, integer *), slarnv_(
	    integer *, integer *, integer *, real *), sptcon_(integer *, real 
	    *, real *, real *, real *, real *, integer *);
    real result[7];
    extern /* Subroutine */ int sptrfs_(integer *, integer *, real *, real *, 
	    real *, real *, real *, integer *, real *, integer *, real *, 
	    real *, real *, integer *), spttrf_(integer *, real *, real *, 
	    integer *), spttrs_(integer *, integer *, real *, real *, real *, 
	    integer *, integer *);

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

/*  SCHKPT tests SPTTRF, -TRS, -RFS, and -CON */

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

/*  A       (workspace) REAL array, dimension (NMAX*2) */

/*  D       (workspace) REAL array, dimension (NMAX*2) */

/*  E       (workspace) REAL array, dimension (NMAX*2) */

/*  B       (workspace) REAL array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) REAL array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) REAL array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) REAL array, dimension */
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

    s_copy(path, "Single precision", (ftnlen)1, (ftnlen)16);
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
	serrgt_(path, nout);
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

/*           Set up parameters with SLATB4. */

	    slatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cond, dist);

	    zerot = imat >= 8 && imat <= 10;
	    if (imat <= 6) {

/*              Type 1-6:  generate a symmetric tridiagonal matrix of */
/*              known condition number in lower triangular band storage. */

		s_copy(srnamc_1.srnamt, "SLATMS", (ftnlen)6, (ftnlen)6);
		slatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cond, 
			&anorm, &kl, &ku, "B", &a[1], &c__2, &work[1], &info);

/*              Check the error code from SLATMS. */

		if (info != 0) {
		    alaerh_(path, "SLATMS", &info, &c__0, " ", &n, &n, &kl, &
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

		    slarnv_(&c__2, iseed, &n, &d__[1]);
		    i__3 = n - 1;
		    slarnv_(&c__2, iseed, &i__3, &e[1]);

/*                 Make the tridiagonal matrix diagonally dominant. */

		    if (n == 1) {
			d__[1] = dabs(d__[1]);
		    } else {
			d__[1] = dabs(d__[1]) + dabs(e[1]);
			d__[n] = (r__1 = d__[n], dabs(r__1)) + (r__2 = e[n - 
				1], dabs(r__2));
			i__3 = n - 1;
			for (i__ = 2; i__ <= i__3; ++i__) {
			    d__[i__] = (r__1 = d__[i__], dabs(r__1)) + (r__2 =
				     e[i__], dabs(r__2)) + (r__3 = e[i__ - 1],
				     dabs(r__3));
/* L30: */
			}
		    }

/*                 Scale D and E so the maximum element is ANORM. */

		    ix = isamax_(&n, &d__[1], &c__1);
		    dmax__ = d__[ix];
		    r__1 = anorm / dmax__;
		    sscal_(&n, &r__1, &d__[1], &c__1);
		    i__3 = n - 1;
		    r__1 = anorm / dmax__;
		    sscal_(&i__3, &r__1, &e[1], &c__1);

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
		    d__[1] = 0.f;
		    if (n > 1) {
			z__[2] = e[1];
			e[1] = 0.f;
		    }
		} else if (imat == 9) {
		    izero = n;
		    if (n > 1) {
			z__[0] = e[n - 1];
			e[n - 1] = 0.f;
		    }
		    z__[1] = d__[n];
		    d__[n] = 0.f;
		} else if (imat == 10) {
		    izero = (n + 1) / 2;
		    if (izero > 1) {
			z__[0] = e[izero - 1];
			e[izero - 1] = 0.f;
			z__[2] = e[izero];
			e[izero] = 0.f;
		    }
		    z__[1] = d__[izero];
		    d__[izero] = 0.f;
		}
	    }

	    scopy_(&n, &d__[1], &c__1, &d__[n + 1], &c__1);
	    if (n > 1) {
		i__3 = n - 1;
		scopy_(&i__3, &e[1], &c__1, &e[n + 1], &c__1);
	    }

/* +    TEST 1 */
/*           Factor A as L*D*L' and compute the ratio */
/*              norm(L*D*L' - A) / (n * norm(A) * EPS ) */

	    spttrf_(&n, &d__[n + 1], &e[n + 1], &info);

/*           Check error code from SPTTRF. */

	    if (info != izero) {
		alaerh_(path, "SPTTRF", &info, &izero, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		goto L100;
	    }

	    if (info > 0) {
		rcondc = 0.f;
		goto L90;
	    }

	    sptt01_(&n, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &work[1], 
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
		do_fio(&c__1, (char *)&result[0], (ftnlen)sizeof(real));
		e_wsfe();
		++nfail;
	    }
	    ++nrun;

/*           Compute RCONDC = 1 / (norm(A) * norm(inv(A)) */

/*           Compute norm(A). */

	    anorm = slanst_("1", &n, &d__[1], &e[1]);

/*           Use SPTTRS to solve for one column at a time of inv(A), */
/*           computing the maximum column sum as we go. */

	    ainvnm = 0.f;
	    i__3 = n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		i__4 = n;
		for (j = 1; j <= i__4; ++j) {
		    x[j] = 0.f;
/* L40: */
		}
		x[i__] = 1.f;
		spttrs_(&n, &c__1, &d__[n + 1], &e[n + 1], &x[1], &lda, &info)
			;
/* Computing MAX */
		r__1 = ainvnm, r__2 = sasum_(&n, &x[1], &c__1);
		ainvnm = dmax(r__1,r__2);
/* L50: */
	    }
/* Computing MAX */
	    r__1 = 1.f, r__2 = anorm * ainvnm;
	    rcondc = 1.f / dmax(r__1,r__2);

	    i__3 = *nns;
	    for (irhs = 1; irhs <= i__3; ++irhs) {
		nrhs = nsval[irhs];

/*           Generate NRHS random solution vectors. */

		ix = 1;
		i__4 = nrhs;
		for (j = 1; j <= i__4; ++j) {
		    slarnv_(&c__2, iseed, &n, &xact[ix]);
		    ix += lda;
/* L60: */
		}

/*           Set the right hand side. */

		slaptm_(&n, &nrhs, &c_b46, &d__[1], &e[1], &xact[1], &lda, &
			c_b47, &b[1], &lda);

/* +    TEST 2 */
/*           Solve A*x = b and compute the residual. */

		slacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);
		spttrs_(&n, &nrhs, &d__[n + 1], &e[n + 1], &x[1], &lda, &info)
			;

/*           Check error code from SPTTRS. */

		if (info != 0) {
		    alaerh_(path, "SPTTRS", &info, &c__0, " ", &n, &n, &c_n1, 
			    &c_n1, &nrhs, &imat, &nfail, &nerrs, nout);
		}

		slacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		sptt02_(&n, &nrhs, &d__[1], &e[1], &x[1], &lda, &work[1], &
			lda, &result[1]);

/* +    TEST 3 */
/*           Check solution from generated exact solution. */

		sget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			result[2]);

/* +    TESTS 4, 5, and 6 */
/*           Use iterative refinement to improve the solution. */

		s_copy(srnamc_1.srnamt, "SPTRFS", (ftnlen)6, (ftnlen)6);
		sptrfs_(&n, &nrhs, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &b[
			1], &lda, &x[1], &lda, &rwork[1], &rwork[nrhs + 1], &
			work[1], &info);

/*           Check error code from SPTRFS. */

		if (info != 0) {
		    alaerh_(path, "SPTRFS", &info, &c__0, " ", &n, &n, &c_n1, 
			    &c_n1, &nrhs, &imat, &nfail, &nerrs, nout);
		}

		sget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			result[3]);
		sptt05_(&n, &nrhs, &d__[1], &e[1], &b[1], &lda, &x[1], &lda, &
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
				real));
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
	    s_copy(srnamc_1.srnamt, "SPTCON", (ftnlen)6, (ftnlen)6);
	    sptcon_(&n, &d__[n + 1], &e[n + 1], &anorm, &rcond, &rwork[1], &
		    info);

/*           Check error code from SPTCON. */

	    if (info != 0) {
		alaerh_(path, "SPTCON", &info, &c__0, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
	    }

	    result[6] = sget06_(&rcond, &rcondc);

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
		do_fio(&c__1, (char *)&result[6], (ftnlen)sizeof(real));
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

/*     End of SCHKPT */

} /* schkpt_ */
