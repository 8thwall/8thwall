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

static integer c__1 = 1;
static integer c__0 = 0;
static integer c_n1 = -1;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__4 = 4;

/* Subroutine */ int derrec_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error ex\002,\002its ***\002)";

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublereal a[16]	/* was [4][4] */, b[16]	/* was [4][4] */, c__[16]	
	    /* was [4][4] */;
    integer i__, j, m;
    doublereal s[4], wi[4];
    integer nt;
    doublereal wr[4];
    logical sel[4];
    doublereal sep[4];
    integer info, ifst, ilst;
    doublereal work[4], scale;
    integer iwork[4];
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), dtrexc_(char *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *, integer *, 
	    doublereal *, integer *), dtrsna_(char *, char *, logical 
	    *, integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, integer *, integer *), dtrsen_(char *, char *, logical *, integer *, doublereal 
	    *, integer *, doublereal *, integer *, doublereal *, doublereal *, 
	     integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    integer *, integer *, integer *), dtrsyl_(char *, 
	    char *, integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___19 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DERREC tests the error exits for the routines for eigen- condition */
/*  estimation for DOUBLE PRECISION matrices: */
/*     DTRSYL, STREXC, STRSNA and STRSEN. */

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
/*     .. Executable Statements .. */

    infoc_1.nout = *nunit;
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Initialize A, B and SEL */

    for (j = 1; j <= 4; ++j) {
	for (i__ = 1; i__ <= 4; ++i__) {
	    a[i__ + (j << 2) - 5] = 0.;
	    b[i__ + (j << 2) - 5] = 0.;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	a[i__ + (i__ << 2) - 5] = 1.;
	sel[i__ - 1] = TRUE_;
/* L30: */
    }

/*     Test DTRSYL */

    s_copy(srnamc_1.srnamt, "DTRSYL", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dtrsyl_("X", "N", &c__1, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dtrsyl_("N", "X", &c__1, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    dtrsyl_("N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dtrsyl_("N", "N", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    dtrsyl_("N", "N", &c__1, &c__0, &c_n1, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dtrsyl_("N", "N", &c__1, &c__2, &c__0, a, &c__1, b, &c__1, c__, &c__2, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 9;
    dtrsyl_("N", "N", &c__1, &c__0, &c__2, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 11;
    dtrsyl_("N", "N", &c__1, &c__2, &c__0, a, &c__2, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("DTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 8;

/*     Test DTREXC */

    s_copy(srnamc_1.srnamt, "DTREXC", (ftnlen)6, (ftnlen)6);
    ifst = 1;
    ilst = 1;
    infoc_1.infot = 1;
    dtrexc_("X", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    dtrexc_("N", &c__0, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    ilst = 2;
    dtrexc_("N", &c__2, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    dtrexc_("V", &c__2, a, &c__2, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ifst = 0;
    ilst = 1;
    dtrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ifst = 2;
    dtrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ifst = 1;
    ilst = 0;
    dtrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ilst = 2;
    dtrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("DTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 8;

/*     Test DTRSNA */

    s_copy(srnamc_1.srnamt, "DTRSNA", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dtrsna_("X", "A", sel, &c__0, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dtrsna_("B", "X", sel, &c__0, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dtrsna_("B", "A", sel, &c_n1, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    dtrsna_("V", "A", sel, &c__2, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__2, &m, work, &c__2, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    dtrsna_("B", "A", sel, &c__2, a, &c__2, b, &c__1, c__, &c__2, s, sep, &
	    c__2, &m, work, &c__2, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    dtrsna_("B", "A", sel, &c__2, a, &c__2, b, &c__2, c__, &c__1, s, sep, &
	    c__2, &m, work, &c__2, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 13;
    dtrsna_("B", "A", sel, &c__1, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__0, &m, work, &c__1, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 13;
    dtrsna_("B", "S", sel, &c__2, a, &c__2, b, &c__2, c__, &c__2, s, sep, &
	    c__1, &m, work, &c__2, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 16;
    dtrsna_("B", "A", sel, &c__2, a, &c__2, b, &c__2, c__, &c__2, s, sep, &
	    c__2, &m, work, &c__1, iwork, &info);
    chkxer_("DTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 9;

/*     Test DTRSEN */

    sel[0] = FALSE_;
    s_copy(srnamc_1.srnamt, "DTRSEN", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    dtrsen_("X", "N", sel, &c__0, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    dtrsen_("N", "X", sel, &c__0, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    dtrsen_("N", "N", sel, &c_n1, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    dtrsen_("N", "N", sel, &c__2, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__2, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    dtrsen_("N", "V", sel, &c__2, a, &c__2, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 15;
    dtrsen_("N", "V", sel, &c__2, a, &c__2, b, &c__2, wr, wi, &m, s, sep, 
	    work, &c__0, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 15;
    dtrsen_("E", "V", sel, &c__3, a, &c__3, b, &c__3, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 15;
    dtrsen_("V", "V", sel, &c__3, a, &c__3, b, &c__3, wr, wi, &m, s, sep, 
	    work, &c__3, iwork, &c__2, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 17;
    dtrsen_("E", "V", sel, &c__2, a, &c__2, b, &c__2, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__0, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 17;
    dtrsen_("V", "V", sel, &c__3, a, &c__3, b, &c__3, wr, wi, &m, s, sep, 
	    work, &c__4, iwork, &c__1, &info);
    chkxer_("DTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 10;

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___19.ciunit = infoc_1.nout;
	s_wsfe(&io___19);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___20.ciunit = infoc_1.nout;
	s_wsfe(&io___20);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }

    return 0;

/*     End of DERREC */

} /* derrec_ */
