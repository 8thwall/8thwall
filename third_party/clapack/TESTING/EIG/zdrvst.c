#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__2 = 2;
static integer c__0 = 0;
static integer c__6 = 6;
static doublereal c_b34 = 1.;
static integer c__1 = 1;
static doublereal c_b44 = 0.;
static integer c__4 = 4;
static integer c__3 = 3;

/* Subroutine */ int zdrvst_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, integer *nounit, 
	doublecomplex *a, integer *lda, doublereal *d1, doublereal *d2, 
	doublereal *d3, doublereal *wa1, doublereal *wa2, doublereal *wa3, 
	doublecomplex *u, integer *ldu, doublecomplex *v, doublecomplex *tau, 
	doublecomplex *z__, doublecomplex *work, integer *lwork, doublereal *
	rwork, integer *lrwork, integer *iwork, integer *liwork, doublereal *
	result, integer *info)
{
    /* Initialized data */

    static integer ktype[18] = { 1,2,4,4,4,4,4,5,5,5,5,5,8,8,8,9,9,9 };
    static integer kmagn[18] = { 1,1,1,1,1,2,3,1,1,1,2,3,1,2,3,1,2,3 };
    static integer kmode[18] = { 0,0,4,3,1,4,4,4,3,1,4,4,0,0,0,4,4,4 };

    /* Format strings */
    static char fmt_9999[] = "(\002 ZDRVST: \002,a,\002 returned INFO=\002,i"
	    "6,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED=(\002,3(i5"
	    ",\002,\002),i5,\002)\002)";
    static char fmt_9998[] = "(\002 ZDRVST: \002,a,\002 returned INFO=\002,i"
	    "6,/9x,\002N=\002,i6,\002, KD=\002,i6,\002, JTYPE=\002,i6,\002, I"
	    "SEED=(\002,3(i5,\002,\002),i5,\002)\002)";

    /* System generated locals */
    address a__1[3];
    integer a_dim1, a_offset, u_dim1, u_offset, v_dim1, v_offset, z_dim1, 
	    z_offset, i__1, i__2, i__3, i__4, i__5, i__6, i__7[3];
    doublereal d__1, d__2, d__3, d__4;
    char ch__1[11], ch__2[13], ch__3[10];

    /* Builtin functions */
    double sqrt(doublereal), log(doublereal);
    integer pow_ii(integer *, integer *), s_wsfe(cilist *), do_fio(integer *, 
	    char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, j, m, n, j1, j2, m2, m3, kd, il, iu;
    doublereal vl, vu;
    integer lgn;
    doublereal ulp, cond;
    integer jcol, ihbw, indx, nmax;
    doublereal unfl, ovfl;
    char uplo[1];
    integer irow;
    doublereal temp1, temp2, temp3;
    extern doublereal dsxt1_(integer *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, doublereal *, doublereal *);
    integer idiag;
    logical badnn;
    integer imode, lwedc, iinfo;
    doublereal aninv, anorm;
    extern /* Subroutine */ int zhet21_(integer *, char *, integer *, integer 
	    *, doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublereal *, doublereal *);
    integer itemp;
    extern /* Subroutine */ int zhbev_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *, doublecomplex *, doublereal *, integer *), zhet22_(integer *, char *, integer *, integer *, integer 
	    *, doublecomplex *, integer *, doublereal *, doublereal *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublereal *, doublereal *), zheev_(char *, char *, integer *, doublecomplex *, 
	    integer *, doublereal *, doublecomplex *, integer *, doublereal *, 
	     integer *);
    integer nmats, jsize, iuplo, nerrs, itype, jtype, ntest;
    extern /* Subroutine */ int zhpev_(char *, char *, integer *, 
	    doublecomplex *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, doublereal *, integer *);
    integer iseed2[4], iseed3[4];
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlarnd_(integer *, integer *);
    integer liwedc, idumma[1];
    extern /* Subroutine */ int dlafts_(char *, integer *, integer *, integer 
	    *, integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *);
    integer ioldsd[4];
    extern /* Subroutine */ int xerbla_(char *, integer *);
    integer lrwedc;
    extern /* Subroutine */ int zhbevd_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *), alasvm_(char *, 
	    integer *, integer *, integer *, integer *);
    doublereal abstol;
    extern /* Subroutine */ int zheevd_(char *, char *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *, doublereal *, integer *, integer *, integer *, integer 
	    *);
    integer indwrk;
    extern /* Subroutine */ int zhpevd_(char *, char *, integer *, 
	    doublecomplex *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    integer *, integer *), zlacpy_(char *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *), zheevr_(char *, char *, char *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublereal *, integer *, 
	     integer *, doublereal *, integer *, doublereal *, doublecomplex *
, integer *, integer *, doublecomplex *, integer *, doublereal *, 
	    integer *, integer *, integer *, integer *), zlaset_(char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, integer *), zhbevx_(
	    char *, char *, char *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublecomplex *, integer *, doublecomplex *, doublereal *, 
	    integer *, integer *, integer *), zheevx_(
	    char *, char *, char *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    integer *);
    doublereal rtunfl, rtovfl, ulpinv;
    integer mtypes, ntestt;
    extern /* Subroutine */ int zhpevx_(char *, char *, char *, integer *, 
	    doublecomplex *, doublereal *, doublereal *, integer *, integer *, 
	     doublereal *, integer *, doublereal *, doublecomplex *, integer *
, doublecomplex *, doublereal *, integer *, integer *, integer *), zlatmr_(integer *, integer *, char *, 
	    integer *, char *, doublecomplex *, integer *, doublereal *, 
	    doublecomplex *, char *, char *, doublecomplex *, integer *, 
	    doublereal *, doublecomplex *, integer *, doublereal *, char *, 
	    integer *, integer *, integer *, doublereal *, doublereal *, char 
	    *, doublecomplex *, integer *, integer *, integer *), zlatms_(integer *, 
	    integer *, char *, integer *, char *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, char *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);

    /* Fortran I/O blocks */
    static cilist io___42 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___71 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___84 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___85 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___86 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___87 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___88 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___89 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___90 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___91 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___92 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___93 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___94 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___95 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*       ZDRVST  checks the Hermitian eigenvalue problem drivers. */

