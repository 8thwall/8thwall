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
static doublereal c_b60 = 0.;
static integer c__2 = 2;

/* Subroutine */ int ddrvpp_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, doublereal *thresh, logical *tsterr, integer *nmax, 
	doublereal *a, doublereal *afac, doublereal *asav, doublereal *b, 
	doublereal *bsav, doublereal *x, doublereal *xact, doublereal *s, 
	doublereal *work, doublereal *rwork, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char uplos[1*2] = "U" "L";
    static char facts[1*3] = "F" "N" "E";
    static char packs[1*2] = "C" "R";
    static char equeds[1*2] = "N" "Y";

    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002, UPLO='\002,a1,\002', N =\002,i5"
	    ",\002, type \002,i1,\002, test(\002,i1,\002)=\002,g12.5)";
    static char fmt_9997[] = "(1x,a6,\002, FACT='\002,a1,\002', UPLO='\002,a"
	    "1,\002', N=\002,i5,\002, EQUED='\002,a1,\002', type \002,i1,\002"
	    ", test(\002,i1,\002)=\002,g12.5)";
    static char fmt_9998[] = "(1x,a6,\002, FACT='\002,a1,\002', UPLO='\002,a"
	    "1,\002', N=\002,i5,\002, type \002,i1,\002, test(\002,i1,\002)"
	    "=\002,g12.5)";

    /* System generated locals */
    address a__1[2];
    integer i__1, i__2, i__3, i__4, i__5[2];
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, k, n, k1, in, kl, ku, nt, lda, npp;
    char fact[1];
    integer ioff, mode;
    doublereal amax;
    char path[3];
    integer imat, info;
    char dist[1], uplo[1], type__[1];
    integer nrun, ifact;
    extern /* Subroutine */ int dget04_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *);
    integer nfail, iseed[4], nfact;
    extern doublereal dget06_(doublereal *, doublereal *);
    extern logical lsame_(char *, char *);
    char equed[1];
    doublereal roldc, rcond, scond;
    integer nimat;
    extern /* Subroutine */ int dppt01_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *), dppt02_(char *, 
	     integer *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *);
    doublereal anorm;
    extern /* Subroutine */ int dppt05_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *), dcopy_(integer *, doublereal *, integer *, doublereal *, 
	     integer *);
    logical equil;
    integer iuplo, izero, nerrs;
    extern /* Subroutine */ int dppsv_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *);
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), aladhd_(integer *, 
	    char *), alaerh_(char *, char *, integer *, integer *, 
	    char *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *);
    logical prefac;
    doublereal rcondc;
    logical nofact;
    char packit[1];
    integer iequed;
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlarhs_(char *, char *, char *, char *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *), dlaset_(char *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    integer *);
    extern doublereal dlansp_(char *, char *, integer *, doublereal *, 
	    doublereal *);
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *);
    doublereal cndnum;
    extern /* Subroutine */ int dlaqsp_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, char *),
	     dlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublereal *, integer *, doublereal *, integer 
	    *);
    doublereal ainvnm;
    extern /* Subroutine */ int dppequ_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    dpptrf_(char *, integer *, doublereal *, integer *), 
	    dpptri_(char *, integer *, doublereal *, integer *), 
	    derrvx_(char *, integer *);
    doublereal result[6];
    extern /* Subroutine */ int dppsvx_(char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, char *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___49 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DDRVPP tests the driver routines DPPSV and -SVX. */

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

/*  THRESH  (input) DOUBLE PRECISION */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  NMAX    (input) INTEGER */
/*          The maximum value permitted for N, used in dimensioning the */
/*          work arrays. */

/*  A       (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  AFAC    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  ASAV    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*(NMAX+1)/2) */

/*  B       (workspace) DOUBLE PRECISION array, dimension (NMAX*NRHS) */

/*  BSAV    (workspace) DOUBLE PRECISION array, dimension (NMAX*NRHS) */

/*  X       (workspace) DOUBLE PRECISION array, dimension (NMAX*NRHS) */

/*  XACT    (workspace) DOUBLE PRECISION array, dimension (NMAX*NRHS) */

/*  S       (workspace) DOUBLE PRECISION array, dimension (NMAX) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*max(3,NRHS)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (NMAX+2*NRHS) */

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
    --s;
    --xact;
    --x;
    --bsav;
    --b;
    --asav;
    --afac;
    --a;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
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
	derrvx_(path, nout);
    }
    infoc_1.infot = 0;

