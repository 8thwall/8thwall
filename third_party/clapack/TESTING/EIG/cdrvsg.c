#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__0 = 0;
static integer c__6 = 6;
static real c_b33 = 1.f;
static integer c__1 = 1;
static real c_b43 = 0.f;
static integer c__4 = 4;
static integer c__5 = 5;
static real c_b78 = 10.f;
static integer c__3 = 3;

/* Subroutine */ int cdrvsg_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, real *thresh, integer *nounit, 
	complex *a, integer *lda, complex *b, integer *ldb, real *d__, 
	complex *z__, integer *ldz, complex *ab, complex *bb, complex *ap, 
	complex *bp, complex *work, integer *nwork, real *rwork, integer *
	lrwork, integer *iwork, integer *liwork, real *result, integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,4,4,4,4,4,5,5,5,5,5,8,8,8,9,9,9,9,9,9 };
    static integer kmagn[21] = { 1,1,1,1,1,2,3,1,1,1,2,3,1,2,3,1,1,1,1,1,1 };
    static integer kmode[21] = { 0,0,4,3,1,4,4,4,3,1,4,4,0,0,0,4,4,4,4,4,4 };

    /* Format strings */
    static char fmt_9999[] = "(\002 CDRVSG: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";

    /* System generated locals */
    address a__1[3];
    integer a_dim1, a_offset, ab_dim1, ab_offset, b_dim1, b_offset, bb_dim1, 
	    bb_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4, i__5, i__6[3]
	    , i__7;
    char ch__1[10], ch__2[11], ch__3[12], ch__4[13];

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, j, m, n, ka, kb, ij, il, iu;
    real vl, vu;
    integer ka9, kb9;
    real ulp, cond;
    integer jcol, nmax;
    real unfl, ovfl;
    char uplo[1];
    logical badnn;
    extern /* Subroutine */ int chbgv_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, real *, 
	    complex *, integer *, complex *, real *, integer *), chegv_(integer *, char *, char *, integer *, complex *, 
	    integer *, complex *, integer *, real *, complex *, integer *, 
	    real *, integer *);
    integer imode;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int csgt01_(integer *, char *, integer *, integer 
	    *, complex *, integer *, complex *, integer *, complex *, integer 
	    *, real *, complex *, real *, real *);
    integer iinfo;
    extern /* Subroutine */ int chpgv_(integer *, char *, char *, integer *, 
	    complex *, complex *, real *, complex *, integer *, complex *, 
	    real *, integer *);
    real aninv, anorm;
    integer itemp, nmats, jsize, nerrs, itype, jtype, ntest, iseed2[4];
    extern /* Subroutine */ int slabad_(real *, real *), chbgvd_(char *, char 
	    *, integer *, integer *, integer *, complex *, integer *, complex 
	    *, integer *, real *, complex *, integer *, complex *, integer *, 
	    real *, integer *, integer *, integer *, integer *), chegvd_(integer *, char *, char *, integer *, complex *, 
	    integer *, complex *, integer *, real *, complex *, integer *, 
	    real *, integer *, integer *, integer *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int chpgvd_(integer *, char *, char *, integer *, 
	    complex *, complex *, real *, complex *, integer *, complex *, 
	    integer *, real *, integer *, integer *, integer *, integer *);
    integer idumma[1];
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *);
    integer ioldsd[4];
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *), xerbla_(char *, 
	    integer *), chbgvx_(char *, char *, char *, integer *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *, integer *, integer *, real *
, integer *, real *, complex *, integer *, complex *, real *, 
	    integer *, integer *, integer *), clatmr_(
	    integer *, integer *, char *, integer *, char *, complex *, 
	    integer *, real *, complex *, char *, char *, complex *, integer *
, real *, complex *, integer *, real *, char *, integer *, 
	    integer *, integer *, real *, real *, char *, complex *, integer *
, integer *, integer *);
    extern doublereal slarnd_(integer *, integer *);
    real abstol;
    extern /* Subroutine */ int chegvx_(integer *, char *, char *, char *, 
	    integer *, complex *, integer *, complex *, integer *, real *, 
	    real *, integer *, integer *, real *, integer *, real *, complex *
, integer *, complex *, integer *, real *, integer *, integer *, 
	    integer *), clatms_(integer *, integer *, 
	    char *, integer *, char *, real *, integer *, real *, real *, 
	    integer *, integer *, char *, complex *, integer *, complex *, 
	    integer *);
    integer ibuplo, ibtype;
    extern /* Subroutine */ int slafts_(char *, integer *, integer *, integer 
	    *, integer *, real *, integer *, real *, integer *, integer *), chpgvx_(integer *, char *, char *, char *, integer *, 
	    complex *, complex *, real *, real *, integer *, integer *, real *
, integer *, real *, complex *, integer *, complex *, real *, 
	    integer *, integer *, integer *), slasum_(
	    char *, integer *, integer *, integer *);
    real rtunfl, rtovfl, ulpinv;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___36 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/* ********************************************************************* */

/*     modified August 1997, a new parameter LRWORK and LIWORK are */
/*     added in the calling sequence. */

/*     test routine CSGT01 is also modified */

/* ********************************************************************* */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*       CDRVSG checks the complex Hermitian generalized eigenproblem */
/*       drivers. */

/*               CHEGV computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite generalized */
/*               eigenproblem. */

/*               CHEGVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite generalized */
/*               eigenproblem using a divide and conquer algorithm. */

/*               CHEGVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite generalized */
/*               eigenproblem. */

/*               CHPGV computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite generalized */
/*               eigenproblem in packed storage. */

/*               CHPGVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite generalized */
/*               eigenproblem in packed storage using a divide and */
/*               conquer algorithm. */

/*               CHPGVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite generalized */
/*               eigenproblem in packed storage. */

/*               CHBGV computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite banded */
/*               generalized eigenproblem. */

/*               CHBGVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite banded */
/*               generalized eigenproblem using a divide and conquer */
/*               algorithm. */

/*               CHBGVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian-definite banded */
/*               generalized eigenproblem. */

/*       When CDRVSG is called, a number of matrix "sizes" ("n's") and a */
/*       number of matrix "types" are specified.  For each size ("n") */
/*       and each type of matrix, one matrix A of the given type will be */
/*       generated; a random well-conditioned matrix B is also generated */
/*       and the pair (A,B) is used to test the drivers. */