/*               ZHEEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian matrix, */
/*               using a divide-and-conquer algorithm. */

/*               ZHEEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian matrix. */

/*               ZHEEVR computes selected eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian matrix */
/*               using the Relatively Robust Representation where it can. */

/*               ZHPEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian matrix in packed */
/*               storage, using a divide-and-conquer algorithm. */

/*               ZHPEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian matrix in packed */
/*               storage. */

/*               ZHBEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian band matrix, */
/*               using a divide-and-conquer algorithm. */

/*               ZHBEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian band matrix. */

/*               ZHEEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian matrix. */

/*               ZHPEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian matrix in packed */
/*               storage. */

/*               ZHBEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a complex Hermitian band matrix. */

/*       When ZDRVST is called, a number of matrix "sizes" ("n's") and a */
/*       number of matrix "types" are specified.  For each size ("n") */
/*       and each type of matrix, one matrix will be generated and used */
/*       to test the appropriate drivers.  For each matrix and each */
/*       driver routine called, the following tests will be performed: */

/*       (1)     | A - Z D Z' | / ( |A| n ulp ) */

/*       (2)     | I - Z Z' | / ( n ulp ) */

/*       (3)     | D1 - D2 | / ( |D1| ulp ) */

/*       where Z is the matrix of eigenvectors returned when the */
/*       eigenvector option is given and D1 and D2 are the eigenvalues */
/*       returned with and without the eigenvector option. */

/*       The "sizes" are specified by an array NN(1:NSIZES); the value of */
/*       each element NN(j) specifies one size. */
/*       The "types" are specified by a logical array DOTYPE( 1:NTYPES ); */
/*       if DOTYPE(j) is .TRUE., then matrix type "j" will be generated. */
/*       Currently, the list of possible types is: */

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

/*       (13) Symmetric matrix with random entries chosen from (-1,1). */
/*       (14) Same as (13), but multiplied by SQRT( overflow threshold ) */
/*       (15) Same as (13), but multiplied by SQRT( underflow threshold ) */
/*       (16) A band matrix with half bandwidth randomly chosen between */
/*            0 and N-1, with evenly spaced eigenvalues 1, ..., ULP */
/*            with random signs. */
/*       (17) Same as (16), but multiplied by SQRT( overflow threshold ) */
/*       (18) Same as (16), but multiplied by SQRT( underflow threshold ) */

/*  Arguments */
/*  ========= */

/*  NSIZES  INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          ZDRVST does nothing.  It must be at least zero. */
/*          Not modified. */

/*  NN      INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */
/*          Not modified. */

/*  NTYPES  INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, ZDRVST */
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
/*          next call to ZDRVST to continue the same random number */
/*          sequence. */
/*          Modified. */

/*  THRESH  DOUBLE PRECISION */
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

/*  A       COMPLEX*16 array, dimension (LDA , max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually */
/*          used. */
/*          Modified. */

/*  LDA     INTEGER */
/*          The leading dimension of A.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  D1      DOUBLE PRECISION array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by ZSTEQR simlutaneously */
/*          with Z.  On exit, the eigenvalues in D1 correspond with the */
/*          matrix in A. */
/*          Modified. */

/*  D2      DOUBLE PRECISION array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by ZSTEQR if Z is not */
/*          computed.  On exit, the eigenvalues in D2 correspond with */
/*          the matrix in A. */
/*          Modified. */

/*  D3      DOUBLE PRECISION array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by DSTERF.  On exit, the */
/*          eigenvalues in D3 correspond with the matrix in A. */
/*          Modified. */

/*  WA1     DOUBLE PRECISION array, dimension */

/*  WA2     DOUBLE PRECISION array, dimension */

/*  WA3     DOUBLE PRECISION array, dimension */

/*  U       COMPLEX*16 array, dimension (LDU, max(NN)) */
/*          The unitary matrix computed by ZHETRD + ZUNGC3. */
/*          Modified. */

/*  LDU     INTEGER */
/*          The leading dimension of U, Z, and V.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  V       COMPLEX*16 array, dimension (LDU, max(NN)) */
/*          The Housholder vectors computed by ZHETRD in reducing A to */
/*          tridiagonal form. */
/*          Modified. */

/*  TAU     COMPLEX*16 array, dimension (max(NN)) */
/*          The Householder factors computed by ZHETRD in reducing A */
/*          to tridiagonal form. */
/*          Modified. */

/*  Z       COMPLEX*16 array, dimension (LDU, max(NN)) */
/*          The unitary matrix of eigenvectors computed by ZHEEVD, */
/*          ZHEEVX, ZHPEVD, CHPEVX, ZHBEVD, and CHBEVX. */
/*          Modified. */

/*  WORK  - COMPLEX*16 array of dimension ( LWORK ) */
/*           Workspace. */
/*           Modified. */

/*  LWORK - INTEGER */
/*           The number of entries in WORK.  This must be at least */
/*           2*max( NN(j), 2 )**2. */
/*           Not modified. */

/*  RWORK   DOUBLE PRECISION array, dimension (3*max(NN)) */
/*           Workspace. */
/*           Modified. */

/*  LRWORK - INTEGER */
/*           The number of entries in RWORK. */

/*  IWORK   INTEGER array, dimension (6*max(NN)) */
/*          Workspace. */
/*          Modified. */

/*  LIWORK - INTEGER */
/*           The number of entries in IWORK. */

/*  RESULT  DOUBLE PRECISION array, dimension (??) */
/*          The values computed by the tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */
/*          Modified. */

