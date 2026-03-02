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
static doublecomplex c_b23 = {0.,0.};
static integer c__8 = 8;

/* Subroutine */ int zchkge_(logical *dotype, integer *nm, integer *mval, 
	integer *nn, integer *nval, integer *nnb, integer *nbval, integer *
	nns, integer *nsval, doublereal *thresh, logical *tsterr, integer *
	nmax, doublecomplex *a, doublecomplex *afac, doublecomplex *ainv, 
	doublecomplex *b, doublecomplex *x, doublecomplex *xact, 
	doublecomplex *work, doublereal *rwork, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char transs[1*3] = "N" "T" "C";

    /* Format strings */
    static char fmt_9999[] = "(\002 M = \002,i5,\002, N =\002,i5,\002, NB "
	    "=\002,i4,\002, type \002,i2,\002, test(\002,i2,\002) =\002,g12.5)"
	    ;
    static char fmt_9998[] = "(\002 TRANS='\002,a1,\002', N =\002,i5,\002, N"
	    "RHS=\002,i3,\002, type \002,i2,\002, test(\002,i2,\002) =\002,g1"
	    "2.5)";
    static char fmt_9997[] = "(\002 NORM ='\002,a1,\002', N =\002,i5,\002"
	    ",\002,10x,\002 type \002,i2,\002, test(\002,i2,\002) =\002,g12.5)"
	    ;

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, k, m, n, nb, im, in, kl, ku, nt, lda, inb, ioff, mode, imat, 
	    info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char norm[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    doublereal rcond;
    integer nimat;
    extern /* Subroutine */ int zget01_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, integer *, doublereal *, 
	    doublereal *), zget02_(char *, integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *);
    doublereal anorm;
    integer itran;
    extern /* Subroutine */ int zget03_(integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *), zget04_(integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublereal *, doublereal *), zget07_(char *, integer *, integer *
, doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    char trans[1];
    integer izero, nerrs;
    doublereal dummy;
    integer lwork;
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int zlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), alaerh_(char *, 
	    char *, integer *, integer *, char *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *);
    doublereal rcondc, rcondi;
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int alasum_(char *, integer *, integer *, integer 
	    *, integer *);
    doublereal cndnum, anormi, rcondo;
    extern /* Subroutine */ int zgecon_(char *, integer *, doublecomplex *, 
	    integer *, doublereal *, doublereal *, doublecomplex *, 
	    doublereal *, integer *);
    doublereal ainvnm;
    logical trfcon;
    doublereal anormo;
    extern /* Subroutine */ int xlaenv_(integer *, integer *), zerrge_(char *, 
	     integer *), zgerfs_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zgetrf_(integer *, integer *, doublecomplex *, 
	     integer *, integer *, integer *), zlacpy_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *), zlarhs_(char *, char *, char *, char *, integer *, 
	    integer *, integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     integer *, integer *), zgetri_(
	    integer *, doublecomplex *, integer *, integer *, doublecomplex *, 
	     integer *, integer *), zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *), zlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublecomplex *, integer *, doublecomplex *, 
	    integer *);
    doublereal result[8];
    extern /* Subroutine */ int zgetrs_(char *, integer *, integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, integer *, 
	     integer *);

    /* Fortran I/O blocks */
    static cilist io___41 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZCHKGE tests ZGETRF, -TRI, -TRS, -RFS, and -CON. */

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

/*  NBVAL   (input) INTEGER array, dimension (NBVAL) */
/*          The values of the blocksize NB. */

/*  NNS     (input) INTEGER */
/*          The number of values of NRHS contained in the vector NSVAL. */

/*  NSVAL   (input) INTEGER array, dimension (NNS) */
/*          The values of the number of right hand sides NRHS. */

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
/*          The maximum value permitted for M or N, used in dimensioning */
/*          the work arrays. */

/*  A       (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  AFAC    (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  AINV    (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  B       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) COMPLEX*16 array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension */
/*                      (max(2*NMAX,2*NSMAX+NWORK)) */

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
    --mval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "GE", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    xlaenv_(&c__1, &c__1);
    if (*tsterr) {
	zerrge_(path, nout);
    }
    infoc_1.infot = 0;
    xlaenv_(&c__2, &c__2);

