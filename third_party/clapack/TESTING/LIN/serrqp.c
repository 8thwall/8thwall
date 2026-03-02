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
static integer c__3 = 3;

/* Subroutine */ int serrqp_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    real a[9]	/* was [3][3] */, w[10];
    char c2[2];
    integer ip[3], lw;
    real tau[3];
    integer info;
    extern /* Subroutine */ int sgeqp3_(integer *, integer *, real *, integer 
	    *, integer *, real *, real *, integer *, integer *), alaesm_(char 
	    *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sgeqpf_(integer *, integer *, real *, 
	    integer *, integer *, real *, real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRQP tests the error exits for SGEQPF and SGEQP3. */

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
/*     .. Executable Statements .. */

    infoc_1.nout = *nunit;
    io___1.ciunit = infoc_1.nout;
    s_wsle(&io___1);
    e_wsle();
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);
    lw = 10;
    a[0] = 1.f;
    a[3] = 2.f;
    a[4] = 3.f;
    a[1] = 4.f;
    infoc_1.ok = TRUE_;

    if (lsamen_(&c__2, c2, "QP")) {

/*        Test error exits for QR factorization with pivoting */

/*        SGEQPF */

	s_copy(srnamc_1.srnamt, "SGEQPF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgeqpf_(&c_n1, &c__0, a, &c__1, ip, tau, w, &info);
	chkxer_("SGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgeqpf_(&c__0, &c_n1, a, &c__1, ip, tau, w, &info);
	chkxer_("SGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgeqpf_(&c__2, &c__0, a, &c__1, ip, tau, w, &info);
	chkxer_("SGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        SGEQP3 */

	s_copy(srnamc_1.srnamt, "SGEQP3", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgeqp3_(&c_n1, &c__0, a, &c__1, ip, tau, w, &lw, &info);
	chkxer_("SGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgeqp3_(&c__1, &c_n1, a, &c__1, ip, tau, w, &lw, &info);
	chkxer_("SGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgeqp3_(&c__2, &c__3, a, &c__1, ip, tau, w, &lw, &info);
	chkxer_("SGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = lw - 10;
	sgeqp3_(&c__2, &c__2, a, &c__2, ip, tau, w, &i__1, &info);
	chkxer_("SGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of SERRQP */

} /* serrqp_ */
