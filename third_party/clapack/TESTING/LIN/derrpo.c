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

/* Subroutine */ int derrpo_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal a[16]	/* was [4][4] */, b[4];
    integer i__, j;
    doublereal w[12], x[4];
    char c2[2];
    doublereal r1[4], r2[4], af[16]	/* was [4][4] */;
    integer iw[4], info;
    doublereal anrm, rcond;
    extern /* Subroutine */ int dpbtf2_(char *, integer *, integer *, 
	    doublereal *, integer *, integer *), dpotf2_(char *, 
	    integer *, doublereal *, integer *, integer *), alaesm_(
	    char *, logical *, integer *), dpbcon_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int dpbequ_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *), dpbrfs_(char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, integer *), 
	    dpbtrf_(char *, integer *, integer *, doublereal *, integer *, 
	    integer *), dpocon_(char *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    integer *), chkxer_(char *, integer *, integer *, logical 
	    *, logical *), dppcon_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *), dpoequ_(integer *, doublereal *, integer *, doublereal *, 
	     doublereal *, doublereal *, integer *), dpbtrs_(char *, integer *
, integer *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, integer *), dporfs_(char *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), dpotrf_(char *, 
	    integer *, doublereal *, integer *, integer *), dpotri_(
	    char *, integer *, doublereal *, integer *, integer *), 
	    dppequ_(char *, integer *, doublereal *, doublereal *, doublereal 
	    *, doublereal *, integer *), dpprfs_(char *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *), dpptrf_(char *, integer *, 
	    doublereal *, integer *), dpptri_(char *, integer *, 
	    doublereal *, integer *), dpotrs_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *), dpptrs_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERRPO tests the error exits for the DOUBLE PRECISION routines */
/*  for symmetric positive definite matrices. */

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
	    a[i__ + (j << 2) - 5] = 1. / (doublereal) (i__ + j);
	    af[i__ + (j << 2) - 5] = 1. / (doublereal) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.;
	r1[j - 1] = 0.;
	r2[j - 1] = 0.;
	w[j - 1] = 0.;
	x[j - 1] = 0.;
	iw[j - 1] = j;
