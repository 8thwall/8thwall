#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer selopt, seldim;
    logical selval[20];
    doublereal selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

/* Table of constant values */

static doublereal c_b17 = 0.;
static integer c__0 = 0;
static doublereal c_b31 = 1.;
static integer c__4 = 4;
static integer c__6 = 6;
static integer c__1 = 1;
static integer c__2 = 2;

/* Subroutine */ int ddrves_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, integer *nounit, 
	doublereal *a, integer *lda, doublereal *h__, doublereal *ht, 
	doublereal *wr, doublereal *wi, doublereal *wrt, doublereal *wit, 
	doublereal *vs, integer *ldvs, doublereal *result, doublereal *work, 
	integer *nwork, integer *iwork, logical *bwork, integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,3,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,9,9,9 };
    static integer kmagn[21] = { 1,1,1,1,1,1,2,3,1,1,1,1,1,1,1,1,2,3,1,2,3 };
    static integer kmode[21] = { 0,0,0,4,3,1,4,4,4,3,1,5,4,3,1,5,5,5,4,3,1 };
    static integer kconds[21] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,0,0,0 };

    /* Format strings */
    static char fmt_9992[] = "(\002 DDRVES: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(/1x,a3,\002 -- Real Schur Form Decomposition "
	    "Driver\002,/\002 Matrix types (see DDRVES for details): \002)";
    static char fmt_9998[] = "(/\002 Special Matrices:\002,/\002  1=Zero mat"
	    "rix.             \002,\002           \002,\002  5=Diagonal: geom"
	    "etr. spaced entries.\002,/\002  2=Identity matrix.              "
	    "      \002,\002  6=Diagona\002,\002l: clustered entries.\002,"
	    "/\002  3=Transposed Jordan block.  \002,\002          \002,\002 "
	    " 7=Diagonal: large, evenly spaced.\002,/\002  \002,\0024=Diagona"
	    "l: evenly spaced entries.    \002,\002  8=Diagonal: s\002,\002ma"
	    "ll, evenly spaced.\002)";
    static char fmt_9997[] = "(\002 Dense, Non-Symmetric Matrices:\002,/\002"
	    "  9=Well-cond., ev\002,\002enly spaced eigenvals.\002,\002 14=Il"
	    "l-cond., geomet. spaced e\002,\002igenals.\002,/\002 10=Well-con"
	    "d., geom. spaced eigenvals. \002,\002 15=Ill-conditioned, cluste"
	    "red e.vals.\002,/\002 11=Well-cond\002,\002itioned, clustered e."
	    "vals. \002,\002 16=Ill-cond., random comp\002,\002lex \002,/\002"
	    " 12=Well-cond., random complex \002,6x,\002   \002,\002 17=Ill-c"
	    "ond., large rand. complx \002,/\002 13=Ill-condi\002,\002tioned,"
	    " evenly spaced.     \002,\002 18=Ill-cond., small rand.\002,\002"
	    " complx \002)";
    static char fmt_9996[] = "(\002 19=Matrix with random O(1) entries.   "
	    " \002,\002 21=Matrix \002,\002with small random entries.\002,"
	    "/\002 20=Matrix with large ran\002,\002dom entries.   \002,/)";
    static char fmt_9995[] = "(\002 Tests performed with test threshold ="
	    "\002,f8.2,/\002 ( A denotes A on input and T denotes A on output)"
	    "\002,//\002 1 = 0 if T in Schur form (no sort), \002,\002  1/ulp"
	    " otherwise\002,/\002 2 = | A - VS T transpose(VS) | / ( n |A| ul"
	    "p ) (no sort)\002,/\002 3 = | I - VS transpose(VS) | / ( n ulp )"
	    " (no sort) \002,/\002 4 = 0 if WR+sqrt(-1)*WI are eigenvalues of"
	    " T (no sort),\002,\002  1/ulp otherwise\002,/\002 5 = 0 if T sam"
	    "e no matter if VS computed (no sort),\002,\002  1/ulp otherwis"
	    "e\002,/\002 6 = 0 if WR, WI same no matter if VS computed (no so"
	    "rt)\002,\002,  1/ulp otherwise\002)";
    static char fmt_9994[] = "(\002 7 = 0 if T in Schur form (sort), \002"
	    ",\002  1/ulp otherwise\002,/\002 8 = | A - VS T transpose(VS) | "
	    "/ ( n |A| ulp ) (sort)\002,/\002 9 = | I - VS transpose(VS) | / "
	    "( n ulp ) (sort) \002,/\002 10 = 0 if WR+sqrt(-1)*WI are eigenva"
	    "lues of T (sort),\002,\002  1/ulp otherwise\002,/\002 11 = 0 if "
	    "T same no matter if VS computed (sort),\002,\002  1/ulp otherwise"
	    "\002,/\002 12 = 0 if WR, WI same no matter if VS computed (sort),"
	    "\002,\002  1/ulp otherwise\002,/\002 13 = 0 if sorting succesful"
	    ", 1/ulp otherwise\002,/)";
    static char fmt_9993[] = "(\002 N=\002,i5,\002, IWK=\002,i2,\002, seed"
	    "=\002,4(i4,\002,\002),\002 type \002,i2,\002, test(\002,i2,\002)="
	    "\002,g10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, ht_dim1, ht_offset, vs_dim1, 
	    vs_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double d_sign(doublereal *, doublereal *);

    /* Local variables */
    integer i__, j, n;
    doublereal res[2];
    integer iwk;
    doublereal tmp, ulp, cond;
    integer jcol;
    char path[3];
    integer sdim, nmax;
    doublereal unfl, ovfl;
    integer rsub;
    char sort[1];
    logical badnn;
    extern /* Subroutine */ int dgees_(char *, char *, L_fp, integer *, 
	    doublereal *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, logical *, 
	    integer *);
    integer nfail, imode;
    extern /* Subroutine */ int dhst01_(integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *);
    integer iinfo;
    doublereal conds, anorm;
    integer jsize, nerrs, itype, jtype, ntest, lwork, isort;
    doublereal rtulp;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    char adumma[1*1];
    extern /* Subroutine */ int dlatme_(integer *, char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, char *, char 
	    *, char *, char *, doublereal *, integer *, doublereal *, integer 
	    *, integer *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *);
    integer idumma[1], ioldsd[4];
    extern logical dslect_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer knteig;
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    dlatmr_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, char *, char 
	    *, doublereal *, integer *, doublereal *, doublereal *, integer *, 
	     doublereal *, char *, integer *, integer *, integer *, 
	    doublereal *, doublereal *, char *, doublereal *, integer *, 
	    integer *, integer *), dlasum_(char *, integer *, integer *, integer *),
	     dlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublereal *, integer *, doublereal *, integer 
	    *), xerbla_(char *, integer *);
    integer ntestf;
    doublereal ulpinv;
    integer nnwork;
    doublereal rtulpi;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___32 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9993, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     DDRVES checks the nonsymmetric eigenvalue (Schur form) problem */
