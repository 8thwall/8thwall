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

/* Subroutine */ int cerrrq_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    complex q__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    complex a[4]	/* was [2][2] */, b[2];
    integer i__, j;
    complex w[2], x[2], af[4]	/* was [2][2] */;
    integer info;
    extern /* Subroutine */ int cgerq2_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *), cungr2_(integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *), cunmr2_(char *, char *, integer *, integer *, integer 
	    *, complex *, integer *, complex *, complex *, integer *, complex 
	    *, integer *), alaesm_(char *, logical *, integer 
	    *), cgerqf_(integer *, integer *, complex *, integer *, 
	    complex *, complex *, integer *, integer *), cgerqs_(integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *, complex *, integer *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), cungrq_(
	    integer *, integer *, integer *, complex *, integer *, complex *, 
	    complex *, integer *, integer *), cunmrq_(char *, char *, integer 
	    *, integer *, integer *, complex *, integer *, complex *, complex 
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

/*  CERRRQ tests the error exits for the COMPLEX routines */
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
	    i__1 = i__ + (j << 1) - 3;
	    r__1 = 1.f / (real) (i__ + j);
	    r__2 = -1.f / (real) (i__ + j);
	    q__1.r = r__1, q__1.i = r__2;
	    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	    i__1 = i__ + (j << 1) - 3;
	    r__1 = 1.f / (real) (i__ + j);
	    r__2 = -1.f / (real) (i__ + j);
	    q__1.r = r__1, q__1.i = r__2;
	    af[i__1].r = q__1.r, af[i__1].i = q__1.i;
/* L10: */
	}
	i__1 = j - 1;
	b[i__1].r = 0.f, b[i__1].i = 0.f;
	i__1 = j - 1;
	w[i__1].r = 0.f, w[i__1].i = 0.f;
	i__1 = j - 1;
	x[i__1].r = 0.f, x[i__1].i = 0.f;
/* L20: */
    }
    infoc_1.ok = TRUE_;

/*     Error exits for RQ factorization */

/*     CGERQF */

    s_copy(srnamc_1.srnamt, "CGERQF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgerqf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgerqf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cgerqf_(&c__2, &c__1, a, &c__1, b, w, &c__2, &info);
    chkxer_("CGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cgerqf_(&c__2, &c__1, a, &c__2, b, w, &c__1, &info);
    chkxer_("CGERQF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CGERQ2 */

    s_copy(srnamc_1.srnamt, "CGERQ2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgerq2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("CGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgerq2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("CGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cgerq2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("CGERQ2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CGERQS */

    s_copy(srnamc_1.srnamt, "CGERQS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgerqs_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgerqs_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgerqs_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cgerqs_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cgerqs_(&c__2, &c__2, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("CGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    cgerqs_(&c__2, &c__2, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cgerqs_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGERQS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNGRQ */

    s_copy(srnamc_1.srnamt, "CUNGRQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cungrq_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungrq_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungrq_(&c__2, &c__1, &c__0, a, &c__2, x, w, &c__2, &info);
    chkxer_("CUNGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungrq_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungrq_(&c__1, &c__2, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cungrq_(&c__2, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("CUNGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    cungrq_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("CUNGRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNGR2 */

    s_copy(srnamc_1.srnamt, "CUNGR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cungr2_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungr2_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungr2_(&c__2, &c__1, &c__0, a, &c__2, x, w, &info);
    chkxer_("CUNGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungr2_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("CUNGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungr2_(&c__1, &c__2, &c__2, a, &c__2, x, w, &info);
    chkxer_("CUNGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cungr2_(&c__2, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNGR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNMRQ */

    s_copy(srnamc_1.srnamt, "CUNMRQ", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cunmrq_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cunmrq_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cunmrq_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cunmrq_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmrq_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmrq_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmrq_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmrq_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmrq_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cunmrq_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    cunmrq_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    cunmrq_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("CUNMRQ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNMR2 */

    s_copy(srnamc_1.srnamt, "CUNMR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cunmr2_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cunmr2_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cunmr2_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cunmr2_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmr2_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmr2_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmr2_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmr2_("L", "N", &c__2, &c__1, &c__2, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmr2_("R", "N", &c__1, &c__2, &c__2, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cunmr2_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNMR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRRQ */

} /* cerrrq_ */
