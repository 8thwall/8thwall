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

/* Subroutine */ int serrtr_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real a[4]	/* was [2][2] */, b[2], w[2], x[2];
    char c2[2];
    real r1[2], r2[2];
    integer iw[2], info;
    real scale, rcond;
    extern /* Subroutine */ int strti2_(char *, char *, integer *, real *, 
	    integer *, integer *), alaesm_(char *, logical *, 
	    integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), slatbs_(char *, char *, char *, char *, 
	    integer *, integer *, real *, integer *, real *, real *, real *, 
	    integer *), stbcon_(char *, char *
, char *, integer *, integer *, real *, integer *, real *, real *, 
	     integer *, integer *), stbrfs_(char *, 
	    char *, char *, integer *, integer *, integer *, real *, integer *
, real *, integer *, real *, integer *, real *, real *, real *, 
	    integer *, integer *), slatps_(char *, 
	    char *, char *, char *, integer *, real *, real *, real *, real *, 
	     integer *), stpcon_(char *, char 
	    *, char *, integer *, real *, real *, real *, integer *, integer *
), slatrs_(char *, char *, char *, char *, 
	     integer *, real *, integer *, real *, real *, real *, integer *), strcon_(char *, char *, char *, 
	    integer *, real *, integer *, real *, real *, integer *, integer *
), stbtrs_(char *, char *, char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, integer *), stprfs_(char *, 
	    char *, char *, integer *, integer *, real *, real *, integer *, 
	    real *, integer *, real *, real *, real *, integer *, integer *), strrfs_(char *, char *, char *, integer *
, integer *, real *, integer *, real *, integer *, real *, 
	    integer *, real *, real *, real *, integer *, integer *), stptri_(char *, char *, integer *, real *, 
	    integer *), strtri_(char *, char *, integer *, 
	    real *, integer *, integer *), stptrs_(char *, 
	    char *, char *, integer *, integer *, real *, real *, integer *, 
	    integer *), strtrs_(char *, char *, char *
, integer *, integer *, real *, integer *, real *, integer *, 
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

/*  SERRTR tests the error exits for the REAL triangular */
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
    a[0] = 1.f;
    a[2] = 2.f;
    a[3] = 3.f;
    a[1] = 4.f;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "TR")) {

/*        Test error exits for the general triangular routines. */

/*        STRTRI */

	s_copy(srnamc_1.srnamt, "STRTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	strtri_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("STRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	strtri_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("STRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	strtri_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("STRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	strtri_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("STRTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STRTI2 */

	s_copy(srnamc_1.srnamt, "STRTI2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	strti2_("/", "N", &c__0, a, &c__1, &info);
	chkxer_("STRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	strti2_("U", "/", &c__0, a, &c__1, &info);
	chkxer_("STRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	strti2_("U", "N", &c_n1, a, &c__1, &info);
	chkxer_("STRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	strti2_("U", "N", &c__2, a, &c__1, &info);
	chkxer_("STRTI2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STRTRS */

	s_copy(srnamc_1.srnamt, "STRTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	strtrs_("/", "N", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	strtrs_("U", "/", "N", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	strtrs_("U", "N", "/", &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	strtrs_("U", "N", "N", &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	strtrs_("U", "N", "N", &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("STRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	strtrs_("U", "N", "N", &c__2, &c__1, a, &c__1, x, &c__2, &info);
	chkxer_("STRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	strtrs_("U", "N", "N", &c__2, &c__1, a, &c__2, x, &c__1, &info);
	chkxer_("STRTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STRRFS */

	s_copy(srnamc_1.srnamt, "STRRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	strrfs_("/", "N", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	strrfs_("U", "/", "N", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	strrfs_("U", "N", "/", &c__0, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	strrfs_("U", "N", "N", &c_n1, &c__0, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	strrfs_("U", "N", "N", &c__0, &c_n1, a, &c__1, b, &c__1, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	strrfs_("U", "N", "N", &c__2, &c__1, a, &c__1, b, &c__2, x, &c__2, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	strrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__1, x, &c__2, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	strrfs_("U", "N", "N", &c__2, &c__1, a, &c__2, b, &c__2, x, &c__1, r1, 
		 r2, w, iw, &info);
	chkxer_("STRRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STRCON */

	s_copy(srnamc_1.srnamt, "STRCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	strcon_("/", "U", "N", &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	strcon_("1", "/", "N", &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	strcon_("1", "U", "/", &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	strcon_("1", "U", "N", &c_n1, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	strcon_("1", "U", "N", &c__2, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STRCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SLATRS */

	s_copy(srnamc_1.srnamt, "SLATRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	slatrs_("/", "N", "N", "N", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("SLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	slatrs_("U", "/", "N", "N", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("SLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	slatrs_("U", "N", "/", "N", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("SLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	slatrs_("U", "N", "N", "/", &c__0, a, &c__1, x, &scale, w, &info);
	chkxer_("SLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	slatrs_("U", "N", "N", "N", &c_n1, a, &c__1, x, &scale, w, &info);
	chkxer_("SLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	slatrs_("U", "N", "N", "N", &c__2, a, &c__1, x, &scale, w, &info);
	chkxer_("SLATRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "TP")) {

/*        Test error exits for the packed triangular routines. */

/*        STPTRI */

	s_copy(srnamc_1.srnamt, "STPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stptri_("/", "N", &c__0, a, &info);
	chkxer_("STPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stptri_("U", "/", &c__0, a, &info);
	chkxer_("STPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stptri_("U", "N", &c_n1, a, &info);
	chkxer_("STPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STPTRS */

	s_copy(srnamc_1.srnamt, "STPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stptrs_("/", "N", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("STPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stptrs_("U", "/", "N", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("STPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stptrs_("U", "N", "/", &c__0, &c__0, a, x, &c__1, &info);
	chkxer_("STPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stptrs_("U", "N", "N", &c_n1, &c__0, a, x, &c__1, &info);
	chkxer_("STPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stptrs_("U", "N", "N", &c__0, &c_n1, a, x, &c__1, &info);
	chkxer_("STPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	stptrs_("U", "N", "N", &c__2, &c__1, a, x, &c__1, &info);
	chkxer_("STPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STPRFS */

	s_copy(srnamc_1.srnamt, "STPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stprfs_("/", "N", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("STPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stprfs_("U", "/", "N", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("STPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stprfs_("U", "N", "/", &c__0, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("STPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stprfs_("U", "N", "N", &c_n1, &c__0, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("STPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stprfs_("U", "N", "N", &c__0, &c_n1, a, b, &c__1, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("STPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	stprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__1, x, &c__2, r1, r2, w, 
		 iw, &info);
	chkxer_("STPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	stprfs_("U", "N", "N", &c__2, &c__1, a, b, &c__2, x, &c__1, r1, r2, w, 
		 iw, &info);
	chkxer_("STPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STPCON */

	s_copy(srnamc_1.srnamt, "STPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stpcon_("/", "U", "N", &c__0, a, &rcond, w, iw, &info);
	chkxer_("STPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stpcon_("1", "/", "N", &c__0, a, &rcond, w, iw, &info);
	chkxer_("STPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stpcon_("1", "U", "/", &c__0, a, &rcond, w, iw, &info);
	chkxer_("STPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stpcon_("1", "U", "N", &c_n1, a, &rcond, w, iw, &info);
	chkxer_("STPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SLATPS */

	s_copy(srnamc_1.srnamt, "SLATPS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	slatps_("/", "N", "N", "N", &c__0, a, x, &scale, w, &info);
	chkxer_("SLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	slatps_("U", "/", "N", "N", &c__0, a, x, &scale, w, &info);
	chkxer_("SLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	slatps_("U", "N", "/", "N", &c__0, a, x, &scale, w, &info);
	chkxer_("SLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	slatps_("U", "N", "N", "/", &c__0, a, x, &scale, w, &info);
	chkxer_("SLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	slatps_("U", "N", "N", "N", &c_n1, a, x, &scale, w, &info);
	chkxer_("SLATPS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "TB")) {

/*        Test error exits for the banded triangular routines. */

/*        STBTRS */

	s_copy(srnamc_1.srnamt, "STBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stbtrs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stbtrs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stbtrs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stbtrs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stbtrs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, x, &c__1, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	stbtrs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, x, &c__1, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	stbtrs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, x, &c__2, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	stbtrs_("U", "N", "N", &c__2, &c__0, &c__1, a, &c__1, x, &c__1, &info);
	chkxer_("STBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STBRFS */

	s_copy(srnamc_1.srnamt, "STBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stbrfs_("/", "N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stbrfs_("U", "/", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stbrfs_("U", "N", "/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stbrfs_("U", "N", "N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stbrfs_("U", "N", "N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	stbrfs_("U", "N", "N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	stbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	stbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__1, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	stbrfs_("U", "N", "N", &c__2, &c__1, &c__1, a, &c__2, b, &c__2, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("STBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        STBCON */

	s_copy(srnamc_1.srnamt, "STBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	stbcon_("/", "U", "N", &c__0, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	stbcon_("1", "/", "N", &c__0, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	stbcon_("1", "U", "/", &c__0, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	stbcon_("1", "U", "N", &c_n1, &c__0, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	stbcon_("1", "U", "N", &c__0, &c_n1, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	stbcon_("1", "U", "N", &c__2, &c__1, a, &c__1, &rcond, w, iw, &info);
	chkxer_("STBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SLATBS */

	s_copy(srnamc_1.srnamt, "SLATBS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	slatbs_("/", "N", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("SLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	slatbs_("U", "/", "N", "N", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("SLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	slatbs_("U", "N", "/", "N", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("SLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	slatbs_("U", "N", "N", "/", &c__0, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("SLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	slatbs_("U", "N", "N", "N", &c_n1, &c__0, a, &c__1, x, &scale, w, &
		info);
	chkxer_("SLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	slatbs_("U", "N", "N", "N", &c__1, &c_n1, a, &c__1, x, &scale, w, &
		info);
	chkxer_("SLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	slatbs_("U", "N", "N", "N", &c__2, &c__1, a, &c__1, x, &scale, w, &
		info);
	chkxer_("SLATBS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRTR */

} /* serrtr_ */
