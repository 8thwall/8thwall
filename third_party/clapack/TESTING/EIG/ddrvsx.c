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

static doublereal c_b18 = 0.;
static integer c__0 = 0;
static doublereal c_b32 = 1.;
static integer c__4 = 4;
static integer c__6 = 6;
static integer c__1 = 1;
static integer c__2 = 2;
static logical c_false = FALSE_;
static integer c__3 = 3;
static integer c__5 = 5;
static logical c_true = TRUE_;
static integer c__22 = 22;

/* Subroutine */ int ddrvsx_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, integer *niunit, 
	integer *nounit, doublereal *a, integer *lda, doublereal *h__, 
	doublereal *ht, doublereal *wr, doublereal *wi, doublereal *wrt, 
	doublereal *wit, doublereal *wrtmp, doublereal *witmp, doublereal *vs, 
	 integer *ldvs, doublereal *vs1, doublereal *result, doublereal *work, 
	 integer *lwork, integer *iwork, logical *bwork, integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,3,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,9,9,9 };
    static integer kmagn[21] = { 1,1,1,1,1,1,2,3,1,1,1,1,1,1,1,1,2,3,1,2,3 };
    static integer kmode[21] = { 0,0,0,4,3,1,4,4,4,3,1,5,4,3,1,5,5,5,4,3,1 };
    static integer kconds[21] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,0,0,0 };

    /* Format strings */
    static char fmt_9991[] = "(\002 DDRVSX: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(/1x,a3,\002 -- Real Schur Form Decomposition "
	    "Expert \002,\002Driver\002,/\002 Matrix types (see DDRVSX for de"
	    "tails):\002)";
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
	    " 12=Well-cond., random complex \002,\002         \002,\002 17=Il"
	    "l-cond., large rand. complx \002,/\002 13=Ill-condi\002,\002tion"
	    "ed, evenly spaced.     \002,\002 18=Ill-cond., small rand.\002"
	    ",\002 complx \002)";
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
	    "T same no matter what else computed (sort),\002,\002  1/ulp othe"
	    "rwise\002,/\002 12 = 0 if WR, WI same no matter what else comput"
	    "ed \002,\002(sort), 1/ulp otherwise\002,/\002 13 = 0 if sorting "
	    "succesful, 1/ulp otherwise\002,/\002 14 = 0 if RCONDE same no ma"
	    "tter what else computed,\002,\002 1/ulp otherwise\002,/\002 15 ="
	    " 0 if RCONDv same no matter what else computed,\002,\002 1/ulp o"
	    "therwise\002,/\002 16 = | RCONDE - RCONDE(precomputed) | / cond("
	    "RCONDE),\002,/\002 17 = | RCONDV - RCONDV(precomputed) | / cond("
	    "RCONDV),\002)";
    static char fmt_9993[] = "(\002 N=\002,i5,\002, IWK=\002,i2,\002, seed"
	    "=\002,4(i4,\002,\002),\002 type \002,i2,\002, test(\002,i2,\002)="
	    "\002,g10.3)";
    static char fmt_9992[] = "(\002 N=\002,i5,\002, input example =\002,i3"
	    ",\002,  test(\002,i2,\002)=\002,g10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, ht_dim1, ht_offset, vs_dim1, 
	    vs_offset, vs1_dim1, vs1_offset, i__1, i__2, i__3, i__4;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void),
	     s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);

    /* Local variables */
    integer i__, j, n, iwk;
    doublereal ulp, cond;
    integer jcol;
    char path[3];
    integer nmax;
    doublereal unfl, ovfl;
    logical badnn;
    integer nfail;
    extern /* Subroutine */ int dget24_(logical *, integer *, doublereal *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, logical *, integer *);
    integer imode, iinfo;
    doublereal conds, anorm;
    integer islct[20], nslct, jsize, nerrs, itype, jtype, ntest;
    doublereal rtulp;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    doublereal rcdein;
    char adumma[1*1];
    extern /* Subroutine */ int dlatme_(integer *, char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, char *, char 
	    *, char *, char *, doublereal *, integer *, doublereal *, integer 
	    *, integer *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *);
    integer idumma[1], ioldsd[4];
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    xerbla_(char *, integer *), dlatmr_(integer *, integer *, 
	    char *, integer *, char *, doublereal *, integer *, doublereal *, 
	    doublereal *, char *, char *, doublereal *, integer *, doublereal 
	    *, doublereal *, integer *, doublereal *, char *, integer *, 
	    integer *, integer *, doublereal *, doublereal *, char *, 
	    doublereal *, integer *, integer *, integer *), dlatms_(integer *, integer *, 
	    char *, integer *, char *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, char *, doublereal *, integer 
	    *, doublereal *, integer *);
    doublereal rcdvin;
    extern /* Subroutine */ int dlasum_(char *, integer *, integer *, integer 
	    *);
    integer ntestf;
    doublereal ulpinv;
    integer nnwork;
    doublereal rtulpi;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___32 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___48 = { 0, 0, 1, 0, 0 };
    static cilist io___49 = { 0, 0, 0, 0, 0 };
    static cilist io___51 = { 0, 0, 0, 0, 0 };
    static cilist io___52 = { 0, 0, 0, 0, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9992, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     DDRVSX checks the nonsymmetric eigenvalue (Schur form) problem */
/*     expert driver DGEESX. */

/*     DDRVSX uses both test matrices generated randomly depending on */
/*     data supplied in the calling sequence, as well as on data */
/*     read from an input file and including precomputed condition */
/*     numbers to which it compares the ones it computes. */

/*     When DDRVSX is called, a number of matrix "sizes" ("n's") and a */
/*     number of matrix "types" are specified.  For each size ("n") */
/*     and each type of matrix, one matrix will be generated and used */
/*     to test the nonsymmetric eigenroutines.  For each matrix, 15 */
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
/*             If workspace sufficient, also compare WR, WI with and */
/*             without reciprocal condition numbers */
/*             (with sorting of eigenvalues) */

/*     (11)    0     if T(with VS) = T(without VS), */
/*             1/ulp otherwise */
/*             If workspace sufficient, also compare T with and without */
/*             reciprocal condition numbers */
/*             (with sorting of eigenvalues) */

/*     (12)    0     if eigenvalues(with VS) = eigenvalues(without VS), */
/*             1/ulp otherwise */
/*             If workspace sufficient, also compare VS with and without */
/*             reciprocal condition numbers */
/*             (with sorting of eigenvalues) */

/*     (13)    if sorting worked and SDIM is the number of */
/*             eigenvalues which were SELECTed */
/*             If workspace sufficient, also compare SDIM with and */
/*             without reciprocal condition numbers */

/*     (14)    if RCONDE the same no matter if VS and/or RCONDV computed */

/*     (15)    if RCONDV the same no matter if VS and/or RCONDE computed */

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

/*     In addition, an input file will be read from logical unit number */
/*     NIUNIT. The file contains matrices along with precomputed */
/*     eigenvalues and reciprocal condition numbers for the eigenvalue */
/*     average and right invariant subspace. For these matrices, in */
/*     addition to tests (1) to (15) we will compute the following two */
/*     tests: */

/*    (16)  |RCONDE - RCDEIN| / cond(RCONDE) */

/*       RCONDE is the reciprocal average eigenvalue condition number */
/*       computed by DGEESX and RCDEIN (the precomputed true value) */
/*       is supplied as input.  cond(RCONDE) is the condition number */
/*       of RCONDE, and takes errors in computing RCONDE into account, */
/*       so that the resulting quantity should be O(ULP). cond(RCONDE) */
/*       is essentially given by norm(A)/RCONDV. */

/*    (17)  |RCONDV - RCDVIN| / cond(RCONDV) */

/*       RCONDV is the reciprocal right invariant subspace condition */
/*       number computed by DGEESX and RCDVIN (the precomputed true */
/*       value) is supplied as input. cond(RCONDV) is the condition */
/*       number of RCONDV, and takes errors in computing RCONDV into */
/*       account, so that the resulting quantity should be O(ULP). */
/*       cond(RCONDV) is essentially given by norm(A)/RCONDE. */

/*  Arguments */
/*  ========= */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  NSIZES must be at */
/*          least zero. If it is zero, no randomly generated matrices */
/*          are tested, but any test matrices read from NIUNIT will be */
/*          tested. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE. NTYPES must be at least */
/*          zero. If it is zero, no randomly generated test matrices */
/*          are tested, but and test matrices read from NIUNIT will be */
/*          tested. If it is MAXTYP+1 and NSIZES is 1, then an */
/*          additional type, MAXTYP+1 is defined, which is to use */
/*          whatever matrix is in A.  This is only useful if */
/*          DOTYPE(1:MAXTYP) is .FALSE. and DOTYPE(MAXTYP+1) is .TRUE. . */

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
/*          next call to DDRVSX to continue the same random number */
/*          sequence. */

/*  THRESH  (input) DOUBLE PRECISION */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error */
/*          is scaled to be O(1), so THRESH should be a reasonably */
/*          small multiple of 1, e.g., 10 or 100.  In particular, */
/*          it should not depend on the precision (single vs. double) */
/*          or the size of the matrix.  It must be at least zero. */

/*  NIUNIT  (input) INTEGER */
/*          The FORTRAN unit number for reading in the data file of */
/*          problems to solve. */

/*  NOUNIT  (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns INFO not equal to 0.) */

/*  A       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least max( NN ). */

/*  H       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Another copy of the test matrix A, modified by DGEESX. */

/*  HT      (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Yet another copy of the test matrix A, modified by DGEESX. */

/*  WR      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  WI      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          The real and imaginary parts of the eigenvalues of A. */
/*          On exit, WR + WI*i are the eigenvalues of the matrix in A. */

/*  WRT     (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  WIT     (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but those computed when DGEESX only computes a partial */
/*          eigendecomposition, i.e. not Schur vectors */

/*  WRTMP   (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  WITMP   (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          More temporary storage for eigenvalues. */

/*  VS      (workspace) DOUBLE PRECISION array, dimension (LDVS, max(NN)) */
/*          VS holds the computed Schur vectors. */

/*  LDVS    (input) INTEGER */
/*          Leading dimension of VS. Must be at least max(1,max(NN)). */

/*  VS1     (workspace) DOUBLE PRECISION array, dimension (LDVS, max(NN)) */
/*          VS1 holds another copy of the computed Schur vectors. */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (17) */
/*          The values computed by the 17 tests described above. */
/*          The values are currently limited to 1/ulp, to avoid overflow. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          max(3*NN(j),2*NN(j)**2) for all j. */

/*  IWORK   (workspace) INTEGER array, dimension (max(NN)*max(NN)) */

/*  INFO    (output) INTEGER */
/*          If 0,  successful exit. */
/*            <0,  input parameter -INFO is incorrect */
/*            >0,  DLATMR, SLATMS, SLATME or DGET24 returned an error */
/*                 code and INFO is its absolute value */

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
    --wrtmp;
    --witmp;
    vs1_dim1 = *ldvs;
    vs1_offset = 1 + vs1_dim1;
    vs1 -= vs1_offset;
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
    s_copy(path + 1, "SX", (ftnlen)2, (ftnlen)2);

/*     Check for errors */

    ntestt = 0;
    ntestf = 0;
    *info = 0;

/*     Important constants */

    badnn = FALSE_;

/*     12 is the largest dimension in the input file of precomputed */
/*     problems */

    nmax = 12;
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
    } else if (*niunit <= 0) {
	*info = -7;
    } else if (*nounit <= 0) {
	*info = -8;
    } else if (*lda < 1 || *lda < nmax) {
	*info = -10;
    } else if (*ldvs < 1 || *ldvs < nmax) {
	*info = -20;
    } else /* if(complicated condition) */ {
/* Computing MAX */
/* Computing 2nd power */
	i__3 = nmax;
	i__1 = nmax * 3, i__2 = i__3 * i__3 << 1;
	if (max(i__1,i__2) > *lwork) {
	    *info = -24;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DDRVSX", &i__1);
	return 0;
    }

/*     If nothing to do check on NIUNIT */

    if (*nsizes == 0 || *ntypes == 0) {
	goto L150;
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
		goto L130;
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

	    dlaset_("Full", lda, &n, &c_b18, &c_b18, &a[a_offset], lda);
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
		dlatme_(&n, "S", &iseed[1], &work[1], &imode, &cond, &c_b32, 
			adumma, "T", "T", "T", &work[n + 1], &c__4, &conds, &
			n, &n, &anorm, &a[a_offset], lda, &work[(n << 1) + 1], 
			 &iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b32, 
			&c_b32, "T", "N", &work[n + 1], &c__1, &c_b32, &work[(
			n << 1) + 1], &c__1, &c_b32, "N", idumma, &c__0, &
			c__0, &c_b18, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b32, 
			&c_b32, "T", "N", &work[n + 1], &c__1, &c_b32, &work[(
			n << 1) + 1], &c__1, &c_b32, "N", idumma, &n, &n, &
			c_b18, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              General, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "N", &work[1], &c__6, &c_b32, 
			&c_b32, "T", "N", &work[n + 1], &c__1, &c_b32, &work[(
			n << 1) + 1], &c__1, &c_b32, "N", idumma, &n, &n, &
			c_b18, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);
		if (n >= 4) {
		    dlaset_("Full", &c__2, &n, &c_b18, &c_b18, &a[a_offset], 
			    lda);
		    i__3 = n - 3;
		    dlaset_("Full", &i__3, &c__1, &c_b18, &c_b18, &a[a_dim1 + 
			    3], lda);
		    i__3 = n - 3;
		    dlaset_("Full", &i__3, &c__2, &c_b18, &c_b18, &a[(n - 1) *
			     a_dim1 + 3], lda);
		    dlaset_("Full", &c__1, &n, &c_b18, &c_b18, &a[n + a_dim1], 
			     lda);
		}

	    } else if (itype == 10) {

/*              Triangular, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "N", &work[1], &c__6, &c_b32, 
			&c_b32, "T", "N", &work[n + 1], &c__1, &c_b32, &work[(
			n << 1) + 1], &c__1, &c_b32, "N", idumma, &n, &c__0, &
			c_b18, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
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
/* Computing MAX */
		    i__3 = n * 3, i__4 = (n << 1) * n;
		    nnwork = max(i__3,i__4);
		}
		nnwork = max(nnwork,1);

		dget24_(&c_false, &jtype, thresh, ioldsd, nounit, &n, &a[
			a_offset], lda, &h__[h_offset], &ht[ht_offset], &wr[1]
, &wi[1], &wrt[1], &wit[1], &wrtmp[1], &witmp[1], &vs[
			vs_offset], ldvs, &vs1[vs1_offset], &rcdein, &rcdvin, 
			&nslct, islct, &result[1], &work[1], &nnwork, &iwork[
			1], &bwork[1], info);

/*              Check for RESULT(j) > THRESH */

		ntest = 0;
		nfail = 0;
		for (j = 1; j <= 15; ++j) {
		    if (result[j] >= 0.) {
			++ntest;
		    }
		    if (result[j] >= *thresh) {
			++nfail;
		    }
/* L100: */
		}

		if (nfail > 0) {
		    ++ntestf;
		}
		if (ntestf == 1) {
		    io___41.ciunit = *nounit;
		    s_wsfe(&io___41);
		    do_fio(&c__1, path, (ftnlen)3);
		    e_wsfe();
		    io___42.ciunit = *nounit;
		    s_wsfe(&io___42);
		    e_wsfe();
		    io___43.ciunit = *nounit;
		    s_wsfe(&io___43);
		    e_wsfe();
		    io___44.ciunit = *nounit;
		    s_wsfe(&io___44);
		    e_wsfe();
		    io___45.ciunit = *nounit;
		    s_wsfe(&io___45);
		    do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(
			    doublereal));
		    e_wsfe();
		    io___46.ciunit = *nounit;
		    s_wsfe(&io___46);
		    e_wsfe();
		    ntestf = 2;
		}

		for (j = 1; j <= 15; ++j) {
		    if (result[j] >= *thresh) {
			io___47.ciunit = *nounit;
			s_wsfe(&io___47);
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
/* L110: */
		}

		nerrs += nfail;
		ntestt += ntest;

/* L120: */
	    }
