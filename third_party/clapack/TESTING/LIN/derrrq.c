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

/* Subroutine */ int derrrq_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal a[4]	/* was [2][2] */, b[2];
    integer i__, j;
    doublereal w[2], x[2], af[4]	/* was [2][2] */;
    integer info;
    extern /* Subroutine */ int dgerq2_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *), dorgr2_(
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *), dormr2_(char *, char *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), alaesm_(char *, logical *, integer *), 
	    dgerqf_(integer *, integer *, doublereal *, integer *, doublereal 
	    *, doublereal *, integer *, integer *), chkxer_(char *, integer *, 
	     integer *, logical *, logical *), dgerqs_(integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), 
	    dorgrq_(integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *), dormrq_(char *, 
	     char *, integer *, integer *, integer *, doublereal *, integer *, 
	     doublereal *, doublereal *, integer *, doublereal *, integer *, 
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

/*  DERRRQ tests the error exits for the DOUBLE PRECISION routines */
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
	    a[i__ + (j << 1) - 3] = 1. / (doublereal) (i__ + j);
	    af[i__ + (j << 1) - 3] = 1. / (doublereal) (i__ + j);
/* L10: */
	}
	b[j - 1] = 0.;
	w[j - 1] = 0.;
	x[j - 1] = 0.;
/* L20: */
    }
    infoc_1.ok = TRUE_;

/*     Error exits for RQ factorization */

/*     DGERQF */

    s_copy(srnamc_1.srnamt, "DGERQF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dgerqf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("DGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgerqf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("DGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dgerqf_(&c__2, &c__1, a, &c__1, b, w, &c__2, &info);
    chkxer_("DGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dgerqf_(&c__2, &c__1, a, &c__2, b, w, &c__1, &info);
    chkxer_("DGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DGERQ2 */

    s_copy(srnamc_1.srnamt, "DGERQ2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dgerq2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("DGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgerq2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("DGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dgerq2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("DGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DGERQS */

    s_copy(srnamc_1.srnamt, "DGERQS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dgerqs_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgerqs_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgerqs_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dgerqs_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dgerqs_(&c__2, &c__2, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("DGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    dgerqs_(&c__2, &c__2, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    dgerqs_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORGRQ */

    s_copy(srnamc_1.srnamt, "DORGRQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dorgrq_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorgrq_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorgrq_(&c__2, &c__1, &c__0, a, &c__2, x, w, &c__2, &info);
    chkxer_("DORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorgrq_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorgrq_(&c__1, &c__2, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dorgrq_(&c__2, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("DORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    dorgrq_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("DORGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORGR2 */

    s_copy(srnamc_1.srnamt, "DORGR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dorgr2_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("DORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorgr2_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("DORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorgr2_(&c__2, &c__1, &c__0, a, &c__2, x, w, &info);
    chkxer_("DORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorgr2_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("DORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorgr2_(&c__1, &c__2, &c__2, a, &c__2, x, w, &info);
    chkxer_("DORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dorgr2_(&c__2, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("DORGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORMRQ */

    s_copy(srnamc_1.srnamt, "DORMRQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dormrq_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dormrq_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dormrq_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dormrq_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormrq_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormrq_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormrq_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dormrq_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dormrq_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    dormrq_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    dormrq_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    dormrq_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("DORMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORMR2 */

    s_copy(srnamc_1.srnamt, "DORMR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dormr2_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dormr2_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dormr2_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dormr2_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormr2_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormr2_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormr2_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dormr2_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dormr2_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    dormr2_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of DERRRQ */

} /* derrrq_ */
