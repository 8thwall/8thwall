#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer infot, iounit;
    logical ok, lerr;
} infoc_;

#define infoc_1 infoc_

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static doublereal c_b14 = 0.;
static doublereal c_b15 = 1.;
static integer c__1 = 1;
static integer c__0 = 0;
static integer c__3 = 3;
static integer c_n1 = -1;
static integer c__6 = 6;
static integer c__4 = 4;
static integer c__7 = 7;
static integer c__8 = 8;

/* Subroutine */ int dchktb_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, doublereal *thresh, logical *tsterr, 
	integer *nmax, doublereal *ab, doublereal *ainv, doublereal *b, 
	doublereal *x, doublereal *xact, doublereal *work, doublereal *rwork, 
	integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char uplos[1*2] = "U" "L";
    static char transs[1*3] = "N" "T" "C";

    /* Format strings */
    static char fmt_9999[] = "(\002 UPLO='\002,a1,\002', TRANS='\002,a1,\002"
	    "',                        DIAG='\002,a1,\002', N=\002,i5,\002, K"
	    "D=\002,i5,\002, NRHS=\002,i5,\002, type \002,i2,\002, test(\002,"
	    "i2,\002)=\002,g12.5)";
    static char fmt_9998[] = "(1x,a6,\002( '\002,a1,\002', '\002,a1,\002', "
	    "'\002,a1,\002',\002,i5,\002,\002,i5,\002,  ... ), type \002,i2"
	    ",\002, test(\002,i2,\002)=\002,g12.5)";
    static char fmt_9997[] = "(1x,a6,\002( '\002,a1,\002', '\002,a1,\002', "
	    "'\002,a1,\002', '\002,a1,\002',\002,i5,\002,\002,i5,\002, ...  )"
	    ",  type \002,i2,\002, test(\002,i1,\002)=\002,g12.5)";

    /* System generated locals */
    address a__1[3], a__2[4];
    integer i__1, i__2, i__3, i__4, i__5, i__6[3], i__7[4];
    char ch__1[3], ch__2[4];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen), s_cat(char *,
	     char **, integer *, integer *, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n, kd, ik, in, nk, lda, ldab;
    char diag[1];
    integer imat, info;
    char path[3];
    integer irhs, nrhs;
    char norm[1], uplo[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer idiag;
    doublereal scale;
    extern /* Subroutine */ int dget04_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *);
    integer nfail, iseed[4];
    extern /* Subroutine */ int dtbt02_(char *, char *, char *, integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *), dtbt03_(char *, char *, char *, integer *
, integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, doublereal *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dtbt05_(char *, char *, char *, integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *),
	     dtbt06_(doublereal *, doublereal *, char *, char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *);
    doublereal rcond;
    integer nimat;
    doublereal anorm;
    integer itran;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), dtbsv_(char *, char *, char *, integer *
, integer *, doublereal *, integer *, doublereal *, integer *);
    char trans[1];
    integer iuplo, nerrs;
    char xtype[1];
    integer nimat2;
    extern /* Subroutine */ int alaerh_(char *, char *, integer *, integer *, 
	    char *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *);
    extern doublereal dlantb_(char *, char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *);
    doublereal rcondc;
    extern /* Subroutine */ int dlatbs_(char *, char *, char *, char *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *), dlattb_(integer *, char *, char *, char *, integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *), dtbcon_(char *, 
	    char *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *), dlacpy_(char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *), dlarhs_(char *, char 
	    *, char *, char *, integer *, integer *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *);
    doublereal rcondi;
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    alasum_(char *, integer *, integer *, integer *, integer *);
    doublereal rcondo;
    extern doublereal dlantr_(char *, char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dtbrfs_(char *, char *, char *, integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *);
    doublereal ainvnm;
    extern /* Subroutine */ int derrtr_(char *, integer *), dtbtrs_(
	    char *, char *, char *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *);
    doublereal result[8];

    /* Fortran I/O blocks */
    static cilist io___39 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKTB tests DTBTRS, -RFS, and -CON, and DLATBS. */

/*  Arguments */
/*  ========= */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          The matrix types to be used for testing.  Matrices of type j */
/*          (for 1 <= j <= NTYPES) are used for testing if DOTYPE(j) = */
/*          .TRUE.; if DOTYPE(j) = .FALSE., then type j is not used. */

/*  NN      (input) INTEGER */
/*          The number of values of N contained in the vector NVAL. */

/*  NVAL    (input) INTEGER array, dimension (NN) */
/*          The values of the matrix column dimension N. */

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
/*          The leading dimension of the work arrays. */
/*          NMAX >= the maximum value of N in NVAL. */

/*  AB      (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX) */

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
    --ab;
    --nsval;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "TB", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	derrtr_(path, nout);
    }
    infoc_1.infot = 0;

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {

/*        Do for each value of N in NVAL */

	n = nval[in];
	lda = max(1,n);
	*(unsigned char *)xtype = 'N';
	nimat = 9;
	nimat2 = 17;
	if (n <= 0) {
	    nimat = 1;
	    nimat2 = 10;
	}

/* Computing MIN */
	i__2 = n + 1;
	nk = min(i__2,4);
	i__2 = nk;
	for (ik = 1; ik <= i__2; ++ik) {

/*           Do for KD = 0, N, (3N-1)/4, and (N+1)/4. This order makes */
/*           it easier to skip redundant values for small values of N. */

	    if (ik == 1) {
		kd = 0;
	    } else if (ik == 2) {
		kd = max(n,0);
	    } else if (ik == 3) {
		kd = (n * 3 - 1) / 4;
	    } else if (ik == 4) {
		kd = (n + 1) / 4;
	    }
	    ldab = kd + 1;

	    i__3 = nimat;
	    for (imat = 1; imat <= i__3; ++imat) {

/*              Do the tests only if DOTYPE( IMAT ) is true. */

		if (! dotype[imat]) {
		    goto L90;
		}

		for (iuplo = 1; iuplo <= 2; ++iuplo) {

/*                 Do first for UPLO = 'U', then for UPLO = 'L' */

		    *(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 
			    1];

/*                 Call DLATTB to generate a triangular test matrix. */

		    s_copy(srnamc_1.srnamt, "DLATTB", (ftnlen)6, (ftnlen)6);
		    dlattb_(&imat, uplo, "No transpose", diag, iseed, &n, &kd, 
			     &ab[1], &ldab, &x[1], &work[1], &info);

/*                 Set IDIAG = 1 for non-unit matrices, 2 for unit. */

		    if (lsame_(diag, "N")) {
			idiag = 1;
		    } else {
			idiag = 2;
		    }

/*                 Form the inverse of A so we can get a good estimate */
/*                 of RCONDC = 1/(norm(A) * norm(inv(A))). */

		    dlaset_("Full", &n, &n, &c_b14, &c_b15, &ainv[1], &lda);
		    if (lsame_(uplo, "U")) {
			i__4 = n;
			for (j = 1; j <= i__4; ++j) {
			    dtbsv_(uplo, "No transpose", diag, &j, &kd, &ab[1]
, &ldab, &ainv[(j - 1) * lda + 1], &c__1);
/* L20: */
			}
		    } else {
			i__4 = n;
			for (j = 1; j <= i__4; ++j) {
			    i__5 = n - j + 1;
			    dtbsv_(uplo, "No transpose", diag, &i__5, &kd, &
				    ab[(j - 1) * ldab + 1], &ldab, &ainv[(j - 
				    1) * lda + j], &c__1);
/* L30: */
			}
		    }

/*                 Compute the 1-norm condition number of A. */

		    anorm = dlantb_("1", uplo, diag, &n, &kd, &ab[1], &ldab, &
			    rwork[1]);
		    ainvnm = dlantr_("1", uplo, diag, &n, &n, &ainv[1], &lda, 
			    &rwork[1]);
		    if (anorm <= 0. || ainvnm <= 0.) {
			rcondo = 1.;
		    } else {
			rcondo = 1. / anorm / ainvnm;
		    }

/*                 Compute the infinity-norm condition number of A. */

		    anorm = dlantb_("I", uplo, diag, &n, &kd, &ab[1], &ldab, &
			    rwork[1]);
		    ainvnm = dlantr_("I", uplo, diag, &n, &n, &ainv[1], &lda, 
			    &rwork[1]);
		    if (anorm <= 0. || ainvnm <= 0.) {
			rcondi = 1.;
		    } else {
			rcondi = 1. / anorm / ainvnm;
		    }

		    i__4 = *nns;
		    for (irhs = 1; irhs <= i__4; ++irhs) {
			nrhs = nsval[irhs];
			*(unsigned char *)xtype = 'N';

			for (itran = 1; itran <= 3; ++itran) {

/*                    Do for op(A) = A, A**T, or A**H. */

			    *(unsigned char *)trans = *(unsigned char *)&
				    transs[itran - 1];
			    if (itran == 1) {
				*(unsigned char *)norm = 'O';
				rcondc = rcondo;
			    } else {
				*(unsigned char *)norm = 'I';
				rcondc = rcondi;
			    }

/* +    TEST 1 */
/*                    Solve and compute residual for op(A)*x = b. */

			    s_copy(srnamc_1.srnamt, "DLARHS", (ftnlen)6, (
				    ftnlen)6);
			    dlarhs_(path, xtype, uplo, trans, &n, &n, &kd, &
				    idiag, &nrhs, &ab[1], &ldab, &xact[1], &
				    lda, &b[1], &lda, iseed, &info);
			    *(unsigned char *)xtype = 'C';
			    dlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &
				    lda);

			    s_copy(srnamc_1.srnamt, "DTBTRS", (ftnlen)6, (
				    ftnlen)6);
			    dtbtrs_(uplo, trans, diag, &n, &kd, &nrhs, &ab[1], 
				     &ldab, &x[1], &lda, &info);

/*                    Check error code from DTBTRS. */

			    if (info != 0) {
/* Writing concatenation */
				i__6[0] = 1, a__1[0] = uplo;
				i__6[1] = 1, a__1[1] = trans;
				i__6[2] = 1, a__1[2] = diag;
				s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)3);
				alaerh_(path, "DTBTRS", &info, &c__0, ch__1, &
					n, &n, &kd, &kd, &nrhs, &imat, &nfail, 
					 &nerrs, nout);
			    }

			    dtbt02_(uplo, trans, diag, &n, &kd, &nrhs, &ab[1], 
				     &ldab, &x[1], &lda, &b[1], &lda, &work[1]
, result)
				    ;

/* +    TEST 2 */
/*                    Check solution from generated exact solution. */

			    dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[1]);

/* +    TESTS 3, 4, and 5 */
/*                    Use iterative refinement to improve the solution */
/*                    and compute error bounds. */

			    s_copy(srnamc_1.srnamt, "DTBRFS", (ftnlen)6, (
				    ftnlen)6);
			    dtbrfs_(uplo, trans, diag, &n, &kd, &nrhs, &ab[1], 
				     &ldab, &b[1], &lda, &x[1], &lda, &rwork[
				    1], &rwork[nrhs + 1], &work[1], &iwork[1], 
				     &info);

/*                    Check error code from DTBRFS. */

			    if (info != 0) {
/* Writing concatenation */
				i__6[0] = 1, a__1[0] = uplo;
				i__6[1] = 1, a__1[1] = trans;
				i__6[2] = 1, a__1[2] = diag;
				s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)3);
				alaerh_(path, "DTBRFS", &info, &c__0, ch__1, &
					n, &n, &kd, &kd, &nrhs, &imat, &nfail, 
					 &nerrs, nout);
			    }

			    dget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[2]);
			    dtbt05_(uplo, trans, diag, &n, &kd, &nrhs, &ab[1], 
				     &ldab, &b[1], &lda, &x[1], &lda, &xact[1]
, &lda, &rwork[1], &rwork[nrhs + 1], &
				    result[3]);

