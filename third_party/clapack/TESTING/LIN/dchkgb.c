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
static integer c__1 = 1;
static integer c__0 = 0;
static integer c_n1 = -1;
static doublereal c_b63 = 0.;
static doublereal c_b64 = 1.;
static integer c__7 = 7;

/* Subroutine */ int dchkgb_(logical *dotype, integer *nm, integer *mval, 
	integer *nn, integer *nval, integer *nnb, integer *nbval, integer *
	nns, integer *nsval, doublereal *thresh, logical *tsterr, doublereal *
	a, integer *la, doublereal *afac, integer *lafac, doublereal *b, 
	doublereal *x, doublereal *xact, doublereal *work, doublereal *rwork, 
	integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char transs[1*3] = "N" "T" "C";

    /* Format strings */
    static char fmt_9999[] = "(\002 *** In DCHKGB, LA=\002,i5,\002 is too sm"
	    "all for M=\002,i5,\002, N=\002,i5,\002, KL=\002,i4,\002, KU=\002"
	    ",i4,/\002 ==> Increase LA to at least \002,i5)";
    static char fmt_9998[] = "(\002 *** In DCHKGB, LAFAC=\002,i5,\002 is too"
	    " small for M=\002,i5,\002, N=\002,i5,\002, KL=\002,i4,\002, KU"
	    "=\002,i4,/\002 ==> Increase LAFAC to at least \002,i5)";
    static char fmt_9997[] = "(\002 M =\002,i5,\002, N =\002,i5,\002, KL="
	    "\002,i5,\002, KU=\002,i5,\002, NB =\002,i4,\002, type \002,i1"
	    ",\002, test(\002,i1,\002)=\002,g12.5)";
    static char fmt_9996[] = "(\002 TRANS='\002,a1,\002', N=\002,i5,\002, "
	    "KL=\002,i5,\002, KU=\002,i5,\002, NRHS=\002,i3,\002, type \002,i"
	    "1,\002, test(\002,i1,\002)=\002,g12.5)";
    static char fmt_9995[] = "(\002 NORM ='\002,a1,\002', N=\002,i5,\002, "
	    "KL=\002,i5,\002, KU=\002,i5,\002,\002,10x,\002 type \002,i1,\002"
	    ", test(\002,i1,\002)=\002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5, i__6, i__7, i__8, i__9, i__10, 
	    i__11;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, m, n, i1, i2, nb, im, in, kl, ku, lda, ldb, inb, ikl, 
	    nkl, iku, nku, ioff, mode, koff, imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char norm[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *), dgbt01_(
	    integer *, integer *, integer *, integer *, doublereal *, integer 
	    *, doublereal *, integer *, integer *, doublereal *, doublereal *)
	    , dgbt02_(char *, integer *, integer *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *), dgbt05_(char *, 
	    integer *, integer *, integer *, integer *, doublereal *, integer 
	    *, doublereal *, integer *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, doublereal *, doublereal *), 
	    dget04_(integer *, integer *, doublereal *, integer *, doublereal 
	    *, integer *, doublereal *, doublereal *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    doublereal rcond;
    integer nimat, klval[4];
    doublereal anorm;
    integer itran;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    integer kuval[4];
    char trans[1];
    integer izero, nerrs;
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int dlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *);
    integer ldafac;
    extern doublereal dlangb_(char *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *), dlange_(char *, 
	    integer *, integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int alaerh_(char *, char *, integer *, integer *, 
	    char *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *), dgbcon_(char *, integer *, integer *, integer *, 
	    doublereal *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), dgbrfs_(char *, 
	    integer *, integer *, integer *, integer *, doublereal *, integer 
	    *, doublereal *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *);
    doublereal rcondc;
    extern /* Subroutine */ int derrge_(char *, integer *), dgbtrf_(
	    integer *, integer *, integer *, integer *, doublereal *, integer 
	    *, integer *, integer *), dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlarhs_(char *, char *, char *, char *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *);
    doublereal rcondi;
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    alasum_(char *, integer *, integer *, integer *, integer *);
    doublereal cndnum, anormi, rcondo;
    extern /* Subroutine */ int dgbtrs_(char *, integer *, integer *, integer 
	    *, integer *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *);
    doublereal ainvnm;
    extern /* Subroutine */ int dlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublereal *, integer *, doublereal 
	    *, integer *);
    logical trfcon;
    doublereal anormo;
    extern /* Subroutine */ int xlaenv_(integer *, integer *);
    doublereal result[7];

    /* Fortran I/O blocks */
    static cilist io___25 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9995, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKGB tests DGBTRF, -TRS, -RFS, and -CON */

/*  Arguments */
/*  ========= */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          The matrix types to be used for testing.  Matrices of type j */
/*          (for 1 <= j <= NTYPES) are used for testing if DOTYPE(j) = */
/*          .TRUE.; if DOTYPE(j) = .FALSE., then type j is not used. */

/*  NM      (input) INTEGER */
/*          The number of values of M contained in the vector MVAL. */

/*  MVAL    (input) INTEGER array, dimension (NM) */
/*          The values of the matrix row dimension M. */

/*  NN      (input) INTEGER */
/*          The number of values of N contained in the vector NVAL. */

/*  NVAL    (input) INTEGER array, dimension (NN) */
/*          The values of the matrix column dimension N. */

/*  NNB     (input) INTEGER */
/*          The number of values of NB contained in the vector NBVAL. */

/*  NBVAL   (input) INTEGER array, dimension (NNB) */
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

/*  A       (workspace) DOUBLE PRECISION array, dimension (LA) */

/*  LA      (input) INTEGER */
/*          The length of the array A.  LA >= (KLMAX+KUMAX+1)*NMAX */
/*          where KLMAX is the largest entry in the local array KLVAL, */
/*                KUMAX is the largest entry in the local array KUVAL and */
/*                NMAX is the largest entry in the input array NVAL. */

/*  AFAC    (workspace) DOUBLE PRECISION array, dimension (LAFAC) */

/*  LAFAC   (input) INTEGER */
/*          The length of the array AFAC. LAFAC >= (2*KLMAX+KUMAX+1)*NMAX */
/*          where KLMAX is the largest entry in the local array KLVAL, */
/*                KUMAX is the largest entry in the local array KUVAL and */
/*                NMAX is the largest entry in the input array NVAL. */

/*  B       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (NMAX*max(3,NSMAX,NMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension */
/*                      (max(NMAX,2*NSMAX)) */

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
    --afac;
    --a;
    --nsval;
    --nbval;
    --nval;
    --mval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "GB", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	derrge_(path, nout);
    }
    infoc_1.infot = 0;
    xlaenv_(&c__2, &c__2);

