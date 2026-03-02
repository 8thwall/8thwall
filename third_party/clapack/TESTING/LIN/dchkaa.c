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

struct {
    integer iparms[100];
} claenv_;

#define claenv_1 claenv_

/* Table of constant values */

static integer c__1 = 1;
static integer c__3 = 3;
static integer c__12 = 12;
static integer c__0 = 0;
static integer c__132 = 132;
static integer c__16 = 16;
static integer c__5 = 5;
static integer c__8 = 8;
static integer c__2 = 2;
static integer c__6 = 6;

/* Main program */ int MAIN__(void)
{
    /* Initialized data */

    static doublereal threq = 2.;
    static char intstr[10] = "0123456789";

    /* Format strings */
    static char fmt_9994[] = "(\002 Tests of the DOUBLE PRECISION LAPACK rou"
	    "tines \002,/\002 LAPACK VERSION \002,i1,\002.\002,i1,\002.\002,i"
	    "1,//\002 The following parameter values will be used:\002)";
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
    static char fmt_9990[] = "(/1x,a3,\002:  Unrecognized path name\002)";
    static char fmt_9989[] = "(/1x,a3,\002 routines were not tested\002)";
    static char fmt_9988[] = "(/1x,a3,\002 driver routines were not teste"
	    "d\002)";
    static char fmt_9998[] = "(/\002 End of tests\002)";
    static char fmt_9997[] = "(\002 Total time used = \002,f12.2,\002 seco"
	    "nds\002,/)";

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;
    cilist ci__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_rsle(cilist *), e_rsle(void), s_wsfe(cilist *), do_fio(integer *
	    , char *, ftnlen), e_wsfe(void), do_lio(integer *, integer *, 
	    char *, ftnlen);
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer s_wsle(cilist *), e_wsle(void), s_rsfe(cilist *), e_rsfe(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer f_clos(cllist *);

    /* Local variables */
    doublereal a[153384]	/* was [21912][7] */, b[8448]	/* was [2112][
	    4] */;
    integer i__, j, k;
    doublereal s[264];
    char c1[1], c2[2];
    doublereal s1, s2;
    integer ic, la, nb, nm, nn, vers_patch__, vers_major__, vers_minor__, lda,
	     nnb;
    doublereal eps;
    integer nns, nnb2;
    char path[3];
    integer mval[12], nval[12], nrhs;
    doublereal work[23496]	/* was [132][178] */;
    integer lafac;
    logical fatal;
    char aline[72];
    extern logical lsame_(char *, char *);
    integer nbval[12], nmats, nsval[12], nxval[12], iwork[3300];
    doublereal rwork[692];
    extern /* Subroutine */ int dchkq3_(logical *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, doublereal 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *);
    integer nbval2[12];
    extern /* Subroutine */ int dchkgb_(logical *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    doublereal *, logical *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, integer *, integer *), dchkge_(logical *, integer *
, integer *, integer *, integer *, integer *, integer *, integer *
, integer *, doublereal *, logical *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int dchkpb_(logical *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, doublereal *, logical 
	    *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), dchkeq_(doublereal *, 
	    integer *);
    extern doublereal dsecnd_(void);
    extern /* Subroutine */ int dchktb_(logical *, integer *, integer *, 
	    integer *, integer *, doublereal *, logical *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    dchkgt_(logical *, integer *, integer *, integer *, integer *, 
	    doublereal *, logical *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, integer *
, integer *), alareq_(char *, integer *, logical *, integer *, 
	    integer *, integer *), dchklq_(logical *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, doublereal *, logical *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *), dchkql_(
	    logical *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, logical *, integer 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    dchkpo_(logical *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, doublereal *, logical *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dchkpp_(logical *, integer *, integer *, integer *, 
	    integer *, doublereal *, logical *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    dchkqp_(logical *, integer *, integer *, integer *, integer *, 
	    doublereal *, logical *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    ddrvgb_(logical *, integer *, integer *, integer *, doublereal *, 
	    logical *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dchkqr_(logical *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, doublereal 
	    *, logical *, integer *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dchkpt_(logical *, integer *, integer *, integer *, 
	    integer *, doublereal *, logical *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *), dchkrq_(logical *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, doublereal *, logical *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int dchksp_(logical *, integer *, integer *, 
	    integer *, integer *, doublereal *, logical *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dchktp_(logical *, integer *, integer *, integer *, 
	    integer *, doublereal *, logical *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *), dchktr_(
	    logical *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, doublereal *, logical *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *), ddrvge_(
	    logical *, integer *, integer *, integer *, doublereal *, logical 
	    *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    ddrvpb_(logical *, integer *, integer *, integer *, doublereal *, 
	    logical *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    dchksy_(logical *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, doublereal *, logical *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), ilaver_(integer *, integer *, integer *), dchktz_(
	    logical *, integer *, integer *, integer *, integer *, doublereal 
	    *, logical *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *), ddrvgt_(
	    logical *, integer *, integer *, integer *, doublereal *, logical 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *);
    doublereal thresh;
    extern /* Subroutine */ int ddrvls_(logical *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    integer *, doublereal *, logical *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *), ddrvpo_(
	    logical *, integer *, integer *, integer *, doublereal *, logical 
	    *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *);
    logical tstchk;
    extern /* Subroutine */ int ddrvpp_(logical *, integer *, integer *, 
	    integer *, doublereal *, logical *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), ddrvpt_(logical *, integer *, 
	     integer *, integer *, doublereal *, logical *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *);
    logical dotype[30];
    extern /* Subroutine */ int ddrvsp_(logical *, integer *, integer *, 
	    integer *, doublereal *, logical *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    ddrvsy_(logical *, integer *, integer *, integer *, doublereal *, 
	    logical *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *);
    integer ntypes;
    logical tsterr, tstdrv;

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 5, 0, 0, 0 };
    static cilist io___10 = { 0, 6, 0, fmt_9994, 0 };
    static cilist io___11 = { 0, 5, 0, 0, 0 };
    static cilist io___13 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___14 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___15 = { 0, 5, 0, 0, 0 };
    static cilist io___18 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___19 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___20 = { 0, 6, 0, fmt_9993, 0 };
    static cilist io___21 = { 0, 5, 0, 0, 0 };
    static cilist io___23 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___24 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___25 = { 0, 5, 0, 0, 0 };
    static cilist io___27 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___28 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___29 = { 0, 6, 0, fmt_9993, 0 };
    static cilist io___30 = { 0, 5, 0, 0, 0 };
    static cilist io___32 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___33 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___34 = { 0, 5, 0, 0, 0 };
    static cilist io___36 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___37 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___38 = { 0, 6, 0, fmt_9993, 0 };
    static cilist io___39 = { 0, 5, 0, 0, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___42 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___43 = { 0, 5, 0, 0, 0 };
    static cilist io___45 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___46 = { 0, 6, 0, fmt_9993, 0 };
    static cilist io___51 = { 0, 5, 0, 0, 0 };
    static cilist io___53 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___54 = { 0, 6, 0, fmt_9993, 0 };
    static cilist io___55 = { 0, 5, 0, 0, 0 };
    static cilist io___57 = { 0, 6, 0, fmt_9992, 0 };
    static cilist io___58 = { 0, 5, 0, 0, 0 };
    static cilist io___60 = { 0, 5, 0, 0, 0 };
    static cilist io___62 = { 0, 5, 0, 0, 0 };
    static cilist io___64 = { 0, 6, 0, fmt_9999, 0 };
    static cilist io___66 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___67 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___68 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___69 = { 0, 6, 0, 0, 0 };
    static cilist io___78 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___79 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___87 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___89 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___92 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___93 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___94 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___95 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___96 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___97 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___98 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___99 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___100 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___101 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___102 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___103 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___104 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___105 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___106 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___107 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___108 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___109 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___110 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___111 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___112 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___113 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___114 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___115 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___116 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___117 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___118 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___119 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___121 = { 0, 6, 0, fmt_9998, 0 };
    static cilist io___122 = { 0, 6, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*  Purpose */
/*  ======= */

/*  DCHKAA is the main test program for the DOUBLE PRECISION LAPACK */
/*  linear equation routines */

/*  The program must be driven by a short data file. The first 14 records */
/*  specify problem dimensions and program options using list-directed */
/*  input.  The remaining lines specify the LAPACK test paths and the */
/*  number of matrix types to use in testing.  An annotated example of a */
/*  data file can be obtained by deleting the first 3 characters from the */
/*  following 36 lines: */
/*  Data file for testing DOUBLE PRECISION LAPACK linear eqn. routines */
/*  7                      Number of values of M */
/*  0 1 2 3 5 10 16        Values of M (row dimension) */
/*  7                      Number of values of N */
/*  0 1 2 3 5 10 16        Values of N (column dimension) */
/*  1                      Number of values of NRHS */
/*  2                      Values of NRHS (number of right hand sides) */
/*  5                      Number of values of NB */
/*  1 3 3 3 20             Values of NB (the blocksize) */
/*  1 0 5 9 1              Values of NX (crossover point) */
/*  20.0                   Threshold value of test ratio */
/*  T                      Put T to test the LAPACK routines */
/*  T                      Put T to test the driver routines */
/*  T                      Put T to test the error exits */
/*  DGE   11               List types on next line if 0 < NTYPES < 11 */
/*  DGB    8               List types on next line if 0 < NTYPES <  8 */
/*  DGT   12               List types on next line if 0 < NTYPES < 12 */
/*  DPO    9               List types on next line if 0 < NTYPES <  9 */
/*  DPP    9               List types on next line if 0 < NTYPES <  9 */
/*  DPB    8               List types on next line if 0 < NTYPES <  8 */
/*  DPT   12               List types on next line if 0 < NTYPES < 12 */
/*  DSY   10               List types on next line if 0 < NTYPES < 10 */
/*  DSP   10               List types on next line if 0 < NTYPES < 10 */
/*  DTR   18               List types on next line if 0 < NTYPES < 18 */
/*  DTP   18               List types on next line if 0 < NTYPES < 18 */
/*  DTB   17               List types on next line if 0 < NTYPES < 17 */
/*  DQR    8               List types on next line if 0 < NTYPES <  8 */
/*  DRQ    8               List types on next line if 0 < NTYPES <  8 */
/*  DLQ    8               List types on next line if 0 < NTYPES <  8 */
/*  DQL    8               List types on next line if 0 < NTYPES <  8 */
/*  DQP    6               List types on next line if 0 < NTYPES <  6 */
/*  DTZ    3               List types on next line if 0 < NTYPES <  3 */
/*  DLS    6               List types on next line if 0 < NTYPES <  6 */
/*  DEQ */

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
/*     .. Arrays in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
/*     .. Executable Statements .. */

    s1 = dsecnd_();
    lda = 132;
    fatal = FALSE_;

/*     Read a dummy line. */

    s_rsle(&io___6);
    e_rsle();

/*     Report values of parameters. */

    ilaver_(&vers_major__, &vers_minor__, &vers_patch__);
    s_wsfe(&io___10);
    do_fio(&c__1, (char *)&vers_major__, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&vers_minor__, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&vers_patch__, (ftnlen)sizeof(integer));
    e_wsfe();

/*     Read the values of M */

    s_rsle(&io___11);
    do_lio(&c__3, &c__1, (char *)&nm, (ftnlen)sizeof(integer));
    e_rsle();
    if (nm < 1) {
	s_wsfe(&io___13);
	do_fio(&c__1, " NM ", (ftnlen)4);
	do_fio(&c__1, (char *)&nm, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	nm = 0;
	fatal = TRUE_;
    } else if (nm > 12) {
	s_wsfe(&io___14);
	do_fio(&c__1, " NM ", (ftnlen)4);
	do_fio(&c__1, (char *)&nm, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__12, (ftnlen)sizeof(integer));
	e_wsfe();
	nm = 0;
	fatal = TRUE_;
    }
    s_rsle(&io___15);
    i__1 = nm;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
    }
    e_rsle();
    i__1 = nm;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (mval[i__ - 1] < 0) {
	    s_wsfe(&io___18);
	    do_fio(&c__1, " M  ", (ftnlen)4);
	    do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (mval[i__ - 1] > 132) {
	    s_wsfe(&io___19);
	    do_fio(&c__1, " M  ", (ftnlen)4);
	    do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
/* L10: */
    }
    if (nm > 0) {
	s_wsfe(&io___20);
	do_fio(&c__1, "M   ", (ftnlen)4);
	i__1 = nm;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the values of N */

    s_rsle(&io___21);
    do_lio(&c__3, &c__1, (char *)&nn, (ftnlen)sizeof(integer));
    e_rsle();
    if (nn < 1) {
	s_wsfe(&io___23);
	do_fio(&c__1, " NN ", (ftnlen)4);
	do_fio(&c__1, (char *)&nn, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	nn = 0;
	fatal = TRUE_;
    } else if (nn > 12) {
	s_wsfe(&io___24);
	do_fio(&c__1, " NN ", (ftnlen)4);
	do_fio(&c__1, (char *)&nn, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__12, (ftnlen)sizeof(integer));
	e_wsfe();
	nn = 0;
	fatal = TRUE_;
    }
    s_rsle(&io___25);
    i__1 = nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(integer));
    }
    e_rsle();
    i__1 = nn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nval[i__ - 1] < 0) {
	    s_wsfe(&io___27);
	    do_fio(&c__1, " N  ", (ftnlen)4);
	    do_fio(&c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nval[i__ - 1] > 132) {
	    s_wsfe(&io___28);
	    do_fio(&c__1, " N  ", (ftnlen)4);
	    do_fio(&c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
/* L20: */
    }
    if (nn > 0) {
	s_wsfe(&io___29);
	do_fio(&c__1, "N   ", (ftnlen)4);
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the values of NRHS */

    s_rsle(&io___30);
    do_lio(&c__3, &c__1, (char *)&nns, (ftnlen)sizeof(integer));
    e_rsle();
    if (nns < 1) {
	s_wsfe(&io___32);
	do_fio(&c__1, " NNS", (ftnlen)4);
	do_fio(&c__1, (char *)&nns, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	nns = 0;
	fatal = TRUE_;
    } else if (nns > 12) {
	s_wsfe(&io___33);
	do_fio(&c__1, " NNS", (ftnlen)4);
	do_fio(&c__1, (char *)&nns, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__12, (ftnlen)sizeof(integer));
	e_wsfe();
	nns = 0;
	fatal = TRUE_;
    }
    s_rsle(&io___34);
    i__1 = nns;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer))
		;
    }
    e_rsle();
    i__1 = nns;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nsval[i__ - 1] < 0) {
	    s_wsfe(&io___36);
	    do_fio(&c__1, "NRHS", (ftnlen)4);
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nsval[i__ - 1] > 16) {
	    s_wsfe(&io___37);
	    do_fio(&c__1, "NRHS", (ftnlen)4);
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__16, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
/* L30: */
    }
    if (nns > 0) {
	s_wsfe(&io___38);
	do_fio(&c__1, "NRHS", (ftnlen)4);
	i__1 = nns;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the values of NB */

    s_rsle(&io___39);
    do_lio(&c__3, &c__1, (char *)&nnb, (ftnlen)sizeof(integer));
    e_rsle();
    if (nnb < 1) {
	s_wsfe(&io___41);
	do_fio(&c__1, "NNB ", (ftnlen)4);
	do_fio(&c__1, (char *)&nnb, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	nnb = 0;
	fatal = TRUE_;
    } else if (nnb > 12) {
	s_wsfe(&io___42);
	do_fio(&c__1, "NNB ", (ftnlen)4);
	do_fio(&c__1, (char *)&nnb, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__12, (ftnlen)sizeof(integer));
	e_wsfe();
	nnb = 0;
	fatal = TRUE_;
    }
    s_rsle(&io___43);
    i__1 = nnb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer))
		;
    }
    e_rsle();
    i__1 = nnb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nbval[i__ - 1] < 0) {
	    s_wsfe(&io___45);
	    do_fio(&c__1, " NB ", (ftnlen)4);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
/* L40: */
    }
    if (nnb > 0) {
	s_wsfe(&io___46);
	do_fio(&c__1, "NB  ", (ftnlen)4);
	i__1 = nnb;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Set NBVAL2 to be the set of unique values of NB */

    nnb2 = 0;
    i__1 = nnb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	nb = nbval[i__ - 1];
	i__2 = nnb2;
	for (j = 1; j <= i__2; ++j) {
	    if (nb == nbval2[j - 1]) {
		goto L60;
	    }
/* L50: */
	}
	++nnb2;
	nbval2[nnb2 - 1] = nb;
L60:
	;
    }

/*     Read the values of NX */

    s_rsle(&io___51);
    i__1 = nnb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__3, &c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer))
		;
    }
    e_rsle();
    i__1 = nnb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (nxval[i__ - 1] < 0) {
	    s_wsfe(&io___53);
	    do_fio(&c__1, " NX ", (ftnlen)4);
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
/* L70: */
    }
    if (nnb > 0) {
	s_wsfe(&io___54);
	do_fio(&c__1, "NX  ", (ftnlen)4);
	i__1 = nnb;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the threshold value for the test ratios. */

    s_rsle(&io___55);
    do_lio(&c__5, &c__1, (char *)&thresh, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_wsfe(&io___57);
    do_fio(&c__1, (char *)&thresh, (ftnlen)sizeof(doublereal));
    e_wsfe();

/*     Read the flag that indicates whether to test the LAPACK routines. */

    s_rsle(&io___58);
    do_lio(&c__8, &c__1, (char *)&tstchk, (ftnlen)sizeof(logical));
    e_rsle();

/*     Read the flag that indicates whether to test the driver routines. */

    s_rsle(&io___60);
    do_lio(&c__8, &c__1, (char *)&tstdrv, (ftnlen)sizeof(logical));
    e_rsle();

/*     Read the flag that indicates whether to test the error exits. */

    s_rsle(&io___62);
    do_lio(&c__8, &c__1, (char *)&tsterr, (ftnlen)sizeof(logical));
    e_rsle();

    if (fatal) {
	s_wsfe(&io___64);
	e_wsfe();
	s_stop("", (ftnlen)0);
    }

/*     Calculate and print the machine dependent constants. */

    eps = dlamch_("Underflow threshold");
    s_wsfe(&io___66);
    do_fio(&c__1, "underflow", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    eps = dlamch_("Overflow threshold");
    s_wsfe(&io___67);
    do_fio(&c__1, "overflow ", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    eps = dlamch_("Epsilon");
    s_wsfe(&io___68);
    do_fio(&c__1, "precision", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    s_wsle(&io___69);
    e_wsle();

L80:

/*     Read a test path and the number of matrix types to use. */

    ci__1.cierr = 0;
    ci__1.ciend = 1;
    ci__1.ciunit = 5;
    ci__1.cifmt = "(A72)";
    i__1 = s_rsfe(&ci__1);
    if (i__1 != 0) {
	goto L140;
    }
    i__1 = do_fio(&c__1, aline, (ftnlen)72);
    if (i__1 != 0) {
	goto L140;
    }
    i__1 = e_rsfe();
    if (i__1 != 0) {
	goto L140;
    }
    s_copy(path, aline, (ftnlen)3, (ftnlen)3);
    nmats = 30;
    i__ = 3;
L90:
    ++i__;
    if (i__ > 72) {
	nmats = 30;
	goto L130;
    }
    if (*(unsigned char *)&aline[i__ - 1] == ' ') {
	goto L90;
    }
    nmats = 0;
L100:
    *(unsigned char *)c1 = *(unsigned char *)&aline[i__ - 1];
    for (k = 1; k <= 10; ++k) {
	if (*(unsigned char *)c1 == *(unsigned char *)&intstr[k - 1]) {
	    ic = k - 1;
	    goto L120;
	}
/* L110: */
    }
    goto L130;
L120:
    nmats = nmats * 10 + ic;
    ++i__;
    if (i__ > 72) {
	goto L130;
    }
    goto L100;
L130:
    *(unsigned char *)c1 = *(unsigned char *)path;
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);
    nrhs = nsval[0];

/*     Check first character for correct precision. */

    if (! lsame_(c1, "Double precision")) {
	s_wsfe(&io___78);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();

    } else if (nmats <= 0) {

/*        Check for a positive number of tests requested. */

	s_wsfe(&io___79);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();

    } else if (lsamen_(&c__2, c2, "GE")) {

/*        GE:  general matrices */

	ntypes = 11;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkge_(dotype, &nm, mval, &nn, nval, &nnb2, nbval2, &nns, nsval, 
		    &thresh, &tsterr, &lda, a, &a[21912], &a[43824], b, &b[
		    2112], &b[4224], work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___87);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvge_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, &lda, a, &a[
		    21912], &a[43824], b, &b[2112], &b[4224], &b[6336], s, 
		    work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___89);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        GB:  general banded matrices */

	la = 43692;
	lafac = 65472;
	ntypes = 8;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkgb_(dotype, &nm, mval, &nn, nval, &nnb2, nbval2, &nns, nsval, 
		    &thresh, &tsterr, a, &la, &a[43824], &lafac, b, &b[2112], 
		    &b[4224], work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___92);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvgb_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, a, &la, &a[
		    43824], &lafac, &a[109560], b, &b[2112], &b[4224], &b[
		    6336], s, work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___93);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "GT")) {

/*        GT:  general tridiagonal matrices */

	ntypes = 12;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkgt_(dotype, &nn, nval, &nns, nsval, &thresh, &tsterr, a, &a[
		    21912], b, &b[2112], &b[4224], work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___94);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvgt_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, a, &a[21912], 
		    b, &b[2112], &b[4224], work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___95);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "PO")) {

/*        PO:  positive definite matrices */

	ntypes = 9;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkpo_(dotype, &nn, nval, &nnb2, nbval2, &nns, nsval, &thresh, &
		    tsterr, &lda, a, &a[21912], &a[43824], b, &b[2112], &b[
		    4224], work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___96);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvpo_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, &lda, a, &a[
		    21912], &a[43824], b, &b[2112], &b[4224], &b[6336], s, 
		    work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___97);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        PP:  positive definite packed matrices */

	ntypes = 9;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkpp_(dotype, &nn, nval, &nns, nsval, &thresh, &tsterr, &lda, a, 
		     &a[21912], &a[43824], b, &b[2112], &b[4224], work, rwork, 
		     iwork, &c__6);
	} else {
	    s_wsfe(&io___98);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvpp_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, &lda, a, &a[
		    21912], &a[43824], b, &b[2112], &b[4224], &b[6336], s, 
		    work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___99);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        PB:  positive definite banded matrices */

	ntypes = 8;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkpb_(dotype, &nn, nval, &nnb2, nbval2, &nns, nsval, &thresh, &
		    tsterr, &lda, a, &a[21912], &a[43824], b, &b[2112], &b[
		    4224], work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___100);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvpb_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, &lda, a, &a[
		    21912], &a[43824], b, &b[2112], &b[4224], &b[6336], s, 
		    work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___101);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        PT:  positive definite tridiagonal matrices */

	ntypes = 12;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkpt_(dotype, &nn, nval, &nns, nsval, &thresh, &tsterr, a, &a[
		    21912], &a[43824], b, &b[2112], &b[4224], work, rwork, &
		    c__6);
	} else {
	    s_wsfe(&io___102);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvpt_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, a, &a[21912], 
		    &a[43824], b, &b[2112], &b[4224], work, rwork, &c__6);
	} else {
	    s_wsfe(&io___103);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "SY")) {

/*        SY:  symmetric indefinite matrices */

	ntypes = 10;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchksy_(dotype, &nn, nval, &nnb2, nbval2, &nns, nsval, &thresh, &
		    tsterr, &lda, a, &a[21912], &a[43824], b, &b[2112], &b[
		    4224], work, rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___104);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvsy_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, &lda, a, &a[
		    21912], &a[43824], b, &b[2112], &b[4224], work, rwork, 
		    iwork, &c__6);
	} else {
	    s_wsfe(&io___105);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        SP:  symmetric indefinite packed matrices */

	ntypes = 10;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchksp_(dotype, &nn, nval, &nns, nsval, &thresh, &tsterr, &lda, a, 
		     &a[21912], &a[43824], b, &b[2112], &b[4224], work, rwork, 
		     iwork, &c__6);
	} else {
	    s_wsfe(&io___106);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

	if (tstdrv) {
	    ddrvsp_(dotype, &nn, nval, &nrhs, &thresh, &tsterr, &lda, a, &a[
		    21912], &a[43824], b, &b[2112], &b[4224], work, rwork, 
		    iwork, &c__6);
	} else {
	    s_wsfe(&io___107);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "TR")) {

/*        TR:  triangular matrices */

	ntypes = 18;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchktr_(dotype, &nn, nval, &nnb2, nbval2, &nns, nsval, &thresh, &
		    tsterr, &lda, a, &a[21912], b, &b[2112], &b[4224], work, 
		    rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___108);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "TP")) {

/*        TP:  triangular packed matrices */

	ntypes = 18;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchktp_(dotype, &nn, nval, &nns, nsval, &thresh, &tsterr, &lda, a, 
		     &a[21912], b, &b[2112], &b[4224], work, rwork, iwork, &
		    c__6);
	} else {
	    s_wsfe(&io___109);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "TB")) {

/*        TB:  triangular banded matrices */

	ntypes = 17;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchktb_(dotype, &nn, nval, &nns, nsval, &thresh, &tsterr, &lda, a, 
		     &a[21912], b, &b[2112], &b[4224], work, rwork, iwork, &
		    c__6);
	} else {
	    s_wsfe(&io___110);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "QR")) {

/*        QR:  QR factorization */

	ntypes = 8;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkqr_(dotype, &nm, mval, &nn, nval, &nnb, nbval, nxval, &nrhs, &
		    thresh, &tsterr, &c__132, a, &a[21912], &a[43824], &a[
		    65736], &a[87648], b, &b[2112], &b[4224], &b[6336], work, 
		    rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___111);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "LQ")) {

/*        LQ:  LQ factorization */

	ntypes = 8;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchklq_(dotype, &nm, mval, &nn, nval, &nnb, nbval, nxval, &nrhs, &
		    thresh, &tsterr, &c__132, a, &a[21912], &a[43824], &a[
		    65736], &a[87648], b, &b[2112], &b[4224], &b[6336], work, 
		    rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___112);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "QL")) {

/*        QL:  QL factorization */

	ntypes = 8;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkql_(dotype, &nm, mval, &nn, nval, &nnb, nbval, nxval, &nrhs, &
		    thresh, &tsterr, &c__132, a, &a[21912], &a[43824], &a[
		    65736], &a[87648], b, &b[2112], &b[4224], &b[6336], work, 
		    rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___113);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "RQ")) {

/*        RQ:  RQ factorization */

	ntypes = 8;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkrq_(dotype, &nm, mval, &nn, nval, &nnb, nbval, nxval, &nrhs, &
		    thresh, &tsterr, &c__132, a, &a[21912], &a[43824], &a[
		    65736], &a[87648], b, &b[2112], &b[4224], &b[6336], work, 
		    rwork, iwork, &c__6);
	} else {
	    s_wsfe(&io___114);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "QP")) {

/*        QP:  QR factorization with pivoting */

	ntypes = 6;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchkqp_(dotype, &nm, mval, &nn, nval, &thresh, &tsterr, a, &a[
		    21912], b, &b[2112], &b[4224], work, iwork, &c__6);
	    dchkq3_(dotype, &nm, mval, &nn, nval, &nnb, nbval, nxval, &thresh, 
		     a, &a[21912], b, &b[2112], &b[4224], work, iwork, &c__6);
	} else {
	    s_wsfe(&io___115);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "TZ")) {

/*        TZ:  Trapezoidal matrix */

	ntypes = 3;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstchk) {
	    dchktz_(dotype, &nm, mval, &nn, nval, &thresh, &tsterr, a, &a[
		    21912], b, &b[2112], &b[4224], work, &c__6);
	} else {
	    s_wsfe(&io___116);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "LS")) {

/*        LS:  Least squares drivers */

	ntypes = 6;
	alareq_(path, &nmats, dotype, &ntypes, &c__5, &c__6);

	if (tstdrv) {
	    ddrvls_(dotype, &nm, mval, &nn, nval, &nns, nsval, &nnb, nbval, 
		    nxval, &thresh, &tsterr, a, &a[21912], b, &b[2112], &b[
		    4224], rwork, &rwork[132], work, iwork, &c__6);
	} else {
	    s_wsfe(&io___117);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "EQ")) {

/*        EQ:  Equilibration routines for general and positive definite */
/*             matrices (THREQ should be between 2 and 10) */

	if (tstchk) {
	    dchkeq_(&threq, &c__6);
	} else {
	    s_wsfe(&io___118);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}

    } else {

	s_wsfe(&io___119);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }

/*     Go back to get another input line. */

    goto L80;

/*     Branch to this line when the last record is read. */

L140:
    cl__1.cerr = 0;
    cl__1.cunit = 5;
    cl__1.csta = 0;
    f_clos(&cl__1);
    s2 = dsecnd_();
    s_wsfe(&io___121);
    e_wsfe();
    s_wsfe(&io___122);
    d__1 = s2 - s1;
    do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
    e_wsfe();


/*     End of DCHKAA */

    return 0;
} /* MAIN__ */

/* Main program alias */ int dchkaa_ () { MAIN__ (); return 0; }
