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
static real c_b24 = 1.f;
static real c_b25 = 0.f;
static complex c_b62 = {0.f,0.f};

/* Subroutine */ int cdrvpt_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, real *thresh, logical *tsterr, complex *a, real *d__, 
	complex *e, complex *b, complex *x, complex *xact, complex *work, 
	real *rwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 0,0,0,1 };

    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002, N =\002,i5,\002, type \002,i2,"
	    "\002, test \002,i2,\002, ratio = \002,g12.5)";
    static char fmt_9998[] = "(1x,a6,\002, FACT='\002,a1,\002', N =\002,i5"
	    ",\002, type \002,i2,\002, test \002,i2,\002, ratio = \002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    real r__1, r__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double c_abs(complex *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n;
    real z__[3];
    integer k1, ia, in, kl, ku, ix, nt, lda;
    char fact[1];
    real cond;
    integer mode;
    real dmax__;
    integer imat, info;
    char path[3], dist[1], type__[1];
    integer nrun, ifact;
    extern /* Subroutine */ int cget04_(integer *, integer *, complex *, 
	    integer *, complex *, integer *, real *, real *);
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
    integer izero, nerrs;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), cptsv_(integer *, integer *, real *, complex *, 
	    complex *, integer *, integer *);
    logical zerot;
    extern /* Subroutine */ int clatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), aladhd_(integer *, char *), 
	    alaerh_(char *, char *, integer *, integer *, char *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *);
    real rcondc;
    extern doublereal clanht_(char *, integer *, real *, complex *);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *), clacpy_(char *, integer *, integer *, complex *, integer *, 
	    complex *, integer *), claset_(char *, integer *, integer 
	    *, complex *, complex *, complex *, integer *), claptm_(
	    char *, integer *, integer *, real *, real *, complex *, complex *
, integer *, real *, complex *, integer *);
    extern integer isamax_(integer *, real *, integer *);
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *), clarnv_(integer *, integer *, integer *, 
	    complex *), clatms_(integer *, integer *, char *, integer *, char 
	    *, real *, integer *, real *, real *, integer *, integer *, char *
, complex *, integer *, complex *, integer *);
    real ainvnm;
    extern doublereal scasum_(integer *, complex *, integer *);
    extern /* Subroutine */ int cpttrf_(integer *, real *, complex *, integer 
	    *), slarnv_(integer *, integer *, integer *, real *), cerrvx_(
	    char *, integer *);
    real result[6];
    extern /* Subroutine */ int cpttrs_(char *, integer *, integer *, real *, 
	    complex *, complex *, integer *, integer *), cptsvx_(char 
	    *, integer *, integer *, real *, complex *, real *, complex *, 
	    complex *, integer *, complex *, integer *, real *, real *, real *
, complex *, real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___35 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CDRVPT tests CPTSV and -SVX. */

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

/*  NRHS    (input) INTEGER */
/*          The number of right hand side vectors to be generated for */
/*          each linear system. */

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  A       (workspace) COMPLEX array, dimension (NMAX*2) */

/*  D       (workspace) REAL array, dimension (NMAX*2) */

/*  E       (workspace) COMPLEX array, dimension (NMAX*2) */

/*  B       (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  X       (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  XACT    (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  WORK    (workspace) COMPLEX array, dimension */
/*                      (NMAX*max(3,NRHS)) */

/*  RWORK   (workspace) REAL array, dimension (NMAX+2*NRHS) */

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
	cerrvx_(path, nout);
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

/*              Type 1-6:  generate a symmetric tridiagonal matrix of */
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
		    i__4 = i__;
		    i__5 = ia;
		    d__[i__4] = a[i__5].r;
		    i__4 = i__;
		    i__5 = ia + 1;
		    e[i__4].r = a[i__5].r, e[i__4].i = a[i__5].i;
		    ia += 2;
/* L20: */
		}
		if (n > 0) {
		    i__3 = n;
		    i__4 = ia;
		    d__[i__3] = a[i__4].r;
		}
	    } else {

/*              Type 7-12:  generate a diagonally dominant matrix with */
/*              unknown condition number in the vectors D and E. */

		if (! zerot || ! dotype[7]) {

/*                 Let D and E have values from [-1,1]. */

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
		    if (n > 1) {
			i__3 = n - 1;
			r__1 = anorm / dmax__;
			csscal_(&i__3, &r__1, &e[1], &c__1);
		    }

		} else if (izero > 0) {

/*                 Reuse the last matrix by copying back the zeroed out */
/*                 elements. */

		    if (izero == 1) {
			d__[1] = z__[1];
			if (n > 1) {
			    e[1].r = z__[2], e[1].i = 0.f;
			}
		    } else if (izero == n) {
			i__3 = n - 1;
			e[i__3].r = z__[0], e[i__3].i = 0.f;
			d__[n] = z__[1];
		    } else {
			i__3 = izero - 1;
			e[i__3].r = z__[0], e[i__3].i = 0.f;
			d__[izero] = z__[1];
			i__3 = izero;
			e[i__3].r = z__[2], e[i__3].i = 0.f;
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
			z__[2] = e[1].r;
			e[1].r = 0.f, e[1].i = 0.f;
		    }
		} else if (imat == 9) {
		    izero = n;
		    if (n > 1) {
			i__3 = n - 1;
			z__[0] = e[i__3].r;
			i__3 = n - 1;
			e[i__3].r = 0.f, e[i__3].i = 0.f;
		    }
		    z__[1] = d__[n];
		    d__[n] = 0.f;
		} else if (imat == 10) {
		    izero = (n + 1) / 2;
		    if (izero > 1) {
			i__3 = izero - 1;
			z__[0] = e[i__3].r;
			i__3 = izero - 1;
			e[i__3].r = 0.f, e[i__3].i = 0.f;
			i__3 = izero;
			z__[2] = e[i__3].r;
			i__3 = izero;
			e[i__3].r = 0.f, e[i__3].i = 0.f;
		    }
		    z__[1] = d__[izero];
		    d__[izero] = 0.f;
		}
	    }

