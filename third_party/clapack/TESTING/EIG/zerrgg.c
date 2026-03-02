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
static integer c__18 = 18;
static integer c__32 = 32;
static logical c_true = TRUE_;
static logical c_false = FALSE_;
static integer c_n5 = -5;
static integer c__20 = 20;
static integer c__5 = 5;

/* Subroutine */ int zerrgg_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublecomplex a[9]	/* was [3][3] */, b[9]	/* was [3][3] */;
    integer i__, j, m;
    doublecomplex q[9]	/* was [3][3] */, u[9]	/* was [3][3] */, v[9]	/* 
	    was [3][3] */, w[18], z__[9]	/* was [3][3] */;
    char c2[2];
    doublereal r1[3], r2[3];
    logical bw[3];
    doublereal ls[3];
    integer iw[18], nt;
    doublereal rs[3], rw[18], dif, rce[3];
    logical sel[3];
    doublecomplex tau[3];
    doublereal rcv[3];
    doublecomplex beta[3];
    integer info, sdim;
    doublereal anrm, bnrm, tola, tolb;
    integer ifst, ilst;
    doublecomplex alpha[3];
    doublereal scale;
    extern /* Subroutine */ int zgges_(char *, char *, char *, L_fp, integer *
, doublecomplex *, integer *, doublecomplex *, integer *, integer 
	    *, doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, logical *, integer *), 
	    zggev_(char *, char *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *);
    integer ncycle;
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zgghrd_(char *, char *, integer *, integer 
	    *, integer *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     integer *), zggglm_(integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex 
	    *, integer *, integer *), zgglse_(integer *, integer *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex *
, integer *, integer *), zggqrf_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, integer *)
	    , zggrqf_(integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, integer *), ztgevc_(
	    char *, char *, logical *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *, integer *, doublecomplex *, 
	     doublereal *, integer *);
    extern logical zlctes_();
    extern /* Subroutine */ int zggsvd_(char *, char *, char *, integer *, 
	    integer *, integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublereal *, 
	    integer *, integer *);
    integer dummyk, dummyl;
    extern /* Subroutine */ int zggesx_(char *, char *, char *, L_fp, char *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    logical *, integer *), zhgeqz_(
	    char *, char *, char *, integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, integer *), zggevx_(char *, 
	    char *, char *, char *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
, doublereal *, doublereal *, doublecomplex *, integer *, 
	    doublereal *, integer *, logical *, integer *), ztgexc_(logical *, logical *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     integer *, integer *), ztgsen_(integer *, logical *, logical *, 
	    logical *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublecomplex *, integer *, integer *, 
	     integer *, integer *), ztgsja_(char *, char *, char *, integer *, 
	     integer *, integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *), 
	    ztgsna_(char *, char *, logical *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, doublereal *, doublereal *, integer *
, integer *, doublecomplex *, integer *, integer *, integer *), zggsvp_(char *, char *, char *, integer *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, doublereal *, doublereal *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *, doublereal *, 
	    doublecomplex *, doublecomplex *, integer *);
    extern logical zlctsx_();
    extern /* Subroutine */ int ztgsyl_(char *, integer *, integer *, integer 
	    *, doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, integer *, integer *, 
	     integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRGG tests the error exits for ZGGES, ZGGESX, ZGGEV, ZGGEVX, */
/*  ZGGGLM, ZGGHRD, ZGGLSE, ZGGQRF, ZGGRQF, ZGGSVD, ZGGSVP, ZHGEQZ, */
/*  ZTGEVC, ZTGEXC, ZTGSEN, ZTGSJA, ZTGSNA, and ZTGSYL. */

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
	    i__1 = i__ + j * 3 - 4;
	    a[i__1].r = 0., a[i__1].i = 0.;
	    i__1 = i__ + j * 3 - 4;
	    b[i__1].r = 0., b[i__1].i = 0.;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 3; ++i__) {
	i__1 = i__ + i__ * 3 - 4;
	a[i__1].r = 1., a[i__1].i = 0.;
	i__1 = i__ + i__ * 3 - 4;
	b[i__1].r = 1., b[i__1].i = 0.;
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

/*        ZGGHRD */

	s_copy(srnamc_1.srnamt, "ZGGHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgghrd_("/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgghrd_("N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgghrd_("N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgghrd_("N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgghrd_("N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zgghrd_("V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zgghrd_("N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("ZGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        ZHGEQZ */

	s_copy(srnamc_1.srnamt, "ZHGEQZ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhgeqz_("/", "N", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhgeqz_("E", "/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhgeqz_("E", "N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhgeqz_("E", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zhgeqz_("E", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zhgeqz_("E", "N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zhgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zhgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zhgeqz_("E", "V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zhgeqz_("E", "N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("ZHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        ZTGEVC */

	s_copy(srnamc_1.srnamt, "ZTGEVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztgevc_("/", "A", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztgevc_("R", "/", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztgevc_("R", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztgevc_("R", "A", sel, &c__2, a, &c__1, b, &c__2, q, &c__1, z__, &
		c__2, &c__0, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__1, q, &c__1, z__, &
		c__2, &c__0, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztgevc_("L", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ztgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ztgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__2, &c__1, &m, w, rw, &info);
	chkxer_("ZTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GSV path. */

    } else if (lsamen_(&c__3, path, "GSV")) {

/*        ZGGSVD */

	s_copy(srnamc_1.srnamt, "ZGGSVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggsvd_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggsvd_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggsvd_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zggsvd_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggsvd_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zggsvd_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zggsvd_("N", "N", "N", &c__2, &c__1, &c__1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zggsvd_("N", "N", "N", &c__1, &c__1, &c__2, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zggsvd_("U", "N", "N", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	zggsvd_("N", "V", "N", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__2, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	zggsvd_("N", "N", "Q", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__2, v, &c__2, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("ZGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        ZGGSVP */

	s_copy(srnamc_1.srnamt, "ZGGSVP", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggsvp_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggsvp_("N", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggsvp_("N", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zggsvp_("N", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggsvp_("N", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zggsvp_("N", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zggsvp_("N", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zggsvp_("N", "N", "N", &c__1, &c__2, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zggsvp_("U", "N", "N", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	zggsvp_("N", "V", "N", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__2, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	zggsvp_("N", "N", "Q", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__2, v, &c__2, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("ZGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        ZTGSJA */

	s_copy(srnamc_1.srnamt, "ZTGSJA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztgsja_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztgsja_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztgsja_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztgsja_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztgsja_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztgsja_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__0, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ztgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__0, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	ztgsja_("U", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__0, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	ztgsja_("N", "V", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__0, q, &
		c__1, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	ztgsja_("N", "N", "Q", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__0, w, &ncycle, &info);
	chkxer_("ZTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*     Test error exits for the GLM path. */

    } else if (lsamen_(&c__3, path, "GLM")) {

/*        ZGGGLM */

	s_copy(srnamc_1.srnamt, "ZGGGLM", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggglm_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggglm_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggglm_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggglm_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggglm_(&c__1, &c__0, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggglm_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zggglm_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zggglm_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__1, &info);
	chkxer_("ZGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the LSE path. */

    } else if (lsamen_(&c__3, path, "LSE")) {

/*        ZGGLSE */

	s_copy(srnamc_1.srnamt, "ZGGLSE", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgglse_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgglse_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgglse_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgglse_(&c__0, &c__0, &c__1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgglse_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgglse_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgglse_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgglse_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__1, &info);
	chkxer_("ZGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GQR path. */

    } else if (lsamen_(&c__3, path, "GQR")) {

/*        ZGGQRF */

	s_copy(srnamc_1.srnamt, "ZGGQRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggqrf_(&c_n1, &c__0, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggqrf_(&c__0, &c_n1, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggqrf_(&c__0, &c__0, &c_n1, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggqrf_(&c__0, &c__0, &c__0, a, &c__0, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zggqrf_(&c__0, &c__0, &c__0, a, &c__1, alpha, b, &c__0, beta, w, &
		c__18, &info);
	chkxer_("ZGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zggqrf_(&c__1, &c__1, &c__2, a, &c__1, alpha, b, &c__1, beta, w, &
		c__1, &info);
	chkxer_("ZGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        ZGGRQF */

	s_copy(srnamc_1.srnamt, "ZGGRQF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggrqf_(&c_n1, &c__0, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggrqf_(&c__0, &c_n1, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggrqf_(&c__0, &c__0, &c_n1, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggrqf_(&c__0, &c__0, &c__0, a, &c__0, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("ZGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zggrqf_(&c__0, &c__0, &c__0, a, &c__1, alpha, b, &c__0, beta, w, &
		c__18, &info);
	chkxer_("ZGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zggrqf_(&c__1, &c__1, &c__2, a, &c__1, alpha, b, &c__1, beta, w, &
		c__1, &info);
	chkxer_("ZGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*     Test error exits for the ZGS, ZGV, ZGX, and ZXV paths. */

    } else if (lsamen_(&c__3, path, "ZGS") || lsamen_(&
	    c__3, path, "ZGV") || lsamen_(&c__3, path, 
	    "ZGX") || lsamen_(&c__3, path, "ZXV")) {

/*        ZGGES */

	s_copy(srnamc_1.srnamt, "ZGGES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgges_("/", "N", "S", (L_fp)zlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgges_("N", "/", "S", (L_fp)zlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgges_("N", "V", "/", (L_fp)zlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgges_("N", "V", "S", (L_fp)zlctes_, &c_n1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgges_("N", "V", "S", (L_fp)zlctes_, &c__1, a, &c__0, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zgges_("N", "V", "S", (L_fp)zlctes_, &c__1, a, &c__1, b, &c__0, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zgges_("N", "V", "S", (L_fp)zlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__0, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zgges_("V", "V", "S", (L_fp)zlctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 alpha, beta, q, &c__1, u, &c__2, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zgges_("N", "V", "S", (L_fp)zlctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__0, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zgges_("V", "V", "S", (L_fp)zlctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 alpha, beta, q, &c__2, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	zgges_("V", "V", "S", (L_fp)zlctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 alpha, beta, q, &c__2, u, &c__2, w, &c__1, rw, bw, &info);
	chkxer_("ZGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        ZGGESX */

	s_copy(srnamc_1.srnamt, "ZGGESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggesx_("/", "N", "S", (L_fp)zlctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggesx_("N", "/", "S", (L_fp)zlctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggesx_("V", "V", "/", (L_fp)zlctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "/", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c_n1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c__1, a, &c__0, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c__1, a, &c__1, b, &c__0, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__0, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__0, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, alpha, beta, q, &c__2, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, alpha, beta, q, &c__2, u, &c__2, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	zggesx_("V", "V", "S", (L_fp)zlctsx_, "V", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__32, 
		rw, iw, &c__0, bw, &info);
	chkxer_("ZGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        ZGGEV */

	s_copy(srnamc_1.srnamt, "ZGGEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggev_("/", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggev_("N", "/", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggev_("V", "V", &c_n1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggev_("V", "V", &c__1, a, &c__0, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zggev_("V", "V", &c__1, a, &c__1, b, &c__0, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zggev_("N", "V", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__0, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zggev_("V", "V", &c__2, a, &c__2, b, &c__2, alpha, beta, q, &c__1, u, 
		&c__2, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zggev_("V", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, &c__2, u, 
		&c__0, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zggev_("V", "V", &c__2, a, &c__2, b, &c__2, alpha, beta, q, &c__2, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zggev_("V", "V", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("ZGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        ZGGEVX */

	s_copy(srnamc_1.srnamt, "ZGGEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zggevx_("/", "N", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zggevx_("N", "/", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zggevx_("N", "N", "/", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zggevx_("N", "N", "N", "/", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zggevx_("N", "N", "N", "N", &c_n1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zggevx_("N", "N", "N", "N", &c__1, a, &c__0, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__0, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__0, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zggevx_("N", "V", "N", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, 
		 &c__1, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__0, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, 
		 &c__2, u, &c__1, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 25;
	zggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, 
		 &c__2, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__0, rw, iw, bw, &info);
	chkxer_("ZGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        ZTGEXC */

	s_copy(srnamc_1.srnamt, "ZTGEXC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 3;
	ztgexc_(&c_true, &c_true, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("ZTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztgexc_(&c_true, &c_true, &c__1, a, &c__0, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("ZTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ztgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__0, q, &c__1, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("ZTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ztgexc_(&c_false, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("ZTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ztgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("ZTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ztgexc_(&c_true, &c_false, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, &info);
	chkxer_("ZTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ztgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, &info);
	chkxer_("ZTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        ZTGSEN */

	s_copy(srnamc_1.srnamt, "ZTGSEN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztgsen_(&c_n1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztgsen_(&c__1, &c_true, &c_true, sel, &c_n1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ztgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__0, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ztgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__0, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ztgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__0, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	ztgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__0, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	ztgsen_(&c__3, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c_n5, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 23;
	ztgsen_(&c__0, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 23;
	ztgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 23;
	ztgsen_(&c__5, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__1, &info);
	chkxer_("ZTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        ZTGSNA */

	s_copy(srnamc_1.srnamt, "ZTGSNA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztgsna_("/", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztgsna_("B", "/", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztgsna_("B", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztgsna_("B", "A", sel, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztgsna_("B", "A", sel, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ztgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__0, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	ztgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__0, &m, w, &c__1, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	ztgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__0, iw, &info);
	chkxer_("ZTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        ZTGSYL */

	s_copy(srnamc_1.srnamt, "ZTGSYL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztgsyl_("/", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztgsyl_("N", &c_n1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztgsyl_("N", &c__0, &c__0, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztgsyl_("N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztgsyl_("N", &c__0, &c__1, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ztgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__0, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	ztgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__0, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	ztgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__0, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	ztgsyl_("N", &c__1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	ztgsyl_("N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("ZTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___40.ciunit = infoc_1.nout;
	s_wsfe(&io___40);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___41.ciunit = infoc_1.nout;
	s_wsfe(&io___41);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of ZERRGG */

} /* zerrgg_ */
