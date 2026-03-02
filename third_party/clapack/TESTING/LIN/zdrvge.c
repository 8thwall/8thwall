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
static doublecomplex c_b20 = {0.,0.};
static integer c__6 = 6;
static integer c__7 = 7;

/* Subroutine */ int zdrvge_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, doublereal *thresh, logical *tsterr, integer *nmax, 
	doublecomplex *a, doublecomplex *afac, doublecomplex *asav, 
	doublecomplex *b, doublecomplex *bsav, doublecomplex *x, 
	doublecomplex *xact, doublereal *s, doublecomplex *work, doublereal *
	rwork, integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char transs[1*3] = "N" "T" "C";
    static char facts[1*3] = "F" "N" "E";
    static char equeds[1*4] = "N" "R" "C" "B";

    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002, N =\002,i5,\002, type \002,i2,"
	    "\002, test(\002,i2,\002) =\002,g12.5)";
    static char fmt_9997[] = "(1x,a6,\002, FACT='\002,a1,\002', TRANS='\002,"
	    "a1,\002', N=\002,i5,\002, EQUED='\002,a1,\002', type \002,i2,"
	    "\002, test(\002,i1,\002)=\002,g12.5)";
    static char fmt_9998[] = "(1x,a6,\002, FACT='\002,a1,\002', TRANS='\002,"
	    "a1,\002', N=\002,i5,\002, type \002,i2,\002, test(\002,i1,\002)"
	    "=\002,g12.5)";

    /* System generated locals */
    address a__1[2];
    integer i__1, i__2, i__3, i__4, i__5[2];
    doublereal d__1, d__2;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, k, n, k1, nb, in, kl, ku, nt, lda;
    char fact[1];
    integer ioff, mode;
    doublereal amax;
    char path[3];
    integer imat, info;
    char dist[1];
    doublereal rdum[1];
    char type__[1];
    integer nrun, ifact, nfail, iseed[4], nfact;
    extern doublereal dget06_(doublereal *, doublereal *);
    extern logical lsame_(char *, char *);
    char equed[1];
    integer nbmin;
    doublereal rcond, roldc;
    integer nimat;
    doublereal roldi;
    extern /* Subroutine */ int zget01_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, integer *, doublereal *, 
	    doublereal *), zget02_(char *, integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *);
    doublereal anorm;
    integer itran;
    extern /* Subroutine */ int zget04_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
);
    logical equil;
    doublereal roldo;
    extern /* Subroutine */ int zget07_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    char trans[1];
    integer izero, nerrs, lwork;
    extern /* Subroutine */ int zgesv_(integer *, integer *, doublecomplex *, 
	    integer *, integer *, doublecomplex *, integer *, integer *);
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int zlatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, char *), aladhd_(integer *, 
	    char *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int alaerh_(char *, char *, integer *, integer *, 
	    char *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *);
    logical prefac;
    doublereal colcnd, rcondc;
    logical nofact;
    integer iequed;
    doublereal rcondi;
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *);
    doublereal cndnum, anormi, rcondo, ainvnm;
    extern /* Subroutine */ int zlaqge_(integer *, integer *, doublecomplex *, 
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
, doublereal *, char *);
    logical trfcon;
    doublereal anormo, rowcnd;
    extern /* Subroutine */ int xlaenv_(integer *, integer *), zgeequ_(
	    integer *, integer *, doublecomplex *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *)
	    , zgetrf_(integer *, integer *, doublecomplex *, integer *, 
	    integer *, integer *), zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), 
	    zgetri_(integer *, doublecomplex *, integer *, integer *, 
	    doublecomplex *, integer *, integer *), zlarhs_(char *, char *, 
	    char *, char *, integer *, integer *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, integer *, integer *), zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);
    extern doublereal zlantr_(char *, char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *);
    extern /* Subroutine */ int zlatms_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, char *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    doublereal result[7];
    extern /* Subroutine */ int zgesvx_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     char *, doublereal *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublecomplex *, doublereal *, integer *);
    doublereal rpvgrw;
    extern /* Subroutine */ int zerrvx_(char *, integer *);

    /* Fortran I/O blocks */
    static cilist io___55 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZDRVGE tests the driver routines ZGESV and -SVX. */

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

/*  S       (workspace) DOUBLE PRECISION array, dimension (2*NMAX) */

