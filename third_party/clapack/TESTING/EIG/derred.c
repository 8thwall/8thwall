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
static integer c__6 = 6;
static integer c__8 = 8;
static integer c__3 = 3;
static integer c__5 = 5;

/* Subroutine */ int derred_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a6,\002 passed the tests of the error exit"
	    "s (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a6,\002 failed the tests of the"
	    " error exits ***\002)";

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    doublereal a[16]	/* was [4][4] */;
    logical b[4];
    integer i__, j;
    doublereal s[4], u[16]	/* was [4][4] */, w[16];
    char c2[2];
    doublereal r1[4], r2[4];
    integer iw[8];
    doublereal wi[4];
    integer nt;
    doublereal vl[16]	/* was [4][4] */, vr[16]	/* was [4][4] */, wr[
	    4], vt[16]	/* was [4][4] */;
    integer ihi, ilo, info, sdim;
    extern /* Subroutine */ int dgees_(char *, char *, L_fp, integer *, 
	    doublereal *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, logical *, 
	    integer *), dgeev_(char *, char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *);
    doublereal abnrm;
    extern /* Subroutine */ int dgesdd_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *), dgesvd_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *);
    extern logical dslect_();
    extern /* Subroutine */ int dgeesx_(char *, char *, L_fp, char *, integer 
	    *, doublereal *, integer *, integer *, doublereal *, doublereal *, 
	     doublereal *, integer *, doublereal *, doublereal *, doublereal *
, integer *, integer *, integer *, logical *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int dgeevx_(char *, char *, char *, char *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     doublereal *, integer *, integer *, integer *), chkxer_(char *, integer *, integer *, logical *, 
	    logical *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9998, 0 };
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

/*  DERRED tests the error exits for the eigenvalue driver routines for */
/*  DOUBLE PRECISION matrices: */

/*  PATH  driver   description */
/*  ----  ------   ----------- */
/*  SEV   DGEEV    find eigenvalues/eigenvectors for nonsymmetric A */
/*  SES   DGEES    find eigenvalues/Schur form for nonsymmetric A */
/*  SVX   DGEEVX   SGEEV + balancing and condition estimation */
/*  SSX   DGEESX   SGEES + balancing and condition estimation */
/*  DBD   DGESVD   compute SVD of an M-by-N matrix A */
/*        DGESDD   compute SVD of an M-by-N matrix A (by divide and */
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
	    a[i__ + (j << 2) - 5] = 0.;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	a[i__ + (i__ << 2) - 5] = 1.;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

    if (lsamen_(&c__2, c2, "EV")) {

/*        Test DGEEV */

	s_copy(srnamc_1.srnamt, "DGEEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgeev_("X", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__1, &info);
	chkxer_("DGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgeev_("N", "X", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__1, &info);
	chkxer_("DGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgeev_("N", "N", &c_n1, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__1, &info);
	chkxer_("DGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgeev_("N", "N", &c__2, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__6, &info);
	chkxer_("DGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dgeev_("V", "N", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__8, &info);
	chkxer_("DGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dgeev_("N", "V", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__8, &info);
	chkxer_("DGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dgeev_("V", "V", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__3, &info);
	chkxer_("DGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "ES")) {

/*        Test DGEES */

	s_copy(srnamc_1.srnamt, "DGEES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgees_("X", "N", (L_fp)dslect_, &c__0, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__1, b, &info);
	chkxer_("DGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgees_("N", "X", (L_fp)dslect_, &c__0, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__1, b, &info);
	chkxer_("DGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgees_("N", "S", (L_fp)dslect_, &c_n1, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__1, b, &info);
	chkxer_("DGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dgees_("N", "S", (L_fp)dslect_, &c__2, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__6, b, &info);
	chkxer_("DGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dgees_("V", "S", (L_fp)dslect_, &c__2, a, &c__2, &sdim, wr, wi, vl, &
		c__1, w, &c__6, b, &info);
	chkxer_("DGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dgees_("N", "S", (L_fp)dslect_, &c__1, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__2, b, &info);
	chkxer_("DGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

    } else if (lsamen_(&c__2, c2, "VX")) {

/*        Test DGEEVX */

	s_copy(srnamc_1.srnamt, "DGEEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgeevx_("X", "N", "N", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgeevx_("N", "X", "N", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgeevx_("N", "N", "X", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgeevx_("N", "N", "N", "X", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgeevx_("N", "N", "N", "N", &c_n1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgeevx_("N", "N", "N", "N", &c__2, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dgeevx_("N", "V", "N", "N", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__6, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	dgeevx_("N", "N", "V", "N", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__6, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	dgeevx_("N", "N", "N", "N", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	dgeevx_("N", "V", "N", "N", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__2, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	dgeevx_("N", "N", "V", "V", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__3, iw, &info);
	chkxer_("DGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

    } else if (lsamen_(&c__2, c2, "SX")) {

/*        Test DGEESX */

	s_copy(srnamc_1.srnamt, "DGEESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgeesx_("X", "N", (L_fp)dslect_, "N", &c__0, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("DGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgeesx_("N", "X", (L_fp)dslect_, "N", &c__0, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("DGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgeesx_("N", "N", (L_fp)dslect_, "X", &c__0, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("DGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgeesx_("N", "N", (L_fp)dslect_, "N", &c_n1, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("DGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	dgeesx_("N", "N", (L_fp)dslect_, "N", &c__2, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__6, iw, &c__1, b, &info);
	chkxer_("DGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	dgeesx_("V", "N", (L_fp)dslect_, "N", &c__2, a, &c__2, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__6, iw, &c__1, b, &info);
	chkxer_("DGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	dgeesx_("N", "N", (L_fp)dslect_, "N", &c__1, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__2, iw, &c__1, b, &info);
	chkxer_("DGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "BD")) {

/*        Test DGESVD */

	s_copy(srnamc_1.srnamt, "DGESVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgesvd_("X", "N", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgesvd_("N", "X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgesvd_("O", "O", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgesvd_("N", "N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	dgesvd_("N", "N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	dgesvd_("N", "N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	dgesvd_("A", "N", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &
		c__5, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	dgesvd_("N", "A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, &info);
	chkxer_("DGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;
	if (infoc_1.ok) {
	    io___24.ciunit = infoc_1.nout;
	    s_wsfe(&io___24);
	    do_fio(&c__1, srnamc_1.srnamt, (ftnlen)6);
	    do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___25.ciunit = infoc_1.nout;
	    s_wsfe(&io___25);
	    e_wsfe();
	}

/*        Test DGESDD */

	s_copy(srnamc_1.srnamt, "DGESDD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	dgesdd_("X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 iw, &info);
	chkxer_("DGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	dgesdd_("N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 iw, &info);
	chkxer_("DGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	dgesdd_("N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 iw, &info);
	chkxer_("DGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	dgesdd_("N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 iw, &info);
	chkxer_("DGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	dgesdd_("A", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &c__5, 
		 iw, &info);
	chkxer_("DGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	dgesdd_("A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 iw, &info);
	chkxer_("DGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
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

/*     End of DERRED */
} /* derred_ */
