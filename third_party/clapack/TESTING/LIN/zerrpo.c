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

/* Subroutine */ int zerrpo_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;
    doublecomplex z__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublecomplex a[16]	/* was [4][4] */, b[4];
    integer i__, j;
    doublereal r__[4];
    doublecomplex w[8], x[4];
    char c2[2];
    doublereal r1[4], r2[4];
    doublecomplex af[16]	/* was [4][4] */;
    integer info;
    doublereal anrm, rcond;
    extern /* Subroutine */ int zpbtf2_(char *, integer *, integer *, 
	    doublecomplex *, integer *, integer *), zpotf2_(char *, 
	    integer *, doublecomplex *, integer *, integer *), 
	    alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zpbcon_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, doublereal *, integer *), zpbequ_(char *, 
	     integer *, integer *, doublecomplex *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *), zpbrfs_(char *, 
	    integer *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, doublereal *, integer *), zpbtrf_(char *, 
	     integer *, integer *, doublecomplex *, integer *, integer *), zpocon_(char *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zppcon_(char *, integer *, doublecomplex *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zpoequ_(integer *, doublecomplex *, integer *, 
	     doublereal *, doublereal *, doublereal *, integer *), zpbtrs_(
	    char *, integer *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, integer *, integer *), zporfs_(char *, 
	     integer *, integer *, doublecomplex *, integer *, doublecomplex *
, integer *, doublecomplex *, integer *, doublecomplex *, integer 
	    *, doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zpotrf_(char *, integer *, doublecomplex *, 
	    integer *, integer *), zpotri_(char *, integer *, 
	    doublecomplex *, integer *, integer *), zppequ_(char *, 
	    integer *, doublecomplex *, doublereal *, doublereal *, 
	    doublereal *, integer *), zpprfs_(char *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublecomplex *, doublereal *, integer *), zpptrf_(char *
, integer *, doublecomplex *, integer *), zpptri_(char *, 
	    integer *, doublecomplex *, integer *), zpotrs_(char *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, integer *), zpptrs_(char *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRPO tests the error exits for the COMPLEX*16 routines */
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
	    d__1 = 1. / (doublereal) (i__ + j);
	    d__2 = -1. / (doublereal) (i__ + j);
	    z__1.r = d__1, z__1.i = d__2;
	    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
	    i__1 = i__ + (j << 2) - 5;
	    d__1 = 1. / (doublereal) (i__ + j);
	    d__2 = -1. / (doublereal) (i__ + j);
	    z__1.r = d__1, z__1.i = d__2;
	    af[i__1].r = z__1.r, af[i__1].i = z__1.i;
/* L10: */
	}
	i__1 = j - 1;
	b[i__1].r = 0., b[i__1].i = 0.;
	r1[j - 1] = 0.;
	r2[j - 1] = 0.;
	i__1 = j - 1;
	w[i__1].r = 0., w[i__1].i = 0.;
	i__1 = j - 1;
	x[i__1].r = 0., x[i__1].i = 0.;
/* L20: */
    }
    anrm = 1.;
    infoc_1.ok = TRUE_;

/*     Test error exits of the routines that use the Cholesky */
/*     decomposition of a Hermitian positive definite matrix. */

    if (lsamen_(&c__2, c2, "PO")) {

/*        ZPOTRF */

	s_copy(srnamc_1.srnamt, "ZPOTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpotrf_("/", &c__0, a, &c__1, &info);
	chkxer_("ZPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpotrf_("U", &c_n1, a, &c__1, &info);
	chkxer_("ZPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpotrf_("U", &c__2, a, &c__1, &info);
	chkxer_("ZPOTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPOTF2 */

	s_copy(srnamc_1.srnamt, "ZPOTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpotf2_("/", &c__0, a, &c__1, &info);
	chkxer_("ZPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpotf2_("U", &c_n1, a, &c__1, &info);
	chkxer_("ZPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpotf2_("U", &c__2, a, &c__1, &info);
	chkxer_("ZPOTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPOTRI */

	s_copy(srnamc_1.srnamt, "ZPOTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpotri_("/", &c__0, a, &c__1, &info);
	chkxer_("ZPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpotri_("U", &c_n1, a, &c__1, &info);
	chkxer_("ZPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpotri_("U", &c__2, a, &c__1, &info);
	chkxer_("ZPOTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPOTRS */

	s_copy(srnamc_1.srnamt, "ZPOTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpotrs_("/", &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("ZPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpotrs_("U", &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("ZPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpotrs_("U", &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("ZPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zpotrs_("U", &c__2, &c__1, a, &c__1, b, &c__2, &info);
	chkxer_("ZPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zpotrs_("U", &c__2, &c__1, a, &c__2, b, &c__1, &info);
	chkxer_("ZPOTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPORFS */

	s_copy(srnamc_1.srnamt, "ZPORFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zporfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("ZPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zporfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("ZPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zporfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("ZPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zporfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &c__2, 
		r1, r2, w, r__, &info);
	chkxer_("ZPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zporfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &c__2, 
		r1, r2, w, r__, &info);
	chkxer_("ZPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__1, x, &c__2, 
		r1, r2, w, r__, &info);
	chkxer_("ZPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zporfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, b, &c__2, x, &c__1, 
		r1, r2, w, r__, &info);
	chkxer_("ZPORFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPOCON */

	s_copy(srnamc_1.srnamt, "ZPOCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpocon_("/", &c__0, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("ZPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpocon_("U", &c_n1, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("ZPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpocon_("U", &c__2, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("ZPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	d__1 = -anrm;
	zpocon_("U", &c__1, a, &c__1, &d__1, &rcond, w, r__, &info)
		;
	chkxer_("ZPOCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPOEQU */

	s_copy(srnamc_1.srnamt, "ZPOEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpoequ_(&c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("ZPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpoequ_(&c__2, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("ZPOEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the Cholesky */
/*     decomposition of a Hermitian positive definite packed matrix. */

    } else if (lsamen_(&c__2, c2, "PP")) {

/*        ZPPTRF */

	s_copy(srnamc_1.srnamt, "ZPPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpptrf_("/", &c__0, a, &info);
	chkxer_("ZPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpptrf_("U", &c_n1, a, &info);
	chkxer_("ZPPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPPTRI */

	s_copy(srnamc_1.srnamt, "ZPPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpptri_("/", &c__0, a, &info);
	chkxer_("ZPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpptri_("U", &c_n1, a, &info);
	chkxer_("ZPPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPPTRS */

	s_copy(srnamc_1.srnamt, "ZPPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpptrs_("/", &c__0, &c__0, a, b, &c__1, &info);
	chkxer_("ZPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpptrs_("U", &c_n1, &c__0, a, b, &c__1, &info);
	chkxer_("ZPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpptrs_("U", &c__0, &c_n1, a, b, &c__1, &info);
	chkxer_("ZPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zpptrs_("U", &c__2, &c__1, a, b, &c__1, &info);
	chkxer_("ZPPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPPRFS */

	s_copy(srnamc_1.srnamt, "ZPPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpprfs_("/", &c__0, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("ZPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpprfs_("U", &c_n1, &c__0, a, af, b, &c__1, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("ZPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpprfs_("U", &c__0, &c_n1, a, af, b, &c__1, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("ZPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zpprfs_("U", &c__2, &c__1, a, af, b, &c__1, x, &c__2, r1, r2, w, r__, 
		&info);
	chkxer_("ZPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zpprfs_("U", &c__2, &c__1, a, af, b, &c__2, x, &c__1, r1, r2, w, r__, 
		&info);
	chkxer_("ZPPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPPCON */

	s_copy(srnamc_1.srnamt, "ZPPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zppcon_("/", &c__0, a, &anrm, &rcond, w, r__, &info);
	chkxer_("ZPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zppcon_("U", &c_n1, a, &anrm, &rcond, w, r__, &info);
	chkxer_("ZPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	d__1 = -anrm;
	zppcon_("U", &c__1, a, &d__1, &rcond, w, r__, &info);
	chkxer_("ZPPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPPEQU */

	s_copy(srnamc_1.srnamt, "ZPPEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zppequ_("/", &c__0, a, r1, &rcond, &anrm, &info);
	chkxer_("ZPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zppequ_("U", &c_n1, a, r1, &rcond, &anrm, &info);
	chkxer_("ZPPEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the Cholesky */
/*     decomposition of a Hermitian positive definite band matrix. */

    } else if (lsamen_(&c__2, c2, "PB")) {

/*        ZPBTRF */

	s_copy(srnamc_1.srnamt, "ZPBTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbtrf_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("ZPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbtrf_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("ZPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbtrf_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("ZPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zpbtrf_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("ZPBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPBTF2 */

	s_copy(srnamc_1.srnamt, "ZPBTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbtf2_("/", &c__0, &c__0, a, &c__1, &info);
	chkxer_("ZPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbtf2_("U", &c_n1, &c__0, a, &c__1, &info);
	chkxer_("ZPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbtf2_("U", &c__1, &c_n1, a, &c__1, &info);
	chkxer_("ZPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zpbtf2_("U", &c__2, &c__1, a, &c__1, &info);
	chkxer_("ZPBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPBTRS */

	s_copy(srnamc_1.srnamt, "ZPBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbtrs_("/", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("ZPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbtrs_("U", &c_n1, &c__0, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("ZPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbtrs_("U", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, &info);
	chkxer_("ZPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpbtrs_("U", &c__0, &c__0, &c_n1, a, &c__1, b, &c__1, &info);
	chkxer_("ZPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zpbtrs_("U", &c__2, &c__1, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("ZPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zpbtrs_("U", &c__2, &c__0, &c__1, a, &c__1, b, &c__1, &info);
	chkxer_("ZPBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPBRFS */

	s_copy(srnamc_1.srnamt, "ZPBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbrfs_("/", &c__0, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbrfs_("U", &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbrfs_("U", &c__1, &c_n1, &c__0, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zpbrfs_("U", &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zpbrfs_("U", &c__2, &c__1, &c__1, a, &c__1, af, &c__2, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zpbrfs_("U", &c__2, &c__1, &c__1, a, &c__2, af, &c__1, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zpbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__1, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zpbrfs_("U", &c__2, &c__0, &c__1, a, &c__1, af, &c__1, b, &c__2, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZPBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPBCON */

	s_copy(srnamc_1.srnamt, "ZPBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbcon_("/", &c__0, &c__0, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("ZPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbcon_("U", &c_n1, &c__0, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("ZPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbcon_("U", &c__1, &c_n1, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("ZPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zpbcon_("U", &c__2, &c__1, a, &c__1, &anrm, &rcond, w, r__, &info);
	chkxer_("ZPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	d__1 = -anrm;
	zpbcon_("U", &c__1, &c__0, a, &c__1, &d__1, &rcond, w, r__, &info);
	chkxer_("ZPBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZPBEQU */

	s_copy(srnamc_1.srnamt, "ZPBEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zpbequ_("/", &c__0, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("ZPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zpbequ_("U", &c_n1, &c__0, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("ZPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zpbequ_("U", &c__1, &c_n1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("ZPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zpbequ_("U", &c__2, &c__1, a, &c__1, r1, &rcond, &anrm, &info);
	chkxer_("ZPBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRPO */

} /* zerrpo_ */