/*     Do for each value of M in MVAL */

    i__1 = *nm;
    for (im = 1; im <= i__1; ++im) {
	m = mval[im];
	lda = max(1,m);

/*        Do for each value of N in NVAL */

	i__2 = *nn;
	for (in = 1; in <= i__2; ++in) {
	    n = nval[in];
	    *(unsigned char *)xtype = 'N';
	    nimat = 11;
	    if (m <= 0 || n <= 0) {
		nimat = 1;
	    }

	    i__3 = nimat;
	    for (imat = 1; imat <= i__3; ++imat) {

/*              Do the tests only if DOTYPE( IMAT ) is true. */

		if (! dotype[imat]) {
		    goto L100;
		}

/*              Skip types 5, 6, or 7 if the matrix size is too small. */

		zerot = imat >= 5 && imat <= 7;
		if (zerot && n < imat - 4) {
		    goto L100;
		}

/*              Set up parameters with ZLATB4 and generate a test matrix */
/*              with ZLATMS. */

		zlatb4_(path, &imat, &m, &n, type__, &kl, &ku, &anorm, &mode, 
			&cndnum, dist);

		s_copy(srnamc_1.srnamt, "ZLATMS", (ftnlen)6, (ftnlen)6);
		zlatms_(&m, &n, dist, iseed, type__, &rwork[1], &mode, &
			cndnum, &anorm, &kl, &ku, "No packing", &a[1], &lda, &
			work[1], &info);

/*              Check error code from ZLATMS. */

		if (info != 0) {
		    alaerh_(path, "ZLATMS", &info, &c__0, " ", &m, &n, &c_n1, 
			    &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L100;
		}

/*              For types 5-7, zero one or more columns of the matrix to */
/*              test that INFO is returned correctly. */

		if (zerot) {
		    if (imat == 5) {
			izero = 1;
		    } else if (imat == 6) {
			izero = min(m,n);
		    } else {
			izero = min(m,n) / 2 + 1;
		    }
		    ioff = (izero - 1) * lda;
		    if (imat < 7) {
			i__4 = m;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = ioff + i__;
			    a[i__5].r = 0., a[i__5].i = 0.;
/* L20: */
			}
		    } else {
			i__4 = n - izero + 1;
			zlaset_("Full", &m, &i__4, &c_b23, &c_b23, &a[ioff + 
				1], &lda);
		    }
		} else {
		    izero = 0;
		}

/*              These lines, if used in place of the calls in the DO 60 */
/*              loop, cause the code to bomb on a Sun SPARCstation. */

/*               ANORMO = ZLANGE( 'O', M, N, A, LDA, RWORK ) */
/*               ANORMI = ZLANGE( 'I', M, N, A, LDA, RWORK ) */

/*              Do for each blocksize in NBVAL */

		i__4 = *nnb;
		for (inb = 1; inb <= i__4; ++inb) {
		    nb = nbval[inb];
		    xlaenv_(&c__1, &nb);

/*                 Compute the LU factorization of the matrix. */

		    zlacpy_("Full", &m, &n, &a[1], &lda, &afac[1], &lda);
		    s_copy(srnamc_1.srnamt, "ZGETRF", (ftnlen)6, (ftnlen)6);
		    zgetrf_(&m, &n, &afac[1], &lda, &iwork[1], &info);

/*                 Check error code from ZGETRF. */

		    if (info != izero) {
			alaerh_(path, "ZGETRF", &info, &izero, " ", &m, &n, &
				c_n1, &c_n1, &nb, &imat, &nfail, &nerrs, nout);
		    }
		    trfcon = FALSE_;

/* +    TEST 1 */
/*                 Reconstruct matrix from factors and compute residual. */

		    zlacpy_("Full", &m, &n, &afac[1], &lda, &ainv[1], &lda);
		    zget01_(&m, &n, &a[1], &lda, &ainv[1], &lda, &iwork[1], &
			    rwork[1], result);
		    nt = 1;

/* +    TEST 2 */
/*                 Form the inverse if the factorization was successful */
/*                 and compute the residual. */

		    if (m == n && info == 0) {
			zlacpy_("Full", &n, &n, &afac[1], &lda, &ainv[1], &
				lda);
			s_copy(srnamc_1.srnamt, "ZGETRI", (ftnlen)6, (ftnlen)
				6);
			nrhs = nsval[1];
			lwork = *nmax * max(3,nrhs);
			zgetri_(&n, &ainv[1], &lda, &iwork[1], &work[1], &
				lwork, &info);

/*                    Check error code from ZGETRI. */

			if (info != 0) {
			    alaerh_(path, "ZGETRI", &info, &c__0, " ", &n, &n, 
				     &c_n1, &c_n1, &nb, &imat, &nfail, &nerrs, 
				     nout);
			}

/*                    Compute the residual for the matrix times its */
/*                    inverse.  Also compute the 1-norm condition number */
/*                    of A. */

			zget03_(&n, &a[1], &lda, &ainv[1], &lda, &work[1], &
				lda, &rwork[1], &rcondo, &result[1]);
			anormo = zlange_("O", &m, &n, &a[1], &lda, &rwork[1]);

/*                    Compute the infinity-norm condition number of A. */

			anormi = zlange_("I", &m, &n, &a[1], &lda, &rwork[1]);
			ainvnm = zlange_("I", &n, &n, &ainv[1], &lda, &rwork[
				1]);
			if (anormi <= 0. || ainvnm <= 0.) {
			    rcondi = 1.;
			} else {
			    rcondi = 1. / anormi / ainvnm;
			}
			nt = 2;
		    } else {

/*                    Do only the condition estimate if INFO > 0. */

			trfcon = TRUE_;
			anormo = zlange_("O", &m, &n, &a[1], &lda, &rwork[1]);
			anormi = zlange_("I", &m, &n, &a[1], &lda, &rwork[1]);
			rcondo = 0.;
			rcondi = 0.;
		    }

/*                 Print information about the tests so far that did not */
/*                 pass the threshold. */

		    i__5 = nt;
		    for (k = 1; k <= i__5; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___41.ciunit = *nout;
			    s_wsfe(&io___41);
			    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(integer)
				    );
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				    sizeof(doublereal));
			    e_wsfe();
			    ++nfail;
			}
/* L30: */
		    }
		    nrun += nt;

/*                 Skip the remaining tests if this is not the first */
/*                 block size or if M .ne. N.  Skip the solve tests if */
/*                 the matrix is singular. */

		    if (inb > 1 || m != n) {
			goto L90;
		    }
		    if (trfcon) {
			goto L70;
		    }

		    i__5 = *nns;
		    for (irhs = 1; irhs <= i__5; ++irhs) {
			nrhs = nsval[irhs];
			*(unsigned char *)xtype = 'N';

			for (itran = 1; itran <= 3; ++itran) {
			    *(unsigned char *)trans = *(unsigned char *)&
				    transs[itran - 1];
			    if (itran == 1) {
				rcondc = rcondo;
			    } else {
				rcondc = rcondi;
			    }

/* +    TEST 3 */
/*                       Solve and compute residual for A * X = B. */

			    s_copy(srnamc_1.srnamt, "ZLARHS", (ftnlen)6, (
				    ftnlen)6);
			    zlarhs_(path, xtype, " ", trans, &n, &n, &kl, &ku, 
				     &nrhs, &a[1], &lda, &xact[1], &lda, &b[1]
, &lda, iseed, &info);
			    *(unsigned char *)xtype = 'C';

			    zlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &
				    lda);
			    s_copy(srnamc_1.srnamt, "ZGETRS", (ftnlen)6, (
				    ftnlen)6);
			    zgetrs_(trans, &n, &nrhs, &afac[1], &lda, &iwork[
				    1], &x[1], &lda, &info);

/*                       Check error code from ZGETRS. */

			    if (info != 0) {
				alaerh_(path, "ZGETRS", &info, &c__0, trans, &
					n, &n, &c_n1, &c_n1, &nrhs, &imat, &
					nfail, &nerrs, nout);
			    }

			    zlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], 
				    &lda);
			    zget02_(trans, &n, &n, &nrhs, &a[1], &lda, &x[1], 
				    &lda, &work[1], &lda, &rwork[1], &result[
				    2]);