/*     driver DGEES. */

/*     When DDRVES is called, a number of matrix "sizes" ("n's") and a */
/*     number of matrix "types" are specified.  For each size ("n") */
/*     and each type of matrix, one matrix will be generated and used */
/*     to test the nonsymmetric eigenroutines.  For each matrix, 13 */
/*     tests will be performed: */

/*     (1)     0 if T is in Schur form, 1/ulp otherwise */
/*            (no sorting of eigenvalues) */

/*     (2)     | A - VS T VS' | / ( n |A| ulp ) */

/*       Here VS is the matrix of Schur eigenvectors, and T is in Schur */
/*       form  (no sorting of eigenvalues). */

/*     (3)     | I - VS VS' | / ( n ulp ) (no sorting of eigenvalues). */

/*     (4)     0     if WR+sqrt(-1)*WI are eigenvalues of T */
/*             1/ulp otherwise */
/*             (no sorting of eigenvalues) */

/*     (5)     0     if T(with VS) = T(without VS), */
/*             1/ulp otherwise */
/*             (no sorting of eigenvalues) */

/*     (6)     0     if eigenvalues(with VS) = eigenvalues(without VS), */
/*             1/ulp otherwise */
/*             (no sorting of eigenvalues) */

/*     (7)     0 if T is in Schur form, 1/ulp otherwise */
/*             (with sorting of eigenvalues) */