L130:
	    ;
	}
/* L140: */
    }

L150:

/*     Read in data from file to check accuracy of condition estimation */
/*     Read input data until N=0 */

    jtype = 0;
L160:
    io___48.ciunit = *niunit;
    i__1 = s_rsle(&io___48);
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = do_lio(&c__3, &c__1, (char *)&nslct, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L200;
    }
    i__1 = e_rsle();
    if (i__1 != 0) {
	goto L200;
    }
    if (n == 0) {
	goto L200;
    }
    ++jtype;
    iseed[1] = jtype;
    if (nslct > 0) {
	io___49.ciunit = *niunit;
	s_rsle(&io___49);
	i__1 = nslct;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_lio(&c__3, &c__1, (char *)&islct[i__ - 1], (ftnlen)sizeof(
		    integer));
	}
	e_rsle();
    }
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___51.ciunit = *niunit;
	s_rsle(&io___51);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__5, &c__1, (char *)&a[i__ + j * a_dim1], (ftnlen)sizeof(
		    doublereal));
	}
	e_rsle();
/* L170: */
    }
    io___52.ciunit = *niunit;
    s_rsle(&io___52);
    do_lio(&c__5, &c__1, (char *)&rcdein, (ftnlen)sizeof(doublereal));
    do_lio(&c__5, &c__1, (char *)&rcdvin, (ftnlen)sizeof(doublereal));
    e_rsle();

    dget24_(&c_true, &c__22, thresh, &iseed[1], nounit, &n, &a[a_offset], lda, 
	     &h__[h_offset], &ht[ht_offset], &wr[1], &wi[1], &wrt[1], &wit[1], 
	     &wrtmp[1], &witmp[1], &vs[vs_offset], ldvs, &vs1[vs1_offset], &
	    rcdein, &rcdvin, &nslct, islct, &result[1], &work[1], lwork, &
	    iwork[1], &bwork[1], info);

