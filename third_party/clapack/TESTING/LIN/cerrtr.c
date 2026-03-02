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

/* Subroutine */ int cerrtr_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    complex a[4]	/* was [2][2] */, b[2], w[2], x[2];
    char c2[2];
    real r1[2], r2[2], rw[2];
    integer info;
    real scale, rcond;
    extern /* Subroutine */ int ctrti2_(char *, char *, integer *, complex *, 
	    integer *, integer *), alaesm_(char *, logical *, 
	    integer *), clatbs_(char *, char *, char *, char *, 
	    integer *, integer *, complex *, integer *, complex *, real *, 
	    real *, integer *), ctbcon_(char *
, char *, char *, integer *, integer *, complex *, integer *, 
	    real *, complex *, real *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int ctbrfs_(char *, char *, char *, integer *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *, complex *, real *, integer *
), chkxer_(char *, integer *, integer *, 
	    logical *, logical *), clatps_(char *, char *, char *, 
	    char *, integer *, complex *, complex *, real *, real *, integer *
), ctpcon_(char *, char *, char *, 
	     integer *, complex *, real *, complex *, real *, integer *), clatrs_(char *, char *, char *, char *, 
	    integer *, complex *, integer *, complex *, real *, real *, 
	    integer *), ctrcon_(char *, char *
, char *, integer *, complex *, integer *, real *, complex *, 
	    real *, integer *), ctbtrs_(char *, char *
, char *, integer *, integer *, integer *, complex *, integer *, 
	    complex *, integer *, integer *), ctprfs_(
	    char *, char *, char *, integer *, integer *, complex *, complex *
, integer *, complex *, integer *, real *, real *, complex *, 
	    real *, integer *), ctrrfs_(char *, char *
, char *, integer *, integer *, complex *, integer *, complex *, 
	    integer *, complex *, integer *, real *, real *, complex *, real *
, integer *), ctptri_(char *, char *, 
	    integer *, complex *, integer *), ctrtri_(char *, 
	    char *, integer *, complex *, integer *, integer *), ctptrs_(char *, char *, char *, integer *, integer *, 
	    complex *, complex *, integer *, integer *), ctrtrs_(char *, char *, char *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRTR tests the error exits for the COMPLEX triangular routines. */

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
    a[0].r = 1.f, a[0].i = 0.f;
    a[2].r = 2.f, a[2].i = 0.f;
    a[3].r = 3.f, a[3].i = 0.f;
    a[1].r = 4.f, a[1].i = 0.f;
    infoc_1.ok = TRUE_;

/*     Test error exits for the general triangular routines. */

    if (lsamen_(&c__2, c2, "TR")) {

/*        CTRTRI */

	s_copy(srnamc_1.srnamt, "CTRTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctrtri_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("CTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctrtri_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("CTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctrtri_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("CTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctrtri_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("CTRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CTRTI2 */

	s_copy(srnamc_1.srnamt, "CTRTI2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctrti2_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("CTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctrti2_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("CTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctrti2_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("CTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctrti2_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("CTRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);


/*        CTRTRS */

	s_copy(srnamc_1.srnamt, "CTRTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctrtrs_("/", "N", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctrtrs_("U", "/", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctrtrs_("U", "N", "/", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctrtrs_("U", "N", "N", &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctrtrs_("U", "N", "N", &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("CTRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;

/*        CTRRFS */

	s_copy(srnamc_1.srnamt, "CTRRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctrrfs_("/", "N", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctrrfs_("U", "/", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctrrfs_("U", "N", "/", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctrrfs_("U", "N", "N", &c_n1, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctrrfs_("U", "N", "N", &c__0, &c_n1, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ctrrfs_("U", "N", "N", &c__2, &c__1, a, &c__1, b, &c__2, x, &c__2, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	ctrrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__1, x, &c__2, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	ctrrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__2, x, &c__1, r1, 
		 r2, w, rw, &info);
	chkxer_("CTRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CTRCON */

	s_copy(srnamc_1.srnamt, "CTRCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctrcon_("/", "U", "N", &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctrcon_("1", "/", "N", &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctrcon_("1", "U", "/", &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctrcon_("1", "U", "N", &c_n1, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctrcon_("1", "U", "N", &c__2, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CLATRS */

	s_copy(srnamc_1.srnamt, "CLATRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	clatrs_("/", "N", "N", "N", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("CLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	clatrs_("U", "/", "N", "N", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("CLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	clatrs_("U", "N", "/", "N", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("CLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	clatrs_("U", "N", "N", "/", &c__0, a, &c__1, x, &scale, rw, &info);
	chkxer_("CLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	clatrs_("U", "N", "N", "N", &c_n1, a, &c__1, x, &scale, rw, &info);
	chkxer_("CLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	clatrs_("U", "N", "N", "N", &c__2, a, &c__1, x, &scale, rw, &info);
	chkxer_("CLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits for the packed triangular routines. */

    } else if (lsamen_(&c__2, c2, "TP")) {

/*        CTPTRI */

	s_copy(srnamc_1.srnamt, "CTPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctptri_("/", "N", &c__0, a, &info);
	chkxer_("CTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctptri_("U", "/", &c__0, a, &info);
	chkxer_("CTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctptri_("U", "N", &c_n1, a, &info);
	chkxer_("CTPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CTPTRS */

	s_copy(srnamc_1.srnamt, "CTPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctptrs_("/", "N", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("CTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctptrs_("U", "/", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("CTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctptrs_("U", "N", "/", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("CTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctptrs_("U", "N", "N", &c_n1, &c__0, a, x, &c__1, &info);
	chkxer_("CTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctptrs_("U", "N", "N", &c__0, &c_n1, a, x, &c__1, &info);
	chkxer_("CTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctptrs_("U", "N", "N", &c__2, &c__1, a, x, &c__1, &info);
	chkxer_("CTPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CTPRFS */

	s_copy(srnamc_1.srnamt, "CTPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctprfs_("/", "N", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("CTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctprfs_("U", "/", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("CTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctprfs_("U", "N", "/", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("CTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctprfs_("U", "N", "N", &c_n1, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("CTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctprfs_("U", "N", "N", &c__0, &c_n1, a, b, &c__1, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("CTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__1, x, &c__2, r1, r2, w, 
		 rw, &info);
	chkxer_("CTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__2, x, &c__1, r1, r2, w, 
		 rw, &info);
	chkxer_("CTPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CTPCON */

	s_copy(srnamc_1.srnamt, "CTPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctpcon_("/", "U", "N", &c__0, a, &rcond, w, rw, &info);
	chkxer_("CTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctpcon_("1", "/", "N", &c__0, a, &rcond, w, rw, &info);
	chkxer_("CTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctpcon_("1", "U", "/", &c__0, a, &rcond, w, rw, &info);
	chkxer_("CTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctpcon_("1", "U", "N", &c_n1, a, &rcond, w, rw, &info);
	chkxer_("CTPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CLATPS */

	s_copy(srnamc_1.srnamt, "CLATPS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	clatps_("/", "N", "N", "N", &c__0, a, x, &scale, rw, &info);
	chkxer_("CLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	clatps_("U", "/", "N", "N", &c__0, a, x, &scale, rw, &info);
	chkxer_("CLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	clatps_("U", "N", "/", "N", &c__0, a, x, &scale, rw, &info);
	chkxer_("CLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	clatps_("U", "N", "N", "/", &c__0, a, x, &scale, rw, &info);
	chkxer_("CLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	clatps_("U", "N", "N", "N", &c_n1, a, x, &scale, rw, &info);
	chkxer_("CLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits for the banded triangular routines. */

    } else if (lsamen_(&c__2, c2, "TB")) {

/*        CTBTRS */

	s_copy(srnamc_1.srnamt, "CTBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctbtrs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctbtrs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctbtrs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctbtrs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctbtrs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctbtrs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctbtrs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, x, &c__2, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctbtrs_("U", "N", "N", &c__2, &c__0, &c__1, a, &c__1, x, &c__1, &info);
	chkxer_("CTBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CTBRFS */

	s_copy(srnamc_1.srnamt, "CTBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctbrfs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctbrfs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctbrfs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctbrfs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctbrfs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ctbrfs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ctbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ctbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, x, &
		c__2, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ctbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, x, &
		c__1, r1, r2, w, rw, &info);
	chkxer_("CTBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CTBCON */

	s_copy(srnamc_1.srnamt, "CTBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ctbcon_("/", "U", "N", &c__0, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ctbcon_("1", "/", "N", &c__0, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ctbcon_("1", "U", "/", &c__0, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ctbcon_("1", "U", "N", &c_n1, &c__0, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ctbcon_("1", "U", "N", &c__0, &c_n1, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ctbcon_("1", "U", "N", &c__2, &c__1, a, &c__1, &rcond, w, rw, &info);
	chkxer_("CTBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CLATBS */

	s_copy(srnamc_1.srnamt, "CLATBS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	clatbs_("/", "N", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("CLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	clatbs_("U", "/", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("CLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	clatbs_("U", "N", "/", "N", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("CLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	clatbs_("U", "N", "N", "/", &c__0, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("CLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	clatbs_("U", "N", "N", "N", &c_n1, &c__0, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("CLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	clatbs_("U", "N", "N", "N", &c__1, &c_n1, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("CLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	clatbs_("U", "N", "N", "N", &c__2, &c__1, a, &c__1, x, &scale, rw, &
		info);
	chkxer_("CLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRTR */

} /* cerrtr_ */