/*  INFO    INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some NN(j) < 0 */
/*           -3: NTYPES < 0 */
/*           -5: THRESH < 0 */
/*           -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*          -16: LDU < 1 or LDU < NMAX. */
/*          -21: LWORK too small. */
/*          If  DLATMR, SLATMS, ZHETRD, DORGC3, ZSTEQR, DSTERF, */
/*              or DORMC2 returns an error code, the */
/*              absolute value of it is returned. */
/*          Modified. */

/* ----------------------------------------------------------------------- */

/*       Some Local Variables and Parameters: */
/*       ---- ----- --------- --- ---------- */
/*       ZERO, ONE       Real 0 and 1. */
/*       MAXTYP          The number of types defined. */
/*       NTEST           The number of tests performed, or which can */
/*                       be performed so far, for the current matrix. */
/*       NTESTT          The total number of tests performed so far. */
/*       NMAX            Largest value in NN. */
/*       NMATS           The number of matrices generated so far. */
/*       NERRS           The number of tests which have exceeded THRESH */
/*                       so far (computed by DLAFTS). */
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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --d1;
    --d2;
    --d3;
    --wa1;
    --wa2;
    --wa3;
    z_dim1 = *ldu;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    v_dim1 = *ldu;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --tau;
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
    nmax = 1;
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
    } else if (*lda < nmax) {
	*info = -9;
    } else if (*ldu < nmax) {
	*info = -16;
    } else /* if(complicated condition) */ {
/* Computing 2nd power */
	i__1 = max(2,nmax);
	if (i__1 * i__1 << 1 > *lwork) {
	    *info = -22;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("ZDRVST", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

/*     More Important constants */

    unfl = dlamch_("Safe minimum");
    ovfl = dlamch_("Overflow");
    dlabad_(&unfl, &ovfl);
    ulp = dlamch_("Epsilon") * dlamch_("Base");
    ulpinv = 1. / ulp;
    rtunfl = sqrt(unfl);
    rtovfl = sqrt(ovfl);

/*     Loop over sizes, types */

    for (i__ = 1; i__ <= 4; ++i__) {
	iseed2[i__ - 1] = iseed[i__];
	iseed3[i__ - 1] = iseed[i__];
/* L20: */
    }

    nerrs = 0;
    nmats = 0;

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	n = nn[jsize];
	if (n > 0) {
	    lgn = (integer) (log((doublereal) n) / log(2.));
	    if (pow_ii(&c__2, &lgn) < n) {
		++lgn;
	    }
	    if (pow_ii(&c__2, &lgn) < n) {
		++lgn;
	    }
/* Computing MAX */
	    i__2 = (n << 1) + n * n, i__3 = (n << 1) * n;
	    lwedc = max(i__2,i__3);
/* Computing 2nd power */
	    i__2 = n;
	    lrwedc = (n << 2) + 1 + (n << 1) * lgn + i__2 * i__2 * 3;
	    liwedc = n * 5 + 3;
	} else {
	    lwedc = 2;
	    lrwedc = 8;
	    liwedc = 8;
	}
	aninv = 1. / (doublereal) max(1,n);

	if (*nsizes != 1) {
	    mtypes = min(18,*ntypes);
	} else {
	    mtypes = min(19,*ntypes);
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L1210;
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
/*           =4         arithmetic   diagonal, (w/ eigenvalues) */
/*           =5         random log   Hermitian, w/ eigenvalues */
/*           =6         random       (none) */
/*           =7                      random diagonal */
/*           =8                      random Hermitian */
/*           =9                      band Hermitian, w/ eigenvalues */

	    if (mtypes > 18) {
		goto L110;
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
	    anorm = 1.;
	    goto L70;

L50:
	    anorm = rtovfl * ulp * aninv;
	    goto L70;

L60:
	    anorm = rtunfl * n * ulpinv;
	    goto L70;

L70:

	    zlaset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);
	    iinfo = 0;
	    cond = ulpinv;

/*           Special Matrices -- Identity & Jordan block */

/*                   Zero */

	    if (itype == 1) {
		iinfo = 0;

	    } else if (itype == 2) {

/*              Identity */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    a[i__4].r = anorm, a[i__4].i = 0.;
/* L80: */
		}

	    } else if (itype == 4) {

/*              Diagonal Matrix, [Eigen]values Specified */

		zlatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &c__0, &c__0, "N", &a[a_offset], lda, &work[
			1], &iinfo);

	    } else if (itype == 5) {

/*              Hermitian, eigenvalues specified */

		zlatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &n, &n, "N", &a[a_offset], lda, &work[1], &
			iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		zlatmr_(&n, &n, "S", &iseed[1], "H", &work[1], &c__6, &c_b34, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b34, &work[(
			n << 1) + 1], &c__1, &c_b34, "N", idumma, &c__0, &
			c__0, &c_b44, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Hermitian, random eigenvalues */

		zlatmr_(&n, &n, "S", &iseed[1], "H", &work[1], &c__6, &c_b34, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b34, &work[(
			n << 1) + 1], &c__1, &c_b34, "N", idumma, &n, &n, &
			c_b44, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              Hermitian banded, eigenvalues specified */

		ihbw = (integer) ((n - 1) * dlarnd_(&c__1, iseed3));
		zlatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &ihbw, &ihbw, "Z", &u[u_offset], ldu, &work[
			1], &iinfo);

/*              Store as dense matrix for most routines. */

		zlaset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);
		i__3 = ihbw;
		for (idiag = -ihbw; idiag <= i__3; ++idiag) {
		    irow = ihbw - idiag + 1;
/* Computing MAX */
		    i__4 = 1, i__5 = idiag + 1;
		    j1 = max(i__4,i__5);
/* Computing MIN */
		    i__4 = n, i__5 = n + idiag;
		    j2 = min(i__4,i__5);
		    i__4 = j2;
		    for (j = j1; j <= i__4; ++j) {
			i__ = j - idiag;
			i__5 = i__ + j * a_dim1;
			i__6 = irow + j * u_dim1;
			a[i__5].r = u[i__6].r, a[i__5].i = u[i__6].i;
/* L90: */
		    }
/* L100: */
		}
	    } else {
		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___42.ciunit = *nounit;
		s_wsfe(&io___42);
		do_fio(&c__1, "Generator", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		return 0;
	    }

L110:

	    abstol = unfl + unfl;
	    if (n <= 1) {
		il = 1;
		iu = n;
	    } else {
		il = (integer) ((n - 1) * dlarnd_(&c__1, iseed2)) + 1;
		iu = (integer) ((n - 1) * dlarnd_(&c__1, iseed2)) + 1;
		if (il > iu) {
		    itemp = il;
		    il = iu;
		    iu = itemp;
		}
	    }

/*           Perform tests storing upper or lower triangular */
/*           part of matrix. */

	    for (iuplo = 0; iuplo <= 1; ++iuplo) {
		if (iuplo == 0) {
		    *(unsigned char *)uplo = 'L';
		} else {
		    *(unsigned char *)uplo = 'U';
		}

/*              Call ZHEEVD and CHEEVX. */

		zlacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

		++ntest;
		zheevd_("V", uplo, &n, &a[a_offset], ldu, &d1[1], &work[1], &
			lwedc, &rwork[1], &lrwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___49.ciunit = *nounit;
		    s_wsfe(&io___49);
/* Writing concatenation */
		    i__7[0] = 9, a__1[0] = "ZHEEVD(V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__7, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__1, (ftnlen)11);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L130;
		    }
		}

/*              Do tests 1 and 2. */

		zhet21_(&c__1, uplo, &n, &c__0, &v[v_offset], ldu, &d1[1], &
			d2[1], &a[a_offset], ldu, &z__[z_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ntest += 2;
		zheevd_("N", uplo, &n, &a[a_offset], ldu, &d3[1], &work[1], &
			lwedc, &rwork[1], &lrwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___50.ciunit = *nounit;
		    s_wsfe(&io___50);
/* Writing concatenation */
		    i__7[0] = 9, a__1[0] = "ZHEEVD(N,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__7, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__1, (ftnlen)11);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L130;
		    }
		}

/*              Do test 3. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d1[j] - d3[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L120: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L130:
		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		++ntest;

		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(d1[1]), d__3 = (d__1 = d1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		    if (il != 1) {
/* Computing MAX */
			d__1 = (d1[il] - d1[il - 1]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vl = d1[il] - max(d__1,d__2);
		    } else if (n > 0) {
/* Computing MAX */
			d__1 = (d1[n] - d1[1]) * .5, d__2 = ulp * 10. * temp3,
				 d__1 = max(d__1,d__2), d__2 = rtunfl * 10.;
			vl = d1[1] - max(d__1,d__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			d__1 = (d1[iu + 1] - d1[iu]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vu = d1[iu] + max(d__1,d__2);
		    } else if (n > 0) {
/* Computing MAX */
			d__1 = (d1[n] - d1[1]) * .5, d__2 = ulp * 10. * temp3,
				 d__1 = max(d__1,d__2), d__2 = rtunfl * 10.;
			vu = d1[n] + max(d__1,d__2);
		    }
		} else {
		    temp3 = 0.;
		    vl = 0.;
		    vu = 1.;
		}

		zheevx_("V", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m, &wa1[1], &z__[z_offset], ldu, &work[
			1], lwork, &rwork[1], &iwork[1], &iwork[n * 5 + 1], &
			iinfo);
		if (iinfo != 0) {
		    io___57.ciunit = *nounit;
		    s_wsfe(&io___57);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVX(V,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L150;
		    }
		}

/*              Do tests 4 and 5. */

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		ntest += 2;
		zheevx_("N", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &rwork[1], &iwork[1], &iwork[n * 5 + 
			1], &iinfo);
		if (iinfo != 0) {
		    io___59.ciunit = *nounit;
		    s_wsfe(&io___59);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVX(N,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L150;
		    }
		}

/*              Do test 6. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = wa1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = wa2[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = wa1[j] - wa2[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L140: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L150:
		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		++ntest;

		zheevx_("V", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &rwork[1], &iwork[1], &iwork[n * 5 + 
			1], &iinfo);
		if (iinfo != 0) {
		    io___60.ciunit = *nounit;
		    s_wsfe(&io___60);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVX(V,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L160;
		    }
		}

/*              Do tests 7 and 8. */

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		zheevx_("N", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			work[1], lwork, &rwork[1], &iwork[1], &iwork[n * 5 + 
			1], &iinfo);
		if (iinfo != 0) {
		    io___62.ciunit = *nounit;
		    s_wsfe(&io___62);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVX(N,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L160;
		    }
		}

/*              Do test 9. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);

L160:
		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		++ntest;

		zheevx_("V", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &rwork[1], &iwork[1], &iwork[n * 5 + 
			1], &iinfo);
		if (iinfo != 0) {
		    io___63.ciunit = *nounit;
		    s_wsfe(&io___63);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVX(V,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L170;
		    }
		}

/*              Do tests 10 and 11. */

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		zheevx_("N", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			work[1], lwork, &rwork[1], &iwork[1], &iwork[n * 5 + 
			1], &iinfo);
		if (iinfo != 0) {
		    io___64.ciunit = *nounit;
		    s_wsfe(&io___64);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVX(N,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L170;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L170;
		}

/*              Do test 12. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);

L170:

/*              Call ZHPEVD and CHPEVX. */

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

/*              Load array WORK with the upper or lower triangular */
/*              part of the matrix in packed form. */

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L180: */
			}
/* L190: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L200: */
			}
/* L210: */
		    }
		}

		++ntest;
		indwrk = n * (n + 1) / 2 + 1;
		zhpevd_("V", uplo, &n, &work[1], &d1[1], &z__[z_offset], ldu, 
			&work[indwrk], &lwedc, &rwork[1], &lrwedc, &iwork[1], 
			&liwedc, &iinfo);
		if (iinfo != 0) {
		    io___67.ciunit = *nounit;
		    s_wsfe(&io___67);
/* Writing concatenation */
		    i__7[0] = 9, a__1[0] = "ZHPEVD(V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__7, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__1, (ftnlen)11);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L270;
		    }
		}

/*              Do tests 13 and 14. */

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L220: */
			}
