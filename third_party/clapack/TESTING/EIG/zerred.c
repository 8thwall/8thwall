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

struct {
    integer selopt, seldim;
    logical selval[20];
    doublereal selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

/* Table of constant values */

static integer c__2 = 2;
static integer c__0 = 0;
static integer c__1 = 1;
static integer c_n1 = -1;
static integer c__4 = 4;
static integer c__5 = 5;

/* Subroutine */ int zerred_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002 passed the tests of the error exit"
	    "s (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a6,\002 failed the tests of the"
	    " error exits ***\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublecomplex a[16]	/* was [4][4] */;
    logical b[4];
    integer i__, j;
    doublereal s[4];
    doublecomplex u[16]	/* was [4][4] */, w[16], x[4];
    char c2[2];
    doublereal r1[4], r2[4];
    integer iw[16], nt;
    doublecomplex vl[16]	/* was [4][4] */, vr[16]	/* was [4][4] 
	    */;
    doublereal rw[20];
    doublecomplex vt[16]	/* was [4][4] */;
    integer ihi, ilo, info, sdim;
    doublereal abnrm;
    extern /* Subroutine */ int zgees_(char *, char *, L_fp, integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, logical *, integer *), zgeev_(char *
, char *, integer *, doublecomplex *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int zgesdd_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublereal *, integer *, integer *), chkxer_(char *, 
	    integer *, integer *, logical *, logical *), zgesvd_(char 
	    *, char *, integer *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublecomplex *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, integer *);
    extern logical zslect_();
    extern /* Subroutine */ int zgeesx_(char *, char *, L_fp, char *, integer 
	    *, doublecomplex *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, integer *, doublereal *, logical *, integer *), zgeevx_(char *, char *, char *, char *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, integer *, 
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
, doublecomplex *, integer *, doublereal *, integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZERRED tests the error exits for the eigenvalue driver routines for */
/*  DOUBLE PRECISION matrices: */

/*  PATH  driver   description */
/*  ----  ------   ----------- */
/*  ZEV   ZGEEV    find eigenvalues/eigenvectors for nonsymmetric A */
/*  ZES   ZGEES    find eigenvalues/Schur form for nonsymmetric A */
/*  ZVX   ZGEEVX   ZGEEV + balancing and condition estimation */
/*  ZSX   ZGEESX   ZGEES + balancing and condition estimation */
/*  ZBD   ZGESVD   compute SVD of an M-by-N matrix A */
/*        ZGESDD   compute SVD of an M-by-N matrix A(by divide and */
/*                 conquer) */

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
/*     .. External Functions .. */
/*     .. */
/*     .. Arrays in Common .. */
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

/*     Initialize A */

    for (j = 1; j <= 4; ++j) {
	for (i__ = 1; i__ <= 4; ++i__) {
	    i__1 = i__ + (j << 2) - 5;
	    a[i__1].r = 0., a[i__1].i = 0.;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	i__1 = i__ + (i__ << 2) - 5;
	a[i__1].r = 1., a[i__1].i = 0.;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

    if (lsamen_(&c__2, c2, "EV")) {

/*        Test ZGEEV */

	s_copy(srnamc_1.srnamt, "ZGEEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgeev_("X", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("ZGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgeev_("N", "X", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("ZGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgeev_("N", "N", &c_n1, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("ZGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgeev_("N", "N", &c__2, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__4, 
		rw, &info);
	chkxer_("ZGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zgeev_("V", "N", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, w, &c__4, 
		rw, &info);
	chkxer_("ZGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgeev_("N", "V", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, w, &c__4, 
		rw, &info);
	chkxer_("ZGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgeev_("V", "V", &c__1, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("ZGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "ES")) {

/*        Test ZGEES */

	s_copy(srnamc_1.srnamt, "ZGEES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgees_("X", "N", (L_fp)zslect_, &c__0, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("ZGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgees_("N", "X", (L_fp)zslect_, &c__0, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("ZGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgees_("N", "S", (L_fp)zslect_, &c_n1, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("ZGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgees_("N", "S", (L_fp)zslect_, &c__2, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__4, rw, b, &info);
	chkxer_("ZGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgees_("V", "S", (L_fp)zslect_, &c__2, a, &c__2, &sdim, x, vl, &c__1, 
		w, &c__4, rw, b, &info);
	chkxer_("ZGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgees_("N", "S", (L_fp)zslect_, &c__1, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("ZGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

    } else if (lsamen_(&c__2, c2, "VX")) {

/*        Test ZGEEVX */

	s_copy(srnamc_1.srnamt, "ZGEEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgeevx_("X", "N", "N", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgeevx_("N", "X", "N", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgeevx_("N", "N", "X", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgeevx_("N", "N", "N", "X", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgeevx_("N", "N", "N", "N", &c_n1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgeevx_("N", "N", "N", "N", &c__2, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgeevx_("N", "V", "N", "N", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	zgeevx_("N", "N", "V", "N", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__4, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	zgeevx_("N", "N", "N", "N", &c__1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	zgeevx_("N", "N", "V", "V", &c__1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__2, rw, &info);
	chkxer_("ZGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

    } else if (lsamen_(&c__2, c2, "SX")) {

/*        Test ZGEESX */

	s_copy(srnamc_1.srnamt, "ZGEESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgeesx_("X", "N", (L_fp)zslect_, "N", &c__0, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("ZGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgeesx_("N", "X", (L_fp)zslect_, "N", &c__0, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("ZGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgeesx_("N", "N", (L_fp)zslect_, "X", &c__0, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("ZGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgeesx_("N", "N", (L_fp)zslect_, "N", &c_n1, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("ZGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	zgeesx_("N", "N", (L_fp)zslect_, "N", &c__2, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__4, rw, b, &info);
	chkxer_("ZGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zgeesx_("V", "N", (L_fp)zslect_, "N", &c__2, a, &c__2, &sdim, x, vl, &
		c__1, r1, r2, w, &c__4, rw, b, &info);
	chkxer_("ZGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	zgeesx_("N", "N", (L_fp)zslect_, "N", &c__1, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("ZGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "BD")) {

/*        Test ZGESVD */

	s_copy(srnamc_1.srnamt, "ZGESVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgesvd_("X", "N", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgesvd_("N", "X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgesvd_("O", "O", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgesvd_("N", "N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	zgesvd_("N", "N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	zgesvd_("N", "N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	zgesvd_("A", "N", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &
		c__5, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	zgesvd_("N", "A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, rw, &info);
	chkxer_("ZGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;
	if (infoc_1.ok) {
	    io___23.ciunit = infoc_1.nout;
	    s_wsfe(&io___23);
	    do_fio(&c__1, srnamc_1.srnamt, (ftnlen)6);
	    do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___24.ciunit = infoc_1.nout;
	    s_wsfe(&io___24);
	    e_wsfe();
	}

/*        Test ZGESDD */

	s_copy(srnamc_1.srnamt, "ZGESDD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	zgesdd_("X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 rw, iw, &info);
	chkxer_("ZGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	zgesdd_("N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 rw, iw, &info);
	chkxer_("ZGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	zgesdd_("N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 rw, iw, &info);
	chkxer_("ZGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	zgesdd_("N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 rw, iw, &info);
	chkxer_("ZGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	zgesdd_("A", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &c__5, 
		 rw, iw, &info);
	chkxer_("ZGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	zgesdd_("A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 rw, iw, &info);
	chkxer_("ZGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += -2;
	if (infoc_1.ok) {
	    io___26.ciunit = infoc_1.nout;
	    s_wsfe(&io___26);
	    do_fio(&c__1, srnamc_1.srnamt, (ftnlen)6);
	    do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___27.ciunit = infoc_1.nout;
	    s_wsfe(&io___27);
	    e_wsfe();
	}
    }

/*     Print a summary line. */

    if (! lsamen_(&c__2, c2, "BD")) {
	if (infoc_1.ok) {
	    io___28.ciunit = infoc_1.nout;
	    s_wsfe(&io___28);
	    do_fio(&c__1, srnamc_1.srnamt, (ftnlen)6);
	    do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___29.ciunit = infoc_1.nout;
	    s_wsfe(&io___29);
	    e_wsfe();
	}
    }

    return 0;

/*     End of ZERRED */

} /* zerred_ */
