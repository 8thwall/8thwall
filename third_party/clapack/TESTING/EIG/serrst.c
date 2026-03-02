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
static real c_b220 = 0.f;
static real c_b221 = 1.f;
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

/* Subroutine */ int serrst_(char *path, integer *nunit)
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
    real a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */, d__[3], e[3]
	    ;
    integer i__, j, m, n;
    real q[9]	/* was [3][3] */, r__[3], w[60], x[3], z__[9]	/* was [3][3] 
	    */;
    char c2[2];
    integer i1[3], i2[3], i3[3], iw[36], nt;
    real tau[3];
    integer info;
    extern /* Subroutine */ int ssbev_(char *, char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *, real *, integer *), sspev_(char *, char *, integer *, real *, real *, 
	     real *, integer *, real *, integer *), sstev_(
	    char *, integer *, real *, real *, real *, integer *, real *, 
	    integer *), ssyev_(char *, char *, integer *, real *, 
	    integer *, real *, real *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sstedc_(char *, integer *, real *, real *, 
	    real *, integer *, real *, integer *, integer *, integer *, 
	    integer *), ssbevd_(char *, char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *, real *, integer *, 
	    integer *, integer *, integer *), ssbtrd_(char *, 
	    char *, integer *, integer *, real *, integer *, real *, real *, 
	    real *, integer *, real *, integer *), sspevd_(
	    char *, char *, integer *, real *, real *, real *, integer *, 
	    real *, integer *, integer *, integer *, integer *), sstein_(integer *, real *, real *, integer *, real *, 
	    integer *, integer *, real *, integer *, real *, integer *, 
	    integer *, integer *), ssterf_(integer *, real *, real *, integer 
	    *), sstevd_(char *, integer *, real *, real *, real *, integer *, 
	    real *, integer *, integer *, integer *, integer *);
    integer nsplit;
    extern /* Subroutine */ int ssbevx_(char *, char *, char *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, real *, integer *, integer *, integer *)
	    , sstebz_(char *, char *, integer *, real *, real *, integer *, 
	    integer *, real *, real *, real *, integer *, integer *, real *, 
	    integer *, integer *, real *, integer *, integer *), ssyevd_(char *, char *, integer *, real *, integer *, 
	    real *, real *, integer *, integer *, integer *, integer *), sopgtr_(char *, integer *, real *, real *, real *
, integer *, real *, integer *), spteqr_(char *, integer *
, real *, real *, real *, integer *, real *, integer *), 
	    sorgtr_(char *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *), ssptrd_(char *, integer *, real *, 
	    real *, real *, real *, integer *), ssteqr_(char *, 
	    integer *, real *, real *, real *, integer *, real *, integer *), sopmtr_(char *, char *, char *, integer *, integer *, 
	    real *, real *, real *, integer *, real *, integer *), sormtr_(char *, char *, char *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *, integer *), sstevr_(char *, 
	    char *, integer *, real *, real *, real *, real *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, integer *
, real *, integer *, integer *, integer *, integer *), sspevx_(char *, char *, char *, integer *, real *, real *
, real *, integer *, integer *, real *, integer *, real *, real *, 
	     integer *, real *, integer *, integer *, integer *), ssytrd_(char *, integer *, real *, integer *, 
	    real *, real *, real *, real *, integer *, integer *), 
	    ssyevr_(char *, char *, char *, integer *, real *, integer *, 
	    real *, real *, integer *, integer *, real *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, integer *, 
	    integer *, integer *), sstevx_(char *, 
	    char *, integer *, real *, real *, real *, real *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *, integer *, integer *), ssyevx_(char *, 
	    char *, char *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, real *, integer *, integer *, integer *, integer *);

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

/*  SERRST tests the error exits for SSYTRD, SORGTR, SORMTR, SSPTRD, */
/*  SOPGTR, SOPMTR, SSTEQR, SSTERF, SSTEBZ, SSTEIN, SPTEQR, SSBTRD, */
/*  SSYEV, SSYEVX, SSYEVD, SSBEV, SSBEVX, SSBEVD, */
/*  SSPEV, SSPEVX, SSPEVD, SSTEV, SSTEVX, SSTEVD, and SSTEDC. */

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
	    a[i__ + j * 3 - 4] = 1.f / (real) (i__ + j);
/* L10: */
	}
