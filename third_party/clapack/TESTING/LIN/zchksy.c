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
static integer c__8 = 8;

/* Subroutine */ int zchksy_(logical *dotype, integer *nn, integer *nval, 
	integer *nnb, integer *nbval, integer *nns, integer *nsval, 
	doublereal *thresh, logical *tsterr, integer *nmax, doublecomplex *a, 
	doublecomplex *afac, doublecomplex *ainv, doublecomplex *b, 
	doublecomplex *x, doublecomplex *xact, doublecomplex *work, 
	doublereal *rwork, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char uplos[1*2] = "U" "L";

    /* Format strings */
    static char fmt_9999[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002, "
	    "NB =\002,i4,\002, type \002,i2,\002, test \002,i2,\002, ratio "
	    "=\002,g12.5)";
    static char fmt_9998[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002, "
	    "NRHS=\002,i3,\002, type \002,i2,\002, test(\002,i2,\002) =\002,g"
	    "12.5)";
    static char fmt_9997[] = "(\002 UPLO = '\002,a1,\002', N =\002,i5,\002"
	    ",\002,10x,\002 type \002,i2,\002, test(\002,i2,\002) =\002,g12.5)"
	    ;

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n, i1, i2, nb, in, kl, ku, nt, lda, inb, ioff, mode, 
	    imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char uplo[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    doublereal rcond;
    integer nimat;
    doublereal anorm;
    extern /* Subroutine */ int zget04_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
);
    integer iuplo, izero, nerrs, lwork;
    extern /* Subroutine */ int zpot05_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int zsyt01_(char *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, integer *, doublecomplex *, 
	     integer *, doublereal *, doublereal *), zsyt02_(char *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
), zsyt03_(char *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *), zlatb4_(char *, 
	     integer *, integer *, integer *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, char *), alaerh_(char *, char *, integer *, integer *, char *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *);
    doublereal rcondc;
    extern /* Subroutine */ int alasum_(char *, integer *, integer *, integer 
	    *, integer *);
    doublereal cndnum;
    logical trfcon;
    extern /* Subroutine */ int xlaenv_(integer *, integer *), zlacpy_(char *, 
	     integer *, integer *, doublecomplex *, integer *, doublecomplex *
, integer *), zlarhs_(char *, char *, char *, char *, 
	    integer *, integer *, integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *, integer *), zlatms_(integer *, integer *, char *, integer *, 
	    char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    doublereal result[8];
    extern doublereal zlansy_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int zsycon_(char *, integer *, doublecomplex *, 
	    integer *, integer *, doublereal *, doublereal *, doublecomplex *, 
	     integer *), zlatsy_(char *, integer *, doublecomplex *, 
	    integer *, integer *), zerrsy_(char *, integer *),
	     zsyrfs_(char *, integer *, integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, integer *, doublecomplex *, integer *
, doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, doublereal *, integer *), zsytrf_(char *, 
	     integer *, doublecomplex *, integer *, integer *, doublecomplex *
, integer *, integer *), zsytri_(char *, integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, integer *), zsytrs_(char *, integer *, integer *, doublecomplex *, 
	    integer *, integer *, doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___39 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9998, 0 };
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

/*  ZCHKSY tests ZSYTRF, -TRI, -TRS, -RFS, and -CON. */

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

/*  A       (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  AFAC    (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  AINV    (workspace) COMPLEX*16 array, dimension (NMAX*NMAX) */

/*  B       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) COMPLEX*16 array, dimension */
/*                      (NMAX*max(2,NSMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, */
/*                                 dimension (NMAX+2*NSMAX) */

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

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "SY", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }

/*     Test the error exits */

    if (*tsterr) {
	zerrsy_(path, nout);
    }
    infoc_1.infot = 0;

/*     Do for each value of N in NVAL */

    i__1 = *nn;
    for (in = 1; in <= i__1; ++in) {
	n = nval[in];
	lda = max(n,1);
	*(unsigned char *)xtype = 'N';
	nimat = 11;
	if (n <= 0) {
	    nimat = 1;
	}

	izero = 0;
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
		*(unsigned char *)uplo = *(unsigned char *)&uplos[iuplo - 1];

		if (imat != 11) {

/*                 Set up parameters with ZLATB4 and generate a test */
/*                 matrix with ZLATMS. */

		    zlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &
			    mode, &cndnum, dist);

		    s_copy(srnamc_1.srnamt, "ZLATMS", (ftnlen)6, (ftnlen)6);
		    zlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &
			    cndnum, &anorm, &kl, &ku, "N", &a[1], &lda, &work[
			    1], &info);

/*                 Check error code from ZLATMS. */

		    if (info != 0) {
			alaerh_(path, "ZLATMS", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &c_n1, &imat, &nfail, &nerrs, 
				nout);
			goto L160;
		    }

/*                 For types 3-6, zero one or more rows and columns of */
/*                 the matrix to test that INFO is returned correctly. */

		    if (zerot) {
			if (imat == 3) {
			    izero = 1;
			} else if (imat == 4) {
			    izero = n;
			} else {
			    izero = n / 2 + 1;
			}

			if (imat < 6) {

/*                       Set row and column IZERO to zero. */

			    if (iuplo == 1) {
				ioff = (izero - 1) * lda;
				i__3 = izero - 1;
				for (i__ = 1; i__ <= i__3; ++i__) {
				    i__4 = ioff + i__;
				    a[i__4].r = 0., a[i__4].i = 0.;
/* L20: */
				}
				ioff += izero;
				i__3 = n;
				for (i__ = izero; i__ <= i__3; ++i__) {
				    i__4 = ioff;
				    a[i__4].r = 0., a[i__4].i = 0.;
				    ioff += lda;
/* L30: */
				}
			    } else {
				ioff = izero;
				i__3 = izero - 1;
				for (i__ = 1; i__ <= i__3; ++i__) {
				    i__4 = ioff;
				    a[i__4].r = 0., a[i__4].i = 0.;
				    ioff += lda;
/* L40: */
				}
				ioff -= izero;
				i__3 = n;
				for (i__ = izero; i__ <= i__3; ++i__) {
				    i__4 = ioff + i__;
				    a[i__4].r = 0., a[i__4].i = 0.;
/* L50: */
				}
			    }
			} else {
			    if (iuplo == 1) {

/*                          Set the first IZERO rows to zero. */

				ioff = 0;
				i__3 = n;
				for (j = 1; j <= i__3; ++j) {
				    i2 = min(j,izero);
				    i__4 = i2;
				    for (i__ = 1; i__ <= i__4; ++i__) {
					i__5 = ioff + i__;
					a[i__5].r = 0., a[i__5].i = 0.;
/* L60: */
				    }
				    ioff += lda;
/* L70: */
				}
			    } else {

/*                          Set the last IZERO rows to zero. */

				ioff = 0;
				i__3 = n;
				for (j = 1; j <= i__3; ++j) {
				    i1 = max(j,izero);
				    i__4 = n;
				    for (i__ = i1; i__ <= i__4; ++i__) {
					i__5 = ioff + i__;
					a[i__5].r = 0., a[i__5].i = 0.;
/* L80: */
				    }
				    ioff += lda;
/* L90: */
				}
			    }
			}
		    } else {
			izero = 0;
		    }
		} else {

/*                 Use a special block diagonal matrix to test alternate */
/*                 code for the 2 x 2 blocks. */

		    zlatsy_(uplo, &n, &a[1], &lda, iseed);
		}

/*              Do for each value of NB in NBVAL */

		i__3 = *nnb;
		for (inb = 1; inb <= i__3; ++inb) {
		    nb = nbval[inb];
		    xlaenv_(&c__1, &nb);

/*                 Compute the L*D*L' or U*D*U' factorization of the */
/*                 matrix. */

		    zlacpy_(uplo, &n, &n, &a[1], &lda, &afac[1], &lda);
		    lwork = max(2,nb) * lda;
		    s_copy(srnamc_1.srnamt, "ZSYTRF", (ftnlen)6, (ftnlen)6);
		    zsytrf_(uplo, &n, &afac[1], &lda, &iwork[1], &ainv[1], &
			    lwork, &info);

/*                 Adjust the expected value of INFO to account for */
/*                 pivoting. */

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

/*                 Check error code from ZSYTRF. */

		    if (info != k) {
			alaerh_(path, "ZSYTRF", &info, &k, uplo, &n, &n, &
				c_n1, &c_n1, &nb, &imat, &nfail, &nerrs, nout);
		    }
		    if (info != 0) {
			trfcon = TRUE_;
		    } else {
			trfcon = FALSE_;
		    }

/* +    TEST 1 */
/*                 Reconstruct matrix from factors and compute residual. */

		    zsyt01_(uplo, &n, &a[1], &lda, &afac[1], &lda, &iwork[1], 
			    &ainv[1], &lda, &rwork[1], result);
		    nt = 1;

/* +    TEST 2 */
/*                 Form the inverse and compute the residual. */

		    if (inb == 1 && ! trfcon) {
			zlacpy_(uplo, &n, &n, &afac[1], &lda, &ainv[1], &lda);
			s_copy(srnamc_1.srnamt, "ZSYTRI", (ftnlen)6, (ftnlen)
				6);
			zsytri_(uplo, &n, &ainv[1], &lda, &iwork[1], &work[1], 
				 &info);

/*                 Check error code from ZSYTRI. */

			if (info != 0) {
			    alaerh_(path, "ZSYTRI", &info, &c__0, uplo, &n, &
				    n, &c_n1, &c_n1, &c_n1, &imat, &nfail, &
				    nerrs, nout);
			}

			zsyt03_(uplo, &n, &a[1], &lda, &ainv[1], &lda, &work[
				1], &lda, &rwork[1], &rcondc, &result[1]);
			nt = 2;
		    }

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    i__4 = nt;
		    for (k = 1; k <= i__4; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___39.ciunit = *nout;
			    s_wsfe(&io___39);
			    do_fio(&c__1, uplo, (ftnlen)1);
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
/* L110: */
		    }
		    nrun += nt;

/*                 Skip the other tests if this is not the first block */
/*                 size. */

		    if (inb > 1) {
			goto L150;
		    }

/*                 Do only the condition estimate if INFO is not 0. */

		    if (trfcon) {
			rcondc = 0.;
			goto L140;
		    }

		    i__4 = *nns;
		    for (irhs = 1; irhs <= i__4; ++irhs) {
			nrhs = nsval[irhs];

/* +    TEST 3 */
/*                 Solve and compute residual for  A * X = B. */

			s_copy(srnamc_1.srnamt, "ZLARHS", (ftnlen)6, (ftnlen)
				6);
			zlarhs_(path, xtype, uplo, " ", &n, &n, &kl, &ku, &
				nrhs, &a[1], &lda, &xact[1], &lda, &b[1], &
				lda, iseed, &info);
			zlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);

			s_copy(srnamc_1.srnamt, "ZSYTRS", (ftnlen)6, (ftnlen)
				6);
			zsytrs_(uplo, &n, &nrhs, &afac[1], &lda, &iwork[1], &
				x[1], &lda, &info);

/*                 Check error code from ZSYTRS. */

			if (info != 0) {
			    alaerh_(path, "ZSYTRS", &info, &c__0, uplo, &n, &
				    n, &c_n1, &c_n1, &nrhs, &imat, &nfail, &
				    nerrs, nout);
			}

			zlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &
				lda);
			zsyt02_(uplo, &n, &nrhs, &a[1], &lda, &x[1], &lda, &
				work[1], &lda, &rwork[1], &result[2]);

/* +    TEST 4 */
/*                 Check solution from generated exact solution. */

			zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[3]);

/* +    TESTS 5, 6, and 7 */
/*                 Use iterative refinement to improve the solution. */

			s_copy(srnamc_1.srnamt, "ZSYRFS", (ftnlen)6, (ftnlen)
				6);
			zsyrfs_(uplo, &n, &nrhs, &a[1], &lda, &afac[1], &lda, 
				&iwork[1], &b[1], &lda, &x[1], &lda, &rwork[1]
, &rwork[nrhs + 1], &work[1], &rwork[(nrhs << 
				1) + 1], &info);

/*                 Check error code from ZSYRFS. */

			if (info != 0) {
			    alaerh_(path, "ZSYRFS", &info, &c__0, uplo, &n, &
				    n, &c_n1, &c_n1, &nrhs, &imat, &nfail, &
				    nerrs, nout);
			}

			zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &
				rcondc, &result[4]);
			zpot05_(uplo, &n, &nrhs, &a[1], &lda, &b[1], &lda, &x[
				1], &lda, &xact[1], &lda, &rwork[1], &rwork[
				nrhs + 1], &result[5]);

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			for (k = 3; k <= 7; ++k) {
			    if (result[k - 1] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    alahd_(nout, path);
				}
				io___42.ciunit = *nout;
				s_wsfe(&io___42);
				do_fio(&c__1, uplo, (ftnlen)1);
				do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
					sizeof(doublereal));
				e_wsfe();
				++nfail;
			    }
