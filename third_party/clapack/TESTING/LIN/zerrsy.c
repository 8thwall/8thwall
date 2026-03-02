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

/* Subroutine */ int zerrsy_(char *path, integer *nunit)
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
    extern /* Subroutine */ int zsytf2_(char *, integer *, doublecomplex *, 
	    integer *, integer *, integer *), alaesm_(char *, logical 
	    *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zspcon_(char *, integer *, doublecomplex *, 
	     integer *, doublereal *, doublereal *, doublecomplex *, integer *
), zsycon_(char *, integer *, doublecomplex *, integer *, 
	    integer *, doublereal *, doublereal *, doublecomplex *, integer *), zsprfs_(char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, doublereal *, integer *), zsptrf_(char *, 
	     integer *, doublecomplex *, integer *, integer *), 
	    zsptri_(char *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zsyrfs_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     integer *, doublecomplex *, integer *, doublecomplex *, integer *
, doublereal *, doublereal *, doublecomplex *, doublereal *, 
	    integer *), zsytrf_(char *, integer *, doublecomplex *, 
	    integer *, integer *, doublecomplex *, integer *, integer *), zsytri_(char *, integer *, doublecomplex *, integer *, 
	    integer *, doublecomplex *, integer *), zsptrs_(char *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *, integer *), zsytrs_(char *, integer *, 
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

/*  ZERRSY tests the error exits for the COMPLEX*16 routines */
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
/*     factorization of a symmetric indefinite matrix. */

    if (lsamen_(&c__2, c2, "SY")) {

/*        ZSYTRF */

	s_copy(srnamc_1.srnamt, "ZSYTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsytrf_("/", &c__0, a, &c__1, ip, w, &c__1, &info);
	chkxer_("ZSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsytrf_("U", &c_n1, a, &c__1, ip, w, &c__1, &info);
	chkxer_("ZSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zsytrf_("U", &c__2, a, &c__1, ip, w, &c__4, &info);
	chkxer_("ZSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSYTF2 */

	s_copy(srnamc_1.srnamt, "ZSYTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsytf2_("/", &c__0, a, &c__1, ip, &info);
	chkxer_("ZSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsytf2_("U", &c_n1, a, &c__1, ip, &info);
	chkxer_("ZSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zsytf2_("U", &c__2, a, &c__1, ip, &info);
	chkxer_("ZSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSYTRI */

	s_copy(srnamc_1.srnamt, "ZSYTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsytri_("/", &c__0, a, &c__1, ip, w, &info);
	chkxer_("ZSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsytri_("U", &c_n1, a, &c__1, ip, w, &info);
	chkxer_("ZSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zsytri_("U", &c__2, a, &c__1, ip, w, &info);
	chkxer_("ZSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSYTRS */

	s_copy(srnamc_1.srnamt, "ZSYTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsytrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsytrs_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zsytrs_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("ZSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zsytrs_("U", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("ZSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zsytrs_("U", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("ZSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSYRFS */

	s_copy(srnamc_1.srnamt, "ZSYRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsyrfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsyrfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zsyrfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zsyrfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zsyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zsyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("ZSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zsyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("ZSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSYCON */

	s_copy(srnamc_1.srnamt, "ZSYCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsycon_("/", &c__0, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("ZSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsycon_("U", &c_n1, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("ZSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zsycon_("U", &c__2, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("ZSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	d__1 = -anrm;
	zsycon_("U", &c__1, a, &c__1, ip, &d__1, &rcond, w, &info);
	chkxer_("ZSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the diagonal pivoting */
/*     factorization of a symmetric indefinite packed matrix. */

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        ZSPTRF */

	s_copy(srnamc_1.srnamt, "ZSPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsptrf_("/", &c__0, a, ip, &info);
	chkxer_("ZSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsptrf_("U", &c_n1, a, ip, &info);
	chkxer_("ZSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSPTRI */

	s_copy(srnamc_1.srnamt, "ZSPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsptri_("/", &c__0, a, ip, w, &info);
	chkxer_("ZSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsptri_("U", &c_n1, a, ip, w, &info);
	chkxer_("ZSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSPTRS */

	s_copy(srnamc_1.srnamt, "ZSPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsptrs_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsptrs_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("ZSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zsptrs_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("ZSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zsptrs_("U", &c__2, &c__1, a, ip, b, &c__1, &info);
	chkxer_("ZSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSPRFS */

	s_copy(srnamc_1.srnamt, "ZSPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zsprfs_("/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zsprfs_("U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zsprfs_("U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zsprfs_("U", &c__2, &c__1, a, af, ip, b, &c__1, x, &c__2, r1, r2, w, 
		r__, &info);
	chkxer_("ZSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zsprfs_("U", &c__2, &c__1, a, af, ip, b, &c__2, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("ZSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZSPCON */

	s_copy(srnamc_1.srnamt, "ZSPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zspcon_("/", &c__0, a, ip, &anrm, &rcond, w, &info);
	chkxer_("ZSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zspcon_("U", &c_n1, a, ip, &anrm, &rcond, w, &info);
	chkxer_("ZSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	d__1 = -anrm;
	zspcon_("U", &c__1, a, ip, &d__1, &rcond, w, &info);
	chkxer_("ZSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRSY */

} /* zerrsy_ */