/* +    TEST 4 */
/*                       Check solution from generated exact solution. */

			    zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[3]);

/* +    TESTS 5, 6, and 7 */
/*                       Use iterative refinement to improve the */
/*                       solution. */

			    s_copy(srnamc_1.srnamt, "ZGERFS", (ftnlen)6, (
				    ftnlen)6);
			    zgerfs_(trans, &n, &nrhs, &a[1], &lda, &afac[1], &
				    lda, &iwork[1], &b[1], &lda, &x[1], &lda, 
				    &rwork[1], &rwork[nrhs + 1], &work[1], &
				    rwork[(nrhs << 1) + 1], &info);

/*                       Check error code from ZGERFS. */

			    if (info != 0) {
				alaerh_(path, "ZGERFS", &info, &c__0, trans, &
					n, &n, &c_n1, &c_n1, &nrhs, &imat, &
					nfail, &nerrs, nout);
			    }

			    zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				    rcondc, &result[4]);
			    zget07_(trans, &n, &nrhs, &a[1], &lda, &b[1], &
				    lda, &x[1], &lda, &xact[1], &lda, &rwork[
				    1], &rwork[nrhs + 1], &result[5]);

/*                       Print information about the tests that did not */
/*                       pass the threshold. */

			    for (k = 3; k <= 7; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					alahd_(nout, path);
				    }
				    io___46.ciunit = *nout;
				    s_wsfe(&io___46);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
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

/* +    TEST 8 */
/*                    Get an estimate of RCOND = 1/CNDNUM. */

L70:
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
			s_copy(srnamc_1.srnamt, "ZGECON", (ftnlen)6, (ftnlen)
				6);
			zgecon_(norm, &n, &afac[1], &lda, &anorm, &rcond, &
				work[1], &rwork[1], &info);

/*                       Check error code from ZGECON. */

			if (info != 0) {
			    alaerh_(path, "ZGECON", &info, &c__0, norm, &n, &
				    n, &c_n1, &c_n1, &c_n1, &imat, &nfail, &
				    nerrs, nout);
			}

/*                       This line is needed on a Sun SPARCstation. */

			dummy = rcond;

			result[7] = dget06_(&rcond, &rcondc);

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			if (result[7] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___50.ciunit = *nout;
			    s_wsfe(&io___50);
			    do_fio(&c__1, norm, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&result[7], (ftnlen)sizeof(
				    doublereal));
			    e_wsfe();
			    ++nfail;
			}
			++nrun;
/* L80: */
		    }
L90:
		    ;
		}
L100:
		;
	    }

/* L110: */
	}
/* L120: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of ZCHKGE */

} /* zchkge_ */
