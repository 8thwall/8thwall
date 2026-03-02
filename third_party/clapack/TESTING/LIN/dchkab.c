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
static integer c__3 = 3;
static integer c__12 = 12;
static integer c__0 = 0;
static integer c__132 = 132;
static integer c__16 = 16;
static integer c__5 = 5;
static integer c__8 = 8;
static integer c__6 = 6;

/* Main program */ int MAIN__(void)
{
    /* Format strings */
    static char fmt_9994[] = "(\002 Tests of the DOUBLE PRECISION LAPACK DSG"
	    "ESV routines \002,/\002 LAPACK VERSION \002,i1,\002.\002,i1,\002."
	    "\002,i1,//\002 The following parameter values will be used:\002)";
    static char fmt_9996[] = "(\002 Invalid input value: \002,a4,\002=\002,i"
	    "6,\002; must be >=\002,i6)";
    static char fmt_9995[] = "(\002 Invalid input value: \002,a4,\002=\002,i"
	    "6,\002; must be <=\002,i6)";
    static char fmt_9993[] = "(4x,a4,\002:  \002,10i6,/11x,10i6)";
    static char fmt_9992[] = "(/\002 Routines pass computational tests if te"
	    "st ratio is \002,\002less than\002,f8.2,/)";
    static char fmt_9999[] = "(/\002 Execution not attempted due to input er"
	    "rors\002)";
    static char fmt_9991[] = "(\002 Relative machine \002,a,\002 is taken to"
	    " be\002,d16.6)";
    static char fmt_9990[] = "(/1x,a6,\002 routines were not tested\002)";
    static char fmt_9989[] = "(/1x,a6,\002 driver routines were not teste"
	    "d\002)";
    static char fmt_9998[] = "(/\002 End of tests\002)";
    static char fmt_9997[] = "(\002 Total time used = \002,f12.2,\002 seco"
	    "nds\002,/)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_rsle(cilist *), e_rsle(void), s_wsfe(cilist *), do_fio(integer *
	    , char *, ftnlen), e_wsfe(void), do_lio(integer *, integer *, 
	    char *, ftnlen);
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer s_wsle(cilist *), e_wsle(void), f_clos(cllist *);

    /* Local variables */
    doublereal a[34848]	/* was [17424][2] */, b[4224]	/* was [2112][2] */;
    integer i__;
    doublereal s1, s2;
    integer nm, vers_patch__, vers_major__, vers_minor__, lda;
    doublereal eps;
    integer nns, mval[12], nrhs;
    real seps;
    doublereal work[4224];
    logical fatal;
    integer nmats, nsval[12], iwork[132];
    doublereal rwork[132];
    real swork[19536];
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int derrab_(integer *);
    extern doublereal dsecnd_(void);
    extern /* Subroutine */ int ddrvab_(logical *, integer *, integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, real *, integer *, integer *), alareq_(char *, 
	    integer *, logical *, integer *, integer *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int ilaver_(integer *, integer *, integer *);
    doublereal thresh;
    logical dotype[30];
    integer ntypes;
    logical tsterr, tstdrv;

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 5, 0, 0, 0 };
    static cilist io___8 = { 0, 6, 0, fmt_9994, 0 };
    static cilist io___9 = { 0, 5, 0, 0, 0 };
    static cilist io___11 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___12 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___13 = { 0, 5, 0, 0, 0 };
    static cilist io___16 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___17 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___18 = { 0, 6, 0, fmt_9993, 0 };
    static cilist io___19 = { 0, 5, 0, 0, 0 };
    static cilist io___21 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___22 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___23 = { 0, 5, 0, 0, 0 };
    static cilist io___25 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___26 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_9993, 0 };
    static cilist io___28 = { 0, 5, 0, 0, 0 };
    static cilist io___30 = { 0, 6, 0, fmt_9992, 0 };
    static cilist io___31 = { 0, 5, 0, 0, 0 };
    static cilist io___33 = { 0, 5, 0, 0, 0 };
    static cilist io___35 = { 0, 6, 0, fmt_9999, 0 };
    static cilist io___37 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___38 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___40 = { 0, 6, 0, 0, 0 };
    static cilist io___42 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___43 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___44 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___45 = { 0, 6, 0, 0, 0 };
    static cilist io___47 = { 0, 5, 0, 0, 0 };
    static cilist io___49 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___58 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___60 = { 0, 6, 0, fmt_9998, 0 };
    static cilist io___61 = { 0, 6, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*  Purpose */
/*  ======= */

/*  DCHKAB is the test program for the DOUBLE PRECISION LAPACK */
/*  DSGESV routine */

/*  The program must be driven by a short data file. The first 5 records */
/*  specify problem dimensions and program options using list-directed */
/*  input. The remaining lines specify the LAPACK test paths and the */
/*  number of matrix types to use in testing.  An annotated example of a */
/*  data file can be obtained by deleting the first 3 characters from the */
/*  following 9 lines: */
/*  Data file for testing DOUBLE PRECISION LAPACK DSGESV */
/*  7                      Number of values of M */
/*  0 1 2 3 5 10 16        Values of M (row dimension) */
/*  1                      Number of values of NRHS */
/*  2                      Values of NRHS (number of right hand sides) */
/*  20.0                   Threshold value of test ratio */
/*  T                      Put T to test the DSGESV routine */
/*  T                      Put T to test the error exits for DSGESV */
/*  11                     List types on next line if 0 < NTYPES < 11 */

/*  Internal Parameters */
/*  =================== */

/*  NMAX    INTEGER */
/*          The maximum allowable value for N */

/*  MAXIN   INTEGER */
/*          The number of different values that can be used for each of */
/*          M, N, NRHS, NB, and NX */

/*  MAXRHS  INTEGER */
/*          The maximum number of right hand sides */

/*  NIN     INTEGER */
/*          The unit number for input */

/*  NOUT    INTEGER */
/*          The unit number for output */

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
/*     .. Executable Statements .. */

    s1 = dsecnd_();
    lda = 132;
    fatal = FALSE_;

/*     Read a dummy line. */

    s_rsle(&io___4);
    e_rsle();

/*     Report values of parameters. */

    ilaver_(&vers_major__, &vers_minor__, &vers_patch__);
    s_wsfe(&io___8);
    do_fio(&c__1, (char *)&vers_major__, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&vers_minor__, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&vers_patch__, (ftnlen)sizeof(integer));
    e_wsfe();

/*     Read the values of M */

    s_rsle(&io___9);
    do_lio(&c__3, &c__1, (char *)&nm, (ftnlen)sizeof(integer));
    e_rsle();
    if (nm < 1) {
	s_wsfe(&io___11);
	do_fio(&c__1, " NM ", (ftnlen)4);
	do_fio(&c__1, (char *)&nm, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	nm = 0;
	fatal = TRUE_;
    } else if (nm > 12) {
	s_wsfe(&io___12);
	do_fio(&c__1, " NM ", (ftnlen)4);
	do_fio(&c__1, (char *)&nm, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__12, (ftnlen)sizeof(integer));
	e_wsfe();
	nm = 0;
	fatal = TRUE_;
    }
    s_rsle(&io___13);
    i__1 = nm;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
    }
    e_rsle();
    i__1 = nm;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (mval[i__ - 1] < 0) {
	    s_wsfe(&io___16);
	    do_fio(&c__1, " M  ", (ftnlen)4);
	    do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (mval[i__ - 1] > 132) {
	    s_wsfe(&io___17);
	    do_fio(&c__1, " M  ", (ftnlen)4);
	    do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
/* L10: */
    }
    if (nm > 0) {
	s_wsfe(&io___18);
	do_fio(&c__1, "M   ", (ftnlen)4);
	i__1 = nm;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the values of NRHS */

    s_rsle(&io___19);
    do_lio(&c__3, &c__1, (char *)&nns, (ftnlen)sizeof(integer));
    e_rsle();
    if (nns < 1) {
	s_wsfe(&io___21);
	do_fio(&c__1, " NNS", (ftnlen)4);
	do_fio(&c__1, (char *)&nns, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	nns = 0;
	fatal = TRUE_;
    } else if (nns > 12) {
	s_wsfe(&io___22);
	do_fio(&c__1, " NNS", (ftnlen)4);
	do_fio(&c__1, (char *)&nns, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__12, (ftnlen)sizeof(integer));
	e_wsfe();
	nns = 0;
	fatal = TRUE_;
    }
    s_rsle(&io___23);
    i__1 = nns;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer))
		;
    }
    e_rsle();
    i__1 = nns;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nsval[i__ - 1] < 0) {
	    s_wsfe(&io___25);
	    do_fio(&c__1, "NRHS", (ftnlen)4);
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nsval[i__ - 1] > 16) {
	    s_wsfe(&io___26);
	    do_fio(&c__1, "NRHS", (ftnlen)4);
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__16, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
/* L30: */
    }
    if (nns > 0) {
	s_wsfe(&io___27);
	do_fio(&c__1, "NRHS", (ftnlen)4);
	i__1 = nns;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the threshold value for the test ratios. */

    s_rsle(&io___28);
    do_lio(&c__5, &c__1, (char *)&thresh, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_wsfe(&io___30);
    do_fio(&c__1, (char *)&thresh, (ftnlen)sizeof(doublereal));
    e_wsfe();

/*     Read the flag that indicates whether to test the driver routine. */

    s_rsle(&io___31);
    do_lio(&c__8, &c__1, (char *)&tstdrv, (ftnlen)sizeof(logical));
    e_rsle();

/*     Read the flag that indicates whether to test the error exits. */

    s_rsle(&io___33);
    do_lio(&c__8, &c__1, (char *)&tsterr, (ftnlen)sizeof(logical));
    e_rsle();

    if (fatal) {
	s_wsfe(&io___35);
	e_wsfe();
	s_stop("", (ftnlen)0);
    }

/*     Calculate and print the machine dependent constants. */

    seps = slamch_("Underflow threshold");
    s_wsfe(&io___37);
    do_fio(&c__1, "(single precision) underflow", (ftnlen)28);
    do_fio(&c__1, (char *)&seps, (ftnlen)sizeof(real));
    e_wsfe();
    seps = slamch_("Overflow threshold");
    s_wsfe(&io___38);
    do_fio(&c__1, "(single precision) overflow ", (ftnlen)28);
    do_fio(&c__1, (char *)&seps, (ftnlen)sizeof(real));
    e_wsfe();
    seps = slamch_("Epsilon");
    s_wsfe(&io___39);
    do_fio(&c__1, "(single precision) precision", (ftnlen)28);
    do_fio(&c__1, (char *)&seps, (ftnlen)sizeof(real));
    e_wsfe();
    s_wsle(&io___40);
    e_wsle();

    eps = dlamch_("Underflow threshold");
    s_wsfe(&io___42);
    do_fio(&c__1, "(double precision) underflow", (ftnlen)28);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    eps = dlamch_("Overflow threshold");
    s_wsfe(&io___43);
    do_fio(&c__1, "(double precision) overflow ", (ftnlen)28);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    eps = dlamch_("Epsilon");
    s_wsfe(&io___44);
    do_fio(&c__1, "(double precision) precision", (ftnlen)28);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    s_wsle(&io___45);
    e_wsle();

    nrhs = nsval[0];
    s_rsle(&io___47);
    do_lio(&c__3, &c__1, (char *)&nmats, (ftnlen)sizeof(integer));
    e_rsle();

    if (nmats <= 0) {

/*        Check for a positive number of tests requested. */

	s_wsfe(&io___49);
	do_fio(&c__1, "DSGESV", (ftnlen)6);
	e_wsfe();
	goto L140;

    }

    ntypes = 11;
    alareq_("DGE", &nmats, dotype, &ntypes, &c__5, &c__6);

/*     Test the error exits */

    if (tsterr) {
	derrab_(&c__6);
    }

    if (tstdrv) {
	ddrvab_(dotype, &nm, mval, &nns, nsval, &thresh, &lda, a, &a[17424], 
		b, &b[2112], work, rwork, swork, iwork, &c__6);
    } else {
	s_wsfe(&io___58);
	do_fio(&c__1, "DSGESV", (ftnlen)6);
	e_wsfe();
    }

L140:
    cl__1.cerr = 0;
    cl__1.cunit = 5;
    cl__1.csta = 0;
    f_clos(&cl__1);
    s2 = dsecnd_();
    s_wsfe(&io___60);
    e_wsfe();
    s_wsfe(&io___61);
    d__1 = s2 - s1;
    do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
    e_wsfe();


/*     End of DCHKAB */

    return 0;
} /* MAIN__ */

/* Main program alias */ int dchkab_ () { MAIN__ (); return 0; }
