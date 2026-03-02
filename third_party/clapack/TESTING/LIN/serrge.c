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
static integer c__12 = 12;
static integer c__3 = 3;
static integer c__4 = 4;

/* Subroutine */ int serrge_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real a[16]	/* was [4][4] */, b[4];
    integer i__, j;
    real w[12], x[4];
    char c2[2];
    real r1[4], r2[4], af[16]	/* was [4][4] */;
    integer ip[4], iw[4], info;
    real anrm, ccond, rcond;
    extern /* Subroutine */ int sgbtf2_(integer *, integer *, integer *, 
	    integer *, real *, integer *, integer *, integer *), sgetf2_(
	    integer *, integer *, real *, integer *, integer *, integer *), 
	    alaesm_(char *, logical *, integer *), sgbcon_(char *, 
	    integer *, integer *, integer *, real *, integer *, integer *, 
	    real *, real *, real *, integer *, integer *), sgecon_(
	    char *, integer *, real *, integer *, real *, real *, real *, 
	    integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sgbequ_(integer *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, real *, 
	    real *, integer *), sgbrfs_(char *, integer *, integer *, integer 
	    *, integer *, real *, integer *, real *, integer *, integer *, 
	    real *, integer *, real *, integer *, real *, real *, real *, 
	    integer *, integer *), sgbtrf_(integer *, integer *, 
	    integer *, integer *, real *, integer *, integer *, integer *), 
	    sgeequ_(integer *, integer *, real *, integer *, real *, real *, 
	    real *, real *, real *, integer *), sgerfs_(char *, integer *, 
	    integer *, real *, integer *, real *, integer *, integer *, real *
, integer *, real *, integer *, real *, real *, real *, integer *, 
	     integer *), sgetrf_(integer *, integer *, real *, 
	    integer *, integer *, integer *), sgetri_(integer *, real *, 
	    integer *, integer *, real *, integer *, integer *), sgbtrs_(char 
	    *, integer *, integer *, integer *, integer *, real *, integer *, 
	    integer *, real *, integer *, integer *), sgetrs_(char *, 
	    integer *, integer *, real *, integer *, integer *, real *, 
	    integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRGE tests the error exits for the REAL routines */
/*  for general matrices. */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    infoc_1.nout = *nunit;
    io___1.ciunit = infoc_1.nout;
    s_wsle(&io___1);
    e_wsle();
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);

/*     Set the variables to innocuous values. */

    for (j = 1; j <= 4; ++j) {
	for (i__ = 1; i__ <= 4; ++i__) {
	    a[i__ + (j << 2) - 5] = 1.f / (real) (i__ + j);
	    af[i__ + (j << 2) - 5] = 1.f / (real) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.f;
	r1[j - 1] = 0.f;
	r2[j - 1] = 0.f;
	w[j - 1] = 0.f;
	x[j - 1] = 0.f;
	ip[j - 1] = j;
	iw[j - 1] = j;
/* L20: */
    }
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GE")) {

/*        Test error exits of the routines that use the LU decomposition */
/*        of a general matrix. */

/*        SGETRF */

	s_copy(srnamc_1.srnamt, "SGETRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgetrf_(&c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("SGETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgetrf_(&c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("SGETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgetrf_(&c__2, &c__1, a, &c__1, ip, &info);
	chkxer_("SGETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGETF2 */

	s_copy(srnamc_1.srnamt, "SGETF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgetf2_(&c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("SGETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgetf2_(&c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("SGETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgetf2_(&c__2, &c__1, a, &c__1, ip, &info);
	chkxer_("SGETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGETRI */

	s_copy(srnamc_1.srnamt, "SGETRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgetri_(&c_n1, a, &c__1, ip, w, &c__12, &info);
	chkxer_("SGETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgetri_(&c__2, a, &c__1, ip, w, &c__12, &info);
	chkxer_("SGETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGETRS */

	s_copy(srnamc_1.srnamt, "SGETRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgetrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgetrs_("N", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgetrs_("N", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgetrs_("N", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("SGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sgetrs_("N", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("SGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGERFS */

	s_copy(srnamc_1.srnamt, "SGERFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgerfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgerfs_("N", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgerfs_("N", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgerfs_("N", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgerfs_("N", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sgerfs_("N", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sgerfs_("N", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGECON */

	s_copy(srnamc_1.srnamt, "SGECON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgecon_("/", &c__0, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SGECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgecon_("1", &c_n1, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SGECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgecon_("1", &c__2, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SGECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGEEQU */

	s_copy(srnamc_1.srnamt, "SGEEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgeequ_(&c_n1, &c__0, a, &c__1, r1, r2, &rcond, &ccond, &anrm, &info);
	chkxer_("SGEEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgeequ_(&c__0, &c_n1, a, &c__1, r1, r2, &rcond, &ccond, &anrm, &info);
	chkxer_("SGEEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgeequ_(&c__2, &c__2, a, &c__1, r1, r2, &rcond, &ccond, &anrm, &info);
	chkxer_("SGEEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        Test error exits of the routines that use the LU decomposition */
/*        of a general band matrix. */

/*        SGBTRF */

	s_copy(srnamc_1.srnamt, "SGBTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbtrf_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("SGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbtrf_(&c__0, &c_n1, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("SGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbtrf_(&c__1, &c__1, &c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("SGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbtrf_(&c__1, &c__1, &c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("SGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgbtrf_(&c__2, &c__2, &c__1, &c__1, a, &c__3, ip, &info);
	chkxer_("SGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGBTF2 */

	s_copy(srnamc_1.srnamt, "SGBTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbtf2_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("SGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbtf2_(&c__0, &c_n1, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("SGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbtf2_(&c__1, &c__1, &c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("SGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbtf2_(&c__1, &c__1, &c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("SGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgbtf2_(&c__2, &c__2, &c__1, &c__1, a, &c__3, ip, &info);
	chkxer_("SGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGBTRS */

	s_copy(srnamc_1.srnamt, "SGBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbtrs_("/", &c__0, &c__0, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("SGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbtrs_("N", &c_n1, &c__0, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("SGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbtrs_("N", &c__1, &c_n1, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("SGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbtrs_("N", &c__1, &c__0, &c_n1, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("SGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgbtrs_("N", &c__1, &c__0, &c__0, &c_n1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("SGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgbtrs_("N", &c__2, &c__1, &c__1, &c__1, a, &c__3, ip, b, &c__2, &
		info);
	chkxer_("SGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sgbtrs_("N", &c__2, &c__0, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("SGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGBRFS */

	s_copy(srnamc_1.srnamt, "SGBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbrfs_("/", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbrfs_("N", &c_n1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbrfs_("N", &c__1, &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbrfs_("N", &c__1, &c__0, &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgbrfs_("N", &c__1, &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgbrfs_("N", &c__2, &c__1, &c__1, &c__1, a, &c__2, af, &c__4, ip, b, &
		c__2, x, &c__2, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sgbrfs_("N", &c__2, &c__1, &c__1, &c__1, a, &c__3, af, &c__3, ip, b, &
		c__2, x, &c__2, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sgbrfs_("N", &c__2, &c__0, &c__0, &c__1, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__2, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	sgbrfs_("N", &c__2, &c__0, &c__0, &c__1, a, &c__1, af, &c__1, ip, b, &
		c__2, x, &c__1, r1, r2, w, iw, &info);
	chkxer_("SGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGBCON */

	s_copy(srnamc_1.srnamt, "SGBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbcon_("/", &c__0, &c__0, &c__0, a, &c__1, ip, &anrm, &rcond, w, iw, 
		&info);
	chkxer_("SGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbcon_("1", &c_n1, &c__0, &c__0, a, &c__1, ip, &anrm, &rcond, w, iw, 
		&info);
	chkxer_("SGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbcon_("1", &c__1, &c_n1, &c__0, a, &c__1, ip, &anrm, &rcond, w, iw, 
		&info);
	chkxer_("SGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbcon_("1", &c__1, &c__0, &c_n1, a, &c__1, ip, &anrm, &rcond, w, iw, 
		&info);
	chkxer_("SGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgbcon_("1", &c__2, &c__1, &c__1, a, &c__3, ip, &anrm, &rcond, w, iw, 
		&info);
	chkxer_("SGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGBEQU */

	s_copy(srnamc_1.srnamt, "SGBEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgbequ_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("SGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgbequ_(&c__0, &c_n1, &c__0, &c__0, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("SGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgbequ_(&c__1, &c__1, &c_n1, &c__0, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("SGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgbequ_(&c__1, &c__1, &c__0, &c_n1, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("SGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgbequ_(&c__2, &c__2, &c__1, &c__1, a, &c__2, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("SGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRGE */

} /* serrge_ */
