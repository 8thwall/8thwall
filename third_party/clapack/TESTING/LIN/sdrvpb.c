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

static integer c__1 = 1;
static integer c__2 = 2;
static integer c__0 = 0;
static integer c_n1 = -1;
static real c_b45 = 0.f;
static real c_b46 = 1.f;

/* Subroutine */ int sdrvpb_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, real *thresh, logical *tsterr, integer *nmax, real *a, 
	real *afac, real *asav, real *b, real *bsav, real *x, real *xact, 
	real *s, real *work, real *rwork, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char facts[1*3] = "F" "N" "E";
    static char equeds[1*2] = "N" "Y";

    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002, UPLO='\002,a1,\002', N =\002,i5"
	    ",\002, KD =\002,i5,\002, type \002,i1,\002, test(\002,i1,\002)"
	    "=\002,g12.5)";
    static char fmt_9997[] = "(1x,a6,\002( '\002,a1,\002', '\002,a1,\002',"
	    " \002,i5,\002, \002,i5,\002, ... ), EQUED='\002,a1,\002', type"
	    " \002,i1,\002, test(\002,i1,\002)=\002,g12.5)";
    static char fmt_9998[] = "(1x,a6,\002( '\002,a1,\002', '\002,a1,\002',"
	    " \002,i5,\002, \002,i5,\002, ... ), type \002,i1,\002, test(\002"
	    ",i1,\002)=\002,g12.5)";

    /* System generated locals */
    address a__1[2];
    integer i__1, i__2, i__3, i__4, i__5, i__6, i__7[2];
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, k, n, i1, i2, k1, kd, nb, in, kl, iw, ku, nt, lda, ikd, nkd, 
	    ldab;
    char fact[1];
    integer ioff, mode, koff;
    real amax;
    char path[3];
    integer imat, info;
    char dist[1], uplo[1], type__[1];
    integer nrun, ifact, nfail, iseed[4], nfact, kdval[4];
    extern logical lsame_(char *, char *);
    char equed[1];
    integer nbmin;
    real rcond, roldc, scond;
    integer nimat;
    extern doublereal sget06_(real *, real *);
    extern /* Subroutine */ int sget04_(integer *, integer *, real *, integer 
	    *, real *, integer *, real *, real *), spbt01_(char *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, real *);
    real anorm;
    extern /* Subroutine */ int spbt02_(char *, integer *, integer *, integer 
	    *, real *, integer *, real *, integer *, real *, integer *, real *
, real *), spbt05_(char *, integer *, integer *, integer *
, real *, integer *, real *, integer *, real *, integer *, real *, 
	     integer *, real *, real *, real *);
    logical equil;
    integer iuplo, izero, nerrs;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), spbsv_(char *, integer *, integer *, integer *, real *
, integer *, real *, integer *, integer *), sswap_(
	    integer *, real *, integer *, real *, integer *);
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int slatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), aladhd_(integer *, char *), 
	    alaerh_(char *, char *, integer *, integer *, char *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *);
    logical prefac;
    real rcondc;
    extern doublereal slange_(char *, integer *, integer *, real *, integer *, 
	     real *);
    logical nofact;
    char packit[1];
    integer iequed;
    extern doublereal slansb_(char *, char *, integer *, integer *, real *, 
	    integer *, real *);
    real cndnum;
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *), slaqsb_(char *, integer *, integer *, real 
	    *, integer *, real *, real *, real *, char *);
    real ainvnm;
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slarhs_(char *, char *, 
	    char *, char *, integer *, integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, integer *, integer *), slaset_(
	    char *, integer *, integer *, real *, real *, real *, integer *), spbequ_(char *, integer *, integer *, real *, integer *, 
	    real *, real *, real *, integer *), spbtrf_(char *, 
	    integer *, integer *, real *, integer *, integer *), 
	    xlaenv_(integer *, integer *), slatms_(integer *, integer *, char 
	    *, integer *, char *, real *, integer *, real *, real *, integer *
, integer *, char *, real *, integer *, real *, integer *), spbtrs_(char *, integer *, integer *, integer *, 
	     real *, integer *, real *, integer *, integer *);
    real result[6];
    extern /* Subroutine */ int spbsvx_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, char *, real *, 
	    real *, integer *, real *, integer *, real *, real *, real *, 
	    real *, integer *, integer *), serrvx_(
	    char *, integer *);

    /* Fortran I/O blocks */
    static cilist io___57 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SDRVPB tests the driver routines SPBSV and -SVX. */

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

