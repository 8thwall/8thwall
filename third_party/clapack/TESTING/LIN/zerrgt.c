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

/* Subroutine */ int zerrgt_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublecomplex b[2];
    doublereal d__[2];
    doublecomplex e[2];
    integer i__;
    doublecomplex w[2], x[2];
    char c2[2];
    doublereal r1[2], r2[2], df[2];
    doublecomplex ef[2], dl[2];
    integer ip[2];
    doublecomplex du[2];
    doublereal rw[2];
    doublecomplex du2[2], dlf[2], duf[2];
    integer info;
    doublereal rcond, anorm;
    extern /* Subroutine */ int alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zgtcon_(char *, integer *, doublecomplex *, 
	     doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, integer *), 
	    zptcon_(integer *, doublereal *, doublecomplex *, doublereal *, 
	    doublereal *, doublereal *, integer *), zgtrfs_(char *, integer *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, doublecomplex *
, integer *, doublecomplex *, integer *, doublecomplex *, integer 
	    *, doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zgttrf_(integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    integer *), zptrfs_(char *, integer *, integer *, doublereal *, 
	    doublecomplex *, doublereal *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublecomplex *, doublereal *, integer *), zpttrf_(
	    integer *, doublereal *, doublecomplex *, integer *), zgttrs_(
	    char *, integer *, integer *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, integer *), zpttrs_(char *, integer *, integer 
	    *, doublereal *, doublecomplex *, doublecomplex *, integer *, 
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRGT tests the error exits for the COMPLEX*16 tridiagonal */
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
    for (i__ = 1; i__ <= 2; ++i__) {
	d__[i__ - 1] = 1.;
	i__1 = i__ - 1;
	e[i__1].r = 2., e[i__1].i = 0.;
	i__1 = i__ - 1;
	dl[i__1].r = 3., dl[i__1].i = 0.;
	i__1 = i__ - 1;
	du[i__1].r = 4., du[i__1].i = 0.;
/* L10: */
    }
    anorm = 1.;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GT")) {

/*        Test error exits for the general tridiagonal routines. */

/*        ZGTTRF */

	s_copy(srnamc_1.srnamt, "ZGTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgttrf_(&c_n1, dl, e, du, du2, ip, &info);
	chkxer_("ZGTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGTTRS */

	s_copy(srnamc_1.srnamt, "ZGTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgttrs_("/", &c__0, &c__0, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("ZGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgttrs_("N", &c_n1, &c__0, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("ZGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgttrs_("N", &c__0, &c_n1, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("ZGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgttrs_("N", &c__2, &c__1, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("ZGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGTRFS */

	s_copy(srnamc_1.srnamt, "ZGTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgtrfs_("/", &c__0, &c__0, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("ZGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgtrfs_("N", &c_n1, &c__0, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("ZGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgtrfs_("N", &c__0, &c_n1, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("ZGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	zgtrfs_("N", &c__2, &c__1, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__2, r1, r2, w, rw, &info);
	chkxer_("ZGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zgtrfs_("N", &c__2, &c__1, dl, e, du, dlf, ef, duf, du2, ip, b, &c__2, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("ZGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGTCON */

	s_copy(srnamc_1.srnamt, "ZGTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgtcon_("/", &c__0, dl, e, du, du2, ip, &anorm, &rcond, w, &info);
	chkxer_("ZGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgtcon_("I", &c_n1, dl, e, du, du2, ip, &anorm, &rcond, w, &info);
	chkxer_("ZGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	d__1 = -anorm;
	zgtcon_("I", &c__0, dl, e, du, du2, ip, &d__1, &rcond, w, &info);
	chkxer_("ZGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        Test error exits for the positive definite tridiagonal */
/*        routines. */

/*        ZPTTRF */

	s_copy(srnamc_1.srnamt, "ZPTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpttrf_(&c_n1, d__, e, &info);
	chkxer_("ZPTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPTTRS */

	s_copy(srnamc_1.srnamt, "ZPTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpttrs_("/", &c__1, &c__0, d__, e, x, &c__1, &info);
	chkxer_("ZPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpttrs_("U", &c_n1, &c__0, d__, e, x, &c__1, &info);
	chkxer_("ZPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpttrs_("U", &c__0, &c_n1, d__, e, x, &c__1, &info);
	chkxer_("ZPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zpttrs_("U", &c__2, &c__1, d__, e, x, &c__1, &info);
	chkxer_("ZPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPTRFS */

	s_copy(srnamc_1.srnamt, "ZPTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zptrfs_("/", &c__1, &c__0, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("ZPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zptrfs_("U", &c_n1, &c__0, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("ZPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zptrfs_("U", &c__0, &c_n1, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("ZPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zptrfs_("U", &c__2, &c__1, d__, e, df, ef, b, &c__1, x, &c__2, r1, r2, 
		 w, rw, &info);
	chkxer_("ZPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zptrfs_("U", &c__2, &c__1, d__, e, df, ef, b, &c__2, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("ZPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPTCON */

	s_copy(srnamc_1.srnamt, "ZPTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zptcon_(&c_n1, d__, e, &anorm, &rcond, rw, &info);
	chkxer_("ZPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	d__1 = -anorm;
	zptcon_(&c__0, d__, e, &d__1, &rcond, rw, &info);
	chkxer_("ZPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRGT */

} /* zerrgt_ */
