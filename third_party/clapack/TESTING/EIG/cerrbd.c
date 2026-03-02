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

/* Subroutine */ int cerrbd_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    complex a[16]	/* was [4][4] */;
    real d__[4], e[4];
    integer i__, j;
    complex u[16]	/* was [4][4] */, v[16]	/* was [4][4] */, w[4];
    char c2[2];
    integer nt;
    complex tp[4], tq[4];
    real rw[16];
    integer info;
    extern /* Subroutine */ int cgebrd_(integer *, integer *, complex *, 
	    integer *, real *, real *, complex *, complex *, complex *, 
	    integer *, integer *), cbdsqr_(char *, integer *, integer *, 
	    integer *, integer *, real *, real *, complex *, integer *, 
	    complex *, integer *, complex *, integer *, real *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int cungbr_(char *, integer *, integer *, integer 
	    *, complex *, integer *, complex *, complex *, integer *, integer 
	    *), chkxer_(char *, integer *, integer *, logical *, 
	    logical *), cunmbr_(char *, char *, char *, integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *, complex *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___16 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRBD tests the error exits for CGEBRD, CUNGBR, CUNMBR, and CBDSQR. */

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
	    i__1 = i__ + (j << 2) - 5;
	    r__1 = 1.f / (real) (i__ + j);
	    a[i__1].r = r__1, a[i__1].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

/*     Test error exits of the SVD routines. */

    if (lsamen_(&c__2, c2, "BD")) {

/*        CGEBRD */

	s_copy(srnamc_1.srnamt, "CGEBRD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgebrd_(&c_n1, &c__0, a, &c__1, d__, e, tq, tp, w, &c__1, &info);
	chkxer_("CGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgebrd_(&c__0, &c_n1, a, &c__1, d__, e, tq, tp, w, &c__1, &info);
	chkxer_("CGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgebrd_(&c__2, &c__1, a, &c__1, d__, e, tq, tp, w, &c__2, &info);
	chkxer_("CGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgebrd_(&c__2, &c__1, a, &c__2, d__, e, tq, tp, w, &c__1, &info);
	chkxer_("CGEBRD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 4;

/*        CUNGBR */

	s_copy(srnamc_1.srnamt, "CUNGBR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cungbr_("/", &c__0, &c__0, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cungbr_("Q", &c_n1, &c__0, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cungbr_("Q", &c__0, &c_n1, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cungbr_("Q", &c__0, &c__1, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cungbr_("Q", &c__1, &c__0, &c__1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cungbr_("P", &c__1, &c__0, &c__0, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cungbr_("P", &c__0, &c__1, &c__1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cungbr_("Q", &c__0, &c__0, &c_n1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cungbr_("Q", &c__2, &c__1, &c__1, a, &c__1, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cungbr_("Q", &c__2, &c__2, &c__1, a, &c__2, tq, w, &c__1, &info);
	chkxer_("CUNGBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

/*        CUNMBR */

	s_copy(srnamc_1.srnamt, "CUNMBR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cunmbr_("/", "L", "T", &c__0, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cunmbr_("Q", "/", "T", &c__0, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cunmbr_("Q", "L", "/", &c__0, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cunmbr_("Q", "L", "C", &c_n1, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cunmbr_("Q", "L", "C", &c__0, &c_n1, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cunmbr_("Q", "L", "C", &c__0, &c__0, &c_n1, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cunmbr_("Q", "L", "C", &c__2, &c__0, &c__0, a, &c__1, tq, u, &c__2, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cunmbr_("Q", "R", "C", &c__0, &c__2, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cunmbr_("P", "L", "C", &c__2, &c__0, &c__2, a, &c__1, tq, u, &c__2, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cunmbr_("P", "R", "C", &c__0, &c__2, &c__2, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cunmbr_("Q", "R", "C", &c__2, &c__0, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__1, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cunmbr_("Q", "L", "C", &c__0, &c__2, &c__0, a, &c__1, tq, u, &c__1, w, 
		 &c__0, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cunmbr_("Q", "R", "C", &c__2, &c__0, &c__0, a, &c__1, tq, u, &c__2, w, 
		 &c__0, &info);
	chkxer_("CUNMBR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 13;

/*        CBDSQR */

	s_copy(srnamc_1.srnamt, "CBDSQR", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cbdsqr_("/", &c__0, &c__0, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cbdsqr_("U", &c_n1, &c__0, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cbdsqr_("U", &c__0, &c_n1, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cbdsqr_("U", &c__0, &c__0, &c_n1, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cbdsqr_("U", &c__0, &c__0, &c__0, &c_n1, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cbdsqr_("U", &c__2, &c__1, &c__0, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cbdsqr_("U", &c__0, &c__0, &c__2, &c__0, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	cbdsqr_("U", &c__2, &c__0, &c__0, &c__1, d__, e, v, &c__1, u, &c__1, 
		a, &c__1, rw, &info);
	chkxer_("CBDSQR", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;
    }

/*     Print a summary line. */

    if (infoc_1.ok) {
	io___16.ciunit = infoc_1.nout;
	s_wsfe(&io___16);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___17.ciunit = infoc_1.nout;
	s_wsfe(&io___17);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }


    return 0;

/*     End of CERRBD */

} /* cerrbd_ */