/* L230: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L240: */
			}
/* L250: */
		    }
		}

		ntest += 2;
		indwrk = n * (n + 1) / 2 + 1;
		zhpevd_("N", uplo, &n, &work[1], &d3[1], &z__[z_offset], ldu, 
			&work[indwrk], &lwedc, &rwork[1], &lrwedc, &iwork[1], 
			&liwedc, &iinfo);
		if (iinfo != 0) {
		    io___68.ciunit = *nounit;
		    s_wsfe(&io___68);
/* Writing concatenation */
		    i__7[0] = 9, a__1[0] = "ZHPEVD(N,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__7, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__1, (ftnlen)11);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L270;
		    }
		}

/*              Do test 15. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d1[j] - d3[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L260: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

/*              Load array WORK with the upper or lower triangular part */
/*              of the matrix in packed form. */

L270:
		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L280: */
			}
/* L290: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L300: */
			}
/* L310: */
		    }
		}

		++ntest;

		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(d1[1]), d__3 = (d__1 = d1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		    if (il != 1) {
/* Computing MAX */
			d__1 = (d1[il] - d1[il - 1]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vl = d1[il] - max(d__1,d__2);
		    } else if (n > 0) {
/* Computing MAX */
			d__1 = (d1[n] - d1[1]) * .5, d__2 = ulp * 10. * temp3,
				 d__1 = max(d__1,d__2), d__2 = rtunfl * 10.;
			vl = d1[1] - max(d__1,d__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			d__1 = (d1[iu + 1] - d1[iu]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vu = d1[iu] + max(d__1,d__2);
		    } else if (n > 0) {
/* Computing MAX */
			d__1 = (d1[n] - d1[1]) * .5, d__2 = ulp * 10. * temp3,
				 d__1 = max(d__1,d__2), d__2 = rtunfl * 10.;
			vu = d1[n] + max(d__1,d__2);
		    }
		} else {
		    temp3 = 0.;
		    vl = 0.;
		    vu = 1.;
		}

		zhpevx_("V", "A", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m, &wa1[1], &z__[z_offset], ldu, &v[v_offset]
, &rwork[1], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___69.ciunit = *nounit;
		    s_wsfe(&io___69);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHPEVX(V,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L370;
		    }
		}

/*              Do tests 16 and 17. */

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L320: */
			}
/* L330: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L340: */
			}
/* L350: */
		    }
		}

		zhpevx_("N", "A", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &rwork[1], &iwork[1], &iwork[n * 5 + 1], &
			iinfo);
		if (iinfo != 0) {
		    io___70.ciunit = *nounit;
		    s_wsfe(&io___70);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHPEVX(N,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L370;
		    }
		}

