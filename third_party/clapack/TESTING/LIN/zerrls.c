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

/* Subroutine */ int zerrls_(char *path, integer *nunit)
{
    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), e_wsle(void);

    /* Local variables */
    doublecomplex a[4]	/* was [2][2] */, b[4]	/* was [2][2] */;
    doublereal s[2];
    doublecomplex w[2];
    char c2[2];
    integer ip[2];
    doublereal rw[2];
    integer info, irnk;
    doublereal rcond;
    extern /* Subroutine */ int zgels_(char *, integer *, integer *, integer *
, doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *), alaesm_(char *, 
	    logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zgelsd_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, integer *, doublecomplex *, integer *, 
	     doublereal *, integer *, integer *), zgelss_(integer *, integer *
, integer *, doublecomplex *, integer *, doublecomplex *, integer 
	    *, doublereal *, doublereal *, integer *, doublecomplex *, 
	    integer *, doublereal *, integer *), zgelsx_(integer *, integer *, 
	     integer *, doublecomplex *, integer *, doublecomplex *, integer *
, integer *, doublereal *, integer *, doublecomplex *, doublereal 
	    *, integer *), zgelsy_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     doublereal *, integer *, doublecomplex *, integer *, doublereal *
, integer *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRLS tests the error exits for the COMPLEX*16 least squares */
/*  driver routines (ZGELS, CGELSS, CGELSX, CGELSY, CGELSD). */

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
    a[0].r = 1., a[0].i = 0.;
    a[2].r = 2., a[2].i = 0.;
    a[3].r = 3., a[3].i = 0.;
    a[1].r = 4., a[1].i = 0.;
    infoc_1.ok = TRUE_;
    io___3.ciunit = infoc_1.nout;
    s_wsle(&io___3);
    e_wsle();

/*     Test error exits for the least squares driver routines. */

    if (lsamen_(&c__2, c2, "LS")) {

/*        ZGELS */

	s_copy(srnamc_1.srnamt, "ZGELS ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgels_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("ZGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgels_("N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("ZGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgels_("N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("ZGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgels_("N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("ZGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgels_("N", &c__2, &c__0, &c__0, a, &c__1, b, &c__2, w, &c__2, &info);
	chkxer_("ZGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zgels_("N", &c__2, &c__0, &c__0, a, &c__2, b, &c__1, w, &c__2, &info);
	chkxer_("ZGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgels_("N", &c__1, &c__1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("ZGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGELSS */

	s_copy(srnamc_1.srnamt, "ZGELSS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgelss_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, rw, &info);
	chkxer_("ZGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgelss_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, rw, &info);
	chkxer_("ZGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgelss_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, rw, &info);
	chkxer_("ZGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgelss_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__2, rw, &info);
	chkxer_("ZGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgelss_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__2, rw, &info);
	chkxer_("ZGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGELSX */

	s_copy(srnamc_1.srnamt, "ZGELSX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgelsx_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("ZGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgelsx_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("ZGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgelsx_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("ZGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgelsx_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("ZGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgelsx_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 rw, &info);
	chkxer_("ZGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGELSY */

	s_copy(srnamc_1.srnamt, "ZGELSY", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgelsy_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("ZGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgelsy_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("ZGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgelsy_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("ZGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgelsy_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("ZGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgelsy_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, rw, &info);
	chkxer_("ZGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgelsy_(&c__0, &c__3, &c__0, a, &c__1, b, &c__3, ip, &rcond, &irnk, w, 
		 &c__1, rw, &info);
	chkxer_("ZGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGELSD */

	s_copy(srnamc_1.srnamt, "ZGELSD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgelsd_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("ZGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgelsd_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("ZGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgelsd_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("ZGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgelsd_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("ZGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgelsd_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, rw, ip, &info);
	chkxer_("ZGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgelsd_(&c__2, &c__2, &c__1, a, &c__2, b, &c__2, s, &rcond, &irnk, w, 
		&c__1, rw, ip, &info);
	chkxer_("ZGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRLS */

} /* zerrls_ */
