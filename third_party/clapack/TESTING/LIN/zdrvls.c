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

static doublecomplex c_b1 = {1.,0.};
static doublecomplex c_b2 = {0.,0.};
static integer c__9 = 9;
static integer c__25 = 25;
static integer c__1 = 1;
static integer c__3 = 3;
static integer c__2 = 2;
static integer c__0 = 0;
static integer c_n1 = -1;
static doublereal c_b91 = -1.;

/* Subroutine */ int zdrvls_(logical *dotype, integer *nm, integer *mval, 
	integer *nn, integer *nval, integer *nns, integer *nsval, integer *
	nnb, integer *nbval, integer *nxval, doublereal *thresh, logical *
	tsterr, doublecomplex *a, doublecomplex *copya, doublecomplex *b, 
	doublecomplex *copyb, doublecomplex *c__, doublereal *s, doublereal *
	copys, doublecomplex *work, doublereal *rwork, integer *iwork, 
	integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };

    /* Format strings */
    static char fmt_9999[] = "(\002 TRANS='\002,a1,\002', M=\002,i5,\002, N"
	    "=\002,i5,\002, NRHS=\002,i4,\002, NB=\002,i4,\002, type\002,i2"
	    ",\002, test(\002,i2,\002)=\002,g12.5)";
    static char fmt_9998[] = "(\002 M=\002,i5,\002, N=\002,i5,\002, NRHS="
	    "\002,i4,\002, NB=\002,i4,\002, type\002,i2,\002, test(\002,i2"
	    ",\002)=\002,g12.5)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5, i__6;
    doublereal d__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, m, n, nb, im, in, lda, ldb, inb;
    doublereal eps;
    integer ins, info;
    char path[3];
    integer rank, nrhs, nrun;
    extern /* Subroutine */ int alahd_(integer *, char *);
    integer nfail, iseed[4], crank, irank;
    doublereal rcond;
    extern doublereal dasum_(integer *, doublereal *, integer *);
    integer itran, mnmin, ncols;
    doublereal norma, normb;
    extern /* Subroutine */ int zgels_(char *, integer *, integer *, integer *
, doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *), daxpy_(integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), 
	    zgemm_(char *, char *, integer *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *);
    char trans[1];
    integer nerrs, itype, lwork;
    extern doublereal zqrt12_(integer *, integer *, doublecomplex *, integer *
, doublereal *, doublecomplex *, integer *, doublereal *), 
	    zqrt14_(char *, integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *);
    extern /* Subroutine */ int zqrt13_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *), zqrt15_(
	    integer *, integer *, integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublecomplex *, integer *);
    integer nrows;
    extern doublereal zqrt17_(char *, integer *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *);
    integer lwlsy;
    extern /* Subroutine */ int zqrt16_(char *, integer *, integer *, integer 
	    *, doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int alaerh_(char *, char *, integer *, integer *, 
	    char *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *);
    integer iscale;
    extern /* Subroutine */ int zdscal_(integer *, doublereal *, 
	    doublecomplex *, integer *), alasvm_(char *, integer *, integer *, 
	     integer *, integer *), zgelsd_(integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublereal *, doublereal *, integer *, doublecomplex *, integer *
, doublereal *, integer *, integer *), xlaenv_(integer *, integer 
	    *);
    integer ldwork;
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), 
	    zgelss_(integer *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, integer *, doublereal *, doublereal *, 
	    integer *, doublecomplex *, integer *, doublereal *, integer *), 
	    zgelsx_(integer *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, integer *, integer *, doublereal *, integer *, 
	     doublecomplex *, doublereal *, integer *), zgelsy_(integer *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, integer *, doublereal *, integer *, doublecomplex *, 
	    integer *, doublereal *, integer *);
    doublereal result[18];
    extern /* Subroutine */ int zlarnv_(integer *, integer *, integer *, 
	    doublecomplex *), zerrls_(char *, integer *);

    /* Fortran I/O blocks */
    static cilist io___34 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZDRVLS tests the least squares driver routines ZGELS, CGELSX, CGELSS, */
/*  ZGELSY and CGELSD. */