/*       For each pair (A,B), the following tests are performed: */

/*       (1) CHEGV with ITYPE = 1 and UPLO ='U': */

/*               | A Z - B Z D | / ( |A| |Z| n ulp ) */

/*       (2) as (1) but calling CHPGV */
/*       (3) as (1) but calling CHBGV */
/*       (4) as (1) but with UPLO = 'L' */
/*       (5) as (4) but calling CHPGV */
/*       (6) as (4) but calling CHBGV */

/*       (7) CHEGV with ITYPE = 2 and UPLO ='U': */

/*               | A B Z - Z D | / ( |A| |Z| n ulp ) */

/*       (8) as (7) but calling CHPGV */
/*       (9) as (7) but with UPLO = 'L' */
/*       (10) as (9) but calling CHPGV */

/*       (11) CHEGV with ITYPE = 3 and UPLO ='U': */

/*               | B A Z - Z D | / ( |A| |Z| n ulp ) */

/*       (12) as (11) but calling CHPGV */
/*       (13) as (11) but with UPLO = 'L' */
/*       (14) as (13) but calling CHPGV */

/*       CHEGVD, CHPGVD and CHBGVD performed the same 14 tests. */

/*       CHEGVX, CHPGVX and CHBGVX performed the above 14 tests with */
/*       the parameter RANGE = 'A', 'N' and 'I', respectively. */

/*       The "sizes" are specified by an array NN(1:NSIZES); the value of */
/*       each element NN(j) specifies one size. */
/*       The "types" are specified by a logical array DOTYPE( 1:NTYPES ); */
/*       if DOTYPE(j) is .TRUE., then matrix type "j" will be generated. */
/*       This type is used for the matrix A which has half-bandwidth KA. */
/*       B is generated as a well-conditioned positive definite matrix */
/*       with half-bandwidth KB (<= KA). */
/*       Currently, the list of possible types for A is: */

/*       (1)  The zero matrix. */
/*       (2)  The identity matrix. */

/*       (3)  A diagonal matrix with evenly spaced entries */
/*            1, ..., ULP  and random signs. */
/*            (ULP = (first number larger than 1) - 1 ) */
/*       (4)  A diagonal matrix with geometrically spaced entries */
/*            1, ..., ULP  and random signs. */
/*       (5)  A diagonal matrix with "clustered" entries 1, ULP, ..., ULP */
/*            and random signs. */

/*       (6)  Same as (4), but multiplied by SQRT( overflow threshold ) */
/*       (7)  Same as (4), but multiplied by SQRT( underflow threshold ) */

/*       (8)  A matrix of the form  U* D U, where U is unitary and */
/*            D has evenly spaced entries 1, ..., ULP with random signs */
/*            on the diagonal. */

/*       (9)  A matrix of the form  U* D U, where U is unitary and */
/*            D has geometrically spaced entries 1, ..., ULP with random */
/*            signs on the diagonal. */

/*       (10) A matrix of the form  U* D U, where U is unitary and */
/*            D has "clustered" entries 1, ULP,..., ULP with random */
/*            signs on the diagonal. */

/*       (11) Same as (8), but multiplied by SQRT( overflow threshold ) */
/*       (12) Same as (8), but multiplied by SQRT( underflow threshold ) */

/*       (13) Hermitian matrix with random entries chosen from (-1,1). */
/*       (14) Same as (13), but multiplied by SQRT( overflow threshold ) */
/*       (15) Same as (13), but multiplied by SQRT( underflow threshold ) */

/*       (16) Same as (8), but with KA = 1 and KB = 1 */
/*       (17) Same as (8), but with KA = 2 and KB = 1 */
/*       (18) Same as (8), but with KA = 2 and KB = 2 */
/*       (19) Same as (8), but with KA = 3 and KB = 1 */
/*       (20) Same as (8), but with KA = 3 and KB = 2 */
/*       (21) Same as (8), but with KA = 3 and KB = 3 */

/*  Arguments */
/*  ========= */

/*  NSIZES  INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          CDRVSG does nothing.  It must be at least zero. */
/*          Not modified. */

/*  NN      INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */
/*          Not modified. */

/*  NTYPES  INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, CDRVSG */
/*          does nothing.  It must be at least zero.  If it is MAXTYP+1 */
/*          and NSIZES is 1, then an additional type, MAXTYP+1 is */
/*          defined, which is to use whatever matrix is in A.  This */
/*          is only useful if DOTYPE(1:MAXTYP) is .FALSE. and */
/*          DOTYPE(MAXTYP+1) is .TRUE. . */
/*          Not modified. */

/*  DOTYPE  LOGICAL array, dimension (NTYPES) */
/*          If DOTYPE(j) is .TRUE., then for each size in NN a */
/*          matrix of that size and of type j will be generated. */
/*          If NTYPES is smaller than the maximum number of types */
/*          defined (PARAMETER MAXTYP), then types NTYPES+1 through */
/*          MAXTYP will not be generated.  If NTYPES is larger */
/*          than MAXTYP, DOTYPE(MAXTYP+1) through DOTYPE(NTYPES) */
/*          will be ignored. */
/*          Not modified. */

/*  ISEED   INTEGER array, dimension (4) */
/*          On entry ISEED specifies the seed of the random number */
/*          generator. The array elements should be between 0 and 4095; */
/*          if not they will be reduced mod 4096.  Also, ISEED(4) must */
/*          be odd.  The random number generator uses a linear */
/*          congruential sequence limited to small integers, and so */
/*          should produce machine independent random numbers. The */
/*          values of ISEED are changed on exit, and can be used in the */
/*          next call to CDRVSG to continue the same random number */
/*          sequence. */
/*          Modified. */

/*  THRESH  REAL */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error */
/*          is scaled to be O(1), so THRESH should be a reasonably */
/*          small multiple of 1, e.g., 10 or 100.  In particular, */
/*          it should not depend on the precision (single vs. double) */
/*          or the size of the matrix.  It must be at least zero. */
/*          Not modified. */

/*  NOUNIT  INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns IINFO not equal to 0.) */
/*          Not modified. */

/*  A       COMPLEX array, dimension (LDA , max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually */
/*          used. */
/*          Modified. */

