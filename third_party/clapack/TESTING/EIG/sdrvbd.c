#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer infot, nunit;
    logical ok, lerr;
} infoc_;

#define infoc_1 infoc_

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static real c_b13 = 0.f;
static real c_b17 = 1.f;
static integer c__4 = 4;
static integer c__1 = 1;
static integer c__0 = 0;

/* Subroutine */ int sdrvbd_(integer *nsizes, integer *mm, integer *nn, 
	integer *ntypes, logical *dotype, integer *iseed, real *thresh, real *
	a, integer *lda, real *u, integer *ldu, real *vt, integer *ldvt, real 
	*asav, real *usav, real *vtsav, real *s, real *ssav, real *e, real *
	work, integer *lwork, integer *iwork, integer *nout, integer *info)
{
    /* Initialized data */

    static char cjob[1*4] = "N" "O" "S" "A";

    /* Format strings */
    static char fmt_9996[] = "(\002 SDRVBD: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002M=\002,i6,\002, N=\002,i6,\002, JTYPE=\002,i"
	    "6,\002, ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9995[] = "(\002 SDRVBD: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002M=\002,i6,\002, N=\002,i6,\002, JTYPE=\002,i"
	    "6,\002, LSWORK=\002,i6,/9x,\002ISEED=(\002,3(i5,\002,\002),i5"
	    ",\002)\002)";
    static char fmt_9999[] = "(\002 SVD -- Real Singular Value Decomposition"
	    " Driver \002,/\002 Matrix types (see SDRVBD for details):\002,/"
	    "/\002 1 = Zero matrix\002,/\002 2 = Identity matrix\002,/\002 3 "
	    "= Evenly spaced singular values near 1\002,/\002 4 = Evenly spac"
	    "ed singular values near underflow\002,/\002 5 = Evenly spaced si"
	    "ngular values near overflow\002,//\002 Tests performed: ( A is d"
	    "ense, U and V are orthogonal,\002,/19x,\002 S is an array, and U"
	    "partial, VTpartial, and\002,/19x,\002 Spartial are partially com"
	    "puted U, VT and S),\002,/)";
    static char fmt_9998[] = "(\002 1 = | A - U diag(S) VT | / ( |A| max(M,N"
	    ") ulp ) \002,/\002 2 = | I - U**T U | / ( M ulp ) \002,/\002 3 ="
	    " | I - VT VT**T | / ( N ulp ) \002,/\002 4 = 0 if S contains min"
	    "(M,N) nonnegative values in\002,\002 decreasing order, else 1/ulp"
	    "\002,/\002 5 = | U - Upartial | / ( M ulp )\002,/\002 6 = | VT -"
	    " VTpartial | / ( N ulp )\002,/\002 7 = | S - Spartial | / ( min("
	    "M,N) ulp |S| )\002,/\002 8 = | A - U diag(S) VT | / ( |A| max(M,"
	    "N) ulp ) \002,/\002 9 = | I - U**T U | / ( M ulp ) \002,/\00210 "
	    "= | I - VT VT**T | / ( N ulp ) \002,/\00211 = 0 if S contains mi"
	    "n(M,N) nonnegative values in\002,\002 decreasing order, else 1/u"
	    "lp\002,/\00212 = | U - Upartial | / ( M ulp )\002,/\00213 = | VT"
	    " - VTpartial | / ( N ulp )\002,/\00214 = | S - Spartial | / ( mi"
	    "n(M,N) ulp |S| )\002,//)";
    static char fmt_9997[] = "(\002 M=\002,i5,\002, N=\002,i5,\002, type "
	    "\002,i1,\002, IWS=\002,i1,\002, seed=\002,4(i4,\002,\002),\002 t"
	    "est(\002,i2,\002)=\002,g11.4)";

    /* System generated locals */
    integer a_dim1, a_offset, asav_dim1, asav_offset, u_dim1, u_offset, 
	    usav_dim1, usav_offset, vt_dim1, vt_offset, vtsav_dim1, 
	    vtsav_offset, i__1, i__2, i__3, i__4, i__5, i__6, i__7, i__8, 
	    i__9, i__10, i__11, i__12, i__13, i__14;
    real r__1, r__2, r__3;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, m, n;
    real dif, div;
    integer ijq, iju;
    real ulp;
    integer iws;
    char jobq[1], path[3], jobu[1];
    integer mmax, nmax;
    real unfl, ovfl;
    integer ijvt;
    logical badmm, badnn;
    integer nfail;
    extern /* Subroutine */ int sbdt01_(integer *, integer *, integer *, real 
	    *, integer *, real *, integer *, real *, real *, real *, integer *
, real *, real *);
    integer iinfo;
    real anorm;
    integer mnmin, mnmax;
    char jobvt[1];
    integer jsize;
    extern /* Subroutine */ int sort01_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *, real *), sort03_(char *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    real *, integer *, real *, integer *, real *, integer *);
    integer jtype, ntest, iwtmp;
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int sgesdd_(char *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    integer *, integer *, integer *), xerbla_(char *, integer 
	    *);
    integer ioldsd[4];
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *), sgesvd_(char *, char *, integer *, integer 
	    *, real *, integer *, real *, real *, integer *, real *, integer *
, real *, integer *, integer *), slacpy_(char *, 
	    integer *, integer *, real *, integer *, real *, integer *), slaset_(char *, integer *, integer *, real *, real *, 
	    real *, integer *), slatms_(integer *, integer *, char *, 
	    integer *, char *, real *, integer *, real *, real *, integer *, 
	    integer *, char *, real *, integer *, real *, integer *);
    integer minwrk;
    real ulpinv, result[14];
    integer lswork, mtypes;

    /* Fortran I/O blocks */
    static cilist io___25 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SDRVBD checks the singular value decomposition (SVD) drivers */
