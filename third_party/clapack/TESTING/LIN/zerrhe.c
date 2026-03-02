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

/* Subroutine */ int zerrhe_(char *path, integer *nunit)
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
    doublereal anrm, rcond;
    extern /* Subroutine */ int zhetf2_(char *, integer *, doublecomplex *, 
	    integer *, integer *, integer *), alaesm_(char *, logical 
	    *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zhecon_(char *, integer *, doublecomplex *, 
	     integer *, integer *, doublereal *, doublereal *, doublecomplex *
, integer *), zherfs_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zhetrf_(char *, integer *, doublecomplex *, 
	    integer *, integer *, doublecomplex *, integer *, integer *), zhpcon_(char *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, integer *), 
	    zhetri_(char *, integer *, doublecomplex *, integer *, integer *, 
	    doublecomplex *, integer *), zhprfs_(char *, integer *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zhptrf_(char *, integer *, doublecomplex *, 
	    integer *, integer *), zhetrs_(char *, integer *, integer 
	    *, doublecomplex *, integer *, integer *, doublecomplex *, 
	    integer *, integer *), zhptri_(char *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), 
	    zhptrs_(char *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRHE tests the error exits for the COMPLEX*16 routines */
/*  for Hermitian indefinite matrices. */

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
    anrm = 1.;
    infoc_1.ok = TRUE_;

/*     Test error exits of the routines that use the diagonal pivoting */
/*     factorization of a Hermitian indefinite matrix. */

    if (lsamen_(&c__2, c2, "HE")) {

/*        ZHETRF */

	s_copy(srnamc_1.srnamt, "ZHETRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhetrf_("/", &c__0, a, &c__1, ip, w, &c__1, &info);
	chkxer_("ZHETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhetrf_("U", &c_n1, a, &c__1, ip, w, &c__1, &info);
	chkxer_("ZHETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhetrf_("U", &c__2, a, &c__1, ip, w, &c__4, &info);
	chkxer_("ZHETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHETF2 */

	s_copy(srnamc_1.srnamt, "ZHETF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhetf2_("/", &c__0, a, &c__1, ip, &info);
	chkxer_("ZHETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhetf2_("U", &c_n1, a, &c__1, ip, &info);
	chkxer_("ZHETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhetf2_("U", &c__2, a, &c__1, ip, &info);
	chkxer_("ZHETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHETRI */

	s_copy(srnamc_1.srnamt, "ZHETRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhetri_("/", &c__0, a, &c__1, ip, w, &info);
	chkxer_("ZHETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhetri_("U", &c_n1, a, &c__1, ip, w, &info);
	chkxer_("ZHETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhetri_("U", &c__2, a, &c__1, ip, w, &info);
	chkxer_("ZHETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHETRS */

	s_copy(srnamc_1.srnamt, "ZHETRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhetrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhetrs_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhetrs_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zhetrs_("U", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("ZHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zhetrs_("U", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("ZHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHERFS */

	s_copy(srnamc_1.srnamt, "ZHERFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zherfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zherfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zherfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zherfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zherfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zherfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zherfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHECON */

	s_copy(srnamc_1.srnamt, "ZHECON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhecon_("/", &c__0, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("ZHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhecon_("U", &c_n1, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("ZHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zhecon_("U", &c__2, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("ZHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	d__1 = -anrm;
	zhecon_("U", &c__1, a, &c__1, ip, &d__1, &rcond, w, &info);
	chkxer_("ZHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the diagonal pivoting */
/*     factorization of a Hermitian indefinite packed matrix. */

    } else if (lsamen_(&c__2, c2, "HP")) {

/*        ZHPTRF */

	s_copy(srnamc_1.srnamt, "ZHPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhptrf_("/", &c__0, a, ip, &info);
	chkxer_("ZHPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhptrf_("U", &c_n1, a, ip, &info);
	chkxer_("ZHPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHPTRI */

	s_copy(srnamc_1.srnamt, "ZHPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhptri_("/", &c__0, a, ip, w, &info);
	chkxer_("ZHPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhptri_("U", &c_n1, a, ip, w, &info);
	chkxer_("ZHPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHPTRS */

	s_copy(srnamc_1.srnamt, "ZHPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhptrs_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhptrs_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhptrs_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("ZHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zhptrs_("U", &c__2, &c__1, a, ip, b, &c__1, &info);
	chkxer_("ZHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHPRFS */

	s_copy(srnamc_1.srnamt, "ZHPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhprfs_("/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhprfs_("U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zhprfs_("U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zhprfs_("U", &c__2, &c__1, a, af, ip, b, &c__1, x, &c__2, r1, r2, w, 
		r__, &info);
	chkxer_("ZHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zhprfs_("U", &c__2, &c__1, a, af, ip, b, &c__2, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZHPCON */

	s_copy(srnamc_1.srnamt, "ZHPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zhpcon_("/", &c__0, a, ip, &anrm, &rcond, w, &info);
	chkxer_("ZHPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zhpcon_("U", &c_n1, a, ip, &anrm, &rcond, w, &info);
	chkxer_("ZHPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	d__1 = -anrm;
	zhpcon_("U", &c__1, a, ip, &d__1, &rcond, w, &info);
	chkxer_("ZHPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRHE */

} /* zerrhe_ */
