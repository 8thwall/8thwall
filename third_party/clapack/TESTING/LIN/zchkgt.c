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
static integer c__7 = 7;
static doublereal c_b63 = 1.;
static doublereal c_b64 = 0.;

/* Subroutine */ int zchkgt_(logical *dotype, integer *nn, integer *nval, 
	integer *nns, integer *nsval, doublereal *thresh, logical *tsterr, 
	doublecomplex *a, doublecomplex *af, doublecomplex *b, doublecomplex *
	x, doublecomplex *xact, doublecomplex *work, doublereal *rwork, 
	integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 0,0,0,1 };
    static char transs[1*3] = "N" "T" "C";

    /* Format strings */
    static char fmt_9999[] = "(12x,\002N =\002,i5,\002,\002,10x,\002 type"
	    " \002,i2,\002, test(\002,i2,\002) = \002,g12.5)";
    static char fmt_9997[] = "(\002 NORM ='\002,a1,\002', N =\002,i5,\002"
	    ",\002,10x,\002 type \002,i2,\002, test(\002,i2,\002) = \002,g12."
	    "5)";
    static char fmt_9998[] = "(\002 TRANS='\002,a1,\002', N =\002,i5,\002, N"
	    "RHS=\002,i3,\002, type \002,i2,\002, test(\002,i2,\002) = \002,g"
	    "12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, m, n;
    doublecomplex z__[3];
    integer in, kl, ku, ix, lda;
    doublereal cond;
    integer mode, koff, imat, info;
    char path[3], dist[1];
    integer irhs, nrhs;
    char norm[1], type__[1];
    integer nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer nfail, iseed[4];
    extern doublereal dget06_(doublereal *, doublereal *);
    doublereal rcond;
    integer nimat;
    doublereal anorm;
    integer itran;
    extern /* Subroutine */ int zget04_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
);
    char trans[1];
    integer izero, nerrs;
    extern /* Subroutine */ int zgtt01_(integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex *
, doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublereal *, doublereal *), zgtt02_(char *, integer *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *), zgtt05_(char *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublereal *);
    logical zerot;
    extern /* Subroutine */ int zcopy_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zlatb4_(char *, integer *, integer *, 
	     integer *, char *, integer *, integer *, doublereal *, integer *, 
	     doublereal *, char *), alaerh_(char *, 
	    char *, integer *, integer *, char *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *);
    doublereal rcondc, rcondi;
    extern /* Subroutine */ int zdscal_(integer *, doublereal *, 
	    doublecomplex *, integer *), alasum_(char *, integer *, integer *, 
	     integer *, integer *);
    doublereal rcondo, ainvnm;
    logical trfcon;
    extern /* Subroutine */ int zerrge_(char *, integer *);
    extern doublereal zlangt_(char *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *);
    extern /* Subroutine */ int zlagtm_(char *, integer *, integer *, 
	    doublereal *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *), zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    extern doublereal dzasum_(integer *, doublecomplex *, integer *);
    extern /* Subroutine */ int zgtcon_(char *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, integer *), 
	    zlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublecomplex *, integer *, doublecomplex *, 
	    integer *), zlarnv_(integer *, integer *, 
	    integer *, doublecomplex *);
    doublereal result[7];
    extern /* Subroutine */ int zgtrfs_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex *
, doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zgttrf_(integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    integer *), zgttrs_(char *, integer *, integer *, doublecomplex *, 
	     doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___29 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9997, 0 };
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

/*  ZCHKGT tests ZGTTRF, -TRS, -RFS, and -CON */

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

/*  A       (workspace) COMPLEX*16 array, dimension (NMAX*4) */

/*  AF      (workspace) COMPLEX*16 array, dimension (NMAX*4) */

/*  B       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */
/*          where NSMAX is the largest entry in NSVAL. */

/*  X       (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  XACT    (workspace) COMPLEX*16 array, dimension (NMAX*NSMAX) */

/*  WORK    (workspace) COMPLEX*16 array, dimension */
/*                      (NMAX*max(3,NSMAX)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension */
/*                      (max(NMAX)+2*NSMAX) */

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
    --af;
    --a;
    --nsval;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
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
	zerrge_(path, nout);
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
		goto L100;
	    }

