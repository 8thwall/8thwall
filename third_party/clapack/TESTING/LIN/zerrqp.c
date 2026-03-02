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

/* Subroutine */ int zerrqp_(char *path, integer *nunit)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsle(cilist *), e_wsle(void);

    /* Local variables */
    doublecomplex a[9]	/* was [3][3] */, w[15];
    char c2[2];
    integer ip[3], lw;
    doublereal rw[6];
    doublecomplex tau[3];
    integer info;
    extern /* Subroutine */ int zgeqp3_(integer *, integer *, doublecomplex *, 
	     integer *, integer *, doublecomplex *, doublecomplex *, integer *
, doublereal *, integer *), alaesm_(char *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), zgeqpf_(integer *, integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRQP tests the error exits for ZGEQPF and CGEQP3. */

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
    a[0].r = 1., a[0].i = -1.;
    a[3].r = 2., a[3].i = -2.;
    a[4].r = 3., a[4].i = -3.;
    a[1].r = 4., a[1].i = -4.;
    infoc_1.ok = TRUE_;
    io___4.ciunit = infoc_1.nout;
    s_wsle(&io___4);
    e_wsle();

/*     Test error exits for QR factorization with pivoting */

    if (lsamen_(&c__2, c2, "QP")) {

/*        ZGEQPF */

	s_copy(srnamc_1.srnamt, "ZGEQPF", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgeqpf_(&c_n1, &c__0, a, &c__1, ip, tau, w, rw, &info);
	chkxer_("ZGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgeqpf_(&c__0, &c_n1, a, &c__1, ip, tau, w, rw, &info);
	chkxer_("ZGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgeqpf_(&c__2, &c__0, a, &c__1, ip, tau, w, rw, &info);
	chkxer_("ZGEQPF", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);

/*        ZGEQP3 */

	s_copy(srnamc_1.srnamt, "ZGEQP3", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgeqp3_(&c_n1, &c__0, a, &c__1, ip, tau, w, &lw, rw, &info);
	chkxer_("ZGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgeqp3_(&c__1, &c_n1, a, &c__1, ip, tau, w, &lw, rw, &info);
	chkxer_("ZGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgeqp3_(&c__2, &c__3, a, &c__1, ip, tau, w, &lw, rw, &info);
	chkxer_("ZGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	i__1 = lw - 10;
	zgeqp3_(&c__2, &c__2, a, &c__2, ip, tau, w, &i__1, rw, &info);
	chkxer_("ZGEQP3", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
    }

/*     Print a summary line. */

    alaesm_(path, &infoc_1.ok, &infoc_1.nout);

    return 0;

/*     End of ZERRQP */

} /* zerrqp_ */
