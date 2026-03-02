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
    real selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

/* Table of constant values */

static integer c__2 = 2;
static integer c__0 = 0;
static integer c__1 = 1;
static integer c_n1 = -1;
static integer c__4 = 4;
static integer c__5 = 5;

/* Subroutine */ int cerred_(char *path, integer *nunit)
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
    complex a[16]	/* was [4][4] */;
    logical b[4];
    integer i__, j;
    real s[4];
    complex u[16]	/* was [4][4] */, w[16], x[4];
    char c2[2];
    real r1[4], r2[4];
    integer iw[16], nt;
    complex vl[16]	/* was [4][4] */, vr[16]	/* was [4][4] */;
    real rw[20];
    complex vt[16]	/* was [4][4] */;
    integer ihi, ilo, info, sdim;
    extern /* Subroutine */ int cgees_(char *, char *, L_fp, integer *, 
	    complex *, integer *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, real *, logical *, integer *), cgeev_(char *, char *, integer *, complex *, integer *, 
	    complex *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, integer *);
    real abnrm;
    extern /* Subroutine */ int cgesdd_(char *, integer *, integer *, complex 
	    *, integer *, real *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, integer *, integer *), 
	    cgesvd_(char *, char *, integer *, integer *, complex *, integer *
, real *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, integer *);
    extern logical cslect_();
    extern /* Subroutine */ int cgeesx_(char *, char *, L_fp, char *, integer 
	    *, complex *, integer *, integer *, complex *, complex *, integer 
	    *, real *, real *, complex *, integer *, real *, logical *, 
	    integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int cgeevx_(char *, char *, char *, char *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *, integer *, real *, real *, real *
, real *, complex *, integer *, real *, integer *), chkxer_(char *, integer *, integer *, logical *, 
	     logical *);

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

/*  CERRED tests the error exits for the eigenvalue driver routines for */
/*  REAL matrices: */

/*  PATH  driver   description */
/*  ----  ------   ----------- */
/*  CEV   CGEEV    find eigenvalues/eigenvectors for nonsymmetric A */
/*  CES   CGEES    find eigenvalues/Schur form for nonsymmetric A */
/*  CVX   CGEEVX   CGEEV + balancing and condition estimation */
/*  CSX   CGEESX   CGEES + balancing and condition estimation */
/*  CBD   CGESVD   compute SVD of an M-by-N matrix A */
/*        CGESDD   compute SVD of an M-by-N matrix A(by divide and */
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
	    a[i__1].r = 0.f, a[i__1].i = 0.f;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	i__1 = i__ + (i__ << 2) - 5;
	a[i__1].r = 1.f, a[i__1].i = 0.f;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

    if (lsamen_(&c__2, c2, "EV")) {

/*        Test CGEEV */

	s_copy(srnamc_1.srnamt, "CGEEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgeev_("X", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("CGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgeev_("N", "X", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("CGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgeev_("N", "N", &c_n1, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("CGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgeev_("N", "N", &c__2, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__4, 
		rw, &info);
	chkxer_("CGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cgeev_("V", "N", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, w, &c__4, 
		rw, &info);
	chkxer_("CGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgeev_("N", "V", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, w, &c__4, 
		rw, &info);
	chkxer_("CGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cgeev_("V", "V", &c__1, a, &c__1, x, vl, &c__1, vr, &c__1, w, &c__1, 
		rw, &info);
	chkxer_("CGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "ES")) {

/*        Test CGEES */

	s_copy(srnamc_1.srnamt, "CGEES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgees_("X", "N", (L_fp)cslect_, &c__0, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("CGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgees_("N", "X", (L_fp)cslect_, &c__0, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("CGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgees_("N", "S", (L_fp)cslect_, &c_n1, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("CGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cgees_("N", "S", (L_fp)cslect_, &c__2, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__4, rw, b, &info);
	chkxer_("CGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgees_("V", "S", (L_fp)cslect_, &c__2, a, &c__2, &sdim, x, vl, &c__1, 
		w, &c__4, rw, b, &info);
	chkxer_("CGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cgees_("N", "S", (L_fp)cslect_, &c__1, a, &c__1, &sdim, x, vl, &c__1, 
		w, &c__1, rw, b, &info);
	chkxer_("CGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

    } else if (lsamen_(&c__2, c2, "VX")) {

/*        Test CGEEVX */

	s_copy(srnamc_1.srnamt, "CGEEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgeevx_("X", "N", "N", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgeevx_("N", "X", "N", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgeevx_("N", "N", "X", "N", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgeevx_("N", "N", "N", "X", &c__0, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgeevx_("N", "N", "N", "N", &c_n1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgeevx_("N", "N", "N", "N", &c__2, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__4, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgeevx_("N", "V", "N", "N", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__4, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	cgeevx_("N", "N", "V", "N", &c__2, a, &c__2, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__4, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	cgeevx_("N", "N", "N", "N", &c__1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 20;
	cgeevx_("N", "N", "V", "V", &c__1, a, &c__1, x, vl, &c__1, vr, &c__1, 
		&ilo, &ihi, s, &abnrm, r1, r2, w, &c__2, rw, &info);
	chkxer_("CGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 10;

    } else if (lsamen_(&c__2, c2, "SX")) {

/*        Test CGEESX */

	s_copy(srnamc_1.srnamt, "CGEESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgeesx_("X", "N", (L_fp)cslect_, "N", &c__0, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("CGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgeesx_("N", "X", (L_fp)cslect_, "N", &c__0, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("CGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgeesx_("N", "N", (L_fp)cslect_, "X", &c__0, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("CGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgeesx_("N", "N", (L_fp)cslect_, "N", &c_n1, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("CGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	cgeesx_("N", "N", (L_fp)cslect_, "N", &c__2, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__4, rw, b, &info);
	chkxer_("CGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cgeesx_("V", "N", (L_fp)cslect_, "N", &c__2, a, &c__2, &sdim, x, vl, &
		c__1, r1, r2, w, &c__4, rw, b, &info);
	chkxer_("CGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 15;
	cgeesx_("N", "N", (L_fp)cslect_, "N", &c__1, a, &c__1, &sdim, x, vl, &
		c__1, r1, r2, w, &c__1, rw, b, &info);
	chkxer_("CGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "BD")) {

/*        Test CGESVD */

	s_copy(srnamc_1.srnamt, "CGESVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgesvd_("X", "N", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgesvd_("N", "X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgesvd_("O", "O", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgesvd_("N", "N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	cgesvd_("N", "N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	cgesvd_("N", "N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	cgesvd_("A", "N", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &
		c__5, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	cgesvd_("N", "A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, rw, &info);
	chkxer_("CGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*        Test CGESDD */

	s_copy(srnamc_1.srnamt, "CGESDD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	cgesdd_("X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 rw, iw, &info);
	chkxer_("CGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	cgesdd_("N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 rw, iw, &info);
	chkxer_("CGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	cgesdd_("N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 rw, iw, &info);
	chkxer_("CGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	cgesdd_("N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 rw, iw, &info);
	chkxer_("CGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	cgesdd_("A", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &c__5, 
		 rw, iw, &info);
	chkxer_("CGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	cgesdd_("A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 rw, iw, &info);
	chkxer_("CGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of CERRED */

} /* cerred_ */