/*  A       (workspace) REAL array, dimension (NMAX*NMAX) */

/*  AFAC    (workspace) REAL array, dimension (NMAX*NMAX) */

/*  ASAV    (workspace) REAL array, dimension (NMAX*NMAX) */

/*  B       (workspace) REAL array, dimension (NMAX*NRHS) */

/*  BSAV    (workspace) REAL array, dimension (NMAX*NRHS) */

/*  X       (workspace) REAL array, dimension (NMAX*NRHS) */

/*  XACT    (workspace) REAL array, dimension (NMAX*NRHS) */

/*  S       (workspace) REAL array, dimension (NMAX) */

/*  WORK    (workspace) REAL array, dimension */
/*                      (NMAX*max(3,NRHS)) */

/*  RWORK   (workspace) REAL array, dimension (NMAX+2*NRHS) */

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

    s_copy(path, "Single precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "PB", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	serrvx_(path, nout);
    }
    infoc_1.infot = 0;
    kdval[0] = 0;

/*     Set the block size and minimum block size for testing. */

    nb = 1;
    nbmin = 2;
    xlaenv_(&c__1, &nb);
    xlaenv_(&c__2, &nbmin);

/*     Do for each value of N in NVAL */

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {
	n = nval[in];
	lda = max(n,1);
	*(unsigned char *)xtype = 'N';

/*        Set limits on the number of loop iterations. */

/* Computing MAX */
	i__2 = 1, i__3 = min(n,4);
	nkd = max(i__2,i__3);
	nimat = 8;
	if (n == 0) {
	    nimat = 1;
	}

	kdval[1] = n + (n + 1) / 4;
	kdval[2] = (n * 3 - 1) / 4;
	kdval[3] = (n + 1) / 4;

	i__2 = nkd;
	for (ikd = 1; ikd <= i__2; ++ikd) {

/*           Do for KD = 0, (5*N+1)/4, (3N-1)/4, and (N+1)/4. This order */
/*           makes it easier to skip redundant values for small values */
/*           of N. */

	    kd = kdval[ikd - 1];
	    ldab = kd + 1;

/*           Do first for UPLO = 'U', then for UPLO = 'L' */

	    for (iuplo = 1; iuplo <= 2; ++iuplo) {
		koff = 1;
		if (iuplo == 1) {
		    *(unsigned char *)uplo = 'U';
		    *(unsigned char *)packit = 'Q';
/* Computing MAX */
		    i__3 = 1, i__4 = kd + 2 - n;
		    koff = max(i__3,i__4);
		} else {
		    *(unsigned char *)uplo = 'L';
		    *(unsigned char *)packit = 'B';
		}

		i__3 = nimat;
		for (imat = 1; imat <= i__3; ++imat) {

/*                 Do the tests only if DOTYPE( IMAT ) is true. */

		    if (! dotype[imat]) {
			goto L80;
		    }

/*                 Skip types 2, 3, or 4 if the matrix size is too small. */

		    zerot = imat >= 2 && imat <= 4;
		    if (zerot && n < imat - 1) {
			goto L80;
		    }

		    if (! zerot || ! dotype[1]) {

/*                    Set up parameters with SLATB4 and generate a test */
/*                    matrix with SLATMS. */

			slatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, 
				 &mode, &cndnum, dist);

			s_copy(srnamc_1.srnamt, "SLATMS", (ftnlen)6, (ftnlen)
				6);
			slatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, 
				 &cndnum, &anorm, &kd, &kd, packit, &a[koff], 
				&ldab, &work[1], &info);

/*                    Check error code from SLATMS. */

			if (info != 0) {
			    alaerh_(path, "SLATMS", &info, &c__0, uplo, &n, &
				    n, &c_n1, &c_n1, &c_n1, &imat, &nfail, &
				    nerrs, nout);
			    goto L80;
			}
		    } else if (izero > 0) {

/*                    Use the same matrix for types 3 and 4 as for type */
/*                    2 by copying back the zeroed out column, */

			iw = (lda << 1) + 1;
			if (iuplo == 1) {
			    ioff = (izero - 1) * ldab + kd + 1;
			    i__4 = izero - i1;
			    scopy_(&i__4, &work[iw], &c__1, &a[ioff - izero + 
				    i1], &c__1);
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    scopy_(&i__4, &work[iw], &c__1, &a[ioff], &i__5);
			} else {
			    ioff = (i1 - 1) * ldab + 1;
			    i__4 = izero - i1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    scopy_(&i__4, &work[iw], &c__1, &a[ioff + izero - 
				    i1], &i__5);
			    ioff = (izero - 1) * ldab + 1;
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
			    scopy_(&i__4, &work[iw], &c__1, &a[ioff], &c__1);
			}
		    }

/*                 For types 2-4, zero one row and column of the matrix */
/*                 to test that INFO is returned correctly. */

		    izero = 0;
		    if (zerot) {
			if (imat == 2) {
			    izero = 1;
			} else if (imat == 3) {
			    izero = n;
			} else {
			    izero = n / 2 + 1;
			}

/*                    Save the zeroed out row and column in WORK(*,3) */

			iw = lda << 1;
/* Computing MIN */
			i__5 = (kd << 1) + 1;
			i__4 = min(i__5,n);
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[iw + i__] = 0.f;
/* L20: */
			}
			++iw;
/* Computing MAX */
			i__4 = izero - kd;
			i1 = max(i__4,1);
/* Computing MIN */
			i__4 = izero + kd;
			i2 = min(i__4,n);

			if (iuplo == 1) {
			    ioff = (izero - 1) * ldab + kd + 1;
			    i__4 = izero - i1;
			    sswap_(&i__4, &a[ioff - izero + i1], &c__1, &work[
				    iw], &c__1);
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    sswap_(&i__4, &a[ioff], &i__5, &work[iw], &c__1);
			} else {
			    ioff = (i1 - 1) * ldab + 1;
			    i__4 = izero - i1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    sswap_(&i__4, &a[ioff + izero - i1], &i__5, &work[
				    iw], &c__1);
			    ioff = (izero - 1) * ldab + 1;
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
			    sswap_(&i__4, &a[ioff], &c__1, &work[iw], &c__1);
			}
		    }

/*                 Save a copy of the matrix A in ASAV. */

		    i__4 = kd + 1;
		    slacpy_("Full", &i__4, &n, &a[1], &ldab, &asav[1], &ldab);

		    for (iequed = 1; iequed <= 2; ++iequed) {
			*(unsigned char *)equed = *(unsigned char *)&equeds[
				iequed - 1];
			if (iequed == 1) {
			    nfact = 3;
			} else {
			    nfact = 1;
			}

			i__4 = nfact;
			for (ifact = 1; ifact <= i__4; ++ifact) {
			    *(unsigned char *)fact = *(unsigned char *)&facts[
				    ifact - 1];
			    prefac = lsame_(fact, "F");
			    nofact = lsame_(fact, "N");
			    equil = lsame_(fact, "E");

			    if (zerot) {
				if (prefac) {
				    goto L60;
				}
				rcondc = 0.f;

			    } else if (! lsame_(fact, "N")) {

/*                          Compute the condition number for comparison */
/*                          with the value returned by SPBSVX (FACT = */
/*                          'N' reuses the condition number from the */
/*                          previous iteration with FACT = 'F'). */

				i__5 = kd + 1;
				slacpy_("Full", &i__5, &n, &asav[1], &ldab, &
					afac[1], &ldab);
				if (equil || iequed > 1) {

/*                             Compute row and column scale factors to */
/*                             equilibrate the matrix A. */

				    spbequ_(uplo, &n, &kd, &afac[1], &ldab, &
					    s[1], &scond, &amax, &info);
				    if (info == 0 && n > 0) {
					if (iequed > 1) {
					    scond = 0.f;
					}

/*                                Equilibrate the matrix. */

					slaqsb_(uplo, &n, &kd, &afac[1], &
						ldab, &s[1], &scond, &amax, 
						equed);
				    }
				}

/*                          Save the condition number of the */
/*                          non-equilibrated system for use in SGET04. */

				if (equil) {
				    roldc = rcondc;
				}

/*                          Compute the 1-norm of A. */

				anorm = slansb_("1", uplo, &n, &kd, &afac[1], 
					&ldab, &rwork[1]);

/*                          Factor the matrix A. */

				spbtrf_(uplo, &n, &kd, &afac[1], &ldab, &info);

/*                          Form the inverse of A. */

				slaset_("Full", &n, &n, &c_b45, &c_b46, &a[1], 
					 &lda);
				s_copy(srnamc_1.srnamt, "SPBTRS", (ftnlen)6, (
					ftnlen)6);
				spbtrs_(uplo, &n, &kd, &n, &afac[1], &ldab, &
					a[1], &lda, &info);

/*                          Compute the 1-norm condition number of A. */

				ainvnm = slange_("1", &n, &n, &a[1], &lda, &
					rwork[1]);
				if (anorm <= 0.f || ainvnm <= 0.f) {
				    rcondc = 1.f;
				} else {
				    rcondc = 1.f / anorm / ainvnm;
				}
			    }

/*                       Restore the matrix A. */

			    i__5 = kd + 1;
			    slacpy_("Full", &i__5, &n, &asav[1], &ldab, &a[1], 
				     &ldab);

/*                       Form an exact solution and set the right hand */
/*                       side. */

			    s_copy(srnamc_1.srnamt, "SLARHS", (ftnlen)6, (
				    ftnlen)6);
			    slarhs_(path, xtype, uplo, " ", &n, &n, &kd, &kd, 
				    nrhs, &a[1], &ldab, &xact[1], &lda, &b[1], 
				     &lda, iseed, &info);
			    *(unsigned char *)xtype = 'C';
			    slacpy_("Full", &n, nrhs, &b[1], &lda, &bsav[1], &
				    lda);

			    if (nofact) {

/*                          --- Test SPBSV  --- */

/*                          Compute the L*L' or U'*U factorization of the */
/*                          matrix and solve the system. */

				i__5 = kd + 1;
				slacpy_("Full", &i__5, &n, &a[1], &ldab, &
					afac[1], &ldab);
				slacpy_("Full", &n, nrhs, &b[1], &lda, &x[1], 
					&lda);

				s_copy(srnamc_1.srnamt, "SPBSV ", (ftnlen)6, (
					ftnlen)6);
				spbsv_(uplo, &n, &kd, nrhs, &afac[1], &ldab, &
					x[1], &lda, &info);

/*                          Check error code from SPBSV . */

				if (info != izero) {
				    alaerh_(path, "SPBSV ", &info, &izero, 
					    uplo, &n, &n, &kd, &kd, nrhs, &
					    imat, &nfail, &nerrs, nout);
				    goto L40;
				} else if (info != 0) {
				    goto L40;
				}

/*                          Reconstruct matrix from factors and compute */
/*                          residual. */

				spbt01_(uplo, &n, &kd, &a[1], &ldab, &afac[1], 
					 &ldab, &rwork[1], result);

/*                          Compute residual of the computed solution. */

				slacpy_("Full", &n, nrhs, &b[1], &lda, &work[
					1], &lda);
				spbt02_(uplo, &n, &kd, nrhs, &a[1], &ldab, &x[
					1], &lda, &work[1], &lda, &rwork[1], &
					result[1]);

/*                          Check solution from generated exact solution. */

				sget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, 
					 &rcondc, &result[2]);
				nt = 3;

/*                          Print information about the tests that did */
/*                          not pass the threshold. */

				i__5 = nt;
				for (k = 1; k <= i__5; ++k) {
				    if (result[k - 1] >= *thresh) {
					if (nfail == 0 && nerrs == 0) {
					    aladhd_(nout, path);
					}
					io___57.ciunit = *nout;
					s_wsfe(&io___57);
					do_fio(&c__1, "SPBSV ", (ftnlen)6);
					do_fio(&c__1, uplo, (ftnlen)1);
					do_fio(&c__1, (char *)&n, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&kd, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&imat, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&k, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&result[k - 1], 
						(ftnlen)sizeof(real));
					e_wsfe();
					++nfail;
				    }
/* L30: */
				}
				nrun += nt;
L40:
				;
			    }

/*                       --- Test SPBSVX --- */

			    if (! prefac) {
				i__5 = kd + 1;
				slaset_("Full", &i__5, &n, &c_b45, &c_b45, &
					afac[1], &ldab);
			    }
			    slaset_("Full", &n, nrhs, &c_b45, &c_b45, &x[1], &
				    lda);
			    if (iequed > 1 && n > 0) {

/*                          Equilibrate the matrix if FACT='F' and */
/*                          EQUED='Y' */

				slaqsb_(uplo, &n, &kd, &a[1], &ldab, &s[1], &
					scond, &amax, equed);
			    }

/*                       Solve the system and compute the condition */
/*                       number and error bounds using SPBSVX. */

			    s_copy(srnamc_1.srnamt, "SPBSVX", (ftnlen)6, (
				    ftnlen)6);
			    spbsvx_(fact, uplo, &n, &kd, nrhs, &a[1], &ldab, &
				    afac[1], &ldab, equed, &s[1], &b[1], &lda, 
				     &x[1], &lda, &rcond, &rwork[1], &rwork[*
				    nrhs + 1], &work[1], &iwork[1], &info);

/*                       Check the error code from SPBSVX. */

			    if (info != izero) {
/* Writing concatenation */
				i__7[0] = 1, a__1[0] = fact;
				i__7[1] = 1, a__1[1] = uplo;
				s_cat(ch__1, a__1, i__7, &c__2, (ftnlen)2);
				alaerh_(path, "SPBSVX", &info, &izero, ch__1, 
					&n, &n, &kd, &kd, nrhs, &imat, &nfail, 
					 &nerrs, nout);
				goto L60;
			    }

			    if (info == 0) {
				if (! prefac) {

/*                             Reconstruct matrix from factors and */
/*                             compute residual. */

				    spbt01_(uplo, &n, &kd, &a[1], &ldab, &
					    afac[1], &ldab, &rwork[(*nrhs << 
					    1) + 1], result);
				    k1 = 1;
				} else {
				    k1 = 2;
				}

/*                          Compute residual of the computed solution. */

				slacpy_("Full", &n, nrhs, &bsav[1], &lda, &
					work[1], &lda);
				spbt02_(uplo, &n, &kd, nrhs, &asav[1], &ldab, 
					&x[1], &lda, &work[1], &lda, &rwork[(*
					nrhs << 1) + 1], &result[1]);

/*                          Check solution from generated exact solution. */

				if (nofact || prefac && lsame_(equed, "N")) {
				    sget04_(&n, nrhs, &x[1], &lda, &xact[1], &
					    lda, &rcondc, &result[2]);
				} else {
				    sget04_(&n, nrhs, &x[1], &lda, &xact[1], &
					    lda, &roldc, &result[2]);
				}

/*                          Check the error bounds from iterative */
/*                          refinement. */

				spbt05_(uplo, &n, &kd, nrhs, &asav[1], &ldab, 
					&b[1], &lda, &x[1], &lda, &xact[1], &
					lda, &rwork[1], &rwork[*nrhs + 1], &
					result[3]);
			    } else {
				k1 = 6;
			    }

/*                       Compare RCOND from SPBSVX with the computed */
/*                       value in RCONDC. */

			    result[5] = sget06_(&rcond, &rcondc);

/*                       Print information about the tests that did not */
/*                       pass the threshold. */

			    for (k = k1; k <= 6; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					aladhd_(nout, path);
				    }
				    if (prefac) {
					io___60.ciunit = *nout;
					s_wsfe(&io___60);
					do_fio(&c__1, "SPBSVX", (ftnlen)6);
					do_fio(&c__1, fact, (ftnlen)1);
					do_fio(&c__1, uplo, (ftnlen)1);
					do_fio(&c__1, (char *)&n, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&kd, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, equed, (ftnlen)1);
					do_fio(&c__1, (char *)&imat, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&k, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&result[k - 1], 
						(ftnlen)sizeof(real));
					e_wsfe();
				    } else {
					io___61.ciunit = *nout;
					s_wsfe(&io___61);
					do_fio(&c__1, "SPBSVX", (ftnlen)6);
					do_fio(&c__1, fact, (ftnlen)1);
					do_fio(&c__1, uplo, (ftnlen)1);
					do_fio(&c__1, (char *)&n, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&kd, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&imat, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&k, (ftnlen)
						sizeof(integer));
					do_fio(&c__1, (char *)&result[k - 1], 
						(ftnlen)sizeof(real));
					e_wsfe();
				    }
				    ++nfail;
				}
/* L50: */
			    }
			    nrun = nrun + 7 - k1;
L60:
			    ;
			}
/* L70: */
		    }
L80:
		    ;
		}
/* L90: */
	    }
/* L100: */
	}
/* L110: */
    }

/*     Print a summary of the results. */

    alasvm_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of SDRVPB */

} /* sdrvpb_ */
