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

/* Subroutine */ int derrlq_(char *path, integer *nunit)
{
    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    doublereal a[4]	/* was [2][2] */, b[2];
    integer i__, j;
    doublereal w[2], x[2], af[4]	/* was [2][2] */;
    integer info;
    extern /* Subroutine */ int dgelq2_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *), dorgl2_(
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *), dorml2_(char *, char *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), alaesm_(char *, logical *, integer *), 
	    dgelqf_(integer *, integer *, doublereal *, integer *, doublereal 
	    *, doublereal *, integer *, integer *), dgelqs_(integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), 
	    chkxer_(char *, integer *, integer *, logical *, logical *), dorglq_(integer *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, integer *), 
	    dormlq_(char *, char *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERRLQ tests the error exits for the DOUBLE PRECISION routines */
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

/*     Error exits for LQ factorization */

/*     DGELQF */

    s_copy(srnamc_1.srnamt, "DGELQF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dgelqf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("DGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgelqf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("DGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dgelqf_(&c__2, &c__1, a, &c__1, b, w, &c__2, &info);
    chkxer_("DGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dgelqf_(&c__2, &c__1, a, &c__2, b, w, &c__1, &info);
    chkxer_("DGELQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DGELQ2 */

    s_copy(srnamc_1.srnamt, "DGELQ2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dgelq2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("DGELQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgelq2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("DGELQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dgelq2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("DGELQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DGELQS */

    s_copy(srnamc_1.srnamt, "DGELQS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dgelqs_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgelqs_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dgelqs_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dgelqs_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dgelqs_(&c__2, &c__2, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("DGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    dgelqs_(&c__1, &c__2, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    dgelqs_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("DGELQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORGLQ */

    s_copy(srnamc_1.srnamt, "DORGLQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dorglq_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorglq_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorglq_(&c__2, &c__1, &c__0, a, &c__2, x, w, &c__2, &info);
    chkxer_("DORGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorglq_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorglq_(&c__1, &c__1, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("DORGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dorglq_(&c__2, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("DORGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    dorglq_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("DORGLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORGL2 */

    s_copy(srnamc_1.srnamt, "DORGL2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dorgl2_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("DORGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorgl2_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("DORGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorgl2_(&c__2, &c__1, &c__0, a, &c__2, x, w, &info);
    chkxer_("DORGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorgl2_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("DORGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorgl2_(&c__1, &c__1, &c__2, a, &c__1, x, w, &info);
    chkxer_("DORGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dorgl2_(&c__2, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("DORGL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORMLQ */

    s_copy(srnamc_1.srnamt, "DORMLQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dormlq_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dormlq_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dormlq_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dormlq_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormlq_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormlq_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dormlq_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dormlq_("L", "N", &c__2, &c__0, &c__2, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dormlq_("R", "N", &c__0, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    dormlq_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    dormlq_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    dormlq_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("DORMLQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     DORML2 */

    s_copy(srnamc_1.srnamt, "DORML2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dorml2_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dorml2_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dorml2_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dorml2_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dorml2_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dorml2_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dorml2_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dorml2_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dorml2_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    dorml2_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &info);
    chkxer_("DORML2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of DERRLQ */

} /* derrlq_ */
