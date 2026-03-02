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

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__0 = 0;
static integer c__6 = 6;
static real c_b37 = 1.f;
static integer c__1 = 1;
static real c_b47 = 0.f;
static integer c__2 = 2;
static integer c__4 = 4;

/* Subroutine */ int cchkbd_(integer *nsizes, integer *mval, integer *nval, 
	integer *ntypes, logical *dotype, integer *nrhs, integer *iseed, real 
	*thresh, complex *a, integer *lda, real *bd, real *be, real *s1, real 
	*s2, complex *x, integer *ldx, complex *y, complex *z__, complex *q, 
	integer *ldq, complex *pt, integer *ldpt, complex *u, complex *vt, 
	complex *work, integer *lwork, real *rwork, integer *nout, integer *
	info)
{
    /* Initialized data */

    static integer ktype[16] = { 1,2,4,4,4,4,4,6,6,6,6,6,9,9,9,10 };
    static integer kmagn[16] = { 1,1,1,1,1,2,3,1,1,1,2,3,1,2,3,0 };
    static integer kmode[16] = { 0,0,4,3,1,4,4,4,3,1,4,4,0,0,0,0 };

    /* Format strings */
    static char fmt_9998[] = "(\002 CCHKBD: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002M=\002,i6,\002, N=\002,i6,\002, JTYPE=\002,i"
	    "6,\002, ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(\002 M=\002,i5,\002, N=\002,i5,\002, type "
	    "\002,i2,\002, seed=\002,4(i4,\002,\002),\002 test(\002,i2,\002)"
	    "=\002,g11.4)";

    /* System generated locals */
    integer a_dim1, a_offset, pt_dim1, pt_offset, q_dim1, q_offset, u_dim1, 
	    u_offset, vt_dim1, vt_offset, x_dim1, x_offset, y_dim1, y_offset, 
	    z_dim1, z_offset, i__1, i__2, i__3, i__4, i__5, i__6, i__7;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double log(doublereal), sqrt(doublereal), exp(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, m, n, mq;
    real ulp, cond;
    integer jcol;
    char path[3];
    integer mmax, nmax;
    real unfl, ovfl;
    char uplo[1];
    real temp1, temp2;
    extern /* Subroutine */ int cbdt01_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, real *, real *, 
	    complex *, integer *, complex *, real *, real *), cbdt02_(integer 
	    *, integer *, complex *, integer *, complex *, integer *, complex 
	    *, integer *, complex *, real *, real *), cbdt03_(char *, integer 
	    *, integer *, real *, real *, complex *, integer *, real *, 
	    complex *, integer *, complex *, real *);
    logical badmm, badnn;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    integer nfail, imode;
    real dumma[1];
    integer iinfo;
    extern /* Subroutine */ int cunt01_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *, real *, real *);
    real anorm;
    integer mnmin, mnmax, jsize, itype, jtype, iwork[1], ntest;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), slahd2_(integer *, char *);
    integer log2ui;
    logical bidiag;
    extern /* Subroutine */ int cgebrd_(integer *, integer *, complex *, 
	    integer *, real *, real *, complex *, complex *, complex *, 
	    integer *, integer *), slabad_(real *, real *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *), claset_(char *, 
	    integer *, integer *, complex *, complex *, complex *, integer *), xerbla_(char *, integer *);
    integer ioldsd[4];
    extern /* Subroutine */ int cbdsqr_(char *, integer *, integer *, integer 
	    *, integer *, real *, real *, complex *, integer *, complex *, 
	    integer *, complex *, integer *, real *, integer *), 
	    cungbr_(char *, integer *, integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, integer *), 
	    alasum_(char *, integer *, integer *, integer *, integer *);
    extern doublereal slarnd_(integer *, integer *);
    extern /* Subroutine */ int clatmr_(integer *, integer *, char *, integer 
	    *, char *, complex *, integer *, real *, complex *, char *, char *
, complex *, integer *, real *, complex *, integer *, real *, 
	    char *, integer *, integer *, integer *, real *, real *, char *, 
	    complex *, integer *, integer *, integer *), clatms_(integer *, integer *, 
	    char *, integer *, char *, real *, integer *, real *, real *, 
	    integer *, integer *, char *, complex *, integer *, complex *, 
	    integer *);
    real amninv;
    extern /* Subroutine */ int ssvdch_(integer *, real *, real *, real *, 
	    real *, integer *);
    integer minwrk;
    real rtunfl, rtovfl, ulpinv, result[14];
    integer mtypes;

    /* Fortran I/O blocks */
    static cilist io___40 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CCHKBD checks the singular value decomposition (SVD) routines. */

