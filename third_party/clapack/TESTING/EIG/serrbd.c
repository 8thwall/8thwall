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

/* Subroutine */ int serrbd_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002,\002 (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    real a[16]	/* was [4][4] */, d__[4], e[4];
    integer i__, j;
    real q[16]	/* was [4][4] */, u[16]	/* was [4][4] */, v[16]	/* was [4][4] 
	    */, w[4];
    char c2[2];
    integer iq[16]	/* was [4][4] */, iw[4], nt;
    real tp[4], tq[4];
    integer info;
    extern /* Subroutine */ int sgebd2_(integer *, integer *, real *, integer 
	    *, real *, real *, real *, real *, real *, integer *), sbdsdc_(
	    char *, char *, integer *, real *, real *, real *, integer *, 
	    real *, integer *, real *, integer *, real *, integer *, integer *
), sgebrd_(integer *, integer *, real *, integer *
, real *, real *, real *, real *, real *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sbdsqr_(char *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *), sorgbr_(
	    char *, integer *, integer *, integer *, real *, integer *, real *
, real *, integer *, integer *), sormbr_(char *, char *, 
	    char *, integer *, integer *, integer *, real *, integer *, real *
, real *, integer *, real *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRBD tests the error exits for SGEBRD, SORGBR, SORMBR, SBDSQR and */
/*  SBDSDC. */

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
	    a[i__ + (j << 2) - 5] = 1.f / (real) (i__ + j);
/* L10: */
	}
/* L20: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits of the SVD routines. */

    if (lsamen_(&c__2, c2, "BD")) {

/*        SGEBRD */

	s_copy(srnamc_1.srnamt, "SGEBRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgebrd_(&c_n1, &c__0, a, &c__1, d__, e, tq, tp, w, &c__1, &info);
	chkxer_("SGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgebrd_(&c__0, &c_n1, a, &c__1, d__, e, tq, tp, w, &c__1, &info);
	chkxer_("SGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgebrd_(&c__2, &c__1, a, &c__1, d__, e, tq, tp, w, &c__2, &info);
	chkxer_("SGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sgebrd_(&c__2, &c__1, a, &c__2, d__, e, tq, tp, w, &c__1, &info);
	chkxer_("SGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        SGEBD2 */

	s_copy(srnamc_1.srnamt, "SGEBD2", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgebd2_(&c_n1, &c__0, a, &c__1, d__, e, tq, tp, w, &info);
	chkxer_("SGEBD2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgebd2_(&c__0, &c_n1, a, &c__1, d__, e, tq, tp, w, &info);
	chkxer_("SGEBD2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgebd2_(&c__2, &c__1, a, &c__1, d__, e, tq, tp, w, &info);
	chkxer_("SGEBD2", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 3;

/*        SORGBR */

	s_copy(srnamc_1.srnamt, "SORGBR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sorgbr_("/", &c__0, &c__0, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sorgbr_("Q", &c_n1, &c__0, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sorgbr_("Q", &c__0, &c_n1, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sorgbr_("Q", &c__0, &c__1, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sorgbr_("Q", &c__1, &c__0, &c__1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sorgbr_("P", &c__1, &c__0, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sorgbr_("P", &c__0, &c__1, &c__1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sorgbr_("Q", &c__0, &c__0, &c_n1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sorgbr_("Q", &c__2, &c__1, &c__1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sorgbr_("Q", &c__2, &c__2, &c__1, a, &c__2, tq, w, &c__1, &info);
	chkxer_("SORGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        SORMBR */

	s_copy(srnamc_1.srnamt, "SORMBR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sormbr_("/", "L", "T", &c__0, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sormbr_("Q", "/", "T", &c__0, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sormbr_("Q", "L", "/", &c__0, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sormbr_("Q", "L", "T", &c_n1, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sormbr_("Q", "L", "T", &c__0, &c_n1, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sormbr_("Q", "L", "T", &c__0, &c__0, &c_n1, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sormbr_("Q", "L", "T", &c__2, &c__0, &c__0, a, &c__1, tq, u, &c__2, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sormbr_("Q", "R", "T", &c__0, &c__2, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sormbr_("P", "L", "T", &c__2, &c__0, &c__2, a, &c__1, tq, u, &c__2, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sormbr_("P", "R", "T", &c__0, &c__2, &c__2, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sormbr_("Q", "R", "T", &c__2, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sormbr_("Q", "L", "T", &c__0, &c__2, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sormbr_("Q", "R", "T", &c__2, &c__0, &c__0, a, &c__1, tq, u, &c__2, w, 
		 &c__1, &info);
	chkxer_("SORMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        SBDSQR */

	s_copy(srnamc_1.srnamt, "SBDSQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sbdsqr_("/", &c__0, &c__0, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sbdsqr_("U", &c_n1, &c__0, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sbdsqr_("U", &c__0, &c_n1, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sbdsqr_("U", &c__0, &c__0, &c_n1, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sbdsqr_("U", &c__0, &c__0, &c__0, &c_n1, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sbdsqr_("U", &c__2, &c__1, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sbdsqr_("U", &c__0, &c__0, &c__2, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sbdsqr_("U", &c__2, &c__0, &c__0, &c__1, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, w, &info);
	chkxer_("SBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        SBDSDC */

	s_copy(srnamc_1.srnamt, "SBDSDC", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sbdsdc_("/", "N", &c__0, d__, e, u, &c__1, v, &c__1, q, iq, w, iw, &
		info);
	chkxer_("SBDSDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sbdsdc_("U", "/", &c__0, d__, e, u, &c__1, v, &c__1, q, iq, w, iw, &
		info);
	chkxer_("SBDSDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sbdsdc_("U", "N", &c_n1, d__, e, u, &c__1, v, &c__1, q, iq, w, iw, &
		info);
	chkxer_("SBDSDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sbdsdc_("U", "I", &c__2, d__, e, u, &c__1, v, &c__1, q, iq, w, iw, &
		info);
	chkxer_("SBDSDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sbdsdc_("U", "I", &c__2, d__, e, u, &c__2, v, &c__1, q, iq, w, iw, &
		info);
	chkxer_("SBDSDC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 5;
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___18.ciunit = infoc_1.nout;
	s_wsfe(&io___18);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___19.ciunit = infoc_1.nout;
	s_wsfe(&io___19);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of SERRBD */

} /* serrbd_ */