/*  Arguments */
/*  ========= */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          The matrix types to be used for testing.  Matrices of type j */
/*          (for 1 <= j <= NTYPES) are used for testing if DOTYPE(j) = */
/*          .TRUE.; if DOTYPE(j) = .FALSE., then type j is not used. */
/*          The matrix of type j is generated as follows: */
/*          j=1: A = U*D*V where U and V are random unitary matrices */
/*               and D has random entries (> 0.1) taken from a uniform */
/*               distribution (0,1). A is full rank. */
/*          j=2: The same of 1, but A is scaled up. */
/*          j=3: The same of 1, but A is scaled down. */
/*          j=4: A = U*D*V where U and V are random unitary matrices */
/*               and D has 3*min(M,N)/4 random entries (> 0.1) taken */
/*               from a uniform distribution (0,1) and the remaining */
/*               entries set to 0. A is rank-deficient. */
/*          j=5: The same of 4, but A is scaled up. */
/*          j=6: The same of 5, but A is scaled down. */

/*  NM      (input) INTEGER */
/*          The number of values of M contained in the vector MVAL. */

/*  MVAL    (input) INTEGER array, dimension (NM) */
/*          The values of the matrix row dimension M. */

/*  NN      (input) INTEGER */
/*          The number of values of N contained in the vector NVAL. */

/*  NVAL    (input) INTEGER array, dimension (NN) */
/*          The values of the matrix column dimension N. */

/*  NNB     (input) INTEGER */
/*          The number of values of NB and NX contained in the */
/*          vectors NBVAL and NXVAL.  The blocking parameters are used */
/*          in pairs (NB,NX). */

/*  NBVAL   (input) INTEGER array, dimension (NNB) */
/*          The values of the blocksize NB. */

/*  NXVAL   (input) INTEGER array, dimension (NNB) */
/*          The values of the crossover point NX. */

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

/*  A       (workspace) COMPLEX*16 array, dimension (MMAX*NMAX) */
/*          where MMAX is the maximum value of M in MVAL and NMAX is the */
/*          maximum value of N in NVAL. */

/*  COPYA   (workspace) COMPLEX*16 array, dimension (MMAX*NMAX) */

/*  B       (workspace) COMPLEX*16 array, dimension (MMAX*NSMAX) */
/*          where MMAX is the maximum value of M in MVAL and NSMAX is the */
/*          maximum value of NRHS in NSVAL. */

/*  COPYB   (workspace) COMPLEX*16 array, dimension (MMAX*NSMAX) */

/*  C       (workspace) COMPLEX*16 array, dimension (MMAX*NSMAX) */

/*  S       (workspace) DOUBLE PRECISION array, dimension */
/*                      (min(MMAX,NMAX)) */

/*  COPYS   (workspace) DOUBLE PRECISION array, dimension */
/*                      (min(MMAX,NMAX)) */

/*  WORK    (workspace) COMPLEX*16 array, dimension */
/*                      (MMAX*NMAX + 4*NMAX + MMAX). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (5*NMAX-1) */

/*  IWORK   (workspace) INTEGER array, dimension (15*NMAX) */

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
    --copys;
    --s;
    --c__;
    --copyb;
    --b;
    --copya;
    --a;
    --nxval;
    --nbval;
    --nsval;
    --nval;
    --mval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "LS", (ftnlen)2, (ftnlen)2);
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = iseedy[i__ - 1];
/* L10: */
    }
    eps = dlamch_("Epsilon");

/*     Threshold for rank estimation */

    rcond = sqrt(eps) - (sqrt(eps) - eps) / 2;

/*     Test the error exits */

    xlaenv_(&c__9, &c__25);
    if (*tsterr) {
	zerrls_(path, nout);
    }

