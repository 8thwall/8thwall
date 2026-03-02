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

/* Subroutine */ int cerrvx_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 drivers passed the tests of the er"
	    "ror exits\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 drivers failed the test"
	    "s of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    complex q__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    complex a[16]	/* was [4][4] */, b[4];
    real c__[4];
    integer i__, j;
    real r__[4];
    complex w[8], x[4];
    char c2[2];
    real r1[4], r2[4];
    complex af[16]	/* was [4][4] */;
    char eq[1];
    real rf[4];
    integer ip[4];
    real rw[4];
    integer info;
    extern /* Subroutine */ int cgbsv_(integer *, integer *, integer *, 
	    integer *, complex *, integer *, integer *, complex *, integer *, 
	    integer *);
    real rcond;
    extern /* Subroutine */ int cgesv_(integer *, integer *, complex *, 
	    integer *, integer *, complex *, integer *, integer *), chesv_(
	    char *, integer *, integer *, complex *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, integer *), 
	    cpbsv_(char *, integer *, integer *, integer *, complex *, 
	    integer *, complex *, integer *, integer *), chpsv_(char *
, integer *, integer *, complex *, integer *, complex *, integer *
, integer *), cgtsv_(integer *, integer *, complex *, 
	    complex *, complex *, complex *, integer *, integer *), cposv_(
	    char *, integer *, integer *, complex *, integer *, complex *, 
	    integer *, integer *), cppsv_(char *, integer *, integer *
, complex *, complex *, integer *, integer *), cspsv_(
	    char *, integer *, integer *, complex *, integer *, complex *, 
	    integer *, integer *), cptsv_(integer *, integer *, real *
, complex *, complex *, integer *, integer *), csysv_(char *, 
	    integer *, integer *, complex *, integer *, integer *, complex *, 
	    integer *, complex *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), cgbsvx_(char *, char *, integer *, integer 
	    *, integer *, integer *, complex *, integer *, complex *, integer 
	    *, integer *, char *, real *, real *, complex *, integer *, 
	    complex *, integer *, real *, real *, real *, complex *, real *, 
	    integer *), cgesvx_(char *, char *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    integer *, char *, real *, real *, complex *, integer *, complex *
, integer *, real *, real *, real *, complex *, real *, integer *), chesvx_(char *, char *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, real *
, complex *, integer *, real *, integer *), 
	    cpbsvx_(char *, char *, integer *, integer *, integer *, complex *
, integer *, complex *, integer *, char *, real *, complex *, 
	    integer *, complex *, integer *, real *, real *, real *, complex *
, real *, integer *), chpsvx_(char *, 
	    char *, integer *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, real *
, complex *, real *, integer *), cgtsvx_(char *, 
	    char *, integer *, integer *, complex *, complex *, complex *, 
	    complex *, complex *, complex *, complex *, integer *, complex *, 
	    integer *, complex *, integer *, real *, real *, real *, complex *
, real *, integer *), cposvx_(char *, char *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    char *, real *, complex *, integer *, complex *, integer *, real *
, real *, real *, complex *, real *, integer *), cppsvx_(char *, char *, integer *, integer *, complex *, 
	    complex *, char *, real *, complex *, integer *, complex *, 
	    integer *, real *, real *, real *, complex *, real *, integer *), cspsvx_(char *, char *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *, real *, complex *, real *, 
	    integer *), cptsvx_(char *, integer *, integer *, 
	    real *, complex *, real *, complex *, complex *, integer *, 
	    complex *, integer *, real *, real *, real *, complex *, real *, 
	    integer *), csysvx_(char *, char *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, integer *, complex *, 
	    integer *, complex *, integer *, real *, real *, real *, complex *
, integer *, real *, integer *);

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

/*  CERRVX tests the error exits for the COMPLEX driver routines */
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
	    r__1 = 1.f / (real) (i__ + j);
	    r__2 = -1.f / (real) (i__ + j);
	    q__1.r = r__1, q__1.i = r__2;
	    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	    i__1 = i__ + (j << 2) - 5;
	    r__1 = 1.f / (real) (i__ + j);
	    r__2 = -1.f / (real) (i__ + j);
	    q__1.r = r__1, q__1.i = r__2;
	    af[i__1].r = q__1.r, af[i__1].i = q__1.i;
/* L10: */
	}
	i__1 = j - 1;
	b[i__1].r = 0.f, b[i__1].i = 0.f;
	r1[j - 1] = 0.f;
	r2[j - 1] = 0.f;
	i__1 = j - 1;
	w[i__1].r = 0.f, w[i__1].i = 0.f;
	i__1 = j - 1;
	x[i__1].r = 0.f, x[i__1].i = 0.f;
	c__[j - 1] = 0.f;
	r__[j - 1] = 0.f;
	ip[j - 1] = j;
/* L20: */
    }
    *(unsigned char *)eq = ' ';
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GE")) {

