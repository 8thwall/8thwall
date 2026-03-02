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
static doublereal c_b48 = 1.;
static doublereal c_b49 = 0.;
static integer c__7 = 7;

/* Subroutine */ int zchkpt_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, doublereal *thresh, logical *tsterr, 
	doublecomplex *a, doublereal *d__, doublecomplex *e, doublecomplex *b, 
	 doublecomplex *x, doublecomplex *xact, doublecomplex *work, 
	doublereal *rwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 0,0,0,1 };
    static char uplos[1*2] = "U" "L";

    /* Format strings */
    static char fmt_9999[] = "(\002 N =\002,i5,\002, type \002,i2,\002, te"
	    "st \002,i2,\002, ratio = \002,g12.5)";
    static char fmt_9998[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002, "
	    "NRHS =\002,i3,\002, type \002,i2,\002, test \002,i2,\002, ratio "
	    "= \002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double z_abs(doublecomplex *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n;
    doublecomplex z__[3];
    integer ia, in, kl, ku, ix, lda;
    doublereal cond;
    integer mode;
    doublereal dmax__;
    integer imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char uplo[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), dscal_(
	    integer *, doublereal *, doublereal *, integer *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    doublereal rcond;
    integer nimat;
    doublereal anorm;
    extern /* Subroutine */ int zget04_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
), dcopy_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    integer iuplo, izero, nerrs;
    logical zerot;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zptt01_(integer *, doublereal *, 
	    doublecomplex *, doublereal *, doublecomplex *, doublecomplex *, 
	    doublereal *), zptt02_(char *, integer *, integer *, doublereal *, 
	     doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublereal *), zptt05_(integer *, integer *, 
	    doublereal *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *), zlatb4_(char *, 
	    integer *, integer *, integer *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, char *), alaerh_(char *, char *, integer *, integer *, char *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *);
    extern integer idamax_(integer *, doublereal *, integer *);
    doublereal rcondc;
    extern /* Subroutine */ int zdscal_(integer *, doublereal *, 
	    doublecomplex *, integer *), alasum_(char *, integer *, integer *, 
	     integer *, integer *), dlarnv_(integer *, integer *, 
	    integer *, doublereal *);
    doublereal ainvnm;
    extern doublereal zlanht_(char *, integer *, doublereal *, doublecomplex *
);
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    extern doublereal dzasum_(integer *, doublecomplex *, integer *);
    extern /* Subroutine */ int zlaptm_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublecomplex *, 
	    integer *, doublereal *, doublecomplex *, integer *), 
	    zlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublecomplex *, integer *, doublecomplex *, 
	    integer *), zlarnv_(integer *, integer *, 
	    integer *, doublecomplex *), zerrgt_(char *, integer *);
    doublereal result[7];
    extern /* Subroutine */ int zptcon_(integer *, doublereal *, 
	    doublecomplex *, doublereal *, doublereal *, doublereal *, 
	    integer *), zptrfs_(char *, integer *, integer *, doublereal *, 
	    doublecomplex *, doublereal *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublecomplex *, doublereal *, integer *), zpttrf_(
	    integer *, doublereal *, doublecomplex *, integer *), zpttrs_(
	    char *, integer *, integer *, doublereal *, doublecomplex *, 
	    doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___30 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZCHKPT tests ZPTTRF, -TRS, -RFS, and -CON */

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

/*  A       (workspace) COMPLEX*16 array, dimension (NMAX*2) */

/*  D       (workspace) DOUBLE PRECISION array, dimension (NMAX*2) */

/*  E       (workspace) COMPLEX*16 array, dimension (NMAX*2) */

/*  B       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) COMPLEX*16 array, dimension */
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

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
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
	zerrgt_(path, nout);
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
		goto L110;
	    }

/*           Set up parameters with ZLATB4. */

	    zlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cond, dist);

	    zerot = imat >= 8 && imat <= 10;
	    if (imat <= 6) {

/*              Type 1-6:  generate a Hermitian tridiagonal matrix of */
/*              known condition number in lower triangular band storage. */

		s_copy(srnamc_1.srnamt, "ZLATMS", (ftnlen)6, (ftnlen)6);
		zlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cond, 
			&anorm, &kl, &ku, "B", &a[1], &c__2, &work[1], &info);

