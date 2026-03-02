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

/* Subroutine */ int serrhs_(char *path, integer *nunit)
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
    real a[9]	/* was [3][3] */, c__[9]	/* was [3][3] */;
    integer i__, j, m;
    real s[3], w[28];
    char c2[2];
    real wi[3];
    integer nt;
    real vl[9]	/* was [3][3] */, vr[9]	/* was [3][3] */, wr[3];
    integer ihi, ilo;
    logical sel[3];
    real tau[3];
    integer info;
    extern /* Subroutine */ int sgebak_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, integer *), sgebal_(char *, integer *, real *, integer *, 
	    integer *, integer *, real *, integer *);
    integer ifaill[3], ifailr[3];
    extern /* Subroutine */ int sgehrd_(integer *, integer *, integer *, real 
	    *, integer *, real *, real *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), shsein_(char *, char *, char *, logical *, 
	    integer *, real *, integer *, real *, real *, real *, integer *, 
	    real *, integer *, integer *, integer *, real *, integer *, 
	    integer *, integer *), sorghr_(integer *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, integer *), shseqr_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, integer *, 
	    real *, integer *, integer *), strevc_(char *, 
	    char *, logical *, integer *, real *, integer *, real *, integer *
, real *, integer *, integer *, integer *, real *, integer *), sormhr_(char *, char *, integer *, integer *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, real *, integer *, integer *);

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

/*  SERRHS tests the error exits for SGEBAK, SGEBAL, SGEHRD, SORGHR, */
/*  SORMHR, SHSEQR, SHSEIN, and STREVC. */

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
	    a[i__ + j * 3 - 4] = 1.f / (real) (i__ + j);
/* L10: */
	}
	wi[j - 1] = (real) j;
	sel[j - 1] = TRUE_;
/* L20: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits of the nonsymmetric eigenvalue routines. */

    if (lsamen_(&c__2, c2, "HS")) {

/*        SGEBAL */

	s_copy(srnamc_1.srnamt, "SGEBAL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgebal_("/", &c__0, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("SGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgebal_("N", &c_n1, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("SGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgebal_("N", &c__2, a, &c__1, &ilo, &ihi, s, &info);
	chkxer_("SGEBAL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        SGEBAK */

	s_copy(srnamc_1.srnamt, "SGEBAK", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgebak_("/", "R", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgebak_("N", "/", &c__0, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgebak_("N", "R", &c_n1, &c__1, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgebak_("N", "R", &c__0, &c__0, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgebak_("N", "R", &c__0, &c__2, &c__0, s, &c__0, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgebak_("N", "R", &c__2, &c__2, &c__1, s, &c__0, a, &c__2, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgebak_("N", "R", &c__0, &c__1, &c__1, s, &c__0, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgebak_("N", "R", &c__0, &c__1, &c__0, s, &c_n1, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sgebak_("N", "R", &c__2, &c__1, &c__2, s, &c__0, a, &c__1, &info);
	chkxer_("SGEBAK", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        SGEHRD */

	s_copy(srnamc_1.srnamt, "SGEHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgehrd_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgehrd_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgehrd_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgehrd_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgehrd_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgehrd_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__2, &info);
	chkxer_("SGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sgehrd_(&c__2, &c__1, &c__2, a, &c__2, tau, w, &c__1, &info);
	chkxer_("SGEHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        SORGHR */

	s_copy(srnamc_1.srnamt, "SORGHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sorghr_(&c_n1, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sorghr_(&c__0, &c__0, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sorghr_(&c__0, &c__2, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sorghr_(&c__1, &c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sorghr_(&c__0, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sorghr_(&c__2, &c__1, &c__1, a, &c__1, tau, w, &c__1, &info);
	chkxer_("SORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sorghr_(&c__3, &c__1, &c__3, a, &c__3, tau, w, &c__1, &info);
	chkxer_("SORGHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        SORMHR */

	s_copy(srnamc_1.srnamt, "SORMHR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sormhr_("/", "N", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sormhr_("L", "/", &c__0, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sormhr_("L", "N", &c_n1, &c__0, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sormhr_("L", "N", &c__0, &c_n1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sormhr_("L", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sormhr_("L", "N", &c__0, &c__0, &c__2, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sormhr_("L", "N", &c__1, &c__2, &c__2, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__2, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sormhr_("R", "N", &c__2, &c__1, &c__2, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__2, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sormhr_("L", "N", &c__1, &c__1, &c__1, &c__0, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sormhr_("L", "N", &c__0, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sormhr_("R", "N", &c__1, &c__0, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sormhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sormhr_("R", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sormhr_("L", "N", &c__2, &c__1, &c__1, &c__1, a, &c__2, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sormhr_("L", "N", &c__1, &c__2, &c__1, &c__1, a, &c__1, tau, c__, &
		c__1, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sormhr_("R", "N", &c__2, &c__1, &c__1, &c__1, a, &c__1, tau, c__, &
		c__2, w, &c__1, &info);
	chkxer_("SORMHR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 16;

/*        SHSEQR */

	s_copy(srnamc_1.srnamt, "SHSEQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	shseqr_("/", "N", &c__0, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	shseqr_("E", "/", &c__0, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	shseqr_("E", "N", &c_n1, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	shseqr_("E", "N", &c__0, &c__0, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	shseqr_("E", "N", &c__0, &c__2, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	shseqr_("E", "N", &c__1, &c__1, &c__0, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	shseqr_("E", "N", &c__1, &c__1, &c__2, a, &c__1, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	shseqr_("E", "N", &c__2, &c__1, &c__2, a, &c__1, wr, wi, c__, &c__2, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	shseqr_("E", "V", &c__2, &c__1, &c__2, a, &c__2, wr, wi, c__, &c__1, 
		w, &c__1, &info);
	chkxer_("SHSEQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        SHSEIN */

	s_copy(srnamc_1.srnamt, "SHSEIN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	shsein_("/", "N", "N", sel, &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	shsein_("R", "/", "N", sel, &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	shsein_("R", "N", "/", sel, &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	shsein_("R", "N", "N", sel, &c_n1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &c__0, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	shsein_("R", "N", "N", sel, &c__2, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__2, &c__4, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	shsein_("L", "N", "N", sel, &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &c__4, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	shsein_("R", "N", "N", sel, &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &c__4, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	shsein_("R", "N", "N", sel, &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__2, &c__1, &m, w, ifaill, ifailr, &info);
	chkxer_("SHSEIN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        STREVC */

	s_copy(srnamc_1.srnamt, "STREVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	strevc_("/", "A", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, &info);
	chkxer_("STREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	strevc_("L", "/", sel, &c__0, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, &info);
	chkxer_("STREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	strevc_("L", "A", sel, &c_n1, a, &c__1, vl, &c__1, vr, &c__1, &c__0, &
		m, w, &info);
	chkxer_("STREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	strevc_("L", "A", sel, &c__2, a, &c__1, vl, &c__2, vr, &c__1, &c__4, &
		m, w, &info);
	chkxer_("STREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	strevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, &info);
	chkxer_("STREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	strevc_("R", "A", sel, &c__2, a, &c__2, vl, &c__1, vr, &c__1, &c__4, &
		m, w, &info);
	chkxer_("STREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	strevc_("L", "A", sel, &c__2, a, &c__2, vl, &c__2, vr, &c__1, &c__1, &
		m, w, &info);
	chkxer_("STREVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of SERRHS */

} /* serrhs_ */
