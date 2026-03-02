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
static complex c_b48 = {0.f,0.f};
static complex c_b49 = {1.f,0.f};
static integer c__6 = 6;
static integer c__7 = 7;

/* Subroutine */ int cdrvgb_(logical *dotype, integer *nn, integer *nval, 
	integer *nrhs, real *thresh, logical *tsterr, complex *a, integer *la, 
	 complex *afb, integer *lafb, complex *asav, complex *b, complex *
	bsav, complex *x, complex *xact, real *s, complex *work, real *rwork, 
	integer *iwork, integer *nout)
{
    /* Initialized data */

    static integer iseedy[4] = { 1988,1989,1990,1991 };
    static char transs[1*3] = "N" "T" "C";
    static char facts[1*3] = "F" "N" "E";
    static char equeds[1*4] = "N" "R" "C" "B";

    /* Format strings */
    static char fmt_9999[] = "(\002 *** In CDRVGB, LA=\002,i5,\002 is too sm"
	    "all for N=\002,i5,\002, KU=\002,i5,\002, KL=\002,i5,/\002 ==> In"
	    "crease LA to at least \002,i5)";
    static char fmt_9998[] = "(\002 *** In CDRVGB, LAFB=\002,i5,\002 is too "
	    "small for N=\002,i5,\002, KU=\002,i5,\002, KL=\002,i5,/\002 ==> "
	    "Increase LAFB to at least \002,i5)";
    static char fmt_9997[] = "(1x,a6,\002, N=\002,i5,\002, KL=\002,i5,\002, "
	    "KU=\002,i5,\002, type \002,i1,\002, test(\002,i1,\002)=\002,g12."
	    "5)";
    static char fmt_9995[] = "(1x,a6,\002( '\002,a1,\002','\002,a1,\002',"
	    "\002,i5,\002,\002,i5,\002,\002,i5,\002,...), EQUED='\002,a1,\002"
	    "', type \002,i1,\002, test(\002,i1,\002)=\002,g12.5)";
    static char fmt_9996[] = "(1x,a6,\002( '\002,a1,\002','\002,a1,\002',"
	    "\002,i5,\002,\002,i5,\002,\002,i5,\002,...), type \002,i1,\002, "
	    "test(\002,i1,\002)=\002,g12.5)";

    /* System generated locals */
    address a__1[2];
    integer i__1, i__2, i__3, i__4, i__5, i__6, i__7, i__8, i__9, i__10, 
	    i__11[2];
    real r__1, r__2;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);
    double c_abs(complex *);

    /* Local variables */
    integer i__, j, k, n, i1, i2, k1, nb, in, kl, ku, nt, lda, ldb, ikl, nkl, 
	    iku, nku;
    char fact[1];
    integer ioff, mode;
    real amax;
    char path[3];
    integer imat, info;
    char dist[1];
    real rdum[1];
    char type__[1];
    integer nrun, ldafb;
    extern /* Subroutine */ int cgbt01_(integer *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, integer *, 
	    complex *, real *), cgbt02_(char *, integer *, integer *, integer 
	    *, integer *, integer *, complex *, integer *, complex *, integer 
	    *, complex *, integer *, real *), cgbt05_(char *, integer 
	    *, integer *, integer *, integer *, complex *, integer *, complex 
	    *, integer *, complex *, integer *, complex *, integer *, real *, 
	    real *, real *);
    integer ifact;
    extern /* Subroutine */ int cget04_(integer *, integer *, complex *, 
	    integer *, complex *, integer *, real *, real *);
    integer nfail, iseed[4], nfact;
    extern logical lsame_(char *, char *);
    char equed[1];
    integer nbmin;
    real rcond, roldc;
    extern /* Subroutine */ int cgbsv_(integer *, integer *, integer *, 
	    integer *, complex *, integer *, integer *, complex *, integer *, 
	    integer *);
    integer nimat;
    real roldi;
    extern doublereal sget06_(real *, real *);
    real anorm;
    integer itran;
    logical equil;
    real roldo;
    char trans[1];
    integer izero, nerrs;
    logical zerot;
    char xtype[1];
    extern /* Subroutine */ int clatb4_(char *, integer *, integer *, integer 
	    *, char *, integer *, integer *, real *, integer *, real *, char *
), aladhd_(integer *, char *);
    extern doublereal clangb_(char *, integer *, integer *, integer *, 
	    complex *, integer *, real *), clange_(char *, integer *, 
	    integer *, complex *, integer *, real *);
    extern /* Subroutine */ int claqgb_(integer *, integer *, integer *, 
	    integer *, complex *, integer *, real *, real *, real *, real *, 
	    real *, char *), alaerh_(char *, char *, integer *, 
	    integer *, char *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *);
    logical prefac;
    real colcnd;
    extern doublereal clantb_(char *, char *, char *, integer *, integer *, 
	    complex *, integer *, real *);
    extern /* Subroutine */ int cgbequ_(integer *, integer *, integer *, 
	    integer *, complex *, integer *, real *, real *, real *, real *, 
	    real *, integer *);
    real rcondc;
    extern doublereal slamch_(char *);
    logical nofact;
    extern /* Subroutine */ int cgbtrf_(integer *, integer *, integer *, 
	    integer *, complex *, integer *, integer *, integer *);
    integer iequed;
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *);
    real rcondi;
    extern /* Subroutine */ int clarhs_(char *, char *, char *, char *, 
	    integer *, integer *, integer *, integer *, integer *, complex *, 
	    integer *, complex *, integer *, complex *, integer *, integer *, 
	    integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *), alasvm_(char *, integer *, integer *, integer *, integer 
	    *);
    real cndnum, anormi, rcondo, ainvnm;
    extern /* Subroutine */ int cgbtrs_(char *, integer *, integer *, integer 
	    *, integer *, complex *, integer *, integer *, complex *, integer 
	    *, integer *), clatms_(integer *, integer *, char *, 
	    integer *, char *, real *, integer *, real *, real *, integer *, 
	    integer *, char *, complex *, integer *, complex *, integer *);
    logical trfcon;
    real anormo, rowcnd;
    extern /* Subroutine */ int cgbsvx_(char *, char *, integer *, integer *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    integer *, char *, real *, real *, complex *, integer *, complex *
, integer *, real *, real *, real *, complex *, real *, integer *), xlaenv_(integer *, integer *);
    real anrmpv;
    extern /* Subroutine */ int cerrvx_(char *, integer *);
    real result[7], rpvgrw;

    /* Fortran I/O blocks */
    static cilist io___26 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_9996, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CDRVGB tests the driver routines CGBSV and -SVX. */

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

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  TSTERR  (input) LOGICAL */
/*          Flag that indicates whether error exits are to be tested. */