/* L20: */
    }
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "PO")) {

/*        Test error exits of the routines that use the Cholesky */
/*        decomposition of a symmetric positive definite matrix. */

/*        DPOTRF */

	s_copy(srnamc_1.srnamt, "DPOTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpotrf_("/", &c__0, a, &c__1, &info);
	chkxer_("DPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpotrf_("U", &c_n1, a, &c__1, &info);
	chkxer_("DPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpotrf_("U", &c__2, a, &c__1, &info);
	chkxer_("DPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPOTF2 */

	s_copy(srnamc_1.srnamt, "DPOTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpotf2_("/", &c__0, a, &c__1, &info);
	chkxer_("DPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpotf2_("U", &c_n1, a, &c__1, &info);
	chkxer_("DPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpotf2_("U", &c__2, a, &c__1, &info);
	chkxer_("DPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPOTRI */

	s_copy(srnamc_1.srnamt, "DPOTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpotri_("/", &c__0, a, &c__1, &info);
	chkxer_("DPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpotri_("U", &c_n1, a, &c__1, &info);
	chkxer_("DPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpotri_("U", &c__2, a, &c__1, &info);
	chkxer_("DPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPOTRS */

	s_copy(srnamc_1.srnamt, "DPOTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpotrs_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("DPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpotrs_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("DPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpotrs_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("DPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dpotrs_("U", &c__2, &c__1, a, &c__1, b, &c__2, &info);
	chkxer_("DPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dpotrs_("U", &c__2, &c__1, a, &c__2, b, &c__1, &info);
	chkxer_("DPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPORFS */

	s_copy(srnamc_1.srnamt, "DPORFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dporfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("DPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dporfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("DPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dporfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("DPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dporfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &c__2, 
		r1, r2, w, iw, &info);
	chkxer_("DPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dporfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &c__2, 
		r1, r2, w, iw, &info);
	chkxer_("DPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__1, x, &c__2, 
		r1, r2, w, iw, &info);
	chkxer_("DPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__2, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("DPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPOCON */

	s_copy(srnamc_1.srnamt, "DPOCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpocon_("/", &c__0, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("DPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpocon_("U", &c_n1, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("DPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpocon_("U", &c__2, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("DPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPOEQU */

	s_copy(srnamc_1.srnamt, "DPOEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpoequ_(&c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("DPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpoequ_(&c__2, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("DPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        Test error exits of the routines that use the Cholesky */
/*        decomposition of a symmetric positive definite packed matrix. */

/*        DPPTRF */

	s_copy(srnamc_1.srnamt, "DPPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpptrf_("/", &c__0, a, &info);
	chkxer_("DPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpptrf_("U", &c_n1, a, &info);
	chkxer_("DPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPPTRI */

	s_copy(srnamc_1.srnamt, "DPPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpptri_("/", &c__0, a, &info);
	chkxer_("DPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpptri_("U", &c_n1, a, &info);
	chkxer_("DPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPPTRS */

	s_copy(srnamc_1.srnamt, "DPPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpptrs_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("DPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpptrs_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("DPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpptrs_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("DPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dpptrs_("U", &c__2, &c__1, a, b, &c__1, &info);
	chkxer_("DPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPPRFS */

	s_copy(srnamc_1.srnamt, "DPPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpprfs_("/", &c__0, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("DPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpprfs_("U", &c_n1, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("DPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpprfs_("U", &c__0, &c_n1, a, af, b, &c__1, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("DPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dpprfs_("U", &c__2, &c__1, a, af, b, &c__1, x, &c__2, r1, r2, w, iw, &
		info);
	chkxer_("DPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dpprfs_("U", &c__2, &c__1, a, af, b, &c__2, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("DPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPPCON */

	s_copy(srnamc_1.srnamt, "DPPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dppcon_("/", &c__0, a, &anrm, &rcond, w, iw, &info);
	chkxer_("DPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dppcon_("U", &c_n1, a, &anrm, &rcond, w, iw, &info);
	chkxer_("DPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPPEQU */

	s_copy(srnamc_1.srnamt, "DPPEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dppequ_("/", &c__0, a, r1, &rcond, &anrm, &info);
	chkxer_("DPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dppequ_("U", &c_n1, a, r1, &rcond, &anrm, &info);
	chkxer_("DPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        Test error exits of the routines that use the Cholesky */
/*        decomposition of a symmetric positive definite band matrix. */

/*        DPBTRF */

	s_copy(srnamc_1.srnamt, "DPBTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbtrf_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("DPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbtrf_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("DPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbtrf_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("DPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dpbtrf_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("DPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPBTF2 */

	s_copy(srnamc_1.srnamt, "DPBTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbtf2_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("DPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbtf2_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("DPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbtf2_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("DPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dpbtf2_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("DPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPBTRS */

	s_copy(srnamc_1.srnamt, "DPBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbtrs_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("DPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbtrs_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("DPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbtrs_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("DPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpbtrs_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("DPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dpbtrs_("U", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("DPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dpbtrs_("U", &c__2, &c__0, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("DPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPBRFS */

	s_copy(srnamc_1.srnamt, "DPBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbrfs_("/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbrfs_("U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbrfs_("U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dpbrfs_("U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dpbrfs_("U", &c__2, &c__1, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dpbrfs_("U", &c__2, &c__1, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dpbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dpbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__2, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPBCON */

	s_copy(srnamc_1.srnamt, "DPBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbcon_("/", &c__0, &c__0, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("DPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbcon_("U", &c_n1, &c__0, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("DPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbcon_("U", &c__1, &c_n1, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("DPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dpbcon_("U", &c__2, &c__1, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("DPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DPBEQU */

	s_copy(srnamc_1.srnamt, "DPBEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dpbequ_("/", &c__0, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("DPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dpbequ_("U", &c_n1, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("DPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dpbequ_("U", &c__1, &c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("DPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dpbequ_("U", &c__2, &c__1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("DPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of DERRPO */

} /* derrpo_ */