/*     (8)     | A - VS T VS' | / ( n |A| ulp ) */

/*       Here VS is the matrix of Schur eigenvectors, and T is in Schur */
/*       form  (with sorting of eigenvalues). */

/*     (9)     | I - VS VS' | / ( n ulp ) (with sorting of eigenvalues). */

/*     (10)    0     if WR+sqrt(-1)*WI are eigenvalues of T */
/*             1/ulp otherwise */
/*             (with sorting of eigenvalues) */

/*     (11)    0     if T(with VS) = T(without VS), */
/*             1/ulp otherwise */
/*             (with sorting of eigenvalues) */

/*     (12)    0     if eigenvalues(with VS) = eigenvalues(without VS), */
/*             1/ulp otherwise */
/*             (with sorting of eigenvalues) */

/*     (13)    if sorting worked and SDIM is the number of */
/*             eigenvalues which were SELECTed */

/*     The "sizes" are specified by an array NN(1:NSIZES); the value of */
/*     each element NN(j) specifies one size. */
/*     The "types" are specified by a logical array DOTYPE( 1:NTYPES ); */
/*     if DOTYPE(j) is .TRUE., then matrix type "j" will be generated. */
/*     Currently, the list of possible types is: */

/*     (1)  The zero matrix. */
/*     (2)  The identity matrix. */
/*     (3)  A (transposed) Jordan block, with 1's on the diagonal. */

/*     (4)  A diagonal matrix with evenly spaced entries */
/*          1, ..., ULP  and random signs. */
/*          (ULP = (first number larger than 1) - 1 ) */
/*     (5)  A diagonal matrix with geometrically spaced entries */
/*          1, ..., ULP  and random signs. */
/*     (6)  A diagonal matrix with "clustered" entries 1, ULP, ..., ULP */
/*          and random signs. */

/*     (7)  Same as (4), but multiplied by a constant near */
/*          the overflow threshold */
/*     (8)  Same as (4), but multiplied by a constant near */
/*          the underflow threshold */

/*     (9)  A matrix of the form  U' T U, where U is orthogonal and */
/*          T has evenly spaced entries 1, ..., ULP with random signs */
/*          on the diagonal and random O(1) entries in the upper */
/*          triangle. */

/*     (10) A matrix of the form  U' T U, where U is orthogonal and */
/*          T has geometrically spaced entries 1, ..., ULP with random */
/*          signs on the diagonal and random O(1) entries in the upper */
/*          triangle. */

/*     (11) A matrix of the form  U' T U, where U is orthogonal and */
/*          T has "clustered" entries 1, ULP,..., ULP with random */
/*          signs on the diagonal and random O(1) entries in the upper */
/*          triangle. */

/*     (12) A matrix of the form  U' T U, where U is orthogonal and */
/*          T has real or complex conjugate paired eigenvalues randomly */
/*          chosen from ( ULP, 1 ) and random O(1) entries in the upper */
/*          triangle. */

/*     (13) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has evenly spaced entries 1, ..., ULP */
/*          with random signs on the diagonal and random O(1) entries */
/*          in the upper triangle. */

/*     (14) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has geometrically spaced entries */
/*          1, ..., ULP with random signs on the diagonal and random */
/*          O(1) entries in the upper triangle. */

/*     (15) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has "clustered" entries 1, ULP,..., ULP */
/*          with random signs on the diagonal and random O(1) entries */
/*          in the upper triangle. */