/*              Do test 18. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = wa1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = wa2[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = wa1[j] - wa2[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L360: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L370:
		++ntest;
		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L380: */
			}
/* L390: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L400: */
			}
/* L410: */
		    }
		}

		zhpevx_("V", "I", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &rwork[1], &iwork[1], &iwork[n * 5 + 1], &
			iinfo);
		if (iinfo != 0) {
		    io___71.ciunit = *nounit;
		    s_wsfe(&io___71);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHPEVX(V,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L460;
		    }
		}

/*              Do tests 19 and 20. */

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L420: */
			}
/* L430: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L440: */
			}
/* L450: */
		    }
		}

		zhpevx_("N", "I", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &v[
			v_offset], &rwork[1], &iwork[1], &iwork[n * 5 + 1], &
			iinfo);
		if (iinfo != 0) {
		    io___72.ciunit = *nounit;
		    s_wsfe(&io___72);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHPEVX(N,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L460;
		    }
		}

/*              Do test 21. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);

L460:
		++ntest;
		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L470: */
			}
/* L480: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L490: */
			}
/* L500: */
		    }
		}

		zhpevx_("V", "V", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &rwork[1], &iwork[1], &iwork[n * 5 + 1], &
			iinfo);
		if (iinfo != 0) {
		    io___73.ciunit = *nounit;
		    s_wsfe(&io___73);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHPEVX(V,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L550;
		    }
		}

/*              Do tests 22 and 23. */

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L510: */
			}
/* L520: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = indx;
			    i__6 = i__ + j * a_dim1;
			    work[i__5].r = a[i__6].r, work[i__5].i = a[i__6]
				    .i;
			    ++indx;
/* L530: */
			}
/* L540: */
		    }
		}

		zhpevx_("N", "V", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &v[
			v_offset], &rwork[1], &iwork[1], &iwork[n * 5 + 1], &
			iinfo);
		if (iinfo != 0) {
		    io___74.ciunit = *nounit;
		    s_wsfe(&io___74);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHPEVX(N,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L550;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L550;
		}

/*              Do test 24. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);

L550:

/*              Call ZHBEVD and CHBEVX. */

		if (jtype <= 7) {
		    kd = 0;
		} else if (jtype >= 8 && jtype <= 15) {
/* Computing MAX */
		    i__3 = n - 1;
		    kd = max(i__3,0);
		} else {
		    kd = ihbw;
		}

/*              Load array V with the upper or lower triangular part */
/*              of the matrix in band form. */

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__4 = 1, i__5 = j - kd;
			i__6 = j;
			for (i__ = max(i__4,i__5); i__ <= i__6; ++i__) {
			    i__4 = kd + 1 + i__ - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L560: */
			}
/* L570: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__6 = min(i__4,i__5);
			for (i__ = j; i__ <= i__6; ++i__) {
			    i__4 = i__ + 1 - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L580: */
			}
