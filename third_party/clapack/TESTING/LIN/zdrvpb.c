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
static doublecomplex c_b47 = {0.,0.};
static doublecomplex c_b48 = {1.,0.};

/* Subroutine */ int zdrvpb_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, doublereal *thresh, logical *tsterr, integer *nmax, 
	doublecomplex *a, doublecomplex *afac, doublecomplex *asav, 
	doublecomplex *b, doublecomplex *bsav, doublecomplex *x, 
	doublecomplex *xact, doublereal *s, doublecomplex *work, doublereal *
	rwork, integer *nout)
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
    doublereal amax;
    char path[3];
    integer imat, info;
    char dist[1], uplo[1], type__[1];
    integer nrun, ifact, nfail, iseed[4], nfact;
    extern doublereal dget06_(doublereal *, doublereal *);
    integer kdval[4];
    extern logical lsame_(char *, char *);
    char equed[1];
    integer nbmin;
    doublereal rcond, roldc, scond;
    integer nimat;
    doublereal anorm;
    extern /* Subroutine */ int zget04_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
);
    logical equil;
    extern /* Subroutine */ int zpbt01_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *), zpbt02_(char *, integer *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
), zpbt05_(char *, integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    integer iuplo, izero, nerrs;
    logical zerot;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zpbsv_(char *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     integer *), zswap_(integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *);
    char xtype[1];
    extern /* Subroutine */ int zlatb4_(char *, integer *, integer *, integer 
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
    extern doublereal zlanhb_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *), 
	    zlange_(char *, integer *, integer *, doublecomplex *, integer *, 
	    doublereal *);
    extern /* Subroutine */ int zlaqhb_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublereal *, char *), alasvm_(char *, integer *, 
	    integer *, integer *, integer *);
    doublereal cndnum;
    extern /* Subroutine */ int zlaipd_(integer *, doublecomplex *, integer *, 
	     integer *);
    doublereal ainvnm;
    extern /* Subroutine */ int xlaenv_(integer *, integer *), zlacpy_(char *, 
	     integer *, integer *, doublecomplex *, integer *, doublecomplex *
, integer *), zlarhs_(char *, char *, char *, char *, 
	    integer *, integer *, integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *, integer *), zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *), zpbequ_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *), zpbtrf_(char *, integer *, integer *, doublecomplex *, 
	    integer *, integer *), zlatms_(integer *, integer *, char 
	    *, integer *, char *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, char *, doublecomplex *, 
	    integer *, doublecomplex *, integer *);
    doublereal result[6];
    extern /* Subroutine */ int zpbtrs_(char *, integer *, integer *, integer 
	    *, doublecomplex *, integer *, doublecomplex *, integer *, 
	    integer *), zpbsvx_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     char *, doublereal *, doublecomplex *, integer *, doublecomplex *
, integer *, doublereal *, doublereal *, doublereal *, 
	    doublecomplex *, doublereal *, integer *),
	     zerrvx_(char *, integer *);

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

/*  ZDRVPB tests the driver routines ZPBSV and -SVX. */

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

/*  A       (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  AFAC    (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  ASAV    (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  B       (workspace) COMPLEX*16 array, dimension (NMAX*NRHS) */

/*  BSAV    (workspace) COMPLEX*16 array, dimension (NMAX*NRHS) */

/*  X       (workspace) COMPLEX*16 array, dimension (NMAX*NRHS) */

/*  XACT    (workspace) COMPLEX*16 array, dimension (NMAX*NRHS) */

/*  S       (workspace) DOUBLE PRECISION array, dimension (NMAX) */

/*  WORK    (workspace) COMPLEX*16 array, dimension */
/*                      (NMAX*max(3,NRHS)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (NMAX+2*NRHS) */

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

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
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
	zerrvx_(path, nout);
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

/*                    Set up parameters with ZLATB4 and generate a test */
/*                    matrix with ZLATMS. */

			zlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, 
				 &mode, &cndnum, dist);

			s_copy(srnamc_1.srnamt, "ZLATMS", (ftnlen)6, (ftnlen)
				6);
			zlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, 
				 &cndnum, &anorm, &kd, &kd, packit, &a[koff], 
				&ldab, &work[1], &info);