/*  CGEBRD reduces a complex general m by n matrix A to real upper or */
/*  lower bidiagonal form by an orthogonal transformation: Q' * A * P = B */
/*  (or A = Q * B * P').  The matrix B is upper bidiagonal if m >= n */
/*  and lower bidiagonal if m < n. */

/*  CUNGBR generates the orthogonal matrices Q and P' from CGEBRD. */
/*  Note that Q and P are not necessarily square. */

/*  CBDSQR computes the singular value decomposition of the bidiagonal */
/*  matrix B as B = U S V'.  It is called three times to compute */
/*     1)  B = U S1 V', where S1 is the diagonal matrix of singular */
/*         values and the columns of the matrices U and V are the left */
/*         and right singular vectors, respectively, of B. */
/*     2)  Same as 1), but the singular values are stored in S2 and the */
/*         singular vectors are not computed. */
/*     3)  A = (UQ) S (P'V'), the SVD of the original matrix A. */
/*  In addition, CBDSQR has an option to apply the left orthogonal matrix */
/*  U to a matrix X, useful in least squares applications. */

/*  For each pair of matrix dimensions (M,N) and each selected matrix */
/*  type, an M by N matrix A and an M by NRHS matrix X are generated. */
/*  The problem dimensions are as follows */
/*     A:          M x N */
/*     Q:          M x min(M,N) (but M x M if NRHS > 0) */
/*     P:          min(M,N) x N */
/*     B:          min(M,N) x min(M,N) */
/*     U, V:       min(M,N) x min(M,N) */
/*     S1, S2      diagonal, order min(M,N) */
/*     X:          M x NRHS */

/*  For each generated matrix, 14 tests are performed: */

/*  Test CGEBRD and CUNGBR */

/*  (1)   | A - Q B PT | / ( |A| max(M,N) ulp ), PT = P' */

/*  (2)   | I - Q' Q | / ( M ulp ) */

/*  (3)   | I - PT PT' | / ( N ulp ) */

/*  Test CBDSQR on bidiagonal matrix B */

/*  (4)   | B - U S1 VT | / ( |B| min(M,N) ulp ), VT = V' */

/*  (5)   | Y - U Z | / ( |Y| max(min(M,N),k) ulp ), where Y = Q' X */
/*                                                   and   Z = U' Y. */
/*  (6)   | I - U' U | / ( min(M,N) ulp ) */

/*  (7)   | I - VT VT' | / ( min(M,N) ulp ) */

/*  (8)   S1 contains min(M,N) nonnegative values in decreasing order. */
/*        (Return 0 if true, 1/ULP if false.) */

/*  (9)   0 if the true singular values of B are within THRESH of */
/*        those in S1.  2*THRESH if they are not.  (Tested using */
/*        SSVDCH) */

/*  (10)  | S1 - S2 | / ( |S1| ulp ), where S2 is computed without */
/*                                    computing U and V. */

/*  Test CBDSQR on matrix A */

/*  (11)  | A - (QU) S (VT PT) | / ( |A| max(M,N) ulp ) */

/*  (12)  | X - (QU) Z | / ( |X| max(M,k) ulp ) */

/*  (13)  | I - (QU)'(QU) | / ( M ulp ) */

/*  (14)  | I - (VT PT) (PT'VT') | / ( N ulp ) */

/*  The possible matrix types are */

/*  (1)  The zero matrix. */
/*  (2)  The identity matrix. */

/*  (3)  A diagonal matrix with evenly spaced entries */
/*       1, ..., ULP  and random signs. */
/*       (ULP = (first number larger than 1) - 1 ) */
/*  (4)  A diagonal matrix with geometrically spaced entries */
/*       1, ..., ULP  and random signs. */
/*  (5)  A diagonal matrix with "clustered" entries 1, ULP, ..., ULP */
/*       and random signs. */

/*  (6)  Same as (3), but multiplied by SQRT( overflow threshold ) */
/*  (7)  Same as (3), but multiplied by SQRT( underflow threshold ) */

/*  (8)  A matrix of the form  U D V, where U and V are orthogonal and */
/*       D has evenly spaced entries 1, ..., ULP with random signs */
/*       on the diagonal. */