/*  LDA     INTEGER */
/*          The leading dimension of A.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  B       COMPLEX array, dimension (LDB , max(NN)) */
/*          Used to hold the Hermitian positive definite matrix for */
/*          the generailzed problem. */
/*          On exit, B contains the last matrix actually */
/*          used. */
/*          Modified. */

/*  LDB     INTEGER */
/*          The leading dimension of B.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  D       REAL array, dimension (max(NN)) */
/*          The eigenvalues of A. On exit, the eigenvalues in D */
/*          correspond with the matrix in A. */
/*          Modified. */

/*  Z       COMPLEX array, dimension (LDZ, max(NN)) */
/*          The matrix of eigenvectors. */
/*          Modified. */

/*  LDZ     INTEGER */
/*          The leading dimension of ZZ.  It must be at least 1 and */
/*          at least max( NN ). */
/*          Not modified. */

/*  AB      COMPLEX array, dimension (LDA, max(NN)) */
/*          Workspace. */
/*          Modified. */

/*  BB      COMPLEX array, dimension (LDB, max(NN)) */
/*          Workspace. */
/*          Modified. */

/*  AP      COMPLEX array, dimension (max(NN)**2) */
/*          Workspace. */
/*          Modified. */

/*  BP      COMPLEX array, dimension (max(NN)**2) */
/*          Workspace. */
/*          Modified. */

/*  WORK    COMPLEX array, dimension (NWORK) */
/*          Workspace. */
/*          Modified. */

/*  NWORK   INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          2*N + N**2  where  N = max( NN(j), 2 ). */
/*          Not modified. */

/*  RWORK   REAL array, dimension (LRWORK) */
/*          Workspace. */
/*          Modified. */

/*  LRWORK  INTEGER */
/*          The number of entries in RWORK.  This must be at least */
/*          max( 7*N, 1 + 4*N + 2*N*lg(N) + 3*N**2 ) where */
/*          N = max( NN(j) ) and lg( N ) = smallest integer k such */
/*          that 2**k >= N . */
/*          Not modified. */

/*  IWORK   INTEGER array, dimension (LIWORK)) */
/*          Workspace. */
/*          Modified. */

/*  LIWORK  INTEGER */
/*          The number of entries in IWORK.  This must be at least */
/*          2 + 5*max( NN(j) ). */
/*          Not modified. */

/*  RESULT  REAL array, dimension (70) */
/*          The values computed by the 70 tests described above. */
/*          Modified. */

/*  INFO    INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some NN(j) < 0 */
/*           -3: NTYPES < 0 */
/*           -5: THRESH < 0 */
/*           -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*          -16: LDZ < 1 or LDZ < NMAX. */
/*          -21: NWORK too small. */
/*          -23: LRWORK too small. */
/*          -25: LIWORK too small. */
/*          If  CLATMR, CLATMS, CHEGV, CHPGV, CHBGV, CHEGVD, CHPGVD, */
/*              CHPGVD, CHEGVX, CHPGVX, CHBGVX returns an error code, */
/*              the absolute value of it is returned. */
/*          Modified. */

/* ----------------------------------------------------------------------- */

/*       Some Local Variables and Parameters: */
/*       ---- ----- --------- --- ---------- */
/*       ZERO, ONE       Real 0 and 1. */
/*       MAXTYP          The number of types defined. */
/*       NTEST           The number of tests that have been run */
/*                       on this matrix. */
/*       NTESTT          The total number of tests for this call. */
/*       NMAX            Largest value in NN. */
/*       NMATS           The number of matrices generated so far. */
/*       NERRS           The number of tests which have exceeded THRESH */
/*                       so far (computed by SLAFTS). */
/*       COND, IMODE     Values to be passed to the matrix generators. */
/*       ANORM           Norm of A; passed to matrix generators. */

/*       OVFL, UNFL      Overflow and underflow thresholds. */
/*       ULP, ULPINV     Finest relative precision and its inverse. */
/*       RTOVFL, RTUNFL  Square roots of the previous 2 values. */
/*               The following four arrays decode JTYPE: */
/*       KTYPE(j)        The general type (1-10) for type "j". */
/*       KMODE(j)        The MODE value to be passed to the matrix */
/*                       generator for type "j". */
/*       KMAGN(j)        The order of magnitude ( O(1), */
/*                       O(overflow^(1/2) ), O(underflow^(1/2) ) */

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
    --nn;
    --dotype;
    --iseed;
    ab_dim1 = *lda;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    bb_dim1 = *ldb;
    bb_offset = 1 + bb_dim1;
    bb -= bb_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --d__;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --ap;
    --bp;
    --work;
    --rwork;
    --iwork;
    --result;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     1)      Check for errors */

    ntestt = 0;
    *info = 0;

    badnn = FALSE_;
    nmax = 0;
    i__1 = *nsizes;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__2 = nmax, i__3 = nn[j];
	nmax = max(i__2,i__3);
	if (nn[j] < 0) {
	    badnn = TRUE_;
	}
/* L10: */
    }