/*           Generate NRHS random solution vectors. */

	    ix = 1;
	    i__3 = *nrhs;
	    for (j = 1; j <= i__3; ++j) {
		clarnv_(&c__2, iseed, &n, &xact[ix]);
		ix += lda;
/* L40: */
	    }

/*           Set the right hand side. */

	    claptm_("Lower", &n, nrhs, &c_b24, &d__[1], &e[1], &xact[1], &lda, 
		     &c_b25, &b[1], &lda);

	    for (ifact = 1; ifact <= 2; ++ifact) {
		if (ifact == 1) {
		    *(unsigned char *)fact = 'F';
		} else {
		    *(unsigned char *)fact = 'N';
		}

/*              Compute the condition number for comparison with */
/*              the value returned by CPTSVX. */

		if (zerot) {
		    if (ifact == 1) {
			goto L100;
		    }
		    rcondc = 0.f;

		} else if (ifact == 1) {

/*                 Compute the 1-norm of A. */

		    anorm = clanht_("1", &n, &d__[1], &e[1]);

		    scopy_(&n, &d__[1], &c__1, &d__[n + 1], &c__1);
		    if (n > 1) {
			i__3 = n - 1;
			ccopy_(&i__3, &e[1], &c__1, &e[n + 1], &c__1);
		    }

/*                 Factor the matrix A. */

		    cpttrf_(&n, &d__[n + 1], &e[n + 1], &info);

/*                 Use CPTTRS to solve for one column at a time of */
/*                 inv(A), computing the maximum column sum as we go. */

		    ainvnm = 0.f;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			i__4 = n;
			for (j = 1; j <= i__4; ++j) {
			    i__5 = j;
			    x[i__5].r = 0.f, x[i__5].i = 0.f;
/* L50: */
			}
			i__4 = i__;
			x[i__4].r = 1.f, x[i__4].i = 0.f;
			cpttrs_("Lower", &n, &c__1, &d__[n + 1], &e[n + 1], &
				x[1], &lda, &info);
/* Computing MAX */
			r__1 = ainvnm, r__2 = scasum_(&n, &x[1], &c__1);
			ainvnm = dmax(r__1,r__2);
/* L60: */
		    }

/*                 Compute the 1-norm condition number of A. */

		    if (anorm <= 0.f || ainvnm <= 0.f) {
			rcondc = 1.f;
		    } else {
			rcondc = 1.f / anorm / ainvnm;
		    }
		}

		if (ifact == 2) {

/*                 --- Test CPTSV -- */

		    scopy_(&n, &d__[1], &c__1, &d__[n + 1], &c__1);
		    if (n > 1) {
			i__3 = n - 1;
			ccopy_(&i__3, &e[1], &c__1, &e[n + 1], &c__1);
		    }
		    clacpy_("Full", &n, nrhs, &b[1], &lda, &x[1], &lda);

/*                 Factor A as L*D*L' and solve the system A*X = B. */

		    s_copy(srnamc_1.srnamt, "CPTSV ", (ftnlen)6, (ftnlen)6);
		    cptsv_(&n, nrhs, &d__[n + 1], &e[n + 1], &x[1], &lda, &
			    info);

/*                 Check error code from CPTSV . */

		    if (info != izero) {
			alaerh_(path, "CPTSV ", &info, &izero, " ", &n, &n, &
				c__1, &c__1, nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }
		    nt = 0;
		    if (izero == 0) {

/*                    Check the factorization by computing the ratio */
/*                       norm(L*D*L' - A) / (n * norm(A) * EPS ) */

			cptt01_(&n, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &
				work[1], result);

/*                    Compute the residual in the solution. */

			clacpy_("Full", &n, nrhs, &b[1], &lda, &work[1], &lda);
			cptt02_("Lower", &n, nrhs, &d__[1], &e[1], &x[1], &
				lda, &work[1], &lda, &result[1]);

/*                    Check solution from generated exact solution. */

			cget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[2]);
			nt = 3;
		    }

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    i__3 = nt;
		    for (k = 1; k <= i__3; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				aladhd_(nout, path);
			    }
			    io___35.ciunit = *nout;
			    s_wsfe(&io___35);
			    do_fio(&c__1, "CPTSV ", (ftnlen)6);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
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
		    nrun += nt;
		}

