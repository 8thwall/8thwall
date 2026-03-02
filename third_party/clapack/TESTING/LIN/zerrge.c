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
static integer c__3 = 3;
static integer c__4 = 4;

/* Subroutine */ int zerrge_(char *path, integer *nunit)
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
    integer ip[4], info;
    doublereal anrm, ccond, rcond;
    extern /* Subroutine */ int zgbtf2_(integer *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, integer *, integer *), 
	    zgetf2_(integer *, integer *, doublecomplex *, integer *, integer 
	    *, integer *), alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int zgbcon_(char *, integer *, integer *, integer 
	    *, doublecomplex *, integer *, integer *, doublereal *, 
	    doublereal *, doublecomplex *, doublereal *, integer *), 
	    chkxer_(char *, integer *, integer *, logical *, logical *), zgecon_(char *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zgbequ_(integer *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, integer *), zgbrfs_(
	    char *, integer *, integer *, integer *, integer *, doublecomplex 
	    *, integer *, doublecomplex *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zgbtrf_(integer *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, integer *, integer *), 
	    zgeequ_(integer *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *), zgerfs_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zgetrf_(integer *, integer *, doublecomplex *, 
	     integer *, integer *, integer *), zgetri_(integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, integer *, 
	     integer *), zgbtrs_(char *, integer *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, integer *, doublecomplex *, 
	     integer *, integer *), zgetrs_(char *, integer *, 
	    integer *, doublecomplex *, integer *, integer *, doublecomplex *, 
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

/*  ZERRGE tests the error exits for the COMPLEX*16 routines */
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
	ip[j - 1] = j;
/* L20: */
    }
    infoc_1.ok = TRUE_;

/*     Test error exits of the routines that use the LU decomposition */
/*     of a general matrix. */

    if (lsamen_(&c__2, c2, "GE")) {

/*        ZGETRF */

	s_copy(srnamc_1.srnamt, "ZGETRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgetrf_(&c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgetrf_(&c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("ZGETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgetrf_(&c__2, &c__1, a, &c__1, ip, &info);
	chkxer_("ZGETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGETF2 */

	s_copy(srnamc_1.srnamt, "ZGETF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgetf2_(&c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgetf2_(&c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("ZGETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgetf2_(&c__2, &c__1, a, &c__1, ip, &info);
	chkxer_("ZGETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGETRI */

	s_copy(srnamc_1.srnamt, "ZGETRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgetri_(&c_n1, a, &c__1, ip, w, &c__1, &info);
	chkxer_("ZGETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgetri_(&c__2, a, &c__1, ip, w, &c__2, &info);
	chkxer_("ZGETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgetri_(&c__2, a, &c__2, ip, w, &c__1, &info);
	chkxer_("ZGETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGETRS */

	s_copy(srnamc_1.srnamt, "ZGETRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgetrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgetrs_("N", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgetrs_("N", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgetrs_("N", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("ZGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zgetrs_("N", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("ZGETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGERFS */

	s_copy(srnamc_1.srnamt, "ZGERFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgerfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgerfs_("N", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgerfs_("N", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgerfs_("N", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgerfs_("N", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgerfs_("N", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgerfs_("N", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZGERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGECON */

	s_copy(srnamc_1.srnamt, "ZGECON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgecon_("/", &c__0, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("ZGECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgecon_("1", &c_n1, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("ZGECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgecon_("1", &c__2, a, &c__1, &anrm, &rcond, w, r__, &info)
		;
	chkxer_("ZGECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGEEQU */

	s_copy(srnamc_1.srnamt, "ZGEEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgeequ_(&c_n1, &c__0, a, &c__1, r1, r2, &rcond, &ccond, &anrm, &info);
	chkxer_("ZGEEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgeequ_(&c__0, &c_n1, a, &c__1, r1, r2, &rcond, &ccond, &anrm, &info);
	chkxer_("ZGEEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgeequ_(&c__2, &c__2, a, &c__1, r1, r2, &rcond, &ccond, &anrm, &info);
	chkxer_("ZGEEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the LU decomposition */
/*     of a general band matrix. */

    } else if (lsamen_(&c__2, c2, "GB")) {

/*        ZGBTRF */

	s_copy(srnamc_1.srnamt, "ZGBTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbtrf_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbtrf_(&c__0, &c_n1, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbtrf_(&c__1, &c__1, &c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbtrf_(&c__1, &c__1, &c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("ZGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgbtrf_(&c__2, &c__2, &c__1, &c__1, a, &c__3, ip, &info);
	chkxer_("ZGBTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGBTF2 */

	s_copy(srnamc_1.srnamt, "ZGBTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbtf2_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbtf2_(&c__0, &c_n1, &c__0, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbtf2_(&c__1, &c__1, &c_n1, &c__0, a, &c__1, ip, &info);
	chkxer_("ZGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbtf2_(&c__1, &c__1, &c__0, &c_n1, a, &c__1, ip, &info);
	chkxer_("ZGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgbtf2_(&c__2, &c__2, &c__1, &c__1, a, &c__3, ip, &info);
	chkxer_("ZGBTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGBTRS */

	s_copy(srnamc_1.srnamt, "ZGBTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbtrs_("/", &c__0, &c__0, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("ZGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbtrs_("N", &c_n1, &c__0, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("ZGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbtrs_("N", &c__1, &c_n1, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("ZGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbtrs_("N", &c__1, &c__0, &c_n1, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("ZGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgbtrs_("N", &c__1, &c__0, &c__0, &c_n1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("ZGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgbtrs_("N", &c__2, &c__1, &c__1, &c__1, a, &c__3, ip, b, &c__2, &
		info);
	chkxer_("ZGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgbtrs_("N", &c__2, &c__0, &c__0, &c__1, a, &c__1, ip, b, &c__1, &
		info);
	chkxer_("ZGBTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGBRFS */

	s_copy(srnamc_1.srnamt, "ZGBRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbrfs_("/", &c__0, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbrfs_("N", &c_n1, &c__0, &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbrfs_("N", &c__1, &c_n1, &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbrfs_("N", &c__1, &c__0, &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgbrfs_("N", &c__1, &c__0, &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__1, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgbrfs_("N", &c__2, &c__1, &c__1, &c__1, a, &c__2, af, &c__4, ip, b, &
		c__2, x, &c__2, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zgbrfs_("N", &c__2, &c__1, &c__1, &c__1, a, &c__3, af, &c__3, ip, b, &
		c__2, x, &c__2, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgbrfs_("N", &c__2, &c__0, &c__0, &c__1, a, &c__1, af, &c__1, ip, b, &
		c__1, x, &c__2, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 14;
	zgbrfs_("N", &c__2, &c__0, &c__0, &c__1, a, &c__1, af, &c__1, ip, b, &
		c__2, x, &c__1, r1, r2, w, r__, &info);
	chkxer_("ZGBRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGBCON */

	s_copy(srnamc_1.srnamt, "ZGBCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbcon_("/", &c__0, &c__0, &c__0, a, &c__1, ip, &anrm, &rcond, w, r__, 
		 &info);
	chkxer_("ZGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbcon_("1", &c_n1, &c__0, &c__0, a, &c__1, ip, &anrm, &rcond, w, r__, 
		 &info);
	chkxer_("ZGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbcon_("1", &c__1, &c_n1, &c__0, a, &c__1, ip, &anrm, &rcond, w, r__, 
		 &info);
	chkxer_("ZGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbcon_("1", &c__1, &c__0, &c_n1, a, &c__1, ip, &anrm, &rcond, w, r__, 
		 &info);
	chkxer_("ZGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgbcon_("1", &c__2, &c__1, &c__1, a, &c__3, ip, &anrm, &rcond, w, r__, 
		 &info);
	chkxer_("ZGBCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGBEQU */

	s_copy(srnamc_1.srnamt, "ZGBEQU", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgbequ_(&c_n1, &c__0, &c__0, &c__0, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("ZGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgbequ_(&c__0, &c_n1, &c__0, &c__0, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("ZGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgbequ_(&c__1, &c__1, &c_n1, &c__0, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("ZGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgbequ_(&c__1, &c__1, &c__0, &c_n1, a, &c__1, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("ZGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgbequ_(&c__2, &c__2, &c__1, &c__1, a, &c__2, r1, r2, &rcond, &ccond, 
		&anrm, &info);
	chkxer_("ZGBEQU", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRGE */

} /* zerrge_ */