/*                    Check error code from ZLATMS. */

			if (info != 0) {
			    alaerh_(path, "ZLATMS", &info, &c__0, uplo, &n, &
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
			    zcopy_(&i__4, &work[iw], &c__1, &a[ioff - izero + 
				    i1], &c__1);
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    zcopy_(&i__4, &work[iw], &c__1, &a[ioff], &i__5);
			} else {
			    ioff = (i1 - 1) * ldab + 1;
			    i__4 = izero - i1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    zcopy_(&i__4, &work[iw], &c__1, &a[ioff + izero - 
				    i1], &i__5);
			    ioff = (izero - 1) * ldab + 1;
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
			    zcopy_(&i__4, &work[iw], &c__1, &a[ioff], &c__1);
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
			    i__5 = iw + i__;
			    work[i__5].r = 0., work[i__5].i = 0.;
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
			    zswap_(&i__4, &a[ioff - izero + i1], &c__1, &work[
				    iw], &c__1);
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    zswap_(&i__4, &a[ioff], &i__5, &work[iw], &c__1);
			} else {
			    ioff = (i1 - 1) * ldab + 1;
			    i__4 = izero - i1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    zswap_(&i__4, &a[ioff + izero - i1], &i__5, &work[
				    iw], &c__1);
			    ioff = (izero - 1) * ldab + 1;
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
			    zswap_(&i__4, &a[ioff], &c__1, &work[iw], &c__1);
			}
		    }

/*                 Set the imaginary part of the diagonals. */

		    if (iuplo == 1) {
			zlaipd_(&n, &a[kd + 1], &ldab, &c__0);
		    } else {
			zlaipd_(&n, &a[1], &ldab, &c__0);
		    }

/*                 Save a copy of the matrix A in ASAV. */

		    i__4 = kd + 1;
		    zlacpy_("Full", &i__4, &n, &a[1], &ldab, &asav[1], &ldab);

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
				rcondc = 0.;

			    } else if (! lsame_(fact, "N")) {

/*                          Compute the condition number for comparison */
/*                          with the value returned by ZPBSVX (FACT = */
/*                          'N' reuses the condition number from the */
/*                          previous iteration with FACT = 'F'). */

				i__5 = kd + 1;
				zlacpy_("Full", &i__5, &n, &asav[1], &ldab, &
					afac[1], &ldab);
				if (equil || iequed > 1) {

/*                             Compute row and column scale factors to */
/*                             equilibrate the matrix A. */

				    zpbequ_(uplo, &n, &kd, &afac[1], &ldab, &
					    s[1], &scond, &amax, &info);
				    if (info == 0 && n > 0) {
					if (iequed > 1) {
					    scond = 0.;
					}

/*                                Equilibrate the matrix. */

					zlaqhb_(uplo, &n, &kd, &afac[1], &
						ldab, &s[1], &scond, &amax, 
						equed);
				    }
				}

/*                          Save the condition number of the */
/*                          non-equilibrated system for use in ZGET04. */

				if (equil) {
				    roldc = rcondc;
				}

/*                          Compute the 1-norm of A. */

				anorm = zlanhb_("1", uplo, &n, &kd, &afac[1], 
					&ldab, &rwork[1]);

/*                          Factor the matrix A. */

				zpbtrf_(uplo, &n, &kd, &afac[1], &ldab, &info);

/*                          Form the inverse of A. */

				zlaset_("Full", &n, &n, &c_b47, &c_b48, &a[1], 
					 &lda);
				s_copy(srnamc_1.srnamt, "ZPBTRS", (ftnlen)6, (
					ftnlen)6);
				zpbtrs_(uplo, &n, &kd, &n, &afac[1], &ldab, &
					a[1], &lda, &info);

/*                          Compute the 1-norm condition number of A. */

				ainvnm = zlange_("1", &n, &n, &a[1], &lda, &
					rwork[1]);
				if (anorm <= 0. || ainvnm <= 0.) {
				    rcondc = 1.;
				} else {
				    rcondc = 1. / anorm / ainvnm;
				}
			    }

/*                       Restore the matrix A. */

			    i__5 = kd + 1;
			    zlacpy_("Full", &i__5, &n, &asav[1], &ldab, &a[1], 
				     &ldab);

/*                       Form an exact solution and set the right hand */
/*                       side. */

			    s_copy(srnamc_1.srnamt, "ZLARHS", (ftnlen)6, (
				    ftnlen)6);
			    zlarhs_(path, xtype, uplo, " ", &n, &n, &kd, &kd, 
				    nrhs, &a[1], &ldab, &xact[1], &lda, &b[1], 
				     &lda, iseed, &info);
			    *(unsigned char *)xtype = 'C';
			    zlacpy_("Full", &n, nrhs, &b[1], &lda, &bsav[1], &
				    lda);

			    if (nofact) {

/*                          --- Test ZPBSV  --- */

/*                          Compute the L*L' or U'*U factorization of the */
/*                          matrix and solve the system. */

				i__5 = kd + 1;
				zlacpy_("Full", &i__5, &n, &a[1], &ldab, &
					afac[1], &ldab);
				zlacpy_("Full", &n, nrhs, &b[1], &lda, &x[1], 
					&lda);

				s_copy(srnamc_1.srnamt, "ZPBSV ", (ftnlen)6, (
					ftnlen)6);
				zpbsv_(uplo, &n, &kd, nrhs, &afac[1], &ldab, &
					x[1], &lda, &info);

/*                          Check error code from ZPBSV . */

				if (info != izero) {
				    alaerh_(path, "ZPBSV ", &info, &izero, 
					    uplo, &n, &n, &kd, &kd, nrhs, &
					    imat, &nfail, &nerrs, nout);
				    goto L40;
				} else if (info != 0) {
				    goto L40;
				}

/*                          Reconstruct matrix from factors and compute */
/*                          residual. */

				zpbt01_(uplo, &n, &kd, &a[1], &ldab, &afac[1], 
					 &ldab, &rwork[1], result);

/*                          Compute residual of the computed solution. */

				zlacpy_("Full", &n, nrhs, &b[1], &lda, &work[
					1], &lda);
				zpbt02_(uplo, &n, &kd, nrhs, &a[1], &ldab, &x[
					1], &lda, &work[1], &lda, &rwork[1], &
					result[1]);

/*                          Check solution from generated exact solution. */

				zget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, 
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
					do_fio(&c__1, "ZPBSV ", (ftnlen)6);
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
						(ftnlen)sizeof(doublereal));
					e_wsfe();
					++nfail;
				    }
