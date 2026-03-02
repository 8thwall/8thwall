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

/* Subroutine */ int serrrq_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real a[4]	/* was [2][2] */, b[2];
    integer i__, j;
    real w[2], x[2], af[4]	/* was [2][2] */;
    integer info;
    extern /* Subroutine */ int sgerq2_(integer *, integer *, real *, integer 
	    *, real *, real *, integer *), sorgr2_(integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *), sormr2_(
	    char *, char *, integer *, integer *, integer *, real *, integer *
, real *, real *, integer *, real *, integer *), 
	    alaesm_(char *, logical *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), sgerqf_(
	    integer *, integer *, real *, integer *, real *, real *, integer *
, integer *), sgerqs_(integer *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *, integer *, integer *
), sorgrq_(integer *, integer *, integer *, real *, integer *, 
	    real *, real *, integer *, integer *), sormrq_(char *, char *, 
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

/*  SERRRQ tests the error exits for the REAL routines */
/*  that use the RQ decomposition of a general matrix. */

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

/*     Error exits for RQ factorization */

/*     SGERQF */

    s_copy(srnamc_1.srnamt, "SGERQF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sgerqf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("SGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgerqf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("SGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sgerqf_(&c__2, &c__1, a, &c__1, b, w, &c__2, &info);
    chkxer_("SGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sgerqf_(&c__2, &c__1, a, &c__2, b, w, &c__1, &info);
    chkxer_("SGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SGERQ2 */

    s_copy(srnamc_1.srnamt, "SGERQ2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sgerq2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("SGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgerq2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("SGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sgerq2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("SGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SGERQS */

    s_copy(srnamc_1.srnamt, "SGERQS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sgerqs_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgerqs_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sgerqs_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sgerqs_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sgerqs_(&c__2, &c__2, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("SGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    sgerqs_(&c__2, &c__2, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    sgerqs_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("SGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORGRQ */

    s_copy(srnamc_1.srnamt, "SORGRQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sorgrq_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorgrq_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorgrq_(&c__2, &c__1, &c__0, a, &c__2, x, w, &c__2, &info);
    chkxer_("SORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorgrq_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorgrq_(&c__1, &c__2, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("SORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sorgrq_(&c__2, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("SORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    sorgrq_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("SORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORGR2 */

    s_copy(srnamc_1.srnamt, "SORGR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sorgr2_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("SORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorgr2_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("SORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sorgr2_(&c__2, &c__1, &c__0, a, &c__2, x, w, &info);
    chkxer_("SORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorgr2_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("SORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sorgr2_(&c__1, &c__2, &c__2, a, &c__2, x, w, &info);
    chkxer_("SORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sorgr2_(&c__2, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("SORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORMRQ */

    s_copy(srnamc_1.srnamt, "SORMRQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sormrq_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sormrq_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sormrq_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sormrq_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormrq_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormrq_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormrq_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sormrq_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sormrq_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    sormrq_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    sormrq_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    sormrq_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("SORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     SORMR2 */

    s_copy(srnamc_1.srnamt, "SORMR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    sormr2_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    sormr2_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    sormr2_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    sormr2_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormr2_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormr2_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    sormr2_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sormr2_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    sormr2_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    sormr2_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("SORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRRQ */

} /* serrrq_ */
