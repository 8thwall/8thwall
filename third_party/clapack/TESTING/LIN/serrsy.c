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
static integer c__4 = 4;
static real c_b152 = -1.f;

/* Subroutine */ int serrsy_(char *path, integer *nunit)
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
    real anrm, rcond;
    extern /* Subroutine */ int ssytf2_(char *, integer *, real *, integer *, 
	    integer *, integer *), alaesm_(char *, logical *, integer 
	    *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sspcon_(char *, integer *, real *, integer 
	    *, real *, real *, real *, integer *, integer *), ssycon_(
	    char *, integer *, real *, integer *, integer *, real *, real *, 
	    real *, integer *, integer *), ssprfs_(char *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    integer *, real *, real *, real *, integer *, integer *), 
	    ssptrf_(char *, integer *, real *, integer *, integer *), 
	    ssptri_(char *, integer *, real *, integer *, real *, integer *), ssyrfs_(char *, integer *, integer *, real *, integer *, 
	    real *, integer *, integer *, real *, integer *, real *, integer *
, real *, real *, real *, integer *, integer *), ssytrf_(
	    char *, integer *, real *, integer *, integer *, real *, integer *
, integer *), ssytri_(char *, integer *, real *, integer *
, integer *, real *, integer *), ssptrs_(char *, integer *
, integer *, real *, integer *, real *, integer *, integer *), ssytrs_(char *, integer *, integer *, real *, integer *, 
	    integer *, real *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRSY tests the error exits for the REAL routines */
/*  for symmetric indefinite matrices. */

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
    anrm = 1.f;
    rcond = 1.f;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "SY")) {

/*        Test error exits of the routines that use the Bunch-Kaufman */
/*        factorization of a symmetric indefinite matrix. */

/*        SSYTRF */

	s_copy(srnamc_1.srnamt, "SSYTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssytrf_("/", &c__0, a, &c__1, ip, w, &c__1, &info);
	chkxer_("SSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssytrf_("U", &c_n1, a, &c__1, ip, w, &c__1, &info);
	chkxer_("SSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssytrf_("U", &c__2, a, &c__1, ip, w, &c__4, &info);
	chkxer_("SSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSYTF2 */

	s_copy(srnamc_1.srnamt, "SSYTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssytf2_("/", &c__0, a, &c__1, ip, &info);
	chkxer_("SSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssytf2_("U", &c_n1, a, &c__1, ip, &info);
	chkxer_("SSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssytf2_("U", &c__2, a, &c__1, ip, &info);
	chkxer_("SSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSYTRI */

	s_copy(srnamc_1.srnamt, "SSYTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssytri_("/", &c__0, a, &c__1, ip, w, &info);
	chkxer_("SSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssytri_("U", &c_n1, a, &c__1, ip, w, &info);
	chkxer_("SSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssytri_("U", &c__2, a, &c__1, ip, w, &info);
	chkxer_("SSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSYTRS */

	s_copy(srnamc_1.srnamt, "SSYTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssytrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssytrs_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssytrs_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("SSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ssytrs_("U", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("SSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssytrs_("U", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("SSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSYRFS */

	s_copy(srnamc_1.srnamt, "SSYRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssyrfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssyrfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssyrfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	ssyrfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ssyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("SSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	ssyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("SSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSYCON */

	s_copy(srnamc_1.srnamt, "SSYCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssycon_("/", &c__0, a, &c__1, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("SSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssycon_("U", &c_n1, a, &c__1, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("SSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	ssycon_("U", &c__2, a, &c__1, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("SSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	ssycon_("U", &c__1, a, &c__1, ip, &c_b152, &rcond, w, iw, &info);
	chkxer_("SSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        Test error exits of the routines that use the Bunch-Kaufman */
/*        factorization of a symmetric indefinite packed matrix. */

/*        SSPTRF */

	s_copy(srnamc_1.srnamt, "SSPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssptrf_("/", &c__0, a, ip, &info);
	chkxer_("SSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssptrf_("U", &c_n1, a, ip, &info);
	chkxer_("SSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSPTRI */

	s_copy(srnamc_1.srnamt, "SSPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssptri_("/", &c__0, a, ip, w, &info);
	chkxer_("SSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssptri_("U", &c_n1, a, ip, w, &info);
	chkxer_("SSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSPTRS */

	s_copy(srnamc_1.srnamt, "SSPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssptrs_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("SSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssptrs_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("SSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssptrs_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("SSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	ssptrs_("U", &c__2, &c__1, a, ip, b, &c__1, &info);
	chkxer_("SSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSPRFS */

	s_copy(srnamc_1.srnamt, "SSPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	ssprfs_("/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("SSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	ssprfs_("U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("SSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	ssprfs_("U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("SSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	ssprfs_("U", &c__2, &c__1, a, af, ip, b, &c__1, x, &c__2, r1, r2, w, 
		iw, &info);
	chkxer_("SSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	ssprfs_("U", &c__2, &c__1, a, af, ip, b, &c__2, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("SSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SSPCON */

	s_copy(srnamc_1.srnamt, "SSPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sspcon_("/", &c__0, a, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("SSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sspcon_("U", &c_n1, a, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("SSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sspcon_("U", &c__1, a, ip, &c_b152, &rcond, w, iw, &info);
	chkxer_("SSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRSY */

} /* serrsy_ */