/* L590: */
		    }
		}

		++ntest;
		zhbevd_("V", uplo, &n, &kd, &v[v_offset], ldu, &d1[1], &z__[
			z_offset], ldu, &work[1], &lwedc, &rwork[1], &lrwedc, 
			&iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___76.ciunit = *nounit;
		    s_wsfe(&io___76);
/* Writing concatenation */
		    i__7[0] = 9, a__1[0] = "ZHBEVD(V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__7, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__1, (ftnlen)11);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L650;
		    }
		}

/*              Do tests 25 and 26. */

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__6 = 1, i__4 = j - kd;
			i__5 = j;
			for (i__ = max(i__6,i__4); i__ <= i__5; ++i__) {
			    i__6 = kd + 1 + i__ - j + j * v_dim1;
			    i__4 = i__ + j * a_dim1;
			    v[i__6].r = a[i__4].r, v[i__6].i = a[i__4].i;
/* L600: */
			}
/* L610: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__6 = n, i__4 = j + kd;
			i__5 = min(i__6,i__4);
			for (i__ = j; i__ <= i__5; ++i__) {
			    i__6 = i__ + 1 - j + j * v_dim1;
			    i__4 = i__ + j * a_dim1;
			    v[i__6].r = a[i__4].r, v[i__6].i = a[i__4].i;
/* L620: */
			}
/* L630: */
		    }
		}

		ntest += 2;
		zhbevd_("N", uplo, &n, &kd, &v[v_offset], ldu, &d3[1], &z__[
			z_offset], ldu, &work[1], &lwedc, &rwork[1], &lrwedc, 
			&iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___77.ciunit = *nounit;
		    s_wsfe(&io___77);
/* Writing concatenation */
		    i__7[0] = 9, a__1[0] = "ZHBEVD(N,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__7, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__1, (ftnlen)11);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L650;
		    }
		}

/*              Do test 27. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d1[j] - d3[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L640: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

/*              Load array V with the upper or lower triangular part */
/*              of the matrix in band form. */

L650:
		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__5 = 1, i__6 = j - kd;
			i__4 = j;
			for (i__ = max(i__5,i__6); i__ <= i__4; ++i__) {
			    i__5 = kd + 1 + i__ - j + j * v_dim1;
			    i__6 = i__ + j * a_dim1;
			    v[i__5].r = a[i__6].r, v[i__5].i = a[i__6].i;
/* L660: */
			}
/* L670: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__5 = n, i__6 = j + kd;
			i__4 = min(i__5,i__6);
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = i__ + 1 - j + j * v_dim1;
			    i__6 = i__ + j * a_dim1;
			    v[i__5].r = a[i__6].r, v[i__5].i = a[i__6].i;
/* L680: */
			}
/* L690: */
		    }
		}

		++ntest;
		zhbevx_("V", "A", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m, &wa1[
			1], &z__[z_offset], ldu, &work[1], &rwork[1], &iwork[
			1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___78.ciunit = *nounit;
		    s_wsfe(&io___78);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHBEVX(V,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L750;
		    }
		}

/*              Do tests 28 and 29. */

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__4 = 1, i__5 = j - kd;
			i__6 = j;
			for (i__ = max(i__4,i__5); i__ <= i__6; ++i__) {
			    i__4 = kd + 1 + i__ - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L700: */
			}
/* L710: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__6 = min(i__4,i__5);
			for (i__ = j; i__ <= i__6; ++i__) {
			    i__4 = i__ + 1 - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L720: */
			}
/* L730: */
		    }
		}

		zhbevx_("N", "A", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m2, &
			wa2[1], &z__[z_offset], ldu, &work[1], &rwork[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___79.ciunit = *nounit;
		    s_wsfe(&io___79);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHBEVX(N,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L750;
		    }
		}

/*              Do test 30. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = wa1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = wa2[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = wa1[j] - wa2[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L740: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

/*              Load array V with the upper or lower triangular part */
/*              of the matrix in band form. */

L750:
		++ntest;
		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__6 = 1, i__4 = j - kd;
			i__5 = j;
			for (i__ = max(i__6,i__4); i__ <= i__5; ++i__) {
			    i__6 = kd + 1 + i__ - j + j * v_dim1;
			    i__4 = i__ + j * a_dim1;
			    v[i__6].r = a[i__4].r, v[i__6].i = a[i__4].i;
/* L760: */
			}
/* L770: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__6 = n, i__4 = j + kd;
			i__5 = min(i__6,i__4);
			for (i__ = j; i__ <= i__5; ++i__) {
			    i__6 = i__ + 1 - j + j * v_dim1;
			    i__4 = i__ + j * a_dim1;
			    v[i__6].r = a[i__4].r, v[i__6].i = a[i__4].i;
/* L780: */
			}
/* L790: */
		    }
		}

		zhbevx_("V", "I", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m2, &
			wa2[1], &z__[z_offset], ldu, &work[1], &rwork[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___80.ciunit = *nounit;
		    s_wsfe(&io___80);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHBEVX(V,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L840;
		    }
		}

/*              Do tests 31 and 32. */

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__5 = 1, i__6 = j - kd;
			i__4 = j;
			for (i__ = max(i__5,i__6); i__ <= i__4; ++i__) {
			    i__5 = kd + 1 + i__ - j + j * v_dim1;
			    i__6 = i__ + j * a_dim1;
			    v[i__5].r = a[i__6].r, v[i__5].i = a[i__6].i;
/* L800: */
			}
/* L810: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__5 = n, i__6 = j + kd;
			i__4 = min(i__5,i__6);
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = i__ + 1 - j + j * v_dim1;
			    i__6 = i__ + j * a_dim1;
			    v[i__5].r = a[i__6].r, v[i__5].i = a[i__6].i;
/* L820: */
			}
