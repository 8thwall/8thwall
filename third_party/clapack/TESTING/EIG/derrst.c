#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer infot, nout;
    logical ok, lerr;
} infoc_;

#define infoc_1 infoc_

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static integer c__2 = 2;
static integer c__0 = 0;
static integer c__1 = 1;
static integer c_n1 = -1;
static integer c__3 = 3;
static doublereal c_b220 = 0.;
static doublereal c_b221 = 1.;
static integer c__23 = 23;
static integer c__28 = 28;
static integer c__12 = 12;
static integer c__19 = 19;
static integer c__11 = 11;
static integer c__4 = 4;
static integer c__20 = 20;
static integer c__5 = 5;
static integer c__27 = 27;
static integer c__16 = 16;
static integer c__8 = 8;
static integer c__25 = 25;
static integer c__18 = 18;

/* Subroutine */ int derrst_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002,\002 (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublereal a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */, d__[
	    3], e[3];
    integer i__, j, m, n;
    doublereal q[9]	/* was [3][3] */, r__[3], w[60], x[3], z__[9]	/* 
	    was [3][3] */;
    char c2[2];
    integer i1[3], i2[3], i3[3], iw[36], nt;
    doublereal tau[3];
    integer info;
    extern /* Subroutine */ int dsbev_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *), dspev_(char *, char *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *), dstev_(char *, integer *
, doublereal *, doublereal *, doublereal *, integer *, doublereal 
	    *, integer *), dsyev_(char *, char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *), dstedc_(char *, integer *, doublereal 
	    *, doublereal *, doublereal *, integer *, doublereal *, integer *, 
	     integer *, integer *, integer *), dsbevd_(char *, char *, 
	     integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int dsbtrd_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, doublereal *, integer *), chkxer_(
	    char *, integer *, integer *, logical *, logical *), 
	    dspevd_(char *, char *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *), dstein_(integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, integer *), dsterf_(integer *, doublereal *, 
	    doublereal *, integer *), dstevd_(char *, integer *, doublereal *, 
	     doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *), dsbevx_(char *, char *, 
	    char *, integer *, integer *, doublereal *, integer *, doublereal 
	    *, integer *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *), dstebz_(char *, char *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, integer *), 
	    dsyevd_(char *, char *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, integer *, 
	    integer *), dopgtr_(char *, integer *, doublereal 
	    *, doublereal *, doublereal *, integer *, doublereal *, integer *), dpteqr_(char *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dorgtr_(char *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, integer *), dsptrd_(char *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     integer *), dsteqr_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), dopmtr_(char *, char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *), dormtr_(char *, char *, char 
	    *, integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), dstevr_(char *, char *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, integer *, integer 
	    *, integer *);
    integer nsplit;
    extern /* Subroutine */ int dspevx_(char *, char *, char *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *), dsytrd_(char *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dsyevr_(char *, char *, char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, integer *, integer 
	    *, integer *), dstevx_(char *, char *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *), dsyevx_(char *, char *, char *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERRST tests the error exits for DSYTRD, DORGTR, DORMTR, DSPTRD, */
/*  DOPGTR, DOPMTR, DSTEQR, SSTERF, SSTEBZ, SSTEIN, DPTEQR, DSBTRD, */
/*  DSYEV, SSYEVX, SSYEVD, DSBEV, SSBEVX, SSBEVD, */
/*  DSPEV, SSPEVX, SSPEVD, DSTEV, SSTEVX, SSTEVD, and SSTEDC. */

/*  Arguments */
/*  ========= */

/*  PATH    (input) CHARACTER*3 */
/*          The LAPACK path name for the routines to be tested. */

/*  NUNIT   (input) INTEGER */
/*          The unit number for output. */

/*  ===================================================================== */

/*     NMAX has to be at least 3 or LIW may be too small */
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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    infoc_1.nout = *nunit;
    io___1.ciunit = infoc_1.nout;
    s_wsle(&io___1);
    e_wsle();
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);

/*     Set the variables to innocuous values. */

    for (j = 1; j <= 3; ++j) {
	for (i__ = 1; i__ <= 3; ++i__) {
	    a[i__ + j * 3 - 4] = 1. / (doublereal) (i__ + j);
/* L10: */
	}
/* L20: */
    }
    for (j = 1; j <= 3; ++j) {
	d__[j - 1] = (doublereal) j;
	e[j - 1] = 0.;
	i1[j - 1] = j;
	i2[j - 1] = j;
	tau[j - 1] = 1.;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits for the ST path. */

    if (lsamen_(&c__2, c2, "ST")) {

/*        DSYTRD */

	s_copy(srnamc_1.srnamt, "DSYTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsytrd_("/", &c__0, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("DSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsytrd_("U", &c_n1, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("DSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsytrd_("U", &c__2, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("DSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dsytrd_("U", &c__0, a, &c__1, d__, e, tau, w, &c__0, &info)
		;
	chkxer_("DSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        DORGTR */

	s_copy(srnamc_1.srnamt, "DORGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dorgtr_("/", &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dorgtr_("U", &c_n1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dorgtr_("U", &c__2, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dorgtr_("U", &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("DORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        DORMTR */

	s_copy(srnamc_1.srnamt, "DORMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dormtr_("/", "U", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dormtr_("L", "/", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dormtr_("L", "U", "/", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dormtr_("L", "U", "N", &c_n1, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dormtr_("L", "U", "N", &c__0, &c_n1, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dormtr_("L", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dormtr_("R", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dormtr_("L", "U", "N", &c__2, &c__0, a, &c__2, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dormtr_("L", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dormtr_("R", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("DORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        DSPTRD */

	s_copy(srnamc_1.srnamt, "DSPTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsptrd_("/", &c__0, a, d__, e, tau, &info);
	chkxer_("DSPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsptrd_("U", &c_n1, a, d__, e, tau, &info);
	chkxer_("DSPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 2;

/*        DOPGTR */

	s_copy(srnamc_1.srnamt, "DOPGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dopgtr_("/", &c__0, a, tau, z__, &c__1, w, &info);
	chkxer_("DOPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dopgtr_("U", &c_n1, a, tau, z__, &c__1, w, &info);
	chkxer_("DOPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dopgtr_("U", &c__2, a, tau, z__, &c__1, w, &info);
	chkxer_("DOPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        DOPMTR */

	s_copy(srnamc_1.srnamt, "DOPMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dopmtr_("/", "U", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("DOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dopmtr_("L", "/", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("DOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dopmtr_("L", "U", "/", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("DOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dopmtr_("L", "U", "N", &c_n1, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("DOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dopmtr_("L", "U", "N", &c__0, &c_n1, a, tau, c__, &c__1, w, &info);
	chkxer_("DOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dopmtr_("L", "U", "N", &c__2, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("DOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        DPTEQR */

	s_copy(srnamc_1.srnamt, "DPTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpteqr_("/", &c__0, d__, e, z__, &c__1, w, &info);
	chkxer_("DPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpteqr_("N", &c_n1, d__, e, z__, &c__1, w, &info);
	chkxer_("DPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dpteqr_("V", &c__2, d__, e, z__, &c__1, w, &info);
	chkxer_("DPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        DSTEBZ */

	s_copy(srnamc_1.srnamt, "DSTEBZ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dstebz_("/", "E", &c__0, &c_b220, &c_b221, &c__1, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dstebz_("A", "/", &c__0, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dstebz_("A", "E", &c_n1, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dstebz_("V", "E", &c__0, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dstebz_("I", "E", &c__0, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dstebz_("I", "E", &c__1, &c_b220, &c_b220, &c__2, &c__1, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dstebz_("I", "E", &c__1, &c_b220, &c_b220, &c__1, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dstebz_("I", "E", &c__1, &c_b220, &c_b220, &c__1, &c__2, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("DSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        DSTEIN */

	s_copy(srnamc_1.srnamt, "DSTEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dstein_(&c_n1, d__, e, &c__0, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("DSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dstein_(&c__0, d__, e, &c_n1, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("DSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dstein_(&c__0, d__, e, &c__1, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("DSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dstein_(&c__2, d__, e, &c__0, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("DSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        DSTEQR */

	s_copy(srnamc_1.srnamt, "DSTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsteqr_("/", &c__0, d__, e, z__, &c__1, w, &info);
	chkxer_("DSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsteqr_("N", &c_n1, d__, e, z__, &c__1, w, &info);
	chkxer_("DSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dsteqr_("V", &c__2, d__, e, z__, &c__1, w, &info);
	chkxer_("DSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        DSTERF */

	s_copy(srnamc_1.srnamt, "DSTERF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsterf_(&c_n1, d__, e, &info);
	chkxer_("DSTERF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	++nt;

/*        DSTEDC */

	s_copy(srnamc_1.srnamt, "DSTEDC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dstedc_("/", &c__0, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dstedc_("N", &c_n1, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dstedc_("V", &c__2, d__, e, z__, &c__1, w, &c__23, iw, &c__28, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dstedc_("N", &c__1, d__, e, z__, &c__1, w, &c__0, iw, &c__1, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__0, iw, &c__12, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__0, iw, &c__28, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dstedc_("N", &c__1, d__, e, z__, &c__1, w, &c__1, iw, &c__0, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__19, iw, &c__0, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__23, iw, &c__0, &info);
	chkxer_("DSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        DSTEVD */

	s_copy(srnamc_1.srnamt, "DSTEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dstevd_("/", &c__0, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("DSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dstevd_("N", &c_n1, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("DSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dstevd_("V", &c__2, d__, e, z__, &c__1, w, &c__19, iw, &c__12, &info);
	chkxer_("DSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dstevd_("N", &c__1, d__, e, z__, &c__1, w, &c__0, iw, &c__1, &info);
	chkxer_("DSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dstevd_("V", &c__2, d__, e, z__, &c__2, w, &c__12, iw, &c__12, &info);
	chkxer_("DSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dstevd_("N", &c__0, d__, e, z__, &c__1, w, &c__1, iw, &c__0, &info);
	chkxer_("DSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dstevd_("V", &c__2, d__, e, z__, &c__2, w, &c__19, iw, &c__11, &info);
	chkxer_("DSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        DSTEV */

	s_copy(srnamc_1.srnamt, "DSTEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dstev_("/", &c__0, d__, e, z__, &c__1, w, &info);
	chkxer_("DSTEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dstev_("N", &c_n1, d__, e, z__, &c__1, w, &info);
	chkxer_("DSTEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dstev_("V", &c__2, d__, e, z__, &c__1, w, &info);
	chkxer_("DSTEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        DSTEVX */

	s_copy(srnamc_1.srnamt, "DSTEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dstevx_("/", "A", &c__0, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dstevx_("N", "/", &c__0, d__, e, &c_b220, &c_b221, &c__1, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dstevx_("N", "A", &c_n1, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dstevx_("N", "V", &c__1, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dstevx_("N", "I", &c__1, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dstevx_("N", "I", &c__1, d__, e, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dstevx_("N", "I", &c__2, d__, e, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dstevx_("N", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__2, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dstevx_("V", "A", &c__2, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        DSTEVR */

	n = 1;
	s_copy(srnamc_1.srnamt, "DSTEVR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	i__1 = n * 20;
	i__2 = n * 10;
	dstevr_("/", "A", &c__0, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	i__1 = n * 20;
	i__2 = n * 10;
	dstevr_("V", "/", &c__0, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	i__1 = n * 20;
	i__2 = n * 10;
	dstevr_("V", "A", &c_n1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	i__1 = n * 20;
	i__2 = n * 10;
	dstevr_("V", "V", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = n * 20;
	i__2 = n * 10;
	dstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__0, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	n = 2;
	i__1 = n * 20;
	i__2 = n * 10;
	dstevr_("V", "I", &c__2, d__, e, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	n = 1;
	i__1 = n * 20;
	i__2 = n * 10;
	dstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, w, z__, &c__0, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	i__1 = n * 20 - 1;
	i__2 = n * 10;
	dstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 19;
	i__1 = n * 20;
	i__2 = n * 10 - 1;
	dstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("DSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        DSYEVD */

	s_copy(srnamc_1.srnamt, "DSYEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsyevd_("/", "U", &c__0, a, &c__1, x, w, &c__1, iw, &c__1, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsyevd_("N", "/", &c__0, a, &c__1, x, w, &c__1, iw, &c__1, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsyevd_("N", "U", &c_n1, a, &c__1, x, w, &c__1, iw, &c__1, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dsyevd_("N", "U", &c__2, a, &c__1, x, w, &c__3, iw, &c__1, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsyevd_("N", "U", &c__1, a, &c__1, x, w, &c__0, iw, &c__1, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsyevd_("N", "U", &c__2, a, &c__2, x, w, &c__4, iw, &c__1, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsyevd_("V", "U", &c__2, a, &c__2, x, w, &c__20, iw, &c__12, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsyevd_("N", "U", &c__1, a, &c__1, x, w, &c__1, iw, &c__0, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsyevd_("N", "U", &c__2, a, &c__2, x, w, &c__5, iw, &c__0, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsyevd_("V", "U", &c__2, a, &c__2, x, w, &c__27, iw, &c__11, &info);
	chkxer_("DSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        DSYEVR */

	s_copy(srnamc_1.srnamt, "DSYEVR", (ftnlen)6, (ftnlen)6);
	n = 1;
	infoc_1.infot = 1;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("/", "A", "U", &c__0, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "/", "U", &c__0, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "A", "/", &c_n1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "A", "U", &c_n1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "A", "U", &c__2, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "V", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;

	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "I", "U", &c__2, a, &c__2, &c_b220, &c_b220, &c__2, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	i__1 = n * 26;
	i__2 = n * 10;
	dsyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__0, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	i__1 = n * 26 - 1;
	i__2 = n * 10;
	dsyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	i__1 = n * 26;
	i__2 = n * 10 - 1;
	dsyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("DSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        DSYEV */

	s_copy(srnamc_1.srnamt, "DSYEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsyev_("/", "U", &c__0, a, &c__1, x, w, &c__1, &info);
	chkxer_("DSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsyev_("N", "/", &c__0, a, &c__1, x, w, &c__1, &info);
	chkxer_("DSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsyev_("N", "U", &c_n1, a, &c__1, x, w, &c__1, &info);
	chkxer_("DSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dsyev_("N", "U", &c__2, a, &c__1, x, w, &c__3, &info);
	chkxer_("DSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsyev_("N", "U", &c__1, a, &c__1, x, w, &c__1, &info);
	chkxer_("DSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 5;

/*        DSYEVX */

	s_copy(srnamc_1.srnamt, "DSYEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsyevx_("/", "A", "U", &c__0, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsyevx_("N", "/", "U", &c__0, a, &c__1, &c_b220, &c_b221, &c__1, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsyevx_("N", "A", "/", &c__0, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	infoc_1.infot = 4;
	dsyevx_("N", "A", "U", &c_n1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dsyevx_("N", "A", "U", &c__2, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__16, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsyevx_("N", "V", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dsyevx_("N", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dsyevx_("N", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__2, &
		c__1, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsyevx_("N", "I", "U", &c__2, a, &c__2, &c_b220, &c_b220, &c__2, &
		c__1, &c_b220, &m, x, z__, &c__1, w, &c__16, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsyevx_("N", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__2, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dsyevx_("V", "A", "U", &c__2, a, &c__2, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__16, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	dsyevx_("V", "A", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__0, iw, i3, &info);
	chkxer_("DSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        DSPEVD */

	s_copy(srnamc_1.srnamt, "DSPEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dspevd_("/", "U", &c__0, a, x, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dspevd_("N", "/", &c__0, a, x, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dspevd_("N", "U", &c_n1, a, x, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dspevd_("V", "U", &c__2, a, x, z__, &c__1, w, &c__23, iw, &c__12, &
		info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dspevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__0, iw, &c__1, &info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dspevd_("N", "U", &c__2, a, x, z__, &c__1, w, &c__3, iw, &c__1, &info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dspevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__16, iw, &c__12, &
		info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dspevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__1, iw, &c__0, &info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dspevd_("N", "U", &c__2, a, x, z__, &c__1, w, &c__4, iw, &c__0, &info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dspevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__23, iw, &c__11, &
		info);
	chkxer_("DSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        DSPEV */

	s_copy(srnamc_1.srnamt, "DSPEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dspev_("/", "U", &c__0, a, w, z__, &c__1, x, &info);
	chkxer_("DSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dspev_("N", "/", &c__0, a, w, z__, &c__1, x, &info);
	chkxer_("DSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dspev_("N", "U", &c_n1, a, w, z__, &c__1, x, &info);
	chkxer_("DSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dspev_("V", "U", &c__2, a, w, z__, &c__1, x, &info);
	chkxer_("DSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        DSPEVX */

	s_copy(srnamc_1.srnamt, "DSPEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dspevx_("/", "A", "U", &c__0, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dspevx_("N", "/", "U", &c__0, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dspevx_("N", "A", "/", &c__0, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	infoc_1.infot = 4;
	dspevx_("N", "A", "U", &c_n1, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dspevx_("N", "V", "U", &c__1, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dspevx_("N", "I", "U", &c__1, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dspevx_("N", "I", "U", &c__1, a, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dspevx_("N", "I", "U", &c__2, a, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dspevx_("N", "I", "U", &c__1, a, &c_b220, &c_b220, &c__1, &c__2, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dspevx_("V", "A", "U", &c__2, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("DSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*     Test error exits for the SB path. */

    } else if (lsamen_(&c__2, c2, "SB")) {

/*        DSBTRD */

	s_copy(srnamc_1.srnamt, "DSBTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsbtrd_("/", "U", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("DSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsbtrd_("N", "/", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("DSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsbtrd_("N", "U", &c_n1, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("DSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsbtrd_("N", "U", &c__0, &c_n1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("DSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dsbtrd_("N", "U", &c__1, &c__1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("DSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsbtrd_("V", "U", &c__2, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("DSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        DSBEVD */

	s_copy(srnamc_1.srnamt, "DSBEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsbevd_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsbevd_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsbevd_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsbevd_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dsbevd_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, &c__4, iw, 
		 &c__1, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dsbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__1, w, &c__25, 
		iw, &c__12, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dsbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__0, iw, 
		 &c__1, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dsbevd_("N", "U", &c__2, &c__0, a, &c__1, x, z__, &c__1, w, &c__3, iw, 
		 &c__1, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dsbevd_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__2, w, &c__18, 
		iw, &c__12, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dsbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__0, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dsbevd_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__2, w, &c__25, 
		iw, &c__11, &info);
	chkxer_("DSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        DSBEV */

	s_copy(srnamc_1.srnamt, "DSBEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsbev_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("DSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsbev_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("DSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsbev_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("DSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsbev_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("DSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dsbev_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("DSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dsbev_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("DSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        DSBEVX */

	s_copy(srnamc_1.srnamt, "DSBEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsbevx_("/", "A", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsbevx_("N", "/", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsbevx_("N", "A", "/", &c__0, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	infoc_1.infot = 4;
	dsbevx_("N", "A", "U", &c_n1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dsbevx_("N", "A", "U", &c__0, &c_n1, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dsbevx_("N", "A", "U", &c__2, &c__1, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dsbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__2, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dsbevx_("N", "V", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dsbevx_("N", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dsbevx_("N", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__2, &c__1, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dsbevx_("N", "I", "U", &c__2, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__2, &c__1, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dsbevx_("N", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__1, &c__2, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dsbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__2, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("DSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___24.ciunit = infoc_1.nout;
	s_wsfe(&io___24);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___25.ciunit = infoc_1.nout;
	s_wsfe(&io___25);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of DERRST */

} /* derrst_ */
