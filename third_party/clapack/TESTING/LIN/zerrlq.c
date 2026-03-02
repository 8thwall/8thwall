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

/* Subroutine */ int zerrlq_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;
    doublecomplex z__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublecomplex a[4]	/* was [2][2] */, b[2];
    integer i__, j;
    doublecomplex w[2], x[2], af[4]	/* was [2][2] */;
    integer info;
    extern /* Subroutine */ int zgelq2_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, doublecomplex *, integer *), zungl2_(
	    integer *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *), zunml2_(char *, 
	    char *, integer *, integer *, integer *, doublecomplex *, integer 
	    *, doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
	    integer *), alaesm_(char *, logical *, integer *), chkxer_(char *, integer *, integer *, logical *, logical 
	    *), zgelqf_(integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, integer *)
	    , zgelqs_(integer *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, integer *), zunglq_(integer *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     doublecomplex *, integer *, integer *), zunmlq_(char *, char *, 
	    integer *, integer *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, integer *, doublecomplex *, 
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

/*  ZERRLQ tests the error exits for the COMPLEX*16 routines */
/*  that use the LQ decomposition of a general matrix. */

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
	    i__1 = i__ + (j << 1) - 3;
	    d__1 = 1. / (doublereal) (i__ + j);
	    d__2 = -1. / (doublereal) (i__ + j);
	    z__1.r = d__1, z__1.i = d__2;
	    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
	    i__1 = i__ + (j << 1) - 3;
	    d__1 = 1. / (doublereal) (i__ + j);
	    d__2 = -1. / (doublereal) (i__ + j);
	    z__1.r = d__1, z__1.i = d__2;
	    af[i__1].r = z__1.r, af[i__1].i = z__1.i;
/* L10: */
	}
	i__1 = j - 1;
	b[i__1].r = 0., b[i__1].i = 0.;
	i__1 = j - 1;
	w[i__1].r = 0., w[i__1].i = 0.;
	i__1 = j - 1;
	x[i__1].r = 0., x[i__1].i = 0.;
/* L20: */
    }
    infoc_1.ok = TRUE_;

/*     Error exits for LQ factorization */

/*     ZGELQF */

    s_copy(srnamc_1.srnamt, "ZGELQF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    zgelqf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("ZGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zgelqf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("ZGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    zgelqf_(&c__2, &c__1, a, &c__1, b, w, &c__2, &info);
    chkxer_("ZGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    zgelqf_(&c__2, &c__1, a, &c__2, b, w, &c__1, &info);
    chkxer_("ZGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     ZGELQ2 */

    s_copy(srnamc_1.srnamt, "ZGELQ2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    zgelq2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("ZGELQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zgelq2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("ZGELQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    zgelq2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("ZGELQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     ZGELQS */

    s_copy(srnamc_1.srnamt, "ZGELQS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    zgelqs_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("ZGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zgelqs_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("ZGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zgelqs_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("ZGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    zgelqs_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("ZGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zgelqs_(&c__2, &c__2, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("ZGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    zgelqs_(&c__1, &c__2, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("ZGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    zgelqs_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("ZGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     ZUNGLQ */

    s_copy(srnamc_1.srnamt, "ZUNGLQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    zunglq_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("ZUNGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zunglq_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("ZUNGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zunglq_(&c__2, &c__1, &c__0, a, &c__2, x, w, &c__2, &info);
    chkxer_("ZUNGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    zunglq_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("ZUNGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    zunglq_(&c__1, &c__1, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("ZUNGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zunglq_(&c__2, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("ZUNGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    zunglq_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("ZUNGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     ZUNGL2 */

    s_copy(srnamc_1.srnamt, "ZUNGL2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    zungl2_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("ZUNGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zungl2_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("ZUNGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zungl2_(&c__2, &c__1, &c__0, a, &c__2, x, w, &info);
    chkxer_("ZUNGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    zungl2_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("ZUNGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    zungl2_(&c__1, &c__1, &c__2, a, &c__1, x, w, &info);
    chkxer_("ZUNGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zungl2_(&c__2, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("ZUNGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     ZUNMLQ */

    s_copy(srnamc_1.srnamt, "ZUNMLQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    zunmlq_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zunmlq_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    zunmlq_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    zunmlq_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zunmlq_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zunmlq_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zunmlq_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    zunmlq_("L", "N", &c__2, &c__0, &c__2, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    zunmlq_("R", "N", &c__0, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    zunmlq_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    zunmlq_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    zunmlq_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("ZUNMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     ZUNML2 */

    s_copy(srnamc_1.srnamt, "ZUNML2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    zunml2_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    zunml2_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    zunml2_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    zunml2_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zunml2_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zunml2_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    zunml2_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    zunml2_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    zunml2_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    zunml2_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &info);
    chkxer_("ZUNML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRLQ */

} /* zerrlq_ */