/*  (9)  A matrix of the form  U D V, where U and V are orthogonal and */
/*       D has geometrically spaced entries 1, ..., ULP with random */
/*       signs on the diagonal. */

/*  (10) A matrix of the form  U D V, where U and V are orthogonal and */
/*       D has "clustered" entries 1, ULP,..., ULP with random */
/*       signs on the diagonal. */

/*  (11) Same as (8), but multiplied by SQRT( overflow threshold ) */
/*  (12) Same as (8), but multiplied by SQRT( underflow threshold ) */

/*  (13) Rectangular matrix with random entries chosen from (-1,1). */
/*  (14) Same as (13), but multiplied by SQRT( overflow threshold ) */
/*  (15) Same as (13), but multiplied by SQRT( underflow threshold ) */

/*  Special case: */
/*  (16) A bidiagonal matrix with random entries chosen from a */
/*       logarithmic distribution on [ulp^2,ulp^(-2)]  (I.e., each */
/*       entry is  e^x, where x is chosen uniformly on */
/*       [ 2 log(ulp), -2 log(ulp) ] .)  For *this* type: */
/*       (a) CGEBRD is not called to reduce it to bidiagonal form. */
/*       (b) the bidiagonal is  min(M,N) x min(M,N); if M<N, the */
/*           matrix will be lower bidiagonal, otherwise upper. */
/*       (c) only tests 5--8 and 14 are performed. */

/*  A subset of the full set of matrix types may be selected through */
/*  the logical array DOTYPE. */

/*  Arguments */
/*  ========== */

/*  NSIZES  (input) INTEGER */
/*          The number of values of M and N contained in the vectors */
/*          MVAL and NVAL.  The matrix sizes are used in pairs (M,N). */

/*  MVAL    (input) INTEGER array, dimension (NM) */
/*          The values of the matrix row dimension M. */

/*  NVAL    (input) INTEGER array, dimension (NM) */
/*          The values of the matrix column dimension N. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, CCHKBD */
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

/*  NRHS    (input) INTEGER */
/*          The number of columns in the "right-hand side" matrices X, Y, */
/*          and Z, used in testing CBDSQR.  If NRHS = 0, then the */
/*          operations on the right-hand side will not be tested. */
/*          NRHS must be at least 0. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry ISEED specifies the seed of the random number */
/*          generator. The array elements should be between 0 and 4095; */
/*          if not they will be reduced mod 4096.  Also, ISEED(4) must */
/*          be odd.  The values of ISEED are changed on exit, and can be */
/*          used in the next call to CCHKBD to continue the same random */
/*          number sequence. */

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0.  Note that the */
/*          expected value of the test ratios is O(1), so THRESH should */
/*          be a reasonably small multiple of 1, e.g., 10 or 100. */

/*  A       (workspace) COMPLEX array, dimension (LDA,NMAX) */
/*          where NMAX is the maximum value of N in NVAL. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,MMAX), */
/*          where MMAX is the maximum value of M in MVAL. */

/*  BD      (workspace) REAL array, dimension */
/*                      (max(min(MVAL(j),NVAL(j)))) */

/*  BE      (workspace) REAL array, dimension */
/*                      (max(min(MVAL(j),NVAL(j)))) */

/*  S1      (workspace) REAL array, dimension */
/*                      (max(min(MVAL(j),NVAL(j)))) */

/*  S2      (workspace) REAL array, dimension */
/*                      (max(min(MVAL(j),NVAL(j)))) */

/*  X       (workspace) COMPLEX array, dimension (LDX,NRHS) */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the arrays X, Y, and Z. */
/*          LDX >= max(1,MMAX). */

/*  Y       (workspace) COMPLEX array, dimension (LDX,NRHS) */

/*  Z       (workspace) COMPLEX array, dimension (LDX,NRHS) */

/*  Q       (workspace) COMPLEX array, dimension (LDQ,MMAX) */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,MMAX). */

/*  PT      (workspace) COMPLEX array, dimension (LDPT,NMAX) */

/*  LDPT    (input) INTEGER */
/*          The leading dimension of the arrays PT, U, and V. */
/*          LDPT >= max(1, max(min(MVAL(j),NVAL(j)))). */

/*  U       (workspace) COMPLEX array, dimension */
/*                      (LDPT,max(min(MVAL(j),NVAL(j)))) */

/*  V       (workspace) COMPLEX array, dimension */
/*                      (LDPT,max(min(MVAL(j),NVAL(j)))) */

