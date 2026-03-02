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

/* Subroutine */ int cerrqp_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), e_wsle(void);

    /* Local variables */
    complex a[9]	/* was [3][3] */, w[15];
    char c2[2];
    integer ip[3], lw;
    real rw[6];
    complex tau[3];
    integer info;
    extern /* Subroutine */ int cgeqp3_(integer *, integer *, complex *, 
	    integer *, integer *, complex *, complex *, integer *, real *, 
	    integer *), alaesm_(char *, logical *, integer *), 
	    cgeqpf_(integer *, integer *, complex *, integer *, integer *, 
	    complex *, complex *, real *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CERRQP tests the error exits for CGEQPF and CGEQP3. */

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
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);
    lw = 4;
    a[0].r = 1.f, a[0].i = -1.f;
    a[3].r = 2.f, a[3].i = -2.f;
    a[4].r = 3.f, a[4].i = -3.f;
    a[1].r = 4.f, a[1].i = -4.f;
    infoc_1.ok = TRUE_;
    io___4.ciunit = infoc_1.nout;
    s_wsle(&io___4);
    e_wsle();

/*     Test error exits for QR factorization with pivoting */

    if (lsamen_(&c__2, c2, "QP")) {

/*        CGEQPF */

	s_copy(srnamc_1.srnamt, "CGEQPF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgeqpf_(&c_n1, &c__0, a, &c__1, ip, tau, w, rw, &info);
	chkxer_("CGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgeqpf_(&c__0, &c_n1, a, &c__1, ip, tau, w, rw, &info);
	chkxer_("CGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgeqpf_(&c__2, &c__0, a, &c__1, ip, tau, w, rw, &info);
	chkxer_("CGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        CGEQP3 */

	s_copy(srnamc_1.srnamt, "CGEQP3", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgeqp3_(&c_n1, &c__0, a, &c__1, ip, tau, w, &lw, rw, &info);
	chkxer_("CGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgeqp3_(&c__1, &c_n1, a, &c__1, ip, tau, w, &lw, rw, &info);
	chkxer_("CGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgeqp3_(&c__2, &c__3, a, &c__1, ip, tau, w, &lw, rw, &info);
	chkxer_("CGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = lw - 10;
	cgeqp3_(&c__2, &c__2, a, &c__2, ip, tau, w, &i__1, rw, &info);
	chkxer_("CGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of CERRQP */

} /* cerrqp_ */