/*  SGESVD and SGESDD. */
/*  Both SGESVD and SGESDD factor A = U diag(S) VT, where U and VT are */
/*  orthogonal and diag(S) is diagonal with the entries of the array S */
/*  on its diagonal. The entries of S are the singular values, */
/*  nonnegative and stored in decreasing order.  U and VT can be */
/*  optionally not computed, overwritten on A, or computed partially. */

/*  A is M by N. Let MNMIN = min( M, N ). S has dimension MNMIN. */
/*  U can be M by M or M by MNMIN. VT can be N by N or MNMIN by N. */

/*  When SDRVBD is called, a number of matrix "sizes" (M's and N's) */
/*  and a number of matrix "types" are specified.  For each size (M,N) */
/*  and each type of matrix, and for the minimal workspace as well as */
/*  workspace adequate to permit blocking, an  M x N  matrix "A" will be */
/*  generated and used to test the SVD routines.  For each matrix, A will */
/*  be factored as A = U diag(S) VT and the following 12 tests computed: */

/*  Test for SGESVD: */

/*  (1)    | A - U diag(S) VT | / ( |A| max(M,N) ulp ) */

/*  (2)    | I - U'U | / ( M ulp ) */

/*  (3)    | I - VT VT' | / ( N ulp ) */

/*  (4)    S contains MNMIN nonnegative values in decreasing order. */
/*         (Return 0 if true, 1/ULP if false.) */

/*  (5)    | U - Upartial | / ( M ulp ) where Upartial is a partially */
/*         computed U. */

/*  (6)    | VT - VTpartial | / ( N ulp ) where VTpartial is a partially */
/*         computed VT. */

/*  (7)    | S - Spartial | / ( MNMIN ulp |S| ) where Spartial is the */
/*         vector of singular values from the partial SVD */

/*  Test for SGESDD: */

/*  (8)    | A - U diag(S) VT | / ( |A| max(M,N) ulp ) */

/*  (9)    | I - U'U | / ( M ulp ) */

/*  (10)   | I - VT VT' | / ( N ulp ) */

/*  (11)   S contains MNMIN nonnegative values in decreasing order. */
/*         (Return 0 if true, 1/ULP if false.) */

/*  (12)   | U - Upartial | / ( M ulp ) where Upartial is a partially */
/*         computed U. */