/*     Print the header if NM = 0 or NN = 0 and THRESH = 0. */

    if ((*nm == 0 || *nn == 0) && *thresh == 0.) {
	alahd_(nout, path);
    }
    infoc_1.infot = 0;

    i__1 = *nm;
    for (im = 1; im <= i__1; ++im) {
	m = mval[im];
	lda = max(1,m);

	i__2 = *nn;
	for (in = 1; in <= i__2; ++in) {
	    n = nval[in];
	    mnmin = min(m,n);
/* Computing MAX */
	    i__3 = max(1,m);
	    ldb = max(i__3,n);

	    i__3 = *nns;
	    for (ins = 1; ins <= i__3; ++ins) {
		nrhs = nsval[ins];
/* Computing MAX */
		i__4 = 1, i__5 = (m + nrhs) * (n + 2), i__4 = max(i__4,i__5), 
			i__5 = (n + nrhs) * (m + 2), i__4 = max(i__4,i__5), 
			i__5 = m * n + (mnmin << 2) + max(m,n), i__4 = max(
			i__4,i__5), i__5 = (n << 1) + m;
		lwork = max(i__4,i__5);

		for (irank = 1; irank <= 2; ++irank) {
		    for (iscale = 1; iscale <= 3; ++iscale) {
			itype = (irank - 1) * 3 + iscale;
			if (! dotype[itype]) {
			    goto L100;
			}

			if (irank == 1) {

/*                       Test ZGELS */

/*                       Generate a matrix of scaling type ISCALE */

			    zqrt13_(&iscale, &m, &n, &copya[1], &lda, &norma, 
				    iseed);
			    i__4 = *nnb;
			    for (inb = 1; inb <= i__4; ++inb) {
				nb = nbval[inb];
				xlaenv_(&c__1, &nb);
				xlaenv_(&c__3, &nxval[inb]);

				for (itran = 1; itran <= 2; ++itran) {
				    if (itran == 1) {
					*(unsigned char *)trans = 'N';
					nrows = m;
					ncols = n;
				    } else {
					*(unsigned char *)trans = 'C';
					nrows = n;
					ncols = m;
				    }
				    ldwork = max(1,ncols);

/*                             Set up a consistent rhs */

				    if (ncols > 0) {
					i__5 = ncols * nrhs;
					zlarnv_(&c__2, iseed, &i__5, &work[1])
						;
					i__5 = ncols * nrhs;
					d__1 = 1. / (doublereal) ncols;
					zdscal_(&i__5, &d__1, &work[1], &c__1)
						;
				    }
				    zgemm_(trans, "No transpose", &nrows, &
					    nrhs, &ncols, &c_b1, &copya[1], &
					    lda, &work[1], &ldwork, &c_b2, &b[
					    1], &ldb);
				    zlacpy_("Full", &nrows, &nrhs, &b[1], &
					    ldb, &copyb[1], &ldb);

/*                             Solve LS or overdetermined system */

				    if (m > 0 && n > 0) {
					zlacpy_("Full", &m, &n, &copya[1], &
						lda, &a[1], &lda);
					zlacpy_("Full", &nrows, &nrhs, &copyb[
						1], &ldb, &b[1], &ldb);
				    }
				    s_copy(srnamc_1.srnamt, "ZGELS ", (ftnlen)
					    6, (ftnlen)6);
				    zgels_(trans, &m, &n, &nrhs, &a[1], &lda, 
					    &b[1], &ldb, &work[1], &lwork, &
					    info);

				    if (info != 0) {
					alaerh_(path, "ZGELS ", &info, &c__0, 
						trans, &m, &n, &nrhs, &c_n1, &
						nb, &itype, &nfail, &nerrs, 
						nout);
				    }

/*                             Check correctness of results */

				    ldwork = max(1,nrows);
				    if (nrows > 0 && nrhs > 0) {
					zlacpy_("Full", &nrows, &nrhs, &copyb[
						1], &ldb, &c__[1], &ldb);
				    }
				    zqrt16_(trans, &m, &n, &nrhs, &copya[1], &
					    lda, &b[1], &ldb, &c__[1], &ldb, &
					    rwork[1], result);

				    if (itran == 1 && m >= n || itran == 2 && 
					    m < n) {

/*                                Solving LS system */

					result[1] = zqrt17_(trans, &c__1, &m, 
						&n, &nrhs, &copya[1], &lda, &
						b[1], &ldb, &copyb[1], &ldb, &
						c__[1], &work[1], &lwork);
				    } else {

/*                                Solving overdetermined system */

					result[1] = zqrt14_(trans, &m, &n, &
						nrhs, &copya[1], &lda, &b[1], 
						&ldb, &work[1], &lwork);
				    }

/*                             Print information about the tests that */
/*                             did not pass the threshold. */

				    for (k = 1; k <= 2; ++k) {
					if (result[k - 1] >= *thresh) {
					    if (nfail == 0 && nerrs == 0) {
			  alahd_(nout, path);
					    }
					    io___34.ciunit = *nout;
					    s_wsfe(&io___34);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&m, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&nrhs, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&nb, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&itype, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&k, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&result[k - 
						    1], (ftnlen)sizeof(
						    doublereal));
					    e_wsfe();
					    ++nfail;
					}
/* L20: */
				    }
				    nrun += 2;
/* L30: */
				}
/* L40: */
			    }
			}

/*                    Generate a matrix of scaling type ISCALE and rank */
/*                    type IRANK. */

			zqrt15_(&iscale, &irank, &m, &n, &nrhs, &copya[1], &
				lda, &copyb[1], &ldb, &copys[1], &rank, &
				norma, &normb, iseed, &work[1], &lwork);

/*                    workspace used: MAX(M+MIN(M,N),NRHS*MIN(M,N),2*N+M) */

			i__4 = n;
			for (j = 1; j <= i__4; ++j) {
			    iwork[j] = 0;
/* L50: */
			}
			ldwork = max(1,m);

/*                    Test ZGELSX */

/*                    ZGELSX:  Compute the minimum-norm solution X */
/*                    to min( norm( A * X - B ) ) */
/*                    using a complete orthogonal factorization. */

			zlacpy_("Full", &m, &n, &copya[1], &lda, &a[1], &lda);
			zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &b[1], &
				ldb);

			s_copy(srnamc_1.srnamt, "ZGELSX", (ftnlen)6, (ftnlen)
				6);
			zgelsx_(&m, &n, &nrhs, &a[1], &lda, &b[1], &ldb, &
				iwork[1], &rcond, &crank, &work[1], &rwork[1], 
				 &info);

			if (info != 0) {
			    alaerh_(path, "ZGELSX", &info, &c__0, " ", &m, &n, 
				     &nrhs, &c_n1, &nb, &itype, &nfail, &
				    nerrs, nout);
			}

/*                    workspace used: MAX( MNMIN+3*N, 2*MNMIN+NRHS ) */

/*                    Test 3:  Compute relative error in svd */
/*                             workspace: M*N + 4*MIN(M,N) + MAX(M,N) */

			result[2] = zqrt12_(&crank, &crank, &a[1], &lda, &
				copys[1], &work[1], &lwork, &rwork[1]);

/*                    Test 4:  Compute error in solution */
/*                             workspace:  M*NRHS + M */

			zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &work[1], 
				&ldwork);
			zqrt16_("No transpose", &m, &n, &nrhs, &copya[1], &
				lda, &b[1], &ldb, &work[1], &ldwork, &rwork[1]
, &result[3]);

/*                    Test 5:  Check norm of r'*A */
/*                             workspace: NRHS*(M+N) */

			result[4] = 0.;
			if (m > crank) {
			    result[4] = zqrt17_("No transpose", &c__1, &m, &n, 
				     &nrhs, &copya[1], &lda, &b[1], &ldb, &
				    copyb[1], &ldb, &c__[1], &work[1], &lwork);
			}

/*                    Test 6:  Check if x is in the rowspace of A */
/*                             workspace: (M+NRHS)*(N+2) */

			result[5] = 0.;

			if (n > crank) {
			    result[5] = zqrt14_("No transpose", &m, &n, &nrhs, 
				     &copya[1], &lda, &b[1], &ldb, &work[1], &
				    lwork);
			}

/*                    Print information about the tests that did not */
/*                    pass the threshold. */

			for (k = 3; k <= 6; ++k) {
			    if (result[k - 1] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    alahd_(nout, path);
				}
				io___39.ciunit = *nout;
				s_wsfe(&io___39);
				do_fio(&c__1, (char *)&m, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&itype, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
					sizeof(doublereal));
				e_wsfe();
				++nfail;
			    }
/* L60: */
			}
			nrun += 4;

/*                    Loop for testing different block sizes. */

			i__4 = *nnb;
			for (inb = 1; inb <= i__4; ++inb) {
			    nb = nbval[inb];
			    xlaenv_(&c__1, &nb);
			    xlaenv_(&c__3, &nxval[inb]);

/*                       Test ZGELSY */

/*                       ZGELSY:  Compute the minimum-norm solution */
/*                       X to min( norm( A * X - B ) ) */
/*                       using the rank-revealing orthogonal */
/*                       factorization. */

			    zlacpy_("Full", &m, &n, &copya[1], &lda, &a[1], &
				    lda);
			    zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &b[1], 
				     &ldb);

/*                       Initialize vector IWORK. */

			    i__5 = n;
			    for (j = 1; j <= i__5; ++j) {
				iwork[j] = 0;
/* L70: */
			    }

/*                       Set LWLSY to the adequate value. */

/* Computing MAX */
			    i__5 = mnmin << 1, i__6 = nb * (n + 1), i__5 = 
				    max(i__5,i__6), i__6 = mnmin + nb * nrhs;
			    lwlsy = mnmin + max(i__5,i__6);
			    lwlsy = max(1,lwlsy);

			    s_copy(srnamc_1.srnamt, "ZGELSY", (ftnlen)6, (
				    ftnlen)6);
			    zgelsy_(&m, &n, &nrhs, &a[1], &lda, &b[1], &ldb, &
				    iwork[1], &rcond, &crank, &work[1], &
				    lwlsy, &rwork[1], &info);
			    if (info != 0) {
				alaerh_(path, "ZGELSY", &info, &c__0, " ", &m, 
					 &n, &nrhs, &c_n1, &nb, &itype, &
					nfail, &nerrs, nout);
			    }

/*                       workspace used: 2*MNMIN+NB*NB+NB*MAX(N,NRHS) */

/*                       Test 7:  Compute relative error in svd */
/*                                workspace: M*N + 4*MIN(M,N) + MAX(M,N) */

			    result[6] = zqrt12_(&crank, &crank, &a[1], &lda, &
				    copys[1], &work[1], &lwork, &rwork[1]);

/*                       Test 8:  Compute error in solution */
/*                                workspace:  M*NRHS + M */

			    zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &work[
				    1], &ldwork);
			    zqrt16_("No transpose", &m, &n, &nrhs, &copya[1], 
				    &lda, &b[1], &ldb, &work[1], &ldwork, &
				    rwork[1], &result[7]);

