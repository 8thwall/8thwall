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
static real c_b458 = 0.f;
static real c_b472 = 1.f;

/* Subroutine */ int cerrst_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002,\002 (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    complex a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */;
    real d__[3], e[3];
    integer i__, j, m, n;
    complex q[9]	/* was [3][3] */;
    real r__[60];
    complex w[60];
    real x[3];
    complex z__[9]	/* was [3][3] */;
    char c2[2];
    integer i1[3], i2[3], i3[3], iw[36], nt;
    real rw[60];
    complex tau[3];
    integer info;
    extern /* Subroutine */ int chbev_(char *, char *, integer *, integer *, 
	    complex *, integer *, real *, complex *, integer *, complex *, 
	    real *, integer *), cheev_(char *, char *, 
	    integer *, complex *, integer *, real *, complex *, integer *, 
	    real *, integer *), chpev_(char *, char *, 
	    integer *, complex *, real *, complex *, integer *, complex *, 
	    real *, integer *), chbevd_(char *, char *, 
	    integer *, integer *, complex *, integer *, real *, complex *, 
	    integer *, complex *, integer *, real *, integer *, integer *, 
	    integer *, integer *), cheevd_(char *, char *, 
	    integer *, complex *, integer *, real *, complex *, integer *, 
	    real *, integer *, integer *, integer *, integer *), cstedc_(char *, integer *, real *, real *, complex *, 
	    integer *, complex *, integer *, real *, integer *, integer *, 
	    integer *, integer *), chbtrd_(char *, char *, integer *, 
	    integer *, complex *, integer *, real *, real *, complex *, 
	    integer *, complex *, integer *), chetrd_(char *, 
	    integer *, complex *, integer *, real *, real *, complex *, 
	    complex *, integer *, integer *), chpevd_(char *, char *, 
	    integer *, complex *, real *, complex *, integer *, complex *, 
	    integer *, real *, integer *, integer *, integer *, integer *), cheevr_(char *, char *, char *, integer *, 
	    complex *, integer *, real *, real *, integer *, integer *, real *
, integer *, real *, complex *, integer *, integer *, complex *, 
	    integer *, real *, integer *, integer *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chbevx_(char *, char *, char *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, real *, complex *
, integer *, complex *, real *, integer *, integer *, integer *), cheevx_(char *, char *, char *, integer *
, complex *, integer *, real *, real *, integer *, integer *, 
	    real *, integer *, real *, complex *, integer *, complex *, 
	    integer *, real *, integer *, integer *, integer *), chkxer_(char *, integer *, integer *, logical *, 
	    logical *), chptrd_(char *, integer *, complex *, real *, 
	    real *, complex *, integer *), cstein_(integer *, real *, 
	    real *, integer *, real *, integer *, integer *, complex *, 
	    integer *, real *, integer *, integer *, integer *), chpevx_(char 
	    *, char *, char *, integer *, complex *, real *, real *, integer *
, integer *, real *, integer *, real *, complex *, integer *, 
	    complex *, real *, integer *, integer *, integer *), cpteqr_(char *, integer *, real *, real *, 
	    complex *, integer *, real *, integer *), csteqr_(char *, 
	    integer *, real *, real *, complex *, integer *, real *, integer *
), cungtr_(char *, integer *, complex *, integer *, 
	    complex *, complex *, integer *, integer *), cupgtr_(char 
	    *, integer *, complex *, complex *, complex *, integer *, complex 
	    *, integer *), cunmtr_(char *, char *, char *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *), cupmtr_(
	    char *, char *, char *, integer *, integer *, complex *, complex *
, complex *, integer *, complex *, integer *);

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

/*  CERRST tests the error exits for CHETRD, CUNGTR, CUNMTR, CHPTRD, */
/*  CUNGTR, CUPMTR, CSTEQR, CSTEIN, CPTEQR, CHBTRD, */
/*  CHEEV, CHEEVX, CHEEVD, CHBEV, CHBEVX, CHBEVD, */
/*  CHPEV, CHPEVX, CHPEVD, and CSTEDC. */

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
	    r__1 = 1.f / (real) (i__ + j);
	    a[i__1].r = r__1, a[i__1].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    for (j = 1; j <= 3; ++j) {
	d__[j - 1] = (real) j;
	e[j - 1] = 0.f;
	i1[j - 1] = j;
	i2[j - 1] = j;
	i__1 = j - 1;
	tau[i__1].r = 1.f, tau[i__1].i = 0.f;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits for the ST path. */

    if (lsamen_(&c__2, c2, "ST")) {

/*        CHETRD */

	s_copy(srnamc_1.srnamt, "CHETRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chetrd_("/", &c__0, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("CHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chetrd_("U", &c_n1, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("CHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chetrd_("U", &c__2, a, &c__1, d__, e, tau, w, &c__1, &info)
		;
	chkxer_("CHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chetrd_("U", &c__0, a, &c__1, d__, e, tau, w, &c__0, &info)
		;
	chkxer_("CHETRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        CUNGTR */

	s_copy(srnamc_1.srnamt, "CUNGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cungtr_("/", &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cungtr_("U", &c_n1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cungtr_("U", &c__2, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cungtr_("U", &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("CUNGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        CUNMTR */

	s_copy(srnamc_1.srnamt, "CUNMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cunmtr_("/", "U", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cunmtr_("L", "/", "N", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cunmtr_("L", "U", "/", &c__0, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cunmtr_("L", "U", "N", &c_n1, &c__0, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cunmtr_("L", "U", "N", &c__0, &c_n1, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cunmtr_("L", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cunmtr_("R", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cunmtr_("L", "U", "N", &c__2, &c__0, a, &c__2, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cunmtr_("L", "U", "N", &c__0, &c__2, a, &c__1, tau, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cunmtr_("R", "U", "N", &c__2, &c__0, a, &c__1, tau, c__, &c__2, w, &
		c__1, &info);
	chkxer_("CUNMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        CHPTRD */

	s_copy(srnamc_1.srnamt, "CHPTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chptrd_("/", &c__0, a, d__, e, tau, &info);
	chkxer_("CHPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chptrd_("U", &c_n1, a, d__, e, tau, &info);
	chkxer_("CHPTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 2;

/*        CUPGTR */

	s_copy(srnamc_1.srnamt, "CUPGTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cupgtr_("/", &c__0, a, tau, z__, &c__1, w, &info);
	chkxer_("CUPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cupgtr_("U", &c_n1, a, tau, z__, &c__1, w, &info);
	chkxer_("CUPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cupgtr_("U", &c__2, a, tau, z__, &c__1, w, &info);
	chkxer_("CUPGTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        CUPMTR */

	s_copy(srnamc_1.srnamt, "CUPMTR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cupmtr_("/", "U", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("CUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cupmtr_("L", "/", "N", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("CUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cupmtr_("L", "U", "/", &c__0, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("CUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cupmtr_("L", "U", "N", &c_n1, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("CUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cupmtr_("L", "U", "N", &c__0, &c_n1, a, tau, c__, &c__1, w, &info);
	chkxer_("CUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cupmtr_("L", "U", "N", &c__2, &c__0, a, tau, c__, &c__1, w, &info);
	chkxer_("CUPMTR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        CPTEQR */

	s_copy(srnamc_1.srnamt, "CPTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpteqr_("/", &c__0, d__, e, z__, &c__1, rw, &info);
	chkxer_("CPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpteqr_("N", &c_n1, d__, e, z__, &c__1, rw, &info);
	chkxer_("CPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cpteqr_("V", &c__2, d__, e, z__, &c__1, rw, &info);
	chkxer_("CPTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        CSTEIN */

	s_copy(srnamc_1.srnamt, "CSTEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cstein_(&c_n1, d__, e, &c__0, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("CSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cstein_(&c__0, d__, e, &c_n1, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("CSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cstein_(&c__0, d__, e, &c__1, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("CSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cstein_(&c__2, d__, e, &c__0, x, i1, i2, z__, &c__1, rw, iw, i3, &
		info);
	chkxer_("CSTEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        CSTEQR */

	s_copy(srnamc_1.srnamt, "CSTEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csteqr_("/", &c__0, d__, e, z__, &c__1, rw, &info);
	chkxer_("CSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csteqr_("N", &c_n1, d__, e, z__, &c__1, rw, &info);
	chkxer_("CSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	csteqr_("V", &c__2, d__, e, z__, &c__1, rw, &info);
	chkxer_("CSTEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        CSTEDC */

	s_copy(srnamc_1.srnamt, "CSTEDC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cstedc_("/", &c__0, d__, e, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cstedc_("N", &c_n1, d__, e, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cstedc_("V", &c__2, d__, e, z__, &c__1, w, &c__4, rw, &c__23, iw, &
		c__28, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cstedc_("N", &c__2, d__, e, z__, &c__1, w, &c__0, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__0, rw, &c__23, iw, &
		c__28, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cstedc_("N", &c__2, d__, e, z__, &c__1, w, &c__1, rw, &c__0, iw, &
		c__1, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__1, rw, &c__1, iw, &
		c__12, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__4, rw, &c__1, iw, &
		c__28, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cstedc_("N", &c__2, d__, e, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__0, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cstedc_("I", &c__2, d__, e, z__, &c__2, w, &c__1, rw, &c__23, iw, &
		c__0, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cstedc_("V", &c__2, d__, e, z__, &c__2, w, &c__4, rw, &c__23, iw, &
		c__0, &info);
	chkxer_("CSTEDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        CHEEVD */

	s_copy(srnamc_1.srnamt, "CHEEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cheevd_("/", "U", &c__0, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cheevd_("N", "/", &c__0, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cheevd_("N", "U", &c_n1, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cheevd_("N", "U", &c__2, a, &c__1, x, w, &c__3, rw, &c__2, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cheevd_("N", "U", &c__1, a, &c__1, x, w, &c__0, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cheevd_("N", "U", &c__2, a, &c__2, x, w, &c__2, rw, &c__2, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cheevd_("V", "U", &c__2, a, &c__2, x, w, &c__3, rw, &c__25, iw, &
		c__12, &info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cheevd_("N", "U", &c__1, a, &c__1, x, w, &c__1, rw, &c__0, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cheevd_("N", "U", &c__2, a, &c__2, x, w, &c__3, rw, &c__1, iw, &c__1, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cheevd_("V", "U", &c__2, a, &c__2, x, w, &c__8, rw, &c__18, iw, &
		c__12, &info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cheevd_("N", "U", &c__1, a, &c__1, x, w, &c__1, rw, &c__1, iw, &c__0, 
		&info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cheevd_("V", "U", &c__2, a, &c__2, x, w, &c__8, rw, &c__25, iw, &
		c__11, &info);
	chkxer_("CHEEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        CHEEV */

	s_copy(srnamc_1.srnamt, "CHEEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cheev_("/", "U", &c__0, a, &c__1, x, w, &c__1, rw, &info);
	chkxer_("CHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cheev_("N", "/", &c__0, a, &c__1, x, w, &c__1, rw, &info);
	chkxer_("CHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cheev_("N", "U", &c_n1, a, &c__1, x, w, &c__1, rw, &info);
	chkxer_("CHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cheev_("N", "U", &c__2, a, &c__1, x, w, &c__3, rw, &info);
	chkxer_("CHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cheev_("N", "U", &c__2, a, &c__2, x, w, &c__2, rw, &info);
	chkxer_("CHEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 5;

/*        CHEEVX */

	s_copy(srnamc_1.srnamt, "CHEEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cheevx_("/", "A", "U", &c__0, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cheevx_("V", "/", "U", &c__0, a, &c__1, &c_b458, &c_b472, &c__1, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cheevx_("V", "A", "/", &c__0, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	infoc_1.infot = 4;
	cheevx_("V", "A", "U", &c_n1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cheevx_("V", "A", "U", &c__2, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__2, w, &c__3, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cheevx_("V", "V", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cheevx_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__1, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cheevx_("V", "I", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__2, &
		c__1, &c_b458, &m, x, z__, &c__2, w, &c__3, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cheevx_("V", "A", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__1, w, &c__3, rw, iw, i3, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	cheevx_("V", "A", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__0, &
		c__0, &c_b458, &m, x, z__, &c__2, w, &c__2, rw, iw, i1, &info);
	chkxer_("CHEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        CHEEVR */

	s_copy(srnamc_1.srnamt, "CHEEVR", (ftnlen)6, (ftnlen)6);
	n = 1;
	infoc_1.infot = 1;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("/", "A", "U", &c__0, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "/", "U", &c__0, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "A", "/", &c_n1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "A", "U", &c_n1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "A", "U", &c__2, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "V", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__0, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;

	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "I", "U", &c__2, a, &c__2, &c_b458, &c_b458, &c__2, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__0, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	i__1 = (n << 1) - 1;
	i__2 = n * 24;
	i__3 = n * 10;
	cheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[n * 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	i__1 = n << 1;
	i__2 = n * 24 - 1;
	i__3 = n * 10;
	cheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, &
		iw[(n << 1) - 2], &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	i__1 = n << 1;
	i__2 = n * 24;
	i__3 = n * 10 - 1;
	cheevr_("V", "I", "U", &c__1, a, &c__1, &c_b458, &c_b458, &c__1, &
		c__1, &c_b458, &m, r__, z__, &c__1, iw, q, &i__1, rw, &i__2, 
		iw, &i__3, &info);
	chkxer_("CHEEVR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        CHPEVD */

	s_copy(srnamc_1.srnamt, "CHPEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chpevd_("/", "U", &c__0, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chpevd_("N", "/", &c__0, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chpevd_("N", "U", &c_n1, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chpevd_("V", "U", &c__2, a, x, z__, &c__1, w, &c__4, rw, &c__25, iw, &
		c__12, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chpevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__0, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chpevd_("N", "U", &c__2, a, x, z__, &c__2, w, &c__1, rw, &c__2, iw, &
		c__1, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chpevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__2, rw, &c__25, iw, &
		c__12, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chpevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__1, rw, &c__0, iw, &
		c__1, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chpevd_("N", "U", &c__2, a, x, z__, &c__2, w, &c__2, rw, &c__1, iw, &
		c__1, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chpevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__4, rw, &c__18, iw, &
		c__12, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chpevd_("N", "U", &c__1, a, x, z__, &c__1, w, &c__1, rw, &c__1, iw, &
		c__0, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chpevd_("N", "U", &c__2, a, x, z__, &c__2, w, &c__2, rw, &c__2, iw, &
		c__0, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chpevd_("V", "U", &c__2, a, x, z__, &c__2, w, &c__4, rw, &c__25, iw, &
		c__2, &info);
	chkxer_("CHPEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        CHPEV */

	s_copy(srnamc_1.srnamt, "CHPEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chpev_("/", "U", &c__0, a, x, z__, &c__1, w, rw, &info);
	chkxer_("CHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chpev_("N", "/", &c__0, a, x, z__, &c__1, w, rw, &info);
	chkxer_("CHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chpev_("N", "U", &c_n1, a, x, z__, &c__1, w, rw, &info);
	chkxer_("CHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chpev_("V", "U", &c__2, a, x, z__, &c__1, w, rw, &info);
	chkxer_("CHPEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        CHPEVX */

	s_copy(srnamc_1.srnamt, "CHPEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chpevx_("/", "A", "U", &c__0, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chpevx_("V", "/", "U", &c__0, a, &c_b458, &c_b472, &c__1, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chpevx_("V", "A", "/", &c__0, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chpevx_("V", "A", "U", &c_n1, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chpevx_("V", "V", "U", &c__1, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	chpevx_("V", "I", "U", &c__1, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chpevx_("V", "I", "U", &c__2, a, &c_b458, &c_b458, &c__2, &c__1, &
		c_b458, &m, x, z__, &c__2, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	chpevx_("V", "A", "U", &c__2, a, &c_b458, &c_b458, &c__0, &c__0, &
		c_b458, &m, x, z__, &c__1, w, rw, iw, i3, &info);
	chkxer_("CHPEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the HB path. */

    } else if (lsamen_(&c__2, c2, "HB")) {

/*        CHBTRD */

	s_copy(srnamc_1.srnamt, "CHBTRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chbtrd_("/", "U", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("CHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chbtrd_("N", "/", &c__0, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("CHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chbtrd_("N", "U", &c_n1, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("CHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chbtrd_("N", "U", &c__0, &c_n1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("CHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	chbtrd_("N", "U", &c__1, &c__1, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("CHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	chbtrd_("V", "U", &c__2, &c__0, a, &c__1, d__, e, z__, &c__1, w, &
		info);
	chkxer_("CHBTRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        CHBEVD */

	s_copy(srnamc_1.srnamt, "CHBEVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chbevd_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chbevd_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chbevd_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chbevd_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	chbevd_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, &c__2, rw, 
		 &c__2, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__1, w, &c__8, rw, 
		 &c__25, iw, &c__12, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__0, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chbevd_("N", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__1, rw, 
		 &c__2, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__2, rw, 
		 &c__25, iw, &c__12, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__0, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chbevd_("N", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__2, rw, 
		 &c__1, iw, &c__1, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__8, rw, 
		 &c__2, iw, &c__12, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	chbevd_("N", "U", &c__1, &c__0, a, &c__1, x, z__, &c__1, w, &c__1, rw, 
		 &c__1, iw, &c__0, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	chbevd_("N", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__2, rw, 
		 &c__2, iw, &c__0, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	chbevd_("V", "U", &c__2, &c__1, a, &c__2, x, z__, &c__2, w, &c__8, rw, 
		 &c__25, iw, &c__2, &info);
	chkxer_("CHBEVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 15;

/*        CHBEV */

	s_copy(srnamc_1.srnamt, "CHBEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chbev_("/", "U", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("CHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chbev_("N", "/", &c__0, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("CHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chbev_("N", "U", &c_n1, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("CHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chbev_("N", "U", &c__0, &c_n1, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("CHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	chbev_("N", "U", &c__2, &c__1, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("CHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chbev_("V", "U", &c__2, &c__0, a, &c__1, x, z__, &c__1, w, rw, &info);
	chkxer_("CHBEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        CHBEVX */

	s_copy(srnamc_1.srnamt, "CHBEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chbevx_("/", "A", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chbevx_("V", "/", "U", &c__0, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b472, &c__1, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chbevx_("V", "A", "/", &c__0, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	infoc_1.infot = 4;
	chbevx_("V", "A", "U", &c_n1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	chbevx_("V", "A", "U", &c__0, &c_n1, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chbevx_("V", "A", "U", &c__2, &c__1, a, &c__1, q, &c__2, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__2, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__2, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chbevx_("V", "V", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	chbevx_("V", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chbevx_("V", "I", "U", &c__1, &c__0, a, &c__1, q, &c__1, &c_b458, &
		c_b458, &c__1, &c__2, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	chbevx_("V", "A", "U", &c__2, &c__0, a, &c__1, q, &c__2, &c_b458, &
		c_b458, &c__0, &c__0, &c_b458, &m, x, z__, &c__1, w, rw, iw, 
		i3, &info);
	chkxer_("CHBEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of CERRST */

} /* cerrst_ */