/*     Do for each value of N in NVAL */

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {
	n = nval[in];
	lda = max(n,1);
	npp = n * (n + 1) / 2;
	*(unsigned char *)xtype = 'N';
	nimat = 9;
	if (n <= 0) {
	    nimat = 1;
	}

	i__2 = nimat;
	for (imat = 1; imat <= i__2; ++imat) {

/*           Do the tests only if DOTYPE( IMAT ) is true. */

	    if (! dotype[imat]) {
		goto L130;
	    }

/*           Skip types 3, 4, or 5 if the matrix size is too small. */

	    zerot = imat >= 3 && imat <= 5;
	    if (zerot && n < imat - 2) {
		goto L130;
	    }

/*           Do first for UPLO = 'U', then for UPLO = 'L' */

	    for (iuplo = 1; iuplo <= 2; ++iuplo) {
		*(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 1];
		*(unsigned char *)packit = *(unsigned char *)&packs[iuplo - 1]
			;

/*              Set up parameters with DLATB4 and generate a test matrix */
/*              with DLATMS. */

		dlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, 
			&cndnum, dist);
		rcondc = 1. / cndnum;

		s_copy(srnamc_1.srnamt, "DLATMS", (ftnlen)6, (ftnlen)6);
		dlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &
			cndnum, &anorm, &kl, &ku, packit, &a[1], &lda, &work[
			1], &info);

/*              Check error code from DLATMS. */

		if (info != 0) {
		    alaerh_(path, "DLATMS", &info, &c__0, uplo, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L120;
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
			    a[ioff + i__] = 0.;
/* L20: */
			}
			ioff += izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    a[ioff] = 0.;
			    ioff += i__;
/* L30: */
			}
		    } else {
			ioff = izero;
			i__3 = izero - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    a[ioff] = 0.;
			    ioff = ioff + n - i__;
/* L40: */
			}
			ioff -= izero;
			i__3 = n;
			for (i__ = izero; i__ <= i__3; ++i__) {
			    a[ioff + i__] = 0.;
/* L50: */
			}
		    }
		} else {
		    izero = 0;
		}