/*  (13)   | VT - VTpartial | / ( N ulp ) where VTpartial is a partially */
/*         computed VT. */

/*  (14)   | S - Spartial | / ( MNMIN ulp |S| ) where Spartial is the */
/*         vector of singular values from the partial SVD */

/*  The "sizes" are specified by the arrays MM(1:NSIZES) and */
/*  NN(1:NSIZES); the value of each element pair (MM(j),NN(j)) */
/*  specifies one size.  The "types" are specified by a logical array */
/*  DOTYPE( 1:NTYPES ); if DOTYPE(j) is .TRUE., then matrix type "j" */
/*  will be generated. */
/*  Currently, the list of possible types is: */

/*  (1)  The zero matrix. */
/*  (2)  The identity matrix. */
/*  (3)  A matrix of the form  U D V, where U and V are orthogonal and */
/*       D has evenly spaced entries 1, ..., ULP with random signs */
/*       on the diagonal. */
/*  (4)  Same as (3), but multiplied by the underflow-threshold / ULP. */
/*  (5)  Same as (3), but multiplied by the overflow-threshold * ULP. */

/*  Arguments */
/*  ========== */

/*  NSIZES  (input) INTEGER */
/*          The number of matrix sizes (M,N) contained in the vectors */
/*          MM and NN. */

/*  MM      (input) INTEGER array, dimension (NSIZES) */
/*          The values of the matrix row dimension M. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          The values of the matrix column dimension N. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, SDRVBD */
/*          does nothing.  It must be at least zero.  If it is MAXTYP+1 */
/*          and NSIZES is 1, then an additional type, MAXTYP+1 is */
/*          defined, which is to use whatever matrices are in A and B. */
/*          This is only useful if DOTYPE(1:MAXTYP) is .FALSE. and */
/*          DOTYPE(MAXTYP+1) is .TRUE. . */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          If DOTYPE(j) is .TRUE., then for each size (m,n), a matrix */
/*          of type j will be generated.  If NTYPES is smaller than the */
/*          maximum number of types defined (PARAMETER MAXTYP), then */
/*          types NTYPES+1 through MAXTYP will not be generated.  If */
/*          NTYPES is larger than MAXTYP, DOTYPE(MAXTYP+1) through */
/*          DOTYPE(NTYPES) will be ignored. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator.  The array */
/*          elements should be between 0 and 4095; if not they will be */
/*          reduced mod 4096.  Also, ISEED(4) must be odd. */
/*          On exit, ISEED is changed and can be used in the next call to */
/*          SDRVBD to continue the same random number sequence. */

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  The test */
/*          ratios are scaled to be O(1), so THRESH should be a small */
/*          multiple of 1, e.g., 10 or 100.  To have every test ratio */
/*          printed, use THRESH = 0. */

/*  A       (workspace) REAL array, dimension (LDA,NMAX) */
/*          where NMAX is the maximum value of N in NN. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,MMAX), */
/*          where MMAX is the maximum value of M in MM. */

/*  U       (workspace) REAL array, dimension (LDU,MMAX) */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= max(1,MMAX). */

/*  VT      (workspace) REAL array, dimension (LDVT,NMAX) */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of the array VT.  LDVT >= max(1,NMAX). */

/*  ASAV    (workspace) REAL array, dimension (LDA,NMAX) */

/*  USAV    (workspace) REAL array, dimension (LDU,MMAX) */

/*  VTSAV   (workspace) REAL array, dimension (LDVT,NMAX) */

/*  S       (workspace) REAL array, dimension */
/*                      (max(min(MM,NN))) */

/*  SSAV    (workspace) REAL array, dimension */
/*                      (max(min(MM,NN))) */

/*  E       (workspace) REAL array, dimension */
/*                      (max(min(MM,NN))) */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          max(3*MN+MX,5*MN-4)+2*MN**2 for all pairs */
/*          pairs  (MN,MX)=( min(MM(j),NN(j), max(MM(j),NN(j)) ) */

/*  IWORK   (workspace) INTEGER array, dimension at least 8*min(M,N) */

