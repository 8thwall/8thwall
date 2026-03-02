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

/* Subroutine */ int derrgt_(char *path, integer *nunit)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal b[2], c__[2], d__[2], e[2], f[2], w[2], x[2];
    char c2[2];
    doublereal r1[2], r2[2], cf[2], df[2], ef[2];
    integer ip[2], iw[2], info;
    doublereal rcond, anorm;
    extern /* Subroutine */ int alaesm_(char *, logical *, integer *),
	     dgtcon_(char *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, doublereal *, 
	     doublereal *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), dptcon_(integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *)
	    , dgtrfs_(char *, integer *, integer *, doublereal *, doublereal *
, doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    integer *), dgttrf_(integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, integer *, integer *), dptrfs_(
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), dpttrf_(
	    integer *, doublereal *, doublereal *, integer *), dgttrs_(char *, 
	     integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), dpttrs_(integer *, integer *, doublereal *, doublereal *, 
	     doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERRGT tests the error exits for the DOUBLE PRECISION tridiagonal */
/*  routines. */

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
    d__[0] = 1.;
    d__[1] = 2.;
    df[0] = 1.;
    df[1] = 2.;
    e[0] = 3.;
    e[1] = 4.;
    ef[0] = 3.;
    ef[1] = 4.;
    anorm = 1.;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GT")) {

/*        Test error exits for the general tridiagonal routines. */

/*        DGTTRF */

	s_copy(srnamc_1.srnamt, "DGTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgttrf_(&c_n1, c__, d__, e, f, ip, &info);
	chkxer_("DGTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGTTRS */

	s_copy(srnamc_1.srnamt, "DGTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgttrs_("/", &c__0, &c__0, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("DGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgttrs_("N", &c_n1, &c__0, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("DGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgttrs_("N", &c__0, &c_n1, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("DGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dgttrs_("N", &c__2, &c__1, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("DGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGTRFS */

	s_copy(srnamc_1.srnamt, "DGTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgtrfs_("/", &c__0, &c__0, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("DGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgtrfs_("N", &c_n1, &c__0, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("DGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgtrfs_("N", &c__0, &c_n1, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("DGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dgtrfs_("N", &c__2, &c__1, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__2, r1, r2, w, iw, &info);
	chkxer_("DGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	dgtrfs_("N", &c__2, &c__1, c__, d__, e, cf, df, ef, f, ip, b, &c__2, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("DGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGTCON */

	s_copy(srnamc_1.srnamt, "DGTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgtcon_("/", &c__0, c__, d__, e, f, ip, &anorm, &rcond, w, iw, &info);
	chkxer_("DGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgtcon_("I", &c_n1, c__, d__, e, f, ip, &anorm, &rcond, w, iw, &info);
	chkxer_("DGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	d__1 = -anorm;
	dgtcon_("I", &c__0, c__, d__, e, f, ip, &d__1, &rcond, w, iw, &info);
	chkxer_("DGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        Test error exits for the positive definite tridiagonal */
/*        routines. */

/*        DPTTRF */

	s_copy(srnamc_1.srnamt, "DPTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpttrf_(&c_n1, d__, e, &info);
	chkxer_("DPTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPTTRS */

	s_copy(srnamc_1.srnamt, "DPTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpttrs_(&c_n1, &c__0, d__, e, x, &c__1, &info);
	chkxer_("DPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpttrs_(&c__0, &c_n1, d__, e, x, &c__1, &info);
	chkxer_("DPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dpttrs_(&c__2, &c__1, d__, e, x, &c__1, &info);
	chkxer_("DPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPTRFS */

	s_copy(srnamc_1.srnamt, "DPTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dptrfs_(&c_n1, &c__0, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, w, &
		info);
	chkxer_("DPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dptrfs_(&c__0, &c_n1, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, w, &
		info);
	chkxer_("DPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dptrfs_(&c__2, &c__1, d__, e, df, ef, b, &c__1, x, &c__2, r1, r2, w, &
		info);
	chkxer_("DPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dptrfs_(&c__2, &c__1, d__, e, df, ef, b, &c__2, x, &c__1, r1, r2, w, &
		info);
	chkxer_("DPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPTCON */

	s_copy(srnamc_1.srnamt, "DPTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dptcon_(&c_n1, d__, e, &anorm, &rcond, w, &info);
	chkxer_("DPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	d__1 = -anorm;
	dptcon_(&c__0, d__, e, &d__1, &rcond, w, &info);
	chkxer_("DPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of DERRGT */

} /* derrgt_ */