/*                       Print information about the tests that did not */
/*                       pass the threshold. */

			    for (k = 1; k <= 5; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					alahd_(nout, path);
				    }
				    io___39.ciunit = *nout;
				    s_wsfe(&io___39);
				    do_fio(&c__1, uplo, (ftnlen)1);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, diag, (ftnlen)1);
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
/* L40: */
			    }
			    nrun += 5;
/* L50: */
			}
/* L60: */
		    }

/* +    TEST 6 */
/*                    Get an estimate of RCOND = 1/CNDNUM. */

		    for (itran = 1; itran <= 2; ++itran) {
			if (itran == 1) {
			    *(unsigned char *)norm = 'O';
			    rcondc = rcondo;
			} else {
			    *(unsigned char *)norm = 'I';
			    rcondc = rcondi;
			}
			s_copy(srnamc_1.srnamt, "DTBCON", (ftnlen)6, (ftnlen)
				6);
			dtbcon_(norm, uplo, diag, &n, &kd, &ab[1], &ldab, &
				rcond, &work[1], &iwork[1], &info);

/*                    Check error code from DTBCON. */

			if (info != 0) {
/* Writing concatenation */
			    i__6[0] = 1, a__1[0] = norm;
			    i__6[1] = 1, a__1[1] = uplo;
			    i__6[2] = 1, a__1[2] = diag;
			    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)3);
			    alaerh_(path, "DTBCON", &info, &c__0, ch__1, &n, &
				    n, &kd, &kd, &c_n1, &imat, &nfail, &nerrs, 
				     nout);
			}

			dtbt06_(&rcond, &rcondc, uplo, diag, &n, &kd, &ab[1], 
				&ldab, &rwork[1], &result[5]);

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			if (result[5] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___41.ciunit = *nout;
			    s_wsfe(&io___41);
			    do_fio(&c__1, "DTBCON", (ftnlen)6);
			    do_fio(&c__1, norm, (ftnlen)1);
			    do_fio(&c__1, uplo, (ftnlen)1);
			    do_fio(&c__1, diag, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&result[5], (ftnlen)sizeof(
				    doublereal));
			    e_wsfe();
			    ++nfail;
			}
			++nrun;