/*     (16) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has real or complex conjugate paired */
/*          eigenvalues randomly chosen from ( ULP, 1 ) and random */
/*          O(1) entries in the upper triangle. */

/*     (17) Same as (16), but multiplied by a constant */
/*          near the overflow threshold */
/*     (18) Same as (16), but multiplied by a constant */
/*          near the underflow threshold */

/*     (19) Nonsymmetric matrix with random entries chosen from (-1,1). */
/*          If N is at least 4, all entries in first two rows and last */
/*          row, and first column and last two columns are zero. */
/*     (20) Same as (19), but multiplied by a constant */
/*          near the overflow threshold */
/*     (21) Same as (19), but multiplied by a constant */
/*          near the underflow threshold */

/*  Arguments */
/*  ========= */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          DDRVES does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, DDRVES */
/*          does nothing.  It must be at least zero.  If it is MAXTYP+1 */
/*          and NSIZES is 1, then an additional type, MAXTYP+1 is */
/*          defined, which is to use whatever matrix is in A.  This */
/*          is only useful if DOTYPE(1:MAXTYP) is .FALSE. and */
/*          DOTYPE(MAXTYP+1) is .TRUE. . */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          If DOTYPE(j) is .TRUE., then for each size in NN a */
/*          matrix of that size and of type j will be generated. */
/*          If NTYPES is smaller than the maximum number of types */
/*          defined (PARAMETER MAXTYP), then types NTYPES+1 through */
/*          MAXTYP will not be generated.  If NTYPES is larger */
/*          than MAXTYP, DOTYPE(MAXTYP+1) through DOTYPE(NTYPES) */
/*          will be ignored. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry ISEED specifies the seed of the random number */
/*          generator. The array elements should be between 0 and 4095; */
/*          if not they will be reduced mod 4096.  Also, ISEED(4) must */
/*          be odd.  The random number generator uses a linear */
/*          congruential sequence limited to small integers, and so */
/*          should produce machine independent random numbers. The */
/*          values of ISEED are changed on exit, and can be used in the */
/*          next call to DDRVES to continue the same random number */
/*          sequence. */

/*  THRESH  (input) DOUBLE PRECISION */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error */
/*          is scaled to be O(1), so THRESH should be a reasonably */
/*          small multiple of 1, e.g., 10 or 100.  In particular, */
/*          it should not depend on the precision (single vs. double) */
/*          or the size of the matrix.  It must be at least zero. */

/*  NOUNIT  (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns INFO not equal to 0.) */

/*  A       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least max(NN). */

/*  H       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Another copy of the test matrix A, modified by DGEES. */

/*  HT      (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Yet another copy of the test matrix A, modified by DGEES. */

/*  WR      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  WI      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          The real and imaginary parts of the eigenvalues of A. */
/*          On exit, WR + WI*i are the eigenvalues of the matrix in A. */

/*  WRT     (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  WIT     (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but those computed when DGEES only computes a partial */
/*          eigendecomposition, i.e. not Schur vectors */

/*  VS      (workspace) DOUBLE PRECISION array, dimension (LDVS, max(NN)) */
/*          VS holds the computed Schur vectors. */

/*  LDVS    (input) INTEGER */
/*          Leading dimension of VS. Must be at least max(1,max(NN)). */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (13) */
/*          The values computed by the 13 tests described above. */
/*          The values are currently limited to 1/ulp, to avoid overflow. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (NWORK) */

/*  NWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          5*NN(j)+2*NN(j)**2 for all j. */

/*  IWORK   (workspace) INTEGER array, dimension (max(NN)) */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some NN(j) < 0 */
/*           -3: NTYPES < 0 */
/*           -6: THRESH < 0 */
/*           -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*          -17: LDVS < 1 or LDVS < NMAX, where NMAX is max( NN(j) ). */
/*          -20: NWORK too small. */
/*          If  DLATMR, SLATMS, SLATME or DGEES returns an error code, */
/*              the absolute value of it is returned. */