/* L830: */
		    }
		}
		zhbevx_("N", "I", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &rwork[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___81.ciunit = *nounit;
		    s_wsfe(&io___81);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHBEVX(N,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L840;
		    }
		}

/*              Do test 33. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);

/*              Load array V with the upper or lower triangular part */
/*              of the matrix in band form. */

L840:
		++ntest;
		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__4 = 1, i__5 = j - kd;
			i__6 = j;
			for (i__ = max(i__4,i__5); i__ <= i__6; ++i__) {
			    i__4 = kd + 1 + i__ - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L850: */
			}
/* L860: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__6 = min(i__4,i__5);
			for (i__ = j; i__ <= i__6; ++i__) {
			    i__4 = i__ + 1 - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L870: */
			}
/* L880: */
		    }
		}
		zhbevx_("V", "V", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m2, &
			wa2[1], &z__[z_offset], ldu, &work[1], &rwork[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___82.ciunit = *nounit;
		    s_wsfe(&io___82);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHBEVX(V,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L930;
		    }
		}

/*              Do tests 34 and 35. */

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__6 = 1, i__4 = j - kd;
			i__5 = j;
			for (i__ = max(i__6,i__4); i__ <= i__5; ++i__) {
			    i__6 = kd + 1 + i__ - j + j * v_dim1;
			    i__4 = i__ + j * a_dim1;
			    v[i__6].r = a[i__4].r, v[i__6].i = a[i__4].i;
/* L890: */
			}
/* L900: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__6 = n, i__4 = j + kd;
			i__5 = min(i__6,i__4);
			for (i__ = j; i__ <= i__5; ++i__) {
			    i__6 = i__ + 1 - j + j * v_dim1;
			    i__4 = i__ + j * a_dim1;
			    v[i__6].r = a[i__4].r, v[i__6].i = a[i__4].i;
/* L910: */
			}
/* L920: */
		    }
		}
		zhbevx_("N", "V", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &rwork[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___83.ciunit = *nounit;
		    s_wsfe(&io___83);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHBEVX(N,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L930;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L930;
		}

/*              Do test 36. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);

L930:

/*              Call ZHEEV */

		zlacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

		++ntest;
		zheev_("V", uplo, &n, &a[a_offset], ldu, &d1[1], &work[1], 
			lwork, &rwork[1], &iinfo);
		if (iinfo != 0) {
		    io___84.ciunit = *nounit;
		    s_wsfe(&io___84);
/* Writing concatenation */
		    i__7[0] = 8, a__1[0] = "ZHEEV(V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__7, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__3, (ftnlen)10);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L950;
		    }
		}

/*              Do tests 37 and 38 */

		zhet21_(&c__1, uplo, &n, &c__0, &v[v_offset], ldu, &d1[1], &
			d2[1], &a[a_offset], ldu, &z__[z_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ntest += 2;
		zheev_("N", uplo, &n, &a[a_offset], ldu, &d3[1], &work[1], 
			lwork, &rwork[1], &iinfo);
		if (iinfo != 0) {
		    io___85.ciunit = *nounit;
		    s_wsfe(&io___85);
/* Writing concatenation */
		    i__7[0] = 8, a__1[0] = "ZHEEV(N,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__7, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__3, (ftnlen)10);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L950;
		    }
		}

/*              Do test 39 */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d1[j] - d3[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L940: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L950:

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

/*              Call ZHPEV */

/*              Load array WORK with the upper or lower triangular */
/*              part of the matrix in packed form. */

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = j;
			for (i__ = 1; i__ <= i__5; ++i__) {
			    i__6 = indx;
			    i__4 = i__ + j * a_dim1;
			    work[i__6].r = a[i__4].r, work[i__6].i = a[i__4]
				    .i;
			    ++indx;
/* L960: */
			}
/* L970: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = n;
			for (i__ = j; i__ <= i__5; ++i__) {
			    i__6 = indx;
			    i__4 = i__ + j * a_dim1;
			    work[i__6].r = a[i__4].r, work[i__6].i = a[i__4]
				    .i;
			    ++indx;
/* L980: */
			}
/* L990: */
		    }
		}

		++ntest;
		indwrk = n * (n + 1) / 2 + 1;
		zhpev_("V", uplo, &n, &work[1], &d1[1], &z__[z_offset], ldu, &
			work[indwrk], &rwork[1], &iinfo)
			;
		if (iinfo != 0) {
		    io___86.ciunit = *nounit;
		    s_wsfe(&io___86);
/* Writing concatenation */
		    i__7[0] = 8, a__1[0] = "ZHPEV(V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__7, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__3, (ftnlen)10);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L1050;
		    }
		}

/*              Do tests 40 and 41. */

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = j;
			for (i__ = 1; i__ <= i__5; ++i__) {
			    i__6 = indx;
			    i__4 = i__ + j * a_dim1;
			    work[i__6].r = a[i__4].r, work[i__6].i = a[i__4]
				    .i;
			    ++indx;
/* L1000: */
			}
/* L1010: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = n;
			for (i__ = j; i__ <= i__5; ++i__) {
			    i__6 = indx;
			    i__4 = i__ + j * a_dim1;
			    work[i__6].r = a[i__4].r, work[i__6].i = a[i__4]
				    .i;
			    ++indx;
/* L1020: */
			}
/* L1030: */
		    }
		}

		ntest += 2;
		indwrk = n * (n + 1) / 2 + 1;
		zhpev_("N", uplo, &n, &work[1], &d3[1], &z__[z_offset], ldu, &
			work[indwrk], &rwork[1], &iinfo)
			;
		if (iinfo != 0) {
		    io___87.ciunit = *nounit;
		    s_wsfe(&io___87);
/* Writing concatenation */
		    i__7[0] = 8, a__1[0] = "ZHPEV(N,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__7, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__3, (ftnlen)10);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L1050;
		    }
		}

