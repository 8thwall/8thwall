#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer nproc, nshift, maxb;
} cenvir_;

#define cenvir_1 cenvir_

struct {
    integer iparms[100];
} claenv_;

#define claenv_1 claenv_

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
    real selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

/* Table of constant values */

static integer c__1 = 1;
static integer c__3 = 3;
static integer c__5 = 5;
static integer c__6 = 6;
static integer c__4 = 4;
static integer c__20 = 20;
static integer c__0 = 0;
static integer c__132 = 132;
static integer c__2 = 2;
static integer c__12 = 12;
static integer c__13 = 13;
static integer c__14 = 14;
static integer c__15 = 15;
static integer c__16 = 16;
static integer c__8 = 8;
static integer c__89760 = 89760;
static integer c__9 = 9;
static integer c__25 = 25;
static integer c__20064 = 20064;
static integer c__18 = 18;
static integer c__400 = 400;
static integer c__20062 = 20062;
static integer c__264 = 264;

/* Main program */ int MAIN__(void)
{
    /* Initialized data */

    static char intstr[10] = "0123456789";
    static integer ioldsd[4] = { 0,0,0,1 };

    /* Format strings */
    static char fmt_9987[] = "(\002 Tests of the Nonsymmetric Eigenvalue Pro"
	    "blem routines\002)";
    static char fmt_9986[] = "(\002 Tests of the Hermitian Eigenvalue Proble"
	    "m routines\002)";
    static char fmt_9985[] = "(\002 Tests of the Singular Value Decompositio"
	    "n routines\002)";
    static char fmt_9979[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Driver\002,/\002    CGEEV (eigenvalues and eigevectors)"
	    "\002)";
    static char fmt_9978[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Driver\002,/\002    CGEES (Schur form)\002)";
    static char fmt_9977[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Expert\002,\002 Driver\002,/\002    CGEEVX (eigenvalues, e"
	    "igenvectors and\002,\002 condition numbers)\002)";
    static char fmt_9976[] = "(/\002 Tests of the Nonsymmetric Eigenvalue Pr"
	    "oblem Expert\002,\002 Driver\002,/\002    CGEESX (Schur form and"
	    " condition\002,\002 numbers)\002)";
    static char fmt_9975[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem routines\002)";
    static char fmt_9964[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Driver CGGES\002)";
    static char fmt_9965[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Expert Driver CGGESX\002)";
    static char fmt_9963[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Driver CGGEV\002)";
    static char fmt_9962[] = "(/\002 Tests of the Generalized Nonsymmetric E"
	    "igenvalue \002,\002Problem Expert Driver CGGEVX\002)";
    static char fmt_9974[] = "(\002 Tests of CHBTRD\002,/\002 (reduction of "
	    "a Hermitian band \002,\002matrix to real tridiagonal form)\002)";
    static char fmt_9967[] = "(\002 Tests of CGBBRD\002,/\002 (reduction of "
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
	    " be\002,e16.6)";
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
    real r__1;
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
    complex a[243936]	/* was [17424][14] */, b[87120]	/* was [17424][5] */, 
	    c__[160000]	/* was [400][400] */;
    integer i__, k;
    real s[17424];
    complex x[660];
    char c1[1], c3[3];
    integer i1;
    real s1, s2;
    complex dc[792]	/* was [132][6] */;
    integer ic;
    real dr[1584]	/* was [132][12] */;
    integer nk, nn, vers_patch__, vers_major__, vers_minor__;
    logical cbb, chb, cbk, cbl, cgg, cgk, cgl, ces, cgs, cev, cgv, glm, cgx, 
	    nep, lse, sep;
    real eps;
    logical gqr, svd, csx, gsv, cvx, cxv;
    real beta[132];
    char line[80];
    complex taua[132];
    integer info;
    char path[3];
    integer kval[20], lenp, mval[20], nval[20];
    complex taub[132];
    integer pval[20], itmp, nrhs;
    complex work[89760];
    integer iacc22[20];
    real alpha[132];
    logical fatal;
    integer iseed[4], nbcol[20], inibl[20], nbval[20], nbmin[20];
    char vname[6];
    integer inmin[20], newsd, nsval[20], inwin[20], nxval[20], iwork[20064];
    real rwork[89760];
    extern /* Subroutine */ int cchkbb_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, logical *, integer *, integer *, 
	    real *, integer *, complex *, integer *, complex *, integer *, 
	    real *, real *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, real *, 
	    real *, integer *), cchkbd_(integer *, integer *, integer *, 
	    integer *, logical *, integer *, integer *, real *, complex *, 
	    integer *, real *, real *, real *, real *, complex *, integer *, 
	    complex *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, complex *, integer *, real *, integer *, 
	    integer *), cchkec_(real *, logical *, integer *, integer *), 
	    cchkhb_(integer *, integer *, integer *, integer *, integer *, 
	    logical *, integer *, real *, integer *, complex *, integer *, 
	    real *, real *, complex *, integer *, complex *, integer *, real *
, real *, integer *), cchkbk_(integer *, integer *), cchkbl_(
	    integer *, integer *), cchkgg_(integer *, integer *, integer *, 
	    logical *, integer *, real *, logical *, real *, integer *, 
	    complex *, integer *, complex *, complex *, complex *, complex *, 
	    complex *, complex *, complex *, complex *, integer *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, complex *, 
	    complex *, complex *, complex *, integer *, real *, logical *, 
	    real *, integer *), cchkgk_(integer *, integer *), cchkgl_(
	    integer *, integer *), cckglm_(integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, real *, 
	    integer *, integer *, integer *), cerrbd_(char *, integer *), cchkhs_(integer *, integer *, integer *, logical *, 
	    integer *, real *, integer *, complex *, integer *, complex *, 
	    complex *, complex *, complex *, integer *, complex *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, complex *, 
	    complex *, complex *, complex *, integer *, real *, integer *, 
	    logical *, real *, integer *), ccklse_(integer *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    complex *, complex *, complex *, complex *, complex *, complex *, 
	    real *, integer *, integer *, integer *), alareq_(char *, integer 
	    *, logical *, integer *, integer *, integer *), cdrvbd_(
	    integer *, integer *, integer *, integer *, logical *, integer *, 
	    real *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, complex *, complex *, complex *, real *, real *, real *
, complex *, integer *, real *, integer *, integer *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int cdrges_(integer *, integer *, integer *, 
	    logical *, integer *, real *, integer *, complex *, integer *, 
	    complex *, complex *, complex *, complex *, integer *, complex *, 
	    complex *, complex *, complex *, integer *, real *, real *, 
	    logical *, integer *), cerred_(char *, integer *), 
	    cckgqr_(integer *, integer *, integer *, integer *, integer *, 
	    integer *, integer *, integer *, real *, integer *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, real *, 
	    integer *, integer *, integer *);
    extern doublereal second_(void);
    extern /* Subroutine */ int cdrgev_(integer *, integer *, integer *, 
	    logical *, integer *, real *, integer *, complex *, integer *, 
	    complex *, complex *, complex *, complex *, integer *, complex *, 
	    complex *, integer *, complex *, complex *, complex *, complex *, 
	    complex *, integer *, real *, real *, integer *), cdrvgg_(integer 
	    *, integer *, integer *, logical *, integer *, real *, real *, 
	    integer *, complex *, integer *, complex *, complex *, complex *, 
	    complex *, complex *, complex *, integer *, complex *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, complex *, 
	    integer *, real *, real *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int cchkst_(integer *, integer *, integer *, 
	    logical *, integer *, real *, integer *, complex *, integer *, 
	    complex *, real *, real *, real *, real *, real *, real *, real *, 
	     real *, real *, real *, real *, complex *, integer *, complex *, 
	    complex *, complex *, complex *, complex *, integer *, real *, 
	    integer *, integer *, integer *, real *, integer *), cckgsv_(
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    real *, integer *, complex *, complex *, complex *, complex *, 
	    complex *, complex *, complex *, real *, real *, complex *, 
	    integer *, complex *, real *, integer *, integer *, integer *), 
	    cerrgg_(char *, integer *), ilaver_(integer *, integer *, 
	    integer *), cdrves_(integer *, integer *, integer *, logical *, 
	    integer *, real *, integer *, complex *, integer *, complex *, 
	    complex *, complex *, complex *, complex *, integer *, real *, 
	    complex *, integer *, real *, integer *, logical *, integer *), 
	    cerrhs_(char *, integer *), cdrvsg_(integer *, integer *, 
	    integer *, logical *, integer *, real *, integer *, complex *, 
	    integer *, complex *, integer *, real *, complex *, integer *, 
	    complex *, complex *, complex *, complex *, complex *, integer *, 
	    real *, integer *, integer *, integer *, real *, integer *);
    integer mxbval[20];
    extern /* Subroutine */ int cdrgsx_(integer *, integer *, real *, integer 
	    *, integer *, complex *, integer *, complex *, complex *, complex 
	    *, complex *, complex *, complex *, complex *, complex *, integer 
	    *, real *, complex *, integer *, real *, integer *, integer *, 
	    logical *, integer *), cdrvev_(integer *, integer *, integer *, 
	    logical *, integer *, real *, integer *, complex *, integer *, 
	    complex *, complex *, complex *, complex *, integer *, complex *, 
	    integer *, complex *, integer *, real *, complex *, integer *, 
	    real *, integer *, integer *);
    logical tstdif;
    real thresh;
    extern /* Subroutine */ int cdrgvx_(integer *, real *, integer *, integer 
	    *, complex *, integer *, complex *, complex *, complex *, complex 
	    *, complex *, complex *, complex *, integer *, integer *, real *, 
	    real *, real *, real *, real *, real *, complex *, integer *, 
	    real *, integer *, integer *, real *, logical *, integer *);
    logical tstchk;
    integer nparms, ishfts[20];
    extern /* Subroutine */ int cerrst_(char *, integer *);
    logical dotype[30], logwrk[132];
    real thrshn;
    extern /* Subroutine */ int cdrvst_(integer *, integer *, integer *, 
	    logical *, integer *, real *, integer *, complex *, integer *, 
	    real *, real *, real *, real *, real *, real *, complex *, 
	    integer *, complex *, complex *, complex *, complex *, integer *, 
	    real *, integer *, integer *, integer *, real *, integer *), 
	    cdrvsx_(integer *, integer *, integer *, logical *, integer *, 
	    real *, integer *, integer *, complex *, integer *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, integer *, 
	    complex *, real *, complex *, integer *, real *, logical *, 
	    integer *), xlaenv_(integer *, integer *), cdrvvx_(integer *, 
	    integer *, integer *, logical *, integer *, real *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, complex *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, complex *, integer *, real *, integer *);
    real result[500];
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
    static cilist io___205 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___206 = { 0, 6, 0, fmt_9997, 0 };
    static cilist io___208 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___209 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___210 = { 0, 6, 0, fmt_9997, 0 };
    static cilist io___211 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___213 = { 0, 6, 0, fmt_9995, 0 };
    static cilist io___214 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___215 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___216 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___217 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___218 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___219 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___220 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___221 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___222 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___223 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___224 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___225 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___226 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___227 = { 0, 6, 0, fmt_9996, 0 };
    static cilist io___230 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___231 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___232 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___233 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___234 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___235 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___238 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___239 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___240 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___241 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___242 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___243 = { 0, 6, 0, fmt_9990, 0 };
    static cilist io___244 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___245 = { 0, 6, 0, fmt_9973, 0 };
    static cilist io___246 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___247 = { 0, 6, 0, fmt_9966, 0 };
    static cilist io___248 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___251 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___254 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___257 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___258 = { 0, 6, 0, fmt_9980, 0 };
    static cilist io___259 = { 0, 6, 0, 0, 0 };
    static cilist io___260 = { 0, 6, 0, 0, 0 };
    static cilist io___261 = { 0, 6, 0, fmt_9992, 0 };
    static cilist io___262 = { 0, 6, 0, fmt_9994, 0 };
    static cilist io___264 = { 0, 6, 0, fmt_9993, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     February 2007 */

/*  Purpose */
/*  ======= */

/*  CCHKEE tests the COMPLEX LAPACK subroutines for the matrix */
/*  eigenvalue problem.  The test paths in this version are */

/*  NEP (Nonsymmetric Eigenvalue Problem): */
/*      Test CGEHRD, CUNGHR, CHSEQR, CTREVC, CHSEIN, and CUNMHR */

/*  SEP (Hermitian Eigenvalue Problem): */
/*      Test CHETRD, CUNGTR, CSTEQR, CSTERF, CSTEIN, CSTEDC, */
/*      and drivers CHEEV(X), CHBEV(X), CHPEV(X), */
/*                  CHEEVD,   CHBEVD,   CHPEVD */

/*  SVD (Singular Value Decomposition): */
/*      Test CGEBRD, CUNGBR, and CBDSQR */
/*      and the drivers CGESVD, CGESDD */

/*  CEV (Nonsymmetric Eigenvalue/eigenvector Driver): */
/*      Test CGEEV */

/*  CES (Nonsymmetric Schur form Driver): */
/*      Test CGEES */

/*  CVX (Nonsymmetric Eigenvalue/eigenvector Expert Driver): */
/*      Test CGEEVX */

/*  CSX (Nonsymmetric Schur form Expert Driver): */
/*      Test CGEESX */

/*  CGG (Generalized Nonsymmetric Eigenvalue Problem): */
/*      Test CGGHRD, CGGBAL, CGGBAK, CHGEQZ, and CTGEVC */
/*      and the driver routines CGEGS and CGEGV */

/*  CGS (Generalized Nonsymmetric Schur form Driver): */
/*      Test CGGES */

/*  CGV (Generalized Nonsymmetric Eigenvalue/eigenvector Driver): */
/*      Test CGGEV */

/*  CGX (Generalized Nonsymmetric Schur form Expert Driver): */
/*      Test CGGESX */

/*  CXV (Generalized Nonsymmetric Eigenvalue/eigenvector Expert Driver): */
/*      Test CGGEVX */

/*  CSG (Hermitian Generalized Eigenvalue Problem): */
/*      Test CHEGST, CHEGV, CHEGVD, CHEGVX, CHPGST, CHPGV, CHPGVD, */
/*      CHPGVX, CHBGST, CHBGV, CHBGVD, and CHBGVX */

/*  CHB (Hermitian Band Eigenvalue Problem): */
/*      Test CHBTRD */

/*  CBB (Band Singular Value Decomposition): */
/*      Test CGBBRD */

/*  CEC (Eigencondition estimation): */
/*      Test CTRSYL, CTREXC, CTRSNA, and CTRSEN */

/*  CBL (Balancing a general matrix) */
/*      Test CGEBAL */

/*  CBK (Back transformation on a balanced matrix) */
/*      Test CGEBAK */

/*  CGL (Balancing a matrix pair) */
/*      Test CGGBAL */

/*  CGK (Back transformation on a matrix pair) */
/*      Test CGGBAK */

/*  GLM (Generalized Linear Regression Model): */
/*      Tests CGGGLM */

/*  GQR (Generalized QR and RQ factorizations): */
/*      Tests CGGQRF and CGGRQF */

/*  GSV (Generalized Singular Value Decomposition): */
/*      Tests CGGSVD, CGGSVP, CTGSJA, CLAGS2, CLAPLL, and CLAPMT */

/*  LSE (Constrained Linear Least Squares): */
/*      Tests CGGLSE */

/*  Each test path has a different set of inputs, but the data sets for */
/*  the driver routines xEV, xES, xVX, and xSX can be concatenated in a */
/*  single input file.  The first line of input should contain one of the */
/*  3-character path names in columns 1-3.  The number of remaining lines */
/*  depends on what is found on the first line. */

/*  The number of matrix types used in testing is often controllable from */
/*  the input file.  The number of matrix types for each path, and the */
/*  test routine that describes them, is as follows: */

/*  Path name(s)  Types    Test routine */

/*  CHS or NEP      21     CCHKHS */
/*  CST or SEP      21     CCHKST (routines) */
/*                  18     CDRVST (drivers) */
/*  CBD or SVD      16     CCHKBD (routines) */
/*                   5     CDRVBD (drivers) */
/*  CEV             21     CDRVEV */
/*  CES             21     CDRVES */
/*  CVX             21     CDRVVX */
/*  CSX             21     CDRVSX */
/*  CGG             26     CCHKGG (routines) */
/*                  26     CDRVGG (drivers) */
/*  CGS             26     CDRGES */
/*  CGX              5     CDRGSX */
/*  CGV             26     CDRGEV */
/*  CXV              2     CDRGVX */
/*  CSG             21     CDRVSG */
/*  CHB             15     CCHKHB */
/*  CBB             15     CCHKBB */
/*  CEC              -     CCHKEC */
/*  CBL              -     CCHKBL */
/*  CBK              -     CCHKBK */
/*  CGL              -     CCHKGL */
/*  CGK              -     CCHKGK */
/*  GLM              8     CCKGLM */
/*  GQR              8     CCKGQR */
/*  GSV              8     CCKGSV */
/*  LSE              8     CCKLSE */

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

/*  line 11:  ISHFTS, INTEGER array, dimension (NPARMS) */
/*           number of simultaneous shifts) */

/*  line 12:  IACC22, INTEGER array, dimension (NPARMS) */
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

/*           The valid 3-character path names are 'NEP' or 'CHS' for the */
/*           nonsymmetric eigenvalue routines. */

/* ----------------------------------------------------------------------- */

/*  SEP or CSG input file: */

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
/*           The valid 3-character path names are 'SEP' or 'CST' for the */
/*           Hermitian eigenvalue routines and driver routines, and */
/*           'CSG' for the routines for the Hermitian generalized */
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
/*           The 3-character path names are 'SVD' or 'CBD' for both the */
/*           SVD routines and the SVD driver routines. */

/* ----------------------------------------------------------------------- */

/*  CEV and CES data files: */

/*  line 1:  'CEV' or 'CES' in columns 1 to 3. */

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

/*  line 6:  NEWSD, INTEGER */
/*           A code indicating how to set the random number seed. */
/*           = 0:  Set the seed to a default value before each run */
/*           = 1:  Initialize the seed to a default value only before the */
/*                 first run */
/*           = 2:  Like 1, but use the seed values on the next line */

/*  If line 6 was 2: */

/*  line 7:  INTEGER array, dimension (4) */
/*           Four integer values for the random number seed. */

/*  lines 8 and following:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'CEV' to test CGEEV, or */
/*           'CES' to test CGEES. */

/* ----------------------------------------------------------------------- */

/*  The CVX data has two parts. The first part is identical to CEV, */
/*  and the second part consists of test matrices with precomputed */
/*  solutions. */

/*  line 1:  'CVX' in columns 1-3. */

/*  line 2:  NSIZES, INTEGER */
/*           If NSIZES = 0, no testing of randomly generated examples */
/*           is done, but any precomputed examples are tested. */

/*  line 3:  NN, INTEGER array, dimension(NSIZES) */

/*  line 4:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */

/*  line 5:  THRESH, REAL */

/*  line 6:  NEWSD, INTEGER */

/*  If line 6 was 2: */

/*  line 7:  INTEGER array, dimension (4) */

/*  lines 8 and following: The first line contains 'CVX' in columns 1-3 */
/*           followed by the number of matrix types, possibly with */
/*           a second line to specify certain matrix types. */
/*           If the number of matrix types = 0, no testing of randomly */
/*           generated examples is done, but any precomputed examples */
/*           are tested. */

/*  remaining lines : Each matrix is stored on 1+N+N**2 lines, where N is */
/*           its dimension. The first line contains the dimension N and */
/*           ISRT (two integers). ISRT indicates whether the last N lines */
/*           are sorted by increasing real part of the eigenvalue */
/*           (ISRT=0) or by increasing imaginary part (ISRT=1). The next */
/*           N**2 lines contain the matrix rowwise, one entry per line. */
/*           The last N lines correspond to each eigenvalue. Each of */
/*           these last N lines contains 4 real values: the real part of */
/*           the eigenvalues, the imaginary part of the eigenvalue, the */
/*           reciprocal condition number of the eigenvalues, and the */
/*           reciprocal condition number of the vector eigenvector. The */
/*           end of data is indicated by dimension N=0. Even if no data */
/*           is to be tested, there must be at least one line containing */
/*           N=0. */

/* ----------------------------------------------------------------------- */

/*  The CSX data is like CVX. The first part is identical to CEV, and the */
/*  second part consists of test matrices with precomputed solutions. */

/*  line 1:  'CSX' in columns 1-3. */

/*  line 2:  NSIZES, INTEGER */
/*           If NSIZES = 0, no testing of randomly generated examples */
/*           is done, but any precomputed examples are tested. */

/*  line 3:  NN, INTEGER array, dimension(NSIZES) */

/*  line 4:  NB, NBMIN, NX, NS, NBCOL, INTEGERs */

/*  line 5:  THRESH, REAL */

/*  line 6:  NEWSD, INTEGER */

/*  If line 6 was 2: */

/*  line 7:  INTEGER array, dimension (4) */

/*  lines 8 and following: The first line contains 'CSX' in columns 1-3 */
/*           followed by the number of matrix types, possibly with */
/*           a second line to specify certain matrix types. */
/*           If the number of matrix types = 0, no testing of randomly */
/*           generated examples is done, but any precomputed examples */
/*           are tested. */

/*  remaining lines : Each matrix is stored on 3+N**2 lines, where N is */
/*           its dimension. The first line contains the dimension N, the */
/*           dimension M of an invariant subspace, and ISRT. The second */
/*           line contains M integers, identifying the eigenvalues in the */
/*           invariant subspace (by their position in a list of */
/*           eigenvalues ordered by increasing real part (if ISRT=0) or */
/*           by increasing imaginary part (if ISRT=1)). The next N**2 */
/*           lines contain the matrix rowwise. The last line contains the */
/*           reciprocal condition number for the average of the selected */
/*           eigenvalues, and the reciprocal condition number for the */
/*           corresponding right invariant subspace. The end of data in */
/*           indicated by a line containing N=0, M=0, and ISRT = 0.  Even */
/*           if no data is to be tested, there must be at least one line */
/*           containing N=0, M=0 and ISRT=0. */

/* ----------------------------------------------------------------------- */

/*  CGG input file: */

/*  line 2:  NN, INTEGER */
/*           Number of values of N. */

/*  line 3:  NVAL, INTEGER array, dimension (NN) */
/*           The values for the matrix dimension N. */

/*  line 4:  NPARMS, INTEGER */
/*           Number of values of the parameters NB, NBMIN, NBCOL, NS, and */
/*           MAXB. */

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

/*  lines 16-EOF:  Lines specifying matrix types, as for NEP. */
/*           The 3-character path name is 'CGG' for the generalized */
/*           eigenvalue problem routines and driver routines. */

/* ----------------------------------------------------------------------- */

/*  CGS and CGV input files: */

/*  line 1:  'CGS' or 'CGV' in columns 1 to 3. */

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
/*           The 3-character path name is 'CGS' for the generalized */
/*           eigenvalue problem routines and driver routines. */

/* ----------------------------------------------------------------------- */

/*  CGX input file: */
/*  line 1:  'CGX' in columns 1 to 3. */

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

/*  remaining lines : Each example is stored on 3+2*N*N lines, where N is */
/*           its dimension. The first line contains the dimension (a */
/*           single integer).  The next line contains an integer k such */
/*           that only the last k eigenvalues will be selected and appear */
/*           in the leading diagonal blocks of $A$ and $B$. The next N*N */
/*           lines contain the matrix A, one element per line. The next N*N */
/*           lines contain the matrix B. The last line contains the */
/*           reciprocal of the eigenvalue cluster condition number and the */
/*           reciprocal of the deflating subspace (associated with the */
/*           selected eigencluster) condition number.  The end of data is */
/*           indicated by dimension N=0.  Even if no data is to be tested, */
/*           there must be at least one line containing N=0. */

/* ----------------------------------------------------------------------- */

/*  CXV input files: */
/*  line 1:  'CXV' in columns 1 to 3. */

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

/*  remaining lines : Each example is stored on 3+2*N*N lines, where N is */
/*           its dimension. The first line contains the dimension (a */
/*           single integer). The next N*N lines contain the matrix A, one */
/*           element per line. The next N*N lines contain the matrix B. */
/*           The next line contains the reciprocals of the eigenvalue */
/*           condition numbers.  The last line contains the reciprocals of */
/*           the eigenvector condition numbers.  The end of data is */
/*           indicated by dimension N=0.  Even if no data is to be tested, */
/*           there must be at least one line containing N=0. */

/* ----------------------------------------------------------------------- */

/*  CHB input file: */

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
/*           The 3-character path name is 'CHB'. */

/* ----------------------------------------------------------------------- */

/*  CBB input file: */

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
/*           The 3-character path name is 'CBB'. */

/* ----------------------------------------------------------------------- */

/*  CEC input file: */

/*  line  2: THRESH, REAL */
/*           Threshold value for the test ratios.  Information will be */
/*           printed about each test for which the test ratio is greater */
/*           than or equal to the threshold. */

/*  lines  3-EOF: */

/*  Input for testing the eigencondition routines consists of a set of */
/*  specially constructed test cases and their solutions.  The data */
/*  format is not intended to be modified by the user. */

/* ----------------------------------------------------------------------- */

/*  CBL and CBK input files: */

/*  line 1:  'CBL' in columns 1-3 to test CGEBAL, or 'CBK' in */
/*           columns 1-3 to test CGEBAK. */

/*  The remaining lines consist of specially constructed test cases. */

/* ----------------------------------------------------------------------- */

/*  CGL and CGK input files: */

/*  line 1:  'CGL' in columns 1-3 to test CGGBAL, or 'CGK' in */
/*           columns 1-3 to test CGGBAK. */

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
/*  precomputed examples, and LWORK = NMAX*(5*NMAX+20) in the parameter */
/*  statements below.  For SVD, we assume NRHS may be as big as N.  The */
/*  parameter NEED is set to 14 to allow for 14 N-by-N matrices for CGG. */

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

    s1 = second_();
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
	    path, "CHS");
    sep = lsamen_(&c__3, path, "SEP") || lsamen_(&c__3, 
	    path, "CST") || lsamen_(&c__3, path, "CSG");
    svd = lsamen_(&c__3, path, "SVD") || lsamen_(&c__3, 
	    path, "CBD");
    cev = lsamen_(&c__3, path, "CEV");
    ces = lsamen_(&c__3, path, "CES");
    cvx = lsamen_(&c__3, path, "CVX");
    csx = lsamen_(&c__3, path, "CSX");
    cgg = lsamen_(&c__3, path, "CGG");
    cgs = lsamen_(&c__3, path, "CGS");
    cgx = lsamen_(&c__3, path, "CGX");
    cgv = lsamen_(&c__3, path, "CGV");
    cxv = lsamen_(&c__3, path, "CXV");
    chb = lsamen_(&c__3, path, "CHB");
    cbb = lsamen_(&c__3, path, "CBB");
    glm = lsamen_(&c__3, path, "GLM");
    gqr = lsamen_(&c__3, path, "GQR") || lsamen_(&c__3, 
	    path, "GRQ");
    gsv = lsamen_(&c__3, path, "GSV");
    lse = lsamen_(&c__3, path, "LSE");
    cbl = lsamen_(&c__3, path, "CBL");
    cbk = lsamen_(&c__3, path, "CBK");
    cgl = lsamen_(&c__3, path, "CGL");
    cgk = lsamen_(&c__3, path, "CGK");

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
    } else if (cev) {
	s_wsfe(&io___32);
	e_wsfe();
    } else if (ces) {
	s_wsfe(&io___33);
	e_wsfe();
    } else if (cvx) {
	s_wsfe(&io___34);
	e_wsfe();
    } else if (csx) {
	s_wsfe(&io___35);
	e_wsfe();
    } else if (cgg) {
	s_wsfe(&io___36);
	e_wsfe();
    } else if (cgs) {
	s_wsfe(&io___37);
	e_wsfe();
    } else if (cgx) {
	s_wsfe(&io___38);
	e_wsfe();
    } else if (cgv) {
	s_wsfe(&io___39);
	e_wsfe();
    } else if (cxv) {
	s_wsfe(&io___40);
	e_wsfe();
    } else if (chb) {
	s_wsfe(&io___41);
	e_wsfe();
    } else if (cbb) {
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
    } else if (cbl) {

/*        CGEBAL:  Balancing */

	cchkbl_(&c__5, &c__6);
	goto L380;
    } else if (cbk) {

/*        CGEBAK:  Back transformation */

	cchkbk_(&c__5, &c__6);
	goto L380;
    } else if (cgl) {

/*        CGGBAL:  Balancing */

	cchkgl_(&c__5, &c__6);
	goto L380;
    } else if (cgk) {

/*        CGGBAK:  Back transformation */

	cchkgk_(&c__5, &c__6);
	goto L380;
    } else if (lsamen_(&c__3, path, "CEC")) {

/*        CEC:  Eigencondition estimation */

	s_rsle(&io___47);
	do_lio(&c__4, &c__1, (char *)&thresh, (ftnlen)sizeof(real));
	e_rsle();
	xlaenv_(&c__1, &c__1);
	tsterr = TRUE_;
	cchkec_(&thresh, &tsterr, &c__5, &c__6);
	goto L380;
    } else {
	s_wsfe(&io___50);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	goto L380;
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

    if (! (cgx || cxv)) {
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

    if (svd || cbb || glm || gqr || gsv || lse) {
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
    if (! (cgx || cxv)) {
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

    if (chb || cbb) {
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

    if (cev || ces || cvx || csx) {

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

    } else if (cgs || cgx || cgv || cxv) {

/*        For the nonsymmetric generalized driver routines, only one set of */
/*        parameters is allowed. */

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
    } else if (! chb && ! glm && ! gqr && ! gsv && ! lse) {

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

	if (! cbb) {
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

	if (nep || sep || svd || cgg) {
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

/*        Read the values of NSHIFT (if CGG) or NRHS (if SVD */
/*        or CBB). */

	if (svd || cbb || cgg) {
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

	if (cgg) {
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

	if (cgg) {
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
    eps = slamch_("Underflow threshold");
    s_wsfe(&io___169);
    do_fio(&c__1, "underflow", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(real));
    e_wsfe();
    eps = slamch_("Overflow threshold");
    s_wsfe(&io___170);
    do_fio(&c__1, "overflow ", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(real));
    e_wsfe();
    eps = slamch_("Epsilon");
    s_wsfe(&io___171);
    do_fio(&c__1, "precision", (ftnlen)9);
    do_fio(&c__1, (char *)&eps, (ftnlen)sizeof(real));
    e_wsfe();

/*     Read the threshold value for the test ratios. */

    s_rsle(&io___172);
    do_lio(&c__4, &c__1, (char *)&thresh, (ftnlen)sizeof(real));
    e_rsle();
    s_wsfe(&io___173);
    do_fio(&c__1, (char *)&thresh, (ftnlen)sizeof(real));
    e_wsfe();
    if (sep || svd || cgg) {

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

    if (! (cgx || cxv)) {

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

	if (! (cev || ces || cvx || csx || cgv || cgs) && ntypes <= 0) {
	    s_wsfe(&io___193);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	    goto L200;
	}

    } else {
	if (cgx) {
	    s_copy(c3, "CGX", (ftnlen)3, (ftnlen)3);
	}
	if (cxv) {
	    s_copy(c3, "CXV", (ftnlen)3, (ftnlen)3);
	}
    }

/*     Reset the random number seed. */

    if (newsd == 0) {
	for (k = 1; k <= 4; ++k) {
	    iseed[k - 1] = ioldsd[k - 1];
/* L250: */
	}
    }

    if (lsamen_(&c__3, c3, "CHS") || lsamen_(&c__3, c3, 
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
	    cerrhs_("CHSEQR", &c__6);
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
	    cchkhs_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], &a[52272], &a[69696], &
		    c__132, &a[87120], &a[104544], dc, &dc[132], &a[121968], &
		    a[139392], &a[156816], &a[174240], &a[191664], &dc[264], 
		    work, &c__89760, rwork, iwork, logwrk, result, &info);
	    if (info != 0) {
		s_wsfe(&io___205);
		do_fio(&c__1, "CCHKHS", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
/* L270: */
	}

    } else if (lsamen_(&c__3, c3, "CST") || lsamen_(&
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
	    cerrst_("CST", &c__6);
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
	    s_wsfe(&io___206);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    if (tstchk) {
		cchkst_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, 
			&c__132, &a[17424], dr, &dr[132], &dr[264], &dr[396], 
			&dr[528], &dr[660], &dr[792], &dr[924], &dr[1056], &
			dr[1188], &dr[1320], &a[34848], &c__132, &a[52272], &
			a[69696], dc, &a[87120], work, &c__89760, rwork, &
			c__89760, iwork, &c__20064, result, &info);
		if (info != 0) {
		    s_wsfe(&io___208);
		    do_fio(&c__1, "CCHKST", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
	    if (tstdrv) {
		cdrvst_(&nn, nval, &c__18, dotype, iseed, &thresh, &c__6, a, &
			c__132, &dr[264], &dr[396], &dr[528], &dr[924], &dr[
			1056], &dr[1188], &a[17424], &c__132, &a[34848], dc, &
			a[52272], work, &c__89760, rwork, &c__89760, iwork, &
			c__20064, result, &info);
		if (info != 0) {
		    s_wsfe(&io___209);
		    do_fio(&c__1, "CDRVST", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
/* L290: */
	}

    } else if (lsamen_(&c__3, c3, "CSG")) {

/*        ---------------------------------------------- */
/*        CSG:  Hermitian Generalized Eigenvalue Problem */
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
	    s_wsfe(&io___210);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    if (tstchk) {
		cdrvsg_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, 
			&c__132, &a[17424], &c__132, &dr[264], &a[34848], &
			c__132, &a[52272], &a[69696], &a[87120], &a[104544], 
			work, &c__89760, rwork, &c__89760, iwork, &c__20064, 
			result, &info);
		if (info != 0) {
		    s_wsfe(&io___211);
		    do_fio(&c__1, "CDRVSG", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
/* L310: */
	}

    } else if (lsamen_(&c__3, c3, "CBD") || lsamen_(&
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
	xlaenv_(&c__9, &c__25);

/*        Test the error exits */

	xlaenv_(&c__1, &c__1);
	if (tsterr && tstchk) {
	    cerrbd_("CBD", &c__6);
	}
	if (tsterr && tstdrv) {
	    cerred_("CBD", &c__6);
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
	    s_wsfe(&io___213);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nxval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(integer));
	    e_wsfe();
	    if (tstchk) {
		cchkbd_(&nn, mval, nval, &maxtyp, dotype, &nrhs, iseed, &
			thresh, a, &c__132, dr, &dr[132], &dr[264], &dr[396], 
			&a[17424], &c__132, &a[34848], &a[52272], &a[69696], &
			c__132, &a[87120], &c__132, &a[104544], &a[121968], 
			work, &c__89760, rwork, &c__6, &info);
		if (info != 0) {
		    s_wsfe(&io___214);
		    do_fio(&c__1, "CCHKBD", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
	    if (tstdrv) {
		cdrvbd_(&nn, mval, nval, &maxtyp, dotype, iseed, &thresh, a, &
			c__132, &a[17424], &c__132, &a[34848], &c__132, &a[
			52272], &a[69696], &a[87120], dr, &dr[132], &dr[264], 
			work, &c__89760, rwork, iwork, &c__6, &info);
	    }
/* L330: */
	}

    } else if (lsamen_(&c__3, c3, "CEV")) {

/*        -------------------------------------------- */
/*        CEV:  Nonsymmetric Eigenvalue Problem Driver */
/*              CGEEV (eigenvalues and eigenvectors) */
/*        -------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___215);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    cdrvev_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], dc, &dc[132], &a[34848], &c__132, &a[
		    52272], &c__132, &a[69696], &c__132, result, work, &
		    c__89760, rwork, iwork, &info);
	    if (info != 0) {
		s_wsfe(&io___216);
		do_fio(&c__1, "CGEEV", (ftnlen)5);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___217);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "CES")) {

/*        -------------------------------------------- */
/*        CES:  Nonsymmetric Eigenvalue Problem Driver */
/*              CGEES (Schur form) */
/*        -------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___218);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    cdrves_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], dc, &dc[132], &a[52272], &
		    c__132, result, work, &c__89760, rwork, iwork, logwrk, &
		    info);
	    if (info != 0) {
		s_wsfe(&io___219);
		do_fio(&c__1, "CGEES", (ftnlen)5);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___220);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "CVX")) {

/*        -------------------------------------------------------------- */
/*        CVX:  Nonsymmetric Eigenvalue Problem Expert Driver */
/*              CGEEVX (eigenvalues, eigenvectors and condition numbers) */
/*        -------------------------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes < 0) {
	    s_wsfe(&io___221);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    cdrvvx_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__5, &c__6, 
		    a, &c__132, &a[17424], dc, &dc[132], &a[34848], &c__132, &
		    a[52272], &c__132, &a[69696], &c__132, dr, &dr[132], &dr[
		    264], &dr[396], &dr[528], &dr[660], &dr[792], &dr[924], 
		    result, work, &c__89760, rwork, &info);
	    if (info != 0) {
		s_wsfe(&io___222);
		do_fio(&c__1, "CGEEVX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___223);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "CSX")) {

/*        --------------------------------------------------- */
/*        CSX:  Nonsymmetric Eigenvalue Problem Expert Driver */
/*              CGEESX (Schur form and condition numbers) */
/*        --------------------------------------------------- */

	maxtyp = 21;
	ntypes = min(maxtyp,ntypes);
	if (ntypes < 0) {
	    s_wsfe(&io___224);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerred_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    cdrvsx_(&nn, nval, &ntypes, dotype, iseed, &thresh, &c__5, &c__6, 
		    a, &c__132, &a[17424], &a[34848], dc, &dc[132], &dc[264], 
		    &a[52272], &c__132, &a[69696], result, work, &c__89760, 
		    rwork, logwrk, &info);
	    if (info != 0) {
		s_wsfe(&io___225);
		do_fio(&c__1, "CGEESX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___226);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "CGG")) {

/*        ------------------------------------------------- */
/*        CGG:  Generalized Nonsymmetric Eigenvalue Problem */
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
	    cerrgg_(c3, &c__6);
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
	    s_wsfe(&io___227);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbmin[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nsval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&mxbval[i__ - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nbcol[i__ - 1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    tstdif = FALSE_;
	    thrshn = 10.f;
	    if (tstchk) {
		cchkgg_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &tstdif, &
			thrshn, &c__6, a, &c__132, &a[17424], &a[34848], &a[
			52272], &a[69696], &a[87120], &a[104544], &a[121968], 
			&a[139392], &c__132, &a[156816], &a[174240], &a[
			191664], dc, &dc[132], &dc[264], &dc[396], &a[209088], 
			 &a[226512], work, &c__89760, rwork, logwrk, result, &
			info);
		if (info != 0) {
		    s_wsfe(&io___230);
		    do_fio(&c__1, "CCHKGG", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
	    xlaenv_(&c__1, &c__1);
	    if (tstdrv) {
		cdrvgg_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &thrshn, &
			c__6, a, &c__132, &a[17424], &a[34848], &a[52272], &a[
			69696], &a[87120], &a[104544], &c__132, &a[121968], 
			dc, &dc[132], &dc[264], &dc[396], &a[121968], &a[
			139392], work, &c__89760, rwork, result, &info);
		if (info != 0) {
		    s_wsfe(&io___231);
		    do_fio(&c__1, "CDRVGG", (ftnlen)6);
		    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		    e_wsfe();
		}
	    }
/* L350: */
	}

    } else if (lsamen_(&c__3, c3, "CGS")) {

/*        ------------------------------------------------- */
/*        CGS:  Generalized Nonsymmetric Eigenvalue Problem */
/*              CGGES (Schur form) */
/*        ------------------------------------------------- */

	maxtyp = 26;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___232);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    cdrges_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], &a[52272], &a[104544], &
		    c__132, &a[121968], dc, &dc[132], work, &c__89760, rwork, 
		    result, logwrk, &info);

	    if (info != 0) {
		s_wsfe(&io___233);
		do_fio(&c__1, "CDRGES", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___234);
	e_wsfe();
	goto L10;

    } else if (cgx) {

/*        ------------------------------------------------- */
/*        CGX  Generalized Nonsymmetric Eigenvalue Problem */
/*              CGGESX (Schur form and condition numbers) */
/*        ------------------------------------------------- */

	maxtyp = 5;
	ntypes = maxtyp;
	if (nn < 0) {
	    s_wsfe(&io___235);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    xlaenv_(&c__5, &c__2);
	    cdrgsx_(&nn, &c__20, &thresh, &c__5, &c__6, a, &c__132, &a[17424], 
		     &a[34848], &a[52272], &a[69696], &a[87120], dc, &dc[132], 
		     c__, &c__400, s, work, &c__89760, rwork, iwork, &
		    c__20064, logwrk, &info);
	    if (info != 0) {
		s_wsfe(&io___238);
		do_fio(&c__1, "CDRGSX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___239);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "CGV")) {

/*        ------------------------------------------------- */
/*        CGV:  Generalized Nonsymmetric Eigenvalue Problem */
/*              CGGEV (Eigenvalue/vector form) */
/*        ------------------------------------------------- */

	maxtyp = 26;
	ntypes = min(maxtyp,ntypes);
	if (ntypes <= 0) {
	    s_wsfe(&io___240);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    cdrgev_(&nn, nval, &maxtyp, dotype, iseed, &thresh, &c__6, a, &
		    c__132, &a[17424], &a[34848], &a[52272], &a[104544], &
		    c__132, &a[121968], &a[139392], &c__132, dc, &dc[132], &
		    dc[264], &dc[396], work, &c__89760, rwork, result, &info);
	    if (info != 0) {
		s_wsfe(&io___241);
		do_fio(&c__1, "CDRGEV", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___242);
	e_wsfe();
	goto L10;

    } else if (cxv) {

/*        ------------------------------------------------- */
/*        CXV:  Generalized Nonsymmetric Eigenvalue Problem */
/*              CGGEVX (eigenvalue/vector with condition numbers) */
/*        ------------------------------------------------- */

	maxtyp = 2;
	ntypes = maxtyp;
	if (nn < 0) {
	    s_wsfe(&io___243);
	    do_fio(&c__1, c3, (ftnlen)3);
	    e_wsfe();
	} else {
	    if (tsterr) {
		cerrgg_(c3, &c__6);
	    }
	    alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	    cdrgvx_(&nn, &thresh, &c__5, &c__6, a, &c__132, &a[17424], &a[
		    34848], &a[52272], dc, &dc[132], &a[69696], &a[87120], 
		    iwork, &iwork[1], dr, &dr[132], &dr[264], &dr[396], &dr[
		    528], &dr[660], work, &c__89760, rwork, &iwork[2], &
		    c__20062, result, logwrk, &info);

	    if (info != 0) {
		s_wsfe(&io___244);
		do_fio(&c__1, "CDRGVX", (ftnlen)6);
		do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	}
	s_wsfe(&io___245);
	e_wsfe();
	goto L10;

    } else if (lsamen_(&c__3, c3, "CHB")) {

/*        ------------------------------ */
/*        CHB:  Hermitian Band Reduction */
/*        ------------------------------ */

	maxtyp = 15;
	ntypes = min(maxtyp,ntypes);
	alareq_(c3, &ntypes, dotype, &maxtyp, &c__5, &c__6);
	if (tsterr) {
	    cerrst_("CHB", &c__6);
	}
	cchkhb_(&nn, nval, &nk, kval, &maxtyp, dotype, iseed, &thresh, &c__6, 
		a, &c__132, dr, &dr[132], &a[17424], &c__132, work, &c__89760, 
		 rwork, result, &info);
	if (info != 0) {
	    s_wsfe(&io___246);
	    do_fio(&c__1, "CCHKHB", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "CBB")) {

/*        ------------------------------ */
/*        CBB:  General Band Reduction */
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
	    s_wsfe(&io___247);
	    do_fio(&c__1, c3, (ftnlen)3);
	    do_fio(&c__1, (char *)&nrhs, (ftnlen)sizeof(integer));
	    e_wsfe();
	    cchkbb_(&nn, mval, nval, &nk, kval, &maxtyp, dotype, &nrhs, iseed, 
		     &thresh, &c__6, a, &c__132, &a[17424], &c__264, dr, &dr[
		    132], &a[52272], &c__132, &a[69696], &c__132, &a[87120], &
		    c__132, &a[104544], work, &c__89760, rwork, result, &info)
		    ;
	    if (info != 0) {
		s_wsfe(&io___248);
		do_fio(&c__1, "CCHKBB", (ftnlen)6);
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
	    cerrgg_("GLM", &c__6);
	}
	cckglm_(&nn, nval, mval, pval, &ntypes, iseed, &thresh, &c__132, a, &
		a[17424], b, &b[17424], x, work, dr, &c__5, &c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___251);
	    do_fio(&c__1, "CCKGLM", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "GQR")) {

/*        ------------------------------------------ */
/*        GQR:  Generalized QR and RQ factorizations */
/*        ------------------------------------------ */

	xlaenv_(&c__1, &c__1);
	if (tsterr) {
	    cerrgg_("GQR", &c__6);
	}
	cckgqr_(&nn, mval, &nn, pval, &nn, nval, &ntypes, iseed, &thresh, &
		c__132, a, &a[17424], &a[34848], &a[52272], taua, b, &b[17424]
, &b[34848], &b[52272], &b[69696], taub, work, dr, &c__5, &
		c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___254);
	    do_fio(&c__1, "CCKGQR", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "GSV")) {

/*        ---------------------------------------------- */
/*        GSV:  Generalized Singular Value Decomposition */
/*        ---------------------------------------------- */

	if (tsterr) {
	    cerrgg_("GSV", &c__6);
	}
	cckgsv_(&nn, mval, pval, nval, &ntypes, iseed, &thresh, &c__132, a, &
		a[17424], b, &b[17424], &a[34848], &b[34848], &a[52272], 
		alpha, beta, &b[52272], iwork, work, dr, &c__5, &c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___257);
	    do_fio(&c__1, "CCKGSV", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}

    } else if (lsamen_(&c__3, c3, "LSE")) {

/*        -------------------------------------- */
/*        LSE:  Constrained Linear Least Squares */
/*        -------------------------------------- */

	xlaenv_(&c__1, &c__1);
	if (tsterr) {
	    cerrgg_("LSE", &c__6);
	}
	ccklse_(&nn, mval, pval, nval, &ntypes, iseed, &thresh, &c__132, a, &
		a[17424], b, &b[17424], x, work, dr, &c__5, &c__6, &info);
	if (info != 0) {
	    s_wsfe(&io___258);
	    do_fio(&c__1, "CCKLSE", (ftnlen)6);
	    do_fio(&c__1, (char *)&info, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
    } else {
	s_wsle(&io___259);
	e_wsle();
	s_wsle(&io___260);
	e_wsle();
	s_wsfe(&io___261);
	do_fio(&c__1, c3, (ftnlen)3);
	e_wsfe();
    }
    if (! (cgx || cxv)) {
	goto L190;
    }
L380:
    s_wsfe(&io___262);
    e_wsfe();
    s2 = second_();
    s_wsfe(&io___264);
    r__1 = s2 - s1;
    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
    e_wsfe();

/* L9998: */

/*     End of CCHKEE */

    return 0;
} /* MAIN__ */

/* Main program alias */ int cchkee_ () { MAIN__ (); return 0; }
