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

/* Subroutine */ int cerrpo_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    complex q__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    complex a[16]	/* was [4][4] */, b[4];
    integer i__, j;
    real r__[4];
    complex w[8], x[4];
    char c2[2];
    real r1[4], r2[4];
    complex af[16]	/* was [4][4] */;
    integer info;
    real anrm, rcond;
    extern /* Subroutine */ int cpbtf2_(char *, integer *, integer *, complex 
	    *, integer *, integer *), cpotf2_(char *, integer *, 
	    complex *, integer *, integer *), alaesm_(char *, logical 
	    *, integer *), cpbcon_(char *, integer *, integer *, 
	    complex *, integer *, real *, real *, complex *, real *, integer *
);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int cpbequ_(char *, integer *, integer *, complex 
	    *, integer *, real *, real *, real *, integer *), cpbrfs_(
	    char *, integer *, integer *, integer *, complex *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    real *, real *, complex *, real *, integer *), cpbtrf_(
	    char *, integer *, integer *, complex *, integer *, integer *), cpocon_(char *, integer *, complex *, integer *, real *, 
	    real *, complex *, real *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), cppcon_(char 
	    *, integer *, complex *, real *, real *, complex *, real *, 
	    integer *), cpoequ_(integer *, complex *, integer *, real 
	    *, real *, real *, integer *), cpbtrs_(char *, integer *, integer 
	    *, integer *, complex *, integer *, complex *, integer *, integer 
	    *), cporfs_(char *, integer *, integer *, complex *, 
	    integer *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, real *, complex *, real *, integer *), 
	    cpotrf_(char *, integer *, complex *, integer *, integer *), cpotri_(char *, integer *, complex *, integer *, integer 
	    *), cppequ_(char *, integer *, complex *, real *, real *, 
	    real *, integer *), cpprfs_(char *, integer *, integer *, 
	    complex *, complex *, complex *, integer *, complex *, integer *, 
	    real *, real *, complex *, real *, integer *), cpptrf_(
	    char *, integer *, complex *, integer *), cpptri_(char *, 
	    integer *, complex *, integer *), cpotrs_(char *, integer 
	    *, integer *, complex *, integer *, complex *, integer *, integer 
	    *), cpptrs_(char *, integer *, integer *, complex *, 
	    complex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRPO tests the error exits for the COMPLEX routines */
/*  for Hermitian positive definite matrices. */

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
	    i__1 = i__ + (j << 2) - 5;
	    r__1 = 1.f / (real) (i__ + j);
	    r__2 = -1.f / (real) (i__ + j);
	    q__1.r = r__1, q__1.i = r__2;
	    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	    i__1 = i__ + (j << 2) - 5;
	    r__1 = 1.f / (real) (i__ + j);
	    r__2 = -1.f / (real) (i__ + j);
	    q__1.r = r__1, q__1.i = r__2;
	    af[i__1].r = q__1.r, af[i__1].i = q__1.i;
/* L10: */
	}
	i__1 = j - 1;
	b[i__1].r = 0.f, b[i__1].i = 0.f;
	r1[j - 1] = 0.f;
	r2[j - 1] = 0.f;
	i__1 = j - 1;
	w[i__1].r = 0.f, w[i__1].i = 0.f;
	i__1 = j - 1;
	x[i__1].r = 0.f, x[i__1].i = 0.f;
/* L20: */
    }
    anrm = 1.f;
    infoc_1.ok = TRUE_;

