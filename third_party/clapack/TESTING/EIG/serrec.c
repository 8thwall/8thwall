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

/* Subroutine */ int serrec_(char *path, integer *nunit)
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
    real a[16]	/* was [4][4] */, b[16]	/* was [4][4] */, c__[16]	/* 
	    was [4][4] */;
    integer i__, j, m;
    real s[4], wi[4];
    integer nt;
    real wr[4];
    logical sel[4];
    real sep[4];
    integer info, ifst, ilst;
    real work[4], scale;
    integer iwork[4];
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), strexc_(char *, integer *, real *, integer 
	    *, real *, integer *, integer *, integer *, real *, integer *), strsna_(char *, char *, logical *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *, real *, integer *, integer *, integer *), strsen_(char *, char *, logical *, integer *, 
	    real *, integer *, real *, integer *, real *, real *, integer *, 
	    real *, real *, real *, integer *, integer *, integer *, integer *
), strsyl_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, real *, integer *);

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

/*  SERREC tests the error exits for the routines for eigen- condition */
/*  estimation for REAL matrices: */
/*     STRSYL, STREXC, STRSNA and STRSEN. */

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
	    a[i__ + (j << 2) - 5] = 0.f;
	    b[i__ + (j << 2) - 5] = 0.f;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	a[i__ + (i__ << 2) - 5] = 1.f;
	sel[i__ - 1] = TRUE_;
/* L30: */
    }

/*     Test STRSYL */

    s_copy(srnamc_1.srnamt, "STRSYL", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    strsyl_("X", "N", &c__1, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    strsyl_("N", "X", &c__1, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    strsyl_("N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    strsyl_("N", "N", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    strsyl_("N", "N", &c__1, &c__0, &c_n1, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    strsyl_("N", "N", &c__1, &c__2, &c__0, a, &c__1, b, &c__1, c__, &c__2, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 9;
    strsyl_("N", "N", &c__1, &c__0, &c__2, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 11;
    strsyl_("N", "N", &c__1, &c__2, &c__0, a, &c__2, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("STRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 8;

/*     Test STREXC */

    s_copy(srnamc_1.srnamt, "STREXC", (ftnlen)6, (ftnlen)6);
    ifst = 1;
    ilst = 1;
    infoc_1.infot = 1;
    strexc_("X", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    strexc_("N", &c__0, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    ilst = 2;
    strexc_("N", &c__2, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    strexc_("V", &c__2, a, &c__2, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ifst = 0;
    ilst = 1;
    strexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ifst = 2;
    strexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ifst = 1;
    ilst = 0;
    strexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ilst = 2;
    strexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, work, &info);
    chkxer_("STREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 8;

/*     Test STRSNA */

    s_copy(srnamc_1.srnamt, "STRSNA", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    strsna_("X", "A", sel, &c__0, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    strsna_("B", "X", sel, &c__0, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    strsna_("B", "A", sel, &c_n1, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    strsna_("V", "A", sel, &c__2, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__2, &m, work, &c__2, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    strsna_("B", "A", sel, &c__2, a, &c__2, b, &c__1, c__, &c__2, s, sep, &
	    c__2, &m, work, &c__2, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    strsna_("B", "A", sel, &c__2, a, &c__2, b, &c__2, c__, &c__1, s, sep, &
	    c__2, &m, work, &c__2, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 13;
    strsna_("B", "A", sel, &c__1, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__0, &m, work, &c__1, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 13;
    strsna_("B", "S", sel, &c__2, a, &c__2, b, &c__2, c__, &c__2, s, sep, &
	    c__1, &m, work, &c__2, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 16;
    strsna_("B", "A", sel, &c__2, a, &c__2, b, &c__2, c__, &c__2, s, sep, &
	    c__2, &m, work, &c__1, iwork, &info);
    chkxer_("STRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 9;

/*     Test STRSEN */

    sel[0] = FALSE_;
    s_copy(srnamc_1.srnamt, "STRSEN", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    strsen_("X", "N", sel, &c__0, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    strsen_("N", "X", sel, &c__0, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    strsen_("N", "N", sel, &c_n1, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    strsen_("N", "N", sel, &c__2, a, &c__1, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__2, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    strsen_("N", "V", sel, &c__2, a, &c__2, b, &c__1, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 15;
    strsen_("N", "V", sel, &c__2, a, &c__2, b, &c__2, wr, wi, &m, s, sep, 
	    work, &c__0, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 15;
    strsen_("E", "V", sel, &c__3, a, &c__3, b, &c__3, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 15;
    strsen_("V", "V", sel, &c__3, a, &c__3, b, &c__3, wr, wi, &m, s, sep, 
	    work, &c__3, iwork, &c__2, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 17;
    strsen_("E", "V", sel, &c__2, a, &c__2, b, &c__2, wr, wi, &m, s, sep, 
	    work, &c__1, iwork, &c__0, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 17;
    strsen_("V", "V", sel, &c__3, a, &c__3, b, &c__3, wr, wi, &m, s, sep, 
	    work, &c__4, iwork, &c__1, &info);
    chkxer_("STRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of SERREC */

} /* serrec_ */
