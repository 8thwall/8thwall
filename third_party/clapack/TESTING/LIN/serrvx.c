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
static integer c_n1 = -1;
static integer c__0 = 0;
static integer c__1 = 1;
static integer c__3 = 3;
static integer c__4 = 4;

/* Subroutine */ int serrvx_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 drivers passed the tests of the er"
	    "ror exits\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 drivers failed the test"
	    "s of the error \002,\002exits ***\002)";

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    real a[16]	/* was [4][4] */, b[4], c__[4];
    integer i__, j;
    real r__[4], w[8], x[4];
    char c2[2];
    real r1[4], r2[4], af[16]	/* was [4][4] */;
    char eq[1];
    integer ip[4], iw[4], info;
    real rcond;
    extern /* Subroutine */ int sgbsv_(integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, real *, integer *, 
	    integer *), sgesv_(integer *, integer *, real *, integer *, 
	    integer *, real *, integer *, integer *), spbsv_(char *, integer *
, integer *, integer *, real *, integer *, real *, integer *, 
	    integer *), sgtsv_(integer *, integer *, real *, real *, 
	    real *, real *, integer *, integer *), sposv_(char *, integer *, 
	    integer *, real *, integer *, real *, integer *, integer *), sppsv_(char *, integer *, integer *, real *, real *, 
	    integer *, integer *), sspsv_(char *, integer *, integer *
, real *, integer *, real *, integer *, integer *), 
	    sptsv_(integer *, integer *, real *, real *, real *, integer *, 
	    integer *), ssysv_(char *, integer *, integer *, real *, integer *
, integer *, real *, integer *, real *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sgbsvx_(char *, char *, integer *, integer 
	    *, integer *, integer *, real *, integer *, real *, integer *, 
	    integer *, char *, real *, real *, real *, integer *, real *, 
	    integer *, real *, real *, real *, real *, integer *, integer *), sgesvx_(char *, char *, integer *, 
	    integer *, real *, integer *, real *, integer *, integer *, char *
, real *, real *, real *, integer *, real *, integer *, real *, 
	    real *, real *, real *, integer *, integer *), spbsvx_(char *, char *, integer *, integer *, integer *, 
	    real *, integer *, real *, integer *, char *, real *, real *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    integer *, integer *), sgtsvx_(char *, 
	    char *, integer *, integer *, real *, real *, real *, real *, 
	    real *, real *, real *, integer *, real *, integer *, real *, 
	    integer *, real *, real *, real *, real *, integer *, integer *), sposvx_(char *, char *, integer *, integer *, 
	    real *, integer *, real *, integer *, char *, real *, real *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    integer *, integer *), sppsvx_(char *, 
	    char *, integer *, integer *, real *, real *, char *, real *, 
	    real *, integer *, real *, integer *, real *, real *, real *, 
	    real *, integer *, integer *), sspsvx_(
	    char *, char *, integer *, integer *, real *, real *, integer *, 
	    real *, integer *, real *, integer *, real *, real *, real *, 
	    real *, integer *, integer *), sptsvx_(char *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    integer *), ssysvx_(char *, char *, integer *, integer *, 
	    real *, integer *, real *, integer *, integer *, real *, integer *
, real *, integer *, real *, real *, real *, real *, integer *, 
	    integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRVX tests the error exits for the REAL driver routines */
/*  for solving linear systems of equations. */

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

    for (j = 1; j <= 4; ++j) {
	for (i__ = 1; i__ <= 4; ++i__) {
	    a[i__ + (j << 2) - 5] = 1.f / (real) (i__ + j);
	    af[i__ + (j << 2) - 5] = 1.f / (real) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.f;
	r1[j - 1] = 0.f;
	r2[j - 1] = 0.f;
	w[j - 1] = 0.f;
	x[j - 1] = 0.f;
	c__[j - 1] = 0.f;
	r__[j - 1] = 0.f;
	ip[j - 1] = j;
/* L20: */
    }
    *(unsigned char *)eq = ' ';
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GE")) {

/*        SGESV */

	s_copy(srnamc_1.srnamt, "SGESV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgesv_(&c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgesv_(&c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgesv_(&c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("SGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgesv_(&c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("SGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGESVX */

	s_copy(srnamc_1.srnamt, "SGESVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgesvx_("/", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgesvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgesvx_("N", "N", &c_n1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgesvx_("N", "N", &c__0, &c_n1, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgesvx_("N", "N", &c__2, &c__1, a, &c__1, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__1, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	sgesvx_("F", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'R';
	sgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = 'C';
	sgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        SGBSV */

	s_copy(srnamc_1.srnamt, "SGBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbsv_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbsv_(&c__1, &c_n1, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbsv_(&c__1, &c__0, &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbsv_(&c__0, &c__0, &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgbsv_(&c__1, &c__1, &c__1, &c__0, a, &c__3, ip, b, &c__1, &info);
	chkxer_("SGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sgbsv_(&c__2, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGBSVX */

	s_copy(srnamc_1.srnamt, "SGBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbsvx_("/", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbsvx_("N", "/", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbsvx_("N", "N", &c_n1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbsvx_("N", "N", &c__1, &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgbsvx_("N", "N", &c__1, &c__0, &c_n1, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgbsvx_("N", "N", &c__0, &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__2, af, &c__4, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__3, af, &c__3, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = '/';
	sgbsvx_("F", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	*(unsigned char *)eq = 'R';
	sgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	*(unsigned char *)eq = 'C';
	sgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	sgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("SGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GT")) {

/*        SGTSV */

	s_copy(srnamc_1.srnamt, "SGTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgtsv_(&c_n1, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("SGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgtsv_(&c__0, &c_n1, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("SGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgtsv_(&c__2, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("SGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGTSVX */

	s_copy(srnamc_1.srnamt, "SGTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgtsvx_("/", "N", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgtsvx_("N", "/", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgtsvx_("N", "N", &c_n1, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgtsvx_("N", "N", &c__0, &c_n1, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PO")) {

/*        SPOSV */

	s_copy(srnamc_1.srnamt, "SPOSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sposv_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("SPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sposv_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("SPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sposv_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("SPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sposv_("U", &c__2, &c__0, a, &c__1, b, &c__2, &info);
	chkxer_("SPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sposv_("U", &c__2, &c__0, a, &c__2, b, &c__1, &info);
	chkxer_("SPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPOSVX */

	s_copy(srnamc_1.srnamt, "SPOSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sposvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sposvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sposvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sposvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sposvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	*(unsigned char *)eq = '/';
	sposvx_("F", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = 'Y';
	sposvx_("F", "U", &c__1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        SPPSV */

	s_copy(srnamc_1.srnamt, "SPPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sppsv_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("SPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sppsv_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("SPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sppsv_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("SPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sppsv_("U", &c__2, &c__0, a, b, &c__1, &info);
	chkxer_("SPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPPSVX */

	s_copy(srnamc_1.srnamt, "SPPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sppsvx_("/", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sppsvx_("N", "/", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sppsvx_("N", "U", &c_n1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sppsvx_("N", "U", &c__0, &c_n1, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	*(unsigned char *)eq = '/';
	sppsvx_("F", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	*(unsigned char *)eq = 'Y';
	sppsvx_("F", "U", &c__1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__1, x, &c__2, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__2, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("SPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        SPBSV */

	s_copy(srnamc_1.srnamt, "SPBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbsv_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("SPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbsv_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("SPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbsv_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("SPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spbsv_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info)
		;
	chkxer_("SPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	spbsv_("U", &c__1, &c__1, &c__0, a, &c__1, b, &c__2, &info)
		;
	chkxer_("SPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	spbsv_("U", &c__2, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("SPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPBSVX */

	s_copy(srnamc_1.srnamt, "SPBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbsvx_("/", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbsvx_("N", "/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbsvx_("N", "U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spbsvx_("N", "U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	spbsvx_("N", "U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	spbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__1, af, &c__2, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	spbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__2, af, &c__1, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	spbsvx_("F", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'Y';
	spbsvx_("F", "U", &c__1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	spbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	spbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("SPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        SPTSV */

	s_copy(srnamc_1.srnamt, "SPTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sptsv_(&c_n1, &c__0, a, &a[4], b, &c__1, &info);
	chkxer_("SPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sptsv_(&c__0, &c_n1, a, &a[4], b, &c__1, &info);
	chkxer_("SPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sptsv_(&c__2, &c__0, a, &a[4], b, &c__1, &info);
	chkxer_("SPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPTSVX */

	s_copy(srnamc_1.srnamt, "SPTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sptsvx_("/", &c__0, &c__0, a, &a[4], af, &af[4], b, &c__1, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("SPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sptsvx_("N", &c_n1, &c__0, a, &a[4], af, &af[4], b, &c__1, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("SPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sptsvx_("N", &c__0, &c_n1, a, &a[4], af, &af[4], b, &c__1, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("SPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sptsvx_("N", &c__2, &c__0, a, &a[4], af, &af[4], b, &c__1, x, &c__2, &
		rcond, r1, r2, w, &info);
	chkxer_("SPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sptsvx_("N", &c__2, &c__0, a, &a[4], af, &af[4], b, &c__2, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("SPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SY")) {

/*        SSYSV */

	s_copy(srnamc_1.srnamt, "SSYSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssysv_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("SSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssysv_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("SSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssysv_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("SSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssysv_("U", &c__2, &c__0, a, &c__2, ip, b, &c__1, w, &c__1, &info);
	chkxer_("SSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSYSVX */

	s_copy(srnamc_1.srnamt, "SSYSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssysvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssysvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssysvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssysvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ssysvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ssysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__1, x, 
		&c__2, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	ssysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__1, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	ssysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__3, iw, &info);
	chkxer_("SSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        SSPSV */

	s_copy(srnamc_1.srnamt, "SSPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sspsv_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("SSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sspsv_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("SSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sspsv_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("SSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sspsv_("U", &c__2, &c__0, a, ip, b, &c__1, &info);
	chkxer_("SSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSPSVX */

	s_copy(srnamc_1.srnamt, "SSPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sspsvx_("/", "U", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("SSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sspsvx_("N", "/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("SSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sspsvx_("N", "U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("SSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sspsvx_("N", "U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("SSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("SSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("SSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___19.ciunit = infoc_1.nout;
	s_wsfe(&io___19);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    } else {
	io___20.ciunit = infoc_1.nout;
	s_wsfe(&io___20);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of SERRVX */

} /* serrvx_ */