/*           Set up parameters with ZLATB4. */

	    zlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cond, dist);

	    zerot = imat >= 8 && imat <= 10;
	    if (imat <= 6) {

/*              Types 1-6:  generate matrices of known condition number. */

/* Computing MAX */
		i__3 = 2 - ku, i__4 = 3 - max(1,n);
		koff = max(i__3,i__4);
		s_copy(srnamc_1.srnamt, "ZLATMS", (ftnlen)6, (ftnlen)6);
		zlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cond, 
			&anorm, &kl, &ku, "Z", &af[koff], &c__3, &work[1], &
			info);

/*              Check the error code from ZLATMS. */

		if (info != 0) {
		    alaerh_(path, "ZLATMS", &info, &c__0, " ", &n, &n, &kl, &
			    ku, &c_n1, &imat, &nfail, &nerrs, nout);
		    goto L100;
		}
		izero = 0;

		if (n > 1) {
		    i__3 = n - 1;
		    zcopy_(&i__3, &af[4], &c__3, &a[1], &c__1);
		    i__3 = n - 1;
		    zcopy_(&i__3, &af[3], &c__3, &a[n + m + 1], &c__1);
		}
		zcopy_(&n, &af[2], &c__3, &a[m + 1], &c__1);
	    } else {

/*              Types 7-12:  generate tridiagonal matrices with */
/*              unknown condition numbers. */

		if (! zerot || ! dotype[7]) {

/*                 Generate a matrix with elements whose real and */
/*                 imaginary parts are from [-1,1]. */

		    i__3 = n + (m << 1);
		    zlarnv_(&c__2, iseed, &i__3, &a[1]);
		    if (anorm != 1.) {
			i__3 = n + (m << 1);
			zdscal_(&i__3, &anorm, &a[1], &c__1);
		    }
		} else if (izero > 0) {

/*                 Reuse the last matrix by copying back the zeroed out */
/*                 elements. */

		    if (izero == 1) {
			i__3 = n;
			a[i__3].r = z__[1].r, a[i__3].i = z__[1].i;
			if (n > 1) {
			    a[1].r = z__[2].r, a[1].i = z__[2].i;
			}
		    } else if (izero == n) {
			i__3 = n * 3 - 2;
			a[i__3].r = z__[0].r, a[i__3].i = z__[0].i;
			i__3 = (n << 1) - 1;
			a[i__3].r = z__[1].r, a[i__3].i = z__[1].i;
		    } else {
			i__3 = (n << 1) - 2 + izero;
			a[i__3].r = z__[0].r, a[i__3].i = z__[0].i;
			i__3 = n - 1 + izero;
			a[i__3].r = z__[1].r, a[i__3].i = z__[1].i;
			i__3 = izero;
			a[i__3].r = z__[2].r, a[i__3].i = z__[2].i;
		    }
		}

/*              If IMAT > 7, set one column of the matrix to 0. */

		if (! zerot) {
		    izero = 0;
		} else if (imat == 8) {
		    izero = 1;
		    i__3 = n;
		    z__[1].r = a[i__3].r, z__[1].i = a[i__3].i;
		    i__3 = n;
		    a[i__3].r = 0., a[i__3].i = 0.;
		    if (n > 1) {
			z__[2].r = a[1].r, z__[2].i = a[1].i;
			a[1].r = 0., a[1].i = 0.;
		    }
		} else if (imat == 9) {
		    izero = n;
		    i__3 = n * 3 - 2;
		    z__[0].r = a[i__3].r, z__[0].i = a[i__3].i;
		    i__3 = (n << 1) - 1;
		    z__[1].r = a[i__3].r, z__[1].i = a[i__3].i;
		    i__3 = n * 3 - 2;
		    a[i__3].r = 0., a[i__3].i = 0.;
		    i__3 = (n << 1) - 1;
		    a[i__3].r = 0., a[i__3].i = 0.;
		} else {
		    izero = (n + 1) / 2;
		    i__3 = n - 1;
		    for (i__ = izero; i__ <= i__3; ++i__) {
			i__4 = (n << 1) - 2 + i__;
			a[i__4].r = 0., a[i__4].i = 0.;
			i__4 = n - 1 + i__;
			a[i__4].r = 0., a[i__4].i = 0.;
			i__4 = i__;
			a[i__4].r = 0., a[i__4].i = 0.;
/* L20: */
		    }
		    i__3 = n * 3 - 2;
		    a[i__3].r = 0., a[i__3].i = 0.;
		    i__3 = (n << 1) - 1;
		    a[i__3].r = 0., a[i__3].i = 0.;
		}
	    }