/*                       Test 9:  Check norm of r'*A */
/*                                workspace: NRHS*(M+N) */

			    result[8] = 0.;
			    if (m > crank) {
				result[8] = zqrt17_("No transpose", &c__1, &m, 
					 &n, &nrhs, &copya[1], &lda, &b[1], &
					ldb, &copyb[1], &ldb, &c__[1], &work[
					1], &lwork);
			    }

/*                       Test 10:  Check if x is in the rowspace of A */
/*                                workspace: (M+NRHS)*(N+2) */

			    result[9] = 0.;

			    if (n > crank) {
				result[9] = zqrt14_("No transpose", &m, &n, &
					nrhs, &copya[1], &lda, &b[1], &ldb, &
					work[1], &lwork);
			    }

/*                       Test ZGELSS */

/*                       ZGELSS:  Compute the minimum-norm solution */
/*                       X to min( norm( A * X - B ) ) */
/*                       using the SVD. */

			    zlacpy_("Full", &m, &n, &copya[1], &lda, &a[1], &
				    lda);
			    zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &b[1], 
				     &ldb);
			    s_copy(srnamc_1.srnamt, "ZGELSS", (ftnlen)6, (
				    ftnlen)6);
			    zgelss_(&m, &n, &nrhs, &a[1], &lda, &b[1], &ldb, &
				    s[1], &rcond, &crank, &work[1], &lwork, &
				    rwork[1], &info);

			    if (info != 0) {
				alaerh_(path, "ZGELSS", &info, &c__0, " ", &m, 
					 &n, &nrhs, &c_n1, &nb, &itype, &
					nfail, &nerrs, nout);
			    }

