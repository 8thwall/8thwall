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

/* Subroutine */ int zerrtr_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublecomplex a[4]	/* was [2][2] */, b[2], w[2], x[2];
    char c2[2];
    doublereal r1[2], r2[2], rw[2];
    integer info;
    doublereal scale, rcond;
    extern /* Subroutine */ int ztrti2_(char *, char *, integer *, 
	    doublecomplex *, integer *, integer *), alaesm_(
	    char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zlatbs_(char *, char *, char *, char *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     doublereal *, doublereal *, integer *), ztbcon_(char *, char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    doublereal *, integer *), ztbrfs_(char *, 
	    char *, char *, integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zlatps_(char *, char *, char *
, char *, integer *, doublecomplex *, doublecomplex *, doublereal 
	    *, doublereal *, integer *), 
	    ztpcon_(char *, char *, char *, integer *, doublecomplex *, 
	    doublereal *, doublecomplex *, doublereal *, integer *), zlatrs_(char *, char *, char *, char *, integer *
, doublecomplex *, integer *, doublecomplex *, doublereal *, 
	    doublereal *, integer *), ztrcon_(
	    char *, char *, char *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublecomplex *, doublereal *, integer *), ztbtrs_(char *, char *, char *, integer *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, integer *), ztprfs_(char *, 
	    char *, char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), ztrrfs_(char *, char *, char *
, integer *, integer *, doublecomplex *, integer *, doublecomplex 
	    *, integer *, doublecomplex *, integer *, doublereal *, 
	    doublereal *, doublecomplex *, doublereal *, integer *), ztptri_(char *, char *, integer *, doublecomplex 
	    *, integer *), ztrtri_(char *, char *, integer *, 
	    doublecomplex *, integer *, integer *), ztptrs_(
	    char *, char *, char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, integer *), 
	    ztrtrs_(char *, char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRTR tests the error exits for the COMPLEX*16 triangular routines. */

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
    a[0].r = 1., a[0].i = 0.;
    a[2].r = 2., a[2].i = 0.;
    a[3].r = 3., a[3].i = 0.;
    a[1].r = 4., a[1].i = 0.;
    infoc_1.ok = TRUE_;

/*     Test error exits for the general triangular routines. */

    if (lsamen_(&c__2, c2, "TR")) {

/*        ZTRTRI */

	s_copy(srnamc_1.srnamt, "ZTRTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztrtri_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("ZTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztrtri_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("ZTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztrtri_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("ZTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztrtri_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("ZTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTRTI2 */

	s_copy(srnamc_1.srnamt, "ZTRTI2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztrti2_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("ZTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztrti2_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("ZTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztrti2_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("ZTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztrti2_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("ZTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);


/*        ZTRTRS */

	s_copy(srnamc_1.srnamt, "ZTRTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztrtrs_("/", "N", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztrtrs_("U", "/", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztrtrs_("U", "N", "/", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztrtrs_("U", "N", "N", &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztrtrs_("U", "N", "N", &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("ZTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;

/*        ZTRRFS */

	s_copy(srnamc_1.srnamt, "ZTRRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztrrfs_("/", "N", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztrrfs_("U", "/", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztrrfs_("U", "N", "/", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztrrfs_("U", "N", "N", &c_n1, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztrrfs_("U", "N", "N", &c__0, &c_n1, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ztrrfs_("U", "N", "N", &c__2, &c__1, a, &c__1, b, &c__2, x, &c__2, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ztrrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__1, x, &c__2, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ztrrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__2, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("ZTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTRCON */

	s_copy(srnamc_1.srnamt, "ZTRCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztrcon_("/", "U", "N", &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztrcon_("1", "/", "N", &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztrcon_("1", "U", "/", &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztrcon_("1", "U", "N", &c_n1, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztrcon_("1", "U", "N", &c__2, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZLATRS */

	s_copy(srnamc_1.srnamt, "ZLATRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zlatrs_("/", "N", "N", "N", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("ZLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zlatrs_("U", "/", "N", "N", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("ZLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zlatrs_("U", "N", "/", "N", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("ZLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zlatrs_("U", "N", "N", "/", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("ZLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zlatrs_("U", "N", "N", "N", &c_n1, a, &c__1, x, &scale, rw, &info);
	chkxer_("ZLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zlatrs_("U", "N", "N", "N", &c__2, a, &c__1, x, &scale, rw, &info);
	chkxer_("ZLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits for the packed triangular routines. */

    } else if (lsamen_(&c__2, c2, "TP")) {

/*        ZTPTRI */

	s_copy(srnamc_1.srnamt, "ZTPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztptri_("/", "N", &c__0, a, &info);
	chkxer_("ZTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztptri_("U", "/", &c__0, a, &info);
	chkxer_("ZTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztptri_("U", "N", &c_n1, a, &info);
	chkxer_("ZTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTPTRS */

	s_copy(srnamc_1.srnamt, "ZTPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztptrs_("/", "N", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("ZTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztptrs_("U", "/", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("ZTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztptrs_("U", "N", "/", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("ZTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztptrs_("U", "N", "N", &c_n1, &c__0, a, x, &c__1, &info);
	chkxer_("ZTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztptrs_("U", "N", "N", &c__0, &c_n1, a, x, &c__1, &info);
	chkxer_("ZTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztptrs_("U", "N", "N", &c__2, &c__1, a, x, &c__1, &info);
	chkxer_("ZTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTPRFS */

	s_copy(srnamc_1.srnamt, "ZTPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztprfs_("/", "N", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("ZTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztprfs_("U", "/", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("ZTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztprfs_("U", "N", "/", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("ZTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztprfs_("U", "N", "N", &c_n1, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("ZTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztprfs_("U", "N", "N", &c__0, &c_n1, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("ZTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__1, x, &c__2, r1, r2, w, 
		 rw, &info);
	chkxer_("ZTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__2, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("ZTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTPCON */

	s_copy(srnamc_1.srnamt, "ZTPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztpcon_("/", "U", "N", &c__0, a, &rcond, w, rw, &info);
	chkxer_("ZTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztpcon_("1", "/", "N", &c__0, a, &rcond, w, rw, &info);
	chkxer_("ZTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztpcon_("1", "U", "/", &c__0, a, &rcond, w, rw, &info);
	chkxer_("ZTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztpcon_("1", "U", "N", &c_n1, a, &rcond, w, rw, &info);
	chkxer_("ZTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZLATPS */

	s_copy(srnamc_1.srnamt, "ZLATPS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zlatps_("/", "N", "N", "N", &c__0, a, x, &scale, rw, &info);
	chkxer_("ZLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zlatps_("U", "/", "N", "N", &c__0, a, x, &scale, rw, &info);
	chkxer_("ZLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zlatps_("U", "N", "/", "N", &c__0, a, x, &scale, rw, &info);
	chkxer_("ZLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zlatps_("U", "N", "N", "/", &c__0, a, x, &scale, rw, &info);
	chkxer_("ZLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zlatps_("U", "N", "N", "N", &c_n1, a, x, &scale, rw, &info);
	chkxer_("ZLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits for the banded triangular routines. */

    } else if (lsamen_(&c__2, c2, "TB")) {

/*        ZTBTRS */

	s_copy(srnamc_1.srnamt, "ZTBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztbtrs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztbtrs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztbtrs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztbtrs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztbtrs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztbtrs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztbtrs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, x, &c__2, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztbtrs_("U", "N", "N", &c__2, &c__0, &c__1, a, &c__1, x, &c__1, &info);
	chkxer_("ZTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTBRFS */

	s_copy(srnamc_1.srnamt, "ZTBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztbrfs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztbrfs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztbrfs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztbrfs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztbrfs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ztbrfs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ztbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ztbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, x, &
		c__2, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ztbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("ZTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZTBCON */

	s_copy(srnamc_1.srnamt, "ZTBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ztbcon_("/", "U", "N", &c__0, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ztbcon_("1", "/", "N", &c__0, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ztbcon_("1", "U", "/", &c__0, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ztbcon_("1", "U", "N", &c_n1, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ztbcon_("1", "U", "N", &c__0, &c_n1, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ztbcon_("1", "U", "N", &c__2, &c__1, a, &c__1, &rcond, w, rw, &info);
	chkxer_("ZTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZLATBS */

	s_copy(srnamc_1.srnamt, "ZLATBS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zlatbs_("/", "N", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("ZLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zlatbs_("U", "/", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("ZLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zlatbs_("U", "N", "/", "N", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("ZLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zlatbs_("U", "N", "N", "/", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("ZLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zlatbs_("U", "N", "N", "N", &c_n1, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("ZLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zlatbs_("U", "N", "N", "N", &c__1, &c_n1, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("ZLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zlatbs_("U", "N", "N", "N", &c__2, &c__1, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("ZLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRTR */

} /* zerrtr_ */
