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
static doublereal c_b50 = 0.;
static doublereal c_b51 = 1.;
static integer c__7 = 7;

/* Subroutine */ int dchkpb_(logical *dotype, integer *nn, integer *nval, 
	integer *nnb, integer *nbval, integer *nns, integer *nsval, 
	doublereal *thresh, logical *tsterr, integer *nmax, doublereal *a, 
	doublereal *afac, doublereal *ainv, doublereal *b, doublereal *x, 
	doublereal *xact, doublereal *work, doublereal *rwork, integer *iwork, 
	 integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };

    /* Format strings */
    static char fmt_9999[] = "(\002 UPLO='\002,a1,\002', N=\002,i5,\002, KD"
	    "=\002,i5,\002, NB=\002,i4,\002, type \002,i2,\002, test \002,i2"
	    ",\002, ratio= \002,g12.5)";
    static char fmt_9998[] = "(\002 UPLO='\002,a1,\002', N=\002,i5,\002, KD"
	    "=\002,i5,\002, NRHS=\002,i3,\002, type \002,i2,\002, test(\002,i"
	    "2,\002) = \002,g12.5)";
    static char fmt_9997[] = "(\002 UPLO='\002,a1,\002', N=\002,i5,\002, KD"
	    "=\002,i5,\002,\002,10x,\002 type \002,i2,\002, test(\002,i2,\002"
	    ") = \002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5, i__6;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, k, n, i1, i2, kd, nb, in, kl, iw, ku, lda, ikd, inb, nkd, 
	    ldab, ioff, mode, koff, imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char uplo[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), dget04_(
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    extern /* Subroutine */ int dpbt01_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *), dpbt02_(char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *), 
	    dpbt05_(char *, integer *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *);
    integer kdval[4];
    doublereal rcond;
    integer nimat;
    doublereal anorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), dswap_(integer *, doublereal *, integer 
	    *, doublereal *, integer *);
    integer iuplo, izero, nerrs;
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *);
    extern doublereal dlange_(char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int alaerh_(char *, char *, integer *, integer *, 
	    char *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *);
    extern doublereal dlansb_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dpbcon_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *);
    doublereal rcondc;
    char packit[1];
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlarhs_(char *, char *, char *, char *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *), dlaset_(char *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    integer *), dpbrfs_(char *, integer *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), dpbtrf_(char *, 
	    integer *, integer *, doublereal *, integer *, integer *),
	     alasum_(char *, integer *, integer *, integer *, integer *);
    doublereal cndnum;
    extern /* Subroutine */ int dlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublereal *, integer *, doublereal 
	    *, integer *);
    doublereal ainvnm;
    extern /* Subroutine */ int derrpo_(char *, integer *), dpbtrs_(
	    char *, integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, integer *), xlaenv_(integer *, 
	    integer *);
    doublereal result[7];

    /* Fortran I/O blocks */
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKPB tests DPBTRF, -TRS, -RFS, and -CON. */

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

/*  NNB     (input) INTEGER */
/*          The number of values of NB contained in the vector NBVAL. */

/*  NBVAL   (input) INTEGER array, dimension (NBVAL) */
/*          The values of the blocksize NB. */

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

/*  NMAX    (input) INTEGER */
/*          The maximum value permitted for N, used in dimensioning the */
/*          work arrays. */

/*  A       (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX) */

/*  AFAC    (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX) */

/*  AINV    (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX) */

/*  B       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension */
/*                      (max(NMAX,2*NSMAX)) */

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
    --xact;
    --x;
    --b;
    --ainv;
    --afac;
    --a;
    --nsval;
    --nbval;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
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
	derrpo_(path, nout);
    }
    infoc_1.infot = 0;
    xlaenv_(&c__2, &c__2);
    kdval[0] = 0;

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
/* Computing MAX */
		    i__3 = 1, i__4 = kd + 2 - n;
		    koff = max(i__3,i__4);
		    *(unsigned char *)packit = 'Q';
		} else {
		    *(unsigned char *)uplo = 'L';
		    *(unsigned char *)packit = 'B';
		}

		i__3 = nimat;
		for (imat = 1; imat <= i__3; ++imat) {

/*                 Do the tests only if DOTYPE( IMAT ) is true. */

		    if (! dotype[imat]) {
			goto L60;
		    }

/*                 Skip types 2, 3, or 4 if the matrix size is too small. */

		    zerot = imat >= 2 && imat <= 4;
		    if (zerot && n < imat - 1) {
			goto L60;
		    }

		    if (! zerot || ! dotype[1]) {

/*                    Set up parameters with DLATB4 and generate a test */
/*                    matrix with DLATMS. */

			dlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, 
				 &mode, &cndnum, dist);

			s_copy(srnamc_1.srnamt, "DLATMS", (ftnlen)6, (ftnlen)
				6);
			dlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, 
				 &cndnum, &anorm, &kd, &kd, packit, &a[koff], 
				&ldab, &work[1], &info);