/*              Save a copy of the matrix A in ASAV. */

		dcopy_(&npp, &a[1], &c__1, &asav[1], &c__1);

		for (iequed = 1; iequed <= 2; ++iequed) {
		    *(unsigned char *)equed = *(unsigned char *)&equeds[
			    iequed - 1];
		    if (iequed == 1) {
			nfact = 3;
		    } else {
			nfact = 1;
		    }

		    i__3 = nfact;
		    for (ifact = 1; ifact <= i__3; ++ifact) {
			*(unsigned char *)fact = *(unsigned char *)&facts[
				ifact - 1];
			prefac = lsame_(fact, "F");
			nofact = lsame_(fact, "N");
			equil = lsame_(fact, "E");

			if (zerot) {
			    if (prefac) {
				goto L100;
			    }
			    rcondc = 0.;

			} else if (! lsame_(fact, "N")) 
				{

/*                       Compute the condition number for comparison with */
/*                       the value returned by DPPSVX (FACT = 'N' reuses */
/*                       the condition number from the previous iteration */
/*                       with FACT = 'F'). */

			    dcopy_(&npp, &asav[1], &c__1, &afac[1], &c__1);
			    if (equil || iequed > 1) {

/*                          Compute row and column scale factors to */
/*                          equilibrate the matrix A. */

				dppequ_(uplo, &n, &afac[1], &s[1], &scond, &
					amax, &info);
				if (info == 0 && n > 0) {
				    if (iequed > 1) {
					scond = 0.;
				    }

/*                             Equilibrate the matrix. */

				    dlaqsp_(uplo, &n, &afac[1], &s[1], &scond, 
					     &amax, equed);
				}
			    }

/*                       Save the condition number of the */
/*                       non-equilibrated system for use in DGET04. */

			    if (equil) {
				roldc = rcondc;
			    }

/*                       Compute the 1-norm of A. */

			    anorm = dlansp_("1", uplo, &n, &afac[1], &rwork[1]
);

/*                       Factor the matrix A. */

			    dpptrf_(uplo, &n, &afac[1], &info);

/*                       Form the inverse of A. */

			    dcopy_(&npp, &afac[1], &c__1, &a[1], &c__1);
			    dpptri_(uplo, &n, &a[1], &info);

/*                       Compute the 1-norm condition number of A. */

			    ainvnm = dlansp_("1", uplo, &n, &a[1], &rwork[1]);
			    if (anorm <= 0. || ainvnm <= 0.) {
				rcondc = 1.;
			    } else {
				rcondc = 1. / anorm / ainvnm;
			    }
			}

/*                    Restore the matrix A. */

			dcopy_(&npp, &asav[1], &c__1, &a[1], &c__1);

/*                    Form an exact solution and set the right hand side. */

			s_copy(srnamc_1.srnamt, "DLARHS", (ftnlen)6, (ftnlen)
				6);
			dlarhs_(path, xtype, uplo, " ", &n, &n, &kl, &ku, 
				nrhs, &a[1], &lda, &xact[1], &lda, &b[1], &
				lda, iseed, &info);
			*(unsigned char *)xtype = 'C';
			dlacpy_("Full", &n, nrhs, &b[1], &lda, &bsav[1], &lda);

			if (nofact) {

/*                       --- Test DPPSV  --- */

/*                       Compute the L*L' or U'*U factorization of the */
/*                       matrix and solve the system. */

			    dcopy_(&npp, &a[1], &c__1, &afac[1], &c__1);
			    dlacpy_("Full", &n, nrhs, &b[1], &lda, &x[1], &
				    lda);

			    s_copy(srnamc_1.srnamt, "DPPSV ", (ftnlen)6, (
				    ftnlen)6);
			    dppsv_(uplo, &n, nrhs, &afac[1], &x[1], &lda, &
				    info);

/*                       Check error code from DPPSV . */

			    if (info != izero) {
				alaerh_(path, "DPPSV ", &info, &izero, uplo, &
					n, &n, &c_n1, &c_n1, nrhs, &imat, &
					nfail, &nerrs, nout);
				goto L70;
			    } else if (info != 0) {
				goto L70;
			    }

/*                       Reconstruct matrix from factors and compute */
/*                       residual. */

			    dppt01_(uplo, &n, &a[1], &afac[1], &rwork[1], 
				    result);

/*                       Compute residual of the computed solution. */

			    dlacpy_("Full", &n, nrhs, &b[1], &lda, &work[1], &
				    lda);
			    dppt02_(uplo, &n, nrhs, &a[1], &x[1], &lda, &work[
				    1], &lda, &rwork[1], &result[1]);

/*                       Check solution from generated exact solution. */

			    dget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[2]);
			    nt = 3;

/*                       Print information about the tests that did not */
/*                       pass the threshold. */

			    i__4 = nt;
			    for (k = 1; k <= i__4; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					aladhd_(nout, path);
				    }
				    io___49.ciunit = *nout;
				    s_wsfe(&io___49);
				    do_fio(&c__1, "DPPSV ", (ftnlen)6);
				    do_fio(&c__1, uplo, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&result[k - 1], (
					    ftnlen)sizeof(doublereal));
				    e_wsfe();
				    ++nfail;
				}
/* L60: */
			    }
			    nrun += nt;