/* +    TEST 1 */
/*           Factor A as L*U and compute the ratio */
/*              norm(L*U - A) / (n * norm(A) * EPS ) */

	    i__3 = n + (m << 1);
	    zcopy_(&i__3, &a[1], &c__1, &af[1], &c__1);
	    s_copy(srnamc_1.srnamt, "ZGTTRF", (ftnlen)6, (ftnlen)6);
	    zgttrf_(&n, &af[1], &af[m + 1], &af[n + m + 1], &af[n + (m << 1) 
		    + 1], &iwork[1], &info);

/*           Check error code from ZGTTRF. */

	    if (info != izero) {
		alaerh_(path, "ZGTTRF", &info, &izero, " ", &n, &n, &c__1, &
			c__1, &c_n1, &imat, &nfail, &nerrs, nout);
	    }
	    trfcon = info != 0;

	    zgtt01_(&n, &a[1], &a[m + 1], &a[n + m + 1], &af[1], &af[m + 1], &
		    af[n + m + 1], &af[n + (m << 1) + 1], &iwork[1], &work[1], 
		     &lda, &rwork[1], result);

/*           Print the test ratio if it is .GE. THRESH. */

	    if (result[0] >= *thresh) {
		if (nfail == 0 && nerrs == 0) {
		    alahd_(nout, path);
		}
		io___29.ciunit = *nout;
		s_wsfe(&io___29);
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&result[0], (ftnlen)sizeof(doublereal));
		e_wsfe();
		++nfail;
	    }
	    ++nrun;

	    for (itran = 1; itran <= 2; ++itran) {
		*(unsigned char *)trans = *(unsigned char *)&transs[itran - 1]
			;
		if (itran == 1) {
		    *(unsigned char *)norm = 'O';
		} else {
		    *(unsigned char *)norm = 'I';
		}
		anorm = zlangt_(norm, &n, &a[1], &a[m + 1], &a[n + m + 1]);

		if (! trfcon) {

/*                 Use ZGTTRS to solve for one column at a time of */
/*                 inv(A), computing the maximum column sum as we go. */

		    ainvnm = 0.;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			i__4 = n;
			for (j = 1; j <= i__4; ++j) {
			    i__5 = j;
			    x[i__5].r = 0., x[i__5].i = 0.;
/* L30: */
			}
			i__4 = i__;
			x[i__4].r = 1., x[i__4].i = 0.;
			zgttrs_(trans, &n, &c__1, &af[1], &af[m + 1], &af[n + 
				m + 1], &af[n + (m << 1) + 1], &iwork[1], &x[
				1], &lda, &info);
/* Computing MAX */
			d__1 = ainvnm, d__2 = dzasum_(&n, &x[1], &c__1);
			ainvnm = max(d__1,d__2);
/* L40: */
		    }

/*                 Compute RCONDC = 1 / (norm(A) * norm(inv(A)) */

		    if (anorm <= 0. || ainvnm <= 0.) {
			rcondc = 1.;
		    } else {
			rcondc = 1. / anorm / ainvnm;
		    }
		    if (itran == 1) {
			rcondo = rcondc;
		    } else {
			rcondi = rcondc;
		    }
		} else {
		    rcondc = 0.;
		}

/* +    TEST 7 */
/*              Estimate the reciprocal of the condition number of the */
/*              matrix. */

		s_copy(srnamc_1.srnamt, "ZGTCON", (ftnlen)6, (ftnlen)6);
		zgtcon_(norm, &n, &af[1], &af[m + 1], &af[n + m + 1], &af[n + 
			(m << 1) + 1], &iwork[1], &anorm, &rcond, &work[1], &
			info);

/*              Check error code from ZGTCON. */

		if (info != 0) {
		    alaerh_(path, "ZGTCON", &info, &c__0, norm, &n, &n, &c_n1, 
			     &c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		}

		result[6] = dget06_(&rcond, &rcondc);

/*              Print the test ratio if it is .GE. THRESH. */

		if (result[6] >= *thresh) {
		    if (nfail == 0 && nerrs == 0) {
			alahd_(nout, path);
		    }
		    io___39.ciunit = *nout;
		    s_wsfe(&io___39);
		    do_fio(&c__1, norm, (ftnlen)1);
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&result[6], (ftnlen)sizeof(
			    doublereal));
		    e_wsfe();
		    ++nfail;
		}
		++nrun;