/* ----------------------------------------------------------------------- */

/*     Some Local Variables and Parameters: */
/*     ---- ----- --------- --- ---------- */

/*     ZERO, ONE       Real 0 and 1. */
/*     MAXTYP          The number of types defined. */
/*     NMAX            Largest value in NN. */
/*     NERRS           The number of tests which have exceeded THRESH */
/*     COND, CONDS, */
/*     IMODE           Values to be passed to the matrix generators. */
/*     ANORM           Norm of A; passed to matrix generators. */

/*     OVFL, UNFL      Overflow and underflow thresholds. */
/*     ULP, ULPINV     Finest relative precision and its inverse. */
/*     RTULP, RTULPI   Square roots of the previous 4 values. */

/*             The following four arrays decode JTYPE: */
/*     KTYPE(j)        The general type (1-10) for type "j". */
/*     KMODE(j)        The MODE value to be passed to the matrix */
/*                     generator for type "j". */
/*     KMAGN(j)        The order of magnitude ( O(1), */
/*                     O(overflow^(1/2) ), O(underflow^(1/2) ) */
/*     KCONDS(j)       Selectw whether CONDS is to be 1 or */
/*                     1/sqrt(ulp).  (0 means irrelevant.) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Arrays in Common .. */
/*     .. */
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
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
    ht_dim1 = *lda;
    ht_offset = 1 + ht_dim1;
    ht -= ht_offset;
    h_dim1 = *lda;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --wr;
    --wi;
    --wrt;
    --wit;
    vs_dim1 = *ldvs;
    vs_offset = 1 + vs_dim1;
    vs -= vs_offset;
    --result;
    --work;
    --iwork;
    --bwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "ES", (ftnlen)2, (ftnlen)2);

/*     Check for errors */

    ntestt = 0;
    ntestf = 0;
    *info = 0;
    sslct_1.selopt = 0;