/*  NOUT    (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns IINFO not equal to 0.) */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some MM(j) < 0 */
/*           -3: Some NN(j) < 0 */
/*           -4: NTYPES < 0 */
/*           -7: THRESH < 0 */
/*          -10: LDA < 1 or LDA < MMAX, where MMAX is max( MM(j) ). */
/*          -12: LDU < 1 or LDU < MMAX. */
/*          -14: LDVT < 1 or LDVT < NMAX, where NMAX is max( NN(j) ). */
/*          -21: LWORK too small. */
/*          If  SLATMS, or SGESVD returns an error code, the */
/*              absolute value of it is returned. */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    --mm;
    --nn;
    --dotype;
    --iseed;
    asav_dim1 = *lda;
    asav_offset = 1 + asav_dim1;
    asav -= asav_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    usav_dim1 = *ldu;
    usav_offset = 1 + usav_dim1;
    usav -= usav_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    vtsav_dim1 = *ldvt;
    vtsav_offset = 1 + vtsav_dim1;
    vtsav -= vtsav_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --s;
    --ssav;
    --e;
    --work;
    --iwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

    *info = 0;
    badmm = FALSE_;
    badnn = FALSE_;
    mmax = 1;
    nmax = 1;
    mnmax = 1;
    minwrk = 1;
    i__1 = *nsizes;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__2 = mmax, i__3 = mm[j];
	mmax = max(i__2,i__3);
	if (mm[j] < 0) {
	    badmm = TRUE_;
	}
/* Computing MAX */
	i__2 = nmax, i__3 = nn[j];
	nmax = max(i__2,i__3);
	if (nn[j] < 0) {
	    badnn = TRUE_;
	}
/* Computing MAX */
/* Computing MIN */
	i__4 = mm[j], i__5 = nn[j];
	i__2 = mnmax, i__3 = min(i__4,i__5);
	mnmax = max(i__2,i__3);
/* Computing MAX */
/* Computing MAX */
/* Computing MIN */
	i__6 = mm[j], i__7 = nn[j];
/* Computing MAX */
	i__8 = mm[j], i__9 = nn[j];
/* Computing MIN */
	i__10 = mm[j], i__11 = nn[j] - 4;
	i__4 = min(i__6,i__7) * 3 + max(i__8,i__9), i__5 = min(i__10,i__11) * 
		5;
/* Computing MIN */
	i__13 = mm[j], i__14 = nn[j];
/* Computing 2nd power */
	i__12 = min(i__13,i__14);
	i__2 = minwrk, i__3 = max(i__4,i__5) + (i__12 * i__12 << 1);
	minwrk = max(i__2,i__3);
/* L10: */
    }

/*     Check for errors */

    if (*nsizes < 0) {
	*info = -1;
    } else if (badmm) {
	*info = -2;
    } else if (badnn) {
	*info = -3;
    } else if (*ntypes < 0) {
	*info = -4;
    } else if (*lda < max(1,mmax)) {
	*info = -10;
    } else if (*ldu < max(1,mmax)) {
	*info = -12;
    } else if (*ldvt < max(1,nmax)) {
	*info = -14;
    } else if (minwrk > *lwork) {
	*info = -21;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SDRVBD", &i__1);
	return 0;
    }