L70:
			    ;
			}

/*                    --- Test DPPSVX --- */

			if (! prefac && npp > 0) {
			    dlaset_("Full", &npp, &c__1, &c_b60, &c_b60, &
				    afac[1], &npp);
			}
			dlaset_("Full", &n, nrhs, &c_b60, &c_b60, &x[1], &lda);
			if (iequed > 1 && n > 0) {

/*                       Equilibrate the matrix if FACT='F' and */
/*                       EQUED='Y'. */

			    dlaqsp_(uplo, &n, &a[1], &s[1], &scond, &amax, 
				    equed);
			}

/*                    Solve the system and compute the condition number */
/*                    and error bounds using DPPSVX. */

			s_copy(srnamc_1.srnamt, "DPPSVX", (ftnlen)6, (ftnlen)
				6);
			dppsvx_(fact, uplo, &n, nrhs, &a[1], &afac[1], equed, 
				&s[1], &b[1], &lda, &x[1], &lda, &rcond, &
				rwork[1], &rwork[*nrhs + 1], &work[1], &iwork[
				1], &info);

/*                    Check the error code from DPPSVX. */

			if (info != izero) {
/* Writing concatenation */
			    i__5[0] = 1, a__1[0] = fact;
			    i__5[1] = 1, a__1[1] = uplo;
			    s_cat(ch__1, a__1, i__5, &c__2, (ftnlen)2);
			    alaerh_(path, "DPPSVX", &info, &izero, ch__1, &n, 
				    &n, &c_n1, &c_n1, nrhs, &imat, &nfail, &
				    nerrs, nout);
			    goto L90;
			}

			if (info == 0) {
			    if (! prefac) {

/*                          Reconstruct matrix from factors and compute */
/*                          residual. */

				dppt01_(uplo, &n, &a[1], &afac[1], &rwork[(*
					nrhs << 1) + 1], result);
				k1 = 1;
			    } else {
				k1 = 2;
			    }

/*                       Compute residual of the computed solution. */

			    dlacpy_("Full", &n, nrhs, &bsav[1], &lda, &work[1]
, &lda);
			    dppt02_(uplo, &n, nrhs, &asav[1], &x[1], &lda, &
				    work[1], &lda, &rwork[(*nrhs << 1) + 1], &
				    result[1]);

/*                       Check solution from generated exact solution. */

			    if (nofact || prefac && lsame_(equed, "N")) {
				dget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, 
					 &rcondc, &result[2]);
			    } else {
				dget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, 
					 &roldc, &result[2]);
			    }

/*                       Check the error bounds from iterative */
/*                       refinement. */

			    dppt05_(uplo, &n, nrhs, &asav[1], &b[1], &lda, &x[
				    1], &lda, &xact[1], &lda, &rwork[1], &
				    rwork[*nrhs + 1], &result[3]);
			} else {
			    k1 = 6;
			}

/*                    Compare RCOND from DPPSVX with the computed value */
/*                    in RCONDC. */

			result[5] = dget06_(&rcond, &rcondc);

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			for (k = k1; k <= 6; ++k) {
			    if (result[k - 1] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    aladhd_(nout, path);
				}
				if (prefac) {
				    io___52.ciunit = *nout;
				    s_wsfe(&io___52);
				    do_fio(&c__1, "DPPSVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, uplo, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, equed, (ftnlen)1);
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&result[k - 1], (
					    ftnlen)sizeof(doublereal));
				    e_wsfe();
				} else {
				    io___53.ciunit = *nout;
				    s_wsfe(&io___53);
				    do_fio(&c__1, "DPPSVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, uplo, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&result[k - 1], (
					    ftnlen)sizeof(doublereal));
				    e_wsfe();
				}
				++nfail;
			    }
/* L80: */
			}
			nrun = nrun + 7 - k1;
L90:
L100:
			;
		    }
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

/*     End of DDRVPP */

} /* ddrvpp_ */
