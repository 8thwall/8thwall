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
static real c_b43 = 1.f;
static real c_b44 = 0.f;
static complex c_b65 = {0.f,0.f};

/* Subroutine */ int cdrvgt_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, real *thresh, logical *tsterr, complex *a, complex *af, 
	 complex *b, complex *x, complex *xact, complex *work, real *rwork, 
	integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 0,0,0,1 };
    static char transs[1*3] = "N" "T" "C";

    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002, N =\002,i5,\002, type \002,i2,"
	    "\002, test \002,i2,\002, ratio = \002,g12.5)";
    static char fmt_9998[] = "(1x,a6,\002, FACT='\002,a1,\002', TRANS='\002,"
	    "a1,\002', N =\002,i5,\002, type \002,i2,\002, test \002,i2,\002,"
	    " ratio = \002,g12.5)";

    /* System generated locals */
    address a__1[2];
    integer i__1, i__2, i__3, i__4, i__5, i__6[2];
    real r__1, r__2;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, j, k, m, n;
    real z__[3];
    integer k1, in, kl, ku, ix, nt, lda;
    char fact[1];
    real cond;
    integer mode, koff, imat, info;
    char path[3], dist[1], type__[1];
    integer nrun, ifact;
    extern /* Subroutine */ int cget04_(integer *, integer *, complex *, 
	    integer *, complex *, integer *, real *, real *);
    integer nfail, iseed[4];
    extern /* Subroutine */ int cgtt01_(integer *, complex *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, integer *, 
	    complex *, integer *, real *, real *), cgtt02_(char *, integer *, 
	    integer *, complex *, complex *, complex *, complex *, integer *, 
	    complex *, integer *, real *, real *);
    real rcond;
    extern /* Subroutine */ int cgtt05_(char *, integer *, integer *, complex 
	    *, complex *, complex *, complex *, integer *, complex *, integer 
	    *, complex *, integer *, real *, real *, real *);
    integer nimat;
    extern doublereal sget06_(real *, real *);
    real anorm;
    integer itran;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *), cgtsv_(integer *, integer *, complex *, 
	    complex *, complex *, complex *, integer *, integer *);
    char trans[1];
    integer izero, nerrs;
    logical zerot;
    extern /* Subroutine */ int clatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), aladhd_(integer *, char *), 
	    alaerh_(char *, char *, integer *, integer *, char *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *), clagtm_(char *, 
	    integer *, integer *, real *, complex *, complex *, complex *, 
	    complex *, integer *, real *, complex *, integer *);
    real rcondc;
    extern doublereal clangt_(char *, integer *, complex *, complex *, 
	    complex *);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *), clacpy_(char *, integer *, integer *, complex *, integer *, 
	    complex *, integer *), claset_(char *, integer *, integer 
	    *, complex *, complex *, complex *, integer *);
    real rcondi;
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *);
    real rcondo, anormi;
    extern /* Subroutine */ int clarnv_(integer *, integer *, integer *, 
	    complex *), clatms_(integer *, integer *, char *, integer *, char 
	    *, real *, integer *, real *, real *, integer *, integer *, char *
, complex *, integer *, complex *, integer *);
    real ainvnm;
    extern /* Subroutine */ int cgttrf_(integer *, complex *, complex *, 
	    complex *, complex *, integer *, integer *);
    logical trfcon;
    real anormo;
    extern doublereal scasum_(integer *, complex *, integer *);
    extern /* Subroutine */ int cgttrs_(char *, integer *, integer *, complex 
	    *, complex *, complex *, complex *, integer *, complex *, integer 
	    *, integer *), cerrvx_(char *, integer *);
    real result[6];
    extern /* Subroutine */ int cgtsvx_(char *, char *, integer *, integer *, 
	    complex *, complex *, complex *, complex *, complex *, complex *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    real *, real *, real *, complex *, real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___42 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CDRVGT tests CGTSV and -SVX. */

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

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  A       (workspace) COMPLEX array, dimension (NMAX*4) */

/*  AF      (workspace) COMPLEX array, dimension (NMAX*4) */

/*  B       (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  X       (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  XACT    (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  WORK    (workspace) COMPLEX array, dimension */
/*                      (NMAX*max(3,NRHS)) */

/*  RWORK   (workspace) REAL array, dimension (NMAX+2*NRHS) */

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
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
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
	cerrvx_(path, nout);
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
		goto L130;
	    }