/*     Test error exits of the routines that use the Cholesky */
/*     decomposition of a Hermitian positive definite matrix. */

    if (lsamen_(&c__2, c2, "PO")) {

/*        CPOTRF */

	s_copy(srnamc_1.srnamt, "CPOTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpotrf_("/", &c__0, a, &c__1, &info);
	chkxer_("CPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpotrf_("U", &c_n1, a, &c__1, &info);
	chkxer_("CPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpotrf_("U", &c__2, a, &c__1, &info);
	chkxer_("CPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPOTF2 */

	s_copy(srnamc_1.srnamt, "CPOTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpotf2_("/", &c__0, a, &c__1, &info);
	chkxer_("CPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpotf2_("U", &c_n1, a, &c__1, &info);
	chkxer_("CPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpotf2_("U", &c__2, a, &c__1, &info);
	chkxer_("CPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPOTRI */

	s_copy(srnamc_1.srnamt, "CPOTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpotri_("/", &c__0, a, &c__1, &info);
	chkxer_("CPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpotri_("U", &c_n1, a, &c__1, &info);
	chkxer_("CPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpotri_("U", &c__2, a, &c__1, &info);
	chkxer_("CPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPOTRS */

	s_copy(srnamc_1.srnamt, "CPOTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpotrs_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("CPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpotrs_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("CPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpotrs_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("CPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cpotrs_("U", &c__2, &c__1, a, &c__1, b, &c__2, &info);
	chkxer_("CPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cpotrs_("U", &c__2, &c__1, a, &c__2, b, &c__1, &info);
	chkxer_("CPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPORFS */

	s_copy(srnamc_1.srnamt, "CPORFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cporfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("CPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cporfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("CPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cporfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("CPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cporfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &c__2, 
		r1, r2, w, r__, &info);
	chkxer_("CPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cporfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &c__2, 
		r1, r2, w, r__, &info);
	chkxer_("CPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__1, x, &c__2, 
		r1, r2, w, r__, &info);
	chkxer_("CPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__2, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("CPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPOCON */

	s_copy(srnamc_1.srnamt, "CPOCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpocon_("/", &c__0, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("CPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpocon_("U", &c_n1, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("CPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpocon_("U", &c__2, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("CPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	r__1 = -anrm;
	cpocon_("U", &c__1, a, &c__1, &r__1, &rcond, w, r__, &info)
		;
	chkxer_("CPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPOEQU */

	s_copy(srnamc_1.srnamt, "CPOEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpoequ_(&c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("CPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpoequ_(&c__2, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("CPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the Cholesky */
/*     decomposition of a Hermitian positive definite packed matrix. */

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        CPPTRF */

	s_copy(srnamc_1.srnamt, "CPPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpptrf_("/", &c__0, a, &info);
	chkxer_("CPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpptrf_("U", &c_n1, a, &info);
	chkxer_("CPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPPTRI */

	s_copy(srnamc_1.srnamt, "CPPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpptri_("/", &c__0, a, &info);
	chkxer_("CPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpptri_("U", &c_n1, a, &info);
	chkxer_("CPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPPTRS */

	s_copy(srnamc_1.srnamt, "CPPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpptrs_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("CPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpptrs_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("CPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpptrs_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("CPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cpptrs_("U", &c__2, &c__1, a, b, &c__1, &info);
	chkxer_("CPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPPRFS */

	s_copy(srnamc_1.srnamt, "CPPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpprfs_("/", &c__0, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("CPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpprfs_("U", &c_n1, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("CPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpprfs_("U", &c__0, &c_n1, a, af, b, &c__1, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("CPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cpprfs_("U", &c__2, &c__1, a, af, b, &c__1, x, &c__2, r1, r2, w, r__, 
		&info);
	chkxer_("CPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cpprfs_("U", &c__2, &c__1, a, af, b, &c__2, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("CPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPPCON */

	s_copy(srnamc_1.srnamt, "CPPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cppcon_("/", &c__0, a, &anrm, &rcond, w, r__, &info);
	chkxer_("CPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cppcon_("U", &c_n1, a, &anrm, &rcond, w, r__, &info);
	chkxer_("CPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	r__1 = -anrm;
	cppcon_("U", &c__1, a, &r__1, &rcond, w, r__, &info);
	chkxer_("CPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPPEQU */

	s_copy(srnamc_1.srnamt, "CPPEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cppequ_("/", &c__0, a, r1, &rcond, &anrm, &info);
	chkxer_("CPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cppequ_("U", &c_n1, a, r1, &rcond, &anrm, &info);
	chkxer_("CPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the Cholesky */
/*     decomposition of a Hermitian positive definite band matrix. */

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        CPBTRF */

	s_copy(srnamc_1.srnamt, "CPBTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbtrf_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("CPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbtrf_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("CPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbtrf_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("CPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cpbtrf_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("CPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPBTF2 */

	s_copy(srnamc_1.srnamt, "CPBTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbtf2_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("CPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbtf2_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("CPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbtf2_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("CPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cpbtf2_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("CPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPBTRS */

	s_copy(srnamc_1.srnamt, "CPBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbtrs_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("CPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbtrs_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("CPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbtrs_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("CPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpbtrs_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("CPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cpbtrs_("U", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("CPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cpbtrs_("U", &c__2, &c__0, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("CPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPBRFS */

	s_copy(srnamc_1.srnamt, "CPBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbrfs_("/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbrfs_("U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbrfs_("U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cpbrfs_("U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cpbrfs_("U", &c__2, &c__1, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cpbrfs_("U", &c__2, &c__1, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cpbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cpbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__2, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPBCON */

	s_copy(srnamc_1.srnamt, "CPBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbcon_("/", &c__0, &c__0, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("CPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbcon_("U", &c_n1, &c__0, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("CPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbcon_("U", &c__1, &c_n1, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("CPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cpbcon_("U", &c__2, &c__1, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("CPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	r__1 = -anrm;
	cpbcon_("U", &c__1, &c__0, a, &c__1, &r__1, &rcond, w, r__, &info);
	chkxer_("CPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPBEQU */

	s_copy(srnamc_1.srnamt, "CPBEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpbequ_("/", &c__0, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("CPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpbequ_("U", &c_n1, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("CPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpbequ_("U", &c__1, &c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("CPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cpbequ_("U", &c__2, &c__1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("CPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRPO */

} /* cerrpo_ */