/*     Initialize constants */

    s_copy(path, "Single precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "BD", (ftnlen)2, (ftnlen)2);
    nfail = 0;
    ntest = 0;
    unfl = slamch_("Safe minimum");
    ovfl = 1.f / unfl;
    slabad_(&unfl, &ovfl);
    ulp = slamch_("Precision");
    ulpinv = 1.f / ulp;
    infoc_1.infot = 0;

/*     Loop over sizes, types */

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	m = mm[jsize];
	n = nn[jsize];
	mnmin = min(m,n);

	if (*nsizes != 1) {
	    mtypes = min(5,*ntypes);
	} else {
	    mtypes = min(6,*ntypes);
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L140;
	    }

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

/*           Compute "A" */

	    if (mtypes > 5) {
		goto L30;
	    }

	    if (jtype == 1) {

/*              Zero matrix */

		slaset_("Full", &m, &n, &c_b13, &c_b13, &a[a_offset], lda);

	    } else if (jtype == 2) {

/*              Identity matrix */

		slaset_("Full", &m, &n, &c_b13, &c_b17, &a[a_offset], lda);

	    } else {

/*              (Scaled) random matrix */

		if (jtype == 3) {
		    anorm = 1.f;
		}
		if (jtype == 4) {
		    anorm = unfl / ulp;
		}
		if (jtype == 5) {
		    anorm = ovfl * ulp;
		}
		r__1 = (real) mnmin;
		i__3 = m - 1;
		i__4 = n - 1;
		slatms_(&m, &n, "U", &iseed[1], "N", &s[1], &c__4, &r__1, &
			anorm, &i__3, &i__4, "N", &a[a_offset], lda, &work[1], 
			 &iinfo);
		if (iinfo != 0) {
		    io___25.ciunit = *nout;
		    s_wsfe(&io___25);
		    do_fio(&c__1, "Generator", (ftnlen)9);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    return 0;
		}
	    }

L30:
	    slacpy_("F", &m, &n, &a[a_offset], lda, &asav[asav_offset], lda);

/*           Do for minimal and adequate (for blocking) workspace */

	    for (iws = 1; iws <= 4; ++iws) {

		for (j = 1; j <= 14; ++j) {
		    result[j - 1] = -1.f;
/* L40: */
		}

/*              Test SGESVD: Factorize A */

/* Computing MAX */
		i__3 = min(m,n) * 3 + max(m,n), i__4 = min(m,n) * 5;
		iwtmp = max(i__3,i__4);
		lswork = iwtmp + (iws - 1) * (*lwork - iwtmp) / 3;
		lswork = min(lswork,*lwork);
		lswork = max(lswork,1);
		if (iws == 4) {
		    lswork = *lwork;
		}

		if (iws > 1) {
		    slacpy_("F", &m, &n, &asav[asav_offset], lda, &a[a_offset]
, lda);
		}
		s_copy(srnamc_1.srnamt, "SGESVD", (ftnlen)6, (ftnlen)6);
		sgesvd_("A", "A", &m, &n, &a[a_offset], lda, &ssav[1], &usav[
			usav_offset], ldu, &vtsav[vtsav_offset], ldvt, &work[
			1], &lswork, &iinfo);
		if (iinfo != 0) {
		    io___30.ciunit = *nout;
		    s_wsfe(&io___30);
		    do_fio(&c__1, "GESVD", (ftnlen)5);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&lswork, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    return 0;
		}

/*              Do tests 1--4 */

		sbdt01_(&m, &n, &c__0, &asav[asav_offset], lda, &usav[
			usav_offset], ldu, &ssav[1], &e[1], &vtsav[
			vtsav_offset], ldvt, &work[1], result);
		if (m != 0 && n != 0) {
		    sort01_("Columns", &m, &m, &usav[usav_offset], ldu, &work[
			    1], lwork, &result[1]);
		    sort01_("Rows", &n, &n, &vtsav[vtsav_offset], ldvt, &work[
			    1], lwork, &result[2]);
		}
		result[3] = 0.f;
		i__3 = mnmin - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    if (ssav[i__] < ssav[i__ + 1]) {
			result[3] = ulpinv;
		    }
		    if (ssav[i__] < 0.f) {
			result[3] = ulpinv;
		    }
/* L50: */
		}
		if (mnmin >= 1) {
		    if (ssav[mnmin] < 0.f) {
			result[3] = ulpinv;
		    }
		}

/*              Do partial SVDs, comparing to SSAV, USAV, and VTSAV */

		result[4] = 0.f;
		result[5] = 0.f;
		result[6] = 0.f;
		for (iju = 0; iju <= 3; ++iju) {
		    for (ijvt = 0; ijvt <= 3; ++ijvt) {
			if (iju == 3 && ijvt == 3 || iju == 1 && ijvt == 1) {
			    goto L70;
			}
			*(unsigned char *)jobu = *(unsigned char *)&cjob[iju];
			*(unsigned char *)jobvt = *(unsigned char *)&cjob[
				ijvt];
			slacpy_("F", &m, &n, &asav[asav_offset], lda, &a[
				a_offset], lda);
			s_copy(srnamc_1.srnamt, "SGESVD", (ftnlen)6, (ftnlen)
				6);
			sgesvd_(jobu, jobvt, &m, &n, &a[a_offset], lda, &s[1], 
				 &u[u_offset], ldu, &vt[vt_offset], ldvt, &
				work[1], &lswork, &iinfo);

/*                    Compare U */

			dif = 0.f;
			if (m > 0 && n > 0) {
			    if (iju == 1) {
				sort03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &a[a_offset], lda, 
					&work[1], lwork, &dif, &iinfo);
			    } else if (iju == 2) {
				sort03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &u[u_offset], ldu, 
					&work[1], lwork, &dif, &iinfo);
			    } else if (iju == 3) {
				sort03_("C", &m, &m, &m, &mnmin, &usav[
					usav_offset], ldu, &u[u_offset], ldu, 
					&work[1], lwork, &dif, &iinfo);
			    }
			}
			result[4] = dmax(result[4],dif);

/*                    Compare VT */

			dif = 0.f;
			if (m > 0 && n > 0) {
			    if (ijvt == 1) {
				sort03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &a[a_offset], 
					lda, &work[1], lwork, &dif, &iinfo);
			    } else if (ijvt == 2) {
				sort03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &vt[vt_offset], 
					ldvt, &work[1], lwork, &dif, &iinfo);
			    } else if (ijvt == 3) {
				sort03_("R", &n, &n, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &vt[vt_offset], 
					ldvt, &work[1], lwork, &dif, &iinfo);
			    }
			}
			result[5] = dmax(result[5],dif);

