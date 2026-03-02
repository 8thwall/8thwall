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

/* Subroutine */ int cerrsy_(char *path, integer *nunit)
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
    extern /* Subroutine */ int csytf2_(char *, integer *, complex *, integer 
	    *, integer *, integer *), alaesm_(char *, logical *, 
	    integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), cspcon_(char *, integer *, complex *, 
	    integer *, real *, real *, complex *, integer *), csycon_(
	    char *, integer *, complex *, integer *, integer *, real *, real *
, complex *, integer *), csprfs_(char *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *, complex *, real *, integer *
), csptrf_(char *, integer *, complex *, integer *, 
	    integer *), csptri_(char *, integer *, complex *, integer 
	    *, complex *, integer *), csyrfs_(char *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, 
	    complex *, real *, integer *), csytrf_(char *, integer *, 
	    complex *, integer *, integer *, complex *, integer *, integer *), csytri_(char *, integer *, complex *, integer *, integer 
	    *, complex *, integer *), csptrs_(char *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, integer *), csytrs_(char *, integer *, integer *, complex *, integer 
	    *, integer *, complex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRSY tests the error exits for the COMPLEX routines */
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
/*     factorization of a symmetric indefinite matrix. */

    if (lsamen_(&c__2, c2, "SY")) {

/*        CSYTRF */

	s_copy(srnamc_1.srnamt, "CSYTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csytrf_("/", &c__0, a, &c__1, ip, w, &c__1, &info);
	chkxer_("CSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csytrf_("U", &c_n1, a, &c__1, ip, w, &c__1, &info);
	chkxer_("CSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	csytrf_("U", &c__2, a, &c__1, ip, w, &c__4, &info);
	chkxer_("CSYTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSYTF2 */

	s_copy(srnamc_1.srnamt, "CSYTF2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csytf2_("/", &c__0, a, &c__1, ip, &info);
	chkxer_("CSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csytf2_("U", &c_n1, a, &c__1, ip, &info);
	chkxer_("CSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	csytf2_("U", &c__2, a, &c__1, ip, &info);
	chkxer_("CSYTF2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSYTRI */

	s_copy(srnamc_1.srnamt, "CSYTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csytri_("/", &c__0, a, &c__1, ip, w, &info);
	chkxer_("CSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csytri_("U", &c_n1, a, &c__1, ip, w, &info);
	chkxer_("CSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	csytri_("U", &c__2, a, &c__1, ip, w, &info);
	chkxer_("CSYTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSYTRS */

	s_copy(srnamc_1.srnamt, "CSYTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csytrs_("/", &c__0, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csytrs_("U", &c_n1, &c__0, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	csytrs_("U", &c__0, &c_n1, a, &c__1, ip, b, &c__1, &info);
	chkxer_("CSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	csytrs_("U", &c__2, &c__1, a, &c__1, ip, b, &c__2, &info);
	chkxer_("CSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	csytrs_("U", &c__2, &c__1, a, &c__2, ip, b, &c__1, &info);
	chkxer_("CSYTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSYRFS */

	s_copy(srnamc_1.srnamt, "CSYRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csyrfs_("/", &c__0, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csyrfs_("U", &c_n1, &c__0, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	csyrfs_("U", &c__0, &c_n1, a, &c__1, af, &c__1, ip, b, &c__1, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	csyrfs_("U", &c__2, &c__1, a, &c__1, af, &c__2, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	csyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__1, ip, b, &c__2, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	csyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__1, x, &
		c__2, r1, r2, w, r__, &info);
	chkxer_("CSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	csyrfs_("U", &c__2, &c__1, a, &c__2, af, &c__2, ip, b, &c__2, x, &
		c__1, r1, r2, w, r__, &info);
	chkxer_("CSYRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSYCON */

	s_copy(srnamc_1.srnamt, "CSYCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csycon_("/", &c__0, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("CSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csycon_("U", &c_n1, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("CSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	csycon_("U", &c__2, a, &c__1, ip, &anrm, &rcond, w, &info);
	chkxer_("CSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	r__1 = -anrm;
	csycon_("U", &c__1, a, &c__1, ip, &r__1, &rcond, w, &info);
	chkxer_("CSYCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*     Test error exits of the routines that use the diagonal pivoting */
/*     factorization of a symmetric indefinite packed matrix. */

    } else if (lsamen_(&c__2, c2, "SP")) {

/*        CSPTRF */

	s_copy(srnamc_1.srnamt, "CSPTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csptrf_("/", &c__0, a, ip, &info);
	chkxer_("CSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csptrf_("U", &c_n1, a, ip, &info);
	chkxer_("CSPTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSPTRI */

	s_copy(srnamc_1.srnamt, "CSPTRI", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csptri_("/", &c__0, a, ip, w, &info);
	chkxer_("CSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csptri_("U", &c_n1, a, ip, w, &info);
	chkxer_("CSPTRI", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSPTRS */

	s_copy(srnamc_1.srnamt, "CSPTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csptrs_("/", &c__0, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csptrs_("U", &c_n1, &c__0, a, ip, b, &c__1, &info);
	chkxer_("CSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	csptrs_("U", &c__0, &c_n1, a, ip, b, &c__1, &info);
	chkxer_("CSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	csptrs_("U", &c__2, &c__1, a, ip, b, &c__1, &info);
	chkxer_("CSPTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSPRFS */

	s_copy(srnamc_1.srnamt, "CSPRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	csprfs_("/", &c__0, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	csprfs_("U", &c_n1, &c__0, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	csprfs_("U", &c__0, &c_n1, a, af, ip, b, &c__1, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	csprfs_("U", &c__2, &c__1, a, af, ip, b, &c__1, x, &c__2, r1, r2, w, 
		r__, &info);
	chkxer_("CSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	csprfs_("U", &c__2, &c__1, a, af, ip, b, &c__2, x, &c__1, r1, r2, w, 
		r__, &info);
	chkxer_("CSPRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CSPCON */

	s_copy(srnamc_1.srnamt, "CSPCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cspcon_("/", &c__0, a, ip, &anrm, &rcond, w, &info);
	chkxer_("CSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cspcon_("U", &c_n1, a, ip, &anrm, &rcond, w, &info);
	chkxer_("CSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	r__1 = -anrm;
	cspcon_("U", &c__1, a, ip, &r__1, &rcond, w, &info);
	chkxer_("CSPCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRSY */

} /* cerrsy_ */
