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

/* Subroutine */ int cerrgt_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    complex b[2];
    real d__[2];
    complex e[2];
    integer i__;
    complex w[2], x[2];
    char c2[2];
    real r1[2], r2[2], df[2];
    complex ef[2], dl[2];
    integer ip[2];
    complex du[2];
    real rw[2];
    complex du2[2], dlf[2], duf[2];
    integer info;
    real rcond, anorm;
    extern /* Subroutine */ int alaesm_(char *, logical *, integer *),
	     cgtcon_(char *, integer *, complex *, complex *, complex *, 
	    complex *, integer *, real *, real *, complex *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), cptcon_(integer *, real *, complex *, real 
	    *, real *, real *, integer *), cgtrfs_(char *, integer *, integer 
	    *, complex *, complex *, complex *, complex *, complex *, complex 
	    *, complex *, integer *, complex *, integer *, complex *, integer 
	    *, real *, real *, complex *, real *, integer *), cgttrf_(
	    integer *, complex *, complex *, complex *, complex *, integer *, 
	    integer *), cptrfs_(char *, integer *, integer *, real *, complex 
	    *, real *, complex *, complex *, integer *, complex *, integer *, 
	    real *, real *, complex *, real *, integer *), cpttrf_(
	    integer *, real *, complex *, integer *), cgttrs_(char *, integer 
	    *, integer *, complex *, complex *, complex *, complex *, integer 
	    *, complex *, integer *, integer *), cpttrs_(char *, 
	    integer *, integer *, real *, complex *, complex *, integer *, 
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRGT tests the error exits for the COMPLEX tridiagonal */
/*  routines. */

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
/*     .. Executable Statements .. */

    infoc_1.nout = *nunit;
    io___1.ciunit = infoc_1.nout;
    s_wsle(&io___1);
    e_wsle();
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);
    for (i__ = 1; i__ <= 2; ++i__) {
	d__[i__ - 1] = 1.f;
	i__1 = i__ - 1;
	e[i__1].r = 2.f, e[i__1].i = 0.f;
	i__1 = i__ - 1;
	dl[i__1].r = 3.f, dl[i__1].i = 0.f;
	i__1 = i__ - 1;
	du[i__1].r = 4.f, du[i__1].i = 0.f;
/* L10: */
    }
    anorm = 1.f;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "GT")) {

/*        Test error exits for the general tridiagonal routines. */

/*        CGTTRF */

	s_copy(srnamc_1.srnamt, "CGTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgttrf_(&c_n1, dl, e, du, du2, ip, &info);
	chkxer_("CGTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGTTRS */

	s_copy(srnamc_1.srnamt, "CGTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgttrs_("/", &c__0, &c__0, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("CGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgttrs_("N", &c_n1, &c__0, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("CGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgttrs_("N", &c__0, &c_n1, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("CGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgttrs_("N", &c__2, &c__1, dl, e, du, du2, ip, x, &c__1, &info);
	chkxer_("CGTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGTRFS */

	s_copy(srnamc_1.srnamt, "CGTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgtrfs_("/", &c__0, &c__0, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("CGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgtrfs_("N", &c_n1, &c__0, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("CGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgtrfs_("N", &c__0, &c_n1, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("CGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cgtrfs_("N", &c__2, &c__1, dl, e, du, dlf, ef, duf, du2, ip, b, &c__1, 
		 x, &c__2, r1, r2, w, rw, &info);
	chkxer_("CGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cgtrfs_("N", &c__2, &c__1, dl, e, du, dlf, ef, duf, du2, ip, b, &c__2, 
		 x, &c__1, r1, r2, w, rw, &info);
	chkxer_("CGTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGTCON */

	s_copy(srnamc_1.srnamt, "CGTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgtcon_("/", &c__0, dl, e, du, du2, ip, &anorm, &rcond, w, &info);
	chkxer_("CGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgtcon_("I", &c_n1, dl, e, du, du2, ip, &anorm, &rcond, w, &info);
	chkxer_("CGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	r__1 = -anorm;
	cgtcon_("I", &c__0, dl, e, du, du2, ip, &r__1, &rcond, w, &info);
	chkxer_("CGTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

    } else if (lsamen_(&c__2, c2, "PT")) {

/*        Test error exits for the positive definite tridiagonal */
/*        routines. */

/*        CPTTRF */

	s_copy(srnamc_1.srnamt, "CPTTRF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpttrf_(&c_n1, d__, e, &info);
	chkxer_("CPTTRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPTTRS */

	s_copy(srnamc_1.srnamt, "CPTTRS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cpttrs_("/", &c__1, &c__0, d__, e, x, &c__1, &info);
	chkxer_("CPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cpttrs_("U", &c_n1, &c__0, d__, e, x, &c__1, &info);
	chkxer_("CPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cpttrs_("U", &c__0, &c_n1, d__, e, x, &c__1, &info);
	chkxer_("CPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cpttrs_("U", &c__2, &c__1, d__, e, x, &c__1, &info);
	chkxer_("CPTTRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPTRFS */

	s_copy(srnamc_1.srnamt, "CPTRFS", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cptrfs_("/", &c__1, &c__0, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("CPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cptrfs_("U", &c_n1, &c__0, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("CPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cptrfs_("U", &c__0, &c_n1, d__, e, df, ef, b, &c__1, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("CPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cptrfs_("U", &c__2, &c__1, d__, e, df, ef, b, &c__1, x, &c__2, r1, r2, 
		 w, rw, &info);
	chkxer_("CPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cptrfs_("U", &c__2, &c__1, d__, e, df, ef, b, &c__2, x, &c__1, r1, r2, 
		 w, rw, &info);
	chkxer_("CPTRFS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CPTCON */

	s_copy(srnamc_1.srnamt, "CPTCON", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cptcon_(&c_n1, d__, e, &anorm, &rcond, rw, &info);
	chkxer_("CPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	r__1 = -anorm;
	cptcon_(&c__0, d__, e, &r__1, &rcond, rw, &info);
	chkxer_("CPTCON", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRGT */

} /* cerrgt_ */