/*                       workspace used: 3*min(m,n) + */
/*                                       max(2*min(m,n),nrhs,max(m,n)) */

/*                       Test 11:  Compute relative error in svd */

			    if (rank > 0) {
				daxpy_(&mnmin, &c_b91, &copys[1], &c__1, &s[1]
, &c__1);
				result[10] = dasum_(&mnmin, &s[1], &c__1) / 
					dasum_(&mnmin, &copys[1], &c__1) / (
					eps * (doublereal) mnmin);
			    } else {
				result[10] = 0.;
			    }

/*                       Test 12:  Compute error in solution */

			    zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &work[
				    1], &ldwork);
			    zqrt16_("No transpose", &m, &n, &nrhs, &copya[1], 
				    &lda, &b[1], &ldb, &work[1], &ldwork, &
				    rwork[1], &result[11]);

/*                       Test 13:  Check norm of r'*A */

			    result[12] = 0.;
			    if (m > crank) {
				result[12] = zqrt17_("No transpose", &c__1, &
					m, &n, &nrhs, &copya[1], &lda, &b[1], 
					&ldb, &copyb[1], &ldb, &c__[1], &work[
					1], &lwork);
			    }

/*                       Test 14:  Check if x is in the rowspace of A */

			    result[13] = 0.;
			    if (n > crank) {
				result[13] = zqrt14_("No transpose", &m, &n, &
					nrhs, &copya[1], &lda, &b[1], &ldb, &
					work[1], &lwork);
			    }

