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

/* Subroutine */ int cerrql_(char *path, integer *nunit)
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
    extern /* Subroutine */ int cgeql2_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *), cung2l_(integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *), cunm2l_(char *, char *, integer *, integer *, integer 
	    *, complex *, integer *, complex *, complex *, integer *, complex 
	    *, integer *), cgeqlf_(integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, integer *),
	     alaesm_(char *, logical *, integer *), cgeqls_(integer *, 
	     integer *, integer *, complex *, integer *, complex *, complex *, 
	     integer *, complex *, integer *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), cungql_(
	    integer *, integer *, integer *, complex *, integer *, complex *, 
	    complex *, integer *, integer *), cunmql_(char *, char *, integer 
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

/*  CERRQL tests the error exits for the COMPLEX routines */
/*  that use the QL decomposition of a general matrix. */

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

/*     Error exits for QL factorization */

/*     CGEQLF */

    s_copy(srnamc_1.srnamt, "CGEQLF", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgeqlf_(&c_n1, &c__0, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQLF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeqlf_(&c__0, &c_n1, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQLF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cgeqlf_(&c__2, &c__1, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQLF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cgeqlf_(&c__1, &c__2, a, &c__1, b, w, &c__1, &info);
    chkxer_("CGEQLF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CGEQL2 */

    s_copy(srnamc_1.srnamt, "CGEQL2", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgeql2_(&c_n1, &c__0, a, &c__1, b, w, &info);
    chkxer_("CGEQL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeql2_(&c__0, &c_n1, a, &c__1, b, w, &info);
    chkxer_("CGEQL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cgeql2_(&c__2, &c__1, a, &c__1, b, w, &info);
    chkxer_("CGEQL2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CGEQLS */

    s_copy(srnamc_1.srnamt, "CGEQLS", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cgeqls_(&c_n1, &c__0, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQLS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeqls_(&c__0, &c_n1, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQLS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cgeqls_(&c__1, &c__2, &c__0, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQLS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cgeqls_(&c__0, &c__0, &c_n1, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQLS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cgeqls_(&c__2, &c__1, &c__0, a, &c__1, x, b, &c__2, w, &c__1, &info);
    chkxer_("CGEQLS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    cgeqls_(&c__2, &c__1, &c__0, a, &c__2, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQLS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cgeqls_(&c__1, &c__1, &c__2, a, &c__1, x, b, &c__1, w, &c__1, &info);
    chkxer_("CGEQLS", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNGQL */

    s_copy(srnamc_1.srnamt, "CUNGQL", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cungql_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungql_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cungql_(&c__1, &c__2, &c__0, a, &c__1, x, w, &c__2, &info);
    chkxer_("CUNGQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungql_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cungql_(&c__1, &c__1, &c__2, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cungql_(&c__2, &c__1, &c__0, a, &c__1, x, w, &c__1, &info);
    chkxer_("CUNGQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    cungql_(&c__2, &c__2, &c__0, a, &c__2, x, w, &c__1, &info);
    chkxer_("CUNGQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNG2L */

    s_copy(srnamc_1.srnamt, "CUNG2L", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cung2l_(&c_n1, &c__0, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cung2l_(&c__0, &c_n1, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cung2l_(&c__1, &c__2, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cung2l_(&c__0, &c__0, &c_n1, a, &c__1, x, w, &info);
    chkxer_("CUNG2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cung2l_(&c__2, &c__1, &c__2, a, &c__2, x, w, &info);
    chkxer_("CUNG2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cung2l_(&c__2, &c__1, &c__0, a, &c__1, x, w, &info);
    chkxer_("CUNG2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNMQL */

    s_copy(srnamc_1.srnamt, "CUNMQL", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cunmql_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cunmql_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cunmql_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cunmql_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmql_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmql_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunmql_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmql_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunmql_("R", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cunmql_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    cunmql_("L", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 12;
    cunmql_("R", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &c__1, &
	    info);
    chkxer_("CUNMQL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     CUNM2L */

    s_copy(srnamc_1.srnamt, "CUNM2L", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    cunm2l_("/", "N", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    cunm2l_("L", "/", &c__0, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    cunm2l_("L", "N", &c_n1, &c__0, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    cunm2l_("L", "N", &c__0, &c_n1, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunm2l_("L", "N", &c__0, &c__0, &c_n1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunm2l_("L", "N", &c__0, &c__1, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    cunm2l_("R", "N", &c__1, &c__0, &c__1, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunm2l_("L", "N", &c__2, &c__1, &c__0, a, &c__1, x, af, &c__2, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    cunm2l_("R", "N", &c__1, &c__2, &c__0, a, &c__1, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    cunm2l_("L", "N", &c__2, &c__1, &c__0, a, &c__2, x, af, &c__1, w, &info);
    chkxer_("CUNM2L", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRQL */

} /* cerrql_ */