/*     Check for errors */

    if (*nsizes < 0) {
	*info = -1;
    } else if (badnn) {
	*info = -2;
    } else if (*ntypes < 0) {
	*info = -3;
    } else if (*lda <= 1 || *lda < nmax) {
	*info = -9;
    } else if (*ldz <= 1 || *ldz < nmax) {
	*info = -16;
    } else /* if(complicated condition) */ {
/* Computing 2nd power */
	i__1 = max(nmax,2);
	if (i__1 * i__1 << 1 > *nwork) {
	    *info = -21;
	} else /* if(complicated condition) */ {
/* Computing 2nd power */
	    i__1 = max(nmax,2);
	    if (i__1 * i__1 << 1 > *lrwork) {
		*info = -23;
	    } else /* if(complicated condition) */ {
/* Computing 2nd power */
		i__1 = max(nmax,2);
		if (i__1 * i__1 << 1 > *liwork) {
		    *info = -25;
		}
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CDRVSG", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

/*     More Important constants */

    unfl = slamch_("Safe minimum");
    ovfl = slamch_("Overflow");
    slabad_(&unfl, &ovfl);
    ulp = slamch_("Epsilon") * slamch_("Base");
    ulpinv = 1.f / ulp;
    rtunfl = sqrt(unfl);
    rtovfl = sqrt(ovfl);

    for (i__ = 1; i__ <= 4; ++i__) {
	iseed2[i__ - 1] = iseed[i__];
/* L20: */
    }

/*     Loop over sizes, types */

    nerrs = 0;
    nmats = 0;

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	n = nn[jsize];
	aninv = 1.f / (real) max(1,n);

	if (*nsizes != 1) {
	    mtypes = min(21,*ntypes);
	} else {
	    mtypes = min(22,*ntypes);
	}

	ka9 = 0;
	kb9 = 0;
	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L640;
	    }
	    ++nmats;
	    ntest = 0;

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L30: */
	    }

/*           2)      Compute "A" */

/*                   Control parameters: */

/*               KMAGN  KMODE        KTYPE */
/*           =1  O(1)   clustered 1  zero */
/*           =2  large  clustered 2  identity */
/*           =3  small  exponential  (none) */
/*           =4         arithmetic   diagonal, w/ eigenvalues */
/*           =5         random log   hermitian, w/ eigenvalues */
/*           =6         random       (none) */
/*           =7                      random diagonal */
/*           =8                      random hermitian */
/*           =9                      banded, w/ eigenvalues */

	    if (mtypes > 21) {
		goto L90;
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
	    anorm = rtovfl * ulp * aninv;
	    goto L70;

L60:
	    anorm = rtunfl * n * ulpinv;
	    goto L70;

L70:

	    iinfo = 0;
	    cond = ulpinv;

/*           Special Matrices -- Identity & Jordan block */

	    if (itype == 1) {

/*              Zero */

		ka = 0;
		kb = 0;
		claset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);

	    } else if (itype == 2) {

/*              Identity */

		ka = 0;
		kb = 0;
		claset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);
		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    a[i__4].r = anorm, a[i__4].i = 0.f;
/* L80: */
		}

	    } else if (itype == 4) {

/*              Diagonal Matrix, [Eigen]values Specified */

		ka = 0;
		kb = 0;
		clatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &c__0, &c__0, "N", &a[a_offset], lda, &work[
			1], &iinfo);

	    } else if (itype == 5) {

/*              Hermitian, eigenvalues specified */

/* Computing MAX */
		i__3 = 0, i__4 = n - 1;
		ka = max(i__3,i__4);
		kb = ka;
		clatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &n, &n, "N", &a[a_offset], lda, &work[1], &
			iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		ka = 0;
		kb = 0;
		clatmr_(&n, &n, "S", &iseed[1], "H", &work[1], &c__6, &c_b33, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b33, &work[(
			n << 1) + 1], &c__1, &c_b33, "N", idumma, &c__0, &
			c__0, &c_b43, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Hermitian, random eigenvalues */

/* Computing MAX */
		i__3 = 0, i__4 = n - 1;
		ka = max(i__3,i__4);
		kb = ka;
		clatmr_(&n, &n, "S", &iseed[1], "H", &work[1], &c__6, &c_b33, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b33, &work[(
			n << 1) + 1], &c__1, &c_b33, "N", idumma, &n, &n, &
			c_b43, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              Hermitian banded, eigenvalues specified */

/*              The following values are used for the half-bandwidths: */

/*                ka = 1   kb = 1 */
/*                ka = 2   kb = 1 */
/*                ka = 2   kb = 2 */
/*                ka = 3   kb = 1 */
/*                ka = 3   kb = 2 */
/*                ka = 3   kb = 3 */

		++kb9;
		if (kb9 > ka9) {
		    ++ka9;
		    kb9 = 1;
		}
/* Computing MAX */
/* Computing MIN */
		i__5 = n - 1;
		i__3 = 0, i__4 = min(i__5,ka9);
		ka = max(i__3,i__4);
/* Computing MAX */
/* Computing MIN */
		i__5 = n - 1;
		i__3 = 0, i__4 = min(i__5,kb9);
		kb = max(i__3,i__4);
		clatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &ka, &ka, "N", &a[a_offset], lda, &work[1], &
			iinfo);

	    } else {

		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___36.ciunit = *nounit;
		s_wsfe(&io___36);
		do_fio(&c__1, "Generator", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		return 0;
	    }

L90:

	    abstol = unfl + unfl;
	    if (n <= 1) {
		il = 1;
		iu = n;
	    } else {
		il = (n - 1) * slarnd_(&c__1, iseed2) + 1;
		iu = (n - 1) * slarnd_(&c__1, iseed2) + 1;
		if (il > iu) {
		    itemp = il;
		    il = iu;
		    iu = itemp;
		}
	    }

/*           3) Call CHEGV, CHPGV, CHBGV, CHEGVD, CHPGVD, CHBGVD, */
/*              CHEGVX, CHPGVX and CHBGVX, do tests. */

/*           loop over the three generalized problems */
/*                 IBTYPE = 1: A*x = (lambda)*B*x */
/*                 IBTYPE = 2: A*B*x = (lambda)*x */
/*                 IBTYPE = 3: B*A*x = (lambda)*x */

	    for (ibtype = 1; ibtype <= 3; ++ibtype) {

/*              loop over the setting UPLO */

		for (ibuplo = 1; ibuplo <= 2; ++ibuplo) {
		    if (ibuplo == 1) {
			*(unsigned char *)uplo = 'U';
		    }
		    if (ibuplo == 2) {
			*(unsigned char *)uplo = 'L';
		    }

/*                 Generate random well-conditioned positive definite */
/*                 matrix B, of bandwidth not greater than that of A. */

		    clatms_(&n, &n, "U", &iseed[1], "P", &rwork[1], &c__5, &
			    c_b78, &c_b33, &kb, &kb, uplo, &b[b_offset], ldb, 
			    &work[n + 1], &iinfo);

/*                 Test CHEGV */

		    ++ntest;

		    clacpy_(" ", &n, &n, &a[a_offset], lda, &z__[z_offset], 
			    ldz);
		    clacpy_(uplo, &n, &n, &b[b_offset], ldb, &bb[bb_offset], 
			    ldb);

		    chegv_(&ibtype, "V", uplo, &n, &z__[z_offset], ldz, &bb[
			    bb_offset], ldb, &d__[1], &work[1], nwork, &rwork[
			    1], &iinfo);
		    if (iinfo != 0) {
			io___44.ciunit = *nounit;
			s_wsfe(&io___44);
/* Writing concatenation */
			i__6[0] = 8, a__1[0] = "CHEGV(V,";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
			do_fio(&c__1, ch__1, (ftnlen)10);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L100;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

/*                 Test CHEGVD */

		    ++ntest;

		    clacpy_(" ", &n, &n, &a[a_offset], lda, &z__[z_offset], 
			    ldz);
		    clacpy_(uplo, &n, &n, &b[b_offset], ldb, &bb[bb_offset], 
			    ldb);

		    chegvd_(&ibtype, "V", uplo, &n, &z__[z_offset], ldz, &bb[
			    bb_offset], ldb, &d__[1], &work[1], nwork, &rwork[
			    1], lrwork, &iwork[1], liwork, &iinfo);
		    if (iinfo != 0) {
			io___45.ciunit = *nounit;
			s_wsfe(&io___45);
/* Writing concatenation */
			i__6[0] = 9, a__1[0] = "CHEGVD(V,";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)11);
			do_fio(&c__1, ch__2, (ftnlen)11);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L100;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

/*                 Test CHEGVX */

		    ++ntest;

		    clacpy_(" ", &n, &n, &a[a_offset], lda, &ab[ab_offset], 
			    lda);
		    clacpy_(uplo, &n, &n, &b[b_offset], ldb, &bb[bb_offset], 
			    ldb);

		    chegvx_(&ibtype, "V", "A", uplo, &n, &ab[ab_offset], lda, 
			    &bb[bb_offset], ldb, &vl, &vu, &il, &iu, &abstol, 
			    &m, &d__[1], &z__[z_offset], ldz, &work[1], nwork, 
			     &rwork[1], &iwork[n + 1], &iwork[1], &iinfo);
		    if (iinfo != 0) {
			io___49.ciunit = *nounit;
			s_wsfe(&io___49);
/* Writing concatenation */
			i__6[0] = 10, a__1[0] = "CHEGVX(V,A";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)12);
			do_fio(&c__1, ch__3, (ftnlen)12);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L100;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

		    ++ntest;

		    clacpy_(" ", &n, &n, &a[a_offset], lda, &ab[ab_offset], 
			    lda);
		    clacpy_(uplo, &n, &n, &b[b_offset], ldb, &bb[bb_offset], 
			    ldb);

/*                 since we do not know the exact eigenvalues of this */
/*                 eigenpair, we just set VL and VU as constants. */
/*                 It is quite possible that there are no eigenvalues */
/*                 in this interval. */

		    vl = 0.f;
		    vu = anorm;
		    chegvx_(&ibtype, "V", "V", uplo, &n, &ab[ab_offset], lda, 
			    &bb[bb_offset], ldb, &vl, &vu, &il, &iu, &abstol, 
			    &m, &d__[1], &z__[z_offset], ldz, &work[1], nwork, 
			     &rwork[1], &iwork[n + 1], &iwork[1], &iinfo);
		    if (iinfo != 0) {
			io___50.ciunit = *nounit;
			s_wsfe(&io___50);
/* Writing concatenation */
			i__6[0] = 11, a__1[0] = "CHEGVX(V,V,";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__4, a__1, i__6, &c__3, (ftnlen)13);
			do_fio(&c__1, ch__4, (ftnlen)13);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L100;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &m, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

		    ++ntest;

		    clacpy_(" ", &n, &n, &a[a_offset], lda, &ab[ab_offset], 
			    lda);
		    clacpy_(uplo, &n, &n, &b[b_offset], ldb, &bb[bb_offset], 
			    ldb);

		    chegvx_(&ibtype, "V", "I", uplo, &n, &ab[ab_offset], lda, 
			    &bb[bb_offset], ldb, &vl, &vu, &il, &iu, &abstol, 
			    &m, &d__[1], &z__[z_offset], ldz, &work[1], nwork, 
			     &rwork[1], &iwork[n + 1], &iwork[1], &iinfo);
		    if (iinfo != 0) {
			io___51.ciunit = *nounit;
			s_wsfe(&io___51);
/* Writing concatenation */
			i__6[0] = 11, a__1[0] = "CHEGVX(V,I,";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__4, a__1, i__6, &c__3, (ftnlen)13);
			do_fio(&c__1, ch__4, (ftnlen)13);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L100;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &m, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

L100:

/*                 Test CHPGV */

		    ++ntest;

/*                 Copy the matrices into packed storage. */

		    if (lsame_(uplo, "U")) {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = j;
			    for (i__ = 1; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L110: */
			    }
/* L120: */
			}
		    } else {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = n;
			    for (i__ = j; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L130: */
			    }
/* L140: */
			}
		    }

		    chpgv_(&ibtype, "V", uplo, &n, &ap[1], &bp[1], &d__[1], &
			    z__[z_offset], ldz, &work[1], &rwork[1], &iinfo);
		    if (iinfo != 0) {
			io___53.ciunit = *nounit;
			s_wsfe(&io___53);
/* Writing concatenation */
			i__6[0] = 8, a__1[0] = "CHPGV(V,";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
			do_fio(&c__1, ch__1, (ftnlen)10);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L310;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

/*                 Test CHPGVD */

		    ++ntest;

/*                 Copy the matrices into packed storage. */

		    if (lsame_(uplo, "U")) {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = j;
			    for (i__ = 1; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L150: */
			    }
/* L160: */
			}
		    } else {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = n;
			    for (i__ = j; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L170: */
			    }
/* L180: */
			}
		    }

		    chpgvd_(&ibtype, "V", uplo, &n, &ap[1], &bp[1], &d__[1], &
			    z__[z_offset], ldz, &work[1], nwork, &rwork[1], 
			    lrwork, &iwork[1], liwork, &iinfo);
		    if (iinfo != 0) {
			io___54.ciunit = *nounit;
			s_wsfe(&io___54);
/* Writing concatenation */
			i__6[0] = 9, a__1[0] = "CHPGVD(V,";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)11);
			do_fio(&c__1, ch__2, (ftnlen)11);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L310;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