/*  WORK    (workspace) COMPLEX*16 array, dimension */
/*                      (NMAX*max(3,NRHS)) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (2*NRHS+NMAX) */

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

    if (*tsterr) {
	zerrvx_(path, nout);
    }
    infoc_1.infot = 0;

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
	nimat = 11;
	if (n <= 0) {
	    nimat = 1;
	}

	i__2 = nimat;
	for (imat = 1; imat <= i__2; ++imat) {

/*           Do the tests only if DOTYPE( IMAT ) is true. */

	    if (! dotype[imat]) {
		goto L80;
	    }

/*           Skip types 5, 6, or 7 if the matrix size is too small. */

	    zerot = imat >= 5 && imat <= 7;
	    if (zerot && n < imat - 4) {
		goto L80;
	    }

/*           Set up parameters with ZLATB4 and generate a test matrix */
/*           with ZLATMS. */

	    zlatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &mode, &
		    cndnum, dist);
	    rcondc = 1. / cndnum;

	    s_copy(srnamc_1.srnamt, "ZLATMS", (ftnlen)6, (ftnlen)6);
	    zlatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &cndnum, &
		    anorm, &kl, &ku, "No packing", &a[1], &lda, &work[1], &
		    info);

/*           Check error code from ZLATMS. */

	    if (info != 0) {
		alaerh_(path, "ZLATMS", &info, &c__0, " ", &n, &n, &c_n1, &
			c_n1, &c_n1, &imat, &nfail, &nerrs, nout);
		goto L80;
	    }

/*           For types 5-7, zero one or more columns of the matrix to */
/*           test that INFO is returned correctly. */

	    if (zerot) {
		if (imat == 5) {
		    izero = 1;
		} else if (imat == 6) {
		    izero = n;
		} else {
		    izero = n / 2 + 1;
		}
		ioff = (izero - 1) * lda;
		if (imat < 7) {
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			i__4 = ioff + i__;
			a[i__4].r = 0., a[i__4].i = 0.;
/* L20: */
		    }
		} else {
		    i__3 = n - izero + 1;
		    zlaset_("Full", &n, &i__3, &c_b20, &c_b20, &a[ioff + 1], &
			    lda);
		}
	    } else {
		izero = 0;
	    }

