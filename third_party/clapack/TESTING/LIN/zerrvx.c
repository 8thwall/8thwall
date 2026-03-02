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

/* Subroutine */ int zerrvx_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 drivers passed the tests of the er"
	    "ror exits\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 drivers failed the test"
	    "s of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;
    doublecomplex z__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublecomplex a[16]	/* was [4][4] */, b[4];
    doublereal c__[4];
    integer i__, j;
    doublereal r__[4];
    doublecomplex w[8], x[4];
    char c2[2];
    doublereal r1[4], r2[4];
    doublecomplex af[16]	/* was [4][4] */;
    char eq[1];
    doublereal rf[4];
    integer ip[4];
    doublereal rw[4];
    integer info;
    doublereal rcond;
    extern /* Subroutine */ int zgbsv_(integer *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, integer *, doublecomplex *, 
	     integer *, integer *), zgesv_(integer *, integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, integer *, 
	     integer *), zhesv_(char *, integer *, integer *, doublecomplex *, 
	     integer *, integer *, doublecomplex *, integer *, doublecomplex *
, integer *, integer *), zpbsv_(char *, integer *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, integer *), zhpsv_(char *, integer *, integer 
	    *, doublecomplex *, integer *, doublecomplex *, integer *, 
	    integer *), zgtsv_(integer *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    integer *), zposv_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, integer *), zppsv_(
	    char *, integer *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, integer *), zspsv_(char *, integer *, integer *
, doublecomplex *, integer *, doublecomplex *, integer *, integer 
	    *), zptsv_(integer *, integer *, doublereal *, 
	    doublecomplex *, doublecomplex *, integer *, integer *), zsysv_(
	    char *, integer *, integer *, doublecomplex *, integer *, integer 
	    *, doublecomplex *, integer *, doublecomplex *, integer *, 
	    integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zgbsvx_(char *, char *, integer *, integer 
	    *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *, char *, doublereal *, 
	    doublereal *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublereal *, doublereal *, doublereal *, 
	    doublecomplex *, doublereal *, integer *),
	     zgesvx_(char *, char *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, integer *, char *, 
	    doublereal *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublecomplex *, doublereal *, integer *), zhesvx_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublecomplex *, 
	    integer *, doublereal *, integer *), zpbsvx_(char 
	    *, char *, integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, char *, doublereal *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublecomplex *, 
	    doublereal *, integer *), zhpsvx_(char *, 
	    char *, integer *, integer *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublereal *, doublereal *, doublereal *, doublecomplex *, 
	    doublereal *, integer *), zgtsvx_(char *, char *, 
	    integer *, integer *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex *
, doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublecomplex *, doublereal *, integer *), zposvx_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, char *, 
	    doublereal *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublereal *, doublereal *, doublereal *, 
	    doublecomplex *, doublereal *, integer *),
	     zppsvx_(char *, char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, char *, doublereal *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublecomplex *, doublereal *, integer *), zspsvx_(char *, char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublereal *, doublecomplex *, doublereal *, integer *), zptsvx_(char *, integer *, integer *, doublereal *, 
	    doublecomplex *, doublereal *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublereal *, doublecomplex *, doublereal *, integer *), 
	    zsysvx_(char *, char *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublereal *, doublereal *
, doublereal *, doublecomplex *, integer *, doublereal *, integer 
	    *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___21 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRVX tests the error exits for the COMPLEX*16 driver routines */
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
	    i__1 = i__ + (j << 2) - 5;
	    d__1 = 1. / (doublereal) (i__ + j);
	    d__2 = -1. / (doublereal) (i__ + j);
	    z__1.r = d__1, z__1.i = d__2;
	    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
	    i__1 = i__ + (j << 2) - 5;
	    d__1 = 1. / (doublereal) (i__ + j);
	    d__2 = -1. / (doublereal) (i__ + j);
	    z__1.r = d__1, z__1.i = d__2;
	    af[i__1].r = z__1.r, af[i__1].i = z__1.i;
/* L10: */
	}
	i__1 = j - 1;
	b[i__1].r = 0., b[i__1].i = 0.;
	r1[j - 1] = 0.;
	r2[j - 1] = 0.;
	i__1 = j - 1;
	w[i__1].r = 0., w[i__1].i = 0.;
	i__1 = j - 1;
	x[i__1].r = 0., x[i__1].i = 0.;
	c__[j - 1] = 0.;
	r__[j - 1] = 0.;
	ip[j - 1] = j;
