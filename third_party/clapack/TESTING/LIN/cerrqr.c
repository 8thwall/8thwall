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

/* Subroutine */ int cerrqr_(char *path, integer *nunit)
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
    extern /* Subroutine */ int cgeqr2_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *), cung2r_(integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *), cunm2r_(char *, char *, integer *, integer *, integer 
	    *, complex *, integer *, complex *, complex *, integer *, complex 
	    *, integer *), alaesm_(char *, logical *, integer 
	    *), cgeqrf_(integer *, integer *, complex *, integer *, 
	    complex *, complex *, integer *, integer *), cgeqrs_(integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *, complex *, integer *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), cungqr_(
	    integer *, integer *, integer *, complex *, integer *, complex *, 
	    complex *, integer *, integer *), cunmqr_(char *, char *, integer 
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

/*  CERRQR tests the error exits for the COMPLEX routines */
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

/*     Error exits for QR factorization */

/*     CGEQRF */

    s_copy(srnamc_1.srnamt, "CGEQRF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgeqrf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeqrf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cgeqrf_(&c__2, &c__1, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cgeqrf_(&c__1, &c__2, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQRF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CGEQR2 */

    s_copy(srnamc_1.srnamt, "CGEQR2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgeqr2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("CGEQR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeqr2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("CGEQR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cgeqr2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("CGEQR2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CGEQRS */

    s_copy(srnamc_1.srnamt, "CGEQRS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgeqrs_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeqrs_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeqrs_(&c__1, &c__2, &c__0, a, &c__2, x, b, &c__2, w, &c__1, &info);
    chkxer_("CGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cgeqrs_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cgeqrs_(&c__2, &c__1, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("CGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    cgeqrs_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cgeqrs_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQRS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNGQR */

    s_copy(srnamc_1.srnamt, "CUNGQR", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cungqr_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungqr_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungqr_(&c__1, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("CUNGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungqr_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungqr_(&c__1, &c__1, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cungqr_(&c__2, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("CUNGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    cungqr_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("CUNGQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNG2R */

    s_copy(srnamc_1.srnamt, "CUNG2R", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cung2r_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cung2r_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cung2r_(&c__1, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cung2r_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("CUNG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cung2r_(&c__2, &c__1, &c__2, a, &c__2, x, w, &info);
    chkxer_("CUNG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cung2r_(&c__2, &c__1, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNMQR */

    s_copy(srnamc_1.srnamt, "CUNMQR", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cunmqr_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cunmqr_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cunmqr_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cunmqr_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmqr_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmqr_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmqr_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmqr_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmqr_("R", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cunmqr_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    cunmqr_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    cunmqr_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("CUNMQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNM2R */

    s_copy(srnamc_1.srnamt, "CUNM2R", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cunm2r_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cunm2r_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cunm2r_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cunm2r_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunm2r_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunm2r_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunm2r_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunm2r_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunm2r_("R", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cunm2r_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &info);
    chkxer_("CUNM2R", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRQR */

} /* cerrqr_ */