/*  WORK    (workspace) COMPLEX array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          3(M+N) and  M(M + max(M,N,k) + 1) + N*min(M,N)  for all */
/*          pairs  (M,N)=(MM(j),NN(j)) */

/*  RWORK   (workspace) REAL array, dimension */
/*                      (5*max(min(M,N))) */

/*  NOUT    (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns IINFO not equal to 0.) */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some MM(j) < 0 */
/*           -3: Some NN(j) < 0 */
/*           -4: NTYPES < 0 */
/*           -6: NRHS  < 0 */
/*           -8: THRESH < 0 */
/*          -11: LDA < 1 or LDA < MMAX, where MMAX is max( MM(j) ). */
/*          -17: LDB < 1 or LDB < MMAX. */
/*          -21: LDQ < 1 or LDQ < MMAX. */
/*          -23: LDP < 1 or LDP < MNMAX. */
/*          -27: LWORK too small. */
/*          If  CLATMR, CLATMS, CGEBRD, CUNGBR, or CBDSQR, */
/*              returns an error code, the */
/*              absolute value of it is returned. */

/* ----------------------------------------------------------------------- */

/*     Some Local Variables and Parameters: */
/*     ---- ----- --------- --- ---------- */

/*     ZERO, ONE       Real 0 and 1. */
/*     MAXTYP          The number of types defined. */
/*     NTEST           The number of tests performed, or which can */
/*                     be performed so far, for the current matrix. */
/*     MMAX            Largest value in NN. */
/*     NMAX            Largest value in NN. */
/*     MNMIN           min(MM(j), NN(j)) (the dimension of the bidiagonal */
/*                     matrix.) */
/*     MNMAX           The maximum value of MNMIN for j=1,...,NSIZES. */
/*     NFAIL           The number of tests which have exceeded THRESH */
/*     COND, IMODE     Values to be passed to the matrix generators. */
/*     ANORM           Norm of A; passed to matrix generators. */

/*     OVFL, UNFL      Overflow and underflow thresholds. */
/*     RTOVFL, RTUNFL  Square roots of the previous 2 values. */
/*     ULP, ULPINV     Finest relative precision and its inverse. */

/*             The following four arrays decode JTYPE: */
/*     KTYPE(j)        The general type (1-10) for type "j". */
/*     KMODE(j)        The MODE value to be passed to the matrix */
/*                     generator for type "j". */
/*     KMAGN(j)        The order of magnitude ( O(1), */
/*                     O(overflow^(1/2) ), O(underflow^(1/2) ) */

/* ====================================================================== */

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
    --mval;
    --nval;
    --dotype;
    --iseed;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --bd;
    --be;
    --s1;
    --s2;
    z_dim1 = *ldx;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    y_dim1 = *ldx;
    y_offset = 1 + y_dim1;
    y -= y_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    vt_dim1 = *ldpt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    u_dim1 = *ldpt;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    pt_dim1 = *ldpt;
    pt_offset = 1 + pt_dim1;
    pt -= pt_offset;
    --work;
    --rwork;

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
	i__2 = mmax, i__3 = mval[j];
	mmax = max(i__2,i__3);
	if (mval[j] < 0) {
	    badmm = TRUE_;
	}
/* Computing MAX */
	i__2 = nmax, i__3 = nval[j];
	nmax = max(i__2,i__3);
	if (nval[j] < 0) {
	    badnn = TRUE_;
	}
/* Computing MAX */
/* Computing MIN */
	i__4 = mval[j], i__5 = nval[j];
	i__2 = mnmax, i__3 = min(i__4,i__5);
	mnmax = max(i__2,i__3);
/* Computing MAX */
/* Computing MAX */
	i__4 = mval[j], i__5 = nval[j], i__4 = max(i__4,i__5);
/* Computing MIN */
	i__6 = nval[j], i__7 = mval[j];
	i__2 = minwrk, i__3 = (mval[j] + nval[j]) * 3, i__2 = max(i__2,i__3), 
		i__3 = mval[j] * (mval[j] + max(i__4,*nrhs) + 1) + nval[j] * 
		min(i__6,i__7);
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
    } else if (*nrhs < 0) {
	*info = -6;
    } else if (*lda < mmax) {
	*info = -11;
    } else if (*ldx < mmax) {
	*info = -17;
    } else if (*ldq < mmax) {
	*info = -21;
    } else if (*ldpt < mnmax) {
	*info = -23;
    } else if (minwrk > *lwork) {
	*info = -27;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CCHKBD", &i__1);
	return 0;
    }

