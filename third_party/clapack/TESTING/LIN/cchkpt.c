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
static real c_b48 = 1.f;
static real c_b49 = 0.f;
static integer c__7 = 7;

/* Subroutine */ int cchkpt_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, real *thresh, logical *tsterr, complex *
	a, real *d__, complex *e, complex *b, complex *x, complex *xact, 
	complex *work, real *rwork, integer *nout)
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
    real r__1, r__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double c_abs(complex *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n;
    complex z__[3];
    integer ia, in, kl, ku, ix, lda;
    real cond;
    integer mode;
    real dmax__;
    integer imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char uplo[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), cget04_(
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    real *, real *);
    integer nfail, iseed[4];
    real rcond;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    integer nimat;
    extern doublereal sget06_(real *, real *);
    extern /* Subroutine */ int cptt01_(integer *, real *, complex *, real *, 
	    complex *, complex *, real *);
    real anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), cptt02_(char *, integer *, integer *, real 
	    *, complex *, complex *, integer *, complex *, integer *, real *), cptt05_(integer *, integer *, real *, complex *, complex 
	    *, integer *, complex *, integer *, complex *, integer *, real *, 
	    real *, real *);
    integer iuplo, izero, nerrs;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    logical zerot;
    extern /* Subroutine */ int clatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), alaerh_(char *, char *, integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    real rcondc;
    extern doublereal clanht_(char *, integer *, real *, complex *);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *), clacpy_(char *, integer *, integer *, complex *, integer *, 
	    complex *, integer *), claptm_(char *, integer *, integer 
	    *, real *, real *, complex *, complex *, integer *, real *, 
	    complex *, integer *);
    extern integer isamax_(integer *, real *, integer *);
    extern /* Subroutine */ int alasum_(char *, integer *, integer *, integer 
	    *, integer *), clarnv_(integer *, integer *, integer *, 
	    complex *), cerrgt_(char *, integer *), clatms_(integer *, 
	     integer *, char *, integer *, char *, real *, integer *, real *, 
	    real *, integer *, integer *, char *, complex *, integer *, 
	    complex *, integer *);
    real ainvnm;
    extern /* Subroutine */ int cptcon_(integer *, real *, complex *, real *, 
	    real *, real *, integer *);
    extern doublereal scasum_(integer *, complex *, integer *);
    extern /* Subroutine */ int cptrfs_(char *, integer *, integer *, real *, 
	    complex *, real *, complex *, complex *, integer *, complex *, 
	    integer *, real *, real *, complex *, real *, integer *), 
	    cpttrf_(integer *, real *, complex *, integer *), slarnv_(integer 
	    *, integer *, integer *, real *);
    real result[7];
    extern /* Subroutine */ int cpttrs_(char *, integer *, integer *, real *, 
	    complex *, complex *, integer *, integer *);

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

/*  CCHKPT tests CPTTRF, -TRS, -RFS, and -CON */

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

/*  A       (workspace) COMPLEX array, dimension (NMAX*2) */

/*  D       (workspace) REAL array, dimension (NMAX*2) */

/*  E       (workspace) COMPLEX array, dimension (NMAX*2) */

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

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
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
	cerrgt_(path, nout);
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

/*           Set up parameters with CLATB4. */

	    clatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cond, dist);

	    zerot = imat >= 8 && imat <= 10;
	    if (imat <= 6) {

/*              Type 1-6:  generate a Hermitian tridiagonal matrix of */
/*              known condition number in lower triangular band storage. */

		s_copy(srnamc_1.srnamt, "CLATMS", (ftnlen)6, (ftnlen)6);
		clatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cond, 
			&anorm, &kl, &ku, "B", &a[1], &c__2, &work[1], &info);

