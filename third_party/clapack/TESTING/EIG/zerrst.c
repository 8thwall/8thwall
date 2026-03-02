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
static integer c__4 = 4;
static integer c__23 = 23;
static integer c__28 = 28;
static integer c__12 = 12;
static integer c__25 = 25;
static integer c__8 = 8;
static integer c__18 = 18;
static integer c__11 = 11;
static doublereal c_b458 = 0.;
static doublereal c_b472 = 1.;

/* Subroutine */ int zerrst_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002,\002 (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1, i__2, i__3;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublecomplex a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */;
    doublereal d__[3], e[3];
    integer i__, j, m, n;
    doublecomplex q[9]	/* was [3][3] */;
    doublereal r__[60];
    doublecomplex w[60];
    doublereal x[3];
    doublecomplex z__[9]	/* was [3][3] */;
    char c2[2];
    integer i1[3], i2[3], i3[3], iw[36], nt;
    doublereal rw[60];
    doublecomplex tau[3];
    integer info;
    extern /* Subroutine */ int zhbev_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *, doublecomplex *, doublereal *, integer *), zheev_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *, doublecomplex *, integer *, doublereal *, 
	     integer *), zhpev_(char *, char *, integer *, 
	    doublecomplex *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, doublereal *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int zhbevd_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), zheevd_(char 
	    *, char *, integer *, doublecomplex *, integer *, doublereal *, 
	    doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *), zstedc_(char *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *), zhbtrd_(char *, char *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublecomplex *, integer *, doublecomplex *, integer *), zhetrd_(char *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublecomplex *, 
	    integer *, integer *), zhpevd_(char *, char *, integer *, 
	    doublecomplex *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *), zheevr_(char *, char *, 
	    char *, integer *, doublecomplex *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublecomplex *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *), zhbevx_(char *, 
	    char *, char *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, integer *, 
	     integer *, doublereal *, integer *, doublereal *, doublecomplex *
, integer *, doublecomplex *, doublereal *, integer *, integer *, 
	    integer *), zheevx_(char *, char *, char *
, integer *, doublecomplex *, integer *, doublereal *, doublereal 
	    *, integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, integer *, integer *, integer *), zhptrd_(char *, integer *, doublecomplex *, doublereal *, 
	     doublereal *, doublecomplex *, integer *), zstein_(
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, integer *, doublecomplex *, integer *, doublereal *, 
	    integer *, integer *, integer *), zhpevx_(char *, char *, char *, 
	    integer *, doublecomplex *, doublereal *, doublereal *, integer *, 
	     integer *, doublereal *, integer *, doublereal *, doublecomplex *
, integer *, doublecomplex *, doublereal *, integer *, integer *, 
	    integer *), zpteqr_(char *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, integer *, 
	    doublereal *, integer *), zsteqr_(char *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, integer *, 
	    doublereal *, integer *), zungtr_(char *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, integer *), zupgtr_(char *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zunmtr_(char *, char *, char 
	    *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, integer *), zupmtr_(char *, 
	    char *, char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *);

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

/*  ZERRST tests the error exits for ZHETRD, ZUNGTR, CUNMTR, ZHPTRD, */
/*  ZUNGTR, ZUPMTR, ZSTEQR, CSTEIN, ZPTEQR, ZHBTRD, */
/*  ZHEEV, CHEEVX, CHEEVD, ZHBEV, CHBEVX, CHBEVD, */
/*  ZHPEV, CHPEVX, CHPEVD, and ZSTEDC. */

/*  Arguments */
/*  ========= */

/*  PATH    (input) CHARACTER*3 */
/*          The LAPACK path name for the routines to be tested. */

/*  NUNIT   (input) INTEGER */
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
	    i__1 = i__ + j * 3 - 4;
	    d__1 = 1. / (doublereal) (i__ + j);
	    a[i__1].r = d__1, a[i__1].i = 0.;
/* L10: */
	}
