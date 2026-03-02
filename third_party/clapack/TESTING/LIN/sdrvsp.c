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
static real c_b59 = 0.f;
static integer c__2 = 2;

/* Subroutine */ int sdrvsp_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, real *thresh, logical *tsterr, integer *nmax, real *a, 
	real *afac, real *ainv, real *b, real *x, real *xact, real *work, 
	real *rwork, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char facts[1*2] = "F" "N";

    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002, UPLO='\002,a1,\002', N =\002,i5"
	    ",\002, type \002,i2,\002, test \002,i2,\002, ratio =\002,g12.5)";
    static char fmt_9998[] = "(1x,a6,\002, FACT='\002,a1,\002', UPLO='\002,a"
	    "1,\002', N =\002,i5,\002, type \002,i2,\002, test \002,i2,\002, "
	    "ratio =\002,g12.5)";

    /* System generated locals */
    address a__1[2];
    integer i__1, i__2, i__3, i__4, i__5[2];
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, j, k, n, i1, i2, k1, in, kl, ku, nt, lda, npp;
    char fact[1];
    integer ioff, mode, imat, info;
    char path[3], dist[1], uplo[1], type__[1];
    integer nrun, ifact, nfail, iseed[4];
    real rcond;
    extern /* Subroutine */ int sget04_(integer *, integer *, real *, integer 
	    *, real *, integer *, real *, real *);
    integer nimat;
    extern doublereal sget06_(real *, real *);
    real anorm;
    integer iuplo, izero, nerrs;
    extern /* Subroutine */ int sppt02_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, real *), 
	    scopy_(integer *, real *, integer *, real *, integer *);
    integer lwork;
    extern /* Subroutine */ int sppt05_(char *, integer *, integer *, real *, 
	    real *, integer *, real *, integer *, real *, integer *, real *, 
	    real *, real *), sspt01_(char *, integer *, real *, real *
, integer *, real *, integer *, real *, real *);
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int sspsv_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *, integer *), slatb4_(char *, 
	    integer *, integer *, integer *, char *, integer *, integer *, 
	    real *, integer *, real *, char *), 
	    aladhd_(integer *, char *), alaerh_(char *, char *, 
	    integer *, integer *, char *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *);
    real rcondc;
    char packit[1];
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *);
    real cndnum, ainvnm;
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slarhs_(char *, char *, 
	    char *, char *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, integer *, integer *), slaset_(
	    char *, integer *, integer *, real *, real *, real *, integer *);
    extern doublereal slansp_(char *, char *, integer *, real *, real *);
    extern /* Subroutine */ int slatms_(integer *, integer *, char *, integer 
	    *, char *, real *, integer *, real *, real *, integer *, integer *
, char *, real *, integer *, real *, integer *);
    real result[6];
    extern /* Subroutine */ int ssptrf_(char *, integer *, real *, integer *, 
	    integer *), ssptri_(char *, integer *, real *, integer *, 
	    real *, integer *), serrvx_(char *, integer *), 
	    sspsvx_(char *, char *, integer *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *, real *, real *, 
	    real *, real *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___41 = { 0, 0, 0, fmt_9999, 0 };
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

/*  SDRVSP tests the driver routines SSPSV and -SVX. */

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

/*  NMAX    (input) INTEGER */
/*          The maximum value permitted for N, used in dimensioning the */
/*          work arrays. */

/*  A       (workspace) REAL array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  AFAC    (workspace) REAL array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  AINV    (workspace) REAL array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  B       (workspace) REAL array, dimension (NMAX*NRHS) */

/*  X       (workspace) REAL array, dimension (NMAX*NRHS) */

/*  XACT    (workspace) REAL array, dimension (NMAX*NRHS) */

/*  WORK    (workspace) REAL array, dimension */
/*                      (NMAX*max(2,NRHS)) */

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
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Single precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "SP", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }
/* Computing MAX */
    i__1 = *nmax << 1, i__2 = *nmax * *nrhs;
    lwork = max(i__1,i__2);

/*     Test the error exits */

    if (*tsterr) {
	serrvx_(path, nout);
    }
    infoc_1.infot = 0;