/*        CGESV */

	s_copy(srnamc_1.srnamt, "CGESV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgesv_(&c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgesv_(&c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgesv_(&c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("CGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgesv_(&c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("CGESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGESVX */

	s_copy(srnamc_1.srnamt, "CGESVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgesvx_("/", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgesvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgesvx_("N", "N", &c_n1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgesvx_("N", "N", &c__0, &c_n1, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cgesvx_("N", "N", &c__2, &c__1, a, &c__1, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__1, ip, eq, r__, c__, 
		 b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	cgesvx_("F", "N", &c__0, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'R';
	cgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = 'C';
	cgesvx_("F", "N", &c__1, &c__0, a, &c__1, af, &c__1, ip, eq, r__, c__, 
		 b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	cgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	cgesvx_("N", "N", &c__2, &c__1, a, &c__2, af, &c__2, ip, eq, r__, c__, 
		 b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        CGBSV */

	s_copy(srnamc_1.srnamt, "CGBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgbsv_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgbsv_(&c__1, &c_n1, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgbsv_(&c__1, &c__0, &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgbsv_(&c__0, &c__0, &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cgbsv_(&c__1, &c__1, &c__1, &c__0, a, &c__3, ip, b, &c__1, &info);
	chkxer_("CGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cgbsv_(&c__2, &c__0, &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CGBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGBSVX */

	s_copy(srnamc_1.srnamt, "CGBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgbsvx_("/", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgbsvx_("N", "/", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgbsvx_("N", "N", &c_n1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgbsvx_("N", "N", &c__1, &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgbsvx_("N", "N", &c__1, &c__0, &c_n1, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cgbsvx_("N", "N", &c__0, &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__2, af, &c__4, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgbsvx_("N", "N", &c__1, &c__1, &c__1, &c__0, a, &c__3, af, &c__3, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	*(unsigned char *)eq = '/';
	cgbsvx_("F", "N", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	*(unsigned char *)eq = 'R';
	cgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	*(unsigned char *)eq = 'C';
	cgbsvx_("F", "N", &c__1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	cgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	cgbsvx_("N", "N", &c__2, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, 
		 eq, r__, c__, b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &
		info);
	chkxer_("CGBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GT")) {

/*        CGTSV */

	s_copy(srnamc_1.srnamt, "CGTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgtsv_(&c_n1, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("CGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgtsv_(&c__0, &c_n1, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("CGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgtsv_(&c__2, &c__0, a, &a[4], &a[8], b, &c__1, &info);
	chkxer_("CGTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGTSVX */

	s_copy(srnamc_1.srnamt, "CGTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgtsvx_("/", "N", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgtsvx_("N", "/", &c__0, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgtsvx_("N", "N", &c_n1, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgtsvx_("N", "N", &c__0, &c_n1, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	cgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	cgtsvx_("N", "N", &c__2, &c__0, a, &a[4], &a[8], af, &af[4], &af[8], &
		af[12], ip, b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CGTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PO")) {

/*        CPOSV */

	s_copy(srnamc_1.srnamt, "CPOSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cposv_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("CPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cposv_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("CPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cposv_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("CPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cposv_("U", &c__2, &c__0, a, &c__1, b, &c__2, &info);
	chkxer_("CPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cposv_("U", &c__2, &c__0, a, &c__2, b, &c__1, &info);
	chkxer_("CPOSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPOSVX */

	s_copy(srnamc_1.srnamt, "CPOSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cposvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cposvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cposvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cposvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cposvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, eq, c__, b, &
		c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	*(unsigned char *)eq = '/';
	cposvx_("F", "U", &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = 'Y';
	cposvx_("F", "U", &c__1, &c__0, a, &c__1, af, &c__1, eq, c__, b, &
		c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	cposvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, eq, c__, b, &
		c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPOSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        CPPSV */

	s_copy(srnamc_1.srnamt, "CPPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cppsv_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("CPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cppsv_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("CPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cppsv_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("CPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cppsv_("U", &c__2, &c__0, a, b, &c__1, &info);
	chkxer_("CPPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPPSVX */

	s_copy(srnamc_1.srnamt, "CPPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cppsvx_("/", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cppsvx_("N", "/", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cppsvx_("N", "U", &c_n1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cppsvx_("N", "U", &c__0, &c_n1, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	*(unsigned char *)eq = '/';
	cppsvx_("F", "U", &c__0, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	*(unsigned char *)eq = 'Y';
	cppsvx_("F", "U", &c__1, &c__0, a, af, eq, c__, b, &c__1, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__1, x, &c__2, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cppsvx_("N", "U", &c__2, &c__0, a, af, eq, c__, b, &c__2, x, &c__1, &
		rcond, r1, r2, w, rw, &info);
	chkxer_("CPPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        CPBSV */

	s_copy(srnamc_1.srnamt, "CPBSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbsv_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("CPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbsv_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("CPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbsv_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("CPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpbsv_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info)
		;
	chkxer_("CPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cpbsv_("U", &c__1, &c__1, &c__0, a, &c__1, b, &c__2, &info)
		;
	chkxer_("CPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cpbsv_("U", &c__2, &c__0, &c__0, a, &c__1, b, &c__1, &info)
		;
	chkxer_("CPBSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPBSVX */

	s_copy(srnamc_1.srnamt, "CPBSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbsvx_("/", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbsvx_("N", "/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbsvx_("N", "U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpbsvx_("N", "U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cpbsvx_("N", "U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cpbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__1, af, &c__2, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cpbsvx_("N", "U", &c__1, &c__1, &c__0, a, &c__2, af, &c__1, eq, c__, 
		b, &c__2, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	*(unsigned char *)eq = '/';
	cpbsvx_("F", "U", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	*(unsigned char *)eq = 'Y';
	cpbsvx_("F", "U", &c__1, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cpbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__1, x, &c__2, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cpbsvx_("N", "U", &c__2, &c__0, &c__0, a, &c__1, af, &c__1, eq, c__, 
		b, &c__2, x, &c__1, &rcond, r1, r2, w, rw, &info);
	chkxer_("CPBSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        CPTSV */

	s_copy(srnamc_1.srnamt, "CPTSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cptsv_(&c_n1, &c__0, r__, a, b, &c__1, &info);
	chkxer_("CPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cptsv_(&c__0, &c_n1, r__, a, b, &c__1, &info);
	chkxer_("CPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cptsv_(&c__2, &c__0, r__, a, b, &c__1, &info);
	chkxer_("CPTSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPTSVX */

	s_copy(srnamc_1.srnamt, "CPTSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cptsvx_("/", &c__0, &c__0, r__, a, rf, af, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cptsvx_("N", &c_n1, &c__0, r__, a, rf, af, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cptsvx_("N", &c__0, &c_n1, r__, a, rf, af, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cptsvx_("N", &c__2, &c__0, r__, a, rf, af, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cptsvx_("N", &c__2, &c__0, r__, a, rf, af, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CPTSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "HE")) {

/*        CHESV */

	s_copy(srnamc_1.srnamt, "CHESV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chesv_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chesv_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chesv_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	chesv_("U", &c__2, &c__0, a, &c__1, ip, b, &c__2, w, &c__1, &info);
	chkxer_("CHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	chesv_("U", &c__2, &c__0, a, &c__2, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CHESV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHESVX */

	s_copy(srnamc_1.srnamt, "CHESVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chesvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chesvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chesvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chesvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	chesvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	chesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__1, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	chesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__1, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	chesvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__3, rw, &info);
	chkxer_("CHESVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "HP")) {

/*        CHPSV */

	s_copy(srnamc_1.srnamt, "CHPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chpsv_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chpsv_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chpsv_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("CHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chpsv_("U", &c__2, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CHPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHPSVX */

	s_copy(srnamc_1.srnamt, "CHPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chpsvx_("/", "U", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chpsvx_("N", "/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chpsvx_("N", "U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chpsvx_("N", "U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	chpsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	chpsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CHPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SY")) {

/*        CSYSV */

	s_copy(srnamc_1.srnamt, "CSYSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csysv_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csysv_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	csysv_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	csysv_("U", &c__2, &c__0, a, &c__2, ip, b, &c__1, w, &c__1, &info);
	chkxer_("CSYSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSYSVX */

	s_copy(srnamc_1.srnamt, "CSYSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csysvx_("/", "U", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csysvx_("N", "/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	csysvx_("N", "U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	csysvx_("N", "U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, 
		&c__1, &rcond, r1, r2, w, &c__1, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	csysvx_("N", "U", &c__2, &c__0, a, &c__1, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	csysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__1, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	csysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__1, x, 
		&c__2, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	csysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__1, &rcond, r1, r2, w, &c__4, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 18;
	csysvx_("N", "U", &c__2, &c__0, a, &c__2, af, &c__2, ip, b, &c__2, x, 
		&c__2, &rcond, r1, r2, w, &c__3, rw, &info);
	chkxer_("CSYSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        CSPSV */

	s_copy(srnamc_1.srnamt, "CSPSV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cspsv_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cspsv_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cspsv_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("CSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cspsv_("U", &c__2, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CSPSV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSPSVX */

	s_copy(srnamc_1.srnamt, "CSPSVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cspsvx_("/", "U", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cspsvx_("N", "/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cspsvx_("N", "U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cspsvx_("N", "U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__1, x, &c__2, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cspsvx_("N", "U", &c__2, &c__0, a, af, ip, b, &c__2, x, &c__1, &rcond, 
		 r1, r2, w, rw, &info);
	chkxer_("CSPSVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of CERRVX */

} /* cerrvx_ */