/*                 Test CHPGVX */

		    ++ntest;

/*                 Copy the matrices into packed storage. */

		    if (lsame_(uplo, "U")) {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = j;
			    for (i__ = 1; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L190: */
			    }
/* L200: */
			}
		    } else {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = n;
			    for (i__ = j; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L210: */
			    }
/* L220: */
			}
		    }

		    chpgvx_(&ibtype, "V", "A", uplo, &n, &ap[1], &bp[1], &vl, 
			    &vu, &il, &iu, &abstol, &m, &d__[1], &z__[
			    z_offset], ldz, &work[1], &rwork[1], &iwork[n + 1]
, &iwork[1], info);
		    if (iinfo != 0) {
			io___55.ciunit = *nounit;
			s_wsfe(&io___55);
/* Writing concatenation */
			i__6[0] = 10, a__1[0] = "CHPGVX(V,A";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)12);
			do_fio(&c__1, ch__3, (ftnlen)12);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L310;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

		    ++ntest;

/*                 Copy the matrices into packed storage. */

		    if (lsame_(uplo, "U")) {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = j;
			    for (i__ = 1; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L230: */
			    }
/* L240: */
			}
		    } else {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = n;
			    for (i__ = j; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L250: */
			    }
/* L260: */
			}
		    }

		    vl = 0.f;
		    vu = anorm;
		    chpgvx_(&ibtype, "V", "V", uplo, &n, &ap[1], &bp[1], &vl, 
			    &vu, &il, &iu, &abstol, &m, &d__[1], &z__[
			    z_offset], ldz, &work[1], &rwork[1], &iwork[n + 1]
, &iwork[1], info);
		    if (iinfo != 0) {
			io___56.ciunit = *nounit;
			s_wsfe(&io___56);
/* Writing concatenation */
			i__6[0] = 10, a__1[0] = "CHPGVX(V,V";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)12);
			do_fio(&c__1, ch__3, (ftnlen)12);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L310;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &m, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

		    ++ntest;

