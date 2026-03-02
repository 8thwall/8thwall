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

/* Subroutine */ int derrls_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal a[4]	/* was [2][2] */, b[4]	/* was [2][2] */, s[2], w[2];
    char c2[2];
    integer ip[2], info, irnk;
    extern /* Subroutine */ int dgels_(char *, integer *, integer *, integer *
, doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, integer *);
    doublereal rcond;
    extern /* Subroutine */ int alaesm_(char *, logical *, integer *),
	     dgelsd_(integer *, integer *, integer *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int dgelss_(integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), 
	    chkxer_(char *, integer *, integer *, logical *, logical *), dgelsx_(integer *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *), dgelsy_(integer *, integer *, 
	     integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
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

/*  DERRLS tests the error exits for the DOUBLE PRECISION least squares */
/*  driver routines (DGELS, SGELSS, SGELSX, SGELSY, SGELSD). */

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

    if (lsamen_(&c__2, c2, "LS")) {

/*        Test error exits for the least squares driver routines. */

/*        DGELS */

	s_copy(srnamc_1.srnamt, "DGELS ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgels_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("DGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgels_("N", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("DGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgels_("N", &c__0, &c_n1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("DGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgels_("N", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("DGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dgels_("N", &c__2, &c__0, &c__0, a, &c__1, b, &c__2, w, &c__2, &info);
	chkxer_("DGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dgels_("N", &c__2, &c__0, &c__0, a, &c__2, b, &c__1, w, &c__2, &info);
	chkxer_("DGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dgels_("N", &c__1, &c__1, &c__0, a, &c__1, b, &c__1, w, &c__1, &info);
	chkxer_("DGELS ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGELSS */

	s_copy(srnamc_1.srnamt, "DGELSS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgelss_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, &info);
	chkxer_("DGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgelss_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, &info);
	chkxer_("DGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgelss_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__1, &info);
	chkxer_("DGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgelss_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__2, &info);
	chkxer_("DGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgelss_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__2, &info);
	chkxer_("DGELSS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGELSX */

	s_copy(srnamc_1.srnamt, "DGELSX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgelsx_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("DGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgelsx_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("DGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgelsx_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("DGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgelsx_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("DGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgelsx_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 &info);
	chkxer_("DGELSX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGELSY */

	s_copy(srnamc_1.srnamt, "DGELSY", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgelsy_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("DGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgelsy_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("DGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgelsy_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("DGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgelsy_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("DGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgelsy_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, ip, &rcond, &irnk, w, 
		 &c__10, &info);
	chkxer_("DGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dgelsy_(&c__2, &c__2, &c__1, a, &c__2, b, &c__2, ip, &rcond, &irnk, w, 
		 &c__1, &info);
	chkxer_("DGELSY", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DGELSD */

	s_copy(srnamc_1.srnamt, "DGELSD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgelsd_(&c_n1, &c__0, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("DGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgelsd_(&c__0, &c_n1, &c__0, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("DGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgelsd_(&c__0, &c__0, &c_n1, a, &c__1, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("DGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgelsd_(&c__2, &c__0, &c__0, a, &c__1, b, &c__2, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("DGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgelsd_(&c__2, &c__0, &c__0, a, &c__2, b, &c__1, s, &rcond, &irnk, w, 
		&c__10, ip, &info);
	chkxer_("DGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dgelsd_(&c__2, &c__2, &c__1, a, &c__2, b, &c__2, s, &rcond, &irnk, w, 
		&c__1, ip, &info);
	chkxer_("DGELSD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of DERRLS */

} /* derrls_ */