/*     Do for each value of N in NVAL */

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {
	n = nval[in];
	lda = max(n,1);
	npp = n * (n + 1) / 2;
	*(unsigned char *)xtype = 'N';
	nimat = 10;
	if (n <= 0) {
	    nimat = 1;
	}

	i__2 = nimat;
	for (imat = 1; imat <= i__2; ++imat) {

/*           Do the tests only if DOTYPE( IMAT ) is true. */

	    if (! dotype[imat]) {
		goto L170;
	    }

/*           Skip types 3, 4, 5, or 6 if the matrix size is too small. */

	    zerot = imat >= 3 && imat <= 6;
	    if (zerot && n < imat - 2) {
		goto L170;
	    }

/*           Do first for UPLO = 'U', then for UPLO = 'L' */

	    for (iuplo = 1; iuplo <= 2; ++iuplo) {
		if (iuplo == 1) {
		    *(unsigned char *)uplo = 'U';
		    *(unsigned char *)packit = 'C';
		} else {
		    *(unsigned char *)uplo = 'L';
		    *(unsigned char *)packit = 'R';
		}

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
		    goto L160;
		}

/*              For types 3-6, zero one or more rows and columns of the */
/*              matrix to test that INFO is returned correctly. */

		if (zerot) {
		    if (imat == 3) {
			izero = 1;
		    } else if (imat == 4) {
			izero = n;
		    } else {
			izero = n / 2 + 1;
		    }

		    if (imat < 6) {

/*                    Set row and column IZERO to zero. */

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
			ioff = 0;
			if (iuplo == 1) {

/*                       Set the first IZERO rows and columns to zero. */

			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
				i2 = min(j,izero);
				i__4 = i2;
				for (i__ = 1; i__ <= i__4; ++i__) {
				    a[ioff + i__] = 0.f;
/* L60: */
				}
				ioff += j;
/* L70: */
			    }
			} else {

/*                       Set the last IZERO rows and columns to zero. */

			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
				i1 = max(j,izero);
				i__4 = n;
				for (i__ = i1; i__ <= i__4; ++i__) {
				    a[ioff + i__] = 0.f;
/* L80: */
				}
				ioff = ioff + n - j;
/* L90: */
			    }
			}
		    }
		} else {
		    izero = 0;
		}

		for (ifact = 1; ifact <= 2; ++ifact) {

/*                 Do first for FACT = 'F', then for other values. */

		    *(unsigned char *)fact = *(unsigned char *)&facts[ifact - 
			    1];

/*                 Compute the condition number for comparison with */
/*                 the value returned by SSPSVX. */

		    if (zerot) {
			if (ifact == 1) {
			    goto L150;
			}
			rcondc = 0.f;

		    } else if (ifact == 1) {

/*                    Compute the 1-norm of A. */

			anorm = slansp_("1", uplo, &n, &a[1], &rwork[1]);

/*                    Factor the matrix A. */

			scopy_(&npp, &a[1], &c__1, &afac[1], &c__1);
			ssptrf_(uplo, &n, &afac[1], &iwork[1], &info);

/*                    Compute inv(A) and take its norm. */

			scopy_(&npp, &afac[1], &c__1, &ainv[1], &c__1);
			ssptri_(uplo, &n, &ainv[1], &iwork[1], &work[1], &
				info);
			ainvnm = slansp_("1", uplo, &n, &ainv[1], &rwork[1]);

/*                    Compute the 1-norm condition number of A. */

			if (anorm <= 0.f || ainvnm <= 0.f) {
			    rcondc = 1.f;
			} else {
			    rcondc = 1.f / anorm / ainvnm;
			}
		    }

/*                 Form an exact solution and set the right hand side. */

		    s_copy(srnamc_1.srnamt, "SLARHS", (ftnlen)6, (ftnlen)6);
		    slarhs_(path, xtype, uplo, " ", &n, &n, &kl, &ku, nrhs, &
			    a[1], &lda, &xact[1], &lda, &b[1], &lda, iseed, &
			    info);
		    *(unsigned char *)xtype = 'C';

/*                 --- Test SSPSV  --- */

		    if (ifact == 2) {
			scopy_(&npp, &a[1], &c__1, &afac[1], &c__1);
			slacpy_("Full", &n, nrhs, &b[1], &lda, &x[1], &lda);

/*                    Factor the matrix and solve the system using SSPSV. */

			s_copy(srnamc_1.srnamt, "SSPSV ", (ftnlen)6, (ftnlen)
				6);
			sspsv_(uplo, &n, nrhs, &afac[1], &iwork[1], &x[1], &
				lda, &info);

/*                    Adjust the expected value of INFO to account for */
/*                    pivoting. */

			k = izero;
			if (k > 0) {
L100:
			    if (iwork[k] < 0) {
				if (iwork[k] != -k) {
				    k = -iwork[k];
				    goto L100;
				}
			    } else if (iwork[k] != k) {
				k = iwork[k];
				goto L100;
			    }
			}

/*                    Check error code from SSPSV . */

			if (info != k) {
			    alaerh_(path, "SSPSV ", &info, &k, uplo, &n, &n, &
				    c_n1, &c_n1, nrhs, &imat, &nfail, &nerrs, 
				    nout);
			    goto L120;
			} else if (info != 0) {
			    goto L120;
			}

/*                    Reconstruct matrix from factors and compute */
/*                    residual. */

			sspt01_(uplo, &n, &a[1], &afac[1], &iwork[1], &ainv[1]
, &lda, &rwork[1], result);

/*                    Compute residual of the computed solution. */

			slacpy_("Full", &n, nrhs, &b[1], &lda, &work[1], &lda);
			sppt02_(uplo, &n, nrhs, &a[1], &x[1], &lda, &work[1], 
				&lda, &rwork[1], &result[1]);

/*                    Check solution from generated exact solution. */

			sget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[2]);
			nt = 3;

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			i__3 = nt;
			for (k = 1; k <= i__3; ++k) {
			    if (result[k - 1] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    aladhd_(nout, path);
				}
				io___41.ciunit = *nout;
				s_wsfe(&io___41);
				do_fio(&c__1, "SSPSV ", (ftnlen)6);
				do_fio(&c__1, uplo, (ftnlen)1);
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
/* L110: */
			}
			nrun += nt;
L120:
			;
		    }

