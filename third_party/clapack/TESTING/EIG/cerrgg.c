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

/* Subroutine */ int cerrgg_(char *path, integer *nunit)
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
    complex a[9]	/* was [3][3] */, b[9]	/* was [3][3] */;
    integer i__, j, m;
    complex q[9]	/* was [3][3] */, u[9]	/* was [3][3] */, v[9]	/* 
	    was [3][3] */, w[18], z__[9]	/* was [3][3] */;
    char c2[2];
    real r1[3], r2[3];
    logical bw[3];
    real ls[3];
    integer iw[18], nt;
    real rs[3], rw[18], dif, rce[3];
    logical sel[3];
    complex tau[3];
    real rcv[3];
    complex beta[3];
    integer info, sdim;
    real anrm, bnrm, tola, tolb;
    integer ifst, ilst;
    complex alpha[3];
    real scale;
    extern /* Subroutine */ int cgges_(char *, char *, char *, L_fp, integer *
, complex *, integer *, complex *, integer *, integer *, complex *
, complex *, complex *, integer *, complex *, integer *, complex *
, integer *, real *, logical *, integer *)
	    , cggev_(char *, char *, integer *, complex *, integer *, complex 
	    *, integer *, complex *, complex *, complex *, integer *, complex 
	    *, integer *, complex *, integer *, real *, integer *), cgghrd_(char *, char *, integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, integer *), cggglm_(integer 
	    *, integer *, integer *, complex *, integer *, complex *, integer 
	    *, complex *, complex *, complex *, complex *, integer *, integer 
	    *), cgglse_(integer *, integer *, integer *, complex *, integer *, 
	     complex *, integer *, complex *, complex *, complex *, complex *, 
	     integer *, integer *), cggqrf_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, complex *, 
	    complex *, integer *, integer *), cggrqf_(integer *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, complex *, integer *, integer *), ctgevc_(char *, char 
	    *, logical *, integer *, complex *, integer *, complex *, integer 
	    *, complex *, integer *, complex *, integer *, integer *, integer 
	    *, complex *, real *, integer *);
    integer ncycle;
    extern logical clctes_(), lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int cggesx_(char *, char *, char *, L_fp, char *, 
	    integer *, complex *, integer *, complex *, integer *, integer *, 
	    complex *, complex *, complex *, integer *, complex *, integer *, 
	    real *, real *, complex *, integer *, real *, integer *, integer *
, logical *, integer *), cggsvd_(
	    char *, char *, char *, integer *, integer *, integer *, integer *
, integer *, complex *, integer *, complex *, integer *, real *, 
	    real *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, complex *, real *, integer *, integer *), chgeqz_(char *, char *, char *, integer *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, integer *), 
	    cggevx_(char *, char *, char *, char *, integer *, complex *, 
	    integer *, complex *, integer *, complex *, complex *, complex *, 
	    integer *, complex *, integer *, integer *, integer *, real *, 
	    real *, real *, real *, real *, real *, complex *, integer *, 
	    real *, integer *, logical *, integer *), chkxer_(char *, integer *, integer *, logical *, logical 
	    *), ctgexc_(logical *, logical *, integer *, complex *, 
	    integer *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, integer *, integer *, integer *), ctgsen_(integer *, 
	    logical *, logical *, logical *, integer *, complex *, integer *, 
	    complex *, integer *, complex *, complex *, complex *, integer *, 
	    complex *, integer *, integer *, real *, real *, real *, complex *
, integer *, integer *, integer *, integer *), ctgsja_(char *, 
	    char *, char *, integer *, integer *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, real *, 
	    real *, real *, real *, complex *, integer *, complex *, integer *
, complex *, integer *, complex *, integer *, integer *), ctgsna_(char *, char *, logical *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *, integer *, integer *, 
	    complex *, integer *, integer *, integer *), 
	    cggsvp_(char *, char *, char *, integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, integer *, real *, complex *, complex *, 
	    integer *);
    extern logical clctsx_();
    extern /* Subroutine */ int ctgsyl_(char *, integer *, integer *, integer 
	    *, complex *, integer *, complex *, integer *, complex *, integer 
	    *, complex *, integer *, complex *, integer *, complex *, integer 
	    *, real *, real *, complex *, integer *, integer *, integer *);
    integer dummyk, dummyl;

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

