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

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__0 = 0;
static integer c__4 = 4;
static integer c__6 = 6;
static doublereal c_b38 = 1.;
static integer c__1 = 1;
static doublereal c_b48 = 0.;
static integer c__2 = 2;

/* Subroutine */ int zdrves_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, integer *nounit, 
	doublecomplex *a, integer *lda, doublecomplex *h__, doublecomplex *ht, 
	 doublecomplex *w, doublecomplex *wt, doublecomplex *vs, integer *
	ldvs, doublereal *result, doublecomplex *work, integer *nwork, 
	doublereal *rwork, integer *iwork, logical *bwork, integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,3,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,9,9,9 };
    static integer kmagn[21] = { 1,1,1,1,1,1,2,3,1,1,1,1,1,1,1,1,2,3,1,2,3 };
    static integer kmode[21] = { 0,0,0,4,3,1,4,4,4,3,1,5,4,3,1,5,5,5,4,3,1 };
    static integer kconds[21] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,0,0,0 };

    /* Format strings */
    static char fmt_9992[] = "(\002 ZDRVES: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(/1x,a3,\002 -- Complex Schur Form Decompositi"
	    "on Driver\002,/\002 Matrix types (see ZDRVES for details): \002)";
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
	    "vals. \002,\002 16=Ill-cond., random comp\002,\002lex \002,a6,"
	    "/\002 12=Well-cond., random complex \002,a6,\002   \002,\002 17="
	    "Ill-cond., large rand. complx \002,a4,/\002 13=Ill-condi\002,"
	    "\002tioned, evenly spaced.     \002,\002 18=Ill-cond., small ran"
	    "d.\002,\002 complx \002,a4)";
    static char fmt_9996[] = "(\002 19=Matrix with random O(1) entries.   "
	    " \002,\002 21=Matrix \002,\002with small random entries.\002,"
	    "/\002 20=Matrix with large ran\002,\002dom entries.   \002,/)";
    static char fmt_9995[] = "(\002 Tests performed with test threshold ="
	    "\002,f8.2,/\002 ( A denotes A on input and T denotes A on output)"
	    "\002,//\002 1 = 0 if T in Schur form (no sort), \002,\002  1/ulp"
	    " otherwise\002,/\002 2 = | A - VS T transpose(VS) | / ( n |A| ul"
	    "p ) (no sort)\002,/\002 3 = | I - VS transpose(VS) | / ( n ulp )"
	    " (no sort) \002,/\002 4 = 0 if W are eigenvalues of T (no sort)"
	    ",\002,\002  1/ulp otherwise\002,/\002 5 = 0 if T same no matter "
	    "if VS computed (no sort),\002,\002  1/ulp otherwise\002,/\002 6 "
	    "= 0 if W same no matter if VS computed (no sort)\002,\002,  1/ul"
	    "p otherwise\002)";
    static char fmt_9994[] = "(\002 7 = 0 if T in Schur form (sort), \002"
	    ",\002  1/ulp otherwise\002,/\002 8 = | A - VS T transpose(VS) | "
	    "/ ( n |A| ulp ) (sort)\002,/\002 9 = | I - VS transpose(VS) | / "
	    "( n ulp ) (sort) \002,/\002 10 = 0 if W are eigenvalues of T (so"
	    "rt),\002,\002  1/ulp otherwise\002,/\002 11 = 0 if T same no mat"
	    "ter if VS computed (sort),\002,\002  1/ulp otherwise\002,/\002 1"
	    "2 = 0 if W same no matter if VS computed (sort),\002,\002  1/ulp"
	    " otherwise\002,/\002 13 = 0 if sorting succesful, 1/ulp otherwise"
	    "\002,/)";
    static char fmt_9993[] = "(\002 N=\002,i5,\002, IWK=\002,i2,\002, seed"
	    "=\002,4(i4,\002,\002),\002 type \002,i2,\002, test(\002,i2,\002)="
	    "\002,g10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, ht_dim1, ht_offset, vs_dim1, 
	    vs_offset, i__1, i__2, i__3, i__4, i__5, i__6;
    doublecomplex z__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, n;
    doublereal res[2];
    integer iwk;
    doublereal ulp, cond;
    integer jcol;
    char path[3];
    integer sdim, nmax;
    doublereal unfl, ovfl;
    integer rsub;
    char sort[1];
    logical badnn;
    integer nfail, imode, iinfo;
    doublereal conds, anorm;
    extern /* Subroutine */ int zgees_(char *, char *, L_fp, integer *, 
	    doublecomplex *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, logical *, integer *);
    integer jsize, nerrs, itype, jtype, ntest, lwork, isort;
    extern /* Subroutine */ int zhst01_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, doublereal *);
    doublereal rtulp;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    integer idumma[1], ioldsd[4];
    extern /* Subroutine */ int xerbla_(char *, integer *);
    integer knteig;
    extern /* Subroutine */ int dlasum_(char *, integer *, integer *, integer 
	    *), zlatme_(integer *, char *, integer *, doublecomplex *, 
	     integer *, doublereal *, doublecomplex *, char *, char *, char *, 
	     char *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, doublereal *, doublecomplex *, integer *, 
	    doublecomplex *, integer *), zlacpy_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublecomplex *, integer *);
    integer ntestf;
    extern logical zslect_(doublecomplex *);
    extern /* Subroutine */ int zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *), zlatmr_(integer *, integer *, char *, integer *, char *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, char *, 
	     char *, doublecomplex *, integer *, doublereal *, doublecomplex *
, integer *, doublereal *, char *, integer *, integer *, integer *
, doublereal *, doublereal *, char *, doublecomplex *, integer *, 
	    integer *, integer *), zlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublecomplex *, integer *, doublecomplex *, 
	    integer *);
    integer nnwork;
    doublereal rtulpi;
    integer mtypes, ntestt;
    doublereal ulpinv;

    /* Fortran I/O blocks */
    static cilist io___31 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9993, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     ZDRVES checks the nonsymmetric eigenvalue (Schur form) problem */
