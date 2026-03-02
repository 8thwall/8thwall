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

/* Subroutine */ int serrgg_(char *path, integer *nunit)
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
    real a[9]	/* was [3][3] */, b[9]	/* was [3][3] */;
    integer i__, j, m;
    real q[9]	/* was [3][3] */, u[9]	/* was [3][3] */, v[9]	/* was [3][3] 
	    */, w[18], z__[9]	/* was [3][3] */;
    char c2[2];
    real r1[3], r2[3], r3[3];
    logical bw[3];
    real ls[3];
    integer iw[3], nt;
    real rs[3], dif, rce[2];
    logical sel[3];
    real tau[3], rcv[2];
    integer info, sdim;
    real anrm, bnrm, tola, tolb;
    integer ifst, ilst;
    real scale;
    extern /* Subroutine */ int sgges_(char *, char *, char *, L_fp, integer *
, real *, integer *, real *, integer *, integer *, real *, real *, 
	     real *, real *, integer *, real *, integer *, real *, integer *, 
	    logical *, integer *), sggev_(char *, 
	    char *, integer *, real *, integer *, real *, integer *, real *, 
	    real *, real *, real *, integer *, real *, integer *, real *, 
	    integer *, integer *);
    integer ncycle;
    extern /* Subroutine */ int sgghrd_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, real *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int sggglm_(integer *, integer *, integer *, real 
	    *, integer *, real *, integer *, real *, real *, real *, real *, 
	    integer *, integer *), chkxer_(char *, integer *, integer *, 
	    logical *, logical *), sgglse_(integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, real *, 
	    real *, real *, integer *, integer *), sggqrf_(integer *, integer 
	    *, integer *, real *, integer *, real *, real *, integer *, real *
, real *, integer *, integer *), sggrqf_(integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    real *, integer *, integer *), stgevc_(char *, char *, logical *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, real *, integer *, integer *, integer *, real *, integer *);
    extern logical slctes_();
    extern /* Subroutine */ int sggsvd_(char *, char *, char *, integer *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    real *, integer *, real *, real *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *, integer *), stgexc_(logical *, logical *, integer *, 
	    real *, integer *, real *, integer *, real *, integer *, real *, 
	    integer *, integer *, integer *, real *, integer *, integer *), 
	    sggesx_(char *, char *, char *, L_fp, char *, integer *, real *, 
	    integer *, real *, integer *, integer *, real *, real *, real *, 
	    real *, integer *, real *, integer *, real *, real *, real *, 
	    integer *, integer *, integer *, logical *, integer *), shgeqz_(char *, char *, char *, integer *
, integer *, integer *, real *, integer *, real *, integer *, 
	    real *, real *, real *, real *, integer *, real *, integer *, 
	    real *, integer *, integer *), stgsja_(
	    char *, char *, char *, integer *, integer *, integer *, integer *
, integer *, real *, integer *, real *, integer *, real *, real *, 
	     real *, real *, real *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, integer *), 
	    sggevx_(char *, char *, char *, char *, integer *, real *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    integer *, real *, integer *, integer *, integer *, real *, real *
, real *, real *, real *, real *, real *, integer *, integer *, 
	    logical *, integer *), stgsen_(
	    integer *, logical *, logical *, logical *, integer *, real *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    integer *, real *, integer *, integer *, real *, real *, real *, 
	    real *, integer *, integer *, integer *, integer *), stgsna_(char 
	    *, char *, logical *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *, real *, integer *, integer *, integer *);
    integer dummyk, dummyl;
    extern /* Subroutine */ int sggsvp_(char *, char *, char *, integer *, 
	    integer *, integer *, real *, integer *, real *, integer *, real *
, real *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, integer *, real *, real *, integer *
);
    extern logical slctsx_();
    extern /* Subroutine */ int stgsyl_(char *, integer *, integer *, integer 
	    *, real *, integer *, real *, integer *, real *, integer *, real *
, integer *, real *, integer *, real *, integer *, real *, real *, 
	     real *, integer *, integer *, integer *);

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