/*     Important constants */

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
    } else if (*thresh < 0.) {
	*info = -6;
    } else if (*nounit <= 0) {
	*info = -7;
    } else if (*lda < 1 || *lda < nmax) {
	*info = -9;
    } else if (*ldvs < 1 || *ldvs < nmax) {
	*info = -17;
    } else /* if(complicated condition) */ {
/* Computing 2nd power */
	i__1 = nmax;
	if (nmax * 5 + (i__1 * i__1 << 1) > *nwork) {
	    *info = -20;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DDRVES", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

/*     More Important constants */

    unfl = dlamch_("Safe minimum");
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    ulp = dlamch_("Precision");
    ulpinv = 1. / ulp;
    rtulp = sqrt(ulp);
    rtulpi = 1. / rtulp;

/*     Loop over sizes, types */

    nerrs = 0;

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	n = nn[jsize];
	mtypes = 21;
	if (*nsizes == 1 && *ntypes == 22) {
	    ++mtypes;
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L260;
	    }

/*           Save ISEED in case of an error. */

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

/*           Compute "A" */

/*           Control parameters: */

/*           KMAGN  KCONDS  KMODE        KTYPE */
/*       =1  O(1)   1       clustered 1  zero */
/*       =2  large  large   clustered 2  identity */
/*       =3  small          exponential  Jordan */
/*       =4                 arithmetic   diagonal, (w/ eigenvalues) */
/*       =5                 random log   symmetric, w/ eigenvalues */
/*       =6                 random       general, w/ eigenvalues */
/*       =7                              random diagonal */
/*       =8                              random symmetric */
/*       =9                              random general */
/*       =10                             random triangular */

	    if (mtypes > 21) {
		goto L90;
	    }

	    itype = ktype[jtype - 1];
	    imode = kmode[jtype - 1];

/*           Compute norm */

	    switch (kmagn[jtype - 1]) {
		case 1:  goto L30;
		case 2:  goto L40;
		case 3:  goto L50;
	    }

L30:
	    anorm = 1.;
	    goto L60;

L40:
	    anorm = ovfl * ulp;
	    goto L60;

L50:
	    anorm = unfl * ulpinv;
	    goto L60;

L60:

	    dlaset_("Full", lda, &n, &c_b17, &c_b17, &a[a_offset], lda);
	    iinfo = 0;
	    cond = ulpinv;

/*           Special Matrices -- Identity & Jordan block */

/*              Zero */

	    if (itype == 1) {
		iinfo = 0;

	    } else if (itype == 2) {

/*              Identity */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    a[jcol + jcol * a_dim1] = anorm;
/* L70: */
		}

	    } else if (itype == 3) {

/*              Jordan Block */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    a[jcol + jcol * a_dim1] = anorm;
		    if (jcol > 1) {
			a[jcol + (jcol - 1) * a_dim1] = 1.;
		    }
/* L80: */
		}

	    } else if (itype == 4) {

/*              Diagonal Matrix, [Eigen]values Specified */

		dlatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &cond, 
			&anorm, &c__0, &c__0, "N", &a[a_offset], lda, &work[n 
			+ 1], &iinfo);

	    } else if (itype == 5) {

/*              Symmetric, eigenvalues specified */

		dlatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &cond, 
			&anorm, &n, &n, "N", &a[a_offset], lda, &work[n + 1], 
			&iinfo);

	    } else if (itype == 6) {

/*              General, eigenvalues specified */

		if (kconds[jtype - 1] == 1) {
		    conds = 1.;
		} else if (kconds[jtype - 1] == 2) {
		    conds = rtulpi;
		} else {
		    conds = 0.;
		}

		*(unsigned char *)&adumma[0] = ' ';
		dlatme_(&n, "S", &iseed[1], &work[1], &imode, &cond, &c_b31, 
			adumma, "T", "T", "T", &work[n + 1], &c__4, &conds, &
			n, &n, &anorm, &a[a_offset], lda, &work[(n << 1) + 1], 
			 &iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &c__0, &
			c__0, &c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &n, &n, &
			c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              General, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "N", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &n, &n, &
			c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);
		if (n >= 4) {
		    dlaset_("Full", &c__2, &n, &c_b17, &c_b17, &a[a_offset], 
			    lda);
		    i__3 = n - 3;
		    dlaset_("Full", &i__3, &c__1, &c_b17, &c_b17, &a[a_dim1 + 
			    3], lda);
		    i__3 = n - 3;
		    dlaset_("Full", &i__3, &c__2, &c_b17, &c_b17, &a[(n - 1) *
			     a_dim1 + 3], lda);
		    dlaset_("Full", &c__1, &n, &c_b17, &c_b17, &a[n + a_dim1], 
			     lda);
		}

	    } else if (itype == 10) {

/*              Triangular, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "N", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &n, &c__0, &
			c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else {

		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___32.ciunit = *nounit;
		s_wsfe(&io___32);
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

/*           Test for minimal and generous workspace */

	    for (iwk = 1; iwk <= 2; ++iwk) {
		if (iwk == 1) {
		    nnwork = n * 3;
		} else {
/* Computing 2nd power */
		    i__3 = n;
		    nnwork = n * 5 + (i__3 * i__3 << 1);
		}
		nnwork = max(nnwork,1);

/*              Initialize RESULT */

		for (j = 1; j <= 13; ++j) {
		    result[j] = -1.;
/* L100: */
		}

/*              Test with and without sorting of eigenvalues */

		for (isort = 0; isort <= 1; ++isort) {
		    if (isort == 0) {
			*(unsigned char *)sort = 'N';
			rsub = 0;
		    } else {
			*(unsigned char *)sort = 'S';
			rsub = 6;
		    }

/*                 Compute Schur form and Schur vectors, and test them */

		    dlacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], 
			    lda);
		    dgees_("V", sort, (L_fp)dslect_, &n, &h__[h_offset], lda, 
			    &sdim, &wr[1], &wi[1], &vs[vs_offset], ldvs, &
			    work[1], &nnwork, &bwork[1], &iinfo);
		    if (iinfo != 0 && iinfo != n + 2) {
			result[rsub + 1] = ulpinv;
			io___39.ciunit = *nounit;
			s_wsfe(&io___39);
			do_fio(&c__1, "DGEES1", (ftnlen)6);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			goto L220;
		    }

/*                 Do Test (1) or Test (7) */

		    result[rsub + 1] = 0.;
		    i__3 = n - 2;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j + 2; i__ <= i__4; ++i__) {
			    if (h__[i__ + j * h_dim1] != 0.) {
				result[rsub + 1] = ulpinv;
			    }
/* L110: */
			}