/*                 --- Test SSPSVX --- */

		    if (ifact == 2 && npp > 0) {
			slaset_("Full", &npp, &c__1, &c_b59, &c_b59, &afac[1], 
				 &npp);
		    }
		    slaset_("Full", &n, nrhs, &c_b59, &c_b59, &x[1], &lda);

/*                 Solve the system and compute the condition number and */
/*                 error bounds using SSPSVX. */

		    s_copy(srnamc_1.srnamt, "SSPSVX", (ftnlen)6, (ftnlen)6);
		    sspsvx_(fact, uplo, &n, nrhs, &a[1], &afac[1], &iwork[1], 
			    &b[1], &lda, &x[1], &lda, &rcond, &rwork[1], &
			    rwork[*nrhs + 1], &work[1], &iwork[n + 1], &info);

/*                 Adjust the expected value of INFO to account for */
/*                 pivoting. */

		    k = izero;
		    if (k > 0) {
L130:
			if (iwork[k] < 0) {
			    if (iwork[k] != -k) {
				k = -iwork[k];
				goto L130;
			    }
			} else if (iwork[k] != k) {
			    k = iwork[k];
			    goto L130;
			}
		    }

/*                 Check the error code from SSPSVX. */

		    if (info != k) {
/* Writing concatenation */
			i__5[0] = 1, a__1[0] = fact;
			i__5[1] = 1, a__1[1] = uplo;
			s_cat(ch__1, a__1, i__5, &c__2, (ftnlen)2);
			alaerh_(path, "SSPSVX", &info, &k, ch__1, &n, &n, &
				c_n1, &c_n1, nrhs, &imat, &nfail, &nerrs, 
				nout);
			goto L150;
		    }

		    if (info == 0) {
			if (ifact >= 2) {

/*                       Reconstruct matrix from factors and compute */
/*                       residual. */

			    sspt01_(uplo, &n, &a[1], &afac[1], &iwork[1], &
				    ainv[1], &lda, &rwork[(*nrhs << 1) + 1], 
				    result);
			    k1 = 1;
			} else {
			    k1 = 2;
			}

/*                    Compute residual of the computed solution. */

			slacpy_("Full", &n, nrhs, &b[1], &lda, &work[1], &lda);
			sppt02_(uplo, &n, nrhs, &a[1], &x[1], &lda, &work[1], 
				&lda, &rwork[(*nrhs << 1) + 1], &result[1]);

/*                    Check solution from generated exact solution. */

			sget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[2]);

/*                    Check the error bounds from iterative refinement. */

			sppt05_(uplo, &n, nrhs, &a[1], &b[1], &lda, &x[1], &
				lda, &xact[1], &lda, &rwork[1], &rwork[*nrhs 
				+ 1], &result[3]);
		    } else {
			k1 = 6;
		    }

/*                 Compare RCOND from SSPSVX with the computed value */
/*                 in RCONDC. */

		    result[5] = sget06_(&rcond, &rcondc);

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    for (k = k1; k <= 6; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				aladhd_(nout, path);
			    }
			    io___44.ciunit = *nout;
			    s_wsfe(&io___44);
			    do_fio(&c__1, "SSPSVX", (ftnlen)6);
			    do_fio(&c__1, fact, (ftnlen)1);
			    do_fio(&c__1, uplo, (ftnlen)1);
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
/* L140: */
		    }
		    nrun = nrun + 7 - k1;

L150:
		    ;
		}

L160:
		;
	    }
L170:
	    ;
	}
/* L180: */
    }

/*     Print a summary of the results. */

    alasvm_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of SDRVSP */

} /* sdrvsp_ */