/* L20: */
    }
    *(unsigned char *)eq = ' ';
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GE")) {

/*        ZGESV */

	s_copy(srnamc_1.srnamt, "ZGESV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgesv_(&c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgesv_(&c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgesv_(&c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("ZGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgesv_(&c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("ZGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGESVX */

	s_copy(srnamc_1.srnamt, "ZGESVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgesvx_("/", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgesvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgesvx_("N", "N", &c_n1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgesvx_("N", "N", &c__0, &c_n1, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgesvx_("N", "N", &c__2, &c__1, a, &c__1, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__1, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	zgesvx_("F", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'R';
	zgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = 'C';
	zgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        ZGBSV */

	s_copy(srnamc_1.srnamt, "ZGBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbsv_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbsv_(&c__1, &c_n1, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbsv_(&c__1, &c__0, &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbsv_(&c__0, &c__0, &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgbsv_(&c__1, &c__1, &c__1, &c__0, a, &c__3, ip, b, &c__1, &info);
	chkxer_("ZGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zgbsv_(&c__2, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGBSVX */

	s_copy(srnamc_1.srnamt, "ZGBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbsvx_("/", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbsvx_("N", "/", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbsvx_("N", "N", &c_n1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbsvx_("N", "N", &c__1, &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgbsvx_("N", "N", &c__1, &c__0, &c_n1, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgbsvx_("N", "N", &c__0, &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__2, af, &c__4, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__3, af, &c__3, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = '/';
	zgbsvx_("F", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	*(unsigned char *)eq = 'R';
	zgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	*(unsigned char *)eq = 'C';
	zgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	zgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("ZGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GT")) {

/*        ZGTSV */

	s_copy(srnamc_1.srnamt, "ZGTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgtsv_(&c_n1, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("ZGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgtsv_(&c__0, &c_n1, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("ZGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgtsv_(&c__2, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("ZGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGTSVX */

	s_copy(srnamc_1.srnamt, "ZGTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgtsvx_("/", "N", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgtsvx_("N", "/", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgtsvx_("N", "N", &c_n1, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgtsvx_("N", "N", &c__0, &c_n1, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	zgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PO")) {

/*        ZPOSV */

	s_copy(srnamc_1.srnamt, "ZPOSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zposv_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("ZPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zposv_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("ZPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zposv_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("ZPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zposv_("U", &c__2, &c__0, a, &c__1, b, &c__2, &info);
	chkxer_("ZPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zposv_("U", &c__2, &c__0, a, &c__2, b, &c__1, &info);
	chkxer_("ZPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPOSVX */

	s_copy(srnamc_1.srnamt, "ZPOSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zposvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zposvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zposvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zposvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zposvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	*(unsigned char *)eq = '/';
	zposvx_("F", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = 'Y';
	zposvx_("F", "U", &c__1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        ZPPSV */

	s_copy(srnamc_1.srnamt, "ZPPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zppsv_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("ZPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zppsv_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("ZPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zppsv_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("ZPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zppsv_("U", &c__2, &c__0, a, b, &c__1, &info);
	chkxer_("ZPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPPSVX */

	s_copy(srnamc_1.srnamt, "ZPPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zppsvx_("/", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zppsvx_("N", "/", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zppsvx_("N", "U", &c_n1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zppsvx_("N", "U", &c__0, &c_n1, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	*(unsigned char *)eq = '/';
	zppsvx_("F", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	*(unsigned char *)eq = 'Y';
	zppsvx_("F", "U", &c__1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__1, x, &c__2, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__2, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("ZPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        ZPBSV */

	s_copy(srnamc_1.srnamt, "ZPBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbsv_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("ZPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbsv_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("ZPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbsv_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("ZPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpbsv_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info)
		;
	chkxer_("ZPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zpbsv_("U", &c__1, &c__1, &c__0, a, &c__1, b, &c__2, &info)
		;
	chkxer_("ZPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zpbsv_("U", &c__2, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("ZPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPBSVX */

	s_copy(srnamc_1.srnamt, "ZPBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbsvx_("/", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbsvx_("N", "/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbsvx_("N", "U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpbsvx_("N", "U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zpbsvx_("N", "U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zpbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__1, af, &c__2, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zpbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__2, af, &c__1, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	zpbsvx_("F", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'Y';
	zpbsvx_("F", "U", &c__1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zpbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zpbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("ZPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        ZPTSV */

	s_copy(srnamc_1.srnamt, "ZPTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zptsv_(&c_n1, &c__0, r__, a, b, &c__1, &info);
	chkxer_("ZPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zptsv_(&c__0, &c_n1, r__, a, b, &c__1, &info);
	chkxer_("ZPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zptsv_(&c__2, &c__0, r__, a, b, &c__1, &info);
	chkxer_("ZPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPTSVX */

	s_copy(srnamc_1.srnamt, "ZPTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zptsvx_("/", &c__0, &c__0, r__, a, rf, af, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zptsvx_("N", &c_n1, &c__0, r__, a, rf, af, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zptsvx_("N", &c__0, &c_n1, r__, a, rf, af, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zptsvx_("N", &c__2, &c__0, r__, a, rf, af, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zptsvx_("N", &c__2, &c__0, r__, a, rf, af, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "HE")) {

/*        ZHESV */

	s_copy(srnamc_1.srnamt, "ZHESV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhesv_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhesv_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhesv_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zhesv_("U", &c__2, &c__0, a, &c__1, ip, b, &c__2, w, &c__1, &info);
	chkxer_("ZHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zhesv_("U", &c__2, &c__0, a, &c__2, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHESVX */

	s_copy(srnamc_1.srnamt, "ZHESVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhesvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhesvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhesvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhesvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zhesvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zhesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__1, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zhesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__1, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	zhesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__3, rw, &info);
	chkxer_("ZHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "HP")) {

/*        ZHPSV */

	s_copy(srnamc_1.srnamt, "ZHPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhpsv_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhpsv_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhpsv_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("ZHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhpsv_("U", &c__2, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHPSVX */

	s_copy(srnamc_1.srnamt, "ZHPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhpsvx_("/", "U", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhpsvx_("N", "/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhpsvx_("N", "U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhpsvx_("N", "U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zhpsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zhpsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SY")) {

/*        ZSYSV */

	s_copy(srnamc_1.srnamt, "ZSYSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsysv_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsysv_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zsysv_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zsysv_("U", &c__2, &c__0, a, &c__2, ip, b, &c__1, w, &c__1, &info);
	chkxer_("ZSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSYSVX */

	s_copy(srnamc_1.srnamt, "ZSYSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsysvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsysvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zsysvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zsysvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zsysvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__1, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__1, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	zsysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__3, rw, &info);
	chkxer_("ZSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        ZSPSV */

	s_copy(srnamc_1.srnamt, "ZSPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zspsv_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zspsv_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zspsv_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("ZSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zspsv_("U", &c__2, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSPSVX */

	s_copy(srnamc_1.srnamt, "ZSPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zspsvx_("/", "U", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zspsvx_("N", "/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zspsvx_("N", "U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zspsvx_("N", "U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("ZSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___20.ciunit = infoc_1.nout;
	s_wsfe(&io___20);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    } else {
	io___21.ciunit = infoc_1.nout;
	s_wsfe(&io___21);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of ZERRVX */

} /* zerrvx_ */