/*              Do test 42 */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d1[j] - d3[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L1040: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L1050:

/*              Call ZHBEV */

		if (jtype <= 7) {
		    kd = 0;
		} else if (jtype >= 8 && jtype <= 15) {
/* Computing MAX */
		    i__3 = n - 1;
		    kd = max(i__3,0);
		} else {
		    kd = ihbw;
		}

/*              Load array V with the upper or lower triangular part */
/*              of the matrix in band form. */

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__5 = 1, i__6 = j - kd;
			i__4 = j;
			for (i__ = max(i__5,i__6); i__ <= i__4; ++i__) {
			    i__5 = kd + 1 + i__ - j + j * v_dim1;
			    i__6 = i__ + j * a_dim1;
			    v[i__5].r = a[i__6].r, v[i__5].i = a[i__6].i;
/* L1060: */
			}
/* L1070: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__5 = n, i__6 = j + kd;
			i__4 = min(i__5,i__6);
			for (i__ = j; i__ <= i__4; ++i__) {
			    i__5 = i__ + 1 - j + j * v_dim1;
			    i__6 = i__ + j * a_dim1;
			    v[i__5].r = a[i__6].r, v[i__5].i = a[i__6].i;
/* L1080: */
			}
/* L1090: */
		    }
		}

		++ntest;
		zhbev_("V", uplo, &n, &kd, &v[v_offset], ldu, &d1[1], &z__[
			z_offset], ldu, &work[1], &rwork[1], &iinfo);
		if (iinfo != 0) {
		    io___88.ciunit = *nounit;
		    s_wsfe(&io___88);
/* Writing concatenation */
		    i__7[0] = 8, a__1[0] = "ZHBEV(V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__7, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__3, (ftnlen)10);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L1140;
		    }
		}

/*              Do tests 43 and 44. */

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__4 = 1, i__5 = j - kd;
			i__6 = j;
			for (i__ = max(i__4,i__5); i__ <= i__6; ++i__) {
			    i__4 = kd + 1 + i__ - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L1100: */
			}
/* L1110: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__6 = min(i__4,i__5);
			for (i__ = j; i__ <= i__6; ++i__) {
			    i__4 = i__ + 1 - j + j * v_dim1;
			    i__5 = i__ + j * a_dim1;
			    v[i__4].r = a[i__5].r, v[i__4].i = a[i__5].i;
/* L1120: */
			}
/* L1130: */
		    }
		}

		ntest += 2;
		zhbev_("N", uplo, &n, &kd, &v[v_offset], ldu, &d3[1], &z__[
			z_offset], ldu, &work[1], &rwork[1], &iinfo);
		if (iinfo != 0) {
		    io___89.ciunit = *nounit;
		    s_wsfe(&io___89);
/* Writing concatenation */
		    i__7[0] = 8, a__1[0] = "ZHBEV(N,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__7, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__3, (ftnlen)10);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&kd, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L1140;
		    }
		}

L1140:

/*              Do test 45. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d1[j] - d3[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L1150: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

		zlacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);
		++ntest;
		i__3 = *liwork - (n << 1);
		zheevr_("V", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m, &wa1[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &rwork[1], lrwork, &iwork[(
			n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___90.ciunit = *nounit;
		    s_wsfe(&io___90);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVR(V,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L1170;
		    }
		}

/*              Do tests 45 and 46 (or ... ) */

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		zhet21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &rwork[1], &result[ntest]);

		ntest += 2;
		i__3 = *liwork - (n << 1);
		zheevr_("N", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &rwork[1], lrwork, &iwork[(
			n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___91.ciunit = *nounit;
		    s_wsfe(&io___91);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVR(N,A,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L1170;
		    }
		}

/*              Do test 47 (or ... ) */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = wa1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = wa2[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = wa1[j] - wa2[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L1160: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L1170:

		++ntest;
		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		i__3 = *liwork - (n << 1);
		zheevr_("V", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &rwork[1], lrwork, &iwork[(
			n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___92.ciunit = *nounit;
		    s_wsfe(&io___92);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVR(V,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L1180;
		    }
		}

/*              Do tests 48 and 49 (or +??) */

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;
		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		i__3 = *liwork - (n << 1);
		zheevr_("N", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &rwork[1], lrwork, &iwork[(
			n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___93.ciunit = *nounit;
		    s_wsfe(&io___93);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVR(N,I,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L1180;
		    }
		}

/*              Do test 50 (or +??) */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * temp3;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);
L1180:

		++ntest;
		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		i__3 = *liwork - (n << 1);
		zheevr_("V", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &rwork[1], lrwork, &iwork[(
			n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___94.ciunit = *nounit;
		    s_wsfe(&io___94);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVR(V,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			result[ntest + 1] = ulpinv;
			result[ntest + 2] = ulpinv;
			goto L1190;
		    }
		}

/*              Do tests 51 and 52 (or +??) */

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		zhet22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &rwork[1], &result[ntest]);

		ntest += 2;
		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		i__3 = *liwork - (n << 1);
		zheevr_("N", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &rwork[1], lrwork, &iwork[(
			n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___95.ciunit = *nounit;
		    s_wsfe(&io___95);
/* Writing concatenation */
		    i__7[0] = 11, a__1[0] = "ZHEEVR(N,V,";
		    i__7[1] = 1, a__1[1] = uplo;
		    i__7[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__7, &c__3, (ftnlen)13);
		    do_fio(&c__1, ch__2, (ftnlen)13);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    if (iinfo < 0) {
			return 0;
		    } else {
			result[ntest] = ulpinv;
			goto L1190;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L1190;
		}

/*              Do test 52 (or +??) */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);

		zlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);




/*              Load array V with the upper or lower triangular part */
/*              of the matrix in band form. */

L1190:

/* L1200: */
		;
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

	    ntestt += ntest;
	    dlafts_("ZST", &n, &n, &jtype, &ntest, &result[1], ioldsd, thresh, 
		     nounit, &nerrs);

L1210:
	    ;
	}
/* L1220: */
    }

/*     Summary */

    alasvm_("ZST", nounit, &nerrs, &ntestt, &c__0);


    return 0;

/*     End of ZDRVST */

} /* zdrvst_ */