/*              Check the error code from CLATMS. */

		if (info != 0) {
		    alaerh_(path, "CLATMS", &info, &c__0, " ", &n, &n, &kl, &
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

		    slarnv_(&c__2, iseed, &n, &d__[1]);
		    i__3 = n - 1;
		    clarnv_(&c__2, iseed, &i__3, &e[1]);

/*                 Make the tridiagonal matrix diagonally dominant. */

		    if (n == 1) {
			d__[1] = dabs(d__[1]);
		    } else {
			d__[1] = dabs(d__[1]) + c_abs(&e[1]);
			d__[n] = (r__1 = d__[n], dabs(r__1)) + c_abs(&e[n - 1]
				);
			i__3 = n - 1;
			for (i__ = 2; i__ <= i__3; ++i__) {
			    d__[i__] = (r__1 = d__[i__], dabs(r__1)) + c_abs(&
				    e[i__]) + c_abs(&e[i__ - 1]);
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
		    csscal_(&i__3, &r__1, &e[1], &c__1);

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
		    z__[1].r = d__[1], z__[1].i = 0.f;
		    d__[1] = 0.f;
		    if (n > 1) {
			z__[2].r = e[1].r, z__[2].i = e[1].i;
			e[1].r = 0.f, e[1].i = 0.f;
		    }
		} else if (imat == 9) {
		    izero = n;
		    if (n > 1) {
			i__3 = n - 1;
			z__[0].r = e[i__3].r, z__[0].i = e[i__3].i;
			i__3 = n - 1;
			e[i__3].r = 0.f, e[i__3].i = 0.f;
		    }
		    i__3 = n;
		    z__[1].r = d__[i__3], z__[1].i = 0.f;
		    d__[n] = 0.f;
		} else if (imat == 10) {
		    izero = (n + 1) / 2;
		    if (izero > 1) {
			i__3 = izero - 1;
			z__[0].r = e[i__3].r, z__[0].i = e[i__3].i;
			i__3 = izero;
			z__[2].r = e[i__3].r, z__[2].i = e[i__3].i;
			i__3 = izero - 1;
			e[i__3].r = 0.f, e[i__3].i = 0.f;
			i__3 = izero;
			e[i__3].r = 0.f, e[i__3].i = 0.f;
		    }
		    i__3 = izero;
		    z__[1].r = d__[i__3], z__[1].i = 0.f;
		    d__[izero] = 0.f;
		}
	    }

	    scopy_(&n, &d__[1], &c__1, &d__[n + 1], &c__1);
	    if (n > 1) {
		i__3 = n - 1;
		ccopy_(&i__3, &e[1], &c__1, &e[n + 1], &c__1);
	    }

/* +    TEST 1 */
/*           Factor A as L*D*L' and compute the ratio */
/*              norm(L*D*L' - A) / (n * norm(A) * EPS ) */

	    cpttrf_(&n, &d__[n + 1], &e[n + 1], &info);

/*           Check error code from CPTTRF. */

	    if (info != izero) {
		alaerh_(path, "CPTTRF", &info, &izero, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		goto L110;
	    }

	    if (info > 0) {
		rcondc = 0.f;
		goto L100;
	    }

	    cptt01_(&n, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &work[1], 
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
		do_fio(&c__1, (char *)&result[0], (ftnlen)sizeof(real));
		e_wsfe();
		++nfail;
	    }
	    ++nrun;

/*           Compute RCONDC = 1 / (norm(A) * norm(inv(A)) */

/*           Compute norm(A). */

	    anorm = clanht_("1", &n, &d__[1], &e[1]);

/*           Use CPTTRS to solve for one column at a time of inv(A), */
/*           computing the maximum column sum as we go. */

	    ainvnm = 0.f;
	    i__3 = n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		i__4 = n;
		for (j = 1; j <= i__4; ++j) {
		    i__5 = j;
		    x[i__5].r = 0.f, x[i__5].i = 0.f;
/* L40: */
		}
		i__4 = i__;
		x[i__4].r = 1.f, x[i__4].i = 0.f;
		cpttrs_("Lower", &n, &c__1, &d__[n + 1], &e[n + 1], &x[1], &
			lda, &info);
/* Computing MAX */
		r__1 = ainvnm, r__2 = scasum_(&n, &x[1], &c__1);
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
		    clarnv_(&c__2, iseed, &n, &xact[ix]);
		    ix += lda;
/* L60: */
		}

		for (iuplo = 1; iuplo <= 2; ++iuplo) {

/*              Do first for UPLO = 'U', then for UPLO = 'L'. */

		    *(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 
			    1];

/*              Set the right hand side. */

		    claptm_(uplo, &n, &nrhs, &c_b48, &d__[1], &e[1], &xact[1], 
			     &lda, &c_b49, &b[1], &lda);

/* +    TEST 2 */
/*              Solve A*x = b and compute the residual. */

		    clacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);
		    cpttrs_(uplo, &n, &nrhs, &d__[n + 1], &e[n + 1], &x[1], &
			    lda, &info);

/*              Check error code from CPTTRS. */

		    if (info != 0) {
			alaerh_(path, "CPTTRS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    clacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		    cptt02_(uplo, &n, &nrhs, &d__[1], &e[1], &x[1], &lda, &
			    work[1], &lda, &result[1]);

/* +    TEST 3 */
/*              Check solution from generated exact solution. */

		    cget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[2]);

/* +    TESTS 4, 5, and 6 */
/*              Use iterative refinement to improve the solution. */

		    s_copy(srnamc_1.srnamt, "CPTRFS", (ftnlen)6, (ftnlen)6);
		    cptrfs_(uplo, &n, &nrhs, &d__[1], &e[1], &d__[n + 1], &e[
			    n + 1], &b[1], &lda, &x[1], &lda, &rwork[1], &
			    rwork[nrhs + 1], &work[1], &rwork[(nrhs << 1) + 1]
, &info);

/*              Check error code from CPTRFS. */

		    if (info != 0) {
			alaerh_(path, "CPTRFS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    cget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[3]);
		    cptt05_(&n, &nrhs, &d__[1], &e[1], &b[1], &lda, &x[1], &
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
				    sizeof(real));
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
	    s_copy(srnamc_1.srnamt, "CPTCON", (ftnlen)6, (ftnlen)6);
	    cptcon_(&n, &d__[n + 1], &e[n + 1], &anorm, &rcond, &rwork[1], &
		    info);

/*           Check error code from CPTCON. */

	    if (info != 0) {
		alaerh_(path, "CPTCON", &info, &c__0, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
	    }

	    result[6] = sget06_(&rcond, &rcondc);

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
		do_fio(&c__1, (char *)&result[6], (ftnlen)sizeof(real));
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

/*     End of CCHKPT */

} /* cchkpt_ */