/*                    Compare S */

			dif = 0.f;
/* Computing MAX */
			r__1 = (real) mnmin * ulp * s[1];
			div = dmax(r__1,unfl);
			i__3 = mnmin - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    if (ssav[i__] < ssav[i__ + 1]) {
				dif = ulpinv;
			    }
			    if (ssav[i__] < 0.f) {
				dif = ulpinv;
			    }
/* Computing MAX */
			    r__2 = dif, r__3 = (r__1 = ssav[i__] - s[i__], 
				    dabs(r__1)) / div;
			    dif = dmax(r__2,r__3);
/* L60: */
			}
			result[6] = dmax(result[6],dif);
L70:
			;
		    }
/* L80: */
		}

/*              Test SGESDD: Factorize A */

		iwtmp = mnmin * 5 * mnmin + mnmin * 9 + max(m,n);
		lswork = iwtmp + (iws - 1) * (*lwork - iwtmp) / 3;
		lswork = min(lswork,*lwork);
		lswork = max(lswork,1);
		if (iws == 4) {
		    lswork = *lwork;
		}

		slacpy_("F", &m, &n, &asav[asav_offset], lda, &a[a_offset], 
			lda);
		s_copy(srnamc_1.srnamt, "SGESDD", (ftnlen)6, (ftnlen)6);
		sgesdd_("A", &m, &n, &a[a_offset], lda, &ssav[1], &usav[
			usav_offset], ldu, &vtsav[vtsav_offset], ldvt, &work[
			1], &lswork, &iwork[1], &iinfo);
		if (iinfo != 0) {
		    io___38.ciunit = *nout;
		    s_wsfe(&io___38);
		    do_fio(&c__1, "GESDD", (ftnlen)5);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&lswork, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    return 0;
		}

/*              Do tests 8--11 */

		sbdt01_(&m, &n, &c__0, &asav[asav_offset], lda, &usav[
			usav_offset], ldu, &ssav[1], &e[1], &vtsav[
			vtsav_offset], ldvt, &work[1], &result[7]);
		if (m != 0 && n != 0) {
		    sort01_("Columns", &m, &m, &usav[usav_offset], ldu, &work[
			    1], lwork, &result[8]);
		    sort01_("Rows", &n, &n, &vtsav[vtsav_offset], ldvt, &work[
			    1], lwork, &result[9]);
		}
		result[10] = 0.f;
		i__3 = mnmin - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    if (ssav[i__] < ssav[i__ + 1]) {
			result[10] = ulpinv;
		    }
		    if (ssav[i__] < 0.f) {
			result[10] = ulpinv;
		    }
