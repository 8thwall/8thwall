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

/* Subroutine */ int zerrtz_(char *path, integer *nunit)
{
    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), e_wsle(void);

    /* Local variables */
    doublecomplex a[4]	/* was [2][2] */, w[2];
    char c2[2];
    doublecomplex tau[2];
    integer info;
    extern /* Subroutine */ int alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), ztzrqf_(integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), ztzrzf_(
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRTZ tests the error exits for ZTZRQF and ZTZRZF. */

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
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);
    a[0].r = 1., a[0].i = -1.;
    a[2].r = 2., a[2].i = -2.;
    a[3].r = 3., a[3].i = -3.;
    a[1].r = 4., a[1].i = -4.;
    w[0].r = 0., w[0].i = 0.;
    w[1].r = 0., w[1].i = 0.;
    infoc_1.ok = TRUE_;

/*     Test error exits for the trapezoidal routines. */

    io___4.ciunit = infoc_1.nout;
    s_wsle(&io___4);
    e_wsle();
    if (lsamen_(&c__2, c2, "TZ")) {

/*        ZTZRQF */

	s_copy(srnamc_1.srnamt, "ZTZRQF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztzrqf_(&c_n1, &c__0, a, &c__1, tau, &info);
	chkxer_("ZTZRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztzrqf_(&c__1, &c__0, a, &c__1, tau, &info);
	chkxer_("ZTZRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztzrqf_(&c__2, &c__2, a, &c__1, tau, &info);
	chkxer_("ZTZRQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTZRZF */

	s_copy(srnamc_1.srnamt, "ZTZRZF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztzrzf_(&c_n1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZTZRZF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztzrzf_(&c__1, &c__0, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZTZRZF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztzrzf_(&c__2, &c__2, a, &c__1, tau, w, &c__1, &info);
	chkxer_("ZTZRZF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ztzrzf_(&c__2, &c__2, a, &c__2, tau, w, &c__1, &info);
	chkxer_("ZTZRZF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRTZ */

} /* zerrtz_ */