/*  SERRGG tests the error exits for SGGES, SGGESX, SGGEV, SGGEVX, */
/*  SGGGLM, SGGHRD, SGGLSE, SGGQRF, SGGRQF, SGGSVD, SGGSVP, SHGEQZ, */
/*  STGEVC, STGEXC, STGSEN, STGSJA, STGSNA, and STGSYL. */

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
	    a[i__ + j * 3 - 4] = 0.f;
	    b[i__ + j * 3 - 4] = 0.f;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 3; ++i__) {
	a[i__ + i__ * 3 - 4] = 1.f;
	b[i__ + i__ * 3 - 4] = 1.f;
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

/*        SGGHRD */

	s_copy(srnamc_1.srnamt, "SGGHRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgghrd_("/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgghrd_("N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgghrd_("N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgghrd_("N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgghrd_("N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sgghrd_("N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sgghrd_("V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sgghrd_("N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, q, &c__1, 
		z__, &c__1, &info);
	chkxer_("SGGHRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        SHGEQZ */

	s_copy(srnamc_1.srnamt, "SHGEQZ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	shgeqz_("/", "N", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	shgeqz_("E", "/", "N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	shgeqz_("E", "N", "/", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	shgeqz_("E", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	shgeqz_("E", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	shgeqz_("E", "N", "N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	shgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	shgeqz_("E", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	shgeqz_("E", "V", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	shgeqz_("E", "N", "V", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, r1, 
		r2, r3, q, &c__1, z__, &c__1, w, &c__18, &info);
	chkxer_("SHGEQZ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        STGEVC */

	s_copy(srnamc_1.srnamt, "STGEVC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stgevc_("/", "A", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stgevc_("R", "/", sel, &c__0, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stgevc_("R", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	stgevc_("R", "A", sel, &c__2, a, &c__1, b, &c__2, q, &c__1, z__, &
		c__2, &c__0, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	stgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__1, q, &c__1, z__, &
		c__2, &c__0, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	stgevc_("L", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	stgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__1, &c__0, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	stgevc_("R", "A", sel, &c__2, a, &c__2, b, &c__2, q, &c__1, z__, &
		c__2, &c__1, &m, w, &info);
	chkxer_("STGEVC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GSV path. */

    } else if (lsamen_(&c__3, path, "GSV")) {

/*        SGGSVD */

	s_copy(srnamc_1.srnamt, "SGGSVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggsvd_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggsvd_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggsvd_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sggsvd_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggsvd_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sggsvd_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sggsvd_("N", "N", "N", &c__2, &c__1, &c__1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sggsvd_("N", "N", "N", &c__1, &c__1, &c__2, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sggsvd_("U", "N", "N", &c__2, &c__2, &c__2, &dummyk, &dummyl, a, &
		c__2, b, &c__2, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	sggsvd_("N", "V", "N", &c__1, &c__1, &c__2, &dummyk, &dummyl, a, &
		c__1, b, &c__2, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	sggsvd_("N", "N", "Q", &c__1, &c__2, &c__1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, r1, r2, u, &c__1, v, &c__1, q, &c__1, w, iw, &
		info);
	chkxer_("SGGSVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        SGGSVP */

	s_copy(srnamc_1.srnamt, "SGGSVP", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggsvp_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggsvp_("N", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggsvp_("N", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sggsvp_("N", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggsvp_("N", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sggsvp_("N", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sggsvp_("N", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sggsvp_("N", "N", "N", &c__1, &c__2, &c__1, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sggsvp_("U", "N", "N", &c__2, &c__2, &c__2, a, &c__2, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	sggsvp_("N", "V", "N", &c__1, &c__2, &c__1, a, &c__1, b, &c__2, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	sggsvp_("N", "N", "Q", &c__1, &c__1, &c__2, a, &c__1, b, &c__1, &tola, 
		 &tolb, &dummyk, &dummyl, u, &c__1, v, &c__1, q, &c__1, iw, 
		tau, w, &info);
	chkxer_("SGGSVP", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        STGSJA */

	s_copy(srnamc_1.srnamt, "STGSJA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stgsja_("/", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stgsja_("N", "/", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stgsja_("N", "N", "/", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stgsja_("N", "N", "N", &c_n1, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stgsja_("N", "N", "N", &c__0, &c_n1, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	stgsja_("N", "N", "N", &c__0, &c__0, &c_n1, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	stgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__0, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	stgsja_("N", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__0, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	stgsja_("U", "N", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__0, v, &c__1, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	stgsja_("N", "V", "N", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__0, q, &
		c__1, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	stgsja_("N", "N", "Q", &c__0, &c__0, &c__0, &dummyk, &dummyl, a, &
		c__1, b, &c__1, &tola, &tolb, r1, r2, u, &c__1, v, &c__1, q, &
		c__0, w, &ncycle, &info);
	chkxer_("STGSJA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*     Test error exits for the GLM path. */

    } else if (lsamen_(&c__3, path, "GLM")) {

/*        SGGGLM */

	s_copy(srnamc_1.srnamt, "SGGGLM", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggglm_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggglm_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggglm_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggglm_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggglm_(&c__1, &c__0, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggglm_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sggglm_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sggglm_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, r1, r2, r3, w, &c__1, 
		 &info);
	chkxer_("SGGGLM", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the LSE path. */

    } else if (lsamen_(&c__3, path, "LSE")) {

/*        SGGLSE */

	s_copy(srnamc_1.srnamt, "SGGLSE", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgglse_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgglse_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgglse_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgglse_(&c__0, &c__0, &c__1, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgglse_(&c__0, &c__1, &c__0, a, &c__1, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgglse_(&c__0, &c__0, &c__0, a, &c__0, b, &c__1, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgglse_(&c__0, &c__0, &c__0, a, &c__1, b, &c__0, r1, r2, r3, w, &
		c__18, &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sgglse_(&c__1, &c__1, &c__1, a, &c__1, b, &c__1, r1, r2, r3, w, &c__1, 
		 &info);
	chkxer_("SGGLSE", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*     Test error exits for the GQR path. */

    } else if (lsamen_(&c__3, path, "GQR")) {

/*        SGGQRF */

	s_copy(srnamc_1.srnamt, "SGGQRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggqrf_(&c_n1, &c__0, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggqrf_(&c__0, &c_n1, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggqrf_(&c__0, &c__0, &c_n1, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggqrf_(&c__0, &c__0, &c__0, a, &c__0, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sggqrf_(&c__0, &c__0, &c__0, a, &c__1, r1, b, &c__0, r2, w, &c__18, &
		info);
	chkxer_("SGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sggqrf_(&c__1, &c__1, &c__2, a, &c__1, r1, b, &c__1, r2, w, &c__1, &
		info);
	chkxer_("SGGQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*        SGGRQF */

	s_copy(srnamc_1.srnamt, "SGGRQF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggrqf_(&c_n1, &c__0, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggrqf_(&c__0, &c_n1, &c__0, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggrqf_(&c__0, &c__0, &c_n1, a, &c__1, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggrqf_(&c__0, &c__0, &c__0, a, &c__0, r1, b, &c__1, r2, w, &c__18, &
		info);
	chkxer_("SGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sggrqf_(&c__0, &c__0, &c__0, a, &c__1, r1, b, &c__0, r2, w, &c__18, &
		info);
	chkxer_("SGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sggrqf_(&c__1, &c__1, &c__2, a, &c__1, r1, b, &c__1, r2, w, &c__1, &
		info);
	chkxer_("SGGRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

/*     Test error exits for the SGS, SGV, SGX, and SXV paths. */

    } else if (lsamen_(&c__3, path, "SGS") || lsamen_(&
	    c__3, path, "SGV") || lsamen_(&c__3, path, 
	    "SGX") || lsamen_(&c__3, path, "SXV")) {

/*        SGGES */

	s_copy(srnamc_1.srnamt, "SGGES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgges_("/", "N", "S", (L_fp)slctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgges_("N", "/", "S", (L_fp)slctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgges_("N", "V", "/", (L_fp)slctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgges_("N", "V", "S", (L_fp)slctes_, &c_n1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgges_("N", "V", "S", (L_fp)slctes_, &c__1, a, &c__0, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sgges_("N", "V", "S", (L_fp)slctes_, &c__1, a, &c__1, b, &c__0, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	sgges_("N", "V", "S", (L_fp)slctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__0, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	sgges_("V", "V", "S", (L_fp)slctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__2, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	sgges_("N", "V", "S", (L_fp)slctes_, &c__1, a, &c__1, b, &c__1, &sdim, 
		 r1, r2, r3, q, &c__1, u, &c__0, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 17;
	sgges_("V", "V", "S", (L_fp)slctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 r1, r2, r3, q, &c__2, u, &c__1, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 19;
	sgges_("V", "V", "S", (L_fp)slctes_, &c__2, a, &c__2, b, &c__2, &sdim, 
		 r1, r2, r3, q, &c__2, u, &c__2, w, &c__1, bw, &info);
	chkxer_("SGGES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

/*        SGGESX */

	s_copy(srnamc_1.srnamt, "SGGESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggesx_("/", "N", "S", (L_fp)slctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggesx_("N", "/", "S", (L_fp)slctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggesx_("V", "V", "/", (L_fp)slctsx_, "N", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "/", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c_n1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c__1, a, &c__0, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c__1, a, &c__1, b, &c__0, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__0, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__0, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, r1, r2, r3, q, &c__2, u, &c__1, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "B", &c__2, a, &c__2, b, &c__2, 
		&sdim, r1, r2, r3, q, &c__2, u, &c__2, rce, rcv, w, &c__1, iw, 
		 &c__1, bw, &info)
		;
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	sggesx_("V", "V", "S", (L_fp)slctsx_, "V", &c__1, a, &c__1, b, &c__1, 
		&sdim, r1, r2, r3, q, &c__1, u, &c__1, rce, rcv, w, &c__32, 
		iw, &c__0, bw, &info);
	chkxer_("SGGESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        SGGEV */

	s_copy(srnamc_1.srnamt, "SGGEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggev_("/", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggev_("N", "/", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggev_("V", "V", &c_n1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggev_("V", "V", &c__1, a, &c__0, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sggev_("V", "V", &c__1, a, &c__1, b, &c__0, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sggev_("N", "V", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__0, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sggev_("V", "V", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, &c__1, u, &
		c__2, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sggev_("V", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, &c__2, u, &
		c__0, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sggev_("V", "V", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, &c__2, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sggev_("V", "V", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, &c__1, u, &
		c__1, w, &c__1, &info);
	chkxer_("SGGEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        SGGEVX */

	s_copy(srnamc_1.srnamt, "SGGEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sggevx_("/", "N", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sggevx_("N", "/", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sggevx_("N", "N", "/", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sggevx_("N", "N", "N", "/", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sggevx_("N", "N", "N", "N", &c_n1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sggevx_("N", "N", "N", "N", &c__1, a, &c__0, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__0, r1, r2, r3, q, 
		&c__1, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__0, u, &c__1, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sggevx_("N", "V", "N", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, 
		&c__1, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sggevx_("N", "N", "N", "N", &c__1, a, &c__1, b, &c__1, r1, r2, r3, q, 
		&c__1, u, &c__0, &c__1, &c__1, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, 
		&c__2, u, &c__1, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 26;
	sggevx_("N", "N", "V", "N", &c__2, a, &c__2, b, &c__2, r1, r2, r3, q, 
		&c__2, u, &c__2, &c__1, &c__2, ls, rs, &anrm, &bnrm, rce, rcv, 
		 w, &c__1, iw, bw, &info);
	chkxer_("SGGEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        STGEXC */

	s_copy(srnamc_1.srnamt, "STGEXC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 3;
	stgexc_(&c_true, &c_true, &c_n1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stgexc_(&c_true, &c_true, &c__1, a, &c__0, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	stgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__0, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	stgexc_(&c_false, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	stgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__0, z__, &
		c__1, &ifst, &ilst, w, &c__1, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	stgexc_(&c_true, &c_false, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, w, &c__1, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	stgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__0, &ifst, &ilst, w, &c__1, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	stgexc_(&c_true, &c_true, &c__1, a, &c__1, b, &c__1, q, &c__1, z__, &
		c__1, &ifst, &ilst, w, &c__0, &info);
	chkxer_("STGEXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        STGSEN */

	s_copy(srnamc_1.srnamt, "STGSEN", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stgsen_(&c_n1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stgsen_(&c__1, &c_true, &c_true, sel, &c_n1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	stgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__0, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	stgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__0, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	stgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__0, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	stgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__0, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	stgsen_(&c__0, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	stgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 22;
	stgsen_(&c__2, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &c__1, 
		 iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	stgsen_(&c__0, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	stgsen_(&c__1, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__0, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 24;
	stgsen_(&c__2, &c_true, &c_true, sel, &c__1, a, &c__1, b, &c__1, r1, 
		r2, r3, q, &c__1, z__, &c__1, &m, &tola, &tolb, rcv, w, &
		c__20, iw, &c__1, &info);
	chkxer_("STGSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 12;

/*        STGSNA */

	s_copy(srnamc_1.srnamt, "STGSNA", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stgsna_("/", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stgsna_("B", "/", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stgsna_("B", "A", sel, &c_n1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	stgsna_("B", "A", sel, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	stgsna_("B", "A", sel, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	stgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	stgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__0, 
		r1, r2, &c__1, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	stgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__0, &m, w, &c__1, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	stgsna_("E", "A", sel, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &c__1, 
		r1, r2, &c__1, &m, w, &c__0, iw, &info);
	chkxer_("STGSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 9;

/*        STGSYL */

	s_copy(srnamc_1.srnamt, "STGSYL", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stgsyl_("/", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stgsyl_("N", &c_n1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stgsyl_("N", &c__0, &c__0, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stgsyl_("N", &c__0, &c__1, &c__0, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	stgsyl_("N", &c__0, &c__1, &c__1, a, &c__0, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	stgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__0, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	stgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__0, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	stgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__0, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	stgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__0, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	stgsyl_("N", &c__0, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__0, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	stgsyl_("N", &c__1, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	stgsyl_("N", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, q, &c__1, u, &
		c__1, v, &c__1, z__, &c__1, &scale, &dif, w, &c__1, iw, &info);
	chkxer_("STGSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of SERRGG */

} /* serrgg_ */
