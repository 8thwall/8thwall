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

/* Subroutine */ int derrhs_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002,\002 (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublereal a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */;
    integer i__, j, m;
    doublereal s[3], w[28];
    char c2[2];
    doublereal wi[3];
    integer nt;
    doublereal vl[9]	/* was [3][3] */, vr[9]	/* was [3][3] */, wr[3];
    integer ihi, ilo;
    logical sel[3];
    doublereal tau[3];
    integer info;
    extern /* Subroutine */ int dgebak_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *), dgebal_(char *, integer *, doublereal 
	    *, integer *, integer *, integer *, doublereal *, integer *), dgehrd_(integer *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, integer *);
    integer ifaill[3], ifailr[3];
    extern /* Subroutine */ int dhsein_(char *, char *, char *, logical *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, doublereal *, integer *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), dorghr_(integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *), dhseqr_(char *, char *, integer *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), dtrevc_(char *, char *, logical *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, integer *, integer *, doublereal *, integer *), dormhr_(char *, char *, integer *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *, integer *);

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

/*  DERRHS tests the error exits for DGEBAK, SGEBAL, SGEHRD, DORGHR, */
/*  DORMHR, DHSEQR, SHSEIN, and DTREVC. */

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
	    a[i__ + j * 3 - 4] = 1. / (doublereal) (i__ + j);
/* L10: */
	}
	wi[j - 1] = (doublereal) j;
	sel[j - 1] = TRUE_;
/* L20: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits of the nonsymmetric eigenvalue routines. */

    if (lsamen_(&c__2, c2, "HS")) {

/*        DGEBAL */

	s_copy(srnamc_1.srnamt, "DGEBAL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgebal_("/", &c__0, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("DGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgebal_("N", &c_n1, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("DGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgebal_("N", &c__2, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("DGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        DGEBAK */

	s_copy(srnamc_1.srnamt, "DGEBAK", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgebak_("/", "R", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgebak_("N", "/", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgebak_("N", "R", &c_n1, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgebak_("N", "R", &c__0, &c__0, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgebak_("N", "R", &c__0, &c__2, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgebak_("N", "R", &c__2, &c__2, &c__1, s, &c__0, a, &c__2, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgebak_("N", "R", &c__0, &c__1, &c__1, s, &c__0, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgebak_("N", "R", &c__0, &c__1, &c__0, s, &c_n1, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dgebak_("N", "R", &c__2, &c__1, &c__2, s, &c__0, a, &c__1, &info);
	chkxer_("DGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        DGEHRD */

	s_copy(srnamc_1.srnamt, "DGEHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgehrd_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgehrd_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgehrd_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgehrd_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgehrd_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgehrd_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__2, &info);
	chkxer_("DGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dgehrd_(&c__2, &c__1, &c__2, a, &c__2, tau, w, &c__1, &info);
	chkxer_("DGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        DORGHR */

	s_copy(srnamc_1.srnamt, "DORGHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dorghr_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dorghr_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dorghr_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dorghr_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dorghr_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dorghr_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("DORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dorghr_(&c__3, &c__1, &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("DORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        DORMHR */

	s_copy(srnamc_1.srnamt, "DORMHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dormhr_("/", "N", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dormhr_("L", "/", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dormhr_("L", "N", &c_n1, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dormhr_("L", "N", &c__0, &c_n1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dormhr_("L", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dormhr_("L", "N", &c__0, &c__0, &c__2, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dormhr_("L", "N", &c__1, &c__2, &c__2, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__2, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dormhr_("R", "N", &c__2, &c__1, &c__2, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__2, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dormhr_("L", "N", &c__1, &c__1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dormhr_("L", "N", &c__0, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dormhr_("R", "N", &c__1, &c__0, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dormhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dormhr_("R", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dormhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__2, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dormhr_("L", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dormhr_("R", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("DORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 16;

/*        DHSEQR */

	s_copy(srnamc_1.srnamt, "DHSEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dhseqr_("/", "N", &c__0, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dhseqr_("E", "/", &c__0, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dhseqr_("E", "N", &c_n1, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dhseqr_("E", "N", &c__0, &c__0, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dhseqr_("E", "N", &c__0, &c__2, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dhseqr_("E", "N", &c__1, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dhseqr_("E", "N", &c__1, &c__1, &c__2, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dhseqr_("E", "N", &c__2, &c__1, &c__2, a, &c__1, wr, wi, c__, &c__2, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dhseqr_("E", "V", &c__2, &c__1, &c__2, a, &c__2, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("DHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        DHSEIN */

	s_copy(srnamc_1.srnamt, "DHSEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dhsein_("/", "N", "N", sel, &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dhsein_("R", "/", "N", sel, &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dhsein_("R", "N", "/", sel, &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dhsein_("R", "N", "N", sel, &c_n1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dhsein_("R", "N", "N", sel, &c__2, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__2, &c__4, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dhsein_("L", "N", "N", sel, &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &c__4, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dhsein_("R", "N", "N", sel, &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &c__4, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dhsein_("R", "N", "N", sel, &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__2, &c__1, &m, w, ifaill, ifailr, &info);
	chkxer_("DHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        DTREVC */

	s_copy(srnamc_1.srnamt, "DTREVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtrevc_("/", "A", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, &info);
	chkxer_("DTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtrevc_("L", "/", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, &info);
	chkxer_("DTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtrevc_("L", "A", sel, &c_n1, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, &info);
	chkxer_("DTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtrevc_("L", "A", sel, &c__2, a, &c__1, vl, &c__2, vr, &c__1, &c__4, &
		m, w, &info);
	chkxer_("DTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtrevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, &info);
	chkxer_("DTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtrevc_("R", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, &info);
	chkxer_("DTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dtrevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__2, vr, &c__1, &c__1, &
		m, w, &info);
	chkxer_("DTREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of DERRHS */

} /* derrhs_ */
