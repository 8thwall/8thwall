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

/* Subroutine */ int cerrhe_(char *path, integer *nunit)
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
    integer ip[4], info;
    real anrm, rcond;
    extern /* Subroutine */ int chetf2_(char *, integer *, complex *, integer 
	    *, integer *, integer *), checon_(char *, integer *, 
	    complex *, integer *, integer *, real *, real *, complex *, 
	    integer *), alaesm_(char *, logical *, integer *),
	     cherfs_(char *, integer *, integer *, complex *, integer *, 
	    complex *, integer *, integer *, complex *, integer *, complex *, 
	    integer *, real *, real *, complex *, real *, integer *), 
	    chetrf_(char *, integer *, complex *, integer *, integer *, 
	    complex *, integer *, integer *), chpcon_(char *, integer 
	    *, complex *, integer *, real *, real *, complex *, integer *), chetri_(char *, integer *, complex *, integer *, integer 
	    *, complex *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), chprfs_(char *, integer *, integer *, 
	    complex *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, real *, complex *, real *, integer *), 
	    chptrf_(char *, integer *, complex *, integer *, integer *), chetrs_(char *, integer *, integer *, complex *, integer 
	    *, integer *, complex *, integer *, integer *), chptri_(
	    char *, integer *, complex *, integer *, complex *, integer *), chptrs_(char *, integer *, integer *, complex *, integer 
	    *, complex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRHE tests the error exits for the COMPLEX routines */
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
	ip[j - 1] = j;
/* L20: */
    }
    anrm = 1.f;
    infoc_1.ok = TRUE_;

/*     Test error exits of the routines that use the diagonal pivoting */
/*     factorization of a Hermitian indefinite matrix. */

    if (lsamen_(&c__2, c2, "HE")) {

/*        CHETRF */

	s_copy(srnamc_1.srnamt, "CHETRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chetrf_("/", &c__0, a, &c__1, ip, w, &c__1, &info);
	chkxer_("CHETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chetrf_("U", &c_n1, a, &c__1, ip, w, &c__1, &info);
	chkxer_("CHETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chetrf_("U", &c__2, a, &c__1, ip, w, &c__4, &info);
	chkxer_("CHETRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHETF2 */

	s_copy(srnamc_1.srnamt, "CHETF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chetf2_("/", &c__0, a, &c__1, ip, &info);
	chkxer_("CHETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chetf2_("U", &c_n1, a, &c__1, ip, &info);
	chkxer_("CHETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chetf2_("U", &c__2, a, &c__1, ip, &info);
	chkxer_("CHETF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHETRI */

	s_copy(srnamc_1.srnamt, "CHETRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chetri_("/", &c__0, a, &c__1, ip, w, &info);
	chkxer_("CHETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chetri_("U", &c_n1, a, &c__1, ip, w, &info);
	chkxer_("CHETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	chetri_("U", &c__2, a, &c__1, ip, w, &info);
	chkxer_("CHETRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHETRS */

	s_copy(srnamc_1.srnamt, "CHETRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chetrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chetrs_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chetrs_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	chetrs_("U", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("CHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	chetrs_("U", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("CHETRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHERFS */

	s_copy(srnamc_1.srnamt, "CHERFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cherfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cherfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cherfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cherfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cherfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cherfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cherfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CHERFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHECON */

	s_copy(srnamc_1.srnamt, "CHECON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	checon_("/", &c__0, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("CHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	checon_("U", &c_n1, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("CHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	checon_("U", &c__2, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("CHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	r__1 = -anrm;
	checon_("U", &c__1, a, &c__1, ip, &r__1, &rcond, w, &info);
	chkxer_("CHECON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the diagonal pivoting */
/*     factorization of a Hermitian indefinite packed matrix. */

    } else if (lsamen_(&c__2, c2, "HP")) {

/*        CHPTRF */

	s_copy(srnamc_1.srnamt, "CHPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chptrf_("/", &c__0, a, ip, &info);
	chkxer_("CHPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chptrf_("U", &c_n1, a, ip, &info);
	chkxer_("CHPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHPTRI */

	s_copy(srnamc_1.srnamt, "CHPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chptri_("/", &c__0, a, ip, w, &info);
	chkxer_("CHPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chptri_("U", &c_n1, a, ip, w, &info);
	chkxer_("CHPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHPTRS */

	s_copy(srnamc_1.srnamt, "CHPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chptrs_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chptrs_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chptrs_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("CHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	chptrs_("U", &c__2, &c__1, a, ip, b, &c__1, &info);
	chkxer_("CHPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHPRFS */

	s_copy(srnamc_1.srnamt, "CHPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chprfs_("/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chprfs_("U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	chprfs_("U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	chprfs_("U", &c__2, &c__1, a, af, ip, b, &c__1, x, &c__2, r1, r2, w, 
		r__, &info);
	chkxer_("CHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	chprfs_("U", &c__2, &c__1, a, af, ip, b, &c__2, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CHPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CHPCON */

	s_copy(srnamc_1.srnamt, "CHPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	chpcon_("/", &c__0, a, ip, &anrm, &rcond, w, &info);
	chkxer_("CHPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	chpcon_("U", &c_n1, a, ip, &anrm, &rcond, w, &info);
	chkxer_("CHPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	r__1 = -anrm;
	chpcon_("U", &c__1, a, ip, &r__1, &rcond, w, &info);
	chkxer_("CHPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRHE */

} /* cerrhe_ */
