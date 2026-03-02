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

/* Subroutine */ int cerrec_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    complex a[16]	/* was [4][4] */, b[16]	/* was [4][4] */, c__[16]	
	    /* was [4][4] */;
    integer i__, j, m;
    real s[4];
    complex x[4];
    integer nt;
    real rw[24];
    logical sel[4];
    real sep[4];
    integer info, ifst, ilst;
    complex work[24];
    real scale;
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), ctrexc_(char *, integer *, complex *, 
	    integer *, complex *, integer *, integer *, integer *, integer *), ctrsna_(char *, char *, logical *, integer *, complex *, 
	    integer *, complex *, integer *, complex *, integer *, real *, 
	    real *, integer *, integer *, complex *, integer *, real *, 
	    integer *), ctrsen_(char *, char *, logical *, 
	    integer *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, real *, complex *, integer *, integer *), ctrsyl_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___18 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERREC tests the error exits for the routines for eigen- condition */
/*  estimation for REAL matrices: */
/*     CTRSYL, CTREXC, CTRSNA and CTRSEN. */

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
	    i__1 = i__ + (j << 2) - 5;
	    a[i__1].r = 0.f, a[i__1].i = 0.f;
	    i__1 = i__ + (j << 2) - 5;
	    b[i__1].r = 0.f, b[i__1].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	i__1 = i__ + (i__ << 2) - 5;
	a[i__1].r = 1.f, a[i__1].i = 0.f;
	sel[i__ - 1] = TRUE_;
/* L30: */
    }

/*     Test CTRSYL */

    s_copy(srnamc_1.srnamt, "CTRSYL", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    ctrsyl_("X", "N", &c__1, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    ctrsyl_("N", "X", &c__1, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 3;
    ctrsyl_("N", "N", &c__0, &c__0, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    ctrsyl_("N", "N", &c__1, &c_n1, &c__0, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 5;
    ctrsyl_("N", "N", &c__1, &c__0, &c_n1, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ctrsyl_("N", "N", &c__1, &c__2, &c__0, a, &c__1, b, &c__1, c__, &c__2, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 9;
    ctrsyl_("N", "N", &c__1, &c__0, &c__2, a, &c__1, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 11;
    ctrsyl_("N", "N", &c__1, &c__2, &c__0, a, &c__2, b, &c__1, c__, &c__1, &
	    scale, &info);
    chkxer_("CTRSYL", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 8;

/*     Test CTREXC */

    s_copy(srnamc_1.srnamt, "CTREXC", (ftnlen)6, (ftnlen)6);
    ifst = 1;
    ilst = 1;
    infoc_1.infot = 1;
    ctrexc_("X", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ctrexc_("N", &c__0, a, &c__1, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    ilst = 2;
    ctrexc_("N", &c__2, a, &c__1, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    ctrexc_("V", &c__2, a, &c__2, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ifst = 0;
    ilst = 1;
    ctrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 7;
    ifst = 2;
    ctrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ifst = 1;
    ilst = 0;
    ctrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ilst = 2;
    ctrexc_("V", &c__1, a, &c__1, b, &c__1, &ifst, &ilst, &info);
    chkxer_("CTREXC", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 8;

/*     Test CTRSNA */

    s_copy(srnamc_1.srnamt, "CTRSNA", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    ctrsna_("X", "A", sel, &c__0, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    ctrsna_("B", "X", sel, &c__0, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    ctrsna_("B", "A", sel, &c_n1, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__1, &m, work, &c__1, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    ctrsna_("V", "A", sel, &c__2, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__2, &m, work, &c__2, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ctrsna_("B", "A", sel, &c__2, a, &c__2, b, &c__1, c__, &c__2, s, sep, &
	    c__2, &m, work, &c__2, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 10;
    ctrsna_("B", "A", sel, &c__2, a, &c__2, b, &c__2, c__, &c__1, s, sep, &
	    c__2, &m, work, &c__2, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 13;
    ctrsna_("B", "A", sel, &c__1, a, &c__1, b, &c__1, c__, &c__1, s, sep, &
	    c__0, &m, work, &c__1, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 13;
    ctrsna_("B", "S", sel, &c__2, a, &c__2, b, &c__2, c__, &c__2, s, sep, &
	    c__1, &m, work, &c__1, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 16;
    ctrsna_("B", "A", sel, &c__2, a, &c__2, b, &c__2, c__, &c__2, s, sep, &
	    c__2, &m, work, &c__1, rw, &info);
    chkxer_("CTRSNA", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 9;

/*     Test CTRSEN */

    sel[0] = FALSE_;
    s_copy(srnamc_1.srnamt, "CTRSEN", (ftnlen)6, (ftnlen)6);
    infoc_1.infot = 1;
    ctrsen_("X", "N", sel, &c__0, a, &c__1, b, &c__1, x, &m, s, sep, work, &
	    c__1, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 2;
    ctrsen_("N", "X", sel, &c__0, a, &c__1, b, &c__1, x, &m, s, sep, work, &
	    c__1, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 4;
    ctrsen_("N", "N", sel, &c_n1, a, &c__1, b, &c__1, x, &m, s, sep, work, &
	    c__1, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 6;
    ctrsen_("N", "N", sel, &c__2, a, &c__1, b, &c__1, x, &m, s, sep, work, &
	    c__2, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 8;
    ctrsen_("N", "V", sel, &c__2, a, &c__2, b, &c__1, x, &m, s, sep, work, &
	    c__1, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 14;
    ctrsen_("N", "V", sel, &c__2, a, &c__2, b, &c__2, x, &m, s, sep, work, &
	    c__0, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 14;
    ctrsen_("E", "V", sel, &c__3, a, &c__3, b, &c__3, x, &m, s, sep, work, &
	    c__1, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    infoc_1.infot = 14;
    ctrsen_("V", "V", sel, &c__3, a, &c__3, b, &c__3, x, &m, s, sep, work, &
	    c__3, &info);
    chkxer_("CTRSEN", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
	    infoc_1.ok);
    nt += 8;

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

/*     End of CERREC */

} /* cerrec_ */