/*  A       (workspace) COMPLEX array, dimension (LA) */

/*  LA      (input) INTEGER */
/*          The length of the array A.  LA >= (2*NMAX-1)*NMAX */
/*          where NMAX is the largest entry in NVAL. */

/*  AFB     (workspace) COMPLEX array, dimension (LAFB) */

/*  LAFB    (input) INTEGER */
/*          The length of the array AFB.  LAFB >= (3*NMAX-2)*NMAX */
/*          where NMAX is the largest entry in NVAL. */

/*  ASAV    (workspace) COMPLEX array, dimension (LA) */

/*  B       (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  BSAV    (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  X       (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  XACT    (workspace) COMPLEX array, dimension (NMAX*NRHS) */

/*  S       (workspace) REAL array, dimension (2*NMAX) */

/*  WORK    (workspace) COMPLEX array, dimension */
/*                      (NMAX*max(3,NRHS,NMAX)) */

/*  RWORK   (workspace) REAL array, dimension */
/*                      (max(NMAX,2*NRHS)) */

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
    --afb;
    --a;
    --nval;
    --dotype;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants and the random number seed. */

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
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
	cerrvx_(path, nout);
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
	ldb = max(n,1);
	*(unsigned char *)xtype = 'N';

/*        Set limits on the number of loop iterations. */