/*  CERRGG tests the error exits for CGGES, CGGESX, CGGEV, CGGEVX, */
/*  CGGGLM, CGGHRD, CGGLSE, CGGQRF, CGGRQF, CGGSVD, CGGSVP, CHGEQZ, */
/*  CTGEVC, CTGEXC, CTGSEN, CTGSJA, CTGSNA, and CTGSYL. */

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
	    a[i__1].r = 0.f, a[i__1].i = 0.f;
	    i__1 = i__ + j * 3 - 4;
	    b[i__1].r = 0.f, b[i__1].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 3; ++i__) {
	i__1 = i__ + i__ * 3 - 4;
	a[i__1].r = 1.f, a[i__1].i = 0.f;
	i__1 = i__ + i__ * 3 - 4;
	b[i__1].r = 1.f, b[i__1].i = 0.f;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    tola = 1.f;
    tolb = 1.f;
    ifst = 1;
    ilst = 1;
    nt = 0;

/*     Test error exits for the GG path. */

    if (lsamen_(&c__2, c2, "GG")) {

/*        CGGHRD */

	s_copy(srnamc_1.srnamt, "CGGHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgghrd_("/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgghrd_("N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgghrd_("N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgghrd_("N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgghrd_("N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cgghrd_("V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cgghrd_("N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("CGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        CHGEQZ */

	s_copy(srnamc_1.srnamt, "CHGEQZ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chgeqz_("/", "N", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chgeqz_("E", "/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chgeqz_("E", "N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chgeqz_("E", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	chgeqz_("E", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	chgeqz_("E", "N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	chgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	chgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	chgeqz_("E", "V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	chgeqz_("E", "N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, alpha, 
		 beta, q, &c__1, z__, &c__1, w, &c__1, rw, &info);
	chkxer_("CHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        CTGEVC */

	s_copy(srnamc_1.srnamt, "CTGEVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctgevc_("/", "A", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctgevc_("R", "/", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctgevc_("R", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctgevc_("R", "A", sel, &c__2, a, &c__1, b, &c__2, q, &c__1, z__, &
		c__2, &c__0, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__1, q, &c__1, z__, &
		c__2, &c__0, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctgevc_("L", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ctgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ctgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__2, &c__1, &m, w, rw, &info);
	chkxer_("CTGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GSV path. */

    } else if (lsamen_(&c__3, path, "GSV")) {

/*        CGGSVD */

	s_copy(srnamc_1.srnamt, "CGGSVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggsvd_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggsvd_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggsvd_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cggsvd_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggsvd_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cggsvd_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cggsvd_("N", "N", "N", &c__2, &c__1, &c__1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cggsvd_("N", "N", "N", &c__1, &c__1, &c__2, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	cggsvd_("U", "N", "N", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	cggsvd_("N", "V", "N", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__2, v, &c__1, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	cggsvd_("N", "N", "Q", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__2, v, &c__2, q, &c__1, w, rw, 
		iw, &info);
	chkxer_("CGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        CGGSVP */

	s_copy(srnamc_1.srnamt, "CGGSVP", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggsvp_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggsvp_("N", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggsvp_("N", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cggsvp_("N", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggsvp_("N", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cggsvp_("N", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cggsvp_("N", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cggsvp_("N", "N", "N", &c__1, &c__2, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	cggsvp_("U", "N", "N", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	cggsvp_("N", "V", "N", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__2, v, &c__1, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	cggsvp_("N", "N", "Q", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__2, v, &c__2, q, &c__1, iw, 
		rw, tau, w, &info);
	chkxer_("CGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        CTGSJA */

	s_copy(srnamc_1.srnamt, "CTGSJA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctgsja_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctgsja_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctgsja_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctgsja_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctgsja_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctgsja_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__0, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ctgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__0, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	ctgsja_("U", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__0, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	ctgsja_("N", "V", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__0, q, &
		c__1, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	ctgsja_("N", "N", "Q", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__0, w, &ncycle, &info);
	chkxer_("CTGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*     Test error exits for the GLM path. */

    } else if (lsamen_(&c__3, path, "GLM")) {

/*        CGGGLM */

	s_copy(srnamc_1.srnamt, "CGGGLM", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggglm_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggglm_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggglm_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggglm_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggglm_(&c__1, &c__0, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggglm_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cggglm_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cggglm_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__1, &info);
	chkxer_("CGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the LSE path. */

    } else if (lsamen_(&c__3, path, "LSE")) {

/*        CGGLSE */

	s_copy(srnamc_1.srnamt, "CGGLSE", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgglse_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgglse_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgglse_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgglse_(&c__0, &c__0, &c__1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgglse_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgglse_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgglse_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, tau, alpha, beta, w, 
		&c__18, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cgglse_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, tau, alpha, beta, w, 
		&c__1, &info);
	chkxer_("CGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GQR path. */

    } else if (lsamen_(&c__3, path, "GQR")) {

/*        CGGQRF */

	s_copy(srnamc_1.srnamt, "CGGQRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggqrf_(&c_n1, &c__0, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggqrf_(&c__0, &c_n1, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggqrf_(&c__0, &c__0, &c_n1, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggqrf_(&c__0, &c__0, &c__0, a, &c__0, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cggqrf_(&c__0, &c__0, &c__0, a, &c__1, alpha, b, &c__0, beta, w, &
		c__18, &info);
	chkxer_("CGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cggqrf_(&c__1, &c__1, &c__2, a, &c__1, alpha, b, &c__1, beta, w, &
		c__1, &info);
	chkxer_("CGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        CGGRQF */

	s_copy(srnamc_1.srnamt, "CGGRQF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggrqf_(&c_n1, &c__0, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggrqf_(&c__0, &c_n1, &c__0, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggrqf_(&c__0, &c__0, &c_n1, a, &c__1, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggrqf_(&c__0, &c__0, &c__0, a, &c__0, alpha, b, &c__1, beta, w, &
		c__18, &info);
	chkxer_("CGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cggrqf_(&c__0, &c__0, &c__0, a, &c__1, alpha, b, &c__0, beta, w, &
		c__18, &info);
	chkxer_("CGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cggrqf_(&c__1, &c__1, &c__2, a, &c__1, alpha, b, &c__1, beta, w, &
		c__1, &info);
	chkxer_("CGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*     Test error exits for the CGS, CGV, CGX, and CXV paths. */

    } else if (lsamen_(&c__3, path, "CGS") || lsamen_(&
	    c__3, path, "CGV") || lsamen_(&c__3, path, 
	    "CGX") || lsamen_(&c__3, path, "CXV")) {

/*        CGGES */

	s_copy(srnamc_1.srnamt, "CGGES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgges_("/", "N", "S", (L_fp)clctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgges_("N", "/", "S", (L_fp)clctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgges_("N", "V", "/", (L_fp)clctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgges_("N", "V", "S", (L_fp)clctes_, &c_n1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgges_("N", "V", "S", (L_fp)clctes_, &c__1, a, &c__0, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cgges_("N", "V", "S", (L_fp)clctes_, &c__1, a, &c__1, b, &c__0, &sdim, 
		 alpha, beta, q, &c__1, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	cgges_("N", "V", "S", (L_fp)clctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__0, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	cgges_("V", "V", "S", (L_fp)clctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 alpha, beta, q, &c__1, u, &c__2, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	cgges_("N", "V", "S", (L_fp)clctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 alpha, beta, q, &c__1, u, &c__0, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	cgges_("V", "V", "S", (L_fp)clctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 alpha, beta, q, &c__2, u, &c__1, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	cgges_("V", "V", "S", (L_fp)clctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 alpha, beta, q, &c__2, u, &c__2, w, &c__1, rw, bw, &info);
	chkxer_("CGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        CGGESX */

	s_copy(srnamc_1.srnamt, "CGGESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggesx_("/", "N", "S", (L_fp)clctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggesx_("N", "/", "S", (L_fp)clctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggesx_("V", "V", "/", (L_fp)clctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "/", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c_n1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c__1, a, &c__0, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c__1, a, &c__1, b, &c__0, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__0, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__0, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, alpha, beta, q, &c__2, u, &c__1, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, alpha, beta, q, &c__2, u, &c__2, rce, rcv, w, &c__1, 
		rw, iw, &c__1, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	cggesx_("V", "V", "S", (L_fp)clctsx_, "V", &c__1, a, &c__1, b, &c__1, 
		&sdim, alpha, beta, q, &c__1, u, &c__1, rce, rcv, w, &c__32, 
		rw, iw, &c__0, bw, &info);
	chkxer_("CGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        CGGEV */

	s_copy(srnamc_1.srnamt, "CGGEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggev_("/", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggev_("N", "/", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggev_("V", "V", &c_n1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggev_("V", "V", &c__1, a, &c__0, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cggev_("V", "V", &c__1, a, &c__1, b, &c__0, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cggev_("N", "V", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__0, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cggev_("V", "V", &c__2, a, &c__2, b, &c__2, alpha, beta, q, &c__1, u, 
		&c__2, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cggev_("V", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, &c__2, u, 
		&c__0, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cggev_("V", "V", &c__2, a, &c__2, b, &c__2, alpha, beta, q, &c__2, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cggev_("V", "V", &c__1, a, &c__1, b, &c__1, alpha, beta, q, &c__1, u, 
		&c__1, w, &c__1, rw, &info);
	chkxer_("CGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        CGGEVX */

	s_copy(srnamc_1.srnamt, "CGGEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cggevx_("/", "N", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cggevx_("N", "/", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cggevx_("N", "N", "/", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cggevx_("N", "N", "N", "/", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cggevx_("N", "N", "N", "N", &c_n1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cggevx_("N", "N", "N", "N", &c__1, a, &c__0, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__0, alpha, beta, q, 
		 &c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__0, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cggevx_("N", "V", "N", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, 
		 &c__1, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, alpha, beta, q, 
		 &c__1, u, &c__0, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, 
		 &c__2, u, &c__1, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__1, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 25;
	cggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, alpha, beta, q, 
		 &c__2, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, 
		rcv, w, &c__0, rw, iw, bw, &info);
	chkxer_("CGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        CTGEXC */

	s_copy(srnamc_1.srnamt, "CTGEXC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 3;
	ctgexc_(&c_true, &c_true, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("CTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctgexc_(&c_true, &c_true, &c__1, a, &c__0, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("CTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ctgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__0, q, &c__1, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("CTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ctgexc_(&c_false, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("CTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ctgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, &info);
	chkxer_("CTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ctgexc_(&c_true, &c_false, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, &info);
	chkxer_("CTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ctgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, &info);
	chkxer_("CTGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

/*        CTGSEN */

	s_copy(srnamc_1.srnamt, "CTGSEN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctgsen_(&c_n1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctgsen_(&c__1, &c_true, &c_true, sel, &c_n1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ctgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__0, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ctgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__0, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ctgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__0, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	ctgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__0, &m, &tola, &tolb, rcv, w, &
		c__1, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	ctgsen_(&c__3, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c_n5, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 23;
	ctgsen_(&c__0, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 23;
	ctgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 23;
	ctgsen_(&c__5, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, 
		alpha, beta, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__1, &info);
	chkxer_("CTGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        CTGSNA */

	s_copy(srnamc_1.srnamt, "CTGSNA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctgsna_("/", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctgsna_("B", "/", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctgsna_("B", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctgsna_("B", "A", sel, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctgsna_("B", "A", sel, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ctgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__0, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	ctgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__0, &m, w, &c__1, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	ctgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__0, iw, &info);
	chkxer_("CTGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        CTGSYL */

	s_copy(srnamc_1.srnamt, "CTGSYL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctgsyl_("/", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctgsyl_("N", &c_n1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctgsyl_("N", &c__0, &c__0, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctgsyl_("N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctgsyl_("N", &c__0, &c__1, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ctgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__0, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	ctgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__0, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	ctgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__0, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	ctgsyl_("N", &c__1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	ctgsyl_("N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("CTGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of CERRGG */

} /* cerrgg_ */
