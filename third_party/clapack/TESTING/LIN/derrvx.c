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

/* Subroutine */ int derrvx_(char *path, integer *nunit)
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
    doublereal a[16]	/* was [4][4] */, b[4], c__[4];
    integer i__, j;
    doublereal r__[4], w[8], x[4];
    char c2[2];
    doublereal r1[4], r2[4], af[16]	/* was [4][4] */;
    char eq[1];
    integer ip[4], iw[4], info;
    doublereal rcond;
    extern /* Subroutine */ int dgbsv_(integer *, integer *, integer *, 
	    integer *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *), dgesv_(integer *, integer *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, integer *), dpbsv_(
	    char *, integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, integer *), dgtsv_(integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *), dposv_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), dppsv_(char *, integer *, integer *, doublereal *, 
	    doublereal *, integer *, integer *), dspsv_(char *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, integer *), dptsv_(integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), 
	    dsysv_(char *, integer *, integer *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), dgbsvx_(char *, char *, integer *, integer 
	    *, integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, integer *, char *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), dgesvx_(char *, char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, char *, doublereal *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *), dpbsvx_(char *, char *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, char *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dgtsvx_(char *, char *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), dposvx_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, char *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dppsvx_(char *, char *, 
	    integer *, integer *, doublereal *, doublereal *, char *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *), dspsvx_(char *, char *, 
	    integer *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), dptsvx_(char *, integer *, integer *, doublereal 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *), dsysvx_(char *, 
	    char *, integer *, integer *, doublereal *, integer *, doublereal 
	    *, integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *, integer *);

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

/*  DERRVX tests the error exits for the DOUBLE PRECISION driver routines */
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
	    a[i__ + (j << 2) - 5] = 1. / (doublereal) (i__ + j);
	    af[i__ + (j << 2) - 5] = 1. / (doublereal) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.;
	r1[j - 1] = 0.;
	r2[j - 1] = 0.;
	w[j - 1] = 0.;
	x[j - 1] = 0.;
	c__[j - 1] = 0.;
	r__[j - 1] = 0.;
	ip[j - 1] = j;
/* L20: */
    }
    *(unsigned char *)eq = ' ';
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GE")) {

