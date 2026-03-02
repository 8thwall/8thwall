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
static integer c__18 = 18;
static integer c__3 = 3;
static integer c__32 = 32;
static logical c_true = TRUE_;
static logical c_false = FALSE_;
static integer c__20 = 20;

/* Subroutine */ int derrgg_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublereal a[9]	/* was [3][3] */, b[9]	/* was [3][3] */;
    integer i__, j, m;
    doublereal q[9]	/* was [3][3] */, u[9]	/* was [3][3] */, v[9]	/* 
	    was [3][3] */, w[18], z__[9]	/* was [3][3] */;
    char c2[2];
    doublereal r1[3], r2[3], r3[3];
    logical bw[3];
    doublereal ls[3];
    integer iw[3], nt;
    doublereal rs[3], dif, rce[2];
    logical sel[3];
    doublereal tau[3], rcv[2];
    integer info, sdim;
    doublereal anrm, bnrm, tola, tolb;
    integer ifst, ilst;
    doublereal scale;
    extern /* Subroutine */ int dgges_(char *, char *, char *, L_fp, integer *
, doublereal *, integer *, doublereal *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, integer *, logical *, 
	    integer *), dggev_(char *, char *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, integer *, integer *), dgghrd_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), dggglm_(integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    dgglse_(integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, integer *, integer *), dggqrf_(integer *, integer *
, integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, integer *), 
	    dggrqf_(integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *, integer *);
    integer ncycle;
    extern logical dlctes_(), lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int dggsvd_(char *, char *, char *, integer *, 
	    integer *, integer *, integer *, integer *, doublereal *, integer 
	    *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *), dggesx_(char *, char *, char *, L_fp, char *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, doublereal *, doublereal *
, integer *, integer *, integer *, logical *, integer *), dhgeqz_(char *, char *, char *, integer *
, integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *), dtgevc_(char *, char *, 
	    logical *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, integer *, doublereal *, integer *), 
	    chkxer_(char *, integer *, integer *, logical *, logical *), dggevx_(char *, char *, char *, char *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, logical *, integer *), dtgexc_(logical *, logical *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *, integer *, 
	    doublereal *, integer *, integer *), dtgsen_(integer *, logical *, 
	     logical *, logical *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, integer *, doublereal *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *, integer *, integer *), dtgsja_(char *, char *, char *, 
	     integer *, integer *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, integer *), dtgsna_(char *, 
	    char *, logical *, integer *, doublereal *, integer *, doublereal 
	    *, integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *, integer *), dggsvp_(char *, 
	    char *, char *, integer *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *, doublereal *, 
	    doublereal *, integer *);
    extern logical dlctsx_();
    integer dummyk, dummyl;
    extern /* Subroutine */ int dtgsyl_(char *, integer *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERRGG tests the error exits for DGGES, DGGESX, DGGEV, DGGEVX, */
/*  DGGGLM, DGGHRD, DGGLSE, DGGQRF, DGGRQF, DGGSVD, DGGSVP, DHGEQZ, */
/*  DTGEVC, DTGEXC, DTGSEN, DTGSJA, DTGSNA, and DTGSYL. */

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
/*     .. Executable Statements .. */

    infoc_1.nout = *nunit;
    io___1.ciunit = infoc_1.nout;
    s_wsle(&io___1);
    e_wsle();
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);

/*     Set the variables to innocuous values. */

    for (j = 1; j <= 3; ++j) {
	sel[j - 1] = TRUE_;
	for (i__ = 1; i__ <= 3; ++i__) {
	    a[i__ + j * 3 - 4] = 0.;
	    b[i__ + j * 3 - 4] = 0.;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 3; ++i__) {
	a[i__ + i__ * 3 - 4] = 1.;
	b[i__ + i__ * 3 - 4] = 1.;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    tola = 1.;
    tolb = 1.;
    ifst = 1;
    ilst = 1;
    nt = 0;

/*     Test error exits for the GG path. */

    if (lsamen_(&c__2, c2, "GG")) {

/*        DGGHRD */

	s_copy(srnamc_1.srnamt, "DGGHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgghrd_("/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgghrd_("N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgghrd_("N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgghrd_("N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgghrd_("N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dgghrd_("V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dgghrd_("N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("DGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        DHGEQZ */

	s_copy(srnamc_1.srnamt, "DHGEQZ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dhgeqz_("/", "N", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dhgeqz_("E", "/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dhgeqz_("E", "N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dhgeqz_("E", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dhgeqz_("E", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dhgeqz_("E", "N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dhgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dhgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dhgeqz_("E", "V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	dhgeqz_("E", "N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("DHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        DTGEVC */

	s_copy(srnamc_1.srnamt, "DTGEVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtgevc_("/", "A", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtgevc_("R", "/", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtgevc_("R", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtgevc_("R", "A", sel, &c__2, a, &c__1, b, &c__2, q, &c__1, z__, &
		c__2, &c__0, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__1, q, &c__1, z__, &
		c__2, &c__0, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtgevc_("L", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dtgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dtgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__2, &c__1, &m, w, &info);
	chkxer_("DTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GSV path. */

    } else if (lsamen_(&c__3, path, "GSV")) {

/*        DGGSVD */

	s_copy(srnamc_1.srnamt, "DGGSVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggsvd_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggsvd_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggsvd_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dggsvd_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggsvd_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dggsvd_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dggsvd_("N", "N", "N", &c__2, &c__1, &c__1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dggsvd_("N", "N", "N", &c__1, &c__1, &c__2, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dggsvd_("U", "N", "N", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dggsvd_("N", "V", "N", &c__1, &c__1, &c__2, &dummyk, &dummyl, a, &
		c__1, b, &c__2, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	dggsvd_("N", "N", "Q", &c__1, &c__2, &c__1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("DGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        DGGSVP */

	s_copy(srnamc_1.srnamt, "DGGSVP", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggsvp_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggsvp_("N", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggsvp_("N", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dggsvp_("N", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggsvp_("N", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dggsvp_("N", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dggsvp_("N", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dggsvp_("N", "N", "N", &c__1, &c__2, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dggsvp_("U", "N", "N", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dggsvp_("N", "V", "N", &c__1, &c__2, &c__1, a, &c__1, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	dggsvp_("N", "N", "Q", &c__1, &c__1, &c__2, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("DGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        DTGSJA */

	s_copy(srnamc_1.srnamt, "DTGSJA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtgsja_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtgsja_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtgsja_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtgsja_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtgsja_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtgsja_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__0, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dtgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__0, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dtgsja_("U", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__0, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	dtgsja_("N", "V", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__0, q, &
		c__1, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	dtgsja_("N", "N", "Q", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__0, w, &ncycle, &info);
	chkxer_("DTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*     Test error exits for the GLM path. */

    } else if (lsamen_(&c__3, path, "GLM")) {

/*        DGGGLM */

	s_copy(srnamc_1.srnamt, "DGGGLM", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggglm_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggglm_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggglm_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggglm_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggglm_(&c__1, &c__0, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggglm_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dggglm_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dggglm_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, r1, r2, r3, w, &c__1, 
		 &info);
	chkxer_("DGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the LSE path. */

    } else if (lsamen_(&c__3, path, "LSE")) {

/*        DGGLSE */

	s_copy(srnamc_1.srnamt, "DGGLSE", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgglse_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgglse_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgglse_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgglse_(&c__0, &c__0, &c__1, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgglse_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgglse_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgglse_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dgglse_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, r1, r2, r3, w, &c__1, 
		 &info);
	chkxer_("DGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GQR path. */

    } else if (lsamen_(&c__3, path, "GQR")) {

/*        DGGQRF */

	s_copy(srnamc_1.srnamt, "DGGQRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggqrf_(&c_n1, &c__0, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggqrf_(&c__0, &c_n1, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggqrf_(&c__0, &c__0, &c_n1, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggqrf_(&c__0, &c__0, &c__0, a, &c__0, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dggqrf_(&c__0, &c__0, &c__0, a, &c__1, r1, b, &c__0, r2, w, &c__18, &
		info);
	chkxer_("DGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dggqrf_(&c__1, &c__1, &c__2, a, &c__1, r1, b, &c__1, r2, w, &c__1, &
		info);
	chkxer_("DGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        DGGRQF */

	s_copy(srnamc_1.srnamt, "DGGRQF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggrqf_(&c_n1, &c__0, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggrqf_(&c__0, &c_n1, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggrqf_(&c__0, &c__0, &c_n1, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggrqf_(&c__0, &c__0, &c__0, a, &c__0, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("DGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dggrqf_(&c__0, &c__0, &c__0, a, &c__1, r1, b, &c__0, r2, w, &c__18, &
		info);
	chkxer_("DGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dggrqf_(&c__1, &c__1, &c__2, a, &c__1, r1, b, &c__1, r2, w, &c__1, &
		info);
	chkxer_("DGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*     Test error exits for the DGS, DGV, DGX, and DXV paths. */

    } else if (lsamen_(&c__3, path, "DGS") || lsamen_(&
	    c__3, path, "DGV") || lsamen_(&c__3, path, 
	    "DGX") || lsamen_(&c__3, path, "DXV")) {

/*        DGGES */

	s_copy(srnamc_1.srnamt, "DGGES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgges_("/", "N", "S", (L_fp)dlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgges_("N", "/", "S", (L_fp)dlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgges_("N", "V", "/", (L_fp)dlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgges_("N", "V", "S", (L_fp)dlctes_, &c_n1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgges_("N", "V", "S", (L_fp)dlctes_, &c__1, a, &c__0, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dgges_("N", "V", "S", (L_fp)dlctes_, &c__1, a, &c__1, b, &c__0, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dgges_("N", "V", "S", (L_fp)dlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__0, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dgges_("V", "V", "S", (L_fp)dlctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__2, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	dgges_("N", "V", "S", (L_fp)dlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__0, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	dgges_("V", "V", "S", (L_fp)dlctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 r1, r2, r3, q, &c__2, u, &c__1, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 19;
	dgges_("V", "V", "S", (L_fp)dlctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 r1, r2, r3, q, &c__2, u, &c__2, w, &c__1, bw, &info);
	chkxer_("DGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        DGGESX */

	s_copy(srnamc_1.srnamt, "DGGESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggesx_("/", "N", "S", (L_fp)dlctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggesx_("N", "/", "S", (L_fp)dlctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggesx_("V", "V", "/", (L_fp)dlctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "/", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c_n1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c__1, a, &c__0, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c__1, a, &c__1, b, &c__0, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__0, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__0, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, r1, r2, r3, q, &c__2, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, r1, r2, r3, q, &c__2, u, &c__2, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	dggesx_("V", "V", "S", (L_fp)dlctsx_, "V", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__32, 
		iw, &c__0, bw, &info);
	chkxer_("DGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        DGGEV */

	s_copy(srnamc_1.srnamt, "DGGEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggev_("/", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggev_("N", "/", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggev_("V", "V", &c_n1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggev_("V", "V", &c__1, a, &c__0, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dggev_("V", "V", &c__1, a, &c__1, b, &c__0, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dggev_("N", "V", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__0, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dggev_("V", "V", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, &c__1, u, &
		c__2, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dggev_("V", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, &c__2, u, &
		c__0, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dggev_("V", "V", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, &c__2, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dggev_("V", "V", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("DGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        DGGEVX */

	s_copy(srnamc_1.srnamt, "DGGEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dggevx_("/", "N", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dggevx_("N", "/", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dggevx_("N", "N", "/", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dggevx_("N", "N", "N", "/", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dggevx_("N", "N", "N", "N", &c_n1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dggevx_("N", "N", "N", "N", &c__1, a, &c__0, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__0, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__0, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dggevx_("N", "V", "N", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, 
		&c__1, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__0, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, 
		&c__2, u, &c__1, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 26;
	dggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, 
		&c__2, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("DGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        DTGEXC */

	s_copy(srnamc_1.srnamt, "DTGEXC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 3;
	dtgexc_(&c_true, &c_true, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtgexc_(&c_true, &c_true, &c__1, a, &c__0, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dtgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__0, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dtgexc_(&c_false, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dtgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dtgexc_(&c_true, &c_false, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, w, &c__1, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dtgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, w, &c__1, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dtgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__0, &info);
	chkxer_("DTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        DTGSEN */

	s_copy(srnamc_1.srnamt, "DTGSEN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtgsen_(&c_n1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtgsen_(&c__1, &c_true, &c_true, sel, &c_n1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dtgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__0, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dtgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__0, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dtgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__0, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dtgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__0, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	dtgsen_(&c__0, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	dtgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	dtgsen_(&c__2, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	dtgsen_(&c__0, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	dtgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	dtgsen_(&c__2, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__1, &info);
	chkxer_("DTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        DTGSNA */

	s_copy(srnamc_1.srnamt, "DTGSNA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtgsna_("/", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtgsna_("B", "/", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtgsna_("B", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtgsna_("B", "A", sel, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtgsna_("B", "A", sel, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dtgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__0, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dtgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__0, &m, w, &c__1, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dtgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__0, iw, &info);
	chkxer_("DTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        DTGSYL */

	s_copy(srnamc_1.srnamt, "DTGSYL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtgsyl_("/", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtgsyl_("N", &c_n1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtgsyl_("N", &c__0, &c__0, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtgsyl_("N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtgsyl_("N", &c__0, &c__1, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dtgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__0, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dtgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__0, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dtgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__0, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	dtgsyl_("N", &c__1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	dtgsyl_("N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("DTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___38.ciunit = infoc_1.nout;
	s_wsfe(&io___38);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___39.ciunit = infoc_1.nout;
	s_wsfe(&io___39);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of DERRGG */

} /* derrgg_ */