/*                    Check error code from DLATMS. */

			if (info != 0) {
			    alaerh_(path, "DLATMS", &info, &c__0, uplo, &n, &
				    n, &kd, &kd, &c_n1, &imat, &nfail, &nerrs, 
				     nout);
			    goto L60;
			}
		    } else if (izero > 0) {

/*                    Use the same matrix for types 3 and 4 as for type */
/*                    2 by copying back the zeroed out column, */

			iw = (lda << 1) + 1;
			if (iuplo == 1) {
			    ioff = (izero - 1) * ldab + kd + 1;
			    i__4 = izero - i1;
			    dcopy_(&i__4, &work[iw], &c__1, &a[ioff - izero + 
				    i1], &c__1);
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    dcopy_(&i__4, &work[iw], &c__1, &a[ioff], &i__5);
			} else {
			    ioff = (i1 - 1) * ldab + 1;
			    i__4 = izero - i1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    dcopy_(&i__4, &work[iw], &c__1, &a[ioff + izero - 
				    i1], &i__5);
			    ioff = (izero - 1) * ldab + 1;
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
			    dcopy_(&i__4, &work[iw], &c__1, &a[ioff], &c__1);
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
			    work[iw + i__] = 0.;
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
			    dswap_(&i__4, &a[ioff - izero + i1], &c__1, &work[
				    iw], &c__1);
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    dswap_(&i__4, &a[ioff], &i__5, &work[iw], &c__1);
			} else {
			    ioff = (i1 - 1) * ldab + 1;
			    i__4 = izero - i1;
/* Computing MAX */
			    i__6 = ldab - 1;
			    i__5 = max(i__6,1);
			    dswap_(&i__4, &a[ioff + izero - i1], &i__5, &work[
				    iw], &c__1);
			    ioff = (izero - 1) * ldab + 1;
			    iw = iw + izero - i1;
			    i__4 = i2 - izero + 1;
			    dswap_(&i__4, &a[ioff], &c__1, &work[iw], &c__1);
			}
		    }

/*                 Do for each value of NB in NBVAL */

		    i__4 = *nnb;
		    for (inb = 1; inb <= i__4; ++inb) {
			nb = nbval[inb];
			xlaenv_(&c__1, &nb);

/*                    Compute the L*L' or U'*U factorization of the band */
/*                    matrix. */

			i__5 = kd + 1;
			dlacpy_("Full", &i__5, &n, &a[1], &ldab, &afac[1], &
				ldab);
			s_copy(srnamc_1.srnamt, "DPBTRF", (ftnlen)6, (ftnlen)
				6);
			dpbtrf_(uplo, &n, &kd, &afac[1], &ldab, &info);

/*                    Check error code from DPBTRF. */

			if (info != izero) {
			    alaerh_(path, "DPBTRF", &info, &izero, uplo, &n, &
				    n, &kd, &kd, &nb, &imat, &nfail, &nerrs, 
				    nout);
			    goto L50;
			}

/*                    Skip the tests if INFO is not 0. */

			if (info != 0) {
			    goto L50;
			}

/* +    TEST 1 */
/*                    Reconstruct matrix from factors and compute */
/*                    residual. */

			i__5 = kd + 1;
			dlacpy_("Full", &i__5, &n, &afac[1], &ldab, &ainv[1], 
				&ldab);
			dpbt01_(uplo, &n, &kd, &a[1], &ldab, &ainv[1], &ldab, 
				&rwork[1], result);

/*                    Print the test ratio if it is .GE. THRESH. */

			if (result[0] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___40.ciunit = *nout;
			    s_wsfe(&io___40);
			    do_fio(&c__1, uplo, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&result[0], (ftnlen)sizeof(
				    doublereal));
			    e_wsfe();
			    ++nfail;
			}
			++nrun;

/*                    Only do other tests if this is the first blocksize. */

			if (inb > 1) {
			    goto L50;
			}

/*                    Form the inverse of A so we can get a good estimate */
/*                    of RCONDC = 1/(norm(A) * norm(inv(A))). */

			dlaset_("Full", &n, &n, &c_b50, &c_b51, &ainv[1], &
				lda);
			s_copy(srnamc_1.srnamt, "DPBTRS", (ftnlen)6, (ftnlen)
				6);
			dpbtrs_(uplo, &n, &kd, &n, &afac[1], &ldab, &ainv[1], 
				&lda, &info);

/*                    Compute RCONDC = 1/(norm(A) * norm(inv(A))). */

			anorm = dlansb_("1", uplo, &n, &kd, &a[1], &ldab, &
				rwork[1]);
			ainvnm = dlange_("1", &n, &n, &ainv[1], &lda, &rwork[
				1]);
			if (anorm <= 0. || ainvnm <= 0.) {
			    rcondc = 1.;
			} else {
			    rcondc = 1. / anorm / ainvnm;
			}

			i__5 = *nns;
			for (irhs = 1; irhs <= i__5; ++irhs) {
			    nrhs = nsval[irhs];

/* +    TEST 2 */
/*                    Solve and compute residual for A * X = B. */

			    s_copy(srnamc_1.srnamt, "DLARHS", (ftnlen)6, (
				    ftnlen)6);
			    dlarhs_(path, xtype, uplo, " ", &n, &n, &kd, &kd, 
				    &nrhs, &a[1], &ldab, &xact[1], &lda, &b[1]
, &lda, iseed, &info);
			    dlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &
				    lda);

			    s_copy(srnamc_1.srnamt, "DPBTRS", (ftnlen)6, (
				    ftnlen)6);
			    dpbtrs_(uplo, &n, &kd, &nrhs, &afac[1], &ldab, &x[
				    1], &lda, &info);

/*                    Check error code from DPBTRS. */

			    if (info != 0) {
				alaerh_(path, "DPBTRS", &info, &c__0, uplo, &
					n, &n, &kd, &kd, &nrhs, &imat, &nfail, 
					 &nerrs, nout);
			    }

			    dlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], 
				    &lda);
			    dpbt02_(uplo, &n, &kd, &nrhs, &a[1], &ldab, &x[1], 
				     &lda, &work[1], &lda, &rwork[1], &result[
				    1]);