/*     Initialize the first value for the lower and upper bandwidths. */

    klval[0] = 0;
    kuval[0] = 0;

/*     Do for each value of M in MVAL */

    i__1 = *nm;
    for (im = 1; im <= i__1; ++im) {
	m = mval[im];

/*        Set values to use for the lower bandwidth. */

	klval[1] = m + (m + 1) / 4;

/*        KLVAL( 2 ) = MAX( M-1, 0 ) */

	klval[2] = (m * 3 - 1) / 4;
	klval[3] = (m + 1) / 4;

/*        Do for each value of N in NVAL */

	i__2 = *nn;
	for (in = 1; in <= i__2; ++in) {
	    n = nval[in];
	    *(unsigned char *)xtype = 'N';

/*           Set values to use for the upper bandwidth. */

	    kuval[1] = n + (n + 1) / 4;

/*           KUVAL( 2 ) = MAX( N-1, 0 ) */

	    kuval[2] = (n * 3 - 1) / 4;
	    kuval[3] = (n + 1) / 4;

/*           Set limits on the number of loop iterations. */

/* Computing MIN */
	    i__3 = m + 1;
	    nkl = min(i__3,4);
	    if (n == 0) {
		nkl = 2;
	    }
/* Computing MIN */
	    i__3 = n + 1;
	    nku = min(i__3,4);
	    if (m == 0) {
		nku = 2;
	    }
	    nimat = 8;
	    if (m <= 0 || n <= 0) {
		nimat = 1;
	    }

	    i__3 = nkl;
	    for (ikl = 1; ikl <= i__3; ++ikl) {

/*              Do for KL = 0, (5*M+1)/4, (3M-1)/4, and (M+1)/4. This */
/*              order makes it easier to skip redundant values for small */
/*              values of M. */

		kl = klval[ikl - 1];
		i__4 = nku;
		for (iku = 1; iku <= i__4; ++iku) {

/*                 Do for KU = 0, (5*N+1)/4, (3N-1)/4, and (N+1)/4. This */
/*                 order makes it easier to skip redundant values for */
/*                 small values of N. */

		    ku = kuval[iku - 1];

/*                 Check that A and AFAC are big enough to generate this */
/*                 matrix. */

		    lda = kl + ku + 1;
		    ldafac = (kl << 1) + ku + 1;
		    if (lda * n > *la || ldafac * n > *lafac) {
			if (nfail == 0 && nerrs == 0) {
			    alahd_(nout, path);
			}
			if (n * (kl + ku + 1) > *la) {
			    io___25.ciunit = *nout;
			    s_wsfe(&io___25);
			    do_fio(&c__1, (char *)&(*la), (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(integer)
				    );
			    i__5 = n * (kl + ku + 1);
			    do_fio(&c__1, (char *)&i__5, (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    ++nerrs;
			}
			if (n * ((kl << 1) + ku + 1) > *lafac) {
			    io___26.ciunit = *nout;
			    s_wsfe(&io___26);
			    do_fio(&c__1, (char *)&(*lafac), (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(integer)
				    );
			    i__5 = n * ((kl << 1) + ku + 1);
			    do_fio(&c__1, (char *)&i__5, (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    ++nerrs;
			}
			goto L130;
		    }

		    i__5 = nimat;
		    for (imat = 1; imat <= i__5; ++imat) {

/*                    Do the tests only if DOTYPE( IMAT ) is true. */

			if (! dotype[imat]) {
			    goto L120;
			}

/*                    Skip types 2, 3, or 4 if the matrix size is too */
/*                    small. */

			zerot = imat >= 2 && imat <= 4;
			if (zerot && n < imat - 1) {
			    goto L120;
			}

			if (! zerot || ! dotype[1]) {

/*                       Set up parameters with DLATB4 and generate a */
/*                       test matrix with DLATMS. */

			    dlatb4_(path, &imat, &m, &n, type__, &kl, &ku, &
				    anorm, &mode, &cndnum, dist);

/* Computing MAX */
			    i__6 = 1, i__7 = ku + 2 - n;
			    koff = max(i__6,i__7);
			    i__6 = koff - 1;
			    for (i__ = 1; i__ <= i__6; ++i__) {
				a[i__] = 0.;
/* L20: */
			    }
			    s_copy(srnamc_1.srnamt, "DLATMS", (ftnlen)6, (
				    ftnlen)6);
			    dlatms_(&m, &n, dist, iseed, type__, &rwork[1], &
				    mode, &cndnum, &anorm, &kl, &ku, "Z", &a[
				    koff], &lda, &work[1], &info);

/*                       Check the error code from DLATMS. */

			    if (info != 0) {
				alaerh_(path, "DLATMS", &info, &c__0, " ", &m, 
					 &n, &kl, &ku, &c_n1, &imat, &nfail, &
					nerrs, nout);
				goto L120;
			    }
			} else if (izero > 0) {

/*                       Use the same matrix for types 3 and 4 as for */
/*                       type 2 by copying back the zeroed out column. */

			    i__6 = i2 - i1 + 1;
			    dcopy_(&i__6, &b[1], &c__1, &a[ioff + i1], &c__1);
			}

/*                    For types 2, 3, and 4, zero one or more columns of */
/*                    the matrix to test that INFO is returned correctly. */

			izero = 0;
			if (zerot) {
			    if (imat == 2) {
				izero = 1;
			    } else if (imat == 3) {
				izero = min(m,n);
			    } else {
				izero = min(m,n) / 2 + 1;
			    }
			    ioff = (izero - 1) * lda;
			    if (imat < 4) {

/*                          Store the column to be zeroed out in B. */

/* Computing MAX */
				i__6 = 1, i__7 = ku + 2 - izero;
				i1 = max(i__6,i__7);
/* Computing MIN */
				i__6 = kl + ku + 1, i__7 = ku + 1 + (m - 
					izero);
				i2 = min(i__6,i__7);
				i__6 = i2 - i1 + 1;
				dcopy_(&i__6, &a[ioff + i1], &c__1, &b[1], &
					c__1);

				i__6 = i2;
				for (i__ = i1; i__ <= i__6; ++i__) {
				    a[ioff + i__] = 0.;
/* L30: */
				}
			    } else {
				i__6 = n;
				for (j = izero; j <= i__6; ++j) {
/* Computing MAX */
				    i__7 = 1, i__8 = ku + 2 - j;
/* Computing MIN */
				    i__10 = kl + ku + 1, i__11 = ku + 1 + (m 
					    - j);
				    i__9 = min(i__10,i__11);
				    for (i__ = max(i__7,i__8); i__ <= i__9; 
					    ++i__) {
					a[ioff + i__] = 0.;
/* L40: */
				    }
				    ioff += lda;
/* L50: */
				}
			    }
			}

/*                    These lines, if used in place of the calls in the */
/*                    loop over INB, cause the code to bomb on a Sun */
/*                    SPARCstation. */

/*                     ANORMO = DLANGB( 'O', N, KL, KU, A, LDA, RWORK ) */
/*                     ANORMI = DLANGB( 'I', N, KL, KU, A, LDA, RWORK ) */

/*                    Do for each blocksize in NBVAL */

			i__6 = *nnb;
			for (inb = 1; inb <= i__6; ++inb) {
			    nb = nbval[inb];
			    xlaenv_(&c__1, &nb);

/*                       Compute the LU factorization of the band matrix. */

			    if (m > 0 && n > 0) {
				i__9 = kl + ku + 1;
				dlacpy_("Full", &i__9, &n, &a[1], &lda, &afac[
					kl + 1], &ldafac);
			    }
			    s_copy(srnamc_1.srnamt, "DGBTRF", (ftnlen)6, (
				    ftnlen)6);
			    dgbtrf_(&m, &n, &kl, &ku, &afac[1], &ldafac, &
				    iwork[1], &info);

/*                       Check error code from DGBTRF. */

			    if (info != izero) {
				alaerh_(path, "DGBTRF", &info, &izero, " ", &
					m, &n, &kl, &ku, &nb, &imat, &nfail, &
					nerrs, nout);
			    }
			    trfcon = FALSE_;

/* +    TEST 1 */
/*                       Reconstruct matrix from factors and compute */
/*                       residual. */

			    dgbt01_(&m, &n, &kl, &ku, &a[1], &lda, &afac[1], &
				    ldafac, &iwork[1], &work[1], result);

/*                       Print information about the tests so far that */
/*                       did not pass the threshold. */

			    if (result[0] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    alahd_(nout, path);
				}
				io___45.ciunit = *nout;
				s_wsfe(&io___45);
				do_fio(&c__1, (char *)&m, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[0], (ftnlen)
					sizeof(doublereal));
				e_wsfe();
				++nfail;
			    }
			    ++nrun;

/*                       Skip the remaining tests if this is not the */
/*                       first block size or if M .ne. N. */

			    if (inb > 1 || m != n) {
				goto L110;
			    }

			    anormo = dlangb_("O", &n, &kl, &ku, &a[1], &lda, &
				    rwork[1]);
			    anormi = dlangb_("I", &n, &kl, &ku, &a[1], &lda, &
				    rwork[1]);

			    if (info == 0) {

/*                          Form the inverse of A so we can get a good */
/*                          estimate of CNDNUM = norm(A) * norm(inv(A)). */

				ldb = max(1,n);
				dlaset_("Full", &n, &n, &c_b63, &c_b64, &work[
					1], &ldb);
				s_copy(srnamc_1.srnamt, "DGBTRS", (ftnlen)6, (
					ftnlen)6);
				dgbtrs_("No transpose", &n, &kl, &ku, &n, &
					afac[1], &ldafac, &iwork[1], &work[1], 
					 &ldb, &info);

/*                          Compute the 1-norm condition number of A. */

				ainvnm = dlange_("O", &n, &n, &work[1], &ldb, 
					&rwork[1]);
				if (anormo <= 0. || ainvnm <= 0.) {
				    rcondo = 1.;
				} else {
				    rcondo = 1. / anormo / ainvnm;
				}

/*                          Compute the infinity-norm condition number of */
/*                          A. */

				ainvnm = dlange_("I", &n, &n, &work[1], &ldb, 
					&rwork[1]);
				if (anormi <= 0. || ainvnm <= 0.) {
				    rcondi = 1.;
				} else {
				    rcondi = 1. / anormi / ainvnm;
				}
			    } else {

/*                          Do only the condition estimate if INFO.NE.0. */

				trfcon = TRUE_;
				rcondo = 0.;
				rcondi = 0.;
			    }

/*                       Skip the solve tests if the matrix is singular. */

			    if (trfcon) {
				goto L90;
			    }

			    i__9 = *nns;
			    for (irhs = 1; irhs <= i__9; ++irhs) {
				nrhs = nsval[irhs];
				*(unsigned char *)xtype = 'N';

				for (itran = 1; itran <= 3; ++itran) {
				    *(unsigned char *)trans = *(unsigned char 
					    *)&transs[itran - 1];
				    if (itran == 1) {
					rcondc = rcondo;
					*(unsigned char *)norm = 'O';
				    } else {
					rcondc = rcondi;
					*(unsigned char *)norm = 'I';
				    }

/* +    TEST 2: */
/*                             Solve and compute residual for A * X = B. */

				    s_copy(srnamc_1.srnamt, "DLARHS", (ftnlen)
					    6, (ftnlen)6);
				    dlarhs_(path, xtype, " ", trans, &n, &n, &
					    kl, &ku, &nrhs, &a[1], &lda, &
					    xact[1], &ldb, &b[1], &ldb, iseed, 
					     &info);
				    *(unsigned char *)xtype = 'C';
				    dlacpy_("Full", &n, &nrhs, &b[1], &ldb, &
					    x[1], &ldb);

				    s_copy(srnamc_1.srnamt, "DGBTRS", (ftnlen)
					    6, (ftnlen)6);
				    dgbtrs_(trans, &n, &kl, &ku, &nrhs, &afac[
					    1], &ldafac, &iwork[1], &x[1], &
					    ldb, &info);

/*                             Check error code from DGBTRS. */

				    if (info != 0) {
					alaerh_(path, "DGBTRS", &info, &c__0, 
						trans, &n, &n, &kl, &ku, &
						c_n1, &imat, &nfail, &nerrs, 
						nout);
				    }

				    dlacpy_("Full", &n, &nrhs, &b[1], &ldb, &
					    work[1], &ldb);
				    dgbt02_(trans, &m, &n, &kl, &ku, &nrhs, &
					    a[1], &lda, &x[1], &ldb, &work[1], 
					     &ldb, &result[1]);

/* +    TEST 3: */
/*                             Check solution from generated exact */
/*                             solution. */

				    dget04_(&n, &nrhs, &x[1], &ldb, &xact[1], 
					    &ldb, &rcondc, &result[2]);

/* +    TESTS 4, 5, 6: */
/*                             Use iterative refinement to improve the */
/*                             solution. */

				    s_copy(srnamc_1.srnamt, "DGBRFS", (ftnlen)
					    6, (ftnlen)6);
				    dgbrfs_(trans, &n, &kl, &ku, &nrhs, &a[1], 
					     &lda, &afac[1], &ldafac, &iwork[
					    1], &b[1], &ldb, &x[1], &ldb, &
					    rwork[1], &rwork[nrhs + 1], &work[
					    1], &iwork[n + 1], &info);

/*                             Check error code from DGBRFS. */

				    if (info != 0) {
					alaerh_(path, "DGBRFS", &info, &c__0, 
						trans, &n, &n, &kl, &ku, &
						nrhs, &imat, &nfail, &nerrs, 
						nout);
				    }

				    dget04_(&n, &nrhs, &x[1], &ldb, &xact[1], 
					    &ldb, &rcondc, &result[3]);
				    dgbt05_(trans, &n, &kl, &ku, &nrhs, &a[1], 
					     &lda, &b[1], &ldb, &x[1], &ldb, &
					    xact[1], &ldb, &rwork[1], &rwork[
					    nrhs + 1], &result[4]);
				    for (k = 2; k <= 6; ++k) {
					if (result[k - 1] >= *thresh) {
					    if (nfail == 0 && nerrs == 0) {
			  alahd_(nout, path);
					    }
					    io___59.ciunit = *nout;
					    s_wsfe(&io___59);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&nrhs, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&k, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&result[k - 
						    1], (ftnlen)sizeof(
						    doublereal));
					    e_wsfe();
					    ++nfail;
					}
/* L60: */
				    }
				    nrun += 5;
/* L70: */
				}
/* L80: */
			    }

/* +    TEST 7: */
/*                          Get an estimate of RCOND = 1/CNDNUM. */

L90:
			    for (itran = 1; itran <= 2; ++itran) {
				if (itran == 1) {
				    anorm = anormo;
				    rcondc = rcondo;
				    *(unsigned char *)norm = 'O';
				} else {
				    anorm = anormi;
				    rcondc = rcondi;
				    *(unsigned char *)norm = 'I';
				}
				s_copy(srnamc_1.srnamt, "DGBCON", (ftnlen)6, (
					ftnlen)6);
				dgbcon_(norm, &n, &kl, &ku, &afac[1], &ldafac, 
					 &iwork[1], &anorm, &rcond, &work[1], 
					&iwork[n + 1], &info);

/*                             Check error code from DGBCON. */

				if (info != 0) {
				    alaerh_(path, "DGBCON", &info, &c__0, 
					    norm, &n, &n, &kl, &ku, &c_n1, &
					    imat, &nfail, &nerrs, nout);
				}

				result[6] = dget06_(&rcond, &rcondc);

/*                          Print information about the tests that did */
/*                          not pass the threshold. */

				if (result[6] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					alahd_(nout, path);
				    }
				    io___61.ciunit = *nout;
				    s_wsfe(&io___61);
				    do_fio(&c__1, norm, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&c__7, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&result[6], (ftnlen)
					    sizeof(doublereal));
				    e_wsfe();
				    ++nfail;
				}
				++nrun;
/* L100: */
			    }

L110:
			    ;
			}
L120:
			;
		    }
L130:
		    ;
		}
/* L140: */
	    }
/* L150: */
	}
/* L160: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);


    return 0;

/*     End of DCHKGB */

} /* dchkgb_ */