/*           Save a copy of the matrix A in ASAV. */

	    zlacpy_("Full", &n, &n, &a[1], &lda, &asav[1], &lda);

	    for (iequed = 1; iequed <= 4; ++iequed) {
		*(unsigned char *)equed = *(unsigned char *)&equeds[iequed - 
			1];
		if (iequed == 1) {
		    nfact = 3;
		} else {
		    nfact = 1;
		}

		i__3 = nfact;
		for (ifact = 1; ifact <= i__3; ++ifact) {
		    *(unsigned char *)fact = *(unsigned char *)&facts[ifact - 
			    1];
		    prefac = lsame_(fact, "F");
		    nofact = lsame_(fact, "N");
		    equil = lsame_(fact, "E");

		    if (zerot) {
			if (prefac) {
			    goto L60;
			}
			rcondo = 0.;
			rcondi = 0.;

		    } else if (! nofact) {

/*                    Compute the condition number for comparison with */
/*                    the value returned by ZGESVX (FACT = 'N' reuses */
/*                    the condition number from the previous iteration */
/*                    with FACT = 'F'). */

			zlacpy_("Full", &n, &n, &asav[1], &lda, &afac[1], &
				lda);
			if (equil || iequed > 1) {

/*                       Compute row and column scale factors to */
/*                       equilibrate the matrix A. */

			    zgeequ_(&n, &n, &afac[1], &lda, &s[1], &s[n + 1], 
				    &rowcnd, &colcnd, &amax, &info);
			    if (info == 0 && n > 0) {
				if (lsame_(equed, "R")) 
					{
				    rowcnd = 0.;
				    colcnd = 1.;
				} else if (lsame_(equed, "C")) {
				    rowcnd = 1.;
				    colcnd = 0.;
				} else if (lsame_(equed, "B")) {
				    rowcnd = 0.;
				    colcnd = 0.;
				}

/*                          Equilibrate the matrix. */

				zlaqge_(&n, &n, &afac[1], &lda, &s[1], &s[n + 
					1], &rowcnd, &colcnd, &amax, equed);
			    }
			}

/*                    Save the condition number of the non-equilibrated */
/*                    system for use in ZGET04. */

			if (equil) {
			    roldo = rcondo;
			    roldi = rcondi;
			}

/*                    Compute the 1-norm and infinity-norm of A. */

			anormo = zlange_("1", &n, &n, &afac[1], &lda, &rwork[
				1]);
			anormi = zlange_("I", &n, &n, &afac[1], &lda, &rwork[
				1]);

/*                    Factor the matrix A. */

			zgetrf_(&n, &n, &afac[1], &lda, &iwork[1], &info);

/*                    Form the inverse of A. */

			zlacpy_("Full", &n, &n, &afac[1], &lda, &a[1], &lda);
			lwork = *nmax * max(3,*nrhs);
			zgetri_(&n, &a[1], &lda, &iwork[1], &work[1], &lwork, 
				&info);

/*                    Compute the 1-norm condition number of A. */

			ainvnm = zlange_("1", &n, &n, &a[1], &lda, &rwork[1]);
			if (anormo <= 0. || ainvnm <= 0.) {
			    rcondo = 1.;
			} else {
			    rcondo = 1. / anormo / ainvnm;
			}

/*                    Compute the infinity-norm condition number of A. */

			ainvnm = zlange_("I", &n, &n, &a[1], &lda, &rwork[1]);
			if (anormi <= 0. || ainvnm <= 0.) {
			    rcondi = 1.;
			} else {
			    rcondi = 1. / anormi / ainvnm;
			}
		    }

		    for (itran = 1; itran <= 3; ++itran) {

/*                    Do for each value of TRANS. */

			*(unsigned char *)trans = *(unsigned char *)&transs[
				itran - 1];
			if (itran == 1) {
			    rcondc = rcondo;
			} else {
			    rcondc = rcondi;
			}

/*                    Restore the matrix A. */

			zlacpy_("Full", &n, &n, &asav[1], &lda, &a[1], &lda);

/*                    Form an exact solution and set the right hand side. */

			s_copy(srnamc_1.srnamt, "ZLARHS", (ftnlen)6, (ftnlen)
				6);
			zlarhs_(path, xtype, "Full", trans, &n, &n, &kl, &ku, 
				nrhs, &a[1], &lda, &xact[1], &lda, &b[1], &
				lda, iseed, &info);
			*(unsigned char *)xtype = 'C';
			zlacpy_("Full", &n, nrhs, &b[1], &lda, &bsav[1], &lda);

			if (nofact && itran == 1) {

/*                       --- Test ZGESV  --- */

/*                       Compute the LU factorization of the matrix and */
/*                       solve the system. */

			    zlacpy_("Full", &n, &n, &a[1], &lda, &afac[1], &
				    lda);
			    zlacpy_("Full", &n, nrhs, &b[1], &lda, &x[1], &
				    lda);

			    s_copy(srnamc_1.srnamt, "ZGESV ", (ftnlen)6, (
				    ftnlen)6);
			    zgesv_(&n, nrhs, &afac[1], &lda, &iwork[1], &x[1], 
				     &lda, &info);

/*                       Check error code from ZGESV . */

			    if (info != izero) {
				alaerh_(path, "ZGESV ", &info, &izero, " ", &
					n, &n, &c_n1, &c_n1, nrhs, &imat, &
					nfail, &nerrs, nout);
			    }

/*                       Reconstruct matrix from factors and compute */
/*                       residual. */

			    zget01_(&n, &n, &a[1], &lda, &afac[1], &lda, &
				    iwork[1], &rwork[1], result);
			    nt = 1;
			    if (izero == 0) {

/*                          Compute residual of the computed solution. */

				zlacpy_("Full", &n, nrhs, &b[1], &lda, &work[
					1], &lda);
				zget02_("No transpose", &n, &n, nrhs, &a[1], &
					lda, &x[1], &lda, &work[1], &lda, &
					rwork[1], &result[1]);

/*                          Check solution from generated exact solution. */

				zget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, 
					 &rcondc, &result[2]);
				nt = 3;
			    }

/*                       Print information about the tests that did not */
/*                       pass the threshold. */

			    i__4 = nt;
			    for (k = 1; k <= i__4; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					aladhd_(nout, path);
				    }
				    io___55.ciunit = *nout;
				    s_wsfe(&io___55);
				    do_fio(&c__1, "ZGESV ", (ftnlen)6);
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
/* L30: */
			    }
			    nrun += nt;
			}

/*                    --- Test ZGESVX --- */

			if (! prefac) {
			    zlaset_("Full", &n, &n, &c_b20, &c_b20, &afac[1], 
				    &lda);
			}
			zlaset_("Full", &n, nrhs, &c_b20, &c_b20, &x[1], &lda);
			if (iequed > 1 && n > 0) {

/*                       Equilibrate the matrix if FACT = 'F' and */
/*                       EQUED = 'R', 'C', or 'B'. */

			    zlaqge_(&n, &n, &a[1], &lda, &s[1], &s[n + 1], &
				    rowcnd, &colcnd, &amax, equed);
			}

/*                    Solve the system and compute the condition number */
/*                    and error bounds using ZGESVX. */

			s_copy(srnamc_1.srnamt, "ZGESVX", (ftnlen)6, (ftnlen)
				6);
			zgesvx_(fact, trans, &n, nrhs, &a[1], &lda, &afac[1], 
				&lda, &iwork[1], equed, &s[1], &s[n + 1], &b[
				1], &lda, &x[1], &lda, &rcond, &rwork[1], &
				rwork[*nrhs + 1], &work[1], &rwork[(*nrhs << 
				1) + 1], &info);

/*                    Check the error code from ZGESVX. */

			if (info != izero) {
/* Writing concatenation */
			    i__5[0] = 1, a__1[0] = fact;
			    i__5[1] = 1, a__1[1] = trans;
			    s_cat(ch__1, a__1, i__5, &c__2, (ftnlen)2);
			    alaerh_(path, "ZGESVX", &info, &izero, ch__1, &n, 
				    &n, &c_n1, &c_n1, nrhs, &imat, &nfail, &
				    nerrs, nout);
			}

/*                    Compare RWORK(2*NRHS+1) from ZGESVX with the */
/*                    computed reciprocal pivot growth factor RPVGRW */

			if (info != 0) {
			    rpvgrw = zlantr_("M", "U", "N", &info, &info, &
				    afac[1], &lda, rdum);
			    if (rpvgrw == 0.) {
				rpvgrw = 1.;
			    } else {
				rpvgrw = zlange_("M", &n, &info, &a[1], &lda, 
					rdum) / rpvgrw;
			    }
			} else {
			    rpvgrw = zlantr_("M", "U", "N", &n, &n, &afac[1], 
				    &lda, rdum);
			    if (rpvgrw == 0.) {
				rpvgrw = 1.;
			    } else {
				rpvgrw = zlange_("M", &n, &n, &a[1], &lda, 
					rdum) / rpvgrw;
			    }
			}
/* Computing MAX */
			d__2 = rwork[(*nrhs << 1) + 1];
			result[6] = (d__1 = rpvgrw - rwork[(*nrhs << 1) + 1], 
				abs(d__1)) / max(d__2,rpvgrw) / dlamch_("E");

			if (! prefac) {

/*                       Reconstruct matrix from factors and compute */
/*                       residual. */

			    zget01_(&n, &n, &a[1], &lda, &afac[1], &lda, &
				    iwork[1], &rwork[(*nrhs << 1) + 1], 
				    result);
			    k1 = 1;
			} else {
			    k1 = 2;
			}

			if (info == 0) {
			    trfcon = FALSE_;

/*                       Compute residual of the computed solution. */

			    zlacpy_("Full", &n, nrhs, &bsav[1], &lda, &work[1]
, &lda);
			    zget02_(trans, &n, &n, nrhs, &asav[1], &lda, &x[1]
, &lda, &work[1], &lda, &rwork[(*nrhs << 
				    1) + 1], &result[1]);

/*                       Check solution from generated exact solution. */

			    if (nofact || prefac && lsame_(equed, "N")) {
				zget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, 
					 &rcondc, &result[2]);
			    } else {
				if (itran == 1) {
				    roldc = roldo;
				} else {
				    roldc = roldi;
				}
				zget04_(&n, nrhs, &x[1], &lda, &xact[1], &lda, 
					 &roldc, &result[2]);
			    }

/*                       Check the error bounds from iterative */
/*                       refinement. */

			    zget07_(trans, &n, nrhs, &asav[1], &lda, &b[1], &
				    lda, &x[1], &lda, &xact[1], &lda, &rwork[
				    1], &rwork[*nrhs + 1], &result[3]);
			} else {
			    trfcon = TRUE_;
			}