/* L20: */
    }
    for (j = 1; j <= 3; ++j) {
	d__[j - 1] = (doublereal) j;
	e[j - 1] = 0.;
	i1[j - 1] = j;
	i2[j - 1] = j;
	i__1 = j - 1;
	tau[i__1].r = 1., tau[i__1].i = 0.;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits for the ST path. */

    if (lsamen_(&c__2, c2, "ST")) {

/*        ZHETRD */

	s_copy(srnamc_1.srnamt, "ZHETRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhetrd_("/", &c__0, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("ZHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhetrd_("U", &c_n1, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("ZHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhetrd_("U", &c__2, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("ZHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhetrd_("U", &c__0, a, &c__1, d__, e, tau, w, &c__0, &info)
		;
	chkxer_("ZHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        ZUNGTR */

	s_copy(srnamc_1.srnamt, "ZUNGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zungtr_("/", &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zungtr_("U", &c_n1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zungtr_("U", &c__2, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zungtr_("U", &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("ZUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        ZUNMTR */

	s_copy(srnamc_1.srnamt, "ZUNMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zunmtr_("/", "U", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zunmtr_("L", "/", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zunmtr_("L", "U", "/", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zunmtr_("L", "U", "N", &c_n1, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zunmtr_("L", "U", "N", &c__0, &c_n1, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zunmtr_("L", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zunmtr_("R", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zunmtr_("L", "U", "N", &c__2, &c__0, a, &c__2, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zunmtr_("L", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zunmtr_("R", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("ZUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        ZHPTRD */

	s_copy(srnamc_1.srnamt, "ZHPTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhptrd_("/", &c__0, a, d__, e, tau, &info);
	chkxer_("ZHPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhptrd_("U", &c_n1, a, d__, e, tau, &info);
	chkxer_("ZHPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 2;

/*        ZUPGTR */

	s_copy(srnamc_1.srnamt, "ZUPGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zupgtr_("/", &c__0, a, tau, z__, &c__1, w, &info);
	chkxer_("ZUPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zupgtr_("U", &c_n1, a, tau, z__, &c__1, w, &info);
	chkxer_("ZUPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zupgtr_("U", &c__2, a, tau, z__, &c__1, w, &info);
	chkxer_("ZUPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        ZUPMTR */

	s_copy(srnamc_1.srnamt, "ZUPMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zupmtr_("/", "U", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("ZUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zupmtr_("L", "/", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("ZUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zupmtr_("L", "U", "/", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("ZUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zupmtr_("L", "U", "N", &c_n1, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("ZUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zupmtr_("L", "U", "N", &c__0, &c_n1, a, tau, c__, &c__1, w, &info);
	chkxer_("ZUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zupmtr_("L", "U", "N", &c__2, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("ZUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        ZPTEQR */

	s_copy(srnamc_1.srnamt, "ZPTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpteqr_("/", &c__0, d__, e, z__, &c__1, rw, &info);
	chkxer_("ZPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpteqr_("N", &c_n1, d__, e, z__, &c__1, rw, &info);
	chkxer_("ZPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zpteqr_("V", &c__2, d__, e, z__, &c__1, rw, &info);
	chkxer_("ZPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        ZSTEIN */

	s_copy(srnamc_1.srnamt, "ZSTEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zstein_(&c_n1, d__, e, &c__0, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("ZSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zstein_(&c__0, d__, e, &c_n1, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("ZSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zstein_(&c__0, d__, e, &c__1, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("ZSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zstein_(&c__2, d__, e, &c__0, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("ZSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        ZSTEQR */

	s_copy(srnamc_1.srnamt, "ZSTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsteqr_("/", &c__0, d__, e, z__, &c__1, rw, &info);
	chkxer_("ZSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsteqr_("N", &c_n1, d__, e, z__, &c__1, rw, &info);
	chkxer_("ZSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zsteqr_("V", &c__2, d__, e, z__, &c__1, rw, &info);
	chkxer_("ZSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        ZSTEDC */

	s_copy(srnamc_1.srnamt, "ZSTEDC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zstedc_("/", &c__0, d__, e, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zstedc_("N", &c_n1, d__, e, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zstedc_("V", &c__2, d__, e, z__, &c__1, w, &c__4, rw, &c__23, iw, &
		c__28, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zstedc_("N", &c__2, d__, e, z__, &c__1, w, &c__0, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__0, rw, &c__23, iw, &
		c__28, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zstedc_("N", &c__2, d__, e, z__, &c__1, w, &c__1, rw, &c__0, iw, &
		c__1, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__1, rw, &c__1, iw, &
		c__12, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__4, rw, &c__1, iw, &
		c__28, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zstedc_("N", &c__2, d__, e, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__0, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__1, rw, &c__23, iw, &
		c__0, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__4, rw, &c__23, iw, &
		c__0, &info);
	chkxer_("ZSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        ZHEEVD */

	s_copy(srnamc_1.srnamt, "ZHEEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zheevd_("/", "U", &c__0, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zheevd_("N", "/", &c__0, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zheevd_("N", "U", &c_n1, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zheevd_("N", "U", &c__2, a, &c__1, x, w, &c__3, rw, &c__2, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zheevd_("N", "U", &c__1, a, &c__1, x, w, &c__0, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zheevd_("N", "U", &c__2, a, &c__2, x, w, &c__2, rw, &c__2, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zheevd_("V", "U", &c__2, a, &c__2, x, w, &c__3, rw, &c__25, iw, &
		c__12, &info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zheevd_("N", "U", &c__1, a, &c__1, x, w, &c__1, rw, &c__0, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zheevd_("N", "U", &c__2, a, &c__2, x, w, &c__3, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zheevd_("V", "U", &c__2, a, &c__2, x, w, &c__8, rw, &c__18, iw, &
		c__12, &info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zheevd_("N", "U", &c__1, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__0, 
		&info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zheevd_("V", "U", &c__2, a, &c__2, x, w, &c__8, rw, &c__25, iw, &
		c__11, &info);
	chkxer_("ZHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        ZHEEV */

	s_copy(srnamc_1.srnamt, "ZHEEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zheev_("/", "U", &c__0, a, &c__1, x, w, &c__1, rw, &info);
	chkxer_("ZHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zheev_("N", "/", &c__0, a, &c__1, x, w, &c__1, rw, &info);
	chkxer_("ZHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zheev_("N", "U", &c_n1, a, &c__1, x, w, &c__1, rw, &info);
	chkxer_("ZHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zheev_("N", "U", &c__2, a, &c__1, x, w, &c__3, rw, &info);
	chkxer_("ZHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zheev_("N", "U", &c__2, a, &c__2, x, w, &c__2, rw, &info);
	chkxer_("ZHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 5;

/*        ZHEEVX */

	s_copy(srnamc_1.srnamt, "ZHEEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zheevx_("/", "A", "U", &c__0, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zheevx_("V", "/", "U", &c__0, a, &c__1, &c_b458, &c_b472, &c__1, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zheevx_("V", "A", "/", &c__0, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	infoc_1.infot = 4;
	zheevx_("V", "A", "U", &c_n1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zheevx_("V", "A", "U", &c__2, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__2, w, &c__3, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zheevx_("V", "V", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zheevx_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zheevx_("V", "I", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__2, &
		c__1, &c_b458, &m, x, z__, &c__2, w, &c__3, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zheevx_("V", "A", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__3, rw, iw, i3, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	zheevx_("V", "A", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__2, w, &c__2, rw, iw, i1, &info);
	chkxer_("ZHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        ZHEEVR */

	s_copy(srnamc_1.srnamt, "ZHEEVR", (ftnlen)6, (ftnlen)6);
	n = 1;
	infoc_1.infot = 1;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("/", "A", "U", &c__0, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "/", "U", &c__0, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "A", "/", &c_n1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "A", "U", &c_n1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "A", "U", &c__2, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "V", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;

	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "I", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__2, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__0, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	i__1 = (n << 1) - 1;
	i__2 = n * 24;
	i__3 = n * 10;
	zheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	i__1 = n << 1;
	i__2 = n * 24 - 1;
	i__3 = n * 10;
	zheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[(n << 1) - 2], &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10 - 1;
	zheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, 
		iw, &i__3, &info);
	chkxer_("ZHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        ZHPEVD */

	s_copy(srnamc_1.srnamt, "ZHPEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhpevd_("/", "U", &c__0, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhpevd_("N", "/", &c__0, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhpevd_("N", "U", &c_n1, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhpevd_("V", "U", &c__2, a, x, z__, &c__1, w, &c__4, rw, &c__25, iw, &
		c__12, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhpevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__0, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhpevd_("N", "U", &c__2, a, x, z__, &c__2, w, &c__1, rw, &c__2, iw, &
		c__1, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhpevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__2, rw, &c__25, iw, &
		c__12, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhpevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__1, rw, &c__0, iw, &
		c__1, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhpevd_("N", "U", &c__2, a, x, z__, &c__2, w, &c__2, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhpevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__4, rw, &c__18, iw, &
		c__12, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhpevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__0, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhpevd_("N", "U", &c__2, a, x, z__, &c__2, w, &c__2, rw, &c__2, iw, &
		c__0, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhpevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__4, rw, &c__25, iw, &
		c__2, &info);
	chkxer_("ZHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        ZHPEV */

	s_copy(srnamc_1.srnamt, "ZHPEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhpev_("/", "U", &c__0, a, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhpev_("N", "/", &c__0, a, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhpev_("N", "U", &c_n1, a, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhpev_("V", "U", &c__2, a, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        ZHPEVX */

	s_copy(srnamc_1.srnamt, "ZHPEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhpevx_("/", "A", "U", &c__0, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhpevx_("V", "/", "U", &c__0, a, &c_b458, &c_b472, &c__1, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhpevx_("V", "A", "/", &c__0, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhpevx_("V", "A", "U", &c_n1, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhpevx_("V", "V", "U", &c__1, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zhpevx_("V", "I", "U", &c__1, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhpevx_("V", "I", "U", &c__2, a, &c_b458, &c_b458, &c__2, &c__1, &
		c_b458, &m, x, z__, &c__2, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zhpevx_("V", "A", "U", &c__2, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("ZHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the HB path. */

    } else if (lsamen_(&c__2, c2, "HB")) {

/*        ZHBTRD */

	s_copy(srnamc_1.srnamt, "ZHBTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhbtrd_("/", "U", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("ZHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhbtrd_("N", "/", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("ZHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhbtrd_("N", "U", &c_n1, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("ZHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhbtrd_("N", "U", &c__0, &c_n1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("ZHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zhbtrd_("N", "U", &c__1, &c__1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("ZHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zhbtrd_("V", "U", &c__2, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("ZHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        ZHBEVD */

	s_copy(srnamc_1.srnamt, "ZHBEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhbevd_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhbevd_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhbevd_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhbevd_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zhbevd_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, &c__2, rw, 
		 &c__2, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__1, w, &c__8, rw, 
		 &c__25, iw, &c__12, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__0, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhbevd_("N", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__1, rw, 
		 &c__2, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__2, rw, 
		 &c__25, iw, &c__12, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__0, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhbevd_("N", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__2, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__8, rw, 
		 &c__2, iw, &c__12, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zhbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__0, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zhbevd_("N", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__2, rw, 
		 &c__2, iw, &c__0, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zhbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__8, rw, 
		 &c__25, iw, &c__2, &info);
	chkxer_("ZHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 15;

/*        ZHBEV */

	s_copy(srnamc_1.srnamt, "ZHBEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhbev_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhbev_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhbev_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhbev_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zhbev_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhbev_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("ZHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        ZHBEVX */

	s_copy(srnamc_1.srnamt, "ZHBEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhbevx_("/", "A", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhbevx_("V", "/", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b472, &c__1, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhbevx_("V", "A", "/", &c__0, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	infoc_1.infot = 4;
	zhbevx_("V", "A", "U", &c_n1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zhbevx_("V", "A", "U", &c__0, &c_n1, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhbevx_("V", "A", "U", &c__2, &c__1, a, &c__1, q, &c__2, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__2, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__2, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhbevx_("V", "V", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zhbevx_("V", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhbevx_("V", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__1, &c__2, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	zhbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__2, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("ZHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;
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

/*     End of ZERRST */

} /* zerrst_ */
