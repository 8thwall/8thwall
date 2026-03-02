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
static doublereal c_b152 = -1.;

/* Subroutine */ int derrsy_(char *path, integer *nunit)
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
    integer ip[4], iw[4], info;
    doublereal anrm, rcond;
    extern /* Subroutine */ int dsytf2_(char *, integer *, doublereal *, 
	    integer *, integer *, integer *), alaesm_(char *, logical 
	    *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), dspcon_(char *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    integer *), dsycon_(char *, integer *, doublereal *, 
	    integer *, integer *, doublereal *, doublereal *, doublereal *, 
	    integer *, integer *), dsprfs_(char *, integer *, integer 
	    *, doublereal *, doublereal *, integer *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *, doublereal *, doublereal *
, integer *, integer *), dsptrf_(char *, integer *, 
	    doublereal *, integer *, integer *), dsptri_(char *, 
	    integer *, doublereal *, integer *, doublereal *, integer *), dsyrfs_(char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *), dsytrf_(char *, 
	    integer *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, integer *), dsytri_(char *, integer *, 
	    doublereal *, integer *, integer *, doublereal *, integer *), dsptrs_(char *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *), dsytrs_(
	    char *, integer *, integer *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERRSY tests the error exits for the DOUBLE PRECISION routines */
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
	    a[i__ + (j << 2) - 5] = 1. / (doublereal) (i__ + j);
	    af[i__ + (j << 2) - 5] = 1. / (doublereal) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.;
	r1[j - 1] = 0.;
	r2[j - 1] = 0.;
	w[j - 1] = 0.;
	x[j - 1] = 0.;
	ip[j - 1] = j;
	iw[j - 1] = j;
/* L20: */
    }
    anrm = 1.;
    rcond = 1.;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "SY")) {

/*        Test error exits of the routines that use the Bunch-Kaufman */
/*        factorization of a symmetric indefinite matrix. */

/*        DSYTRF */

	s_copy(srnamc_1.srnamt, "DSYTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsytrf_("/", &c__0, a, &c__1, ip, w, &c__1, &info);
	chkxer_("DSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsytrf_("U", &c_n1, a, &c__1, ip, w, &c__1, &info);
	chkxer_("DSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsytrf_("U", &c__2, a, &c__1, ip, w, &c__4, &info);
	chkxer_("DSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSYTF2 */

	s_copy(srnamc_1.srnamt, "DSYTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsytf2_("/", &c__0, a, &c__1, ip, &info);
	chkxer_("DSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsytf2_("U", &c_n1, a, &c__1, ip, &info);
	chkxer_("DSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsytf2_("U", &c__2, a, &c__1, ip, &info);
	chkxer_("DSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSYTRI */

	s_copy(srnamc_1.srnamt, "DSYTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsytri_("/", &c__0, a, &c__1, ip, w, &info);
	chkxer_("DSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsytri_("U", &c_n1, a, &c__1, ip, w, &info);
	chkxer_("DSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsytri_("U", &c__2, a, &c__1, ip, w, &info);
	chkxer_("DSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSYTRS */

	s_copy(srnamc_1.srnamt, "DSYTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsytrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsytrs_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsytrs_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("DSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dsytrs_("U", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("DSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsytrs_("U", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("DSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSYRFS */

	s_copy(srnamc_1.srnamt, "DSYRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsyrfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsyrfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsyrfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dsyrfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dsyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, iw, &info);
	chkxer_("DSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dsyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, iw, &info);
	chkxer_("DSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSYCON */

	s_copy(srnamc_1.srnamt, "DSYCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsycon_("/", &c__0, a, &c__1, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("DSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsycon_("U", &c_n1, a, &c__1, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("DSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dsycon_("U", &c__2, a, &c__1, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("DSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dsycon_("U", &c__1, a, &c__1, ip, &c_b152, &rcond, w, iw, &info);
	chkxer_("DSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        Test error exits of the routines that use the Bunch-Kaufman */
/*        factorization of a symmetric indefinite packed matrix. */

/*        DSPTRF */

	s_copy(srnamc_1.srnamt, "DSPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsptrf_("/", &c__0, a, ip, &info);
	chkxer_("DSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsptrf_("U", &c_n1, a, ip, &info);
	chkxer_("DSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSPTRI */

	s_copy(srnamc_1.srnamt, "DSPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsptri_("/", &c__0, a, ip, w, &info);
	chkxer_("DSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsptri_("U", &c_n1, a, ip, w, &info);
	chkxer_("DSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSPTRS */

	s_copy(srnamc_1.srnamt, "DSPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsptrs_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("DSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsptrs_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("DSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsptrs_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("DSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dsptrs_("U", &c__2, &c__1, a, ip, b, &c__1, &info);
	chkxer_("DSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSPRFS */

	s_copy(srnamc_1.srnamt, "DSPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dsprfs_("/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("DSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dsprfs_("U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("DSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dsprfs_("U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("DSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dsprfs_("U", &c__2, &c__1, a, af, ip, b, &c__1, x, &c__2, r1, r2, w, 
		iw, &info);
	chkxer_("DSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dsprfs_("U", &c__2, &c__1, a, af, ip, b, &c__2, x, &c__1, r1, r2, w, 
		iw, &info);
	chkxer_("DSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        DSPCON */

	s_copy(srnamc_1.srnamt, "DSPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dspcon_("/", &c__0, a, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("DSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dspcon_("U", &c_n1, a, ip, &anrm, &rcond, w, iw, &info);
	chkxer_("DSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dspcon_("U", &c__1, a, ip, &c_b152, &rcond, w, iw, &info);
	chkxer_("DSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of DERRSY */

} /* derrsy_ */