/*     Check for RESULT(j) > THRESH */

    ntest = 0;
    nfail = 0;
    for (j = 1; j <= 17; ++j) {
	if (result[j] >= 0.) {
	    ++ntest;
	}
	if (result[j] >= *thresh) {
	    ++nfail;
	}
/* L180: */
    }

    if (nfail > 0) {
	++ntestf;
    }
    if (ntestf == 1) {
	io___53.ciunit = *nounit;
	s_wsfe(&io___53);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___54.ciunit = *nounit;
	s_wsfe(&io___54);
	e_wsfe();
	io___55.ciunit = *nounit;
	s_wsfe(&io___55);
	e_wsfe();
	io___56.ciunit = *nounit;
	s_wsfe(&io___56);
	e_wsfe();
	io___57.ciunit = *nounit;
	s_wsfe(&io___57);
	do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(doublereal));
	e_wsfe();
	io___58.ciunit = *nounit;
	s_wsfe(&io___58);
	e_wsfe();
	ntestf = 2;
    }
    for (j = 1; j <= 17; ++j) {
	if (result[j] >= *thresh) {
	    io___59.ciunit = *nounit;
	    s_wsfe(&io___59);
	    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&result[j], (ftnlen)sizeof(doublereal));
	    e_wsfe();
	}
/* L190: */
    }

    nerrs += nfail;
    ntestt += ntest;
    goto L160;
L200:

/*     Summary */

    dlasum_(path, nounit, &nerrs, &ntestt);



    return 0;

/*     End of DDRVSX */

} /* ddrvsx_ */
