#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int slasum_(char *type__, integer *iounit, integer *ie, 
	integer *nrun)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,a2,i4,a8,i5,a35)";
    static char fmt_9998[] = "(/1x,a14,a3,a23,i5,a11)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___2 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK auxiliary test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLASUM prints a summary of the results from one of the test routines. */

/*  ===================================================================== */

/*     .. Executable Statements .. */

    if (*ie > 0) {
	io___1.ciunit = *iounit;
	s_wsfe(&io___1);
	do_fio(&c__1, type__, (ftnlen)3);
	do_fio(&c__1, ": ", (ftnlen)2);
	do_fio(&c__1, (char *)&(*ie), (ftnlen)sizeof(integer));
	do_fio(&c__1, " out of ", (ftnlen)8);
	do_fio(&c__1, (char *)&(*nrun), (ftnlen)sizeof(integer));
	do_fio(&c__1, " tests failed to pass the threshold", (ftnlen)35);
	e_wsfe();
    } else {
	io___2.ciunit = *iounit;
	s_wsfe(&io___2);
	do_fio(&c__1, "All tests for ", (ftnlen)14);
	do_fio(&c__1, type__, (ftnlen)3);
	do_fio(&c__1, " passed the threshold (", (ftnlen)23);
	do_fio(&c__1, (char *)&(*nrun), (ftnlen)sizeof(integer));
	do_fio(&c__1, " tests run)", (ftnlen)11);
	e_wsfe();
    }
    return 0;

/*     End of SLASUM */

} /* slasum_ */