/*     driver ZGEES. */

/*     When ZDRVES is called, a number of matrix "sizes" ("n's") and a */
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

/*     (4)     0     if W are eigenvalues of T */
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

/*     (10)    0     if W are eigenvalues of T */
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
/*          1, ..., ULP  and random complex angles. */
/*          (ULP = (first number larger than 1) - 1 ) */
/*     (5)  A diagonal matrix with geometrically spaced entries */
/*          1, ..., ULP  and random complex angles. */
/*     (6)  A diagonal matrix with "clustered" entries 1, ULP, ..., ULP */
/*          and random complex angles. */

/*     (7)  Same as (4), but multiplied by a constant near */
/*          the overflow threshold */
/*     (8)  Same as (4), but multiplied by a constant near */
/*          the underflow threshold */

/*     (9)  A matrix of the form  U' T U, where U is unitary and */
/*          T has evenly spaced entries 1, ..., ULP with random */
/*          complex angles on the diagonal and random O(1) entries in */
/*          the upper triangle. */

/*     (10) A matrix of the form  U' T U, where U is unitary and */
/*          T has geometrically spaced entries 1, ..., ULP with random */
/*          complex angles on the diagonal and random O(1) entries in */
/*          the upper triangle. */

/*     (11) A matrix of the form  U' T U, where U is orthogonal and */
/*          T has "clustered" entries 1, ULP,..., ULP with random */
/*          complex angles on the diagonal and random O(1) entries in */
/*          the upper triangle. */

/*     (12) A matrix of the form  U' T U, where U is unitary and */
/*          T has complex eigenvalues randomly chosen from */
/*          ULP < |z| < 1   and random O(1) entries in the upper */
/*          triangle. */

/*     (13) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has evenly spaced entries 1, ..., ULP */
/*          with random complex angles on the diagonal and random O(1) */
/*          entries in the upper triangle. */

/*     (14) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has geometrically spaced entries */
/*          1, ..., ULP with random complex angles on the diagonal */
/*          and random O(1) entries in the upper triangle. */

/*     (15) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has "clustered" entries 1, ULP,..., ULP */
/*          with random complex angles on the diagonal and random O(1) */
/*          entries in the upper triangle. */

/*     (16) A matrix of the form  X' T X, where X has condition */
/*          SQRT( ULP ) and T has complex eigenvalues randomly chosen */
/*          from ULP < |z| < 1 and random O(1) entries in the upper */
/*          triangle. */

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
/*          ZDRVES does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, ZDRVES */
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
/*          next call to ZDRVES to continue the same random number */
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

/*  A       (workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least max( NN ). */

/*  H       (workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          Another copy of the test matrix A, modified by ZGEES. */

/*  HT      (workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          Yet another copy of the test matrix A, modified by ZGEES. */

/*  W       (workspace) COMPLEX*16 array, dimension (max(NN)) */
/*          The computed eigenvalues of A. */

/*  WT      (workspace) COMPLEX*16 array, dimension (max(NN)) */
/*          Like W, this array contains the eigenvalues of A, */
/*          but those computed when ZGEES only computes a partial */
/*          eigendecomposition, i.e. not Schur vectors */

/*  VS      (workspace) COMPLEX*16 array, dimension (LDVS, max(NN)) */
/*          VS holds the computed Schur vectors. */

