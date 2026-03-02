#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int alasmg_(char *type__, integer *nout, integer *nfail, 
	integer *nrun, integer *nerrs)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002: \002,i6,\002 out of \002,i6,\002 "
	    "tests failed to pass the threshold\002)";
    static char fmt_9998[] = "(/1x,\002All tests for \002,a3,\002 routines p"
	    "assed the threshold (\002,i6,\002 tests run)\002)";
    static char fmt_9997[] = "(6x,i6,\002 error messages recorded\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___2 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___3 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ALASMG prints a summary of results from one of the -CHK- routines. */

/*  Arguments */
/*  ========= */

/*  TYPE    (input) CHARACTER*3 */
/*          The LAPACK path name. */

/*  NOUT    (input) INTEGER */
/*          The unit number on which results are to be printed. */
/*          NOUT >= 0. */

/*  NFAIL   (input) INTEGER */
/*          The number of tests which did not pass the threshold ratio. */

/*  NRUN    (input) INTEGER */
/*          The total number of tests. */

/*  NERRS   (input) INTEGER */
/*          The number of error messages recorded. */

/* ====================================================================== */

/*     .. Executable Statements .. */

    if (*nfail > 0) {
	io___1.ciunit = *nout;
	s_wsfe(&io___1);
	do_fio(&c__1, type__, (ftnlen)3);
	do_fio(&c__1, (char *)&(*nfail), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*nrun), (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	io___2.ciunit = *nout;
	s_wsfe(&io___2);
	do_fio(&c__1, type__, (ftnlen)3);
	do_fio(&c__1, (char *)&(*nrun), (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (*nerrs > 0) {
	io___3.ciunit = *nout;
	s_wsfe(&io___3);
	do_fio(&c__1, (char *)&(*nerrs), (ftnlen)sizeof(integer));
	e_wsfe();
    }

    return 0;

/*     End of ALASMG */

} /* alasmg_ */