/*                 Copy the matrices into packed storage. */

		    if (lsame_(uplo, "U")) {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = j;
			    for (i__ = 1; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L270: */
			    }
/* L280: */
			}
		    } else {
			ij = 1;
			i__3 = n;
			for (j = 1; j <= i__3; ++j) {
			    i__4 = n;
			    for (i__ = j; i__ <= i__4; ++i__) {
				i__5 = ij;
				i__7 = i__ + j * a_dim1;
				ap[i__5].r = a[i__7].r, ap[i__5].i = a[i__7]
					.i;
				i__5 = ij;
				i__7 = i__ + j * b_dim1;
				bp[i__5].r = b[i__7].r, bp[i__5].i = b[i__7]
					.i;
				++ij;
/* L290: */
			    }
/* L300: */
			}
		    }

		    chpgvx_(&ibtype, "V", "I", uplo, &n, &ap[1], &bp[1], &vl, 
			    &vu, &il, &iu, &abstol, &m, &d__[1], &z__[
			    z_offset], ldz, &work[1], &rwork[1], &iwork[n + 1]
, &iwork[1], info);
		    if (iinfo != 0) {
			io___57.ciunit = *nounit;
			s_wsfe(&io___57);
/* Writing concatenation */
			i__6[0] = 10, a__1[0] = "CHPGVX(V,I";
			i__6[1] = 1, a__1[1] = uplo;
			i__6[2] = 1, a__1[2] = ")";
			s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)12);
			do_fio(&c__1, ch__3, (ftnlen)12);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			if (iinfo < 0) {
			    return 0;
			} else {
			    result[ntest] = ulpinv;
			    goto L310;
			}
		    }

/*                 Do Test */

		    csgt01_(&ibtype, uplo, &n, &m, &a[a_offset], lda, &b[
			    b_offset], ldb, &z__[z_offset], ldz, &d__[1], &
			    work[1], &rwork[1], &result[ntest]);

L310:

		    if (ibtype == 1) {

/*                    TEST CHBGV */

			++ntest;

/*                    Copy the matrices into band storage. */

			if (lsame_(uplo, "U")) {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
				i__4 = 1, i__5 = j - ka;
				i__7 = j;
				for (i__ = max(i__4,i__5); i__ <= i__7; ++i__)
					 {
				    i__4 = ka + 1 + i__ - j + j * ab_dim1;
				    i__5 = i__ + j * a_dim1;
				    ab[i__4].r = a[i__5].r, ab[i__4].i = a[
					    i__5].i;
/* L320: */
				}
/* Computing MAX */
				i__7 = 1, i__4 = j - kb;
				i__5 = j;
				for (i__ = max(i__7,i__4); i__ <= i__5; ++i__)
					 {
				    i__7 = kb + 1 + i__ - j + j * bb_dim1;
				    i__4 = i__ + j * b_dim1;
				    bb[i__7].r = b[i__4].r, bb[i__7].i = b[
					    i__4].i;
/* L330: */
				}
/* L340: */
			    }
			} else {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
				i__7 = n, i__4 = j + ka;
				i__5 = min(i__7,i__4);
				for (i__ = j; i__ <= i__5; ++i__) {
				    i__7 = i__ + 1 - j + j * ab_dim1;
				    i__4 = i__ + j * a_dim1;
				    ab[i__7].r = a[i__4].r, ab[i__7].i = a[
					    i__4].i;
/* L350: */
				}
/* Computing MIN */
				i__7 = n, i__4 = j + kb;
				i__5 = min(i__7,i__4);
				for (i__ = j; i__ <= i__5; ++i__) {
				    i__7 = i__ + 1 - j + j * bb_dim1;
				    i__4 = i__ + j * b_dim1;
				    bb[i__7].r = b[i__4].r, bb[i__7].i = b[
					    i__4].i;
/* L360: */
				}
/* L370: */
			    }
			}

			chbgv_("V", uplo, &n, &ka, &kb, &ab[ab_offset], lda, &
				bb[bb_offset], ldb, &d__[1], &z__[z_offset], 
				ldz, &work[1], &rwork[1], &iinfo);
			if (iinfo != 0) {
			    io___58.ciunit = *nounit;
			    s_wsfe(&io___58);
/* Writing concatenation */
			    i__6[0] = 8, a__1[0] = "CHBGV(V,";
			    i__6[1] = 1, a__1[1] = uplo;
			    i__6[2] = 1, a__1[2] = ")";
			    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
			    do_fio(&c__1, ch__1, (ftnlen)10);
			    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    *info = abs(iinfo);
			    if (iinfo < 0) {
				return 0;
			    } else {
				result[ntest] = ulpinv;
				goto L620;
			    }
			}

/*                    Do Test */

			csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
				b_offset], ldb, &z__[z_offset], ldz, &d__[1], 
				&work[1], &rwork[1], &result[ntest]);

/*                    TEST CHBGVD */

			++ntest;

