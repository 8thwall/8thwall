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

/* Subroutine */ int zerrhs_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002,\002 (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublecomplex a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */;
    integer i__, j, m;
    doublereal s[3];
    doublecomplex w[9], x[3];
    char c2[2];
    integer nt;
    doublecomplex vl[9]	/* was [3][3] */, vr[9]	/* was [3][3] */;
    doublereal rw[3];
    integer ihi, ilo;
    logical sel[3];
    doublecomplex tau[3];
    integer info, ifaill[3];
    extern /* Subroutine */ int zgebak_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublecomplex *, integer *, 
	    integer *), zgebal_(char *, integer *, 
	    doublecomplex *, integer *, integer *, integer *, doublereal *, 
	    integer *);
    integer ifailr[3];
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int zgehrd_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, integer *), chkxer_(char *, integer *, integer *, 
	    logical *, logical *), zhsein_(char *, char *, char *, 
	    logical *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, integer *
, integer *, doublecomplex *, doublereal *, integer *, integer *, 
	    integer *), zhseqr_(char *, char *, 
	    integer *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, integer *), ztrevc_(char *, char *, 
	    logical *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, integer *, integer *, 
	    doublecomplex *, doublereal *, integer *), 
	    zunghr_(integer *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, doublecomplex *, integer *, integer *), 
	    zunmhr_(char *, char *, integer *, integer *, integer *, integer *
, doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRHS tests the error exits for ZGEBAK, CGEBAL, CGEHRD, ZUNGHR, */
/*  ZUNMHR, ZHSEQR, CHSEIN, and ZTREVC. */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
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
	sel[j - 1] = TRUE_;
/* L20: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits of the nonsymmetric eigenvalue routines. */

    if (lsamen_(&c__2, c2, "HS")) {

/*        ZGEBAL */

	s_copy(srnamc_1.srnamt, "ZGEBAL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgebal_("/", &c__0, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("ZGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgebal_("N", &c_n1, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("ZGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgebal_("N", &c__2, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("ZGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        ZGEBAK */

	s_copy(srnamc_1.srnamt, "ZGEBAK", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgebak_("/", "R", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgebak_("N", "/", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgebak_("N", "R", &c_n1, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgebak_("N", "R", &c__0, &c__0, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgebak_("N", "R", &c__0, &c__2, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgebak_("N", "R", &c__2, &c__2, &c__1, s, &c__0, a, &c__2, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgebak_("N", "R", &c__0, &c__1, &c__1, s, &c__0, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgebak_("N", "R", &c__0, &c__1, &c__0, s, &c_n1, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zgebak_("N", "R", &c__2, &c__1, &c__2, s, &c__0, a, &c__1, &info);
	chkxer_("ZGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        ZGEHRD */

	s_copy(srnamc_1.srnamt, "ZGEHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgehrd_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgehrd_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgehrd_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgehrd_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgehrd_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgehrd_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__2, &info);
	chkxer_("ZGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zgehrd_(&c__2, &c__1, &c__2, a, &c__2, tau, w, &c__1, &info);
	chkxer_("ZGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        ZUNGHR */

	s_copy(srnamc_1.srnamt, "ZUNGHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zunghr_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zunghr_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zunghr_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zunghr_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zunghr_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zunghr_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zunghr_(&c__3, &c__1, &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("ZUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        ZUNMHR */

	s_copy(srnamc_1.srnamt, "ZUNMHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zunmhr_("/", "N", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zunmhr_("L", "/", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zunmhr_("L", "N", &c_n1, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zunmhr_("L", "N", &c__0, &c_n1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zunmhr_("L", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zunmhr_("L", "N", &c__0, &c__0, &c__2, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zunmhr_("L", "N", &c__1, &c__2, &c__2, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__2, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zunmhr_("R", "N", &c__2, &c__1, &c__2, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__2, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zunmhr_("L", "N", &c__1, &c__1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zunmhr_("L", "N", &c__0, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zunmhr_("R", "N", &c__1, &c__0, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zunmhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zunmhr_("R", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zunmhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__2, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zunmhr_("L", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zunmhr_("R", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("ZUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 16;

/*        ZHSEQR */

	s_copy(srnamc_1.srnamt, "ZHSEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhseqr_("/", "N", &c__0, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhseqr_("E", "/", &c__0, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhseqr_("E", "N", &c_n1, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhseqr_("E", "N", &c__0, &c__0, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhseqr_("E", "N", &c__0, &c__2, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zhseqr_("E", "N", &c__1, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zhseqr_("E", "N", &c__1, &c__1, &c__2, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhseqr_("E", "N", &c__2, &c__1, &c__2, a, &c__1, x, c__, &c__2, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zhseqr_("E", "V", &c__2, &c__1, &c__2, a, &c__2, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("ZHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        ZHSEIN */

	s_copy(srnamc_1.srnamt, "ZHSEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhsein_("/", "N", "N", sel, &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhsein_("R", "/", "N", sel, &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhsein_("R", "N", "/", sel, &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zhsein_("R", "N", "N", sel, &c_n1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhsein_("R", "N", "N", sel, &c__2, a, &c__1, x, vl, &c__1, vr, &c__2, 
		&c__4, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zhsein_("L", "N", "N", sel, &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&c__4, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zhsein_("R", "N", "N", sel, &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&c__4, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhsein_("R", "N", "N", sel, &c__2, a, &c__2, x, vl, &c__1, vr, &c__2, 
		&c__1, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("ZHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        ZTREVC */

	s_copy(srnamc_1.srnamt, "ZTREVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztrevc_("/", "A", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, rw, &info);
	chkxer_("ZTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztrevc_("L", "/", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, rw, &info);
	chkxer_("ZTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztrevc_("L", "A", sel, &c_n1, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, rw, &info);
	chkxer_("ZTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztrevc_("L", "A", sel, &c__2, a, &c__1, vl, &c__2, vr, &c__1, &c__4, &
		m, w, rw, &info);
	chkxer_("ZTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztrevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, rw, &info);
	chkxer_("ZTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztrevc_("R", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, rw, &info);
	chkxer_("ZTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ztrevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__2, vr, &c__1, &c__1, &
		m, w, rw, &info);
	chkxer_("ZTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___22.ciunit = infoc_1.nout;
	s_wsfe(&io___22);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___23.ciunit = infoc_1.nout;
	s_wsfe(&io___23);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of ZERRHS */

} /* zerrhs_ */