/* L120: */
			}
			nrun += 5;
/* L130: */
		    }

/* +    TEST 8 */
/*                 Get an estimate of RCOND = 1/CNDNUM. */

L140:
		    anorm = zlansy_("1", uplo, &n, &a[1], &lda, &rwork[1]);
		    s_copy(srnamc_1.srnamt, "ZSYCON", (ftnlen)6, (ftnlen)6);
		    zsycon_(uplo, &n, &afac[1], &lda, &iwork[1], &anorm, &
			    rcond, &work[1], &info);

/*                 Check error code from ZSYCON. */

		    if (info != 0) {
			alaerh_(path, "ZSYCON", &info, &c__0, uplo, &n, &n, &
				c_n1, &c_n1, &c_n1, &imat, &nfail, &nerrs, 
				nout);
		    }

		    result[7] = dget06_(&rcond, &rcondc);

/*                 Print information about the tests that did not pass */
/*                 the threshold. */

		    if (result[7] >= *thresh) {
			if (nfail == 0 && nerrs == 0) {
			    alahd_(nout, path);
			}
			io___44.ciunit = *nout;
			s_wsfe(&io___44);
			do_fio(&c__1, uplo, (ftnlen)1);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[7], (ftnlen)sizeof(
				doublereal));
			e_wsfe();
			++nfail;
		    }
		    ++nrun;
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

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of ZCHKSY */

} /* zchksy_ */