/*     Initialize constants */

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
    s_copy(path + 1, "BD", (ftnlen)2, (ftnlen)2);
    nfail = 0;
    ntest = 0;
    unfl = slamch_("Safe minimum");
    ovfl = slamch_("Overflow");
    slabad_(&unfl, &ovfl);
    ulp = slamch_("Precision");
    ulpinv = 1.f / ulp;
    log2ui = (integer) (log(ulpinv) / log(2.f));
    rtunfl = sqrt(unfl);
    rtovfl = sqrt(ovfl);
    infoc_1.infot = 0;

/*     Loop over sizes, types */

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	m = mval[jsize];
	n = nval[jsize];
	mnmin = min(m,n);
/* Computing MAX */
	i__2 = max(m,n);
	amninv = 1.f / max(i__2,1);

	if (*nsizes != 1) {
	    mtypes = min(16,*ntypes);
	} else {
	    mtypes = min(17,*ntypes);
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L170;
	    }

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

	    for (j = 1; j <= 14; ++j) {
		result[j - 1] = -1.f;
/* L30: */
	    }

	    *(unsigned char *)uplo = ' ';

/*           Compute "A" */

/*           Control parameters: */

/*           KMAGN  KMODE        KTYPE */
/*       =1  O(1)   clustered 1  zero */
/*       =2  large  clustered 2  identity */
/*       =3  small  exponential  (none) */
/*       =4         arithmetic   diagonal, (w/ eigenvalues) */
/*       =5         random       symmetric, w/ eigenvalues */
/*       =6                      nonsymmetric, w/ singular values */
/*       =7                      random diagonal */
/*       =8                      random symmetric */
/*       =9                      random nonsymmetric */
/*       =10                     random bidiagonal (log. distrib.) */

	    if (mtypes > 16) {
		goto L100;
	    }

	    itype = ktype[jtype - 1];
	    imode = kmode[jtype - 1];

/*           Compute norm */

	    switch (kmagn[jtype - 1]) {
		case 1:  goto L40;
		case 2:  goto L50;
		case 3:  goto L60;
	    }

L40:
	    anorm = 1.f;
	    goto L70;

L50:
	    anorm = rtovfl * ulp * amninv;
	    goto L70;

L60:
	    anorm = rtunfl * max(m,n) * ulpinv;
	    goto L70;