/*                    Copy the matrices into band storage. */

			if (lsame_(uplo, "U")) {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
				i__5 = 1, i__7 = j - ka;
				i__4 = j;
				for (i__ = max(i__5,i__7); i__ <= i__4; ++i__)
					 {
				    i__5 = ka + 1 + i__ - j + j * ab_dim1;
				    i__7 = i__ + j * a_dim1;
				    ab[i__5].r = a[i__7].r, ab[i__5].i = a[
					    i__7].i;
/* L380: */
				}
/* Computing MAX */
				i__4 = 1, i__5 = j - kb;
				i__7 = j;
				for (i__ = max(i__4,i__5); i__ <= i__7; ++i__)
					 {
				    i__4 = kb + 1 + i__ - j + j * bb_dim1;
				    i__5 = i__ + j * b_dim1;
				    bb[i__4].r = b[i__5].r, bb[i__4].i = b[
					    i__5].i;
/* L390: */
				}
/* L400: */
			    }
			} else {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
				i__4 = n, i__5 = j + ka;
				i__7 = min(i__4,i__5);
				for (i__ = j; i__ <= i__7; ++i__) {
				    i__4 = i__ + 1 - j + j * ab_dim1;
				    i__5 = i__ + j * a_dim1;
				    ab[i__4].r = a[i__5].r, ab[i__4].i = a[
					    i__5].i;
/* L410: */
				}
/* Computing MIN */
				i__4 = n, i__5 = j + kb;
				i__7 = min(i__4,i__5);
				for (i__ = j; i__ <= i__7; ++i__) {
				    i__4 = i__ + 1 - j + j * bb_dim1;
				    i__5 = i__ + j * b_dim1;
				    bb[i__4].r = b[i__5].r, bb[i__4].i = b[
					    i__5].i;
/* L420: */
				}
/* L430: */
			    }
			}

			chbgvd_("V", uplo, &n, &ka, &kb, &ab[ab_offset], lda, 
				&bb[bb_offset], ldb, &d__[1], &z__[z_offset], 
				ldz, &work[1], nwork, &rwork[1], lrwork, &
				iwork[1], liwork, &iinfo);
			if (iinfo != 0) {
			    io___59.ciunit = *nounit;
			    s_wsfe(&io___59);
/* Writing concatenation */
			    i__6[0] = 9, a__1[0] = "CHBGVD(V,";
			    i__6[1] = 1, a__1[1] = uplo;
			    i__6[2] = 1, a__1[2] = ")";
			    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)11);
			    do_fio(&c__1, ch__2, (ftnlen)11);
			    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    *info = abs(iinfo);
			    if (iinfo < 0) {
				return 0;
			    } else {
				result[ntest] = ulpinv;
				goto L620;
			    }
			}

/*                    Do Test */

			csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
				b_offset], ldb, &z__[z_offset], ldz, &d__[1], 
				&work[1], &rwork[1], &result[ntest]);

/*                    Test CHBGVX */

			++ntest;

/*                    Copy the matrices into band storage. */

			if (lsame_(uplo, "U")) {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
				i__7 = 1, i__4 = j - ka;
				i__5 = j;
				for (i__ = max(i__7,i__4); i__ <= i__5; ++i__)
					 {
				    i__7 = ka + 1 + i__ - j + j * ab_dim1;
				    i__4 = i__ + j * a_dim1;
				    ab[i__7].r = a[i__4].r, ab[i__7].i = a[
					    i__4].i;
/* L440: */
				}
/* Computing MAX */
				i__5 = 1, i__7 = j - kb;
				i__4 = j;
				for (i__ = max(i__5,i__7); i__ <= i__4; ++i__)
					 {
				    i__5 = kb + 1 + i__ - j + j * bb_dim1;
				    i__7 = i__ + j * b_dim1;
				    bb[i__5].r = b[i__7].r, bb[i__5].i = b[
					    i__7].i;
/* L450: */
				}
/* L460: */
			    }
			} else {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
				i__5 = n, i__7 = j + ka;
				i__4 = min(i__5,i__7);
				for (i__ = j; i__ <= i__4; ++i__) {
				    i__5 = i__ + 1 - j + j * ab_dim1;
				    i__7 = i__ + j * a_dim1;
				    ab[i__5].r = a[i__7].r, ab[i__5].i = a[
					    i__7].i;
/* L470: */
				}
/* Computing MIN */
				i__5 = n, i__7 = j + kb;
				i__4 = min(i__5,i__7);
				for (i__ = j; i__ <= i__4; ++i__) {
				    i__5 = i__ + 1 - j + j * bb_dim1;
				    i__7 = i__ + j * b_dim1;
				    bb[i__5].r = b[i__7].r, bb[i__5].i = b[
					    i__7].i;
/* L480: */
				}
/* L490: */
			    }
			}

			i__3 = max(1,n);
			chbgvx_("V", "A", uplo, &n, &ka, &kb, &ab[ab_offset], 
				lda, &bb[bb_offset], ldb, &bp[1], &i__3, &vl, 
				&vu, &il, &iu, &abstol, &m, &d__[1], &z__[
				z_offset], ldz, &work[1], &rwork[1], &iwork[n 
				+ 1], &iwork[1], &iinfo);
			if (iinfo != 0) {
			    io___60.ciunit = *nounit;
			    s_wsfe(&io___60);
/* Writing concatenation */
			    i__6[0] = 10, a__1[0] = "CHBGVX(V,A";
			    i__6[1] = 1, a__1[1] = uplo;
			    i__6[2] = 1, a__1[2] = ")";
			    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)12);
			    do_fio(&c__1, ch__3, (ftnlen)12);
			    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    *info = abs(iinfo);
			    if (iinfo < 0) {
				return 0;
			    } else {
				result[ntest] = ulpinv;
				goto L620;
			    }
			}

/*                    Do Test */

			csgt01_(&ibtype, uplo, &n, &n, &a[a_offset], lda, &b[
				b_offset], ldb, &z__[z_offset], ldz, &d__[1], 
				&work[1], &rwork[1], &result[ntest]);

			++ntest;

