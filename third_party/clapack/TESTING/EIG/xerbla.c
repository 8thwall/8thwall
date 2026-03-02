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

/* Subroutine */ int xerbla_(char *srname, integer *info)
{
    /* Format strings */
    static char fmt_9999[] = "(\002 *** XERBLA was called from \002,a6,\002 "
	    "with INFO = \002,i6,\002 instead of \002,i2,\002 ***\002)";
    static char fmt_9997[] = "(\002 *** On entry to \002,a6,\002 parameter n"
	    "umber \002,i6,\002 had an illegal value ***\002)";
    static char fmt_9998[] = "(\002 *** XERBLA was called with SRNAME = \002"
	    ",a6,\002 instead of \002,a6,\002 ***\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void),
	     s_cmp(char *, char *, ftnlen, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___2 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___3 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This is a special version of XERBLA to be used only as part of */
/*  the test program for testing error exits from the LAPACK routines. */
/*  Error messages are printed if INFO.NE.INFOT or if SRNAME.NE.SRNAMT, */
/*  where INFOT and SRNAMT are values stored in COMMON. */

/*  Arguments */
/*  ========= */

/*  SRNAME  (input) CHARACTER*6 */
/*          The name of the subroutine calling XERBLA.  This name should */
/*          match the COMMON variable SRNAMT. */

/*  INFO    (input) INTEGER */
/*          The error return code from the calling subroutine.  INFO */
/*          should equal the COMMON variable INFOT. */

/*  Further Details */
/*  ======= ======= */

/*  The following variables are passed via the common blocks INFOC and */
/*  SRNAMC: */

/*  INFOT   INTEGER      Expected integer return code */
/*  NOUT    INTEGER      Unit number for printing error messages */
/*  OK      LOGICAL      Set to .TRUE. if INFO = INFOT and */
/*                       SRNAME = SRNAMT, otherwise set to .FALSE. */
/*  LERR    LOGICAL      Set to .TRUE., indicating that XERBLA was called */
/*  SRNAMT  CHARACTER*6  Expected name of calling subroutine */


/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Executable Statements .. */

    infoc_1.lerr = TRUE_;
    if (*info != infoc_1.infot) {
	if (infoc_1.infot != 0) {
	    io___1.ciunit = infoc_1.nout;
	    s_wsfe(&io___1);
	    do_fio(&c__1, srnamc_1.srnamt, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&infoc_1.infot, (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___2.ciunit = infoc_1.nout;
	    s_wsfe(&io___2);
	    do_fio(&c__1, srname, (ftnlen)6);
	    do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	infoc_1.ok = FALSE_;
    }
    if (s_cmp(srname, srnamc_1.srnamt, (ftnlen)6, (ftnlen)6) != 0) {
	io___3.ciunit = infoc_1.nout;
	s_wsfe(&io___3);
	do_fio(&c__1, srname, (ftnlen)6);
	do_fio(&c__1, srnamc_1.srnamt, (ftnlen)6);
	e_wsfe();
	infoc_1.ok = FALSE_;
    }
    return 0;


/*     End of XERBLA */

} /* xerbla_ */