/* L20: */
    }
    for (j = 1; j <= 3; ++j) {
	d__[j - 1] = (real) j;
	e[j - 1] = 0.f;
	i1[j - 1] = j;
	i2[j - 1] = j;
	tau[j - 1] = 1.f;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits for the ST path. */

    if (lsamen_(&c__2, c2, "ST")) {

/*        SSYTRD */

	s_copy(srnamc_1.srnamt, "SSYTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssytrd_("/", &c__0, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("SSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssytrd_("U", &c_n1, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("SSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssytrd_("U", &c__2, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("SSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ssytrd_("U", &c__0, a, &c__1, d__, e, tau, w, &c__0, &info)
		;
	chkxer_("SSYTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        SORGTR */

	s_copy(srnamc_1.srnamt, "SORGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sorgtr_("/", &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sorgtr_("U", &c_n1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sorgtr_("U", &c__2, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sorgtr_("U", &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("SORGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        SORMTR */

	s_copy(srnamc_1.srnamt, "SORMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sormtr_("/", "U", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sormtr_("L", "/", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sormtr_("L", "U", "/", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sormtr_("L", "U", "N", &c_n1, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sormtr_("L", "U", "N", &c__0, &c_n1, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sormtr_("L", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sormtr_("R", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sormtr_("L", "U", "N", &c__2, &c__0, a, &c__2, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sormtr_("L", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sormtr_("R", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("SORMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        SSPTRD */

	s_copy(srnamc_1.srnamt, "SSPTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssptrd_("/", &c__0, a, d__, e, tau, &info);
	chkxer_("SSPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssptrd_("U", &c_n1, a, d__, e, tau, &info);
	chkxer_("SSPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 2;

/*        SOPGTR */

	s_copy(srnamc_1.srnamt, "SOPGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sopgtr_("/", &c__0, a, tau, z__, &c__1, w, &info);
	chkxer_("SOPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sopgtr_("U", &c_n1, a, tau, z__, &c__1, w, &info);
	chkxer_("SOPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sopgtr_("U", &c__2, a, tau, z__, &c__1, w, &info);
	chkxer_("SOPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        SOPMTR */

	s_copy(srnamc_1.srnamt, "SOPMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sopmtr_("/", "U", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("SOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sopmtr_("L", "/", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("SOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sopmtr_("L", "U", "/", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("SOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sopmtr_("L", "U", "N", &c_n1, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("SOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sopmtr_("L", "U", "N", &c__0, &c_n1, a, tau, c__, &c__1, w, &info);
	chkxer_("SOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sopmtr_("L", "U", "N", &c__2, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("SOPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        SPTEQR */

	s_copy(srnamc_1.srnamt, "SPTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spteqr_("/", &c__0, d__, e, z__, &c__1, w, &info);
	chkxer_("SPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spteqr_("N", &c_n1, d__, e, z__, &c__1, w, &info);
	chkxer_("SPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	spteqr_("V", &c__2, d__, e, z__, &c__1, w, &info);
	chkxer_("SPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        SSTEBZ */

	s_copy(srnamc_1.srnamt, "SSTEBZ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sstebz_("/", "E", &c__0, &c_b220, &c_b221, &c__1, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sstebz_("A", "/", &c__0, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sstebz_("A", "E", &c_n1, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sstebz_("V", "E", &c__0, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sstebz_("I", "E", &c__0, &c_b220, &c_b220, &c__0, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sstebz_("I", "E", &c__1, &c_b220, &c_b220, &c__2, &c__1, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sstebz_("I", "E", &c__1, &c_b220, &c_b220, &c__1, &c__0, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sstebz_("I", "E", &c__1, &c_b220, &c_b220, &c__1, &c__2, &c_b220, d__, 
		 e, &m, &nsplit, x, i1, i2, w, iw, &info);
	chkxer_("SSTEBZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        SSTEIN */

	s_copy(srnamc_1.srnamt, "SSTEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sstein_(&c_n1, d__, e, &c__0, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("SSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sstein_(&c__0, d__, e, &c_n1, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("SSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sstein_(&c__0, d__, e, &c__1, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("SSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sstein_(&c__2, d__, e, &c__0, x, i1, i2, z__, &c__1, w, iw, i3, &info)
		;
	chkxer_("SSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        SSTEQR */

	s_copy(srnamc_1.srnamt, "SSTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssteqr_("/", &c__0, d__, e, z__, &c__1, w, &info);
	chkxer_("SSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssteqr_("N", &c_n1, d__, e, z__, &c__1, w, &info);
	chkxer_("SSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ssteqr_("V", &c__2, d__, e, z__, &c__1, w, &info);
	chkxer_("SSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        SSTERF */

	s_copy(srnamc_1.srnamt, "SSTERF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssterf_(&c_n1, d__, e, &info);
	chkxer_("SSTERF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	++nt;

/*        SSTEDC */

	s_copy(srnamc_1.srnamt, "SSTEDC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sstedc_("/", &c__0, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sstedc_("N", &c_n1, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sstedc_("V", &c__2, d__, e, z__, &c__1, w, &c__23, iw, &c__28, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sstedc_("N", &c__1, d__, e, z__, &c__1, w, &c__0, iw, &c__1, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__0, iw, &c__12, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__0, iw, &c__28, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sstedc_("N", &c__1, d__, e, z__, &c__1, w, &c__1, iw, &c__0, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__19, iw, &c__0, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__23, iw, &c__0, &info);
	chkxer_("SSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        SSTEVD */

	s_copy(srnamc_1.srnamt, "SSTEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sstevd_("/", &c__0, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("SSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sstevd_("N", &c_n1, d__, e, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("SSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sstevd_("V", &c__2, d__, e, z__, &c__1, w, &c__19, iw, &c__12, &info);
	chkxer_("SSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sstevd_("N", &c__1, d__, e, z__, &c__1, w, &c__0, iw, &c__1, &info);
	chkxer_("SSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sstevd_("V", &c__2, d__, e, z__, &c__2, w, &c__12, iw, &c__12, &info);
	chkxer_("SSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sstevd_("N", &c__0, d__, e, z__, &c__1, w, &c__1, iw, &c__0, &info);
	chkxer_("SSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sstevd_("V", &c__2, d__, e, z__, &c__2, w, &c__19, iw, &c__11, &info);
	chkxer_("SSTEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        SSTEV */

	s_copy(srnamc_1.srnamt, "SSTEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sstev_("/", &c__0, d__, e, z__, &c__1, w, &info);
	chkxer_("SSTEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sstev_("N", &c_n1, d__, e, z__, &c__1, w, &info);
	chkxer_("SSTEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sstev_("V", &c__2, d__, e, z__, &c__1, w, &info);
	chkxer_("SSTEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        SSTEVX */

	s_copy(srnamc_1.srnamt, "SSTEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sstevx_("/", "A", &c__0, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sstevx_("N", "/", &c__0, d__, e, &c_b220, &c_b221, &c__1, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sstevx_("N", "A", &c_n1, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sstevx_("N", "V", &c__1, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sstevx_("N", "I", &c__1, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sstevx_("N", "I", &c__1, d__, e, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sstevx_("N", "I", &c__2, d__, e, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sstevx_("N", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__2, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sstevx_("V", "A", &c__2, d__, e, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSTEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        SSTEVR */

	n = 1;
	s_copy(srnamc_1.srnamt, "SSTEVR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	i__1 = n * 20;
	i__2 = n * 10;
	sstevr_("/", "A", &c__0, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	i__1 = n * 20;
	i__2 = n * 10;
	sstevr_("V", "/", &c__0, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	i__1 = n * 20;
	i__2 = n * 10;
	sstevr_("V", "A", &c_n1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	i__1 = n * 20;
	i__2 = n * 10;
	sstevr_("V", "V", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, r__, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, 
		&info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = n * 20;
	i__2 = n * 10;
	sstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__0, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	n = 2;
	i__1 = n * 20;
	i__2 = n * 10;
	sstevr_("V", "I", &c__2, d__, e, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	n = 1;
	i__1 = n * 20;
	i__2 = n * 10;
	sstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, w, z__, &c__0, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	i__1 = n * 20 - 1;
	i__2 = n * 10;
	sstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 19;
	i__1 = n * 20;
	i__2 = n * 10 - 1;
	sstevr_("V", "I", &c__1, d__, e, &c_b220, &c_b220, &c__1, &c__1, &
		c_b220, &m, w, z__, &c__1, iw, x, &i__1, &iw[n * 2], &i__2, &
		info);
	chkxer_("SSTEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        SSYEVD */

	s_copy(srnamc_1.srnamt, "SSYEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssyevd_("/", "U", &c__0, a, &c__1, x, w, &c__1, iw, &c__1, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssyevd_("N", "/", &c__0, a, &c__1, x, w, &c__1, iw, &c__1, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssyevd_("N", "U", &c_n1, a, &c__1, x, w, &c__1, iw, &c__1, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ssyevd_("N", "U", &c__2, a, &c__1, x, w, &c__3, iw, &c__1, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssyevd_("N", "U", &c__1, a, &c__1, x, w, &c__0, iw, &c__1, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssyevd_("N", "U", &c__2, a, &c__2, x, w, &c__4, iw, &c__1, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssyevd_("V", "U", &c__2, a, &c__2, x, w, &c__20, iw, &c__12, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssyevd_("N", "U", &c__1, a, &c__1, x, w, &c__1, iw, &c__0, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssyevd_("N", "U", &c__2, a, &c__2, x, w, &c__5, iw, &c__0, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssyevd_("V", "U", &c__2, a, &c__2, x, w, &c__27, iw, &c__11, &info);
	chkxer_("SSYEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        SSYEVR */

	s_copy(srnamc_1.srnamt, "SSYEVR", (ftnlen)6, (ftnlen)6);
	n = 1;
	infoc_1.infot = 1;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("/", "A", "U", &c__0, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "/", "U", &c__0, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "A", "/", &c_n1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "A", "U", &c_n1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "A", "U", &c__2, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "V", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;

	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "I", "U", &c__2, a, &c__2, &c_b220, &c_b220, &c__2, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	i__1 = n * 26;
	i__2 = n * 10;
	ssyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__0, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	i__1 = n * 26 - 1;
	i__2 = n * 10;
	ssyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	i__1 = n * 26;
	i__2 = n * 10 - 1;
	ssyevr_("V", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__1, &c_b220, &m, r__, z__, &c__1, iw, q, &i__1, &iw[n * 2], 
		&i__2, &info);
	chkxer_("SSYEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        SSYEV */

	s_copy(srnamc_1.srnamt, "SSYEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssyev_("/", "U", &c__0, a, &c__1, x, w, &c__1, &info);
	chkxer_("SSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssyev_("N", "/", &c__0, a, &c__1, x, w, &c__1, &info);
	chkxer_("SSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssyev_("N", "U", &c_n1, a, &c__1, x, w, &c__1, &info);
	chkxer_("SSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ssyev_("N", "U", &c__2, a, &c__1, x, w, &c__3, &info);
	chkxer_("SSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssyev_("N", "U", &c__1, a, &c__1, x, w, &c__1, &info);
	chkxer_("SSYEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 5;

/*        SSYEVX */

	s_copy(srnamc_1.srnamt, "SSYEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssyevx_("/", "A", "U", &c__0, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssyevx_("N", "/", "U", &c__0, a, &c__1, &c_b220, &c_b221, &c__1, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssyevx_("N", "A", "/", &c__0, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	infoc_1.infot = 4;
	ssyevx_("N", "A", "U", &c_n1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__1, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ssyevx_("N", "A", "U", &c__2, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__16, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssyevx_("N", "V", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ssyevx_("N", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ssyevx_("N", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__2, &
		c__1, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssyevx_("N", "I", "U", &c__2, a, &c__2, &c_b220, &c_b220, &c__2, &
		c__1, &c_b220, &m, x, z__, &c__1, w, &c__16, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssyevx_("N", "I", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__1, &
		c__2, &c_b220, &m, x, z__, &c__1, w, &c__8, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	ssyevx_("V", "A", "U", &c__2, a, &c__2, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__16, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	ssyevx_("V", "A", "U", &c__1, a, &c__1, &c_b220, &c_b220, &c__0, &
		c__0, &c_b220, &m, x, z__, &c__1, w, &c__0, iw, i3, &info);
	chkxer_("SSYEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        SSPEVD */

	s_copy(srnamc_1.srnamt, "SSPEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sspevd_("/", "U", &c__0, a, x, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sspevd_("N", "/", &c__0, a, x, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sspevd_("N", "U", &c_n1, a, x, z__, &c__1, w, &c__1, iw, &c__1, &info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sspevd_("V", "U", &c__2, a, x, z__, &c__1, w, &c__23, iw, &c__12, &
		info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sspevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__0, iw, &c__1, &info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sspevd_("N", "U", &c__2, a, x, z__, &c__1, w, &c__3, iw, &c__1, &info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sspevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__16, iw, &c__12, &
		info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sspevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__1, iw, &c__0, &info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sspevd_("N", "U", &c__2, a, x, z__, &c__1, w, &c__4, iw, &c__0, &info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sspevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__23, iw, &c__11, &
		info);
	chkxer_("SSPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        SSPEV */

	s_copy(srnamc_1.srnamt, "SSPEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sspev_("/", "U", &c__0, a, w, z__, &c__1, x, &info);
	chkxer_("SSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sspev_("N", "/", &c__0, a, w, z__, &c__1, x, &info);
	chkxer_("SSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sspev_("N", "U", &c_n1, a, w, z__, &c__1, x, &info);
	chkxer_("SSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sspev_("V", "U", &c__2, a, w, z__, &c__1, x, &info);
	chkxer_("SSPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        SSPEVX */

	s_copy(srnamc_1.srnamt, "SSPEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sspevx_("/", "A", "U", &c__0, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sspevx_("N", "/", "U", &c__0, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sspevx_("N", "A", "/", &c__0, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	infoc_1.infot = 4;
	sspevx_("N", "A", "U", &c_n1, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sspevx_("N", "V", "U", &c__1, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sspevx_("N", "I", "U", &c__1, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sspevx_("N", "I", "U", &c__1, a, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sspevx_("N", "I", "U", &c__2, a, &c_b220, &c_b220, &c__2, &c__1, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sspevx_("N", "I", "U", &c__1, a, &c_b220, &c_b220, &c__1, &c__2, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sspevx_("V", "A", "U", &c__2, a, &c_b220, &c_b220, &c__0, &c__0, &
		c_b220, &m, x, z__, &c__1, w, iw, i3, &info);
	chkxer_("SSPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*     Test error exits for the SB path. */

    } else if (lsamen_(&c__2, c2, "SB")) {

/*        SSBTRD */

	s_copy(srnamc_1.srnamt, "SSBTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssbtrd_("/", "U", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("SSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssbtrd_("N", "/", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("SSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssbtrd_("N", "U", &c_n1, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("SSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssbtrd_("N", "U", &c__0, &c_n1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("SSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ssbtrd_("N", "U", &c__1, &c__1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("SSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssbtrd_("V", "U", &c__2, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("SSBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        SSBEVD */

	s_copy(srnamc_1.srnamt, "SSBEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssbevd_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssbevd_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssbevd_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssbevd_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__1, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ssbevd_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, &c__4, iw, 
		 &c__1, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ssbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__1, w, &c__25, 
		iw, &c__12, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ssbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__0, iw, 
		 &c__1, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ssbevd_("N", "U", &c__2, &c__0, a, &c__1, x, z__, &c__1, w, &c__3, iw, 
		 &c__1, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ssbevd_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__2, w, &c__18, 
		iw, &c__12, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ssbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, iw, 
		 &c__0, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ssbevd_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__2, w, &c__25, 
		iw, &c__11, &info);
	chkxer_("SSBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        SSBEV */

	s_copy(srnamc_1.srnamt, "SSBEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssbev_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("SSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssbev_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("SSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssbev_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("SSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssbev_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("SSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ssbev_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("SSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ssbev_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__1, w, &info);
	chkxer_("SSBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        SSBEVX */

	s_copy(srnamc_1.srnamt, "SSBEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssbevx_("/", "A", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssbevx_("N", "/", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssbevx_("N", "A", "/", &c__0, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	infoc_1.infot = 4;
	ssbevx_("N", "A", "U", &c_n1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ssbevx_("N", "A", "U", &c__0, &c_n1, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ssbevx_("N", "A", "U", &c__2, &c__1, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ssbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__2, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ssbevx_("N", "V", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ssbevx_("N", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ssbevx_("N", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__2, &c__1, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ssbevx_("N", "I", "U", &c__2, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__2, &c__1, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ssbevx_("N", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b220, &
		c_b220, &c__1, &c__2, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	ssbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__2, &c_b220, &
		c_b220, &c__0, &c__0, &c_b220, &m, x, z__, &c__1, w, iw, i3, &
		info);
	chkxer_("SSBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of SERRST */

} /* serrst_ */