/* L50: */
	    }

/*           Skip the remaining tests if the matrix is singular. */

	    if (trfcon) {
		goto L100;
	    }

	    i__3 = *nns;
	    for (irhs = 1; irhs <= i__3; ++irhs) {
		nrhs = nsval[irhs];

/*              Generate NRHS random solution vectors. */

		ix = 1;
		i__4 = nrhs;
		for (j = 1; j <= i__4; ++j) {
		    zlarnv_(&c__2, iseed, &n, &xact[ix]);
		    ix += lda;
/* L60: */
		}

		for (itran = 1; itran <= 3; ++itran) {
		    *(unsigned char *)trans = *(unsigned char *)&transs[itran 
			    - 1];
		    if (itran == 1) {
			rcondc = rcondo;
		    } else {
			rcondc = rcondi;
		    }

/*                 Set the right hand side. */

		    zlagtm_(trans, &n, &nrhs, &c_b63, &a[1], &a[m + 1], &a[n 
			    + m + 1], &xact[1], &lda, &c_b64, &b[1], &lda);

/* +    TEST 2 */
/*              Solve op(A) * X = B and compute the residual. */

		    zlacpy_("Full", &n, &nrhs, &b[1], &lda, &x[1], &lda);
		    s_copy(srnamc_1.srnamt, "ZGTTRS", (ftnlen)6, (ftnlen)6);
		    zgttrs_(trans, &n, &nrhs, &af[1], &af[m + 1], &af[n + m + 
			    1], &af[n + (m << 1) + 1], &iwork[1], &x[1], &lda, 
			     &info);

/*              Check error code from ZGTTRS. */

		    if (info != 0) {
			alaerh_(path, "ZGTTRS", &info, &c__0, trans, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    zlacpy_("Full", &n, &nrhs, &b[1], &lda, &work[1], &lda);
		    zgtt02_(trans, &n, &nrhs, &a[1], &a[m + 1], &a[n + m + 1], 
			     &x[1], &lda, &work[1], &lda, &rwork[1], &result[
			    1]);

/* +    TEST 3 */
/*              Check solution from generated exact solution. */

		    zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[2]);

/* +    TESTS 4, 5, and 6 */
/*              Use iterative refinement to improve the solution. */

		    s_copy(srnamc_1.srnamt, "ZGTRFS", (ftnlen)6, (ftnlen)6);
		    zgtrfs_(trans, &n, &nrhs, &a[1], &a[m + 1], &a[n + m + 1], 
			     &af[1], &af[m + 1], &af[n + m + 1], &af[n + (m <<
			     1) + 1], &iwork[1], &b[1], &lda, &x[1], &lda, &
			    rwork[1], &rwork[nrhs + 1], &work[1], &rwork[(
			    nrhs << 1) + 1], &info);

/*              Check error code from ZGTRFS. */

		    if (info != 0) {
			alaerh_(path, "ZGTRFS", &info, &c__0, trans, &n, &n, &
				c_n1, &c_n1, &nrhs, &imat, &nfail, &nerrs, 
				nout);
		    }

		    zget04_(&n, &nrhs, &x[1], &lda, &xact[1], &lda, &rcondc, &
			    result[3]);
		    zgtt05_(trans, &n, &nrhs, &a[1], &a[m + 1], &a[n + m + 1], 
			     &b[1], &lda, &x[1], &lda, &xact[1], &lda, &rwork[
			    1], &rwork[nrhs + 1], &result[4]);

/*              Print information about the tests that did not pass the */
/*              threshold. */

		    for (k = 2; k <= 6; ++k) {
			if (result[k - 1] >= *thresh) {
			    if (nfail == 0 && nerrs == 0) {
				alahd_(nout, path);
			    }
			    io___44.ciunit = *nout;
			    s_wsfe(&io___44);
			    do_fio(&c__1, trans, (ftnlen)1);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				    sizeof(doublereal));
			    e_wsfe();
			    ++nfail;
			}
/* L70: */
		    }
		    nrun += 5;
/* L80: */
		}
/* L90: */
	    }
L100:
	    ;
	}
/* L110: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of ZCHKGT */

} /* zchkgt_ */