/* L30: */
				}
				nrun += nt;
L40:
				;
			    }

/*                       --- Test ZPBSVX --- */

			    if (! prefac) {
				i__5 = kd + 1;
				zlaset_("Full", &i__5, &n, &c_b47, &c_b47, &
					afac[1], &ldab);
			    }
			    zlaset_("Full", &n, nrhs, &c_b47, &c_b47, &x[1], &
				    lda);
			    if (iequed > 1 && n > 0) {

/*                          Equilibrate the matrix if FACT='F' and */
/*                          EQUED='Y' */

				zlaqhb_(uplo, &n, &kd, &a[1], &ldab, &s[1], &
					scond, &amax, equed);
			    }

/*                       Solve the system and compute the condition */
/*                       number and error bounds using ZPBSVX. */

			    s_copy(srnamc_1.srnamt, "ZPBSVX", (ftnlen)6, (
				    ftnlen)6);
			    zpbsvx_(fact, uplo, &n, &kd, nrhs, &a[1], &ldab, &
				    afac[1], &ldab, equed, &s[1], &b[1], &lda, 
				     &x[1], &lda, &rcond, &rwork[1], &rwork[*
				    nrhs + 1], &work[1], &rwork[(*nrhs << 1) 
				    + 1], &info);

/*                       Check the error code from ZPBSVX. */

			    if (info != izero) {
/* Writing concatenation */
				i__7[0] = 1, a__1[0] = fact;
				i__7[1] = 1, a__1[1] = uplo;
				s_cat(ch__1, a__1, i__7, &c__2, (ftnlen)2);
				alaerh_(path, "ZPBSVX", &info, &izero, ch__1, 
					&n, &n, &kd, &kd, nrhs, &imat, &nfail, 
					 &nerrs, nout);
				goto L60;
			    }

			    if (info == 0) {
				if (! prefac) {

/*                             Reconstruct matrix from factors and */
/*                             compute residual. */

				    zpbt01_(uplo, &n, &kd, &a[1], &ldab, &
					    afac[1], &ldab, &rwork[(*nrhs << 
					    1) + 1], result);
				    k1 = 1;
				} else {
				    k1 = 2;
				}

/*                          Compute residual of the computed solution. */

				zlacpy_("Full", &n, nrhs, &bsav[1], &lda, &
					work[1], &lda);
				zpbt02_(uplo, &n, &kd, nrhs, &asav[1], &ldab, 
					&x[1], &lda, &work[1], &lda, &rwork[(*
					nrhs << 1) + 1], &result[1]);

/*                          Check solution from generated exact solution. */

				if (nofact || prefac && lsame_(equed, "N")) {
				    zget04_(&n, nrhs, &x[1], &lda, &xact[1], &
					    lda, &rcondc, &result[2]);
				} else {
				    zget04_(&n, nrhs, &x[1], &lda, &xact[1], &
					    lda, &roldc, &result[2]);
				}

/*                          Check the error bounds from iterative */
/*                          refinement. */

				zpbt05_(uplo, &n, &kd, nrhs, &asav[1], &ldab, 
					&b[1], &lda, &x[1], &lda, &xact[1], &
					lda, &rwork[1], &rwork[*nrhs + 1], &
					result[3]);
			    } else {
				k1 = 6;
			    }

/*                       Compare RCOND from ZPBSVX with the computed */
/*                       value in RCONDC. */

			    result[5] = dget06_(&rcond, &rcondc);

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
					do_fio(&c__1, "ZPBSVX", (ftnlen)6);
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
						(ftnlen)sizeof(doublereal));
					e_wsfe();
				    } else {
					io___61.ciunit = *nout;
					s_wsfe(&io___61);
					do_fio(&c__1, "ZPBSVX", (ftnlen)6);
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
						(ftnlen)sizeof(doublereal));
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

/*     End of ZDRVPB */

} /* zdrvpb_ */