/*              --- Test CPTSVX --- */

		if (ifact > 1) {

/*                 Initialize D( N+1:2*N ) and E( N+1:2*N ) to zero. */

		    i__3 = n - 1;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			d__[n + i__] = 0.f;
			i__4 = n + i__;
			e[i__4].r = 0.f, e[i__4].i = 0.f;
/* L80: */
		    }
		    if (n > 0) {
			d__[n + n] = 0.f;
		    }
		}

		claset_("Full", &n, nrhs, &c_b62, &c_b62, &x[1], &lda);

/*              Solve the system and compute the condition number and */
/*              error bounds using CPTSVX. */

		s_copy(srnamc_1.srnamt, "CPTSVX", (ftnlen)6, (ftnlen)6);
		cptsvx_(fact, &n, nrhs, &d__[1], &e[1], &d__[n + 1], &e[n + 1]
, &b[1], &lda, &x[1], &lda, &rcond, &rwork[1], &rwork[
			*nrhs + 1], &work[1], &rwork[(*nrhs << 1) + 1], &info);

/*              Check the error code from CPTSVX. */

		if (info != izero) {
		    alaerh_(path, "CPTSVX", &info, &izero, fact, &n, &n, &
			    c__1, &c__1, nrhs, &imat, &nfail, &nerrs, nout);
		}
		if (izero == 0) {
		    if (ifact == 2) {

/*                    Check the factorization by computing the ratio */
/*                       norm(L*D*L' - A) / (n * norm(A) * EPS ) */

			k1 = 1;
			cptt01_(&n, &d__[1], &e[1], &d__[n + 1], &e[n + 1], &
				work[1], result);
		    } else {
			k1 = 2;
		    }

/*                 Compute the residual in the solution. */

		    clacpy_("Full", &n, nrhs, &b[1], &lda, &work[1], &lda);
		    cptt02_("Lower", &n, nrhs, &d__[1], &e[1], &x[1], &lda, &
			    work[1], &lda, &result[1]);

/*                 Check solution from generated exact solution. */

		    cget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[2]);

/*                 Check error bounds from iterative refinement. */

		    cptt05_(&n, nrhs, &d__[1], &e[1], &b[1], &lda, &x[1], &
			    lda, &xact[1], &lda, &rwork[1], &rwork[*nrhs + 1], 
			     &result[3]);
		} else {
		    k1 = 6;
		}

/*              Check the reciprocal of the condition number. */

		result[5] = sget06_(&rcond, &rcondc);

/*              Print information about the tests that did not pass */
/*              the threshold. */

		for (k = k1; k <= 6; ++k) {
		    if (result[k - 1] >= *thresh) {
			if (nfail == 0 && nerrs == 0) {
			    aladhd_(nout, path);
			}
			io___38.ciunit = *nout;
			s_wsfe(&io___38);
			do_fio(&c__1, "CPTSVX", (ftnlen)6);
			do_fio(&c__1, fact, (ftnlen)1);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[k - 1], (ftnlen)sizeof(
				real));
			e_wsfe();
			++nfail;
		    }
/* L90: */
		}
		nrun = nrun + 7 - k1;
L100:
		;
	    }
L110:
	    ;
	}
/* L120: */
    }

/*     Print a summary of the results. */

    alasvm_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of CDRVPT */

} /* cdrvpt_ */