/*                    Copy the matrices into band storage. */

			if (lsame_(uplo, "U")) {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
				i__4 = 1, i__5 = j - ka;
				i__7 = j;
				for (i__ = max(i__4,i__5); i__ <= i__7; ++i__)
					 {
				    i__4 = ka + 1 + i__ - j + j * ab_dim1;
				    i__5 = i__ + j * a_dim1;
				    ab[i__4].r = a[i__5].r, ab[i__4].i = a[
					    i__5].i;
/* L500: */
				}
/* Computing MAX */
				i__7 = 1, i__4 = j - kb;
				i__5 = j;
				for (i__ = max(i__7,i__4); i__ <= i__5; ++i__)
					 {
				    i__7 = kb + 1 + i__ - j + j * bb_dim1;
				    i__4 = i__ + j * b_dim1;
				    bb[i__7].r = b[i__4].r, bb[i__7].i = b[
					    i__4].i;
/* L510: */
				}
/* L520: */
			    }
			} else {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
				i__7 = n, i__4 = j + ka;
				i__5 = min(i__7,i__4);
				for (i__ = j; i__ <= i__5; ++i__) {
				    i__7 = i__ + 1 - j + j * ab_dim1;
				    i__4 = i__ + j * a_dim1;
				    ab[i__7].r = a[i__4].r, ab[i__7].i = a[
					    i__4].i;
/* L530: */
				}
/* Computing MIN */
				i__7 = n, i__4 = j + kb;
				i__5 = min(i__7,i__4);
				for (i__ = j; i__ <= i__5; ++i__) {
				    i__7 = i__ + 1 - j + j * bb_dim1;
				    i__4 = i__ + j * b_dim1;
				    bb[i__7].r = b[i__4].r, bb[i__7].i = b[
					    i__4].i;
/* L540: */
				}
/* L550: */
			    }
			}

			vl = 0.f;
			vu = anorm;
			i__3 = max(1,n);
			chbgvx_("V", "V", uplo, &n, &ka, &kb, &ab[ab_offset], 
				lda, &bb[bb_offset], ldb, &bp[1], &i__3, &vl, 
				&vu, &il, &iu, &abstol, &m, &d__[1], &z__[
				z_offset], ldz, &work[1], &rwork[1], &iwork[n 
				+ 1], &iwork[1], &iinfo);
			if (iinfo != 0) {
			    io___61.ciunit = *nounit;
			    s_wsfe(&io___61);
/* Writing concatenation */
			    i__6[0] = 10, a__1[0] = "CHBGVX(V,V";
			    i__6[1] = 1, a__1[1] = uplo;
			    i__6[2] = 1, a__1[2] = ")";
			    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)12);
			    do_fio(&c__1, ch__3, (ftnlen)12);
			    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    *info = abs(iinfo);
			    if (iinfo < 0) {
				return 0;
			    } else {
				result[ntest] = ulpinv;
				goto L620;
			    }
			}

/*                    Do Test */

			csgt01_(&ibtype, uplo, &n, &m, &a[a_offset], lda, &b[
				b_offset], ldb, &z__[z_offset], ldz, &d__[1], 
				&work[1], &rwork[1], &result[ntest]);

			++ntest;

/*                    Copy the matrices into band storage. */

			if (lsame_(uplo, "U")) {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
				i__5 = 1, i__7 = j - ka;
				i__4 = j;
				for (i__ = max(i__5,i__7); i__ <= i__4; ++i__)
					 {
				    i__5 = ka + 1 + i__ - j + j * ab_dim1;
				    i__7 = i__ + j * a_dim1;
				    ab[i__5].r = a[i__7].r, ab[i__5].i = a[
					    i__7].i;
/* L560: */
				}
/* Computing MAX */
				i__4 = 1, i__5 = j - kb;
				i__7 = j;
				for (i__ = max(i__4,i__5); i__ <= i__7; ++i__)
					 {
				    i__4 = kb + 1 + i__ - j + j * bb_dim1;
				    i__5 = i__ + j * b_dim1;
				    bb[i__4].r = b[i__5].r, bb[i__4].i = b[
					    i__5].i;
/* L570: */
				}
/* L580: */
			    }
			} else {
			    i__3 = n;
			    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
				i__4 = n, i__5 = j + ka;
				i__7 = min(i__4,i__5);
				for (i__ = j; i__ <= i__7; ++i__) {
				    i__4 = i__ + 1 - j + j * ab_dim1;
				    i__5 = i__ + j * a_dim1;
				    ab[i__4].r = a[i__5].r, ab[i__4].i = a[
					    i__5].i;
/* L590: */
				}
/* Computing MIN */
				i__4 = n, i__5 = j + kb;
				i__7 = min(i__4,i__5);
				for (i__ = j; i__ <= i__7; ++i__) {
				    i__4 = i__ + 1 - j + j * bb_dim1;
				    i__5 = i__ + j * b_dim1;
				    bb[i__4].r = b[i__5].r, bb[i__4].i = b[
					    i__5].i;
/* L600: */
				}
/* L610: */
			    }
			}

			i__3 = max(1,n);
			chbgvx_("V", "I", uplo, &n, &ka, &kb, &ab[ab_offset], 
				lda, &bb[bb_offset], ldb, &bp[1], &i__3, &vl, 
				&vu, &il, &iu, &abstol, &m, &d__[1], &z__[
				z_offset], ldz, &work[1], &rwork[1], &iwork[n 
				+ 1], &iwork[1], &iinfo);
			if (iinfo != 0) {
			    io___62.ciunit = *nounit;
			    s_wsfe(&io___62);
/* Writing concatenation */
			    i__6[0] = 10, a__1[0] = "CHBGVX(V,I";
			    i__6[1] = 1, a__1[1] = uplo;
			    i__6[2] = 1, a__1[2] = ")";
			    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)12);
			    do_fio(&c__1, ch__3, (ftnlen)12);
			    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			    *info = abs(iinfo);
			    if (iinfo < 0) {
				return 0;
			    } else {
				result[ntest] = ulpinv;
				goto L620;
			    }
			}

/*                    Do Test */

			csgt01_(&ibtype, uplo, &n, &m, &a[a_offset], lda, &b[
				b_offset], ldb, &z__[z_offset], ldz, &d__[1], 
				&work[1], &rwork[1], &result[ntest]);

		    }

L620:
		    ;
		}
/* L630: */
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

	    ntestt += ntest;
	    slafts_("CSG", &n, &n, &jtype, &ntest, &result[1], ioldsd, thresh, 
		     nounit, &nerrs);
L640:
	    ;
	}
/* L650: */
    }

/*     Summary */

    slasum_("CSG", nounit, &nerrs, &ntestt);

    return 0;


/*     End of CDRVSG */

} /* cdrvsg_ */