/* L120: */
		    }
		    i__3 = n - 2;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			if (h__[i__ + 1 + i__ * h_dim1] != 0. && h__[i__ + 2 
				+ (i__ + 1) * h_dim1] != 0.) {
			    result[rsub + 1] = ulpinv;
			}
/* L130: */
		    }
		    i__3 = n - 1;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			if (h__[i__ + 1 + i__ * h_dim1] != 0.) {
			    if (h__[i__ + i__ * h_dim1] != h__[i__ + 1 + (i__ 
				    + 1) * h_dim1] || h__[i__ + (i__ + 1) * 
				    h_dim1] == 0. || d_sign(&c_b31, &h__[i__ 
				    + 1 + i__ * h_dim1]) == d_sign(&c_b31, &
				    h__[i__ + (i__ + 1) * h_dim1])) {
				result[rsub + 1] = ulpinv;
			    }
			}
/* L140: */
		    }

/*                 Do Tests (2) and (3) or Tests (8) and (9) */

/* Computing MAX */
		    i__3 = 1, i__4 = (n << 1) * n;
		    lwork = max(i__3,i__4);
		    dhst01_(&n, &c__1, &n, &a[a_offset], lda, &h__[h_offset], 
			    lda, &vs[vs_offset], ldvs, &work[1], &lwork, res);
		    result[rsub + 2] = res[0];
		    result[rsub + 3] = res[1];

/*                 Do Test (4) or Test (10) */

		    result[rsub + 4] = 0.;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			if (h__[i__ + i__ * h_dim1] != wr[i__]) {
			    result[rsub + 4] = ulpinv;
			}
/* L150: */
		    }
		    if (n > 1) {
			if (h__[h_dim1 + 2] == 0. && wi[1] != 0.) {
			    result[rsub + 4] = ulpinv;
			}
			if (h__[n + (n - 1) * h_dim1] == 0. && wi[n] != 0.) {
			    result[rsub + 4] = ulpinv;
			}
		    }
		    i__3 = n - 1;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			if (h__[i__ + 1 + i__ * h_dim1] != 0.) {
			    tmp = sqrt((d__1 = h__[i__ + 1 + i__ * h_dim1], 
				    abs(d__1))) * sqrt((d__2 = h__[i__ + (i__ 
				    + 1) * h_dim1], abs(d__2)));
/* Computing MAX */
/* Computing MAX */
			    d__4 = ulp * tmp;
			    d__2 = result[rsub + 4], d__3 = (d__1 = wi[i__] - 
				    tmp, abs(d__1)) / max(d__4,unfl);
			    result[rsub + 4] = max(d__2,d__3);
/* Computing MAX */
/* Computing MAX */
			    d__4 = ulp * tmp;
			    d__2 = result[rsub + 4], d__3 = (d__1 = wi[i__ + 
				    1] + tmp, abs(d__1)) / max(d__4,unfl);
			    result[rsub + 4] = max(d__2,d__3);
			} else if (i__ > 1) {
			    if (h__[i__ + 1 + i__ * h_dim1] == 0. && h__[i__ 
				    + (i__ - 1) * h_dim1] == 0. && wi[i__] != 
				    0.) {
				result[rsub + 4] = ulpinv;
			    }
			}