/*  LDVS    (input) INTEGER */
/*          Leading dimension of VS. Must be at least max(1,max(NN)). */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (13) */
/*          The values computed by the 13 tests described above. */
/*          The values are currently limited to 1/ulp, to avoid overflow. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (NWORK) */

/*  NWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          5*NN(j)+2*NN(j)**2 for all j. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (max(NN)) */

/*  IWORK   (workspace) INTEGER array, dimension (max(NN)) */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some NN(j) < 0 */
/*           -3: NTYPES < 0 */
/*           -6: THRESH < 0 */
/*           -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*          -15: LDVS < 1 or LDVS < NMAX, where NMAX is max( NN(j) ). */
/*          -18: NWORK too small. */
/*          If  ZLATMR, CLATMS, CLATME or ZGEES returns an error code, */
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
/*     KCONDS(j)       Select whether CONDS is to be 1 or */
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
    --w;
    --wt;
    vs_dim1 = *ldvs;
    vs_offset = 1 + vs_dim1;
    vs -= vs_offset;
    --result;
    --work;
    --rwork;
    --iwork;
    --bwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Zomplex precision", (ftnlen)1, (ftnlen)17);
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
	*info = -15;
    } else /* if(complicated condition) */ {
/* Computing 2nd power */
	i__1 = nmax;
	if (nmax * 5 + (i__1 * i__1 << 1) > *nwork) {
	    *info = -18;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("ZDRVES", &i__1);
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
	if (*nsizes != 1) {
	    mtypes = min(21,*ntypes);
	} else {
	    mtypes = min(22,*ntypes);
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L230;
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

	    zlaset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);
	    iinfo = 0;
	    cond = ulpinv;

/*           Special Matrices -- Identity & Jordan block */

	    if (itype == 1) {

/*              Zero */

		iinfo = 0;

	    } else if (itype == 2) {

/*              Identity */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    z__1.r = anorm, z__1.i = 0.;
		    a[i__4].r = z__1.r, a[i__4].i = z__1.i;
/* L70: */
		}

	    } else if (itype == 3) {

/*              Jordan Block */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    z__1.r = anorm, z__1.i = 0.;
		    a[i__4].r = z__1.r, a[i__4].i = z__1.i;
		    if (jcol > 1) {
			i__4 = jcol + (jcol - 1) * a_dim1;
			a[i__4].r = 1., a[i__4].i = 0.;
		    }
/* L80: */
		}

	    } else if (itype == 4) {

/*              Diagonal Matrix, [Eigen]values Specified */

		zlatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &c__0, &c__0, "N", &a[a_offset], lda, &work[
			n + 1], &iinfo);

	    } else if (itype == 5) {

/*              Symmetric, eigenvalues specified */

		zlatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
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

		zlatme_(&n, "D", &iseed[1], &work[1], &imode, &cond, &c_b2, 
			" ", "T", "T", "T", &rwork[1], &c__4, &conds, &n, &n, 
			&anorm, &a[a_offset], lda, &work[(n << 1) + 1], &
			iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		zlatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &c__0, &
			c__0, &c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		zlatmr_(&n, &n, "D", &iseed[1], "H", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &n, &n, &
			c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              General, random eigenvalues */

		zlatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &n, &n, &
			c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);
		if (n >= 4) {
		    zlaset_("Full", &c__2, &n, &c_b1, &c_b1, &a[a_offset], 
			    lda);
		    i__3 = n - 3;
		    zlaset_("Full", &i__3, &c__1, &c_b1, &c_b1, &a[a_dim1 + 3]
, lda);
		    i__3 = n - 3;
		    zlaset_("Full", &i__3, &c__2, &c_b1, &c_b1, &a[(n - 1) * 
			    a_dim1 + 3], lda);
		    zlaset_("Full", &c__1, &n, &c_b1, &c_b1, &a[n + a_dim1], 
			    lda);
		}

	    } else if (itype == 10) {

/*              Triangular, random eigenvalues */

		zlatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &n, &c__0, &
			c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else {

		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___31.ciunit = *nounit;
		s_wsfe(&io___31);
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

		    zlacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], 
			    lda);
		    zgees_("V", sort, (L_fp)zslect_, &n, &h__[h_offset], lda, 
			    &sdim, &w[1], &vs[vs_offset], ldvs, &work[1], &
			    nnwork, &rwork[1], &bwork[1], &iinfo);
		    if (iinfo != 0) {
			result[rsub + 1] = ulpinv;
			io___38.ciunit = *nounit;
			s_wsfe(&io___38);
			do_fio(&c__1, "ZGEES1", (ftnlen)6);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			goto L190;
		    }

/*                 Do Test (1) or Test (7) */

		    result[rsub + 1] = 0.;
		    i__3 = n - 1;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j + 1; i__ <= i__4; ++i__) {
			    i__5 = i__ + j * h_dim1;
			    if (h__[i__5].r != 0. || h__[i__5].i != 0.) {
				result[rsub + 1] = ulpinv;
			    }
/* L110: */
			}