/* L90: */
		}
		if (mnmin >= 1) {
		    if (ssav[mnmin] < 0.f) {
			result[10] = ulpinv;
		    }
		}

/*              Do partial SVDs, comparing to SSAV, USAV, and VTSAV */

		result[11] = 0.f;
		result[12] = 0.f;
		result[13] = 0.f;
		for (ijq = 0; ijq <= 2; ++ijq) {
		    *(unsigned char *)jobq = *(unsigned char *)&cjob[ijq];
		    slacpy_("F", &m, &n, &asav[asav_offset], lda, &a[a_offset]
, lda);
		    s_copy(srnamc_1.srnamt, "SGESDD", (ftnlen)6, (ftnlen)6);
		    sgesdd_(jobq, &m, &n, &a[a_offset], lda, &s[1], &u[
			    u_offset], ldu, &vt[vt_offset], ldvt, &work[1], &
			    lswork, &iwork[1], &iinfo);

/*                 Compare U */

		    dif = 0.f;
		    if (m > 0 && n > 0) {
			if (ijq == 1) {
			    if (m >= n) {
				sort03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &a[a_offset], lda, 
					&work[1], lwork, &dif, info);
			    } else {
				sort03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &u[u_offset], ldu, 
					&work[1], lwork, &dif, info);
			    }
			} else if (ijq == 2) {
			    sort03_("C", &m, &mnmin, &m, &mnmin, &usav[
				    usav_offset], ldu, &u[u_offset], ldu, &
				    work[1], lwork, &dif, info);
			}
		    }
		    result[11] = dmax(result[11],dif);

/*                 Compare VT */

		    dif = 0.f;
		    if (m > 0 && n > 0) {
			if (ijq == 1) {
			    if (m >= n) {
				sort03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &vt[vt_offset], 
					ldvt, &work[1], lwork, &dif, info);
			    } else {
				sort03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &a[a_offset], 
					lda, &work[1], lwork, &dif, info);
			    }
			} else if (ijq == 2) {
			    sort03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
				    vtsav_offset], ldvt, &vt[vt_offset], ldvt, 
				     &work[1], lwork, &dif, info);
			}
		    }
		    result[12] = dmax(result[12],dif);

/*                 Compare S */

		    dif = 0.f;
/* Computing MAX */
		    r__1 = (real) mnmin * ulp * s[1];
		    div = dmax(r__1,unfl);
		    i__3 = mnmin - 1;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			if (ssav[i__] < ssav[i__ + 1]) {
			    dif = ulpinv;
			}
			if (ssav[i__] < 0.f) {
			    dif = ulpinv;
			}
/* Computing MAX */
			r__2 = dif, r__3 = (r__1 = ssav[i__] - s[i__], dabs(
				r__1)) / div;
			dif = dmax(r__2,r__3);
/* L100: */
		    }
		    result[13] = dmax(result[13],dif);
/* L110: */
		}

/*              End of Loop -- Check for RESULT(j) > THRESH */

		for (j = 1; j <= 14; ++j) {
		    if (result[j - 1] >= *thresh) {
			if (nfail == 0) {
			    io___41.ciunit = *nout;
			    s_wsfe(&io___41);
			    e_wsfe();
			    io___42.ciunit = *nout;
			    s_wsfe(&io___42);
			    e_wsfe();
			}
			io___43.ciunit = *nout;
			s_wsfe(&io___43);
			do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&iws, (ftnlen)sizeof(integer));
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[j - 1], (ftnlen)sizeof(
				real));
			e_wsfe();
			++nfail;
		    }
/* L120: */
		}
		ntest += 14;

/* L130: */
	    }
L140:
	    ;
	}
/* L150: */
    }

/*     Summary */

    alasvm_(path, nout, &nfail, &ntest, &c__0);


    return 0;

/*     End of SDRVBD */

} /* sdrvbd_ */
