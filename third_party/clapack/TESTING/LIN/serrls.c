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
static integer c__10 = 10;

/* Subroutine */ int serrls_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real a[4]	/* was [2][2] */, b[4]	/* was [2][2] */, s[2], w[2];
    char c2[2];
    integer ip[2], info, irnk;
    real rcond;
    extern /* Subroutine */ int sgels_(char *, integer *, integer *, integer *
, real *, integer *, real *, integer *, real *, integer *, 
	    integer *), alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int sgelsd_(integer *, integer *, integer *, real 
	    *, integer *, real *, integer *, real *, real *, integer *, real *
, integer *, integer *, integer *), chkxer_(char *, integer *, 
	    integer *, logical *, logical *), sgelss_(integer *, 
	    integer *, integer *, real *, integer *, real *, integer *, real *
, real *, integer *, real *, integer *, integer *), sgelsx_(
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, integer *, real *, integer *, real *, integer *), 
	    sgelsy_(integer *, integer *, integer *, real *, integer *, real *
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

/*  SERRLS tests the error exits for the REAL least squares */
/*  driver routines (SGELS, SGELSS, SGELSX, SGELSY, SGELSD). */

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

    if (lsamen_(&c__2, c2, "LS")) {

/*        Test error exits for the least squares driver routines. */

/*        SGELS */

	s_copy(srnamc_1.srnamt, "SGELS ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgels_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("SGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgels_("N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("SGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgels_("N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("SGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgels_("N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("SGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgels_("N", &c__2, &c__0, &c__0, a, &c__1, b, &c__2, w, &c__2, &info);
	chkxer_("SGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sgels_("N", &c__2, &c__0, &c__0, a, &c__2, b, &c__1, w, &c__2, &info);
	chkxer_("SGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sgels_("N", &c__1, &c__1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("SGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGELSS */

	s_copy(srnamc_1.srnamt, "SGELSS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgelss_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, &info);
	chkxer_("SGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgelss_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, &info);
	chkxer_("SGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgelss_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, &info);
	chkxer_("SGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgelss_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__2, &info);
	chkxer_("SGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgelss_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__2, &info);
	chkxer_("SGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGELSX */

	s_copy(srnamc_1.srnamt, "SGELSX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgelsx_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("SGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgelsx_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("SGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgelsx_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("SGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgelsx_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("SGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgelsx_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("SGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGELSY */

	s_copy(srnamc_1.srnamt, "SGELSY", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgelsy_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("SGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgelsy_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("SGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgelsy_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("SGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgelsy_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("SGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgelsy_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("SGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sgelsy_(&c__2, &c__2, &c__1, a, &c__2, b, &c__2, ip, &rcond, &irnk, w, 
		 &c__1, &info);
	chkxer_("SGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGELSD */

	s_copy(srnamc_1.srnamt, "SGELSD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgelsd_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("SGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgelsd_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("SGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgelsd_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("SGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgelsd_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("SGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgelsd_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("SGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sgelsd_(&c__2, &c__2, &c__1, a, &c__2, b, &c__2, s, &rcond, &irnk, w, 
		&c__1, ip, &info);
	chkxer_("SGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRLS */

} /* serrls_ */
