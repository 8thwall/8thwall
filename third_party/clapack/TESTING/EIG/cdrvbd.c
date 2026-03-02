#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__4 = 4;
static integer c__1 = 1;
static integer c__0 = 0;

/* Subroutine */ int cdrvbd_(integer *nsizes, integer *mm, integer *nn, 
	integer *ntypes, logical *dotype, integer *iseed, real *thresh, 
	complex *a, integer *lda, complex *u, integer *ldu, complex *vt, 
	integer *ldvt, complex *asav, complex *usav, complex *vtsav, real *s, 
	real *ssav, real *e, complex *work, integer *lwork, real *rwork, 
	integer *iwork, integer *nounit, integer *info)
{
    /* Initialized data */

    static char cjob[1*4] = "N" "O" "S" "A";

    /* Format strings */
    static char fmt_9996[] = "(\002 CDRVBD: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002M=\002,i6,\002, N=\002,i6,\002, JTYPE=\002,i"
	    "6,\002, ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9995[] = "(\002 CDRVBD: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002M=\002,i6,\002, N=\002,i6,\002, JTYPE=\002,i"
	    "6,\002, LSWORK=\002,i6,/9x,\002ISEED=(\002,3(i5,\002,\002),i5"
	    ",\002)\002)";
    static char fmt_9999[] = "(\002 SVD -- Complex Singular Value Decomposit"
	    "ion Driver \002,/\002 Matrix types (see CDRVBD for details):\002"
	    ",//\002 1 = Zero matrix\002,/\002 2 = Identity matrix\002,/\002 "
	    "3 = Evenly spaced singular values near 1\002,/\002 4 = Evenly sp"
	    "aced singular values near underflow\002,/\002 5 = Evenly spaced "
	    "singular values near overflow\002,//\002 Tests performed: ( A is"
	    " dense, U and V are unitary,\002,/19x,\002 S is an array, and Up"
	    "artial, VTpartial, and\002,/19x,\002 Spartial are partially comp"
	    "uted U, VT and S),\002,/)";
    static char fmt_9998[] = "(\002 Tests performed with Test Threshold ="
	    " \002,f8.2,/\002 CGESVD: \002,/\002 1 = | A - U diag(S) VT | / ("
	    " |A| max(M,N) ulp ) \002,/\002 2 = | I - U**T U | / ( M ulp )"
	    " \002,/\002 3 = | I - VT VT**T | / ( N ulp ) \002,/\002 4 = 0 if"
	    " S contains min(M,N) nonnegative values in\002,\002 decreasing o"
	    "rder, else 1/ulp\002,/\002 5 = | U - Upartial | / ( M ulp )\002,/"
	    "\002 6 = | VT - VTpartial | / ( N ulp )\002,/\002 7 = | S - Spar"
	    "tial | / ( min(M,N) ulp |S| )\002,/\002 CGESDD: \002,/\002 8 = |"
	    " A - U diag(S) VT | / ( |A| max(M,N) ulp ) \002,/\002 9 = | I - "
	    "U**T U | / ( M ulp ) \002,/\00210 = | I - VT VT**T | / ( N ulp ) "
	    "\002,/\00211 = 0 if S contains min(M,N) nonnegative values in"
	    "\002,\002 decreasing order, else 1/ulp\002,/\00212 = | U - Upart"
	    "ial | / ( M ulp )\002,/\00213 = | VT - VTpartial | / ( N ulp "
	    ")\002,/\00214 = | S - Spartial | / ( min(M,N) ulp |S| )\002,//)";
    static char fmt_9997[] = "(\002 M=\002,i5,\002, N=\002,i5,\002, type "
	    "\002,i1,\002, IWS=\002,i1,\002, seed=\002,4(i4,\002,\002),\002 t"
	    "est(\002,i1,\002)=\002,g11.4)";

    /* System generated locals */
    integer a_dim1, a_offset, asav_dim1, asav_offset, u_dim1, u_offset, 
	    usav_dim1, usav_offset, vt_dim1, vt_offset, vtsav_dim1, 
	    vtsav_offset, i__1, i__2, i__3, i__4, i__5, i__6, i__7, i__8, 
	    i__9, i__10, i__11, i__12, i__13, i__14;
    real r__1, r__2, r__3;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, m, n;
    real dif, div;
    integer ijq, iju;
    real ulp;
    char jobq[1], jobu[1];
    integer mmax, nmax;
    real unfl, ovfl;
    integer ijvt;
    extern /* Subroutine */ int cbdt01_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, 
	    complex *, integer *, complex *, real *, real *);
    logical badmm, badnn;
    integer nfail, iinfo;
    extern /* Subroutine */ int cunt01_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *, real *, real *);
    real anorm;
    extern /* Subroutine */ int cunt03_(char *, integer *, integer *, integer 
	    *, integer *, complex *, integer *, complex *, integer *, complex 
	    *, integer *, real *, real *, integer *);
    integer mnmin, mnmax;
    char jobvt[1];
    integer iwspc, jsize, nerrs, jtype, ntest, iwtmp;
    extern /* Subroutine */ int cgesdd_(char *, integer *, integer *, complex 
	    *, integer *, real *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, integer *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int cgesvd_(char *, char *, integer *, integer *, 
	    complex *, integer *, real *, complex *, integer *, complex *, 
	    integer *, complex *, integer *, real *, integer *), clacpy_(char *, integer *, integer *, complex *, integer 
	    *, complex *, integer *), claset_(char *, integer *, 
	    integer *, complex *, complex *, complex *, integer *);
    integer ioldsd[4];
    extern /* Subroutine */ int xerbla_(char *, integer *), alasvm_(
	    char *, integer *, integer *, integer *, integer *), 
	    clatms_(integer *, integer *, char *, integer *, char *, real *, 
	    integer *, real *, real *, integer *, integer *, char *, complex *
, integer *, complex *, integer *);
    integer ntestf, minwrk;
    real ulpinv, result[14];
    integer lswork, mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___27 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CDRVBD checks the singular value decomposition (SVD) driver CGESVD */
/*  and CGESDD. */
/*  CGESVD and CGESDD factors A = U diag(S) VT, where U and VT are */
/*  unitary and diag(S) is diagonal with the entries of the array S on */
/*  its diagonal. The entries of S are the singular values, nonnegative */
/*  and stored in decreasing order.  U and VT can be optionally not */
/*  computed, overwritten on A, or computed partially. */

/*  A is M by N. Let MNMIN = min( M, N ). S has dimension MNMIN. */
/*  U can be M by M or M by MNMIN. VT can be N by N or MNMIN by N. */

/*  When CDRVBD is called, a number of matrix "sizes" (M's and N's) */
/*  and a number of matrix "types" are specified.  For each size (M,N) */
/*  and each type of matrix, and for the minimal workspace as well as */
/*  workspace adequate to permit blocking, an  M x N  matrix "A" will be */
/*  generated and used to test the SVD routines.  For each matrix, A will */
/*  be factored as A = U diag(S) VT and the following 12 tests computed: */

/*  Test for CGESVD: */

/*  (1)   | A - U diag(S) VT | / ( |A| max(M,N) ulp ) */

/*  (2)   | I - U'U | / ( M ulp ) */

/*  (3)   | I - VT VT' | / ( N ulp ) */

/*  (4)   S contains MNMIN nonnegative values in decreasing order. */
/*        (Return 0 if true, 1/ULP if false.) */

/*  (5)   | U - Upartial | / ( M ulp ) where Upartial is a partially */
/*        computed U. */

/*  (6)   | VT - VTpartial | / ( N ulp ) where VTpartial is a partially */
/*        computed VT. */

/*  (7)   | S - Spartial | / ( MNMIN ulp |S| ) where Spartial is the */
/*        vector of singular values from the partial SVD */

/*  Test for CGESDD: */

/*  (1)   | A - U diag(S) VT | / ( |A| max(M,N) ulp ) */

/*  (2)   | I - U'U | / ( M ulp ) */

/*  (3)   | I - VT VT' | / ( N ulp ) */

/*  (4)   S contains MNMIN nonnegative values in decreasing order. */
/*        (Return 0 if true, 1/ULP if false.) */

/*  (5)   | U - Upartial | / ( M ulp ) where Upartial is a partially */
/*        computed U. */

/*  (6)   | VT - VTpartial | / ( N ulp ) where VTpartial is a partially */
/*        computed VT. */

/*  (7)   | S - Spartial | / ( MNMIN ulp |S| ) where Spartial is the */
/*        vector of singular values from the partial SVD */

/*  The "sizes" are specified by the arrays MM(1:NSIZES) and */
/*  NN(1:NSIZES); the value of each element pair (MM(j),NN(j)) */
/*  specifies one size.  The "types" are specified by a logical array */
/*  DOTYPE( 1:NTYPES ); if DOTYPE(j) is .TRUE., then matrix type "j" */
/*  will be generated. */
/*  Currently, the list of possible types is: */

/*  (1)  The zero matrix. */
/*  (2)  The identity matrix. */
/*  (3)  A matrix of the form  U D V, where U and V are unitary and */
/*       D has evenly spaced entries 1, ..., ULP with random signs */
/*       on the diagonal. */
/*  (4)  Same as (3), but multiplied by the underflow-threshold / ULP. */
/*  (5)  Same as (3), but multiplied by the overflow-threshold * ULP. */

/*  Arguments */
/*  ========== */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          CDRVBD does nothing.  It must be at least zero. */

/*  MM      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the matrix "heights" to be used.  For */
/*          each j=1,...,NSIZES, if MM(j) is zero, then MM(j) and NN(j) */
/*          will be ignored.  The MM(j) values must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the matrix "widths" to be used.  For */
/*          each j=1,...,NSIZES, if NN(j) is zero, then MM(j) and NN(j) */
/*          will be ignored.  The NN(j) values must be at least zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, CDRVBD */
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
/*          On entry ISEED specifies the seed of the random number */
/*          generator. The array elements should be between 0 and 4095; */
/*          if not they will be reduced mod 4096.  Also, ISEED(4) must */
/*          be odd.  The random number generator uses a linear */
/*          congruential sequence limited to small integers, and so */
/*          should produce machine independent random numbers. The */
/*          values of ISEED are changed on exit, and can be used in the */
/*          next call to CDRVBD to continue the same random number */
/*          sequence. */

/*  THRESH  (input) REAL */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error */
/*          is scaled to be O(1), so THRESH should be a reasonably */
/*          small multiple of 1, e.g., 10 or 100.  In particular, */
/*          it should not depend on the precision (single vs. double) */
/*          or the size of the matrix.  It must be at least zero. */

/*  NOUNIT  (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns IINFO not equal to 0.) */

/*  A       (output) COMPLEX array, dimension (LDA,max(NN)) */
/*          Used to hold the matrix whose singular values are to be */
/*          computed.  On exit, A contains the last matrix actually */
/*          used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at */
/*          least 1 and at least max( MM ). */

/*  U       (output) COMPLEX array, dimension (LDU,max(MM)) */
/*          Used to hold the computed matrix of right singular vectors. */
/*          On exit, U contains the last such vectors actually computed. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  It must be at */
/*          least 1 and at least max( MM ). */

/*  VT      (output) COMPLEX array, dimension (LDVT,max(NN)) */
/*          Used to hold the computed matrix of left singular vectors. */
/*          On exit, VT contains the last such vectors actually computed. */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of VT.  It must be at */
/*          least 1 and at least max( NN ). */

/*  ASAV    (output) COMPLEX array, dimension (LDA,max(NN)) */
/*          Used to hold a different copy of the matrix whose singular */
/*          values are to be computed.  On exit, A contains the last */
/*          matrix actually used. */

/*  USAV    (output) COMPLEX array, dimension (LDU,max(MM)) */
/*          Used to hold a different copy of the computed matrix of */
/*          right singular vectors. On exit, USAV contains the last such */
/*          vectors actually computed. */

/*  VTSAV   (output) COMPLEX array, dimension (LDVT,max(NN)) */
/*          Used to hold a different copy of the computed matrix of */
/*          left singular vectors. On exit, VTSAV contains the last such */
/*          vectors actually computed. */

/*  S       (output) REAL array, dimension (max(min(MM,NN))) */
/*          Contains the computed singular values. */

/*  SSAV    (output) REAL array, dimension (max(min(MM,NN))) */
/*          Contains another copy of the computed singular values. */

/*  E       (output) REAL array, dimension (max(min(MM,NN))) */
/*          Workspace for CGESVD. */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          MAX(3*MIN(M,N)+MAX(M,N)**2,5*MIN(M,N),3*MAX(M,N)) for all */
/*          pairs  (M,N)=(MM(j),NN(j)) */

/*  RWORK   (workspace) REAL array, */
/*                      dimension ( 5*max(max(MM,NN)) ) */

/*  IWORK   (workspace) INTEGER array, dimension at least 8*min(M,N) */

/*  RESULT  (output) REAL array, dimension (7) */
/*          The values computed by the 7 tests described above. */
/*          The values are currently limited to 1/ULP, to avoid */
/*          overflow. */

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
/*          If  CLATMS, or CGESVD returns an error code, the */
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
    --rwork;
    --iwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

    *info = 0;

/*     Important constants */

    nerrs = 0;
    ntestt = 0;
    ntestf = 0;
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
	i__9 = mm[j], i__10 = nn[j];
/* Computing 2nd power */
	i__8 = max(i__9,i__10);
/* Computing MIN */
	i__11 = mm[j], i__12 = nn[j];
/* Computing MAX */
	i__13 = mm[j], i__14 = nn[j];
	i__4 = min(i__6,i__7) * 3 + i__8 * i__8, i__5 = min(i__11,i__12) * 5, 
		i__4 = max(i__4,i__5), i__5 = max(i__13,i__14) * 3;
	i__2 = minwrk, i__3 = max(i__4,i__5);
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
	xerbla_("CDRVBD", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

/*     More Important constants */

    unfl = slamch_("S");
    ovfl = 1.f / unfl;
    ulp = slamch_("E");
    ulpinv = 1.f / ulp;

/*     Loop over sizes, types */

    nerrs = 0;

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
		goto L170;
	    }
	    ntest = 0;

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

/*           Compute "A" */

	    if (mtypes > 5) {
		goto L50;
	    }

	    if (jtype == 1) {

/*              Zero matrix */

		claset_("Full", &m, &n, &c_b1, &c_b1, &a[a_offset], lda);
		i__3 = min(m,n);
		for (i__ = 1; i__ <= i__3; ++i__) {
		    s[i__] = 0.f;
/* L30: */
		}

	    } else if (jtype == 2) {

/*              Identity matrix */

		claset_("Full", &m, &n, &c_b1, &c_b2, &a[a_offset], lda);
		i__3 = min(m,n);
		for (i__ = 1; i__ <= i__3; ++i__) {
		    s[i__] = 1.f;
/* L40: */
		}

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
		clatms_(&m, &n, "U", &iseed[1], "N", &s[1], &c__4, &r__1, &
			anorm, &i__3, &i__4, "N", &a[a_offset], lda, &work[1], 
			 &iinfo);
		if (iinfo != 0) {
		    io___27.ciunit = *nounit;
		    s_wsfe(&io___27);
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

L50:
	    clacpy_("F", &m, &n, &a[a_offset], lda, &asav[asav_offset], lda);

/*           Do for minimal and adequate (for blocking) workspace */

	    for (iwspc = 1; iwspc <= 4; ++iwspc) {

/*              Test for CGESVD */

		iwtmp = (min(m,n) << 1) + max(m,n);
		lswork = iwtmp + (iwspc - 1) * (*lwork - iwtmp) / 3;
		lswork = min(lswork,*lwork);
		lswork = max(lswork,1);
		if (iwspc == 4) {
		    lswork = *lwork;
		}

		for (j = 1; j <= 14; ++j) {
		    result[j - 1] = -1.f;
/* L60: */
		}

/*              Factorize A */

		if (iwspc > 1) {
		    clacpy_("F", &m, &n, &asav[asav_offset], lda, &a[a_offset]
, lda);
		}
		cgesvd_("A", "A", &m, &n, &a[a_offset], lda, &ssav[1], &usav[
			usav_offset], ldu, &vtsav[vtsav_offset], ldvt, &work[
			1], &lswork, &rwork[1], &iinfo);
		if (iinfo != 0) {
		    io___32.ciunit = *nounit;
		    s_wsfe(&io___32);
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

		cbdt01_(&m, &n, &c__0, &asav[asav_offset], lda, &usav[
			usav_offset], ldu, &ssav[1], &e[1], &vtsav[
			vtsav_offset], ldvt, &work[1], &rwork[1], result);
		if (m != 0 && n != 0) {
		    cunt01_("Columns", &mnmin, &m, &usav[usav_offset], ldu, &
			    work[1], lwork, &rwork[1], &result[1]);
		    cunt01_("Rows", &mnmin, &n, &vtsav[vtsav_offset], ldvt, &
			    work[1], lwork, &rwork[1], &result[2]);
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
/* L70: */
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
			    goto L90;
			}
			*(unsigned char *)jobu = *(unsigned char *)&cjob[iju];
			*(unsigned char *)jobvt = *(unsigned char *)&cjob[
				ijvt];
			clacpy_("F", &m, &n, &asav[asav_offset], lda, &a[
				a_offset], lda);
			cgesvd_(jobu, jobvt, &m, &n, &a[a_offset], lda, &s[1], 
				 &u[u_offset], ldu, &vt[vt_offset], ldvt, &
				work[1], &lswork, &rwork[1], &iinfo);

/*                    Compare U */

			dif = 0.f;
			if (m > 0 && n > 0) {
			    if (iju == 1) {
				cunt03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &a[a_offset], lda, 
					&work[1], lwork, &rwork[1], &dif, &
					iinfo);
			    } else if (iju == 2) {
				cunt03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &u[u_offset], ldu, 
					&work[1], lwork, &rwork[1], &dif, &
					iinfo);
			    } else if (iju == 3) {
				cunt03_("C", &m, &m, &m, &mnmin, &usav[
					usav_offset], ldu, &u[u_offset], ldu, 
					&work[1], lwork, &rwork[1], &dif, &
					iinfo);
			    }
			}
			result[4] = dmax(result[4],dif);

/*                    Compare VT */

			dif = 0.f;
			if (m > 0 && n > 0) {
			    if (ijvt == 1) {
				cunt03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &a[a_offset], 
					lda, &work[1], lwork, &rwork[1], &dif, 
					 &iinfo);
			    } else if (ijvt == 2) {
				cunt03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &vt[vt_offset], 
					ldvt, &work[1], lwork, &rwork[1], &
					dif, &iinfo);
			    } else if (ijvt == 3) {
				cunt03_("R", &n, &n, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &vt[vt_offset], 
					ldvt, &work[1], lwork, &rwork[1], &
					dif, &iinfo);
			    }
			}
			result[5] = dmax(result[5],dif);

/*                    Compare S */

			dif = 0.f;
/* Computing MAX */
			r__1 = (real) mnmin * ulp * s[1], r__2 = slamch_(
				"Safe minimum");
			div = dmax(r__1,r__2);
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
/* L80: */
			}
			result[6] = dmax(result[6],dif);