/*              Check the error code from ZLATMS. */

		if (info != 0) {
		    alaerh_(path, "ZLATMS", &info, &c__0, " ", &n, &n, &kl, &
			    ku, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L110;
		}
		izero = 0;

/*              Copy the matrix to D and E. */

		ia = 1;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    i__4 = ia;
		    d__[i__] = a[i__4].r;
		    i__4 = i__;
		    i__5 = ia + 1;
		    e[i__4].r = a[i__5].r, e[i__4].i = a[i__5].i;
		    ia += 2;
/* L20: */
		}
		if (n > 0) {
		    i__3 = ia;
		    d__[n] = a[i__3].r;
		}
	    } else {

/*              Type 7-12:  generate a diagonally dominant matrix with */
/*              unknown condition number in the vectors D and E. */

		if (! zerot || ! dotype[7]) {

/*                 Let E be complex, D real, with values from [-1,1]. */

		    dlarnv_(&c__2, iseed, &n, &d__[1]);
		    i__3 = n - 1;
		    zlarnv_(&c__2, iseed, &i__3, &e[1]);

/*                 Make the tridiagonal matrix diagonally dominant. */

		    if (n == 1) {
			d__[1] = abs(d__[1]);
		    } else {
			d__[1] = abs(d__[1]) + z_abs(&e[1]);
			d__[n] = (d__1 = d__[n], abs(d__1)) + z_abs(&e[n - 1])
				;
			i__3 = n - 1;
			for (i__ = 2; i__ <= i__3; ++i__) {
			    d__[i__] = (d__1 = d__[i__], abs(d__1)) + z_abs(&
				    e[i__]) + z_abs(&e[i__ - 1]);
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
		    zdscal_(&i__3, &d__1, &e[1], &c__1);

		} else if (izero > 0) {

/*                 Reuse the last matrix by copying back the zeroed out */
/*                 elements. */

		    if (izero == 1) {
			d__[1] = z__[1].r;
			if (n > 1) {
			    e[1].r = z__[2].r, e[1].i = z__[2].i;
			}
		    } else if (izero == n) {
			i__3 = n - 1;
			e[i__3].r = z__[0].r, e[i__3].i = z__[0].i;
			i__3 = n;
			d__[i__3] = z__[1].r;
		    } else {
			i__3 = izero - 1;
			e[i__3].r = z__[0].r, e[i__3].i = z__[0].i;
			i__3 = izero;
			d__[i__3] = z__[1].r;
			i__3 = izero;
			e[i__3].r = z__[2].r, e[i__3].i = z__[2].i;
		    }
		}

/*              For types 8-10, set one row and column of the matrix to */
/*              zero. */

		izero = 0;
		if (imat == 8) {
		    izero = 1;
		    z__[1].r = d__[1], z__[1].i = 0.;
		    d__[1] = 0.;
		    if (n > 1) {
			z__[2].r = e[1].r, z__[2].i = e[1].i;
			e[1].r = 0., e[1].i = 0.;
		    }
		} else if (imat == 9) {
		    izero = n;
		    if (n > 1) {
			i__3 = n - 1;
			z__[0].r = e[i__3].r, z__[0].i = e[i__3].i;
			i__3 = n - 1;
			e[i__3].r = 0., e[i__3].i = 0.;
		    }
		    i__3 = n;
		    z__[1].r = d__[i__3], z__[1].i = 0.;
		    d__[n] = 0.;
		} else if (imat == 10) {
		    izero = (n + 1) / 2;
		    if (izero > 1) {
			i__3 = izero - 1;
			z__[0].r = e[i__3].r, z__[0].i = e[i__3].i;
			i__3 = izero;
			z__[2].r = e[i__3].r, z__[2].i = e[i__3].i;
			i__3 = izero - 1;
			e[i__3].r = 0., e[i__3].i = 0.;
			i__3 = izero;
			e[i__3].r = 0., e[i__3].i = 0.;
		    }
		    i__3 = izero;
		    z__[1].r = d__[i__3], z__[1].i = 0.;
		    d__[izero] = 0.;
		}
	    }

	    dcopy_(&n, &d__[1], &c__1, &d__[n + 1], &c__1);
	    if (n > 1) {
		i__3 = n - 1;
		zcopy_(&i__3, &e[1], &c__1, &e[n + 1], &c__1);
	    }

/* +    TEST 1 */
/*           Factor A as L*D*L' and compute the ratio */
/*              norm(L*D*L' - A) / (n * norm(A) * EPS ) */

	    zpttrf_(&n, &d__[n + 1], &e[n + 1], &info);

/*           Check error code from ZPTTRF. */

	    if (info != izero) {
		alaerh_(path, "ZPTTRF", &info, &izero, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		goto L110;
	    }

	    if (info > 0) {
		rcondc = 0.;
		goto L100;
	    }

	    zptt01_(&n, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &work[1], 
		    result);

/*           Print the test ratio if greater than or equal to THRESH. */

	    if (result[0] >= *thresh) {
		if (nfail == 0 && nerrs == 0) {
		    alahd_(nout, path);
		}
		io___30.ciunit = *nout;
		s_wsfe(&io___30);
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

	    anorm = zlanht_("1", &n, &d__[1], &e[1]);

/*           Use ZPTTRS to solve for one column at a time of inv(A), */
/*           computing the maximum column sum as we go. */

	    ainvnm = 0.;
	    i__3 = n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		i__4 = n;
		for (j = 1; j <= i__4; ++j) {
		    i__5 = j;
		    x[i__5].r = 0., x[i__5].i = 0.;
/* L40: */
		}
		i__4 = i__;
		x[i__4].r = 1., x[i__4].i = 0.;
		zpttrs_("Lower", &n, &c__1, &d__[n + 1], &e[n + 1], &x[1], &
			lda, &info);
/* Computing MAX */
		d__1 = ainvnm, d__2 = dzasum_(&n, &x[1], &c__1);
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
		    zlarnv_(&c__2, iseed, &n, &xact[ix]);
		    ix += lda;
/* L60: */
		}

		for (iuplo = 1; iuplo <= 2; ++iuplo) {

/*              Do first for UPLO = 'U', then for UPLO = 'L'. */

		    *(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 
			    1];

/*              Set the right hand side. */

		    zlaptm_(uplo, &n, &nrhs, &c_b48, &d__[1], &e[1], &xact[1], 
			     &lda, &c_b49, &b[1], &lda);

/* +    TEST 2 */
/*              Solve A*x = b and compute the residual. */

		    zlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);
		    zpttrs_(uplo, &n, &nrhs, &d__[n + 1], &e[n + 1], &x[1], &
			    lda, &info);

/*              Check error code from ZPTTRS. */

		    if (info != 0) {
			alaerh_(path, "ZPTTRS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    zlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		    zptt02_(uplo, &n, &nrhs, &d__[1], &e[1], &x[1], &lda, &
			    work[1], &lda, &result[1]);

/* +    TEST 3 */
/*              Check solution from generated exact solution. */

		    zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[2]);

/* +    TESTS 4, 5, and 6 */
/*              Use iterative refinement to improve the solution. */

		    s_copy(srnamc_1.srnamt, "ZPTRFS", (ftnlen)6, (ftnlen)6);
		    zptrfs_(uplo, &n, &nrhs, &d__[1], &e[1], &d__[n + 1], &e[
			    n + 1], &b[1], &lda, &x[1], &lda, &rwork[1], &
			    rwork[nrhs + 1], &work[1], &rwork[(nrhs << 1) + 1]
, &info);

/*              Check error code from ZPTRFS. */

		    if (info != 0) {
			alaerh_(path, "ZPTRFS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[3]);
		    zptt05_(&n, &nrhs, &d__[1], &e[1], &b[1], &lda, &x[1], &
			    lda, &xact[1], &lda, &rwork[1], &rwork[nrhs + 1], 
			    &result[4]);

/*              Print information about the tests that did not pass the */
/*              threshold. */

		    for (k = 2; k <= 6; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___38.ciunit = *nout;
			    s_wsfe(&io___38);
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

/* +    TEST 7 */
/*           Estimate the reciprocal of the condition number of the */
/*           matrix. */

L100:
	    s_copy(srnamc_1.srnamt, "ZPTCON", (ftnlen)6, (ftnlen)6);
	    zptcon_(&n, &d__[n + 1], &e[n + 1], &anorm, &rcond, &rwork[1], &
		    info);

/*           Check error code from ZPTCON. */

	    if (info != 0) {
		alaerh_(path, "ZPTCON", &info, &c__0, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
	    }

	    result[6] = dget06_(&rcond, &rcondc);

/*           Print the test ratio if greater than or equal to THRESH. */

	    if (result[6] >= *thresh) {
		if (nfail == 0 && nerrs == 0) {
		    alahd_(nout, path);
		}
		io___40.ciunit = *nout;
		s_wsfe(&io___40);
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&result[6], (ftnlen)sizeof(doublereal));
		e_wsfe();
		++nfail;
	    }
	    ++nrun;
L110:
	    ;
	}
/* L120: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of ZCHKPT */

} /* zchkpt_ */
