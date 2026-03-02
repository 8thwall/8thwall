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
static integer c__6 = 6;
static integer c__8 = 8;
static integer c__3 = 3;
static integer c__5 = 5;

/* Subroutine */ int serred_(char *path, integer *nunit)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002 routines passed the tests of the e"
	    "rror exits (\002,i3,\002 tests done)\002)";
    static char fmt_9998[] = "(\002 *** \002,a3,\002 routines failed the tes"
	    "ts of the error ex\002,\002its ***\002)";

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    real a[16]	/* was [4][4] */;
    logical b[4];
    integer i__, j;
    real s[4], u[16]	/* was [4][4] */, w[16];
    char c2[2];
    real r1[4], r2[4];
    integer iw[8];
    real wi[4];
    integer nt;
    real vl[16]	/* was [4][4] */, vr[16]	/* was [4][4] */, wr[4], vt[
	    16]	/* was [4][4] */;
    integer ihi, ilo, info, sdim;
    real abnrm;
    extern /* Subroutine */ int sgees_(char *, char *, L_fp, integer *, real *
, integer *, integer *, real *, real *, real *, integer *, real *, 
	     integer *, logical *, integer *), sgeev_(char *, 
	    char *, integer *, real *, integer *, real *, real *, real *, 
	    integer *, real *, integer *, real *, integer *, integer *), sgesdd_(char *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    integer *, integer *, integer *);
    extern logical lsamen_(integer *, char *, char *);
    extern /* Subroutine */ int chkxer_(char *, integer *, integer *, logical 
	    *, logical *), sgesvd_(char *, char *, integer *, integer 
	    *, real *, integer *, real *, real *, integer *, real *, integer *
, real *, integer *, integer *);
    extern logical sslect_();
    extern /* Subroutine */ int sgeesx_(char *, char *, L_fp, char *, integer 
	    *, real *, integer *, integer *, real *, real *, real *, integer *
, real *, real *, real *, integer *, integer *, integer *, 
	    logical *, integer *), sgeevx_(char *, 
	    char *, char *, char *, integer *, real *, integer *, real *, 
	    real *, real *, integer *, real *, integer *, integer *, integer *
, real *, real *, real *, real *, real *, integer *, integer *, 
	    integer *);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9998, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SERRED tests the error exits for the eigenvalue driver routines for */
/*  REAL matrices: */

/*  PATH  driver   description */
/*  ----  ------   ----------- */
/*  SEV   SGEEV    find eigenvalues/eigenvectors for nonsymmetric A */
/*  SES   SGEES    find eigenvalues/Schur form for nonsymmetric A */
/*  SVX   SGEEVX   SGEEV + balancing and condition estimation */
/*  SSX   SGEESX   SGEES + balancing and condition estimation */
/*  SBD   SGESVD   compute SVD of an M-by-N matrix A */
/*        SGESDD   compute SVD of an M-by-N matrix A (by divide and */
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
	    a[i__ + (j << 2) - 5] = 0.f;
/* L10: */
	}