/*           Set up parameters with CLATB4. */

	    clatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cond, dist);

	    zerot = imat >= 8 && imat <= 10;
	    if (imat <= 6) {

/*              Types 1-6:  generate matrices of known condition number. */

/* Computing MAX */
		i__3 = 2 - ku, i__4 = 3 - max(1,n);
		koff = max(i__3,i__4);
		s_copy(srnamc_1.srnamt, "CLATMS", (ftnlen)6, (ftnlen)6);
		clatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cond, 
			&anorm, &kl, &ku, "Z", &af[koff], &c__3, &work[1], &
			info);

/*              Check the error code from CLATMS. */

		if (info != 0) {
		    alaerh_(path, "CLATMS", &info, &c__0, " ", &n, &n, &kl, &
			    ku, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L130;
		}
		izero = 0;

		if (n > 1) {
		    i__3 = n - 1;
		    ccopy_(&i__3, &af[4], &c__3, &a[1], &c__1);
		    i__3 = n - 1;
		    ccopy_(&i__3, &af[3], &c__3, &a[n + m + 1], &c__1);
		}
		ccopy_(&n, &af[2], &c__3, &a[m + 1], &c__1);
	    } else {

/*              Types 7-12:  generate tridiagonal matrices with */
/*              unknown condition numbers. */

		if (! zerot || ! dotype[7]) {

/*                 Generate a matrix with elements from [-1,1]. */

		    i__3 = n + (m << 1);
		    clarnv_(&c__2, iseed, &i__3, &a[1]);
		    if (anorm != 1.f) {
			i__3 = n + (m << 1);
			csscal_(&i__3, &anorm, &a[1], &c__1);
		    }
		} else if (izero > 0) {

/*                 Reuse the last matrix by copying back the zeroed out */
/*                 elements. */

		    if (izero == 1) {
			i__3 = n;
			a[i__3].r = z__[1], a[i__3].i = 0.f;
			if (n > 1) {
			    a[1].r = z__[2], a[1].i = 0.f;
			}
		    } else if (izero == n) {
			i__3 = n * 3 - 2;
			a[i__3].r = z__[0], a[i__3].i = 0.f;
			i__3 = (n << 1) - 1;
			a[i__3].r = z__[1], a[i__3].i = 0.f;
		    } else {
			i__3 = (n << 1) - 2 + izero;
			a[i__3].r = z__[0], a[i__3].i = 0.f;
			i__3 = n - 1 + izero;
			a[i__3].r = z__[1], a[i__3].i = 0.f;
			i__3 = izero;
			a[i__3].r = z__[2], a[i__3].i = 0.f;
		    }
		}

/*              If IMAT > 7, set one column of the matrix to 0. */

		if (! zerot) {
		    izero = 0;
		} else if (imat == 8) {
		    izero = 1;
		    i__3 = n;
		    z__[1] = a[i__3].r;
		    i__3 = n;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
		    if (n > 1) {
			z__[2] = a[1].r;
			a[1].r = 0.f, a[1].i = 0.f;
		    }
		} else if (imat == 9) {
		    izero = n;
		    i__3 = n * 3 - 2;
		    z__[0] = a[i__3].r;
		    i__3 = (n << 1) - 1;
		    z__[1] = a[i__3].r;
		    i__3 = n * 3 - 2;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
		    i__3 = (n << 1) - 1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
		} else {
		    izero = (n + 1) / 2;
		    i__3 = n - 1;
		    for (i__ = izero; i__ <= i__3; ++i__) {
			i__4 = (n << 1) - 2 + i__;
			a[i__4].r = 0.f, a[i__4].i = 0.f;
			i__4 = n - 1 + i__;
			a[i__4].r = 0.f, a[i__4].i = 0.f;
			i__4 = i__;
			a[i__4].r = 0.f, a[i__4].i = 0.f;
/* L20: */
		    }
		    i__3 = n * 3 - 2;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
		    i__3 = (n << 1) - 1;
		    a[i__3].r = 0.f, a[i__3].i = 0.f;
		}
	    }

	    for (ifact = 1; ifact <= 2; ++ifact) {
		if (ifact == 1) {
		    *(unsigned char *)fact = 'F';
		} else {
		    *(unsigned char *)fact = 'N';
		}

/*              Compute the condition number for comparison with */
/*              the value returned by CGTSVX. */

		if (zerot) {
		    if (ifact == 1) {
			goto L120;
		    }
		    rcondo = 0.f;
		    rcondi = 0.f;

		} else if (ifact == 1) {
		    i__3 = n + (m << 1);
		    ccopy_(&i__3, &a[1], &c__1, &af[1], &c__1);

/*                 Compute the 1-norm and infinity-norm of A. */

		    anormo = clangt_("1", &n, &a[1], &a[m + 1], &a[n + m + 1]);
		    anormi = clangt_("I", &n, &a[1], &a[m + 1], &a[n + m + 1]);

/*                 Factor the matrix A. */

		    cgttrf_(&n, &af[1], &af[m + 1], &af[n + m + 1], &af[n + (
			    m << 1) + 1], &iwork[1], &info);

/*                 Use CGTTRS to solve for one column at a time of */
/*                 inv(A), computing the maximum column sum as we go. */

		    ainvnm = 0.f;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			i__4 = n;
			for (j = 1; j <= i__4; ++j) {
			    i__5 = j;
			    x[i__5].r = 0.f, x[i__5].i = 0.f;
/* L30: */
			}
			i__4 = i__;
			x[i__4].r = 1.f, x[i__4].i = 0.f;
			cgttrs_("No transpose", &n, &c__1, &af[1], &af[m + 1], 
				 &af[n + m + 1], &af[n + (m << 1) + 1], &
				iwork[1], &x[1], &lda, &info);
/* Computing MAX */
			r__1 = ainvnm, r__2 = scasum_(&n, &x[1], &c__1);
			ainvnm = dmax(r__1,r__2);
/* L40: */
		    }

/*                 Compute the 1-norm condition number of A. */

		    if (anormo <= 0.f || ainvnm <= 0.f) {
			rcondo = 1.f;
		    } else {
			rcondo = 1.f / anormo / ainvnm;
		    }

/*                 Use CGTTRS to solve for one column at a time of */
/*                 inv(A'), computing the maximum column sum as we go. */

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
			cgttrs_("Conjugate transpose", &n, &c__1, &af[1], &af[
				m + 1], &af[n + m + 1], &af[n + (m << 1) + 1], 
				 &iwork[1], &x[1], &lda, &info);
/* Computing MAX */
			r__1 = ainvnm, r__2 = scasum_(&n, &x[1], &c__1);
			ainvnm = dmax(r__1,r__2);
/* L60: */
		    }