L70:

	    claset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);
	    iinfo = 0;
	    cond = ulpinv;

	    bidiag = FALSE_;
	    if (itype == 1) {

/*              Zero matrix */

		iinfo = 0;

	    } else if (itype == 2) {

/*              Identity */

		i__3 = mnmin;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    a[i__4].r = anorm, a[i__4].i = 0.f;
/* L80: */
		}

	    } else if (itype == 4) {

/*              Diagonal Matrix, [Eigen]values Specified */

		clatms_(&mnmin, &mnmin, "S", &iseed[1], "N", &rwork[1], &
			imode, &cond, &anorm, &c__0, &c__0, "N", &a[a_offset], 
			 lda, &work[1], &iinfo);

	    } else if (itype == 5) {

/*              Symmetric, eigenvalues specified */

		clatms_(&mnmin, &mnmin, "S", &iseed[1], "S", &rwork[1], &
			imode, &cond, &anorm, &m, &n, "N", &a[a_offset], lda, 
			&work[1], &iinfo);

	    } else if (itype == 6) {

/*              Nonsymmetric, singular values specified */

		clatms_(&m, &n, "S", &iseed[1], "N", &rwork[1], &imode, &cond, 
			 &anorm, &m, &n, "N", &a[a_offset], lda, &work[1], &
			iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random entries */

		clatmr_(&mnmin, &mnmin, "S", &iseed[1], "N", &work[1], &c__6, 
			&c_b37, &c_b2, "T", "N", &work[mnmin + 1], &c__1, &
			c_b37, &work[(mnmin << 1) + 1], &c__1, &c_b37, "N", 
			iwork, &c__0, &c__0, &c_b47, &anorm, "NO", &a[
			a_offset], lda, iwork, &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random entries */

		clatmr_(&mnmin, &mnmin, "S", &iseed[1], "S", &work[1], &c__6, 
			&c_b37, &c_b2, "T", "N", &work[mnmin + 1], &c__1, &
			c_b37, &work[m + mnmin + 1], &c__1, &c_b37, "N", 
			iwork, &m, &n, &c_b47, &anorm, "NO", &a[a_offset], 
			lda, iwork, &iinfo);

	    } else if (itype == 9) {

/*              Nonsymmetric, random entries */

		clatmr_(&m, &n, "S", &iseed[1], "N", &work[1], &c__6, &c_b37, 
			&c_b2, "T", "N", &work[mnmin + 1], &c__1, &c_b37, &
			work[m + mnmin + 1], &c__1, &c_b37, "N", iwork, &m, &
			n, &c_b47, &anorm, "NO", &a[a_offset], lda, iwork, &
			iinfo);

	    } else if (itype == 10) {

/*              Bidiagonal, random entries */

		temp1 = log(ulp) * -2.f;
		i__3 = mnmin;
		for (j = 1; j <= i__3; ++j) {
		    bd[j] = exp(temp1 * slarnd_(&c__2, &iseed[1]));
		    if (j < mnmin) {
			be[j] = exp(temp1 * slarnd_(&c__2, &iseed[1]));
		    }
/* L90: */
		}

		iinfo = 0;
		bidiag = TRUE_;
		if (m >= n) {
		    *(unsigned char *)uplo = 'U';
		} else {
		    *(unsigned char *)uplo = 'L';
		}
	    } else {
		iinfo = 1;
	    }

	    if (iinfo == 0) {

/*              Generate Right-Hand Side */

		if (bidiag) {
		    clatmr_(&mnmin, nrhs, "S", &iseed[1], "N", &work[1], &
			    c__6, &c_b37, &c_b2, "T", "N", &work[mnmin + 1], &
			    c__1, &c_b37, &work[(mnmin << 1) + 1], &c__1, &
			    c_b37, "N", iwork, &mnmin, nrhs, &c_b47, &c_b37, 
			    "NO", &y[y_offset], ldx, iwork, &iinfo);
		} else {
		    clatmr_(&m, nrhs, "S", &iseed[1], "N", &work[1], &c__6, &
			    c_b37, &c_b2, "T", "N", &work[m + 1], &c__1, &
			    c_b37, &work[(m << 1) + 1], &c__1, &c_b37, "N", 
			    iwork, &m, nrhs, &c_b47, &c_b37, "NO", &x[
			    x_offset], ldx, iwork, &iinfo);
		}
	    }

/*           Error Exit */

	    if (iinfo != 0) {
		io___40.ciunit = *nout;
		s_wsfe(&io___40);
		do_fio(&c__1, "Generator", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		return 0;
	    }

L100:

/*           Call CGEBRD and CUNGBR to compute B, Q, and P, do tests. */

	    if (! bidiag) {

/*              Compute transformations to reduce A to bidiagonal form: */
/*              B := Q' * A * P. */

		clacpy_(" ", &m, &n, &a[a_offset], lda, &q[q_offset], ldq);
		i__3 = *lwork - (mnmin << 1);
		cgebrd_(&m, &n, &q[q_offset], ldq, &bd[1], &be[1], &work[1], &
			work[mnmin + 1], &work[(mnmin << 1) + 1], &i__3, &
			iinfo);

/*              Check error code from CGEBRD. */

		if (iinfo != 0) {
		    io___41.ciunit = *nout;
		    s_wsfe(&io___41);
		    do_fio(&c__1, "CGEBRD", (ftnlen)6);
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

		clacpy_(" ", &m, &n, &q[q_offset], ldq, &pt[pt_offset], ldpt);
		if (m >= n) {
		    *(unsigned char *)uplo = 'U';
		} else {
		    *(unsigned char *)uplo = 'L';
		}

/*              Generate Q */

		mq = m;
		if (*nrhs <= 0) {
		    mq = mnmin;
		}
		i__3 = *lwork - (mnmin << 1);
		cungbr_("Q", &m, &mq, &n, &q[q_offset], ldq, &work[1], &work[(
			mnmin << 1) + 1], &i__3, &iinfo);

/*              Check error code from CUNGBR. */

		if (iinfo != 0) {
		    io___43.ciunit = *nout;
		    s_wsfe(&io___43);
		    do_fio(&c__1, "CUNGBR(Q)", (ftnlen)9);
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

/*              Generate P' */

		i__3 = *lwork - (mnmin << 1);
		cungbr_("P", &mnmin, &n, &m, &pt[pt_offset], ldpt, &work[
			mnmin + 1], &work[(mnmin << 1) + 1], &i__3, &iinfo);

/*              Check error code from CUNGBR. */

		if (iinfo != 0) {
		    io___44.ciunit = *nout;
		    s_wsfe(&io___44);
		    do_fio(&c__1, "CUNGBR(P)", (ftnlen)9);
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

/*              Apply Q' to an M by NRHS matrix X:  Y := Q' * X. */

		cgemm_("Conjugate transpose", "No transpose", &m, nrhs, &m, &
			c_b2, &q[q_offset], ldq, &x[x_offset], ldx, &c_b1, &y[
			y_offset], ldx);

/*              Test 1:  Check the decomposition A := Q * B * PT */
/*                   2:  Check the orthogonality of Q */
/*                   3:  Check the orthogonality of PT */

		cbdt01_(&m, &n, &c__1, &a[a_offset], lda, &q[q_offset], ldq, &
			bd[1], &be[1], &pt[pt_offset], ldpt, &work[1], &rwork[
			1], result);
		cunt01_("Columns", &m, &mq, &q[q_offset], ldq, &work[1], 
			lwork, &rwork[1], &result[1]);
		cunt01_("Rows", &mnmin, &n, &pt[pt_offset], ldpt, &work[1], 
			lwork, &rwork[1], &result[2]);
	    }

/*           Use CBDSQR to form the SVD of the bidiagonal matrix B: */
/*           B := U * S1 * VT, and compute Z = U' * Y. */

	    scopy_(&mnmin, &bd[1], &c__1, &s1[1], &c__1);
	    if (mnmin > 0) {
		i__3 = mnmin - 1;
		scopy_(&i__3, &be[1], &c__1, &rwork[1], &c__1);
	    }
	    clacpy_(" ", &m, nrhs, &y[y_offset], ldx, &z__[z_offset], ldx);
	    claset_("Full", &mnmin, &mnmin, &c_b1, &c_b2, &u[u_offset], ldpt);
	    claset_("Full", &mnmin, &mnmin, &c_b1, &c_b2, &vt[vt_offset], 
		    ldpt);

	    cbdsqr_(uplo, &mnmin, &mnmin, &mnmin, nrhs, &s1[1], &rwork[1], &
		    vt[vt_offset], ldpt, &u[u_offset], ldpt, &z__[z_offset], 
		    ldx, &rwork[mnmin + 1], &iinfo);

/*           Check error code from CBDSQR. */

	    if (iinfo != 0) {
		io___45.ciunit = *nout;
		s_wsfe(&io___45);
		do_fio(&c__1, "CBDSQR(vects)", (ftnlen)13);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[3] = ulpinv;
		    goto L150;
		}
	    }

/*           Use CBDSQR to compute only the singular values of the */
/*           bidiagonal matrix B;  U, VT, and Z should not be modified. */

	    scopy_(&mnmin, &bd[1], &c__1, &s2[1], &c__1);
	    if (mnmin > 0) {
		i__3 = mnmin - 1;
		scopy_(&i__3, &be[1], &c__1, &rwork[1], &c__1);
	    }

	    cbdsqr_(uplo, &mnmin, &c__0, &c__0, &c__0, &s2[1], &rwork[1], &vt[
		    vt_offset], ldpt, &u[u_offset], ldpt, &z__[z_offset], ldx, 
		     &rwork[mnmin + 1], &iinfo);

/*           Check error code from CBDSQR. */

	    if (iinfo != 0) {
		io___46.ciunit = *nout;
		s_wsfe(&io___46);
		do_fio(&c__1, "CBDSQR(values)", (ftnlen)14);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[8] = ulpinv;
		    goto L150;
		}
	    }

/*           Test 4:  Check the decomposition B := U * S1 * VT */
/*                5:  Check the computation Z := U' * Y */
/*                6:  Check the orthogonality of U */
/*                7:  Check the orthogonality of VT */

	    cbdt03_(uplo, &mnmin, &c__1, &bd[1], &be[1], &u[u_offset], ldpt, &
		    s1[1], &vt[vt_offset], ldpt, &work[1], &result[3]);
	    cbdt02_(&mnmin, nrhs, &y[y_offset], ldx, &z__[z_offset], ldx, &u[
		    u_offset], ldpt, &work[1], &rwork[1], &result[4]);
	    cunt01_("Columns", &mnmin, &mnmin, &u[u_offset], ldpt, &work[1], 
		    lwork, &rwork[1], &result[5]);
	    cunt01_("Rows", &mnmin, &mnmin, &vt[vt_offset], ldpt, &work[1], 
		    lwork, &rwork[1], &result[6]);

/*           Test 8:  Check that the singular values are sorted in */
/*                    non-increasing order and are non-negative */

	    result[7] = 0.f;
	    i__3 = mnmin - 1;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		if (s1[i__] < s1[i__ + 1]) {
		    result[7] = ulpinv;
		}
		if (s1[i__] < 0.f) {
		    result[7] = ulpinv;
		}
/* L110: */
	    }
	    if (mnmin >= 1) {
		if (s1[mnmin] < 0.f) {
		    result[7] = ulpinv;
		}
	    }

/*           Test 9:  Compare CBDSQR with and without singular vectors */

	    temp2 = 0.f;

	    i__3 = mnmin;
	    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
/* Computing MAX */
		r__6 = (r__1 = s1[j], dabs(r__1)), r__7 = (r__2 = s2[j], dabs(
			r__2));
		r__4 = sqrt(unfl) * dmax(s1[1],1.f), r__5 = ulp * dmax(r__6,
			r__7);
		temp1 = (r__3 = s1[j] - s2[j], dabs(r__3)) / dmax(r__4,r__5);
		temp2 = dmax(temp1,temp2);
/* L120: */
	    }

	    result[8] = temp2;

/*           Test 10:  Sturm sequence test of singular values */
/*                     Go up by factors of two until it succeeds */

	    temp1 = *thresh * (.5f - ulp);

	    i__3 = log2ui;
	    for (j = 0; j <= i__3; ++j) {
		ssvdch_(&mnmin, &bd[1], &be[1], &s1[1], &temp1, &iinfo);
		if (iinfo == 0) {
		    goto L140;
		}
		temp1 *= 2.f;
/* L130: */
	    }

L140:
	    result[9] = temp1;

/*           Use CBDSQR to form the decomposition A := (QU) S (VT PT) */
/*           from the bidiagonal form A := Q B PT. */

	    if (! bidiag) {
		scopy_(&mnmin, &bd[1], &c__1, &s2[1], &c__1);
		if (mnmin > 0) {
		    i__3 = mnmin - 1;
		    scopy_(&i__3, &be[1], &c__1, &rwork[1], &c__1);
		}

		cbdsqr_(uplo, &mnmin, &n, &m, nrhs, &s2[1], &rwork[1], &pt[
			pt_offset], ldpt, &q[q_offset], ldq, &y[y_offset], 
			ldx, &rwork[mnmin + 1], &iinfo);

/*              Test 11:  Check the decomposition A := Q*U * S2 * VT*PT */
/*                   12:  Check the computation Z := U' * Q' * X */
/*                   13:  Check the orthogonality of Q*U */
/*                   14:  Check the orthogonality of VT*PT */

		cbdt01_(&m, &n, &c__0, &a[a_offset], lda, &q[q_offset], ldq, &
			s2[1], dumma, &pt[pt_offset], ldpt, &work[1], &rwork[
			1], &result[10]);
		cbdt02_(&m, nrhs, &x[x_offset], ldx, &y[y_offset], ldx, &q[
			q_offset], ldq, &work[1], &rwork[1], &result[11]);
		cunt01_("Columns", &m, &mq, &q[q_offset], ldq, &work[1], 
			lwork, &rwork[1], &result[12]);
		cunt01_("Rows", &mnmin, &n, &pt[pt_offset], ldpt, &work[1], 
			lwork, &rwork[1], &result[13]);
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

L150:
	    for (j = 1; j <= 14; ++j) {
		if (result[j - 1] >= *thresh) {
		    if (nfail == 0) {
			slahd2_(nout, path);
		    }
		    io___50.ciunit = *nout;
		    s_wsfe(&io___50);
		    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&result[j - 1], (ftnlen)sizeof(real)
			    );
		    e_wsfe();
		    ++nfail;
		}
/* L160: */
	    }
	    if (! bidiag) {
		ntest += 14;
	    } else {
		ntest += 5;
	    }

L170:
	    ;
	}
/* L180: */
    }

/*     Summary */

    alasum_(path, nout, &nfail, &ntest, &c__0);

    return 0;

/*     End of CCHKBD */


} /* cchkbd_ */