/*                       Test ZGELSD */

/*                       ZGELSD:  Compute the minimum-norm solution X */
/*                       to min( norm( A * X - B ) ) using a */
/*                       divide and conquer SVD. */

			    xlaenv_(&c__9, &c__25);

			    zlacpy_("Full", &m, &n, &copya[1], &lda, &a[1], &
				    lda);
			    zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &b[1], 
				     &ldb);

			    s_copy(srnamc_1.srnamt, "ZGELSD", (ftnlen)6, (
				    ftnlen)6);
			    zgelsd_(&m, &n, &nrhs, &a[1], &lda, &b[1], &ldb, &
				    s[1], &rcond, &crank, &work[1], &lwork, &
				    rwork[1], &iwork[1], &info);
			    if (info != 0) {
				alaerh_(path, "ZGELSD", &info, &c__0, " ", &m, 
					 &n, &nrhs, &c_n1, &nb, &itype, &
					nfail, &nerrs, nout);
			    }

/*                       Test 15:  Compute relative error in svd */

			    if (rank > 0) {
				daxpy_(&mnmin, &c_b91, &copys[1], &c__1, &s[1]
, &c__1);
				result[14] = dasum_(&mnmin, &s[1], &c__1) / 
					dasum_(&mnmin, &copys[1], &c__1) / (
					eps * (doublereal) mnmin);
			    } else {
				result[14] = 0.;
			    }

/*                       Test 16:  Compute error in solution */

			    zlacpy_("Full", &m, &nrhs, &copyb[1], &ldb, &work[
				    1], &ldwork);
			    zqrt16_("No transpose", &m, &n, &nrhs, &copya[1], 
				    &lda, &b[1], &ldb, &work[1], &ldwork, &
				    rwork[1], &result[15]);

/*                       Test 17:  Check norm of r'*A */

			    result[16] = 0.;
			    if (m > crank) {
				result[16] = zqrt17_("No transpose", &c__1, &
					m, &n, &nrhs, &copya[1], &lda, &b[1], 
					&ldb, &copyb[1], &ldb, &c__[1], &work[
					1], &lwork);
			    }

/*                       Test 18:  Check if x is in the rowspace of A */

			    result[17] = 0.;
			    if (n > crank) {
				result[17] = zqrt14_("No transpose", &m, &n, &
					nrhs, &copya[1], &lda, &b[1], &ldb, &
					work[1], &lwork);
			    }

/*                       Print information about the tests that did not */
/*                       pass the threshold. */

			    for (k = 7; k <= 18; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					alahd_(nout, path);
				    }
				    io___41.ciunit = *nout;
				    s_wsfe(&io___41);
				    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&nrhs, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&nb, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&itype, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&k, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&result[k - 1], (
					    ftnlen)sizeof(doublereal));
				    e_wsfe();
				    ++nfail;
				}
/* L80: */
			    }
			    nrun += 12;

/* L90: */
			}
L100:
			;
		    }
/* L110: */
		}
/* L120: */
	    }
/* L130: */
	}
/* L140: */
    }

/*     Print a summary of the results. */

    alasvm_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of ZDRVLS */

} /* zdrvls_ */
