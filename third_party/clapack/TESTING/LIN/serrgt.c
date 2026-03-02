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

/* Subroutine */ int serrgt_(char *path, integer *nunit)
{
    /* System generated locals */
    real r__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real b[2], c__[2], d__[2], e[2], f[2], w[2], x[2];
    char c2[2];
    real r1[2], r2[2], cf[2], df[2], ef[2];
    integer ip[2], iw[2], info;
    real rcond, anorm;
    extern /* Subroutine */ int alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sgtcon_(char *, integer *, real *, real *, 
	    real *, real *, integer *, real *, real *, real *, integer *, 
	    integer *), sptcon_(integer *, real *, real *, real *, 
	    real *, real *, integer *), sgtrfs_(char *, integer *, integer *, 
	    real *, real *, real *, real *, real *, real *, real *, integer *, 
	     real *, integer *, real *, integer *, real *, real *, real *, 
	    integer *, integer *), sgttrf_(integer *, real *, real *, 
	    real *, real *, integer *, integer *), sptrfs_(integer *, integer 
	    *, real *, real *, real *, real *, real *, integer *, real *, 
	    integer *, real *, real *, real *, integer *), spttrf_(integer *, 
	    real *, real *, integer *), sgttrs_(char *, integer *, integer *, 
	    real *, real *, real *, real *, integer *, real *, integer *, 
	    integer *), spttrs_(integer *, integer *, real *, real *, 
	    real *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRGT tests the error exits for the REAL tridiagonal */
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
    d__[0] = 1.f;
    d__[1] = 2.f;
    df[0] = 1.f;
    df[1] = 2.f;
    e[0] = 3.f;
    e[1] = 4.f;
    ef[0] = 3.f;
    ef[1] = 4.f;
    anorm = 1.f;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GT")) {

/*        Test error exits for the general tridiagonal routines. */

/*        SGTTRF */

	s_copy(srnamc_1.srnamt, "SGTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgttrf_(&c_n1, c__, d__, e, f, ip, &info);
	chkxer_("SGTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGTTRS */

	s_copy(srnamc_1.srnamt, "SGTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgttrs_("/", &c__0, &c__0, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("SGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgttrs_("N", &c_n1, &c__0, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("SGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgttrs_("N", &c__0, &c_n1, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("SGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sgttrs_("N", &c__2, &c__1, c__, d__, e, f, ip, x, &c__1, &info);
	chkxer_("SGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGTRFS */

	s_copy(srnamc_1.srnamt, "SGTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgtrfs_("/", &c__0, &c__0, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgtrfs_("N", &c_n1, &c__0, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgtrfs_("N", &c__0, &c_n1, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sgtrfs_("N", &c__2, &c__1, c__, d__, e, cf, df, ef, f, ip, b, &c__1, 
		x, &c__2, r1, r2, w, iw, &info);
	chkxer_("SGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	sgtrfs_("N", &c__2, &c__1, c__, d__, e, cf, df, ef, f, ip, b, &c__2, 
		x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGTCON */

	s_copy(srnamc_1.srnamt, "SGTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgtcon_("/", &c__0, c__, d__, e, f, ip, &anorm, &rcond, w, iw, &info);
	chkxer_("SGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgtcon_("I", &c_n1, c__, d__, e, f, ip, &anorm, &rcond, w, iw, &info);
	chkxer_("SGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	r__1 = -anorm;
	sgtcon_("I", &c__0, c__, d__, e, f, ip, &r__1, &rcond, w, iw, &info);
	chkxer_("SGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        Test error exits for the positive definite tridiagonal */
/*        routines. */

/*        SPTTRF */

	s_copy(srnamc_1.srnamt, "SPTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spttrf_(&c_n1, d__, e, &info);
	chkxer_("SPTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPTTRS */

	s_copy(srnamc_1.srnamt, "SPTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spttrs_(&c_n1, &c__0, d__, e, x, &c__1, &info);
	chkxer_("SPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spttrs_(&c__0, &c_n1, d__, e, x, &c__1, &info);
	chkxer_("SPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	spttrs_(&c__2, &c__1, d__, e, x, &c__1, &info);
	chkxer_("SPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPTRFS */

	s_copy(srnamc_1.srnamt, "SPTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sptrfs_(&c_n1, &c__0, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, w, &
		info);
	chkxer_("SPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sptrfs_(&c__0, &c_n1, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, w, &
		info);
	chkxer_("SPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sptrfs_(&c__2, &c__1, d__, e, df, ef, b, &c__1, x, &c__2, r1, r2, w, &
		info);
	chkxer_("SPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sptrfs_(&c__2, &c__1, d__, e, df, ef, b, &c__2, x, &c__1, r1, r2, w, &
		info);
	chkxer_("SPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPTCON */

	s_copy(srnamc_1.srnamt, "SPTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sptcon_(&c_n1, d__, e, &anorm, &rcond, w, &info);
	chkxer_("SPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	r__1 = -anorm;
	sptcon_(&c__0, d__, e, &r__1, &rcond, w, &info);
	chkxer_("SPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRGT */

} /* serrgt_ */
