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

static integer c_n1 = -1;
static integer c__0 = 0;
static integer c__1 = 1;
static integer c__2 = 2;

/* Subroutine */ int serrqr_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real a[4]	/* was [2][2] */, b[2];
    integer i__, j;
    real w[2], x[2], af[4]	/* was [2][2] */;
    integer info;
    extern /* Subroutine */ int sgeqr2_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *), sorg2r_(integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *), sorm2r_(
	    char *, char *, integer *, integer *, integer *, real *, integer *
, real *, real *, integer *, real *, integer *), 
	    alaesm_(char *, logical *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), sgeqrf_(
	    integer *, integer *, real *, integer *, real *, real *, integer *
, integer *), sgeqrs_(integer *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *, integer *, integer *
), sorgqr_(integer *, integer *, integer *, real *, integer *, 
	    real *, real *, integer *, integer *), sormqr_(char *, char *, 
	    integer *, integer *, integer *, real *, integer *, real *, real *
, integer *, real *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRQR tests the error exits for the REAL routines */
/*  that use the QR decomposition of a general matrix. */

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

/*     Set the variables to innocuous values. */

    for (j = 1; j <= 2; ++j) {
	for (i__ = 1; i__ <= 2; ++i__) {
	    a[i__ + (j << 1) - 3] = 1.f / (real) (i__ + j);
	    af[i__ + (j << 1) - 3] = 1.f / (real) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.f;
	w[j - 1] = 0.f;
	x[j - 1] = 0.f;
/* L20: */
    }
    infoc_1.ok = TRUE_;

/*     Error exits for QR factorization */

/*     SGEQRF */

    s_copy(srnamc_1.srnamt, "SGEQRF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sgeqrf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("SGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgeqrf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("SGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sgeqrf_(&c__2, &c__1, a, &c__1, b, w, &c__1, &info);
    chkxer_("SGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sgeqrf_(&c__1, &c__2, a, &c__1, b, w, &c__1, &info);
    chkxer_("SGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SGEQR2 */

    s_copy(srnamc_1.srnamt, "SGEQR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sgeqr2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("SGEQR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgeqr2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("SGEQR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sgeqr2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("SGEQR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SGEQRS */

    s_copy(srnamc_1.srnamt, "SGEQRS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sgeqrs_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgeqrs_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgeqrs_(&c__1, &c__2, &c__0, a, &c__2, x, b, &c__2, w, &c__1, &info);
    chkxer_("SGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sgeqrs_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sgeqrs_(&c__2, &c__1, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("SGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    sgeqrs_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    sgeqrs_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORGQR */

    s_copy(srnamc_1.srnamt, "SORGQR", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sorgqr_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorgqr_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorgqr_(&c__1, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("SORGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorgqr_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorgqr_(&c__1, &c__1, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sorgqr_(&c__2, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("SORGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    sorgqr_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("SORGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORG2R */

    s_copy(srnamc_1.srnamt, "SORG2R", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sorg2r_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("SORG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorg2r_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("SORG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorg2r_(&c__1, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("SORG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorg2r_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("SORG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorg2r_(&c__2, &c__1, &c__2, a, &c__2, x, w, &info);
    chkxer_("SORG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sorg2r_(&c__2, &c__1, &c__0, a, &c__1, x, w, &info);
    chkxer_("SORG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORMQR */

    s_copy(srnamc_1.srnamt, "SORMQR", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sormqr_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sormqr_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sormqr_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sormqr_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormqr_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormqr_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormqr_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sormqr_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sormqr_("R", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    sormqr_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    sormqr_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    sormqr_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("SORMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORM2R */

    s_copy(srnamc_1.srnamt, "SORM2R", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sorm2r_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorm2r_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorm2r_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sorm2r_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sorm2r_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sorm2r_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sorm2r_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sorm2r_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sorm2r_("R", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    sorm2r_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &info);
    chkxer_("SORM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRQR */

} /* serrqr_ */