L90:
			;
		    }
/* L100: */
		}

/*              Test for CGESDD */

		iwtmp = (mnmin << 1) * mnmin + (mnmin << 1) + max(m,n);
		lswork = iwtmp + (iwspc - 1) * (*lwork - iwtmp) / 3;
		lswork = min(lswork,*lwork);
		lswork = max(lswork,1);
		if (iwspc == 4) {
		    lswork = *lwork;
		}

/*              Factorize A */

		clacpy_("F", &m, &n, &asav[asav_offset], lda, &a[a_offset], 
			lda);
		cgesdd_("A", &m, &n, &a[a_offset], lda, &ssav[1], &usav[
			usav_offset], ldu, &vtsav[vtsav_offset], ldvt, &work[
			1], &lswork, &rwork[1], &iwork[1], &iinfo);
		if (iinfo != 0) {
		    io___39.ciunit = *nounit;
		    s_wsfe(&io___39);
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

/*              Do tests 1--4 */

		cbdt01_(&m, &n, &c__0, &asav[asav_offset], lda, &usav[
			usav_offset], ldu, &ssav[1], &e[1], &vtsav[
			vtsav_offset], ldvt, &work[1], &rwork[1], &result[7]);
		if (m != 0 && n != 0) {
		    cunt01_("Columns", &mnmin, &m, &usav[usav_offset], ldu, &
			    work[1], lwork, &rwork[1], &result[8]);
		    cunt01_("Rows", &mnmin, &n, &vtsav[vtsav_offset], ldvt, &
			    work[1], lwork, &rwork[1], &result[9]);
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
/* L110: */
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
		    clacpy_("F", &m, &n, &asav[asav_offset], lda, &a[a_offset]
, lda);
		    cgesdd_(jobq, &m, &n, &a[a_offset], lda, &s[1], &u[
			    u_offset], ldu, &vt[vt_offset], ldvt, &work[1], &
			    lswork, &rwork[1], &iwork[1], &iinfo);

/*                 Compare U */

		    dif = 0.f;
		    if (m > 0 && n > 0) {
			if (ijq == 1) {
			    if (m >= n) {
				cunt03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &a[a_offset], lda, 
					&work[1], lwork, &rwork[1], &dif, &
					iinfo);
			    } else {
				cunt03_("C", &m, &mnmin, &m, &mnmin, &usav[
					usav_offset], ldu, &u[u_offset], ldu, 
					&work[1], lwork, &rwork[1], &dif, &
					iinfo);
			    }
			} else if (ijq == 2) {
			    cunt03_("C", &m, &mnmin, &m, &mnmin, &usav[
				    usav_offset], ldu, &u[u_offset], ldu, &
				    work[1], lwork, &rwork[1], &dif, &iinfo);
			}
		    }
		    result[11] = dmax(result[11],dif);

/*                 Compare VT */

		    dif = 0.f;
		    if (m > 0 && n > 0) {
			if (ijq == 1) {
			    if (m >= n) {
				cunt03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &vt[vt_offset], 
					ldvt, &work[1], lwork, &rwork[1], &
					dif, &iinfo);
			    } else {
				cunt03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
					vtsav_offset], ldvt, &a[a_offset], 
					lda, &work[1], lwork, &rwork[1], &dif, 
					 &iinfo);
			    }
			} else if (ijq == 2) {
			    cunt03_("R", &n, &mnmin, &n, &mnmin, &vtsav[
				    vtsav_offset], ldvt, &vt[vt_offset], ldvt, 
				     &work[1], lwork, &rwork[1], &dif, &iinfo);
			}
		    }
		    result[12] = dmax(result[12],dif);