/*                 Compute the infinity-norm condition number of A. */

		    if (anormi <= 0.f || ainvnm <= 0.f) {
			rcondi = 1.f;
		    } else {
			rcondi = 1.f / anormi / ainvnm;
		    }
		}

		for (itran = 1; itran <= 3; ++itran) {
		    *(unsigned char *)trans = *(unsigned char *)&transs[itran 
			    - 1];
		    if (itran == 1) {
			rcondc = rcondo;
		    } else {
			rcondc = rcondi;
		    }

/*                 Generate NRHS random solution vectors. */

		    ix = 1;
		    i__3 = *nrhs;
		    for (j = 1; j <= i__3; ++j) {
			clarnv_(&c__2, iseed, &n, &xact[ix]);
			ix += lda;
/* L70: */
		    }

/*                 Set the right hand side. */

		    clagtm_(trans, &n, nrhs, &c_b43, &a[1], &a[m + 1], &a[n + 
			    m + 1], &xact[1], &lda, &c_b44, &b[1], &lda);

		    if (ifact == 2 && itran == 1) {

/*                    --- Test CGTSV  --- */

/*                    Solve the system using Gaussian elimination with */
/*                    partial pivoting. */

			i__3 = n + (m << 1);
			ccopy_(&i__3, &a[1], &c__1, &af[1], &c__1);
			clacpy_("Full", &n, nrhs, &b[1], &lda, &x[1], &lda);

			s_copy(srnamc_1.srnamt, "CGTSV ", (ftnlen)6, (ftnlen)
				6);
			cgtsv_(&n, nrhs, &af[1], &af[m + 1], &af[n + m + 1], &
				x[1], &lda, &info);

/*                    Check error code from CGTSV . */

			if (info != izero) {
			    alaerh_(path, "CGTSV ", &info, &izero, " ", &n, &
				    n, &c__1, &c__1, nrhs, &imat, &nfail, &
				    nerrs, nout);
			}
			nt = 1;
			if (izero == 0) {

/*                       Check residual of computed solution. */

			    clacpy_("Full", &n, nrhs, &b[1], &lda, &work[1], &
				    lda);
			    cgtt02_(trans, &n, nrhs, &a[1], &a[m + 1], &a[n + 
				    m + 1], &x[1], &lda, &work[1], &lda, &
				    rwork[1], &result[1]);

/*                       Check solution from generated exact solution. */

			    cget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[2]);
			    nt = 3;
			}

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			i__3 = nt;
			for (k = 2; k <= i__3; ++k) {
			    if (result[k - 1] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    aladhd_(nout, path);
				}
				io___42.ciunit = *nout;
				s_wsfe(&io___42);
				do_fio(&c__1, "CGTSV ", (ftnlen)6);
				do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
					sizeof(real));
				e_wsfe();
				++nfail;
			    }