/* L70: */
		    }
/* L80: */
		}
L90:
		;
	    }

/*           Use pathological test matrices to test DLATBS. */

	    i__3 = nimat2;
	    for (imat = 10; imat <= i__3; ++imat) {

/*              Do the tests only if DOTYPE( IMAT ) is true. */

		if (! dotype[imat]) {
		    goto L120;
		}

		for (iuplo = 1; iuplo <= 2; ++iuplo) {

/*                 Do first for UPLO = 'U', then for UPLO = 'L' */

		    *(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 
			    1];
		    for (itran = 1; itran <= 3; ++itran) {

/*                    Do for op(A) = A, A**T, and A**H. */

			*(unsigned char *)trans = *(unsigned char *)&transs[
				itran - 1];

/*                    Call DLATTB to generate a triangular test matrix. */

			s_copy(srnamc_1.srnamt, "DLATTB", (ftnlen)6, (ftnlen)
				6);
			dlattb_(&imat, uplo, trans, diag, iseed, &n, &kd, &ab[
				1], &ldab, &x[1], &work[1], &info);

/* +    TEST 7 */
/*                    Solve the system op(A)*x = b */

			s_copy(srnamc_1.srnamt, "DLATBS", (ftnlen)6, (ftnlen)
				6);
			dcopy_(&n, &x[1], &c__1, &b[1], &c__1);
			dlatbs_(uplo, trans, diag, "N", &n, &kd, &ab[1], &
				ldab, &b[1], &scale, &rwork[1], &info);

/*                    Check error code from DLATBS. */

			if (info != 0) {
/* Writing concatenation */
			    i__7[0] = 1, a__2[0] = uplo;
			    i__7[1] = 1, a__2[1] = trans;
			    i__7[2] = 1, a__2[2] = diag;
			    i__7[3] = 1, a__2[3] = "N";
			    s_cat(ch__2, a__2, i__7, &c__4, (ftnlen)4);
			    alaerh_(path, "DLATBS", &info, &c__0, ch__2, &n, &
				    n, &kd, &kd, &c_n1, &imat, &nfail, &nerrs, 
				     nout);
			}

			dtbt03_(uplo, trans, diag, &n, &kd, &c__1, &ab[1], &
				ldab, &scale, &rwork[1], &c_b15, &b[1], &lda, 
				&x[1], &lda, &work[1], &result[6]);

/* +    TEST 8 */
/*                    Solve op(A)*x = b again with NORMIN = 'Y'. */

			dcopy_(&n, &x[1], &c__1, &b[1], &c__1);
			dlatbs_(uplo, trans, diag, "Y", &n, &kd, &ab[1], &
				ldab, &b[1], &scale, &rwork[1], &info);

/*                    Check error code from DLATBS. */

			if (info != 0) {
/* Writing concatenation */
			    i__7[0] = 1, a__2[0] = uplo;
			    i__7[1] = 1, a__2[1] = trans;
			    i__7[2] = 1, a__2[2] = diag;
			    i__7[3] = 1, a__2[3] = "Y";
			    s_cat(ch__2, a__2, i__7, &c__4, (ftnlen)4);
			    alaerh_(path, "DLATBS", &info, &c__0, ch__2, &n, &
				    n, &kd, &kd, &c_n1, &imat, &nfail, &nerrs, 
				     nout);
			}

			dtbt03_(uplo, trans, diag, &n, &kd, &c__1, &ab[1], &
				ldab, &scale, &rwork[1], &c_b15, &b[1], &lda, 
				&x[1], &lda, &work[1], &result[7]);

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			if (result[6] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___43.ciunit = *nout;
			    s_wsfe(&io___43);
			    do_fio(&c__1, "DLATBS", (ftnlen)6);
			    do_fio(&c__1, uplo, (ftnlen)1);
			    do_fio(&c__1, trans, (ftnlen)1);
			    do_fio(&c__1, diag, (ftnlen)1);
			    do_fio(&c__1, "N", (ftnlen)1);
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
			if (result[7] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___44.ciunit = *nout;
			    s_wsfe(&io___44);
			    do_fio(&c__1, "DLATBS", (ftnlen)6);
			    do_fio(&c__1, uplo, (ftnlen)1);
			    do_fio(&c__1, trans, (ftnlen)1);
			    do_fio(&c__1, diag, (ftnlen)1);
			    do_fio(&c__1, "Y", (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&result[7], (ftnlen)sizeof(
				    doublereal));
			    e_wsfe();
			    ++nfail;
			}
			nrun += 2;
/* L100: */
		    }
/* L110: */
		}
L120:
		;
	    }
/* L130: */
	}
/* L140: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of DCHKTB */

} /* dchktb_ */