/* L160: */
		    }

/*                 Do Test (5) or Test (11) */

		    dlacpy_("F", &n, &n, &a[a_offset], lda, &ht[ht_offset], 
			    lda);
		    dgees_("N", sort, (L_fp)dslect_, &n, &ht[ht_offset], lda, 
			    &sdim, &wrt[1], &wit[1], &vs[vs_offset], ldvs, &
			    work[1], &nnwork, &bwork[1], &iinfo);
		    if (iinfo != 0 && iinfo != n + 2) {
			result[rsub + 5] = ulpinv;
			io___44.ciunit = *nounit;
			s_wsfe(&io___44);
			do_fio(&c__1, "DGEES2", (ftnlen)6);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			goto L220;
		    }

		    result[rsub + 5] = 0.;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]
				    ) {
				result[rsub + 5] = ulpinv;
			    }
/* L170: */
			}
/* L180: */
		    }

/*                 Do Test (6) or Test (12) */

		    result[rsub + 6] = 0.;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
			    result[rsub + 6] = ulpinv;
			}
/* L190: */
		    }

/*                 Do Test (13) */

		    if (isort == 1) {
			result[13] = 0.;
			knteig = 0;
			i__3 = n;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    d__1 = -wi[i__];
			    if (dslect_(&wr[i__], &wi[i__]) || dslect_(&wr[
				    i__], &d__1)) {
				++knteig;
			    }
			    if (i__ < n) {
				d__1 = -wi[i__ + 1];
				d__2 = -wi[i__];
				if ((dslect_(&wr[i__ + 1], &wi[i__ + 1]) || 
					dslect_(&wr[i__ + 1], &d__1)) && ! (
					dslect_(&wr[i__], &wi[i__]) || 
					dslect_(&wr[i__], &d__2)) && iinfo != 
					n + 2) {
				    result[13] = ulpinv;
				}
			    }
/* L200: */
			}
			if (sdim != knteig) {
			    result[13] = ulpinv;
			}
		    }

/* L210: */
		}

/*              End of Loop -- Check for RESULT(j) > THRESH */

L220:

		ntest = 0;
		nfail = 0;
		for (j = 1; j <= 13; ++j) {
		    if (result[j] >= 0.) {
			++ntest;
		    }
		    if (result[j] >= *thresh) {
			++nfail;
		    }
/* L230: */
		}

		if (nfail > 0) {
		    ++ntestf;
		}
		if (ntestf == 1) {
		    io___48.ciunit = *nounit;
		    s_wsfe(&io___48);
		    do_fio(&c__1, path, (ftnlen)3);
		    e_wsfe();
		    io___49.ciunit = *nounit;
		    s_wsfe(&io___49);
		    e_wsfe();
		    io___50.ciunit = *nounit;
		    s_wsfe(&io___50);
		    e_wsfe();
		    io___51.ciunit = *nounit;
		    s_wsfe(&io___51);
		    e_wsfe();
		    io___52.ciunit = *nounit;
		    s_wsfe(&io___52);
		    do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(
			    doublereal));
		    e_wsfe();
		    io___53.ciunit = *nounit;
		    s_wsfe(&io___53);
		    e_wsfe();
		    ntestf = 2;
		}

		for (j = 1; j <= 13; ++j) {
		    if (result[j] >= *thresh) {
			io___54.ciunit = *nounit;
			s_wsfe(&io___54);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&iwk, (ftnlen)sizeof(integer));
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[j], (ftnlen)sizeof(
				doublereal));
			e_wsfe();
		    }
/* L240: */
		}

		nerrs += nfail;
		ntestt += ntest;

/* L250: */
	    }
L260:
	    ;
	}
/* L270: */
    }

/*     Summary */

    dlasum_(path, nounit, &nerrs, &ntestt);



    return 0;

/*     End of DDRVES */

} /* ddrves_ */
