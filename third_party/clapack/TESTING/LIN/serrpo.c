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

/* Subroutine */ int serrpo_(char *path, integer *nunit)
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
    integer iw[4], info;
    real anrm, rcond;
    extern /* Subroutine */ int spbtf2_(char *, integer *, integer *, real *, 
	    integer *, integer *), spotf2_(char *, integer *, real *, 
	    integer *, integer *), alaesm_(char *, logical *, integer 
	    *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), spbcon_(char *, integer *, integer *, real 
	    *, integer *, real *, real *, real *, integer *, integer *), spbequ_(char *, integer *, integer *, real *, integer *, 
	    real *, real *, real *, integer *), spbrfs_(char *, 
	    integer *, integer *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *, real *, real *, 
	    real *, integer *, integer *), spbtrf_(char *, integer *, 
	    integer *, real *, integer *, integer *), spocon_(char *, 
	    integer *, real *, integer *, real *, real *, real *, integer *, 
	    integer *), sppcon_(char *, integer *, real *, real *, 
	    real *, real *, integer *, integer *), spoequ_(integer *, 
	    real *, integer *, real *, real *, real *, integer *), spbtrs_(
	    char *, integer *, integer *, integer *, real *, integer *, real *
, integer *, integer *), sporfs_(char *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, real *, integer *, real *, real *, real *, integer *, integer *), spotrf_(char *, integer *, real *, integer *, integer *), spotri_(char *, integer *, real *, integer *, integer *), sppequ_(char *, integer *, real *, real *, real *, real 
	    *, integer *), spprfs_(char *, integer *, integer *, real 
	    *, real *, real *, integer *, real *, integer *, real *, real *, 
	    real *, integer *, integer *), spptrf_(char *, integer *, 
	    real *, integer *), spptri_(char *, integer *, real *, 
	    integer *), spotrs_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *, integer *), spptrs_(char *, 
	    integer *, integer *, real *, real *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRPO tests the error exits for the REAL routines */
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
	    a[i__ + (j << 2) - 5] = 1.f / (real) (i__ + j);
	    af[i__ + (j << 2) - 5] = 1.f / (real) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.f;
	r1[j - 1] = 0.f;
	r2[j - 1] = 0.f;
	w[j - 1] = 0.f;
	x[j - 1] = 0.f;
	iw[j - 1] = j;
/* L20: */
    }
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "PO")) {

/*        Test error exits of the routines that use the Cholesky */
/*        decomposition of a symmetric positive definite matrix. */

/*        SPOTRF */

	s_copy(srnamc_1.srnamt, "SPOTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spotrf_("/", &c__0, a, &c__1, &info);
	chkxer_("SPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spotrf_("U", &c_n1, a, &c__1, &info);
	chkxer_("SPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spotrf_("U", &c__2, a, &c__1, &info);
	chkxer_("SPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPOTF2 */

	s_copy(srnamc_1.srnamt, "SPOTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spotf2_("/", &c__0, a, &c__1, &info);
	chkxer_("SPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spotf2_("U", &c_n1, a, &c__1, &info);
	chkxer_("SPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spotf2_("U", &c__2, a, &c__1, &info);
	chkxer_("SPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPOTRI */

	s_copy(srnamc_1.srnamt, "SPOTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spotri_("/", &c__0, a, &c__1, &info);
	chkxer_("SPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spotri_("U", &c_n1, a, &c__1, &info);
	chkxer_("SPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spotri_("U", &c__2, a, &c__1, &info);
	chkxer_("SPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPOTRS */

	s_copy(srnamc_1.srnamt, "SPOTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spotrs_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("SPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spotrs_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("SPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spotrs_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("SPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	spotrs_("U", &c__2, &c__1, a, &c__1, b, &c__2, &info);
	chkxer_("SPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	spotrs_("U", &c__2, &c__1, a, &c__2, b, &c__1, &info);
	chkxer_("SPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPORFS */

	s_copy(srnamc_1.srnamt, "SPORFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sporfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("SPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sporfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("SPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sporfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("SPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sporfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &c__2, 
		r1, r2, w, iw, &info);
	chkxer_("SPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sporfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &c__2, 
		r1, r2, w, iw, &info);
	chkxer_("SPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__1, x, &c__2, 
		r1, r2, w, iw, &info);
	chkxer_("SPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__2, x, &c__1, 
		r1, r2, w, iw, &info);
	chkxer_("SPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPOCON */

	s_copy(srnamc_1.srnamt, "SPOCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spocon_("/", &c__0, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spocon_("U", &c_n1, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spocon_("U", &c__2, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPOEQU */

	s_copy(srnamc_1.srnamt, "SPOEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spoequ_(&c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("SPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spoequ_(&c__2, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("SPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        Test error exits of the routines that use the Cholesky */
/*        decomposition of a symmetric positive definite packed matrix. */

/*        SPPTRF */

	s_copy(srnamc_1.srnamt, "SPPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spptrf_("/", &c__0, a, &info);
	chkxer_("SPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spptrf_("U", &c_n1, a, &info);
	chkxer_("SPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPPTRI */

	s_copy(srnamc_1.srnamt, "SPPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spptri_("/", &c__0, a, &info);
	chkxer_("SPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spptri_("U", &c_n1, a, &info);
	chkxer_("SPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPPTRS */

	s_copy(srnamc_1.srnamt, "SPPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spptrs_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("SPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spptrs_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("SPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spptrs_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("SPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	spptrs_("U", &c__2, &c__1, a, b, &c__1, &info);
	chkxer_("SPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPPRFS */

	s_copy(srnamc_1.srnamt, "SPPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spprfs_("/", &c__0, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("SPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spprfs_("U", &c_n1, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("SPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spprfs_("U", &c__0, &c_n1, a, af, b, &c__1, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("SPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	spprfs_("U", &c__2, &c__1, a, af, b, &c__1, x, &c__2, r1, r2, w, iw, &
		info);
	chkxer_("SPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	spprfs_("U", &c__2, &c__1, a, af, b, &c__2, x, &c__1, r1, r2, w, iw, &
		info);
	chkxer_("SPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPPCON */

	s_copy(srnamc_1.srnamt, "SPPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sppcon_("/", &c__0, a, &anrm, &rcond, w, iw, &info);
	chkxer_("SPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sppcon_("U", &c_n1, a, &anrm, &rcond, w, iw, &info);
	chkxer_("SPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPPEQU */

	s_copy(srnamc_1.srnamt, "SPPEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sppequ_("/", &c__0, a, r1, &rcond, &anrm, &info);
	chkxer_("SPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sppequ_("U", &c_n1, a, r1, &rcond, &anrm, &info);
	chkxer_("SPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        Test error exits of the routines that use the Cholesky */
/*        decomposition of a symmetric positive definite band matrix. */

/*        SPBTRF */

	s_copy(srnamc_1.srnamt, "SPBTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbtrf_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("SPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbtrf_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("SPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbtrf_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("SPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	spbtrf_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("SPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPBTF2 */

	s_copy(srnamc_1.srnamt, "SPBTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbtf2_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("SPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbtf2_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("SPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbtf2_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("SPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	spbtf2_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("SPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPBTRS */

	s_copy(srnamc_1.srnamt, "SPBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbtrs_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("SPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbtrs_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("SPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbtrs_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("SPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spbtrs_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("SPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	spbtrs_("U", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("SPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	spbtrs_("U", &c__2, &c__0, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("SPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPBRFS */

	s_copy(srnamc_1.srnamt, "SPBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbrfs_("/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbrfs_("U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbrfs_("U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	spbrfs_("U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	spbrfs_("U", &c__2, &c__1, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	spbrfs_("U", &c__2, &c__1, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	spbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	spbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__2, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPBCON */

	s_copy(srnamc_1.srnamt, "SPBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbcon_("/", &c__0, &c__0, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbcon_("U", &c_n1, &c__0, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbcon_("U", &c__1, &c_n1, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	spbcon_("U", &c__2, &c__1, a, &c__1, &anrm, &rcond, w, iw, &info);
	chkxer_("SPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SPBEQU */

	s_copy(srnamc_1.srnamt, "SPBEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	spbequ_("/", &c__0, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("SPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	spbequ_("U", &c_n1, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("SPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	spbequ_("U", &c__1, &c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("SPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	spbequ_("U", &c__2, &c__1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("SPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRPO */

} /* serrpo_ */