/* L80: */
			}
			nrun = nrun + nt - 1;
		    }

/*                 --- Test CGTSVX --- */

		    if (ifact > 1) {

/*                    Initialize AF to zero. */

			i__3 = n * 3 - 2;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    i__4 = i__;
			    af[i__4].r = 0.f, af[i__4].i = 0.f;
/* L90: */
			}
		    }
		    claset_("Full", &n, nrhs, &c_b65, &c_b65, &x[1], &lda);

/*                 Solve the system and compute the condition number and */
/*                 error bounds using CGTSVX. */

		    s_copy(srnamc_1.srnamt, "CGTSVX", (ftnlen)6, (ftnlen)6);
		    cgtsvx_(fact, trans, &n, nrhs, &a[1], &a[m + 1], &a[n + m 
			    + 1], &af[1], &af[m + 1], &af[n + m + 1], &af[n + 
			    (m << 1) + 1], &iwork[1], &b[1], &lda, &x[1], &
			    lda, &rcond, &rwork[1], &rwork[*nrhs + 1], &work[
			    1], &rwork[(*nrhs << 1) + 1], &info);

/*                 Check the error code from CGTSVX. */

		    if (info != izero) {
/* Writing concatenation */
			i__6[0] = 1, a__1[0] = fact;
			i__6[1] = 1, a__1[1] = trans;
			s_cat(ch__1, a__1, i__6, &c__2, (ftnlen)2);
			alaerh_(path, "CGTSVX", &info, &izero, ch__1, &n, &n, 
				&c__1, &c__1, nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    if (ifact >= 2) {

/*                    Reconstruct matrix from factors and compute */
/*                    residual. */

			cgtt01_(&n, &a[1], &a[m + 1], &a[n + m + 1], &af[1], &
				af[m + 1], &af[n + m + 1], &af[n + (m << 1) + 
				1], &iwork[1], &work[1], &lda, &rwork[1], 
				result);
			k1 = 1;
		    } else {
			k1 = 2;
		    }

		    if (info == 0) {
			trfcon = FALSE_;

/*                    Check residual of computed solution. */

			clacpy_("Full", &n, nrhs, &b[1], &lda, &work[1], &lda);
			cgtt02_(trans, &n, nrhs, &a[1], &a[m + 1], &a[n + m + 
				1], &x[1], &lda, &work[1], &lda, &rwork[1], &
				result[1]);

/*                    Check solution from generated exact solution. */

			cget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[2]);

/*                    Check the error bounds from iterative refinement. */

			cgtt05_(trans, &n, nrhs, &a[1], &a[m + 1], &a[n + m + 
				1], &b[1], &lda, &x[1], &lda, &xact[1], &lda, 
				&rwork[1], &rwork[*nrhs + 1], &result[3]);
			nt = 5;
		    }

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    i__3 = nt;
		    for (k = k1; k <= i__3; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				aladhd_(nout, path);
			    }
			    io___46.ciunit = *nout;
			    s_wsfe(&io___46);
			    do_fio(&c__1, "CGTSVX", (ftnlen)6);
			    do_fio(&c__1, fact, (ftnlen)1);
			    do_fio(&c__1, trans, (ftnlen)1);
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
/* L100: */
		    }

/*                 Check the reciprocal of the condition number. */

		    result[5] = sget06_(&rcond, &rcondc);
		    if (result[5] >= *thresh) {
			if (nfail == 0 && nerrs == 0) {
			    aladhd_(nout, path);
			}
			io___47.ciunit = *nout;
			s_wsfe(&io___47);
			do_fio(&c__1, "CGTSVX", (ftnlen)6);
			do_fio(&c__1, fact, (ftnlen)1);
			do_fio(&c__1, trans, (ftnlen)1);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[k - 1], (ftnlen)sizeof(
				real));
			e_wsfe();
			++nfail;
		    }
		    nrun = nrun + nt - k1 + 2;

/* L110: */
		}
L120:
		;
	    }
L130:
	    ;
	}
/* L140: */
    }

/*     Print a summary of the results. */

    alasvm_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of CDRVGT */

} /* cdrvgt_ */