/*        DGESV */

	s_copy(srnamc_1.srnamt, "DGESV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgesv_(&c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgesv_(&c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgesv_(&c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("DGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgesv_(&c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("DGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGESVX */

	s_copy(srnamc_1.srnamt, "DGESVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgesvx_("/", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgesvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgesvx_("N", "N", &c_n1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgesvx_("N", "N", &c__0, &c_n1, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dgesvx_("N", "N", &c__2, &c__1, a, &c__1, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__1, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	dgesvx_("F", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'R';
	dgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = 'C';
	dgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        DGBSV */

	s_copy(srnamc_1.srnamt, "DGBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgbsv_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgbsv_(&c__1, &c_n1, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgbsv_(&c__1, &c__0, &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgbsv_(&c__0, &c__0, &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dgbsv_(&c__1, &c__1, &c__1, &c__0, a, &c__3, ip, b, &c__1, &info);
	chkxer_("DGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dgbsv_(&c__2, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGBSVX */

	s_copy(srnamc_1.srnamt, "DGBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgbsvx_("/", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgbsvx_("N", "/", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgbsvx_("N", "N", &c_n1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgbsvx_("N", "N", &c__1, &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgbsvx_("N", "N", &c__1, &c__0, &c_n1, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dgbsvx_("N", "N", &c__0, &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__2, af, &c__4, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__3, af, &c__3, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = '/';
	dgbsvx_("F", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	*(unsigned char *)eq = 'R';
	dgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	*(unsigned char *)eq = 'C';
	dgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &
		info);
	chkxer_("DGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GT")) {

/*        DGTSV */

	s_copy(srnamc_1.srnamt, "DGTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgtsv_(&c_n1, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("DGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgtsv_(&c__0, &c_n1, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("DGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgtsv_(&c__2, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("DGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGTSVX */

	s_copy(srnamc_1.srnamt, "DGTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgtsvx_("/", "N", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgtsvx_("N", "/", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgtsvx_("N", "N", &c_n1, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgtsvx_("N", "N", &c__0, &c_n1, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PO")) {

/*        DPOSV */

	s_copy(srnamc_1.srnamt, "DPOSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dposv_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("DPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dposv_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("DPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dposv_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("DPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dposv_("U", &c__2, &c__0, a, &c__1, b, &c__2, &info);
	chkxer_("DPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dposv_("U", &c__2, &c__0, a, &c__2, b, &c__1, &info);
	chkxer_("DPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPOSVX */

	s_copy(srnamc_1.srnamt, "DPOSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dposvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dposvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dposvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dposvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dposvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	*(unsigned char *)eq = '/';
	dposvx_("F", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = 'Y';
	dposvx_("F", "U", &c__1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	dposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        DPPSV */

	s_copy(srnamc_1.srnamt, "DPPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dppsv_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("DPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dppsv_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("DPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dppsv_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("DPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dppsv_("U", &c__2, &c__0, a, b, &c__1, &info);
	chkxer_("DPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPPSVX */

	s_copy(srnamc_1.srnamt, "DPPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dppsvx_("/", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dppsvx_("N", "/", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dppsvx_("N", "U", &c_n1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dppsvx_("N", "U", &c__0, &c_n1, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	*(unsigned char *)eq = '/';
	dppsvx_("F", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	*(unsigned char *)eq = 'Y';
	dppsvx_("F", "U", &c__1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__1, x, &c__2, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__2, x, &c__1, &
		rcond, r1, r2, w, iw, &info);
	chkxer_("DPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        DPBSV */

	s_copy(srnamc_1.srnamt, "DPBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbsv_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("DPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbsv_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("DPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbsv_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("DPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpbsv_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info)
		;
	chkxer_("DPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dpbsv_("U", &c__1, &c__1, &c__0, a, &c__1, b, &c__2, &info)
		;
	chkxer_("DPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dpbsv_("U", &c__2, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("DPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPBSVX */

	s_copy(srnamc_1.srnamt, "DPBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbsvx_("/", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbsvx_("N", "/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbsvx_("N", "U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpbsvx_("N", "U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dpbsvx_("N", "U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dpbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__1, af, &c__2, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dpbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__2, af, &c__1, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	dpbsvx_("F", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'Y';
	dpbsvx_("F", "U", &c__1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dpbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__2, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dpbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__2, x, &c__1, &rcond, r1, r2, w, iw, &info);
	chkxer_("DPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        DPTSV */

	s_copy(srnamc_1.srnamt, "DPTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dptsv_(&c_n1, &c__0, a, &a[4], b, &c__1, &info);
	chkxer_("DPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dptsv_(&c__0, &c_n1, a, &a[4], b, &c__1, &info);
	chkxer_("DPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dptsv_(&c__2, &c__0, a, &a[4], b, &c__1, &info);
	chkxer_("DPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPTSVX */

	s_copy(srnamc_1.srnamt, "DPTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dptsvx_("/", &c__0, &c__0, a, &a[4], af, &af[4], b, &c__1, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("DPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dptsvx_("N", &c_n1, &c__0, a, &a[4], af, &af[4], b, &c__1, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("DPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dptsvx_("N", &c__0, &c_n1, a, &a[4], af, &af[4], b, &c__1, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("DPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dptsvx_("N", &c__2, &c__0, a, &a[4], af, &af[4], b, &c__1, x, &c__2, &
		rcond, r1, r2, w, &info);
	chkxer_("DPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dptsvx_("N", &c__2, &c__0, a, &a[4], af, &af[4], b, &c__2, x, &c__1, &
		rcond, r1, r2, w, &info);
	chkxer_("DPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SY")) {

/*        DSYSV */

	s_copy(srnamc_1.srnamt, "DSYSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsysv_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("DSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsysv_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("DSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsysv_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("DSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsysv_("U", &c__2, &c__0, a, &c__2, ip, b, &c__1, w, &c__1, &info);
	chkxer_("DSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSYSVX */

	s_copy(srnamc_1.srnamt, "DSYSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsysvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsysvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsysvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsysvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dsysvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__1, x, 
		&c__2, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__1, &rcond, r1, r2, w, &c__4, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	dsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__3, iw, &info);
	chkxer_("DSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        DSPSV */

	s_copy(srnamc_1.srnamt, "DSPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dspsv_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("DSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dspsv_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("DSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dspsv_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("DSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dspsv_("U", &c__2, &c__0, a, ip, b, &c__1, &info);
	chkxer_("DSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSPSVX */

	s_copy(srnamc_1.srnamt, "DSPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dspsvx_("/", "U", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("DSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dspsvx_("N", "/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("DSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dspsvx_("N", "U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("DSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dspsvx_("N", "U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("DSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("DSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, iw, &info);
	chkxer_("DSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of DERRVX */

} /* derrvx_ */