/*                    Compare RCOND from ZGESVX with the computed value */
/*                    in RCONDC. */

			result[5] = dget06_(&rcond, &rcondc);

/*                    Print information about the tests that did not pass */
/*                    the threshold. */

			if (! trfcon) {
			    for (k = k1; k <= 7; ++k) {
				if (result[k - 1] >= *thresh) {
				    if (nfail == 0 && nerrs == 0) {
					aladhd_(nout, path);
				    }
				    if (prefac) {
					io___62.ciunit = *nout;
					s_wsfe(&io___62);
					do_fio(&c__1, "ZGESVX", (ftnlen)6);
					do_fio(&c__1, fact, (ftnlen)1);
					do_fio(&c__1, trans, (ftnlen)1);
					do_fio(&c__1, (char *)&n, (ftnlen)
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
					io___63.ciunit = *nout;
					s_wsfe(&io___63);
					do_fio(&c__1, "ZGESVX", (ftnlen)6);
					do_fio(&c__1, fact, (ftnlen)1);
					do_fio(&c__1, trans, (ftnlen)1);
					do_fio(&c__1, (char *)&n, (ftnlen)
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
/* L40: */
			    }
			    nrun = nrun + 7 - k1;
			} else {
			    if (result[0] >= *thresh && ! prefac) {
				if (nfail == 0 && nerrs == 0) {
				    aladhd_(nout, path);
				}
				if (prefac) {
				    io___64.ciunit = *nout;
				    s_wsfe(&io___64);
				    do_fio(&c__1, "ZGESVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, equed, (ftnlen)1);
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&c__1, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&result[0], (ftnlen)
					    sizeof(doublereal));
				    e_wsfe();
				} else {
				    io___65.ciunit = *nout;
				    s_wsfe(&io___65);
				    do_fio(&c__1, "ZGESVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&c__1, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&result[0], (ftnlen)
					    sizeof(doublereal));
				    e_wsfe();
				}
				++nfail;
				++nrun;
			    }
			    if (result[5] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    aladhd_(nout, path);
				}
				if (prefac) {
				    io___66.ciunit = *nout;
				    s_wsfe(&io___66);
				    do_fio(&c__1, "ZGESVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, equed, (ftnlen)1);
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&c__6, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&result[5], (ftnlen)
					    sizeof(doublereal));
				    e_wsfe();
				} else {
				    io___67.ciunit = *nout;
				    s_wsfe(&io___67);
				    do_fio(&c__1, "ZGESVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&c__6, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&result[5], (ftnlen)
					    sizeof(doublereal));
				    e_wsfe();
				}
				++nfail;
				++nrun;
			    }
			    if (result[6] >= *thresh) {
				if (nfail == 0 && nerrs == 0) {
				    aladhd_(nout, path);
				}
				if (prefac) {
				    io___68.ciunit = *nout;
				    s_wsfe(&io___68);
				    do_fio(&c__1, "ZGESVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, equed, (ftnlen)1);
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&c__7, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&result[6], (ftnlen)
					    sizeof(doublereal));
				    e_wsfe();
				} else {
				    io___69.ciunit = *nout;
				    s_wsfe(&io___69);
				    do_fio(&c__1, "ZGESVX", (ftnlen)6);
				    do_fio(&c__1, fact, (ftnlen)1);
				    do_fio(&c__1, trans, (ftnlen)1);
				    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					    integer));
				    do_fio(&c__1, (char *)&imat, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&c__7, (ftnlen)
					    sizeof(integer));
				    do_fio(&c__1, (char *)&result[6], (ftnlen)
					    sizeof(doublereal));
				    e_wsfe();
				}
				++nfail;
				++nrun;
			    }

			}

/* L50: */
		    }
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

/*     Print a summary of the results. */

    alasvm_(path, nout, &nfail, &nrun, &nerrs);

    return 0;

/*     End of ZDRVGE */

} /* zdrvge_ */
