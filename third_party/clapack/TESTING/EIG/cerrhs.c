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

/* Subroutine */ int cerrhs_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002,\002 (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    complex a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */;
    integer i__, j, m;
    real s[3];
    complex w[9], x[3];
    char c2[2];
    integer nt;
    complex vl[9]	/* was [3][3] */, vr[9]	/* was [3][3] */;
    real rw[3];
    integer ihi, ilo;
    logical sel[3];
    complex tau[3];
    integer info;
    extern /* Subroutine */ int cgebak_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, complex *, integer *, integer *), cgebal_(char *, integer *, complex *, integer *, 
	    integer *, integer *, real *, integer *), cgehrd_(integer 
	    *, integer *, integer *, complex *, integer *, complex *, complex 
	    *, integer *, integer *);
    integer ifaill[3], ifailr[3];
    extern /* Subroutine */ int chsein_(char *, char *, char *, logical *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *, integer *, complex *, real *, 
	    integer *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), chseqr_(char *, char *, integer *, integer 
	    *, integer *, complex *, integer *, complex *, complex *, integer 
	    *, complex *, integer *, integer *), cunghr_(
	    integer *, integer *, integer *, complex *, integer *, complex *, 
	    complex *, integer *, integer *), ctrevc_(char *, char *, logical 
	    *, integer *, complex *, integer *, complex *, integer *, complex 
	    *, integer *, integer *, integer *, complex *, real *, integer *), cunmhr_(char *, char *, integer *, integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *, complex *, integer *, integer *);

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

/*  CERRHS tests the error exits for CGEBAK, CGEBAL, CGEHRD, CUNGHR, */
/*  CUNMHR, CHSEQR, CHSEIN, and CTREVC. */

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
	    r__1 = 1.f / (real) (i__ + j);
	    a[i__1].r = r__1, a[i__1].i = 0.f;
/* L10: */
	}
	sel[j - 1] = TRUE_;
/* L20: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits of the nonsymmetric eigenvalue routines. */

    if (lsamen_(&c__2, c2, "HS")) {

/*        CGEBAL */

	s_copy(srnamc_1.srnamt, "CGEBAL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgebal_("/", &c__0, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("CGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgebal_("N", &c_n1, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("CGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgebal_("N", &c__2, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("CGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        CGEBAK */

	s_copy(srnamc_1.srnamt, "CGEBAK", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgebak_("/", "R", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgebak_("N", "/", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgebak_("N", "R", &c_n1, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgebak_("N", "R", &c__0, &c__0, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgebak_("N", "R", &c__0, &c__2, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgebak_("N", "R", &c__2, &c__2, &c__1, s, &c__0, a, &c__2, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgebak_("N", "R", &c__0, &c__1, &c__1, s, &c__0, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgebak_("N", "R", &c__0, &c__1, &c__0, s, &c_n1, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cgebak_("N", "R", &c__2, &c__1, &c__2, s, &c__0, a, &c__1, &info);
	chkxer_("CGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        CGEHRD */

	s_copy(srnamc_1.srnamt, "CGEHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgehrd_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgehrd_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgehrd_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgehrd_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgehrd_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgehrd_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__2, &info);
	chkxer_("CGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cgehrd_(&c__2, &c__1, &c__2, a, &c__2, tau, w, &c__1, &info);
	chkxer_("CGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        CUNGHR */

	s_copy(srnamc_1.srnamt, "CUNGHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cunghr_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cunghr_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cunghr_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cunghr_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cunghr_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cunghr_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("CUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cunghr_(&c__3, &c__1, &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("CUNGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        CUNMHR */

	s_copy(srnamc_1.srnamt, "CUNMHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cunmhr_("/", "N", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cunmhr_("L", "/", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cunmhr_("L", "N", &c_n1, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cunmhr_("L", "N", &c__0, &c_n1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cunmhr_("L", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cunmhr_("L", "N", &c__0, &c__0, &c__2, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cunmhr_("L", "N", &c__1, &c__2, &c__2, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__2, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cunmhr_("R", "N", &c__2, &c__1, &c__2, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__2, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cunmhr_("L", "N", &c__1, &c__1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cunmhr_("L", "N", &c__0, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cunmhr_("R", "N", &c__1, &c__0, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cunmhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cunmhr_("R", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cunmhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__2, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cunmhr_("L", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cunmhr_("R", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("CUNMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 16;

/*        CHSEQR */

	s_copy(srnamc_1.srnamt, "CHSEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chseqr_("/", "N", &c__0, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chseqr_("E", "/", &c__0, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chseqr_("E", "N", &c_n1, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chseqr_("E", "N", &c__0, &c__0, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chseqr_("E", "N", &c__0, &c__2, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	chseqr_("E", "N", &c__1, &c__1, &c__0, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	chseqr_("E", "N", &c__1, &c__1, &c__2, a, &c__1, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chseqr_("E", "N", &c__2, &c__1, &c__2, a, &c__1, x, c__, &c__2, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	chseqr_("E", "V", &c__2, &c__1, &c__2, a, &c__2, x, c__, &c__1, w, &
		c__1, &info);
	chkxer_("CHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        CHSEIN */

	s_copy(srnamc_1.srnamt, "CHSEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chsein_("/", "N", "N", sel, &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chsein_("R", "/", "N", sel, &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chsein_("R", "N", "/", sel, &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	chsein_("R", "N", "N", sel, &c_n1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&c__0, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chsein_("R", "N", "N", sel, &c__2, a, &c__1, x, vl, &c__1, vr, &c__2, 
		&c__4, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	chsein_("L", "N", "N", sel, &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&c__4, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	chsein_("R", "N", "N", sel, &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&c__4, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chsein_("R", "N", "N", sel, &c__2, a, &c__2, x, vl, &c__1, vr, &c__2, 
		&c__1, &m, w, rw, ifaill, ifailr, &info);
	chkxer_("CHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        CTREVC */

	s_copy(srnamc_1.srnamt, "CTREVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctrevc_("/", "A", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, rw, &info);
	chkxer_("CTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctrevc_("L", "/", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, rw, &info);
	chkxer_("CTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctrevc_("L", "A", sel, &c_n1, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, rw, &info);
	chkxer_("CTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctrevc_("L", "A", sel, &c__2, a, &c__1, vl, &c__2, vr, &c__1, &c__4, &
		m, w, rw, &info);
	chkxer_("CTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctrevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, rw, &info);
	chkxer_("CTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctrevc_("R", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, rw, &info);
	chkxer_("CTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ctrevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__2, vr, &c__1, &c__1, &
		m, w, rw, &info);
	chkxer_("CTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of CERRHS */

} /* cerrhs_ */