/* Computing MAX */
	i__2 = 1, i__3 = min(n,4);
	nkl = max(i__2,i__3);
	if (n == 0) {
	    nkl = 1;
	}
	nku = nkl;
	nimat = 8;
	if (n <= 0) {
	    nimat = 1;
	}

	i__2 = nkl;
	for (ikl = 1; ikl <= i__2; ++ikl) {

/*           Do for KL = 0, N-1, (3N-1)/4, and (N+1)/4. This order makes */
/*           it easier to skip redundant values for small values of N. */

	    if (ikl == 1) {
		kl = 0;
	    } else if (ikl == 2) {
/* Computing MAX */
		i__3 = n - 1;
		kl = max(i__3,0);
	    } else if (ikl == 3) {
		kl = (n * 3 - 1) / 4;
	    } else if (ikl == 4) {
		kl = (n + 1) / 4;
	    }
	    i__3 = nku;
	    for (iku = 1; iku <= i__3; ++iku) {

/*              Do for KU = 0, N-1, (3N-1)/4, and (N+1)/4. This order */
/*              makes it easier to skip redundant values for small */
/*              values of N. */

		if (iku == 1) {
		    ku = 0;
		} else if (iku == 2) {
/* Computing MAX */
		    i__4 = n - 1;
		    ku = max(i__4,0);
		} else if (iku == 3) {
		    ku = (n * 3 - 1) / 4;
		} else if (iku == 4) {
		    ku = (n + 1) / 4;
		}

/*              Check that A and AFB are big enough to generate this */
/*              matrix. */

		lda = kl + ku + 1;
		ldafb = (kl << 1) + ku + 1;
		if (lda * n > *la || ldafb * n > *lafb) {
		    if (nfail == 0 && nerrs == 0) {
			aladhd_(nout, path);
		    }
		    if (lda * n > *la) {
			io___26.ciunit = *nout;
			s_wsfe(&io___26);
			do_fio(&c__1, (char *)&(*la), (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(integer));
			i__4 = n * (kl + ku + 1);
			do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
			e_wsfe();
			++nerrs;
		    }
		    if (ldafb * n > *lafb) {
			io___27.ciunit = *nout;
			s_wsfe(&io___27);
			do_fio(&c__1, (char *)&(*lafb), (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(integer));
			i__4 = n * ((kl << 1) + ku + 1);
			do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
			e_wsfe();
			++nerrs;
		    }
		    goto L130;
		}

		i__4 = nimat;
		for (imat = 1; imat <= i__4; ++imat) {

/*                 Do the tests only if DOTYPE( IMAT ) is true. */

		    if (! dotype[imat]) {
			goto L120;
		    }

/*                 Skip types 2, 3, or 4 if the matrix is too small. */

		    zerot = imat >= 2 && imat <= 4;
		    if (zerot && n < imat - 1) {
			goto L120;
		    }

/*                 Set up parameters with CLATB4 and generate a */
/*                 test matrix with CLATMS. */

		    clatb4_(path, &imat, &n, &n, type__, &kl, &ku, &anorm, &
			    mode, &cndnum, dist);
		    rcondc = 1.f / cndnum;

		    s_copy(srnamc_1.srnamt, "CLATMS", (ftnlen)6, (ftnlen)6);
		    clatms_(&n, &n, dist, iseed, type__, &rwork[1], &mode, &
			    cndnum, &anorm, &kl, &ku, "Z", &a[1], &lda, &work[
			    1], &info);

/*                 Check the error code from CLATMS. */

		    if (info != 0) {
			alaerh_(path, "CLATMS", &info, &c__0, " ", &n, &n, &
				kl, &ku, &c_n1, &imat, &nfail, &nerrs, nout);
			goto L120;
		    }

/*                 For types 2, 3, and 4, zero one or more columns of */
/*                 the matrix to test that INFO is returned correctly. */

		    izero = 0;
		    if (zerot) {
			if (imat == 2) {
			    izero = 1;
			} else if (imat == 3) {
			    izero = n;
			} else {
			    izero = n / 2 + 1;
			}
			ioff = (izero - 1) * lda;
			if (imat < 4) {
/* Computing MAX */
			    i__5 = 1, i__6 = ku + 2 - izero;
			    i1 = max(i__5,i__6);
/* Computing MIN */
			    i__5 = kl + ku + 1, i__6 = ku + 1 + (n - izero);
			    i2 = min(i__5,i__6);
			    i__5 = i2;
			    for (i__ = i1; i__ <= i__5; ++i__) {
				i__6 = ioff + i__;
				a[i__6].r = 0.f, a[i__6].i = 0.f;
/* L20: */
			    }
			} else {
			    i__5 = n;
			    for (j = izero; j <= i__5; ++j) {
/* Computing MAX */
				i__6 = 1, i__7 = ku + 2 - j;
/* Computing MIN */
				i__9 = kl + ku + 1, i__10 = ku + 1 + (n - j);
				i__8 = min(i__9,i__10);
				for (i__ = max(i__6,i__7); i__ <= i__8; ++i__)
					 {
				    i__6 = ioff + i__;
				    a[i__6].r = 0.f, a[i__6].i = 0.f;
/* L30: */
				}
				ioff += lda;
/* L40: */
			    }
			}
		    }

/*                 Save a copy of the matrix A in ASAV. */

		    i__5 = kl + ku + 1;
		    clacpy_("Full", &i__5, &n, &a[1], &lda, &asav[1], &lda);

		    for (iequed = 1; iequed <= 4; ++iequed) {
			*(unsigned char *)equed = *(unsigned char *)&equeds[
				iequed - 1];
			if (iequed == 1) {
			    nfact = 3;
			} else {
			    nfact = 1;
			}

			i__5 = nfact;
			for (ifact = 1; ifact <= i__5; ++ifact) {
			    *(unsigned char *)fact = *(unsigned char *)&facts[
				    ifact - 1];
			    prefac = lsame_(fact, "F");
			    nofact = lsame_(fact, "N");
			    equil = lsame_(fact, "E");

			    if (zerot) {
				if (prefac) {
				    goto L100;
				}
				rcondo = 0.f;
				rcondi = 0.f;

			    } else if (! nofact) {

/*                          Compute the condition number for comparison */
/*                          with the value returned by SGESVX (FACT = */
/*                          'N' reuses the condition number from the */
/*                          previous iteration with FACT = 'F'). */

				i__8 = kl + ku + 1;
				clacpy_("Full", &i__8, &n, &asav[1], &lda, &
					afb[kl + 1], &ldafb);
				if (equil || iequed > 1) {

/*                             Compute row and column scale factors to */
/*                             equilibrate the matrix A. */

				    cgbequ_(&n, &n, &kl, &ku, &afb[kl + 1], &
					    ldafb, &s[1], &s[n + 1], &rowcnd, 
					    &colcnd, &amax, &info);
				    if (info == 0 && n > 0) {
					if (lsame_(equed, "R")) {
					    rowcnd = 0.f;
					    colcnd = 1.f;
					} else if (lsame_(equed, "C")) {
					    rowcnd = 1.f;
					    colcnd = 0.f;
					} else if (lsame_(equed, "B")) {
					    rowcnd = 0.f;
					    colcnd = 0.f;
					}

/*                                Equilibrate the matrix. */

					claqgb_(&n, &n, &kl, &ku, &afb[kl + 1]
, &ldafb, &s[1], &s[n + 1], &
						rowcnd, &colcnd, &amax, equed);
				    }
				}

/*                          Save the condition number of the */
/*                          non-equilibrated system for use in CGET04. */

				if (equil) {
				    roldo = rcondo;
				    roldi = rcondi;
				}

/*                          Compute the 1-norm and infinity-norm of A. */

				anormo = clangb_("1", &n, &kl, &ku, &afb[kl + 
					1], &ldafb, &rwork[1]);
				anormi = clangb_("I", &n, &kl, &ku, &afb[kl + 
					1], &ldafb, &rwork[1]);

/*                          Factor the matrix A. */

				cgbtrf_(&n, &n, &kl, &ku, &afb[1], &ldafb, &
					iwork[1], &info);

/*                          Form the inverse of A. */

				claset_("Full", &n, &n, &c_b48, &c_b49, &work[
					1], &ldb);
				s_copy(srnamc_1.srnamt, "CGBTRS", (ftnlen)6, (
					ftnlen)6);
				cgbtrs_("No transpose", &n, &kl, &ku, &n, &
					afb[1], &ldafb, &iwork[1], &work[1], &
					ldb, &info);

/*                          Compute the 1-norm condition number of A. */

				ainvnm = clange_("1", &n, &n, &work[1], &ldb, 
					&rwork[1]);
				if (anormo <= 0.f || ainvnm <= 0.f) {
				    rcondo = 1.f;
				} else {
				    rcondo = 1.f / anormo / ainvnm;
				}

/*                          Compute the infinity-norm condition number */
/*                          of A. */

				ainvnm = clange_("I", &n, &n, &work[1], &ldb, 
					&rwork[1]);
				if (anormi <= 0.f || ainvnm <= 0.f) {
				    rcondi = 1.f;
				} else {
				    rcondi = 1.f / anormi / ainvnm;
				}
			    }

			    for (itran = 1; itran <= 3; ++itran) {

/*                          Do for each value of TRANS. */

				*(unsigned char *)trans = *(unsigned char *)&
					transs[itran - 1];
				if (itran == 1) {
				    rcondc = rcondo;
				} else {
				    rcondc = rcondi;
				}

/*                          Restore the matrix A. */

				i__8 = kl + ku + 1;
				clacpy_("Full", &i__8, &n, &asav[1], &lda, &a[
					1], &lda);

/*                          Form an exact solution and set the right hand */
/*                          side. */

				s_copy(srnamc_1.srnamt, "CLARHS", (ftnlen)6, (
					ftnlen)6);
				clarhs_(path, xtype, "Full", trans, &n, &n, &
					kl, &ku, nrhs, &a[1], &lda, &xact[1], 
					&ldb, &b[1], &ldb, iseed, &info);
				*(unsigned char *)xtype = 'C';
				clacpy_("Full", &n, nrhs, &b[1], &ldb, &bsav[
					1], &ldb);

				if (nofact && itran == 1) {

/*                             --- Test CGBSV  --- */

/*                             Compute the LU factorization of the matrix */
/*                             and solve the system. */

				    i__8 = kl + ku + 1;
				    clacpy_("Full", &i__8, &n, &a[1], &lda, &
					    afb[kl + 1], &ldafb);
				    clacpy_("Full", &n, nrhs, &b[1], &ldb, &x[
					    1], &ldb);

				    s_copy(srnamc_1.srnamt, "CGBSV ", (ftnlen)
					    6, (ftnlen)6);
				    cgbsv_(&n, &kl, &ku, nrhs, &afb[1], &
					    ldafb, &iwork[1], &x[1], &ldb, &
					    info);

/*                             Check error code from CGBSV . */

				    if (info != izero) {
					alaerh_(path, "CGBSV ", &info, &izero, 
						 " ", &n, &n, &kl, &ku, nrhs, 
						&imat, &nfail, &nerrs, nout);
				    }

/*                             Reconstruct matrix from factors and */
/*                             compute residual. */

				    cgbt01_(&n, &n, &kl, &ku, &a[1], &lda, &
					    afb[1], &ldafb, &iwork[1], &work[
					    1], result);
				    nt = 1;
				    if (izero == 0) {

/*                                Compute residual of the computed */
/*                                solution. */

					clacpy_("Full", &n, nrhs, &b[1], &ldb, 
						 &work[1], &ldb);
					cgbt02_("No transpose", &n, &n, &kl, &
						ku, nrhs, &a[1], &lda, &x[1], 
						&ldb, &work[1], &ldb, &result[
						1]);

/*                                Check solution from generated exact */
/*                                solution. */

					cget04_(&n, nrhs, &x[1], &ldb, &xact[
						1], &ldb, &rcondc, &result[2])
						;
					nt = 3;
				    }

/*                             Print information about the tests that did */
/*                             not pass the threshold. */

				    i__8 = nt;
				    for (k = 1; k <= i__8; ++k) {
					if (result[k - 1] >= *thresh) {
					    if (nfail == 0 && nerrs == 0) {
			  aladhd_(nout, path);
					    }
					    io___65.ciunit = *nout;
					    s_wsfe(&io___65);
					    do_fio(&c__1, "CGBSV ", (ftnlen)6)
						    ;
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&k, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&result[k - 
						    1], (ftnlen)sizeof(real));
					    e_wsfe();
					    ++nfail;
					}
/* L50: */
				    }
				    nrun += nt;
				}

/*                          --- Test CGBSVX --- */

				if (! prefac) {
				    i__8 = (kl << 1) + ku + 1;
				    claset_("Full", &i__8, &n, &c_b48, &c_b48, 
					     &afb[1], &ldafb);
				}
				claset_("Full", &n, nrhs, &c_b48, &c_b48, &x[
					1], &ldb);
				if (iequed > 1 && n > 0) {

/*                             Equilibrate the matrix if FACT = 'F' and */
/*                             EQUED = 'R', 'C', or 'B'. */

				    claqgb_(&n, &n, &kl, &ku, &a[1], &lda, &s[
					    1], &s[n + 1], &rowcnd, &colcnd, &
					    amax, equed);
				}

/*                          Solve the system and compute the condition */
/*                          number and error bounds using CGBSVX. */

				s_copy(srnamc_1.srnamt, "CGBSVX", (ftnlen)6, (
					ftnlen)6);
				cgbsvx_(fact, trans, &n, &kl, &ku, nrhs, &a[1]
, &lda, &afb[1], &ldafb, &iwork[1], 
					equed, &s[1], &s[ldb + 1], &b[1], &
					ldb, &x[1], &ldb, &rcond, &rwork[1], &
					rwork[*nrhs + 1], &work[1], &rwork[(*
					nrhs << 1) + 1], &info);

/*                          Check the error code from CGBSVX. */

				if (info != izero) {
/* Writing concatenation */
				    i__11[0] = 1, a__1[0] = fact;
				    i__11[1] = 1, a__1[1] = trans;
				    s_cat(ch__1, a__1, i__11, &c__2, (ftnlen)
					    2);
				    alaerh_(path, "CGBSVX", &info, &izero, 
					    ch__1, &n, &n, &kl, &ku, nrhs, &
					    imat, &nfail, &nerrs, nout);
				}
/*                          Compare RWORK(2*NRHS+1) from CGBSVX with the */
/*                          computed reciprocal pivot growth RPVGRW */

				if (info != 0) {
				    anrmpv = 0.f;
				    i__8 = info;
				    for (j = 1; j <= i__8; ++j) {
/* Computing MAX */
					i__6 = ku + 2 - j;
/* Computing MIN */
					i__9 = n + ku + 1 - j, i__10 = kl + 
						ku + 1;
					i__7 = min(i__9,i__10);
					for (i__ = max(i__6,1); i__ <= i__7; 
						++i__) {
/* Computing MAX */
					    r__1 = anrmpv, r__2 = c_abs(&a[
						    i__ + (j - 1) * lda]);
					    anrmpv = dmax(r__1,r__2);
/* L60: */
					}
/* L70: */
				    }
/* Computing MIN */
				    i__7 = info - 1, i__6 = kl + ku;
				    i__8 = min(i__7,i__6);
/* Computing MAX */
				    i__9 = 1, i__10 = kl + ku + 2 - info;
				    rpvgrw = clantb_("M", "U", "N", &info, &
					    i__8, &afb[max(i__9, i__10)], &
					    ldafb, rdum);
				    if (rpvgrw == 0.f) {
					rpvgrw = 1.f;
				    } else {
					rpvgrw = anrmpv / rpvgrw;
				    }
				} else {
				    i__8 = kl + ku;
				    rpvgrw = clantb_("M", "U", "N", &n, &i__8, 
					     &afb[1], &ldafb, rdum);
				    if (rpvgrw == 0.f) {
					rpvgrw = 1.f;
				    } else {
					rpvgrw = clangb_("M", &n, &kl, &ku, &
						a[1], &lda, rdum) /
						 rpvgrw;
				    }
				}
/* Computing MAX */
				r__2 = rwork[(*nrhs << 1) + 1];
				result[6] = (r__1 = rpvgrw - rwork[(*nrhs << 
					1) + 1], dabs(r__1)) / dmax(r__2,
					rpvgrw) / slamch_("E");

				if (! prefac) {

/*                             Reconstruct matrix from factors and */
/*                             compute residual. */

				    cgbt01_(&n, &n, &kl, &ku, &a[1], &lda, &
					    afb[1], &ldafb, &iwork[1], &work[
					    1], result);
				    k1 = 1;
				} else {
				    k1 = 2;
				}

				if (info == 0) {
				    trfcon = FALSE_;

/*                             Compute residual of the computed solution. */

				    clacpy_("Full", &n, nrhs, &bsav[1], &ldb, 
					    &work[1], &ldb);
				    cgbt02_(trans, &n, &n, &kl, &ku, nrhs, &
					    asav[1], &lda, &x[1], &ldb, &work[
					    1], &ldb, &result[1]);

/*                             Check solution from generated exact */
/*                             solution. */

				    if (nofact || prefac && lsame_(equed, 
					    "N")) {
					cget04_(&n, nrhs, &x[1], &ldb, &xact[
						1], &ldb, &rcondc, &result[2])
						;
				    } else {
					if (itran == 1) {
					    roldc = roldo;
					} else {
					    roldc = roldi;
					}
					cget04_(&n, nrhs, &x[1], &ldb, &xact[
						1], &ldb, &roldc, &result[2]);
				    }

/*                             Check the error bounds from iterative */
/*                             refinement. */

				    cgbt05_(trans, &n, &kl, &ku, nrhs, &asav[
					    1], &lda, &bsav[1], &ldb, &x[1], &
					    ldb, &xact[1], &ldb, &rwork[1], &
					    rwork[*nrhs + 1], &result[3]);
				} else {
				    trfcon = TRUE_;
				}

/*                          Compare RCOND from CGBSVX with the computed */
/*                          value in RCONDC. */

				result[5] = sget06_(&rcond, &rcondc);

/*                          Print information about the tests that did */
/*                          not pass the threshold. */

				if (! trfcon) {
				    for (k = k1; k <= 7; ++k) {
					if (result[k - 1] >= *thresh) {
					    if (nfail == 0 && nerrs == 0) {
			  aladhd_(nout, path);
					    }
					    if (prefac) {
			  io___73.ciunit = *nout;
			  s_wsfe(&io___73);
			  do_fio(&c__1, "CGBSVX", (ftnlen)6);
			  do_fio(&c__1, fact, (ftnlen)1);
			  do_fio(&c__1, trans, (ftnlen)1);
			  do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			  do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(integer));
			  do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(integer));
			  do_fio(&c__1, equed, (ftnlen)1);
			  do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer)
				  );
			  do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
			  do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				  sizeof(real));
			  e_wsfe();
					    } else {
			  io___74.ciunit = *nout;
			  s_wsfe(&io___74);
			  do_fio(&c__1, "CGBSVX", (ftnlen)6);
			  do_fio(&c__1, fact, (ftnlen)1);
			  do_fio(&c__1, trans, (ftnlen)1);
			  do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			  do_fio(&c__1, (char *)&kl, (ftnlen)sizeof(integer));
			  do_fio(&c__1, (char *)&ku, (ftnlen)sizeof(integer));
			  do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(integer)
				  );
			  do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
			  do_fio(&c__1, (char *)&result[k - 1], (ftnlen)
				  sizeof(real));
			  e_wsfe();
					    }
					    ++nfail;
					}
/* L80: */
				    }
				    nrun = nrun + 7 - k1;
				} else {
				    if (result[0] >= *thresh && ! prefac) {
					if (nfail == 0 && nerrs == 0) {
					    aladhd_(nout, path);
					}
					if (prefac) {
					    io___75.ciunit = *nout;
					    s_wsfe(&io___75);
					    do_fio(&c__1, "CGBSVX", (ftnlen)6)
						    ;
					    do_fio(&c__1, fact, (ftnlen)1);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, equed, (ftnlen)1);
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&c__1, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&result[0], 
						    (ftnlen)sizeof(real));
					    e_wsfe();
					} else {
					    io___76.ciunit = *nout;
					    s_wsfe(&io___76);
					    do_fio(&c__1, "CGBSVX", (ftnlen)6)
						    ;
					    do_fio(&c__1, fact, (ftnlen)1);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&c__1, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&result[0], 
						    (ftnlen)sizeof(real));
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
					    io___77.ciunit = *nout;
					    s_wsfe(&io___77);
					    do_fio(&c__1, "CGBSVX", (ftnlen)6)
						    ;
					    do_fio(&c__1, fact, (ftnlen)1);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, equed, (ftnlen)1);
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&c__6, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&result[5], 
						    (ftnlen)sizeof(real));
					    e_wsfe();
					} else {
					    io___78.ciunit = *nout;
					    s_wsfe(&io___78);
					    do_fio(&c__1, "CGBSVX", (ftnlen)6)
						    ;
					    do_fio(&c__1, fact, (ftnlen)1);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&c__6, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&result[5], 
						    (ftnlen)sizeof(real));
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
					    io___79.ciunit = *nout;
					    s_wsfe(&io___79);
					    do_fio(&c__1, "CGBSVX", (ftnlen)6)
						    ;
					    do_fio(&c__1, fact, (ftnlen)1);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, equed, (ftnlen)1);
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&c__7, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&result[6], 
						    (ftnlen)sizeof(real));
					    e_wsfe();
					} else {
					    io___80.ciunit = *nout;
					    s_wsfe(&io___80);
					    do_fio(&c__1, "CGBSVX", (ftnlen)6)
						    ;
					    do_fio(&c__1, fact, (ftnlen)1);
					    do_fio(&c__1, trans, (ftnlen)1);
					    do_fio(&c__1, (char *)&n, (ftnlen)
						    sizeof(integer));
					    do_fio(&c__1, (char *)&kl, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&ku, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&imat, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&c__7, (
						    ftnlen)sizeof(integer));
					    do_fio(&c__1, (char *)&result[6], 
						    (ftnlen)sizeof(real));
					    e_wsfe();
					}
					++nfail;
					++nrun;
				    }
				}
/* L90: */
			    }
L100:
			    ;
			}
/* L110: */
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

/*     Print a summary of the results. */

    alasvm_(path, nout, &nfail, &nrun, &nerrs);


    return 0;

/*     End of CDRVGB */

} /* cdrvgb_ */