/* L120: */
		    }

/*                 Do Tests (2) and (3) or Tests (8) and (9) */

/* Computing MAX */
		    i__3 = 1, i__4 = (n << 1) * n;
		    lwork = max(i__3,i__4);
		    zhst01_(&n, &c__1, &n, &a[a_offset], lda, &h__[h_offset], 
			    lda, &vs[vs_offset], ldvs, &work[1], &lwork, &
			    rwork[1], res);
		    result[rsub + 2] = res[0];
		    result[rsub + 3] = res[1];

/*                 Do Test (4) or Test (10) */

		    result[rsub + 4] = 0.;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			i__4 = i__ + i__ * h_dim1;
			i__5 = i__;
			if (h__[i__4].r != w[i__5].r || h__[i__4].i != w[i__5]
				.i) {
			    result[rsub + 4] = ulpinv;
			}
/* L130: */
		    }

/*                 Do Test (5) or Test (11) */

		    zlacpy_("F", &n, &n, &a[a_offset], lda, &ht[ht_offset], 
			    lda);
		    zgees_("N", sort, (L_fp)zslect_, &n, &ht[ht_offset], lda, 
			    &sdim, &wt[1], &vs[vs_offset], ldvs, &work[1], &
			    nnwork, &rwork[1], &bwork[1], &iinfo);
		    if (iinfo != 0) {
			result[rsub + 5] = ulpinv;
			io___42.ciunit = *nounit;
			s_wsfe(&io___42);
			do_fio(&c__1, "ZGEES2", (ftnlen)6);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
			*info = abs(iinfo);
			goto L190;
		    }

		    result[rsub + 5] = 0.;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    i__5 = i__ + j * h_dim1;
			    i__6 = i__ + j * ht_dim1;
			    if (h__[i__5].r != ht[i__6].r || h__[i__5].i != 
				    ht[i__6].i) {
				result[rsub + 5] = ulpinv;
			    }
/* L140: */
			}
/* L150: */
		    }

/*                 Do Test (6) or Test (12) */

		    result[rsub + 6] = 0.;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			i__4 = i__;
			i__5 = i__;
			if (w[i__4].r != wt[i__5].r || w[i__4].i != wt[i__5]
				.i) {
			    result[rsub + 6] = ulpinv;
			}
/* L160: */
		    }

/*                 Do Test (13) */

		    if (isort == 1) {
			result[13] = 0.;
			knteig = 0;
			i__3 = n;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    if (zslect_(&w[i__])) {
				++knteig;
			    }
			    if (i__ < n) {
				if (zslect_(&w[i__ + 1]) && ! zslect_(&w[i__])
					) {
				    result[13] = ulpinv;
				}
			    }
/* L170: */
			}
			if (sdim != knteig) {
			    result[13] = ulpinv;
			}
		    }

/* L180: */
		}

/*              End of Loop -- Check for RESULT(j) > THRESH */

L190:

		ntest = 0;
		nfail = 0;
		for (j = 1; j <= 13; ++j) {
		    if (result[j] >= 0.) {
			++ntest;
		    }
		    if (result[j] >= *thresh) {
			++nfail;
		    }
/* L200: */
		}

		if (nfail > 0) {
		    ++ntestf;
		}
		if (ntestf == 1) {
		    io___46.ciunit = *nounit;
		    s_wsfe(&io___46);
		    do_fio(&c__1, path, (ftnlen)3);
		    e_wsfe();
		    io___47.ciunit = *nounit;
		    s_wsfe(&io___47);
		    e_wsfe();
		    io___48.ciunit = *nounit;
		    s_wsfe(&io___48);
		    e_wsfe();
		    io___49.ciunit = *nounit;
		    s_wsfe(&io___49);
		    e_wsfe();
		    io___50.ciunit = *nounit;
		    s_wsfe(&io___50);
		    do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(
			    doublereal));
		    e_wsfe();
		    io___51.ciunit = *nounit;
		    s_wsfe(&io___51);
		    e_wsfe();
		    ntestf = 2;
		}

		for (j = 1; j <= 13; ++j) {
		    if (result[j] >= *thresh) {
			io___52.ciunit = *nounit;
			s_wsfe(&io___52);
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
/* L210: */
		}

		nerrs += nfail;
		ntestt += ntest;

/* L220: */
	    }
L230:
	    ;
	}
/* L240: */
    }

/*     Summary */

    dlasum_(path, nounit, &nerrs, &ntestt);



    return 0;

/*     End of ZDRVES */

} /* zdrves_ */