/* L20: */
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	a[i__ + (i__ << 2) - 5] = 1.f;
/* L30: */
    }
    infoc_1.ok = TRUE_;
    nt = 0;

    if (lsamen_(&c__2, c2, "EV")) {

/*        Test SGEEV */

	s_copy(srnamc_1.srnamt, "SGEEV ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgeev_("X", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__1, &info);
	chkxer_("SGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgeev_("N", "X", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__1, &info);
	chkxer_("SGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgeev_("N", "N", &c_n1, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__1, &info);
	chkxer_("SGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgeev_("N", "N", &c__2, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__6, &info);
	chkxer_("SGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sgeev_("V", "N", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__8, &info);
	chkxer_("SGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sgeev_("N", "V", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__8, &info);
	chkxer_("SGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sgeev_("V", "V", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &c__1, w, &
		c__3, &info);
	chkxer_("SGEEV ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "ES")) {

/*        Test SGEES */

	s_copy(srnamc_1.srnamt, "SGEES ", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgees_("X", "N", (L_fp)sslect_, &c__0, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__1, b, &info);
	chkxer_("SGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgees_("N", "X", (L_fp)sslect_, &c__0, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__1, b, &info);
	chkxer_("SGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgees_("N", "S", (L_fp)sslect_, &c_n1, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__1, b, &info);
	chkxer_("SGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgees_("N", "S", (L_fp)sslect_, &c__2, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__6, b, &info);
	chkxer_("SGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sgees_("V", "S", (L_fp)sslect_, &c__2, a, &c__2, &sdim, wr, wi, vl, &
		c__1, w, &c__6, b, &info);
	chkxer_("SGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sgees_("N", "S", (L_fp)sslect_, &c__1, a, &c__1, &sdim, wr, wi, vl, &
		c__1, w, &c__2, b, &info);
	chkxer_("SGEES ", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;

    } else if (lsamen_(&c__2, c2, "VX")) {

/*        Test SGEEVX */

	s_copy(srnamc_1.srnamt, "SGEEVX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgeevx_("X", "N", "N", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgeevx_("N", "X", "N", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgeevx_("N", "N", "X", "N", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgeevx_("N", "N", "N", "X", &c__0, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgeevx_("N", "N", "N", "N", &c_n1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgeevx_("N", "N", "N", "N", &c__2, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sgeevx_("N", "V", "N", "N", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__6, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 13;
	sgeevx_("N", "N", "V", "N", &c__2, a, &c__2, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__6, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	sgeevx_("N", "N", "N", "N", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__1, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	sgeevx_("N", "V", "N", "N", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__2, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 21;
	sgeevx_("N", "N", "V", "V", &c__1, a, &c__1, wr, wi, vl, &c__1, vr, &
		c__1, &ilo, &ihi, s, &abnrm, r1, r2, w, &c__3, iw, &info);
	chkxer_("SGEEVX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 11;

    } else if (lsamen_(&c__2, c2, "SX")) {

/*        Test SGEESX */

	s_copy(srnamc_1.srnamt, "SGEESX", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgeesx_("X", "N", (L_fp)sslect_, "N", &c__0, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("SGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgeesx_("N", "X", (L_fp)sslect_, "N", &c__0, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("SGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgeesx_("N", "N", (L_fp)sslect_, "X", &c__0, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("SGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgeesx_("N", "N", (L_fp)sslect_, "N", &c_n1, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__1, iw, &c__1, b, &info);
	chkxer_("SGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 7;
	sgeesx_("N", "N", (L_fp)sslect_, "N", &c__2, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__6, iw, &c__1, b, &info);
	chkxer_("SGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 12;
	sgeesx_("V", "N", (L_fp)sslect_, "N", &c__2, a, &c__2, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__6, iw, &c__1, b, &info);
	chkxer_("SGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 16;
	sgeesx_("N", "N", (L_fp)sslect_, "N", &c__1, a, &c__1, &sdim, wr, wi, 
		vl, &c__1, r1, r2, w, &c__2, iw, &c__1, b, &info);
	chkxer_("SGEESX", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 7;

    } else if (lsamen_(&c__2, c2, "BD")) {

/*        Test SGESVD */

	s_copy(srnamc_1.srnamt, "SGESVD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgesvd_("X", "N", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgesvd_("N", "X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgesvd_("O", "O", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgesvd_("N", "N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 4;
	sgesvd_("N", "N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__1, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 6;
	sgesvd_("N", "N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 9;
	sgesvd_("A", "N", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &
		c__5, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 11;
	sgesvd_("N", "A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &
		c__5, &info);
	chkxer_("SGESVD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 8;

/*        Test SGESDD */

	s_copy(srnamc_1.srnamt, "SGESDD", (ftnlen)6, (ftnlen)6);
	infoc_1.infot = 1;
	sgesdd_("X", &c__0, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 iw, &info);
	chkxer_("SGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 2;
	sgesdd_("N", &c_n1, &c__0, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 iw, &info);
	chkxer_("SGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 3;
	sgesdd_("N", &c__0, &c_n1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__1, 
		 iw, &info);
	chkxer_("SGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 5;
	sgesdd_("N", &c__2, &c__1, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 iw, &info);
	chkxer_("SGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 8;
	sgesdd_("A", &c__2, &c__1, a, &c__2, s, u, &c__1, vt, &c__1, w, &c__5, 
		 iw, &info);
	chkxer_("SGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	infoc_1.infot = 10;
	sgesdd_("A", &c__1, &c__2, a, &c__1, s, u, &c__1, vt, &c__1, w, &c__5, 
		 iw, &info);
	chkxer_("SGESDD", &infoc_1.infot, &infoc_1.nout, &infoc_1.lerr, &
		infoc_1.ok);
	nt += 6;
    }

/*     Print a summary line. */

    if (! lsamen_(&c__2, c2, "BD")) {
	if (infoc_1.ok) {
	    io___24.ciunit = infoc_1.nout;
	    s_wsfe(&io___24);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, (char *)&nt, (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___25.ciunit = infoc_1.nout;
	    s_wsfe(&io___25);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
	}
    }

    return 0;

/*     End of SERRED */

} /* serred_ */
