#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer nproc, nshift, maxb;
} cenvir_;

#define cenvir_1 cenvir_

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
    integer selopt, seldim;
    logical selval[20];
    doublereal selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

struct {
    integer iparms[100];
} zlaenv_;

#define zlaenv_1 zlaenv_

/* Table of constant values */

static integer c__1 = 1;
static integer c__3 = 3;
static integer c__5 = 5;
static integer c__6 = 6;
static integer c__12 = 12;
static integer c__11 = 11;
static integer c__13 = 13;
static integer c__2 = 2;
static integer c__14 = 14;
static integer c__0 = 0;
static integer c__15 = 15;
static integer c__16 = 16;
static integer c__20 = 20;
static integer c__132 = 132;
static integer c__4 = 4;
static integer c__8 = 8;
static integer c__87781 = 87781;
static integer c__9 = 9;
static integer c__25 = 25;
static integer c__89760 = 89760;
static integer c__18 = 18;
static integer c__400 = 400;
static integer c__89758 = 89758;
static integer c__264 = 264;

/* Main program */ int MAIN__(void)
{
    /* Initialized data */

    static char intstr[10] = "0123456789";
    static integer ioldsd[4] = { 0,0,0,1 };

    /* Format strings */
    static char fmt_9987[] = "(\002 Tests of the Nonsymmetric Eigenvalue Pro"
	    "blem routines\002)";
    static char fmt_9986[] = "(\002 Tests of the Symmetric Eigenvalue Proble"
	    "m routines\002)";
    static char fmt_9985[] = "(\002 Tests of the Singular Value Decompositio"
	    "n routines\002)";
    static char fmt_9979[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Driver\002,/\002    DGEEV (eigenvalues and eigevectors)"
	    "\002)";
    static char fmt_9978[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Driver\002,/\002    DGEES (Schur form)\002)";
    static char fmt_9977[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Expert\002,\002 Driver\002,/\002    DGEEVX (eigenvalues, e"
	    "igenvectors and\002,\002 condition numbers)\002)";
    static char fmt_9976[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Expert\002,\002 Driver\002,/\002    DGEESX (Schur form and"
	    " condition\002,\002 numbers)\002)";
    static char fmt_9975[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem routines\002)";
    static char fmt_9964[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Driver DGGES\002)";
    static char fmt_9965[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Expert Driver DGGESX\002)";
    static char fmt_9963[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Driver DGGEV\002)";
    static char fmt_9962[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Expert Driver DGGEVX\002)";
    static char fmt_9974[] = "(\002 Tests of DSBTRD\002,/\002 (reduction of "
	    "a symmetric band \002,\002matrix to tridiagonal form)\002)";
    static char fmt_9967[] = "(\002 Tests of DGBBRD\002,/\002 (reduction of "
	    "a general band \002,\002matrix to real bidiagonal form)\002)";
    static char fmt_9971[] = "(/\002 Tests of the Generalized Linear Regress"
	    "ion Model \002,\002routines\002)";
    static char fmt_9970[] = "(/\002 Tests of the Generalized QR and RQ rout"
	    "ines\002)";
    static char fmt_9969[] = "(/\002 Tests of the Generalized Singular Valu"
	    "e\002,\002 Decomposition routines\002)";
    static char fmt_9968[] = "(/\002 Tests of the Linear Least Squares routi"
	    "nes\002)";
    static char fmt_9992[] = "(1x,a3,\002:  Unrecognized path name\002)";
    static char fmt_9972[] = "(/\002 LAPACK VERSION \002,i1,\002.\002,i1,"
	    "\002.\002,i1)";
    static char fmt_9984[] = "(/\002 The following parameter values will be "
	    "used:\002)";
    static char fmt_9989[] = "(\002 Invalid input value: \002,a6,\002=\002,i"
	    "6,\002; must be >=\002,i6)";
    static char fmt_9988[] = "(\002 Invalid input value: \002,a6,\002=\002,i"
	    "6,\002; must be <=\002,i6)";
    static char fmt_9983[] = "(4x,a6,10i6,/10x,10i6)";
    static char fmt_9981[] = "(\002 Relative machine \002,a,\002 is taken to"
	    " be\002,d16.6)";
    static char fmt_9982[] = "(/\002 Routines pass computational tests if te"
	    "st ratio is \002,\002less than\002,f8.2,/)";
    static char fmt_9999[] = "(/\002 Execution not attempted due to input er"
	    "rors\002)";
    static char fmt_9991[] = "(//\002 *** Invalid integer value in column"
	    " \002,i2,\002 of input\002,\002 line:\002,/a79)";
    static char fmt_9990[] = "(//1x,a3,\002 routines were not tested\002)";
    static char fmt_9961[] = "(//1x,a3,\002:  NB =\002,i4,\002, NBMIN =\002,"
	    "i4,\002, NX =\002,i4,\002, INMIN=\002,i4,\002, INWIN =\002,i4"
	    ",\002, INIBL =\002,i4,\002, ISHFTS =\002,i4,\002, IACC22 =\002,i"
	    "4)";
    static char fmt_9980[] = "(\002 *** Error code from \002,a6,\002 = \002,"
	    "i4)";
    static char fmt_9997[] = "(//1x,a3,\002:  NB =\002,i4,\002, NBMIN =\002,"
	    "i4,\002, NX =\002,i4)";
    static char fmt_9995[] = "(//1x,a3,\002:  NB =\002,i4,\002, NBMIN =\002,"
	    "i4,\002, NX =\002,i4,\002, NRHS =\002,i4)";
    static char fmt_9973[] = "(/1x,71(\002-\002))";
    static char fmt_9996[] = "(//1x,a3,\002:  NB =\002,i4,\002, NBMIN =\002,"
	    "i4,\002, NS =\002,i4,\002, MAXB =\002,i4,\002, NBCOL =\002,i4)";
    static char fmt_9966[] = "(//1x,a3,\002:  NRHS =\002,i4)";
    static char fmt_9994[] = "(//\002 End of tests\002)";
    static char fmt_9993[] = "(\002 Total time used = \002,f12.2,\002 seco"
	    "nds\002,/)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublereal d__1;
    cilist ci__1;

    /* Builtin functions */
    integer s_rsfe(cilist *), do_fio(integer *, char *, ftnlen), e_rsfe(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen), s_wsfe(cilist *), e_wsfe(
	    void), s_rsle(cilist *), do_lio(integer *, integer *, char *, 
	    ftnlen), e_rsle(void), s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer i_len(char *, ftnlen);

    /* Local variables */
    doublereal a[243936]	/* was [17424][14] */, b[87120]	/* was [17424]
	    [5] */, c__[160000]	/* was [400][400] */, d__[1584]	/* was [132][
	    12] */;
    integer i__, k;
    doublereal x[660];
    char c1[1], c3[3];
    integer i1;
    doublereal s1, s2;
    integer ic, nk, nn, vers_patch__, vers_major__, vers_minor__;
    logical dbb, dbk, dgg, dbl, dgk, dgl, dsb, des, dgs, dev, glm, dgv, nep, 
	    lse, dgx, sep;
    doublereal eps;
    logical gqr, svd, dsx, gsv, dvx, dxv;
    char line[80];
    doublereal taua[132];
    integer info;
    char path[3];
    integer kval[20], lenp, mval[20], nval[20];
    doublereal taub[132];
    integer pval[20], itmp, nrhs;
    doublereal work[87781];
    integer iacc22[20];
    logical fatal;
    integer iseed[4], nbcol[20], inibl[20], nbval[20], nbmin[20];
    char vname[6];
    integer inmin[20], newsd, nsval[20], inwin[20], nxval[20], iwork[89760];
    extern /* Subroutine */ int dchkbb_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *), dchkbd_(
	    integer *, integer *, integer *, integer *, logical *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, doublereal *, doublereal *, integer *, doublereal *
, integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    integer *, integer *, integer *), dchkec_(doublereal *, logical *, 
	     integer *, integer *), dchkbk_(integer *, integer *), dchkbl_(
	    integer *, integer *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int dchkgg_(integer *, integer *, integer *, 
	    logical *, integer *, doublereal *, logical *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, logical *, doublereal *, 
	    integer *), dchkgk_(integer *, integer *), dchkgl_(integer *, 
	    integer *), dchksb_(integer *, integer *, integer *, integer *, 
	    integer *, logical *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, doublereal *, integer *, doublereal *, integer *);
    extern doublereal dsecnd_(void);
    extern /* Subroutine */ int dckglm_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    integer *), derrbd_(char *, integer *), dchkhs_(integer *, 
	     integer *, integer *, logical *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    logical *, doublereal *, integer *), alareq_(char *, integer *, 
	    logical *, integer *, integer *, integer *), dcklse_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, integer *
, integer *, integer *), ddrvbd_(integer *, integer *, integer *, 
	    integer *, logical *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    integer *, integer *), ddrges_(integer *, integer *, integer *, 
	    logical *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
, doublereal *, integer *, doublereal *, logical *, integer *), 
	    derred_(char *, integer *), derrgg_(char *, integer *), dckgqr_(integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, doublereal *, integer 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, integer *), ddrgev_(integer *, 
	     integer *, integer *, logical *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
, doublereal *, doublereal *, doublereal *, integer *, doublereal 
	    *, integer *), ddrvgg_(integer *, integer *, integer *, logical *, 
	     integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, integer *, doublereal *, doublereal *
, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int dchkst_(integer *, integer *, integer *, 
	    logical *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, integer *, integer *, integer *, 
	    doublereal *, integer *), dckgsv_(integer *, integer *, integer *, 
	     integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *, integer *, integer *), ilaver_(integer *, integer *, 
	    integer *), ddrves_(integer *, integer *, integer *, logical *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     doublereal *, integer *, integer *, logical *, integer *), 
	    derrhs_(char *, integer *);
    integer mxbval[20];
    extern /* Subroutine */ int ddrvev_(integer *, integer *, integer *, 
	    logical *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, integer *), ddrgsx_(integer *, integer *, doublereal *, 
	     integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, doublereal *, integer *, integer *, integer *, 
	    logical *, integer *), ddrvsg_(integer *, integer *, integer *, 
	    logical *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, integer *, integer *, integer *, doublereal *, 
	    integer *);
    logical tstdif;
    doublereal thresh;
    extern /* Subroutine */ int ddrgvx_(integer *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *, integer *, 
	    doublereal *, logical *, integer *);
    logical tstchk;
    integer nparms, ishfts[20];
    extern /* Subroutine */ int derrst_(char *, integer *);
    logical dotype[30], logwrk[132];
    doublereal thrshn;
    extern /* Subroutine */ int ddrvst_(integer *, integer *, integer *, 
	    logical *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, integer *, integer *, integer *, doublereal *, 
	    integer *), xlaenv_(integer *, integer *), ddrvsx_(integer *, 
	    integer *, integer *, logical *, integer *, doublereal *, integer 
	    *, integer *, doublereal *, integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     doublereal *, doublereal *, integer *, integer *, logical *, 
	    integer *), ddrvvx_(integer *, integer *, integer *, logical *, 
	    integer *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    integer *);
    doublereal result[500];
    integer maxtyp;
    logical tsterr;
    integer ntypes;
    logical tstdrv;

    /* Fortran I/O blocks */
    static cilist io___29 = { 0, 6, 0, fmt_9987, 0 };
    static cilist io___30 = { 0, 6, 0, fmt_9986, 0 };
    static cilist io___31 = { 0, 6, 0, fmt_9985, 0 };
    static cilist io___32 = { 0, 6, 0, fmt_9979, 0 };
    static cilist io___33 = { 0, 6, 0, fmt_9978, 0 };
    static cilist io___34 = { 0, 6, 0, fmt_9977, 0 };
    static cilist io___35 = { 0, 6, 0, fmt_9976, 0 };
    static cilist io___36 = { 0, 6, 0, fmt_9975, 0 };
    static cilist io___37 = { 0, 6, 0, fmt_9964, 0 };
    static cilist io___38 = { 0, 6, 0, fmt_9965, 0 };
    static cilist io___39 = { 0, 6, 0, fmt_9963, 0 };
    static cilist io___40 = { 0, 6, 0, fmt_9962, 0 };
    static cilist io___41 = { 0, 6, 0, fmt_9974, 0 };
    static cilist io___42 = { 0, 6, 0, fmt_9967, 0 };
    static cilist io___43 = { 0, 6, 0, fmt_9971, 0 };
    static cilist io___44 = { 0, 6, 0, fmt_9970, 0 };
    static cilist io___45 = { 0, 6, 0, fmt_9969, 0 };
    static cilist io___46 = { 0, 6, 0, fmt_9968, 0 };
    static cilist io___47 = { 0, 5, 0, 0, 0 };
    static cilist io___50 = { 0, 6, 0, fmt_9992, 0 };
    static cilist io___54 = { 0, 6, 0, fmt_9972, 0 };
    static cilist io___55 = { 0, 6, 0, fmt_9984, 0 };
    static cilist io___56 = { 0, 5, 0, 0, 0 };
    static cilist io___58 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___59 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___60 = { 0, 5, 0, 0, 0 };
    static cilist io___64 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___65 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___66 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___67 = { 0, 5, 0, 0, 0 };
    static cilist io___69 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___70 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___71 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___72 = { 0, 5, 0, 0, 0 };
    static cilist io___74 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___75 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___76 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___77 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___78 = { 0, 5, 0, 0, 0 };
    static cilist io___80 = { 0, 5, 0, 0, 0 };
    static cilist io___82 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___83 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___84 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___85 = { 0, 5, 0, 0, 0 };
    static cilist io___94 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___95 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___96 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___97 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___98 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___99 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___100 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___101 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___102 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___103 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___104 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___105 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___106 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___107 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___108 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___109 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___110 = { 0, 5, 0, 0, 0 };
    static cilist io___113 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___114 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___115 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___116 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___117 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___118 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___119 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___120 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___121 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___122 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___123 = { 0, 5, 0, 0, 0 };
    static cilist io___125 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___126 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___127 = { 0, 5, 0, 0, 0 };
    static cilist io___128 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___129 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___130 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___131 = { 0, 5, 0, 0, 0 };
    static cilist io___132 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___133 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___134 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___135 = { 0, 5, 0, 0, 0 };
    static cilist io___136 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___137 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___138 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___139 = { 0, 5, 0, 0, 0 };
    static cilist io___140 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___141 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___142 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___143 = { 0, 5, 0, 0, 0 };
    static cilist io___144 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___145 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___146 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___147 = { 0, 5, 0, 0, 0 };
    static cilist io___148 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___149 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___150 = { 0, 5, 0, 0, 0 };
    static cilist io___151 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___152 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___153 = { 0, 5, 0, 0, 0 };
    static cilist io___154 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___155 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___156 = { 0, 5, 0, 0, 0 };
    static cilist io___157 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___158 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___159 = { 0, 5, 0, 0, 0 };
    static cilist io___160 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___161 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___162 = { 0, 5, 0, 0, 0 };
    static cilist io___164 = { 0, 6, 0, fmt_9989, 0 };
    static cilist io___165 = { 0, 6, 0, fmt_9988, 0 };
    static cilist io___166 = { 0, 6, 0, fmt_9983, 0 };
    static cilist io___167 = { 0, 6, 0, 0, 0 };
    static cilist io___169 = { 0, 6, 0, fmt_9981, 0 };
    static cilist io___170 = { 0, 6, 0, fmt_9981, 0 };
    static cilist io___171 = { 0, 6, 0, fmt_9981, 0 };
    static cilist io___172 = { 0, 5, 0, 0, 0 };
    static cilist io___173 = { 0, 6, 0, fmt_9982, 0 };
    static cilist io___174 = { 0, 5, 0, 0, 0 };
    static cilist io___176 = { 0, 5, 0, 0, 0 };
    static cilist io___178 = { 0, 5, 0, 0, 0 };
    static cilist io___179 = { 0, 5, 0, 0, 0 };
    static cilist io___181 = { 0, 5, 0, 0, 0 };
    static cilist io___183 = { 0, 6, 0, fmt_9999, 0 };
    static cilist io___192 = { 0, 6, 0, fmt_9991, 0 };
    static cilist io___193 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___196 = { 0, 6, 0, fmt_9961, 0 };
    static cilist io___204 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___205 = { 0, 6, 0, fmt_9997, 0 };
    static cilist io___206 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___207 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___208 = { 0, 6, 0, fmt_9997, 0 };
    static cilist io___209 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___211 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___212 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___213 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___214 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___215 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___216 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___217 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___218 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___219 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___220 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___221 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___222 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___223 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___224 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___225 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___228 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___229 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___230 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___231 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___232 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___233 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___235 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___236 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___237 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___238 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___239 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___240 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___241 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___242 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___243 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___244 = { 0, 6, 0, fmt_9966, 0 };
    static cilist io___245 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___248 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___251 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___252 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___253 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___254 = { 0, 6, 0, 0, 0 };
    static cilist io___255 = { 0, 6, 0, 0, 0 };
    static cilist io___256 = { 0, 6, 0, fmt_9992, 0 };
    static cilist io___257 = { 0, 6, 0, fmt_9994, 0 };
    static cilist io___259 = { 0, 6, 0, fmt_9993, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*  Purpose */
/*  ======= */

/*  DCHKEE tests the DOUBLE PRECISION LAPACK subroutines for the matrix */
/*  eigenvalue problem.  The test paths in this version are */

/*  NEP (Nonsymmetric Eigenvalue Problem): */
/*      Test DGEHRD, DORGHR, DHSEQR, DTREVC, DHSEIN, and DORMHR */

/*  SEP (Symmetric Eigenvalue Problem): */
/*      Test DSYTRD, DORGTR, DSTEQR, DSTERF, DSTEIN, DSTEDC, */
/*      and drivers DSYEV(X), DSBEV(X), DSPEV(X), DSTEV(X), */
/*                  DSYEVD,   DSBEVD,   DSPEVD,   DSTEVD */

/*  SVD (Singular Value Decomposition): */
/*      Test DGEBRD, DORGBR, DBDSQR, DBDSDC */
/*      and the drivers DGESVD, DGESDD */

/*  DEV (Nonsymmetric Eigenvalue/eigenvector Driver): */
/*      Test DGEEV */

/*  DES (Nonsymmetric Schur form Driver): */
/*      Test DGEES */

/*  DVX (Nonsymmetric Eigenvalue/eigenvector Expert Driver): */
/*      Test DGEEVX */

/*  DSX (Nonsymmetric Schur form Expert Driver): */
/*      Test DGEESX */

/*  DGG (Generalized Nonsymmetric Eigenvalue Problem): */
/*      Test DGGHRD, DGGBAL, DGGBAK, DHGEQZ, and DTGEVC */
/*      and the driver routines DGEGS and DGEGV */

/*  DGS (Generalized Nonsymmetric Schur form Driver): */
/*      Test DGGES */

/*  DGV (Generalized Nonsymmetric Eigenvalue/eigenvector Driver): */
/*      Test DGGEV */

/*  DGX (Generalized Nonsymmetric Schur form Expert Driver): */
/*      Test DGGESX */

/*  DXV (Generalized Nonsymmetric Eigenvalue/eigenvector Expert Driver): */
/*      Test DGGEVX */

/*  DSG (Symmetric Generalized Eigenvalue Problem): */
/*      Test DSYGST, DSYGV, DSYGVD, DSYGVX, DSPGST, DSPGV, DSPGVD, */
/*      DSPGVX, DSBGST, DSBGV, DSBGVD, and DSBGVX */

/*  DSB (Symmetric Band Eigenvalue Problem): */
/*      Test DSBTRD */

/*  DBB (Band Singular Value Decomposition): */
/*      Test DGBBRD */

/*  DEC (Eigencondition estimation): */
/*      Test DLALN2, DLASY2, DLAEQU, DLAEXC, DTRSYL, DTREXC, DTRSNA, */
/*      DTRSEN, and DLAQTR */

/*  DBL (Balancing a general matrix) */
/*      Test DGEBAL */

/*  DBK (Back transformation on a balanced matrix) */
/*      Test DGEBAK */

/*  DGL (Balancing a matrix pair) */
/*      Test DGGBAL */

/*  DGK (Back transformation on a matrix pair) */
/*      Test DGGBAK */

/*  GLM (Generalized Linear Regression Model): */
/*      Tests DGGGLM */

/*  GQR (Generalized QR and RQ factorizations): */
/*      Tests DGGQRF and DGGRQF */

/*  GSV (Generalized Singular Value Decomposition): */
/*      Tests DGGSVD, DGGSVP, DTGSJA, DLAGS2, DLAPLL, and DLAPMT */

/*  LSE (Constrained Linear Least Squares): */
/*      Tests DGGLSE */

/*  Each test path has a different set of inputs, but the data sets for */
/*  the driver routines xEV, xES, xVX, and xSX can be concatenated in a */
/*  single input file.  The first line of input should contain one of the */
/*  3-character path names in columns 1-3.  The number of remaining lines */
/*  depends on what is found on the first line. */

/*  The number of matrix types used in testing is often controllable from */
/*  the input file.  The number of matrix types for each path, and the */
/*  test routine that describes them, is as follows: */

/*  Path name(s)  Types    Test routine */

/*  DHS or NEP      21     DCHKHS */
/*  DST or SEP      21     DCHKST (routines) */
/*                  18     DDRVST (drivers) */
/*  DBD or SVD      16     DCHKBD (routines) */
/*                   5     DDRVBD (drivers) */
/*  DEV             21     DDRVEV */
/*  DES             21     DDRVES */
/*  DVX             21     DDRVVX */
/*  DSX             21     DDRVSX */
/*  DGG             26     DCHKGG (routines) */
/*                  26     DDRVGG (drivers) */
/*  DGS             26     DDRGES */
/*  DGX              5     DDRGSX */
/*  DGV             26     DDRGEV */
/*  DXV              2     DDRGVX */
/*  DSG             21     DDRVSG */
/*  DSB             15     DCHKSB */
/*  DBB             15     DCHKBB */
/*  DEC              -     DCHKEC */
/*  DBL              -     DCHKBL */
/*  DBK              -     DCHKBK */
/*  DGL              -     DCHKGL */
/*  DGK              -     DCHKGK */
/*  GLM              8     DCKGLM */
/*  GQR              8     DCKGQR */
/*  GSV              8     DCKGSV */
/*  LSE              8     DCKLSE */

/* ----------------------------------------------------------------------- */

/*  NEP input file: */

/*  line 2:  NN, INTEGER */
/*           Number of values of N. */

/*  line 3:  NVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix dimension N. */

/*  line 4:  NPARMS, INTEGER */
/*           Number of values of the parameters NB, NBMIN, NX, NS, and */
/*           MAXB. */

/*  line 5:  NBVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the blocksize NB. */

/*  line 6:  NBMIN, INTEGER array, dimension (NPARMS) */
/*           The values for the minimum blocksize NBMIN. */

/*  line 7:  NXVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the crossover point NX. */

/*  line 8:  INMIN, INTEGER array, dimension (NPARMS) */
/*           LAHQR vs TTQRE crossover point, >= 11 */

/*  line 9:  INWIN, INTEGER array, dimension (NPARMS) */
/*           recommended deflation window size */

/*  line 10: INIBL, INTEGER array, dimension (NPARMS) */
/*           nibble crossover point */

/*  line 11: ISHFTS, INTEGER array, dimension (NPARMS) */
/*           number of simultaneous shifts) */

/*  line 12: IACC22, INTEGER array, dimension (NPARMS) */
/*           select structured matrix multiply: 0, 1 or 2) */

/*  line 13: THRESH */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold.  To have all of the test */
/*           ratios printed, use THRESH = 0.0 . */

/*  line 14: NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 14 was 2: */

/*  line 15: INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 15-EOF:  The remaining lines occur in sets of 1 or 2 and allow */
/*           the user to specify the matrix types.  Each line contains */
/*           a 3-character path name in columns 1-3, and the number */
/*           of matrix types must be the first nonblank item in columns */
/*           4-80.  If the number of matrix types is at least 1 but is */
/*           less than the maximum number of possible types, a second */
/*           line will be read to get the numbers of the matrix types to */
/*           be used.  For example, */
/*  NEP 21 */
/*           requests all of the matrix types for the nonsymmetric */
/*           eigenvalue problem, while */
/*  NEP  4 */
/*  9 10 11 12 */
/*           requests only matrices of type 9, 10, 11, and 12. */

/*           The valid 3-character path names are 'NEP' or 'SHS' for the */
/*           nonsymmetric eigenvalue routines. */

/* ----------------------------------------------------------------------- */

/*  SEP or DSG input file: */

/*  line 2:  NN, INTEGER */
/*           Number of values of N. */

/*  line 3:  NVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix dimension N. */

/*  line 4:  NPARMS, INTEGER */
/*           Number of values of the parameters NB, NBMIN, and NX. */

/*  line 5:  NBVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the blocksize NB. */

/*  line 6:  NBMIN, INTEGER array, dimension (NPARMS) */
/*           The values for the minimum blocksize NBMIN. */

/*  line 7:  NXVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the crossover point NX. */

/*  line 8:  THRESH */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 9:  TSTCHK, LOGICAL */
/*           Flag indicating whether or not to test the LAPACK routines. */

/*  line 10: TSTDRV, LOGICAL */
/*           Flag indicating whether or not to test the driver routines. */

/*  line 11: TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 12: NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 12 was 2: */

/*  line 13: INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 13-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path names are 'SEP' or 'SST' for the */
/*           symmetric eigenvalue routines and driver routines, and */
/*           'DSG' for the routines for the symmetric generalized */
/*           eigenvalue problem. */

/* ----------------------------------------------------------------------- */

/*  SVD input file: */

/*  line 2:  NN, INTEGER */
/*           Number of values of M and N. */

/*  line 3:  MVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix row dimension M. */

/*  line 4:  NVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix column dimension N. */

/*  line 5:  NPARMS, INTEGER */
/*           Number of values of the parameter NB, NBMIN, NX, and NRHS. */

/*  line 6:  NBVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the blocksize NB. */

/*  line 7:  NBMIN, INTEGER array, dimension (NPARMS) */
/*           The values for the minimum blocksize NBMIN. */

/*  line 8:  NXVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the crossover point NX. */

/*  line 9:  NSVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the number of right hand sides NRHS. */

/*  line 10: THRESH */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 11: TSTCHK, LOGICAL */
/*           Flag indicating whether or not to test the LAPACK routines. */

/*  line 12: TSTDRV, LOGICAL */
/*           Flag indicating whether or not to test the driver routines. */

/*  line 13: TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 14: NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 14 was 2: */

/*  line 15: INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 15-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path names are 'SVD' or 'SBD' for both the */
/*           SVD routines and the SVD driver routines. */

/* ----------------------------------------------------------------------- */

/*  DEV and DES data files: */

/*  line 1:  'DEV' or 'DES' in columns 1 to 3. */

/*  line 2:  NSIZES, INTEGER */
/*           Number of sizes of matrices to use. Should be at least 0 */
/*           and at most 20. If NSIZES = 0, no testing is done */
/*           (although the remaining  3 lines are still read). */

/*  line 3:  NN, INTEGER array, dimension(NSIZES) */
/*           Dimensions of matrices to be tested. */

/*  line 4:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */
/*           These integer parameters determine how blocking is done */
/*           (see ILAENV for details) */
/*           NB     : block size */
/*           NBMIN  : minimum block size */
/*           NX     : minimum dimension for blocking */
/*           NS     : number of shifts in xHSEQR */
/*           NBCOL  : minimum column dimension for blocking */

/*  line 5:  THRESH, REAL */
/*           The test threshold against which computed residuals are */
/*           compared. Should generally be in the range from 10. to 20. */
/*           If it is 0., all test case data will be printed. */

/*  line 6:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits. */

/*  line 7:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 7 was 2: */

/*  line 8:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 9 and following:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'DEV' to test SGEEV, or */
/*           'DES' to test SGEES. */

/* ----------------------------------------------------------------------- */

/*  The DVX data has two parts. The first part is identical to DEV, */
/*  and the second part consists of test matrices with precomputed */
/*  solutions. */

/*  line 1:  'DVX' in columns 1-3. */

/*  line 2:  NSIZES, INTEGER */
/*           If NSIZES = 0, no testing of randomly generated examples */
/*           is done, but any precomputed examples are tested. */

/*  line 3:  NN, INTEGER array, dimension(NSIZES) */

/*  line 4:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */

/*  line 5:  THRESH, REAL */

/*  line 6:  TSTERR, LOGICAL */

/*  line 7:  NEWSD, INTEGER */

/*  If line 7 was 2: */

/*  line 8:  INTEGER array, dimension (4) */

/*  lines 9 and following: The first line contains 'DVX' in columns 1-3 */
/*           followed by the number of matrix types, possibly with */
/*           a second line to specify certain matrix types. */
/*           If the number of matrix types = 0, no testing of randomly */
/*           generated examples is done, but any precomputed examples */
/*           are tested. */

/*  remaining lines : Each matrix is stored on 1+2*N lines, where N is */
/*           its dimension. The first line contains the dimension (a */
/*           single integer). The next N lines contain the matrix, one */
/*           row per line. The last N lines correspond to each */
/*           eigenvalue. Each of these last N lines contains 4 real */
/*           values: the real part of the eigenvalue, the imaginary */
/*           part of the eigenvalue, the reciprocal condition number of */
/*           the eigenvalues, and the reciprocal condition number of the */
/*           eigenvector.  The end of data is indicated by dimension N=0. */
/*           Even if no data is to be tested, there must be at least one */
/*           line containing N=0. */

/* ----------------------------------------------------------------------- */

/*  The DSX data is like DVX. The first part is identical to DEV, and the */
/*  second part consists of test matrices with precomputed solutions. */

/*  line 1:  'DSX' in columns 1-3. */

/*  line 2:  NSIZES, INTEGER */
/*           If NSIZES = 0, no testing of randomly generated examples */
/*           is done, but any precomputed examples are tested. */

/*  line 3:  NN, INTEGER array, dimension(NSIZES) */

/*  line 4:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */

/*  line 5:  THRESH, REAL */

/*  line 6:  TSTERR, LOGICAL */

/*  line 7:  NEWSD, INTEGER */

/*  If line 7 was 2: */

/*  line 8:  INTEGER array, dimension (4) */

/*  lines 9 and following: The first line contains 'DSX' in columns 1-3 */
/*           followed by the number of matrix types, possibly with */
/*           a second line to specify certain matrix types. */
/*           If the number of matrix types = 0, no testing of randomly */
/*           generated examples is done, but any precomputed examples */
/*           are tested. */

/*  remaining lines : Each matrix is stored on 3+N lines, where N is its */
/*           dimension. The first line contains the dimension N and the */
/*           dimension M of an invariant subspace. The second line */
/*           contains M integers, identifying the eigenvalues in the */
/*           invariant subspace (by their position in a list of */
/*           eigenvalues ordered by increasing real part). The next N */
/*           lines contain the matrix. The last line contains the */
/*           reciprocal condition number for the average of the selected */
/*           eigenvalues, and the reciprocal condition number for the */
/*           corresponding right invariant subspace. The end of data is */
/*           indicated by a line containing N=0 and M=0. Even if no data */
/*           is to be tested, there must be at least one line containing */
/*           N=0 and M=0. */

/* ----------------------------------------------------------------------- */

/*  DGG input file: */

/*  line 2:  NN, INTEGER */
/*           Number of values of N. */

/*  line 3:  NVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix dimension N. */

/*  line 4:  NPARMS, INTEGER */
/*           Number of values of the parameters NB, NBMIN, NS, MAXB, and */
/*           NBCOL. */

/*  line 5:  NBVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the blocksize NB. */

/*  line 6:  NBMIN, INTEGER array, dimension (NPARMS) */
/*           The values for NBMIN, the minimum row dimension for blocks. */

/*  line 7:  NSVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the number of shifts. */

/*  line 8:  MXBVAL, INTEGER array, dimension (NPARMS) */
/*           The values for MAXB, used in determining minimum blocksize. */

/*  line 9:  NBCOL, INTEGER array, dimension (NPARMS) */
/*           The values for NBCOL, the minimum column dimension for */
/*           blocks. */

/*  line 10: THRESH */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 11: TSTCHK, LOGICAL */
/*           Flag indicating whether or not to test the LAPACK routines. */

/*  line 12: TSTDRV, LOGICAL */
/*           Flag indicating whether or not to test the driver routines. */

/*  line 13: TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 14: NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 14 was 2: */

/*  line 15: INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 15-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'DGG' for the generalized */
/*           eigenvalue problem routines and driver routines. */

/* ----------------------------------------------------------------------- */

/*  DGS and DGV input files: */

/*  line 1:  'DGS' or 'DGV' in columns 1 to 3. */

/*  line 2:  NN, INTEGER */
/*           Number of values of N. */

/*  line 3:  NVAL, INTEGER array, dimension(NN) */
/*           Dimensions of matrices to be tested. */

/*  line 4:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */
/*           These integer parameters determine how blocking is done */
/*           (see ILAENV for details) */
/*           NB     : block size */
/*           NBMIN  : minimum block size */
/*           NX     : minimum dimension for blocking */
/*           NS     : number of shifts in xHGEQR */
/*           NBCOL  : minimum column dimension for blocking */

/*  line 5:  THRESH, REAL */
/*           The test threshold against which computed residuals are */
/*           compared. Should generally be in the range from 10. to 20. */
/*           If it is 0., all test case data will be printed. */

/*  line 6:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits. */

/*  line 7:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 17 was 2: */

/*  line 7:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 7-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'DGS' for the generalized */
/*           eigenvalue problem routines and driver routines. */

/* ----------------------------------------------------------------------- */

/*  DXV input files: */

/*  line 1:  'DXV' in columns 1 to 3. */

/*  line 2:  N, INTEGER */
/*           Value of N. */

/*  line 3:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */
/*           These integer parameters determine how blocking is done */
/*           (see ILAENV for details) */
/*           NB     : block size */
/*           NBMIN  : minimum block size */
/*           NX     : minimum dimension for blocking */
/*           NS     : number of shifts in xHGEQR */
/*           NBCOL  : minimum column dimension for blocking */

/*  line 4:  THRESH, REAL */
/*           The test threshold against which computed residuals are */
/*           compared. Should generally be in the range from 10. to 20. */
/*           Information will be printed about each test for which the */
/*           test ratio is greater than or equal to the threshold. */

/*  line 5:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 6:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 6 was 2: */

/*  line 7: INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  If line 2 was 0: */

/*  line 7-EOF: Precomputed examples are tested. */

/*  remaining lines : Each example is stored on 3+2*N lines, where N is */
/*           its dimension. The first line contains the dimension (a */
/*           single integer). The next N lines contain the matrix A, one */
/*           row per line. The next N lines contain the matrix B.  The */
/*           next line contains the reciprocals of the eigenvalue */
/*           condition numbers.  The last line contains the reciprocals of */
/*           the eigenvector condition numbers.  The end of data is */
/*           indicated by dimension N=0.  Even if no data is to be tested, */
/*           there must be at least one line containing N=0. */

/* ----------------------------------------------------------------------- */

/*  DGX input files: */

/*  line 1:  'DGX' in columns 1 to 3. */

/*  line 2:  N, INTEGER */
/*           Value of N. */

/*  line 3:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */
/*           These integer parameters determine how blocking is done */
/*           (see ILAENV for details) */
/*           NB     : block size */
/*           NBMIN  : minimum block size */
/*           NX     : minimum dimension for blocking */
/*           NS     : number of shifts in xHGEQR */
/*           NBCOL  : minimum column dimension for blocking */

/*  line 4:  THRESH, REAL */
/*           The test threshold against which computed residuals are */
/*           compared. Should generally be in the range from 10. to 20. */
/*           Information will be printed about each test for which the */
/*           test ratio is greater than or equal to the threshold. */

/*  line 5:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 6:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 6 was 2: */

/*  line 7: INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  If line 2 was 0: */

/*  line 7-EOF: Precomputed examples are tested. */

/*  remaining lines : Each example is stored on 3+2*N lines, where N is */
/*           its dimension. The first line contains the dimension (a */
/*           single integer).  The next line contains an integer k such */
/*           that only the last k eigenvalues will be selected and appear */
/*           in the leading diagonal blocks of $A$ and $B$. The next N */
/*           lines contain the matrix A, one row per line.  The next N */
/*           lines contain the matrix B.  The last line contains the */
/*           reciprocal of the eigenvalue cluster condition number and the */
/*           reciprocal of the deflating subspace (associated with the */
/*           selected eigencluster) condition number.  The end of data is */
/*           indicated by dimension N=0.  Even if no data is to be tested, */
/*           there must be at least one line containing N=0. */

/* ----------------------------------------------------------------------- */

/*  DSB input file: */

/*  line 2:  NN, INTEGER */
/*           Number of values of N. */

/*  line 3:  NVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix dimension N. */

/*  line 4:  NK, INTEGER */
/*           Number of values of K. */

/*  line 5:  KVAL, INTEGER array, dimension (NK) */
/*           The values for the matrix dimension K. */

/*  line 6:  THRESH */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 7:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 7 was 2: */

/*  line 8:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 8-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'DSB'. */

/* ----------------------------------------------------------------------- */

/*  DBB input file: */

/*  line 2:  NN, INTEGER */
/*           Number of values of M and N. */

/*  line 3:  MVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix row dimension M. */

/*  line 4:  NVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix column dimension N. */

/*  line 4:  NK, INTEGER */
/*           Number of values of K. */

/*  line 5:  KVAL, INTEGER array, dimension (NK) */
/*           The values for the matrix bandwidth K. */

/*  line 6:  NPARMS, INTEGER */
/*           Number of values of the parameter NRHS */

/*  line 7:  NSVAL, INTEGER array, dimension (NPARMS) */
/*           The values for the number of right hand sides NRHS. */

/*  line 8:  THRESH */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 9:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 9 was 2: */

/*  line 10: INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 10-EOF:  Lines specifying matrix types, as for SVD. */
/*           The 3-character path name is 'DBB'. */

/* ----------------------------------------------------------------------- */

/*  DEC input file: */

/*  line  2: THRESH, REAL */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  lines  3-EOF: */

/*  Input for testing the eigencondition routines consists of a set of */
/*  specially constructed test cases and their solutions.  The data */
/*  format is not intended to be modified by the user. */

/* ----------------------------------------------------------------------- */

/*  DBL and DBK input files: */

/*  line 1:  'DBL' in columns 1-3 to test SGEBAL, or 'DBK' in */
/*           columns 1-3 to test SGEBAK. */

/*  The remaining lines consist of specially constructed test cases. */

/* ----------------------------------------------------------------------- */

/*  DGL and DGK input files: */

/*  line 1:  'DGL' in columns 1-3 to test DGGBAL, or 'DGK' in */
/*           columns 1-3 to test DGGBAK. */

/*  The remaining lines consist of specially constructed test cases. */

/* ----------------------------------------------------------------------- */

/*  GLM data file: */

/*  line 1:  'GLM' in columns 1 to 3. */

/*  line 2:  NN, INTEGER */
/*           Number of values of M, P, and N. */

/*  line 3:  MVAL, INTEGER array, dimension(NN) */
/*           Values of M (row dimension). */

/*  line 4:  PVAL, INTEGER array, dimension(NN) */
/*           Values of P (row dimension). */

/*  line 5:  NVAL, INTEGER array, dimension(NN) */
/*           Values of N (column dimension), note M <= N <= M+P. */

/*  line 6:  THRESH, REAL */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 7:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 8:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 8 was 2: */

/*  line 9:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 9-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'GLM' for the generalized */
/*           linear regression model routines. */

/* ----------------------------------------------------------------------- */

/*  GQR data file: */

/*  line 1:  'GQR' in columns 1 to 3. */

/*  line 2:  NN, INTEGER */
/*           Number of values of M, P, and N. */

/*  line 3:  MVAL, INTEGER array, dimension(NN) */
/*           Values of M. */

/*  line 4:  PVAL, INTEGER array, dimension(NN) */
/*           Values of P. */

/*  line 5:  NVAL, INTEGER array, dimension(NN) */
/*           Values of N. */

/*  line 6:  THRESH, REAL */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 7:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 8:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 8 was 2: */

/*  line 9:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 9-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'GQR' for the generalized */
/*           QR and RQ routines. */

/* ----------------------------------------------------------------------- */

/*  GSV data file: */

/*  line 1:  'GSV' in columns 1 to 3. */

/*  line 2:  NN, INTEGER */
/*           Number of values of M, P, and N. */

/*  line 3:  MVAL, INTEGER array, dimension(NN) */
/*           Values of M (row dimension). */

/*  line 4:  PVAL, INTEGER array, dimension(NN) */
/*           Values of P (row dimension). */

/*  line 5:  NVAL, INTEGER array, dimension(NN) */
/*           Values of N (column dimension). */

/*  line 6:  THRESH, REAL */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 7:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 8:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 8 was 2: */

/*  line 9:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 9-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'GSV' for the generalized */
/*           SVD routines. */

/* ----------------------------------------------------------------------- */

/*  LSE data file: */

/*  line 1:  'LSE' in columns 1 to 3. */

/*  line 2:  NN, INTEGER */
/*           Number of values of M, P, and N. */

/*  line 3:  MVAL, INTEGER array, dimension(NN) */
/*           Values of M. */

/*  line 4:  PVAL, INTEGER array, dimension(NN) */
/*           Values of P. */

/*  line 5:  NVAL, INTEGER array, dimension(NN) */
/*           Values of N, note P <= N <= P+M. */

/*  line 6:  THRESH, REAL */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  line 7:  TSTERR, LOGICAL */
/*           Flag indicating whether or not to test the error exits for */
/*           the LAPACK routines and driver routines. */

/*  line 8:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 8 was 2: */

/*  line 9:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 9-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'GSV' for the generalized */
/*           SVD routines. */

/* ----------------------------------------------------------------------- */

/*  NMAX is currently set to 132 and must be at least 12 for some of the */
/*  precomputed examples, and LWORK = NMAX*(5*NMAX+5)+1 in the parameter */
/*  statements below.  For SVD, we assume NRHS may be as big as N.  The */
/*  parameter NEED is set to 14 to allow for 14 N-by-N matrices for DGG. */

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
/*     .. Arrays in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
/*     .. Executable Statements .. */

    s1 = dsecnd_();
    fatal = FALSE_;
    infoc_1.nunit = 6;

/*     Return to here to read multiple sets of data */

L10:

/*     Read the first line and set the 3-character test path */

    ci__1.cierr = 0;
    ci__1.ciend = 1;
    ci__1.ciunit = 5;
    ci__1.cifmt = "(A80)";
    i__1 = s_rsfe(&ci__1);
    if (i__1 != 0) {
	goto L380;
    }
    i__1 = do_fio(&c__1, line, (ftnlen)80);
    if (i__1 != 0) {
	goto L380;
    }
    i__1 = e_rsfe();
    if (i__1 != 0) {
	goto L380;
    }
    s_copy(path, line, (ftnlen)3, (ftnlen)3);
    nep = lsamen_(&c__3, path, "NEP") || lsamen_(&c__3, 
	    path, "DHS");
    sep = lsamen_(&c__3, path, "SEP") || lsamen_(&c__3, 
	    path, "DST") || lsamen_(&c__3, path, "DSG");
    svd = lsamen_(&c__3, path, "SVD") || lsamen_(&c__3, 
	    path, "DBD");
    dev = lsamen_(&c__3, path, "DEV");
    des = lsamen_(&c__3, path, "DES");
    dvx = lsamen_(&c__3, path, "DVX");
    dsx = lsamen_(&c__3, path, "DSX");
    dgg = lsamen_(&c__3, path, "DGG");
    dgs = lsamen_(&c__3, path, "DGS");
    dgx = lsamen_(&c__3, path, "DGX");
    dgv = lsamen_(&c__3, path, "DGV");
    dxv = lsamen_(&c__3, path, "DXV");
    dsb = lsamen_(&c__3, path, "DSB");
    dbb = lsamen_(&c__3, path, "DBB");
    glm = lsamen_(&c__3, path, "GLM");
    gqr = lsamen_(&c__3, path, "GQR") || lsamen_(&c__3, 
	    path, "GRQ");
    gsv = lsamen_(&c__3, path, "GSV");
    lse = lsamen_(&c__3, path, "LSE");
    dbl = lsamen_(&c__3, path, "DBL");
    dbk = lsamen_(&c__3, path, "DBK");
    dgl = lsamen_(&c__3, path, "DGL");
    dgk = lsamen_(&c__3, path, "DGK");

/*     Report values of parameters. */

    if (s_cmp(path, "   ", (ftnlen)3, (ftnlen)3) == 0) {
	goto L10;
    } else if (nep) {
	s_wsfe(&io___29);
	e_wsfe();
    } else if (sep) {
	s_wsfe(&io___30);
	e_wsfe();
    } else if (svd) {
	s_wsfe(&io___31);
	e_wsfe();
    } else if (dev) {
	s_wsfe(&io___32);
	e_wsfe();
    } else if (des) {
	s_wsfe(&io___33);
	e_wsfe();
    } else if (dvx) {
	s_wsfe(&io___34);
	e_wsfe();
    } else if (dsx) {
	s_wsfe(&io___35);
	e_wsfe();
    } else if (dgg) {
	s_wsfe(&io___36);
	e_wsfe();
    } else if (dgs) {
	s_wsfe(&io___37);
	e_wsfe();
    } else if (dgx) {
	s_wsfe(&io___38);
	e_wsfe();
    } else if (dgv) {
	s_wsfe(&io___39);
	e_wsfe();
    } else if (dxv) {
	s_wsfe(&io___40);
	e_wsfe();
    } else if (dsb) {
	s_wsfe(&io___41);
	e_wsfe();
    } else if (dbb) {
	s_wsfe(&io___42);
	e_wsfe();
    } else if (glm) {
	s_wsfe(&io___43);
	e_wsfe();
    } else if (gqr) {
	s_wsfe(&io___44);
	e_wsfe();
    } else if (gsv) {
	s_wsfe(&io___45);
	e_wsfe();
    } else if (lse) {
	s_wsfe(&io___46);
	e_wsfe();
    } else if (dbl) {

/*        DGEBAL:  Balancing */

	dchkbl_(&c__5, &c__6);
	goto L10;
    } else if (dbk) {

/*        DGEBAK:  Back transformation */

	dchkbk_(&c__5, &c__6);
	goto L10;
    } else if (dgl) {

/*        DGGBAL:  Balancing */

	dchkgl_(&c__5, &c__6);
	goto L10;
    } else if (dgk) {

/*        DGGBAK:  Back transformation */

	dchkgk_(&c__5, &c__6);
	goto L10;
    } else if (lsamen_(&c__3, path, "DEC")) {

/*        DEC:  Eigencondition estimation */

	s_rsle(&io___47);
	do_lio(&c__5, &c__1, (char *)&thresh, (ftnlen)sizeof(doublereal));
	e_rsle();
	xlaenv_(&c__1, &c__1);
	xlaenv_(&c__12, &c__11);
	xlaenv_(&c__13, &c__2);
	xlaenv_(&c__14, &c__0);
	xlaenv_(&c__15, &c__2);
	xlaenv_(&c__16, &c__2);
	tsterr = TRUE_;
	dchkec_(&thresh, &tsterr, &c__5, &c__6);
	goto L10;
    } else {
	s_wsfe(&io___50);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	goto L10;
    }
    ilaver_(&vers_major__, &vers_minor__, &vers_patch__);
    s_wsfe(&io___54);
    do_fio(&c__1, (char *)&vers_major__, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&vers_minor__, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&vers_patch__, (ftnlen)sizeof(integer));
    e_wsfe();
    s_wsfe(&io___55);
    e_wsfe();

/*     Read the number of values of M, P, and N. */

    s_rsle(&io___56);
    do_lio(&c__3, &c__1, (char *)&nn, (ftnlen)sizeof(integer));
    e_rsle();
    if (nn < 0) {
	s_wsfe(&io___58);
	do_fio(&c__1, "   NN ", (ftnlen)6);
	do_fio(&c__1, (char *)&nn, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	nn = 0;
	fatal = TRUE_;
    } else if (nn > 20) {
	s_wsfe(&io___59);
	do_fio(&c__1, "   NN ", (ftnlen)6);
	do_fio(&c__1, (char *)&nn, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&c__20, (ftnlen)sizeof(integer));
	e_wsfe();
	nn = 0;
	fatal = TRUE_;
    }

/*     Read the values of M */

    if (! (dgx || dxv)) {
	s_rsle(&io___60);
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_lio(&c__3, &c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(
		    integer));
	}
	e_rsle();
	if (svd) {
	    s_copy(vname, "    M ", (ftnlen)6, (ftnlen)6);
	} else {
	    s_copy(vname, "    N ", (ftnlen)6, (ftnlen)6);
	}
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (mval[i__ - 1] < 0) {
		s_wsfe(&io___64);
		do_fio(&c__1, vname, (ftnlen)6);
		do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    } else if (mval[i__ - 1] > 132) {
		s_wsfe(&io___65);
		do_fio(&c__1, vname, (ftnlen)6);
		do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    }
/* L20: */
	}
	s_wsfe(&io___66);
	do_fio(&c__1, "M:    ", (ftnlen)6);
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&mval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the values of P */

    if (glm || gqr || gsv || lse) {
	s_rsle(&io___67);
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_lio(&c__3, &c__1, (char *)&pval[i__ - 1], (ftnlen)sizeof(
		    integer));
	}
	e_rsle();
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (pval[i__ - 1] < 0) {
		s_wsfe(&io___69);
		do_fio(&c__1, " P  ", (ftnlen)4);
		do_fio(&c__1, (char *)&pval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    } else if (pval[i__ - 1] > 132) {
		s_wsfe(&io___70);
		do_fio(&c__1, " P  ", (ftnlen)4);
		do_fio(&c__1, (char *)&pval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    }
/* L30: */
	}
	s_wsfe(&io___71);
	do_fio(&c__1, "P:    ", (ftnlen)6);
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&pval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

/*     Read the values of N */

    if (svd || dbb || glm || gqr || gsv || lse) {
	s_rsle(&io___72);
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_lio(&c__3, &c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(
		    integer));
	}
	e_rsle();
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (nval[i__ - 1] < 0) {
		s_wsfe(&io___74);
		do_fio(&c__1, "    N ", (ftnlen)6);
		do_fio(&c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    } else if (nval[i__ - 1] > 132) {
		s_wsfe(&io___75);
		do_fio(&c__1, "    N ", (ftnlen)6);
		do_fio(&c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    }
/* L40: */
	}
    } else {
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nval[i__ - 1] = mval[i__ - 1];
/* L50: */
	}
    }
    if (! (dgx || dxv)) {
	s_wsfe(&io___76);
	do_fio(&c__1, "N:    ", (ftnlen)6);
	i__1 = nn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&nval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    } else {
	s_wsfe(&io___77);
	do_fio(&c__1, "N:    ", (ftnlen)6);
	do_fio(&c__1, (char *)&nn, (ftnlen)sizeof(integer));
	e_wsfe();
    }

/*     Read the number of values of K, followed by the values of K */

    if (dsb || dbb) {
	s_rsle(&io___78);
	do_lio(&c__3, &c__1, (char *)&nk, (ftnlen)sizeof(integer));
	e_rsle();
	s_rsle(&io___80);
	i__1 = nk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_lio(&c__3, &c__1, (char *)&kval[i__ - 1], (ftnlen)sizeof(
		    integer));
	}
	e_rsle();
	i__1 = nk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (kval[i__ - 1] < 0) {
		s_wsfe(&io___82);
		do_fio(&c__1, "    K ", (ftnlen)6);
		do_fio(&c__1, (char *)&kval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    } else if (kval[i__ - 1] > 132) {
		s_wsfe(&io___83);
		do_fio(&c__1, "    K ", (ftnlen)6);
		do_fio(&c__1, (char *)&kval[i__ - 1], (ftnlen)sizeof(integer))
			;
		do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		e_wsfe();
		fatal = TRUE_;
	    }
/* L60: */
	}
	s_wsfe(&io___84);
	do_fio(&c__1, "K:    ", (ftnlen)6);
	i__1 = nk;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&kval[i__ - 1], (ftnlen)sizeof(integer));
	}
	e_wsfe();
    }

    if (dev || des || dvx || dsx) {

/*        For the nonsymmetric QR driver routines, only one set of */
/*        parameters is allowed. */

	s_rsle(&io___85);
	do_lio(&c__3, &c__1, (char *)&nbval[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&nbmin[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&nxval[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&inmin[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&inwin[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&inibl[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&ishfts[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&iacc22[0], (ftnlen)sizeof(integer));
	e_rsle();
	if (nbval[0] < 1) {
	    s_wsfe(&io___94);
	    do_fio(&c__1, "   NB ", (ftnlen)6);
	    do_fio(&c__1, (char *)&nbval[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nbmin[0] < 1) {
	    s_wsfe(&io___95);
	    do_fio(&c__1, "NBMIN ", (ftnlen)6);
	    do_fio(&c__1, (char *)&nbmin[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nxval[0] < 1) {
	    s_wsfe(&io___96);
	    do_fio(&c__1, "   NX ", (ftnlen)6);
	    do_fio(&c__1, (char *)&nxval[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (inmin[0] < 1) {
	    s_wsfe(&io___97);
	    do_fio(&c__1, "   INMIN ", (ftnlen)9);
	    do_fio(&c__1, (char *)&inmin[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (inwin[0] < 1) {
	    s_wsfe(&io___98);
	    do_fio(&c__1, "   INWIN ", (ftnlen)9);
	    do_fio(&c__1, (char *)&inwin[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (inibl[0] < 1) {
	    s_wsfe(&io___99);
	    do_fio(&c__1, "   INIBL ", (ftnlen)9);
	    do_fio(&c__1, (char *)&inibl[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (ishfts[0] < 1) {
	    s_wsfe(&io___100);
	    do_fio(&c__1, "   ISHFTS ", (ftnlen)10);
	    do_fio(&c__1, (char *)&ishfts[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (iacc22[0] < 0) {
	    s_wsfe(&io___101);
	    do_fio(&c__1, "   IACC22 ", (ftnlen)10);
	    do_fio(&c__1, (char *)&iacc22[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
	xlaenv_(&c__1, nbval);
	xlaenv_(&c__2, nbmin);
	xlaenv_(&c__3, nxval);
	i__1 = max(11,inmin[0]);
	xlaenv_(&c__12, &i__1);
	xlaenv_(&c__13, inwin);
	xlaenv_(&c__14, inibl);
	xlaenv_(&c__15, ishfts);
	xlaenv_(&c__16, iacc22);
	s_wsfe(&io___102);
	do_fio(&c__1, "NB:   ", (ftnlen)6);
	do_fio(&c__1, (char *)&nbval[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___103);
	do_fio(&c__1, "NBMIN:", (ftnlen)6);
	do_fio(&c__1, (char *)&nbmin[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___104);
	do_fio(&c__1, "NX:   ", (ftnlen)6);
	do_fio(&c__1, (char *)&nxval[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___105);
	do_fio(&c__1, "INMIN:   ", (ftnlen)9);
	do_fio(&c__1, (char *)&inmin[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___106);
	do_fio(&c__1, "INWIN: ", (ftnlen)7);
	do_fio(&c__1, (char *)&inwin[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___107);
	do_fio(&c__1, "INIBL: ", (ftnlen)7);
	do_fio(&c__1, (char *)&inibl[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___108);
	do_fio(&c__1, "ISHFTS: ", (ftnlen)8);
	do_fio(&c__1, (char *)&ishfts[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___109);
	do_fio(&c__1, "IACC22: ", (ftnlen)8);
	do_fio(&c__1, (char *)&iacc22[0], (ftnlen)sizeof(integer));
	e_wsfe();

    } else if (dgs || dgx || dgv || dxv) {

/*        For the nonsymmetric generalized driver routines, only one set */
/*        of parameters is allowed. */

	s_rsle(&io___110);
	do_lio(&c__3, &c__1, (char *)&nbval[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&nbmin[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&nxval[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&nsval[0], (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&mxbval[0], (ftnlen)sizeof(integer));
	e_rsle();
	if (nbval[0] < 1) {
	    s_wsfe(&io___113);
	    do_fio(&c__1, "   NB ", (ftnlen)6);
	    do_fio(&c__1, (char *)&nbval[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nbmin[0] < 1) {
	    s_wsfe(&io___114);
	    do_fio(&c__1, "NBMIN ", (ftnlen)6);
	    do_fio(&c__1, (char *)&nbmin[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nxval[0] < 1) {
	    s_wsfe(&io___115);
	    do_fio(&c__1, "   NX ", (ftnlen)6);
	    do_fio(&c__1, (char *)&nxval[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (nsval[0] < 2) {
	    s_wsfe(&io___116);
	    do_fio(&c__1, "   NS ", (ftnlen)6);
	    do_fio(&c__1, (char *)&nsval[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	} else if (mxbval[0] < 1) {
	    s_wsfe(&io___117);
	    do_fio(&c__1, " MAXB ", (ftnlen)6);
	    do_fio(&c__1, (char *)&mxbval[0], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    fatal = TRUE_;
	}
	xlaenv_(&c__1, nbval);
	xlaenv_(&c__2, nbmin);
	xlaenv_(&c__3, nxval);
	xlaenv_(&c__4, nsval);
	xlaenv_(&c__8, mxbval);
	s_wsfe(&io___118);
	do_fio(&c__1, "NB:   ", (ftnlen)6);
	do_fio(&c__1, (char *)&nbval[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___119);
	do_fio(&c__1, "NBMIN:", (ftnlen)6);
	do_fio(&c__1, (char *)&nbmin[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___120);
	do_fio(&c__1, "NX:   ", (ftnlen)6);
	do_fio(&c__1, (char *)&nxval[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___121);
	do_fio(&c__1, "NS:   ", (ftnlen)6);
	do_fio(&c__1, (char *)&nsval[0], (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___122);
	do_fio(&c__1, "MAXB: ", (ftnlen)6);
	do_fio(&c__1, (char *)&mxbval[0], (ftnlen)sizeof(integer));
	e_wsfe();

    } else if (! dsb && ! glm && ! gqr && ! gsv && ! lse) {

/*        For the other paths, the number of parameters can be varied */
/*        from the input file.  Read the number of parameter values. */

	s_rsle(&io___123);
	do_lio(&c__3, &c__1, (char *)&nparms, (ftnlen)sizeof(integer));
	e_rsle();
	if (nparms < 1) {
	    s_wsfe(&io___125);
	    do_fio(&c__1, "NPARMS", (ftnlen)6);
	    do_fio(&c__1, (char *)&nparms, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    nparms = 0;
	    fatal = TRUE_;
	} else if (nparms > 20) {
	    s_wsfe(&io___126);
	    do_fio(&c__1, "NPARMS", (ftnlen)6);
	    do_fio(&c__1, (char *)&nparms, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&c__20, (ftnlen)sizeof(integer));
	    e_wsfe();
	    nparms = 0;
	    fatal = TRUE_;
	}

/*        Read the values of NB */

	if (! dbb) {
	    s_rsle(&io___127);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (nbval[i__ - 1] < 0) {
		    s_wsfe(&io___128);
		    do_fio(&c__1, "   NB ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		} else if (nbval[i__ - 1] > 132) {
		    s_wsfe(&io___129);
		    do_fio(&c__1, "   NB ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L70: */
	    }
	    s_wsfe(&io___130);
	    do_fio(&c__1, "NB:   ", (ftnlen)6);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	}

/*        Read the values of NBMIN */

	if (nep || sep || svd || dgg) {
	    s_rsle(&io___131);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (nbmin[i__ - 1] < 0) {
		    s_wsfe(&io___132);
		    do_fio(&c__1, "NBMIN ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		} else if (nbmin[i__ - 1] > 132) {
		    s_wsfe(&io___133);
		    do_fio(&c__1, "NBMIN ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L80: */
	    }
	    s_wsfe(&io___134);
	    do_fio(&c__1, "NBMIN:", (ftnlen)6);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		nbmin[i__ - 1] = 1;
/* L90: */
	    }
	}

/*        Read the values of NX */

	if (nep || sep || svd) {
	    s_rsle(&io___135);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (nxval[i__ - 1] < 0) {
		    s_wsfe(&io___136);
		    do_fio(&c__1, "   NX ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		} else if (nxval[i__ - 1] > 132) {
		    s_wsfe(&io___137);
		    do_fio(&c__1, "   NX ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L100: */
	    }
	    s_wsfe(&io___138);
	    do_fio(&c__1, "NX:   ", (ftnlen)6);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		nxval[i__ - 1] = 1;
/* L110: */
	    }
	}

/*        Read the values of NSHIFT (if DGG) or NRHS (if SVD */
/*        or DBB). */

	if (svd || dbb || dgg) {
	    s_rsle(&io___139);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (nsval[i__ - 1] < 0) {
		    s_wsfe(&io___140);
		    do_fio(&c__1, "   NS ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		} else if (nsval[i__ - 1] > 132) {
		    s_wsfe(&io___141);
		    do_fio(&c__1, "   NS ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L120: */
	    }
	    s_wsfe(&io___142);
	    do_fio(&c__1, "NS:   ", (ftnlen)6);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		nsval[i__ - 1] = 1;
/* L130: */
	    }
	}

/*        Read the values for MAXB. */

	if (dgg) {
	    s_rsle(&io___143);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&mxbval[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (mxbval[i__ - 1] < 0) {
		    s_wsfe(&io___144);
		    do_fio(&c__1, " MAXB ", (ftnlen)6);
		    do_fio(&c__1, (char *)&mxbval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		} else if (mxbval[i__ - 1] > 132) {
		    s_wsfe(&io___145);
		    do_fio(&c__1, " MAXB ", (ftnlen)6);
		    do_fio(&c__1, (char *)&mxbval[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L140: */
	    }
	    s_wsfe(&io___146);
	    do_fio(&c__1, "MAXB: ", (ftnlen)6);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&mxbval[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		mxbval[i__ - 1] = 1;
/* L150: */
	    }
	}

/*        Read the values for INMIN. */

	if (nep) {
	    s_rsle(&io___147);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&inmin[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (inmin[i__ - 1] < 0) {
		    s_wsfe(&io___148);
		    do_fio(&c__1, " INMIN ", (ftnlen)7);
		    do_fio(&c__1, (char *)&inmin[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L540: */
	    }
	    s_wsfe(&io___149);
	    do_fio(&c__1, "INMIN: ", (ftnlen)7);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&inmin[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		inmin[i__ - 1] = 1;
/* L550: */
	    }
	}

/*        Read the values for INWIN. */

	if (nep) {
	    s_rsle(&io___150);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&inwin[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (inwin[i__ - 1] < 0) {
		    s_wsfe(&io___151);
		    do_fio(&c__1, " INWIN ", (ftnlen)7);
		    do_fio(&c__1, (char *)&inwin[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L560: */
	    }
	    s_wsfe(&io___152);
	    do_fio(&c__1, "INWIN: ", (ftnlen)7);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&inwin[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		inwin[i__ - 1] = 1;
/* L570: */
	    }
	}

/*        Read the values for INIBL. */

	if (nep) {
	    s_rsle(&io___153);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&inibl[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (inibl[i__ - 1] < 0) {
		    s_wsfe(&io___154);
		    do_fio(&c__1, " INIBL ", (ftnlen)7);
		    do_fio(&c__1, (char *)&inibl[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L580: */
	    }
	    s_wsfe(&io___155);
	    do_fio(&c__1, "INIBL: ", (ftnlen)7);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&inibl[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		inibl[i__ - 1] = 1;
/* L590: */
	    }
	}

/*        Read the values for ISHFTS. */

	if (nep) {
	    s_rsle(&io___156);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&ishfts[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (ishfts[i__ - 1] < 0) {
		    s_wsfe(&io___157);
		    do_fio(&c__1, " ISHFTS ", (ftnlen)8);
		    do_fio(&c__1, (char *)&ishfts[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L600: */
	    }
	    s_wsfe(&io___158);
	    do_fio(&c__1, "ISHFTS: ", (ftnlen)8);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&ishfts[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		ishfts[i__ - 1] = 1;
/* L610: */
	    }
	}

/*        Read the values for IACC22. */

	if (nep) {
	    s_rsle(&io___159);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&iacc22[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (iacc22[i__ - 1] < 0) {
		    s_wsfe(&io___160);
		    do_fio(&c__1, " IACC22 ", (ftnlen)8);
		    do_fio(&c__1, (char *)&iacc22[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L620: */
	    }
	    s_wsfe(&io___161);
	    do_fio(&c__1, "IACC22: ", (ftnlen)8);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&iacc22[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		iacc22[i__ - 1] = 1;
/* L630: */
	    }
	}

/*        Read the values for NBCOL. */

	if (dgg) {
	    s_rsle(&io___162);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_lio(&c__3, &c__1, (char *)&nbcol[i__ - 1], (ftnlen)sizeof(
			integer));
	    }
	    e_rsle();
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (nbcol[i__ - 1] < 0) {
		    s_wsfe(&io___164);
		    do_fio(&c__1, "NBCOL ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nbcol[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__0, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		} else if (nbcol[i__ - 1] > 132) {
		    s_wsfe(&io___165);
		    do_fio(&c__1, "NBCOL ", (ftnlen)6);
		    do_fio(&c__1, (char *)&nbcol[i__ - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&c__132, (ftnlen)sizeof(integer));
		    e_wsfe();
		    fatal = TRUE_;
		}
/* L160: */
	    }
	    s_wsfe(&io___166);
	    do_fio(&c__1, "NBCOL:", (ftnlen)6);
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&nbcol[i__ - 1], (ftnlen)sizeof(integer)
			);
	    }
	    e_wsfe();
	} else {
	    i__1 = nparms;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		nbcol[i__ - 1] = 1;
/* L170: */
	    }
	}
    }

/*     Calculate and print the machine dependent constants. */

    s_wsle(&io___167);
    e_wsle();
    eps = dlamch_("Underflow threshold");
    s_wsfe(&io___169);
    do_fio(&c__1, "underflow", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    eps = dlamch_("Overflow threshold");
    s_wsfe(&io___170);
    do_fio(&c__1, "overflow ", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();
    eps = dlamch_("Epsilon");
    s_wsfe(&io___171);
    do_fio(&c__1, "precision", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(doublereal));
    e_wsfe();

/*     Read the threshold value for the test ratios. */

    s_rsle(&io___172);
    do_lio(&c__5, &c__1, (char *)&thresh, (ftnlen)sizeof(doublereal));
    e_rsle();
    s_wsfe(&io___173);
    do_fio(&c__1, (char *)&thresh, (ftnlen)sizeof(doublereal));
    e_wsfe();
    if (sep || svd || dgg) {

/*        Read the flag that indicates whether to test LAPACK routines. */

	s_rsle(&io___174);
	do_lio(&c__8, &c__1, (char *)&tstchk, (ftnlen)sizeof(logical));
	e_rsle();

/*        Read the flag that indicates whether to test driver routines. */

	s_rsle(&io___176);
	do_lio(&c__8, &c__1, (char *)&tstdrv, (ftnlen)sizeof(logical));
	e_rsle();
    }

/*     Read the flag that indicates whether to test the error exits. */

    s_rsle(&io___178);
    do_lio(&c__8, &c__1, (char *)&tsterr, (ftnlen)sizeof(logical));
    e_rsle();

/*     Read the code describing how to set the random number seed. */

    s_rsle(&io___179);
    do_lio(&c__3, &c__1, (char *)&newsd, (ftnlen)sizeof(integer));
    e_rsle();

/*     If NEWSD = 2, read another line with 4 integers for the seed. */

    if (newsd == 2) {
	s_rsle(&io___181);
	for (i__ = 1; i__ <= 4; ++i__) {
	    do_lio(&c__3, &c__1, (char *)&ioldsd[i__ - 1], (ftnlen)sizeof(
		    integer));
	}
	e_rsle();
    }

    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = ioldsd[i__ - 1];
/* L180: */
    }

    if (fatal) {
	s_wsfe(&io___183);
	e_wsfe();
	s_stop("", (ftnlen)0);
    }

/*     Read the input lines indicating the test path and its parameters. */
/*     The first three characters indicate the test path, and the number */
/*     of test matrix types must be the first nonblank item in columns */
/*     4-80. */

L190:

    if (! (dgx || dxv)) {

L200:
	ci__1.cierr = 0;
	ci__1.ciend = 1;
	ci__1.ciunit = 5;
	ci__1.cifmt = "(A80)";
	i__1 = s_rsfe(&ci__1);
	if (i__1 != 0) {
	    goto L380;
	}
	i__1 = do_fio(&c__1, line, (ftnlen)80);
	if (i__1 != 0) {
	    goto L380;
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L380;
	}
	s_copy(c3, line, (ftnlen)3, (ftnlen)3);
	lenp = i_len(line, (ftnlen)80);
	i__ = 3;
	itmp = 0;
	i1 = 0;
L210:
	++i__;
	if (i__ > lenp) {
	    if (i1 > 0) {
		goto L240;
	    } else {
		ntypes = 30;
		goto L240;
	    }
	}
	if (*(unsigned char *)&line[i__ - 1] != ' ' && *(unsigned char *)&
		line[i__ - 1] != ',') {
	    i1 = i__;
	    *(unsigned char *)c1 = *(unsigned char *)&line[i1 - 1];

/*        Check that a valid integer was read */

	    for (k = 1; k <= 10; ++k) {
		if (*(unsigned char *)c1 == *(unsigned char *)&intstr[k - 1]) 
			{
		    ic = k - 1;
		    goto L230;
		}
/* L220: */
	    }
	    s_wsfe(&io___192);
	    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	    do_fio(&c__1, line, (ftnlen)80);
	    e_wsfe();
	    goto L200;
L230:
	    itmp = itmp * 10 + ic;
	    goto L210;
	} else if (i1 > 0) {
	    goto L240;
	} else {
	    goto L210;
	}
L240:
	ntypes = itmp;

/*     Skip the tests if NTYPES is <= 0. */

	if (! (dev || des || dvx || dsx || dgv || dgs) && ntypes <= 0) {
	    s_wsfe(&io___193);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	    goto L200;
	}

    } else {
	if (dxv) {
	    s_copy(c3, "DXV", (ftnlen)3, (ftnlen)3);
	}
	if (dgx) {
	    s_copy(c3, "DGX", (ftnlen)3, (ftnlen)3);
	}
    }

/*     Reset the random number seed. */

    if (newsd == 0) {
	for (k = 1; k <= 4; ++k) {
	    iseed[k - 1] = ioldsd[k - 1];
/* L250: */
	}
    }

    if (lsamen_(&c__3, c3, "DHS") || lsamen_(&c__3, c3, 
	    "NEP")) {

/*        ------------------------------------- */
/*        NEP:  Nonsymmetric Eigenvalue Problem */
/*        ------------------------------------- */
/*        Vary the parameters */
/*           NB    = block size */
/*           NBMIN = minimum block size */
/*           NX    = crossover point */
/*           NS    = number of shifts */
/*           MAXB  = minimum submatrix size */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	xlaenv_(&c__1, &c__1);
	if (tsterr) {
	    derrhs_("DHSEQR", &c__6);
	}
	i__1 = nparms;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    xlaenv_(&c__1, &nbval[i__ - 1]);
	    xlaenv_(&c__2, &nbmin[i__ - 1]);
	    xlaenv_(&c__3, &nxval[i__ - 1]);
/* Computing MAX */
	    i__3 = 11, i__4 = inmin[i__ - 1];
	    i__2 = max(i__3,i__4);
	    xlaenv_(&c__12, &i__2);
	    xlaenv_(&c__13, &inwin[i__ - 1]);
	    xlaenv_(&c__14, &inibl[i__ - 1]);
	    xlaenv_(&c__15, &ishfts[i__ - 1]);
	    xlaenv_(&c__16, &iacc22[i__ - 1]);

	    if (newsd == 0) {
		for (k = 1; k <= 4; ++k) {
		    iseed[k - 1] = ioldsd[k - 1];
/* L260: */
		}
	    }
	    s_wsfe(&io___196);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
/* Computing MAX */
	    i__3 = 11, i__4 = inmin[i__ - 1];
	    i__2 = max(i__3,i__4);
	    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&inwin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&inibl[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&ishfts[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iacc22[i__ - 1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    dchkhs_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], &a[52272], &a[69696], &
		    c__132, &a[87120], &a[104544], d__, &d__[132], &d__[264], 
		    &d__[396], &a[121968], &a[139392], &a[156816], &a[174240], 
		     &a[191664], &d__[528], work, &c__87781, iwork, logwrk, 
		    result, &info);
	    if (info != 0) {
		s_wsfe(&io___204);
		do_fio(&c__1, "DCHKHS", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
/* L270: */
	}

    } else if (lsamen_(&c__3, c3, "DST") || lsamen_(&
	    c__3, c3, "SEP")) {

/*        ---------------------------------- */
/*        SEP:  Symmetric Eigenvalue Problem */
/*        ---------------------------------- */
/*        Vary the parameters */
/*           NB    = block size */
/*           NBMIN = minimum block size */
/*           NX    = crossover point */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	xlaenv_(&c__1, &c__1);
	xlaenv_(&c__9, &c__25);
	if (tsterr) {
	    derrst_("DST", &c__6);
	}
	i__1 = nparms;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    xlaenv_(&c__1, &nbval[i__ - 1]);
	    xlaenv_(&c__2, &nbmin[i__ - 1]);
	    xlaenv_(&c__3, &nxval[i__ - 1]);

	    if (newsd == 0) {
		for (k = 1; k <= 4; ++k) {
		    iseed[k - 1] = ioldsd[k - 1];
/* L280: */
		}
	    }
	    s_wsfe(&io___205);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    if (tstchk) {
		dchkst_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, 
			&c__132, &a[17424], d__, &d__[132], &d__[264], &d__[
			396], &d__[528], &d__[660], &d__[792], &d__[924], &
			d__[1056], &d__[1188], &d__[1320], &a[34848], &c__132, 
			 &a[52272], &a[69696], &d__[1452], &a[87120], work, &
			c__87781, iwork, &c__89760, result, &info);
		if (info != 0) {
		    s_wsfe(&io___206);
		    do_fio(&c__1, "DCHKST", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
	    if (tstdrv) {
		ddrvst_(&nn, nval, &c__18, dotype, iseed, &thresh, &c__6, a, &
			c__132, &d__[264], &d__[396], &d__[528], &d__[660], &
			d__[924], &d__[1056], &d__[1188], &d__[1320], &a[
			17424], &c__132, &a[34848], &d__[1452], &a[52272], 
			work, &c__87781, iwork, &c__89760, result, &info);
		if (info != 0) {
		    s_wsfe(&io___207);
		    do_fio(&c__1, "DDRVST", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
/* L290: */
	}

    } else if (lsamen_(&c__3, c3, "DSG")) {

/*        ---------------------------------------------- */
/*        DSG:  Symmetric Generalized Eigenvalue Problem */
/*        ---------------------------------------------- */
/*        Vary the parameters */
/*           NB    = block size */
/*           NBMIN = minimum block size */
/*           NX    = crossover point */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	xlaenv_(&c__9, &c__25);
	i__1 = nparms;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    xlaenv_(&c__1, &nbval[i__ - 1]);
	    xlaenv_(&c__2, &nbmin[i__ - 1]);
	    xlaenv_(&c__3, &nxval[i__ - 1]);

	    if (newsd == 0) {
		for (k = 1; k <= 4; ++k) {
		    iseed[k - 1] = ioldsd[k - 1];
/* L300: */
		}
	    }
	    s_wsfe(&io___208);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    if (tstchk) {
		ddrvsg_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, 
			&c__132, &a[17424], &c__132, &d__[264], &a[34848], &
			c__132, &a[52272], &a[69696], &a[87120], &a[104544], 
			work, &c__87781, iwork, &c__89760, result, &info);
		if (info != 0) {
		    s_wsfe(&io___209);
		    do_fio(&c__1, "DDRVSG", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
/* L310: */
	}

    } else if (lsamen_(&c__3, c3, "DBD") || lsamen_(&
	    c__3, c3, "SVD")) {

/*        ---------------------------------- */
/*        SVD:  Singular Value Decomposition */
/*        ---------------------------------- */
/*        Vary the parameters */
/*           NB    = block size */
/*           NBMIN = minimum block size */
/*           NX    = crossover point */
/*           NRHS  = number of right hand sides */

	maxtyp = 16;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	xlaenv_(&c__1, &c__1);
	xlaenv_(&c__9, &c__25);

/*        Test the error exits */

	if (tsterr && tstchk) {
	    derrbd_("DBD", &c__6);
	}
	if (tsterr && tstdrv) {
	    derred_("DBD", &c__6);
	}

	i__1 = nparms;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nrhs = nsval[i__ - 1];
	    xlaenv_(&c__1, &nbval[i__ - 1]);
	    xlaenv_(&c__2, &nbmin[i__ - 1]);
	    xlaenv_(&c__3, &nxval[i__ - 1]);
	    if (newsd == 0) {
		for (k = 1; k <= 4; ++k) {
		    iseed[k - 1] = ioldsd[k - 1];
/* L320: */
		}
	    }
	    s_wsfe(&io___211);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(integer));
	    e_wsfe();
	    if (tstchk) {
		dchkbd_(&nn, mval, nval, &maxtyp, dotype, &nrhs, iseed, &
			thresh, a, &c__132, d__, &d__[132], &d__[264], &d__[
			396], &a[17424], &c__132, &a[34848], &a[52272], &a[
			69696], &c__132, &a[87120], &c__132, &a[104544], &a[
			121968], work, &c__87781, iwork, &c__6, &info);
		if (info != 0) {
		    s_wsfe(&io___212);
		    do_fio(&c__1, "DCHKBD", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
	    if (tstdrv) {
		ddrvbd_(&nn, mval, nval, &maxtyp, dotype, iseed, &thresh, a, &
			c__132, &a[17424], &c__132, &a[34848], &c__132, &a[
			52272], &a[69696], &a[87120], d__, &d__[132], &d__[
			264], work, &c__87781, iwork, &c__6, &info);
	    }
/* L330: */
	}

    } else if (lsamen_(&c__3, c3, "DEV")) {

/*        -------------------------------------------- */
/*        DEV:  Nonsymmetric Eigenvalue Problem Driver */
/*              DGEEV (eigenvalues and eigenvectors) */
/*        -------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___213);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    ddrvev_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], d__, &d__[132], &d__[264], &d__[396], &
		    a[34848], &c__132, &a[52272], &c__132, &a[69696], &c__132, 
		     result, work, &c__87781, iwork, &info);
	    if (info != 0) {
		s_wsfe(&io___214);
		do_fio(&c__1, "DGEEV", (ftnlen)5);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___215);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "DES")) {

/*        -------------------------------------------- */
/*        DES:  Nonsymmetric Eigenvalue Problem Driver */
/*              DGEES (Schur form) */
/*        -------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___216);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    ddrves_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], d__, &d__[132], &d__[264], &
		    d__[396], &a[52272], &c__132, result, work, &c__87781, 
		    iwork, logwrk, &info);
	    if (info != 0) {
		s_wsfe(&io___217);
		do_fio(&c__1, "DGEES", (ftnlen)5);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___218);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "DVX")) {

/*        -------------------------------------------------------------- */
/*        DVX:  Nonsymmetric Eigenvalue Problem Expert Driver */
/*              DGEEVX (eigenvalues, eigenvectors and condition numbers) */
/*        -------------------------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes < 0) {
	    s_wsfe(&io___219);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    ddrvvx_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__5, &c__6, 
		    a, &c__132, &a[17424], d__, &d__[132], &d__[264], &d__[
		    396], &a[34848], &c__132, &a[52272], &c__132, &a[69696], &
		    c__132, &d__[528], &d__[660], &d__[792], &d__[924], &d__[
		    1056], &d__[1188], &d__[1320], &d__[1452], result, work, &
		    c__87781, iwork, &info);
	    if (info != 0) {
		s_wsfe(&io___220);
		do_fio(&c__1, "DGEEVX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___221);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "DSX")) {

/*        --------------------------------------------------- */
/*        DSX:  Nonsymmetric Eigenvalue Problem Expert Driver */
/*              DGEESX (Schur form and condition numbers) */
/*        --------------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes < 0) {
	    s_wsfe(&io___222);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    ddrvsx_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__5, &c__6, 
		    a, &c__132, &a[17424], &a[34848], d__, &d__[132], &d__[
		    264], &d__[396], &d__[528], &d__[660], &a[52272], &c__132, 
		     &a[69696], result, work, &c__87781, iwork, logwrk, &info)
		    ;
	    if (info != 0) {
		s_wsfe(&io___223);
		do_fio(&c__1, "DGEESX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___224);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "DGG")) {

/*        ------------------------------------------------- */
/*        DGG:  Generalized Nonsymmetric Eigenvalue Problem */
/*        ------------------------------------------------- */
/*        Vary the parameters */
/*           NB    = block size */
/*           NBMIN = minimum block size */
/*           NS    = number of shifts */
/*           MAXB  = minimum submatrix size */
/*           NBCOL = minimum column dimension for blocks */

	maxtyp = 26;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	if (tstchk && tsterr) {
	    derrgg_(c3, &c__6);
	}
	i__1 = nparms;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    xlaenv_(&c__1, &nbval[i__ - 1]);
	    xlaenv_(&c__2, &nbmin[i__ - 1]);
	    xlaenv_(&c__4, &nsval[i__ - 1]);
	    xlaenv_(&c__8, &mxbval[i__ - 1]);
	    xlaenv_(&c__5, &nbcol[i__ - 1]);

	    if (newsd == 0) {
		for (k = 1; k <= 4; ++k) {
		    iseed[k - 1] = ioldsd[k - 1];
/* L340: */
		}
	    }
	    s_wsfe(&io___225);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&mxbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbcol[i__ - 1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    tstdif = FALSE_;
	    thrshn = 10.;
	    if (tstchk) {
		dchkgg_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &tstdif, &
			thrshn, &c__6, a, &c__132, &a[17424], &a[34848], &a[
			52272], &a[69696], &a[87120], &a[104544], &a[121968], 
			&a[139392], &c__132, &a[156816], &a[174240], &a[
			191664], d__, &d__[132], &d__[264], &d__[396], &d__[
			528], &d__[660], &a[209088], &a[226512], work, &
			c__87781, logwrk, result, &info);
		if (info != 0) {
		    s_wsfe(&io___228);
		    do_fio(&c__1, "DCHKGG", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
	    xlaenv_(&c__1, &c__1);
	    if (tstdrv) {
		ddrvgg_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &thrshn, &
			c__6, a, &c__132, &a[17424], &a[34848], &a[52272], &a[
			69696], &a[87120], &a[104544], &c__132, &a[121968], 
			d__, &d__[132], &d__[264], &d__[396], &d__[528], &d__[
			660], &a[209088], &a[226512], work, &c__87781, result, 
			 &info);
		if (info != 0) {
		    s_wsfe(&io___229);
		    do_fio(&c__1, "DDRVGG", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
/* L350: */
	}

    } else if (lsamen_(&c__3, c3, "DGS")) {

/*        ------------------------------------------------- */
/*        DGS:  Generalized Nonsymmetric Eigenvalue Problem */
/*              DGGES (Schur form) */
/*        ------------------------------------------------- */

	maxtyp = 26;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___230);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    ddrges_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], &a[52272], &a[104544], &
		    c__132, &a[121968], d__, &d__[132], &d__[264], work, &
		    c__87781, result, logwrk, &info);

	    if (info != 0) {
		s_wsfe(&io___231);
		do_fio(&c__1, "DDRGES", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___232);
	e_wsfe();
	goto L10;

    } else if (dgx) {

/*        ------------------------------------------------- */
/*        DGX:  Generalized Nonsymmetric Eigenvalue Problem */
/*              DGGESX (Schur form and condition numbers) */
/*        ------------------------------------------------- */

	maxtyp = 5;
	ntypes = maxtyp;
	if (nn < 0) {
	    s_wsfe(&io___233);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    xlaenv_(&c__5, &c__2);
	    ddrgsx_(&nn, &c__20, &thresh, &c__5, &c__6, a, &c__132, &a[17424], 
		     &a[34848], &a[52272], &a[69696], &a[87120], d__, &d__[
		    132], &d__[264], c__, &c__400, &a[191664], work, &
		    c__87781, iwork, &c__89760, logwrk, &info);
	    if (info != 0) {
		s_wsfe(&io___235);
		do_fio(&c__1, "DDRGSX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___236);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "DGV")) {

/*        ------------------------------------------------- */
/*        DGV:  Generalized Nonsymmetric Eigenvalue Problem */
/*              DGGEV (Eigenvalue/vector form) */
/*        ------------------------------------------------- */

	maxtyp = 26;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___237);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    ddrgev_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], &a[52272], &a[104544], &
		    c__132, &a[121968], &a[139392], &c__132, d__, &d__[132], &
		    d__[264], &d__[396], &d__[528], &d__[660], work, &
		    c__87781, result, &info);
	    if (info != 0) {
		s_wsfe(&io___238);
		do_fio(&c__1, "DDRGEV", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___239);
	e_wsfe();
	goto L10;

    } else if (dxv) {

/*        ------------------------------------------------- */
/*        DXV:  Generalized Nonsymmetric Eigenvalue Problem */
/*              DGGEVX (eigenvalue/vector with condition numbers) */
/*        ------------------------------------------------- */

	maxtyp = 2;
	ntypes = maxtyp;
	if (nn < 0) {
	    s_wsfe(&io___240);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		derrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    ddrgvx_(&nn, &thresh, &c__5, &c__6, a, &c__132, &a[17424], &a[
		    34848], &a[52272], d__, &d__[132], &d__[264], &a[69696], &
		    a[87120], iwork, &iwork[1], &d__[396], &d__[528], &d__[
		    660], &d__[792], &d__[924], &d__[1056], work, &c__87781, &
		    iwork[2], &c__89758, result, logwrk, &info);

	    if (info != 0) {
		s_wsfe(&io___241);
		do_fio(&c__1, "DDRGVX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___242);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "DSB")) {

/*        ------------------------------ */
/*        DSB:  Symmetric Band Reduction */
/*        ------------------------------ */

	maxtyp = 15;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	if (tsterr) {
	    derrst_("DSB", &c__6);
	}
	dchksb_(&nn, nval, &nk, kval, &maxtyp, dotype, iseed, &thresh, &c__6, 
		a, &c__132, d__, &d__[132], &a[17424], &c__132, work, &
		c__87781, result, &info);
	if (info != 0) {
	    s_wsfe(&io___243);
	    do_fio(&c__1, "DCHKSB", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "DBB")) {

/*        ------------------------------ */
/*        DBB:  General Band Reduction */
/*        ------------------------------ */

	maxtyp = 15;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	i__1 = nparms;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nrhs = nsval[i__ - 1];

	    if (newsd == 0) {
		for (k = 1; k <= 4; ++k) {
		    iseed[k - 1] = ioldsd[k - 1];
/* L360: */
		}
	    }
	    s_wsfe(&io___244);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(integer));
	    e_wsfe();
	    dchkbb_(&nn, mval, nval, &nk, kval, &maxtyp, dotype, &nrhs, iseed, 
		     &thresh, &c__6, a, &c__132, &a[17424], &c__264, d__, &
		    d__[132], &a[52272], &c__132, &a[69696], &c__132, &a[
		    87120], &c__132, &a[104544], work, &c__87781, result, &
		    info);
	    if (info != 0) {
		s_wsfe(&io___245);
		do_fio(&c__1, "DCHKBB", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
/* L370: */
	}

    } else if (lsamen_(&c__3, c3, "GLM")) {

/*        ----------------------------------------- */
/*        GLM:  Generalized Linear Regression Model */
/*        ----------------------------------------- */

	xlaenv_(&c__1, &c__1);
	if (tsterr) {
	    derrgg_("GLM", &c__6);
	}
	dckglm_(&nn, mval, pval, nval, &ntypes, iseed, &thresh, &c__132, a, &
		a[17424], b, &b[17424], x, work, d__, &c__5, &c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___248);
	    do_fio(&c__1, "DCKGLM", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "GQR")) {

/*        ------------------------------------------ */
/*        GQR:  Generalized QR and RQ factorizations */
/*        ------------------------------------------ */

	xlaenv_(&c__1, &c__1);
	if (tsterr) {
	    derrgg_("GQR", &c__6);
	}
	dckgqr_(&nn, mval, &nn, pval, &nn, nval, &ntypes, iseed, &thresh, &
		c__132, a, &a[17424], &a[34848], &a[52272], taua, b, &b[17424]
, &b[34848], &b[52272], &b[69696], taub, work, d__, &c__5, &
		c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___251);
	    do_fio(&c__1, "DCKGQR", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "GSV")) {

/*        ---------------------------------------------- */
/*        GSV:  Generalized Singular Value Decomposition */
/*        ---------------------------------------------- */

	if (tsterr) {
	    derrgg_("GSV", &c__6);
	}
	dckgsv_(&nn, mval, pval, nval, &ntypes, iseed, &thresh, &c__132, a, &
		a[17424], b, &b[17424], &a[34848], &b[34848], &a[52272], taua, 
		 taub, &b[52272], iwork, work, d__, &c__5, &c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___252);
	    do_fio(&c__1, "DCKGSV", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "LSE")) {

/*        -------------------------------------- */
/*        LSE:  Constrained Linear Least Squares */
/*        -------------------------------------- */

	xlaenv_(&c__1, &c__1);
	if (tsterr) {
	    derrgg_("LSE", &c__6);
	}
	dcklse_(&nn, mval, pval, nval, &ntypes, iseed, &thresh, &c__132, a, &
		a[17424], b, &b[17424], x, work, d__, &c__5, &c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___253);
	    do_fio(&c__1, "DCKLSE", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else {
	s_wsle(&io___254);
	e_wsle();
	s_wsle(&io___255);
	e_wsle();
	s_wsfe(&io___256);
	do_fio(&c__1, c3, (ftnlen)3);
	e_wsfe();
    }
    if (! (dgx || dxv)) {
	goto L190;
    }
L380:
    s_wsfe(&io___257);
    e_wsfe();
    s2 = dsecnd_();
    s_wsfe(&io___259);
    d__1 = s2 - s1;
    do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
    e_wsfe();

/* L9998: */

/*     End of DCHKEE */

    return 0;
} /* MAIN__ */

/* Main program alias */ int dchkee_ () { MAIN__ (); return 0; }
