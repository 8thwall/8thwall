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

/* Subroutine */ int derrtr_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal a[4]	/* was [2][2] */, b[2], w[2], x[2];
    char c2[2];
    doublereal r1[2], r2[2];
    integer iw[2], info;
    doublereal scale, rcond;
    extern /* Subroutine */ int dtrti2_(char *, char *, integer *, doublereal 
	    *, integer *, integer *), alaesm_(char *, logical 
	    *, integer *), dlatbs_(char *, char *, char *, char *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *), dtbcon_(char *, char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), dtbrfs_(char *, char *, char *, integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), 
	    dlatps_(char *, char *, char *, char *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *), dtpcon_(char *, char *, char *, integer *
, doublereal *, doublereal *, doublereal *, integer *, integer *), dlatrs_(char *, char *, char *, char *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *), dtrcon_(
	    char *, char *, char *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *), dtbtrs_(char *, char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *), dtprfs_(char *, char *, char *
, integer *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *), dtrrfs_(char *, 
	    char *, char *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *), dtptri_(char *, char *, integer *, doublereal *, integer 
	    *), dtrtri_(char *, char *, integer *, doublereal 
	    *, integer *, integer *), dtptrs_(char *, char *, 
	    char *, integer *, integer *, doublereal *, doublereal *, integer 
	    *, integer *), dtrtrs_(char *, char *, 
	    char *, integer *, integer *, doublereal *, integer *, doublereal 
	    *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERRTR tests the error exits for the DOUBLE PRECISION triangular */
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
    a[0] = 1.;
    a[2] = 2.;
    a[3] = 3.;
    a[1] = 4.;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "TR")) {

/*        Test error exits for the general triangular routines. */

/*        DTRTRI */

	s_copy(srnamc_1.srnamt, "DTRTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtrtri_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("DTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtrtri_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("DTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtrtri_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("DTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtrtri_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("DTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTRTI2 */

	s_copy(srnamc_1.srnamt, "DTRTI2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtrti2_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("DTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtrti2_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("DTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtrti2_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("DTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtrti2_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("DTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTRTRS */

	s_copy(srnamc_1.srnamt, "DTRTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtrtrs_("/", "N", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtrtrs_("U", "/", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtrtrs_("U", "N", "/", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtrtrs_("U", "N", "N", &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtrtrs_("U", "N", "N", &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("DTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dtrtrs_("U", "N", "N", &c__2, &c__1, a, &c__1, x, &c__2, &info);
	chkxer_("DTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dtrtrs_("U", "N", "N", &c__2, &c__1, a, &c__2, x, &c__1, &info);
	chkxer_("DTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTRRFS */

	s_copy(srnamc_1.srnamt, "DTRRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtrrfs_("/", "N", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtrrfs_("U", "/", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtrrfs_("U", "N", "/", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtrrfs_("U", "N", "N", &c_n1, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtrrfs_("U", "N", "N", &c__0, &c_n1, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dtrrfs_("U", "N", "N", &c__2, &c__1, a, &c__1, b, &c__2, x, &c__2, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dtrrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__1, x, &c__2, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dtrrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__2, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("DTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTRCON */

	s_copy(srnamc_1.srnamt, "DTRCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtrcon_("/", "U", "N", &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtrcon_("1", "/", "N", &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtrcon_("1", "U", "/", &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtrcon_("1", "U", "N", &c_n1, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtrcon_("1", "U", "N", &c__2, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DLATRS */

	s_copy(srnamc_1.srnamt, "DLATRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dlatrs_("/", "N", "N", "N", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("DLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dlatrs_("U", "/", "N", "N", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("DLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dlatrs_("U", "N", "/", "N", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("DLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dlatrs_("U", "N", "N", "/", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("DLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dlatrs_("U", "N", "N", "N", &c_n1, a, &c__1, x, &scale, w, &info);
	chkxer_("DLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dlatrs_("U", "N", "N", "N", &c__2, a, &c__1, x, &scale, w, &info);
	chkxer_("DLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "TP")) {

/*        Test error exits for the packed triangular routines. */

/*        DTPTRI */

	s_copy(srnamc_1.srnamt, "DTPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtptri_("/", "N", &c__0, a, &info);
	chkxer_("DTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtptri_("U", "/", &c__0, a, &info);
	chkxer_("DTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtptri_("U", "N", &c_n1, a, &info);
	chkxer_("DTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTPTRS */

	s_copy(srnamc_1.srnamt, "DTPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtptrs_("/", "N", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("DTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtptrs_("U", "/", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("DTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtptrs_("U", "N", "/", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("DTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtptrs_("U", "N", "N", &c_n1, &c__0, a, x, &c__1, &info);
	chkxer_("DTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtptrs_("U", "N", "N", &c__0, &c_n1, a, x, &c__1, &info);
	chkxer_("DTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtptrs_("U", "N", "N", &c__2, &c__1, a, x, &c__1, &info);
	chkxer_("DTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTPRFS */

	s_copy(srnamc_1.srnamt, "DTPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtprfs_("/", "N", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("DTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtprfs_("U", "/", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("DTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtprfs_("U", "N", "/", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("DTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtprfs_("U", "N", "N", &c_n1, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("DTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtprfs_("U", "N", "N", &c__0, &c_n1, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("DTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__1, x, &c__2, r1, r2, w, 
		 iw, &info);
	chkxer_("DTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__2, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("DTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTPCON */

	s_copy(srnamc_1.srnamt, "DTPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtpcon_("/", "U", "N", &c__0, a, &rcond, w, iw, &info);
	chkxer_("DTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtpcon_("1", "/", "N", &c__0, a, &rcond, w, iw, &info);
	chkxer_("DTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtpcon_("1", "U", "/", &c__0, a, &rcond, w, iw, &info);
	chkxer_("DTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtpcon_("1", "U", "N", &c_n1, a, &rcond, w, iw, &info);
	chkxer_("DTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DLATPS */

	s_copy(srnamc_1.srnamt, "DLATPS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dlatps_("/", "N", "N", "N", &c__0, a, x, &scale, w, &info);
	chkxer_("DLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dlatps_("U", "/", "N", "N", &c__0, a, x, &scale, w, &info);
	chkxer_("DLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dlatps_("U", "N", "/", "N", &c__0, a, x, &scale, w, &info);
	chkxer_("DLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dlatps_("U", "N", "N", "/", &c__0, a, x, &scale, w, &info);
	chkxer_("DLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dlatps_("U", "N", "N", "N", &c_n1, a, x, &scale, w, &info);
	chkxer_("DLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "TB")) {

/*        Test error exits for the banded triangular routines. */

/*        DTBTRS */

	s_copy(srnamc_1.srnamt, "DTBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtbtrs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtbtrs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtbtrs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtbtrs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtbtrs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtbtrs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtbtrs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, x, &c__2, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtbtrs_("U", "N", "N", &c__2, &c__0, &c__1, a, &c__1, x, &c__1, &info);
	chkxer_("DTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTBRFS */

	s_copy(srnamc_1.srnamt, "DTBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtbrfs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtbrfs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtbrfs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtbrfs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtbrfs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dtbrfs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dtbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dtbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dtbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DTBCON */

	s_copy(srnamc_1.srnamt, "DTBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dtbcon_("/", "U", "N", &c__0, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dtbcon_("1", "/", "N", &c__0, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dtbcon_("1", "U", "/", &c__0, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dtbcon_("1", "U", "N", &c_n1, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dtbcon_("1", "U", "N", &c__0, &c_n1, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dtbcon_("1", "U", "N", &c__2, &c__1, a, &c__1, &rcond, w, iw, &info);
	chkxer_("DTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DLATBS */

	s_copy(srnamc_1.srnamt, "DLATBS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dlatbs_("/", "N", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("DLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dlatbs_("U", "/", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("DLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dlatbs_("U", "N", "/", "N", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("DLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dlatbs_("U", "N", "N", "/", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("DLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dlatbs_("U", "N", "N", "N", &c_n1, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("DLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dlatbs_("U", "N", "N", "N", &c__1, &c_n1, a, &c__1, x, &scale, w, &
		info);
	chkxer_("DLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dlatbs_("U", "N", "N", "N", &c__2, &c__1, a, &c__1, x, &scale, w, &
		info);
	chkxer_("DLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of DERRTR */

} /* derrtr_ */
