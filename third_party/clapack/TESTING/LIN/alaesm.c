#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int alaesm_(char *path, logical *ok, integer *nout)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error \002,\002exits ***\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___2 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ALAESM prints a summary of results from one of the -ERR- routines. */

/*  Arguments */
/*  ========= */

/*  PATH    (input) CHARACTER*3 */
/*          The LAPACK path name. */

/*  OK      (input) LOGICAL */
/*          The flag from CHKXER that indicates whether or not the tests */
/*          of error exits passed. */

/*  NOUT    (input) INTEGER */
/*          The unit number on which results are to be printed. */
/*          NOUT >= 0. */

/*  ===================================================================== */

/*     .. Executable Statements .. */

    if (*ok) {
	io___1.ciunit = *nout;
	s_wsfe(&io___1);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    } else {
	io___2.ciunit = *nout;
	s_wsfe(&io___2);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }

    return 0;

/*     End of ALAESM */

} /* alaesm_ */