/* +    TEST 3 */
/*                    Check solution from generated exact solution. */

			    dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[2]);

/* +    TESTS 4, 5, and 6 */
/*                    Use iterative refinement to improve the solution. */

			    s_copy(srnamc_1.srnamt, "DPBRFS", (ftnlen)6, (
				    ftnlen)6);
			    dpbrfs_(uplo, &n, &kd, &nrhs, &a[1], &ldab, &afac[
				    1], &ldab, &b[1], &lda, &x[1], &lda, &
				    rwork[1], &rwork[nrhs + 1], &work[1], &
				    iwork[1], &info);

/*                    Check error code from DPBRFS. */

			    if (info != 0) {
				alaerh_(path, "DPBRFS", &info, &c__0, uplo, &
					n, &n, &kd, &kd, &nrhs, &imat, &nfail, 
					 &nerrs, nout);
			    }

			    dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[3]);
			    dpbt05_(uplo, &n, &kd, &nrhs, &a[1], &ldab, &b[1], 
				     &lda, &x[1], &lda, &xact[1], &lda, &
				    rwork[1], &rwork[nrhs + 1], &result[4]);

/*                       Print information about the tests that did not */
/*                       pass the threshold. */

			    for (k = 2; k <= 6; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					alahd_(nout, path);
				    }
				    io___46.ciunit = *nout;
				    s_wsfe(&io___46);
				    do_fio(&c__1, uplo, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&nrhs, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&result[k - 1], (
					    ftnlen)sizeof(doublereal));
				    e_wsfe();
				    ++nfail;
				}
/* L30: */
			    }
			    nrun += 5;
/* L40: */
			}

/* +    TEST 7 */
/*                    Get an estimate of RCOND = 1/CNDNUM. */

			s_copy(srnamc_1.srnamt, "DPBCON", (ftnlen)6, (ftnlen)
				6);
			dpbcon_(uplo, &n, &kd, &afac[1], &ldab, &anorm, &
				rcond, &work[1], &iwork[1], &info);

/*                    Check error code from DPBCON. */

			if (info != 0) {
			    alaerh_(path, "DPBCON", &info, &c__0, uplo, &n, &
				    n, &kd, &kd, &c_n1, &imat, &nfail, &nerrs, 
				     nout);
			}

			result[6] = dget06_(&rcond, &rcondc);

/*                    Print the test ratio if it is .GE. THRESH. */

			if (result[6] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___48.ciunit = *nout;
			    s_wsfe(&io___48);
			    do_fio(&c__1, uplo, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&result[6], (ftnlen)sizeof(
				    doublereal));
			    e_wsfe();
			    ++nfail;
			}
			++nrun;
L50:
			;
		    }
L60:
		    ;
		}
/* L70: */
	    }
/* L80: */
	}
/* L90: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of DCHKPB */

} /* dchkpb_ */
