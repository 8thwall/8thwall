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
static integer c__3 = 3;

/* Subroutine */ int cerrls_(char *path, integer *nunit)
{
    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), e_wsle(void);

    /* Local variables */
    complex a[4]	/* was [2][2] */, b[4]	/* was [2][2] */;
    real s[2];
    complex w[2];
    char c2[2];
    integer ip[2];
    real rw[2];
    integer info, irnk;
    extern /* Subroutine */ int cgels_(char *, integer *, integer *, integer *
, complex *, integer *, complex *, integer *, complex *, integer *
, integer *);
    real rcond;
    extern /* Subroutine */ int cgelsd_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, 
	    integer *, complex *, integer *, real *, integer *, integer *), 
	    alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int cgelss_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, 
	    integer *, complex *, integer *, real *, integer *), chkxer_(char 
	    *, integer *, integer *, logical *, logical *), cgelsx_(
	    integer *, integer *, integer *, complex *, integer *, complex *, 
	    integer *, integer *, real *, integer *, complex *, real *, 
	    integer *), cgelsy_(integer *, integer *, integer *, complex *, 
	    integer *, complex *, integer *, integer *, real *, integer *, 
	    complex *, integer *, real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRLS tests the error exits for the COMPLEX least squares */
/*  driver routines (CGELS, CGELSS, CGELSX, CGELSY, CGELSD). */

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
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);
    a[0].r = 1.f, a[0].i = 0.f;
    a[2].r = 2.f, a[2].i = 0.f;
    a[3].r = 3.f, a[3].i = 0.f;
    a[1].r = 4.f, a[1].i = 0.f;
    infoc_1.ok = TRUE_;
    io___3.ciunit = infoc_1.nout;
    s_wsle(&io___3);
    e_wsle();

/*     Test error exits for the least squares driver routines. */

    if (lsamen_(&c__2, c2, "LS")) {

/*        CGELS */

	s_copy(srnamc_1.srnamt, "CGELS ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgels_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("CGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgels_("N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("CGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgels_("N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("CGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgels_("N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("CGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cgels_("N", &c__2, &c__0, &c__0, a, &c__1, b, &c__2, w, &c__2, &info);
	chkxer_("CGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cgels_("N", &c__2, &c__0, &c__0, a, &c__2, b, &c__1, w, &c__2, &info);
	chkxer_("CGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgels_("N", &c__1, &c__1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("CGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGELSS */

	s_copy(srnamc_1.srnamt, "CGELSS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgelss_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, rw, &info);
	chkxer_("CGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgelss_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, rw, &info);
	chkxer_("CGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgelss_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, rw, &info);
	chkxer_("CGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgelss_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__2, rw, &info);
	chkxer_("CGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgelss_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__2, rw, &info);
	chkxer_("CGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGELSX */

	s_copy(srnamc_1.srnamt, "CGELSX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgelsx_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("CGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgelsx_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("CGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgelsx_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("CGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgelsx_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("CGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgelsx_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("CGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGELSY */

	s_copy(srnamc_1.srnamt, "CGELSY", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgelsy_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("CGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgelsy_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("CGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgelsy_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("CGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgelsy_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("CGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgelsy_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("CGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cgelsy_(&c__0, &c__3, &c__0, a, &c__1, b, &c__3, ip, &rcond, &irnk, w, 
		 &c__1, rw, &info);
	chkxer_("CGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGELSD */

	s_copy(srnamc_1.srnamt, "CGELSD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgelsd_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("CGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgelsd_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("CGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgelsd_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("CGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgelsd_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("CGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgelsd_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("CGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cgelsd_(&c__2, &c__2, &c__1, a, &c__2, b, &c__2, s, &rcond, &irnk, w, 
		&c__1, rw, ip, &info);
	chkxer_("CGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRLS */

} /* cerrls_ */