/*                 Compare S */

		    dif = 0.f;
/* Computing MAX */
		    r__1 = (real) mnmin * ulp * s[1], r__2 = slamch_("Safe m"
			    "inimum");
		    div = dmax(r__1,r__2);
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
/* L120: */
		    }
		    result[13] = dmax(result[13],dif);
/* L130: */
		}

/*              End of Loop -- Check for RESULT(j) > THRESH */

		ntest = 0;
		nfail = 0;
		for (j = 1; j <= 14; ++j) {
		    if (result[j - 1] >= 0.f) {
			++ntest;
		    }
		    if (result[j - 1] >= *thresh) {
			++nfail;
		    }
/* L140: */
		}

		if (nfail > 0) {
		    ++ntestf;
		}
		if (ntestf == 1) {
		    io___43.ciunit = *nounit;
		    s_wsfe(&io___43);
		    e_wsfe();
		    io___44.ciunit = *nounit;
		    s_wsfe(&io___44);
		    do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(real));
		    e_wsfe();
		    ntestf = 2;
		}

		for (j = 1; j <= 14; ++j) {
		    if (result[j - 1] >= *thresh) {
			io___45.ciunit = *nounit;
			s_wsfe(&io___45);
			do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&iwspc, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[j - 1], (ftnlen)sizeof(
				real));
			e_wsfe();
		    }
/* L150: */
		}

		nerrs += nfail;
		ntestt += ntest;

/* L160: */
	    }

L170:
	    ;
	}
/* L180: */
    }

/*     Summary */

    alasvm_("CBD", nounit, &nerrs, &ntestt, &c__0);


    return 0;

/*     End of CDRVBD */

} /* cdrvbd_ */
