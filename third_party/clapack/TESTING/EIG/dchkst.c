#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c_n1 = -1;
static integer c__2 = 2;
static doublereal c_b25 = 0.;
static integer c__0 = 0;
static integer c__6 = 6;
static doublereal c_b39 = 1.;
static integer c__4 = 4;
static integer c__3 = 3;
static integer c__10 = 10;
static integer c__11 = 11;

/* Subroutine */ int dchkst_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, integer *nounit, 
	doublereal *a, integer *lda, doublereal *ap, doublereal *sd, 
	doublereal *se, doublereal *d1, doublereal *d2, doublereal *d3, 
	doublereal *d4, doublereal *d5, doublereal *wa1, doublereal *wa2, 
	doublereal *wa3, doublereal *wr, doublereal *u, integer *ldu, 
	doublereal *v, doublereal *vp, doublereal *tau, doublereal *z__, 
	doublereal *work, integer *lwork, integer *iwork, integer *liwork, 
	doublereal *result, integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,4,4,4,4,4,5,5,5,5,5,8,8,8,9,9,9,9,9,10 };
    static integer kmagn[21] = { 1,1,1,1,1,2,3,1,1,1,2,3,1,2,3,1,1,1,2,3,1 };
    static integer kmode[21] = { 0,0,4,3,1,4,4,4,3,1,4,4,0,0,0,4,3,1,4,4,3 };

    /* Format strings */
    static char fmt_9999[] = "(\002 DCHKST: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9998[] = "(/1x,a3,\002 -- Real Symmetric eigenvalue prob"
	    "lem\002)";
    static char fmt_9997[] = "(\002 Matrix types (see DCHKST for details):"
	    " \002)";
    static char fmt_9996[] = "(/\002 Special Matrices:\002,/\002  1=Zero mat"
	    "rix.                        \002,\002  5=Diagonal: clustered ent"
	    "ries.\002,/\002  2=Identity matrix.                    \002,\002"
	    "  6=Diagonal: large, evenly spaced.\002,/\002  3=Diagonal: evenl"
	    "y spaced entries.    \002,\002  7=Diagonal: small, evenly spaced."
	    "\002,/\002  4=Diagonal: geometr. spaced entries.\002)";
    static char fmt_9995[] = "(\002 Dense \002,a,\002 Matrices:\002,/\002  8"
	    "=Evenly spaced eigenvals.            \002,\002 12=Small, evenly "
	    "spaced eigenvals.\002,/\002  9=Geometrically spaced eigenvals.  "
	    "   \002,\002 13=Matrix with random O(1) entries.\002,/\002 10=Cl"
	    "ustered eigenvalues.              \002,\002 14=Matrix with large"
	    " random entries.\002,/\002 11=Large, evenly spaced eigenvals.   "
	    "  \002,\002 15=Matrix with small random entries.\002)";
    static char fmt_9994[] = "(\002 16=Positive definite, evenly spaced eige"
	    "nvalues\002,/\002 17=Positive definite, geometrically spaced eig"
	    "envlaues\002,/\002 18=Positive definite, clustered eigenvalue"
	    "s\002,/\002 19=Positive definite, small evenly spaced eigenvalues"
	    "\002,/\002 20=Positive definite, large evenly spaced eigenvalue"
	    "s\002,/\002 21=Diagonally dominant tridiagonal, geometrically"
	    "\002,\002 spaced eigenvalues\002)";
    static char fmt_9988[] = "(/\002Test performed:  see DCHKST for details"
	    ".\002,/)";
    static char fmt_9990[] = "(\002 N=\002,i5,\002, seed=\002,4(i4,\002,\002"
	    "),\002 type \002,i2,\002, test(\002,i2,\002)=\002,g10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, u_dim1, u_offset, v_dim1, v_offset, z_dim1, 
	    z_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double log(doublereal), sqrt(doublereal);
    integer pow_ii(integer *, integer *), s_wsfe(cilist *), do_fio(integer *, 
	    char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, m, n, m2, m3, jc, il, jr, iu;
    doublereal vl, vu;
    integer nap, lgn;
    doublereal ulp, cond;
    integer nmax;
    doublereal unfl, ovfl, temp1, temp2, temp3, temp4;
    extern doublereal dsxt1_(integer *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, doublereal *, doublereal *);
    logical badnn;
    integer imode, lwedc;
    doublereal dumma[1];
    integer iinfo;
    doublereal aninv, anorm;
    extern /* Subroutine */ int dspt21_(integer *, char *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *);
    integer itemp;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), dstt21_(integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *);
    integer nmats;
    extern /* Subroutine */ int dstt22_(integer *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *);
    integer jsize;
    extern /* Subroutine */ int dsyt21_(integer *, char *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *);
    integer nerrs, itype, jtype, ntest, iseed2[4], log2ui;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlarnd_(integer *, integer *);
    extern /* Subroutine */ int dstedc_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *);
    integer liwedc, nblock;
    extern /* Subroutine */ int dstech_(integer *, doublereal *, doublereal *, 
	     doublereal *, doublereal *, doublereal *, integer *);
    integer idumma[1];
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ioldsd[4];
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *, 
	    integer *, integer *);
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    dlatmr_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, char *, char 
	    *, doublereal *, integer *, doublereal *, doublereal *, integer *, 
	     doublereal *, char *, integer *, integer *, integer *, 
	    doublereal *, doublereal *, char *, doublereal *, integer *, 
	    integer *, integer *);
    doublereal abstol;
    extern /* Subroutine */ int dlasum_(char *, integer *, integer *, integer 
	    *), dlatms_(integer *, integer *, char *, integer *, char 
	    *, doublereal *, integer *, doublereal *, doublereal *, integer *, 
	     integer *, char *, doublereal *, integer *, doublereal *, 
	    integer *), dstein_(integer *, doublereal 
	    *, doublereal *, integer *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *), dsterf_(integer *, doublereal *, doublereal *, 
	    integer *), xerbla_(char *, integer *), dstebz_(char *, 
	    char *, integer *, doublereal *, doublereal *, integer *, integer 
	    *, doublereal *, doublereal *, doublereal *, integer *, integer *, 
	     doublereal *, integer *, integer *, doublereal *, integer *, 
	    integer *), dstemr_(char *, char *, integer *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     integer *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, integer *, logical *, doublereal *, integer *, integer 
	    *, integer *, integer *), dopgtr_(char *, integer 
	    *, doublereal *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *), dpteqr_(char *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *), dorgtr_(char *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, integer *), dsptrd_(char *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *), dsteqr_(char *, 
	    integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *);
    logical tryrac;
    integer nsplit;
    doublereal rtunfl, rtovfl, ulpinv;
    extern /* Subroutine */ int dsytrd_(char *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *, 
	     integer *, integer *);
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___84 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___85 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___86 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___87 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___88 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___89 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___90 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___91 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___92 = { 0, 0, 0, fmt_9990, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKST  checks the symmetric eigenvalue problem routines. */

/*     DSYTRD factors A as  U S U' , where ' means transpose, */
/*     S is symmetric tridiagonal, and U is orthogonal. */
/*     DSYTRD can use either just the lower or just the upper triangle */
/*     of A; DCHKST checks both cases. */
/*     U is represented as a product of Householder */
/*     transformations, whose vectors are stored in the first */
/*     n-1 columns of V, and whose scale factors are in TAU. */

/*     DSPTRD does the same as DSYTRD, except that A and V are stored */
/*     in "packed" format. */

/*     DORGTR constructs the matrix U from the contents of V and TAU. */

/*     DOPGTR constructs the matrix U from the contents of VP and TAU. */

/*     DSTEQR factors S as  Z D1 Z' , where Z is the orthogonal */
/*     matrix of eigenvectors and D1 is a diagonal matrix with */
/*     the eigenvalues on the diagonal.  D2 is the matrix of */
/*     eigenvalues computed when Z is not computed. */

/*     DSTERF computes D3, the matrix of eigenvalues, by the */
/*     PWK method, which does not yield eigenvectors. */

/*     DPTEQR factors S as  Z4 D4 Z4' , for a */
/*     symmetric positive definite tridiagonal matrix. */
/*     D5 is the matrix of eigenvalues computed when Z is not */
/*     computed. */

/*     DSTEBZ computes selected eigenvalues.  WA1, WA2, and */
/*     WA3 will denote eigenvalues computed to high */
/*     absolute accuracy, with different range options. */
/*     WR will denote eigenvalues computed to high relative */
/*     accuracy. */

/*     DSTEIN computes Y, the eigenvectors of S, given the */
/*     eigenvalues. */

/*     DSTEDC factors S as Z D1 Z' , where Z is the orthogonal */
/*     matrix of eigenvectors and D1 is a diagonal matrix with */
/*     the eigenvalues on the diagonal ('I' option). It may also */
/*     update an input orthogonal matrix, usually the output */
/*     from DSYTRD/DORGTR or DSPTRD/DOPGTR ('V' option). It may */
/*     also just compute eigenvalues ('N' option). */

/*     DSTEMR factors S as Z D1 Z' , where Z is the orthogonal */
/*     matrix of eigenvectors and D1 is a diagonal matrix with */
/*     the eigenvalues on the diagonal ('I' option).  DSTEMR */
/*     uses the Relatively Robust Representation whenever possible. */

/*  When DCHKST is called, a number of matrix "sizes" ("n's") and a */
/*  number of matrix "types" are specified.  For each size ("n") */
/*  and each type of matrix, one matrix will be generated and used */
/*  to test the symmetric eigenroutines.  For each matrix, a number */
/*  of tests will be performed: */

/*  (1)     | A - V S V' | / ( |A| n ulp ) DSYTRD( UPLO='U', ... ) */

/*  (2)     | I - UV' | / ( n ulp )        DORGTR( UPLO='U', ... ) */

/*  (3)     | A - V S V' | / ( |A| n ulp ) DSYTRD( UPLO='L', ... ) */

/*  (4)     | I - UV' | / ( n ulp )        DORGTR( UPLO='L', ... ) */

/*  (5-8)   Same as 1-4, but for DSPTRD and DOPGTR. */

/*  (9)     | S - Z D Z' | / ( |S| n ulp ) DSTEQR('V',...) */

/*  (10)    | I - ZZ' | / ( n ulp )        DSTEQR('V',...) */

/*  (11)    | D1 - D2 | / ( |D1| ulp )        DSTEQR('N',...) */

/*  (12)    | D1 - D3 | / ( |D1| ulp )        DSTERF */

/*  (13)    0 if the true eigenvalues (computed by sturm count) */
/*          of S are within THRESH of */
/*          those in D1.  2*THRESH if they are not.  (Tested using */
/*          DSTECH) */

/*  For S positive definite, */

/*  (14)    | S - Z4 D4 Z4' | / ( |S| n ulp ) DPTEQR('V',...) */

/*  (15)    | I - Z4 Z4' | / ( n ulp )        DPTEQR('V',...) */

/*  (16)    | D4 - D5 | / ( 100 |D4| ulp )       DPTEQR('N',...) */

/*  When S is also diagonally dominant by the factor gamma < 1, */

/*  (17)    max | D4(i) - WR(i) | / ( |D4(i)| omega ) , */
/*           i */
/*          omega = 2 (2n-1) ULP (1 + 8 gamma**2) / (1 - gamma)**4 */
/*                                               DSTEBZ( 'A', 'E', ...) */

/*  (18)    | WA1 - D3 | / ( |D3| ulp )          DSTEBZ( 'A', 'E', ...) */

/*  (19)    ( max { min | WA2(i)-WA3(j) | } + */
/*             i     j */
/*            max { min | WA3(i)-WA2(j) | } ) / ( |D3| ulp ) */
/*             i     j */
/*                                               DSTEBZ( 'I', 'E', ...) */

/*  (20)    | S - Y WA1 Y' | / ( |S| n ulp )  DSTEBZ, SSTEIN */

/*  (21)    | I - Y Y' | / ( n ulp )          DSTEBZ, SSTEIN */

/*  (22)    | S - Z D Z' | / ( |S| n ulp )    DSTEDC('I') */

/*  (23)    | I - ZZ' | / ( n ulp )           DSTEDC('I') */

/*  (24)    | S - Z D Z' | / ( |S| n ulp )    DSTEDC('V') */

/*  (25)    | I - ZZ' | / ( n ulp )           DSTEDC('V') */

/*  (26)    | D1 - D2 | / ( |D1| ulp )           DSTEDC('V') and */
/*                                               DSTEDC('N') */

/*  Test 27 is disabled at the moment because DSTEMR does not */
/*  guarantee high relatvie accuracy. */

/*  (27)    max | D6(i) - WR(i) | / ( |D6(i)| omega ) , */
/*           i */
/*          omega = 2 (2n-1) ULP (1 + 8 gamma**2) / (1 - gamma)**4 */
/*                                               DSTEMR('V', 'A') */

/*  (28)    max | D6(i) - WR(i) | / ( |D6(i)| omega ) , */
/*           i */
/*          omega = 2 (2n-1) ULP (1 + 8 gamma**2) / (1 - gamma)**4 */
/*                                               DSTEMR('V', 'I') */

/*  Tests 29 through 34 are disable at present because DSTEMR */
/*  does not handle partial specturm requests. */

/*  (29)    | S - Z D Z' | / ( |S| n ulp )    DSTEMR('V', 'I') */

/*  (30)    | I - ZZ' | / ( n ulp )           DSTEMR('V', 'I') */

/*  (31)    ( max { min | WA2(i)-WA3(j) | } + */
/*             i     j */
/*            max { min | WA3(i)-WA2(j) | } ) / ( |D3| ulp ) */
/*             i     j */
/*          DSTEMR('N', 'I') vs. SSTEMR('V', 'I') */

/*  (32)    | S - Z D Z' | / ( |S| n ulp )    DSTEMR('V', 'V') */

/*  (33)    | I - ZZ' | / ( n ulp )           DSTEMR('V', 'V') */

/*  (34)    ( max { min | WA2(i)-WA3(j) | } + */
/*             i     j */
/*            max { min | WA3(i)-WA2(j) | } ) / ( |D3| ulp ) */
/*             i     j */
/*          DSTEMR('N', 'V') vs. SSTEMR('V', 'V') */

/*  (35)    | S - Z D Z' | / ( |S| n ulp )    DSTEMR('V', 'A') */

/*  (36)    | I - ZZ' | / ( n ulp )           DSTEMR('V', 'A') */

/*  (37)    ( max { min | WA2(i)-WA3(j) | } + */
/*             i     j */
/*            max { min | WA3(i)-WA2(j) | } ) / ( |D3| ulp ) */
/*             i     j */
/*          DSTEMR('N', 'A') vs. SSTEMR('V', 'A') */

/*  The "sizes" are specified by an array NN(1:NSIZES); the value of */
/*  each element NN(j) specifies one size. */
/*  The "types" are specified by a logical array DOTYPE( 1:NTYPES ); */
/*  if DOTYPE(j) is .TRUE., then matrix type "j" will be generated. */
/*  Currently, the list of possible types is: */

/*  (1)  The zero matrix. */
/*  (2)  The identity matrix. */

/*  (3)  A diagonal matrix with evenly spaced entries */
/*       1, ..., ULP  and random signs. */
/*       (ULP = (first number larger than 1) - 1 ) */
/*  (4)  A diagonal matrix with geometrically spaced entries */
/*       1, ..., ULP  and random signs. */
/*  (5)  A diagonal matrix with "clustered" entries 1, ULP, ..., ULP */
/*       and random signs. */

/*  (6)  Same as (4), but multiplied by SQRT( overflow threshold ) */
/*  (7)  Same as (4), but multiplied by SQRT( underflow threshold ) */

/*  (8)  A matrix of the form  U' D U, where U is orthogonal and */
/*       D has evenly spaced entries 1, ..., ULP with random signs */
/*       on the diagonal. */

/*  (9)  A matrix of the form  U' D U, where U is orthogonal and */
/*       D has geometrically spaced entries 1, ..., ULP with random */
/*       signs on the diagonal. */

/*  (10) A matrix of the form  U' D U, where U is orthogonal and */
/*       D has "clustered" entries 1, ULP,..., ULP with random */
/*       signs on the diagonal. */

/*  (11) Same as (8), but multiplied by SQRT( overflow threshold ) */
/*  (12) Same as (8), but multiplied by SQRT( underflow threshold ) */

/*  (13) Symmetric matrix with random entries chosen from (-1,1). */
/*  (14) Same as (13), but multiplied by SQRT( overflow threshold ) */
/*  (15) Same as (13), but multiplied by SQRT( underflow threshold ) */
/*  (16) Same as (8), but diagonal elements are all positive. */
/*  (17) Same as (9), but diagonal elements are all positive. */
/*  (18) Same as (10), but diagonal elements are all positive. */
/*  (19) Same as (16), but multiplied by SQRT( overflow threshold ) */
/*  (20) Same as (16), but multiplied by SQRT( underflow threshold ) */
/*  (21) A diagonally dominant tridiagonal matrix with geometrically */
/*       spaced diagonal entries 1, ..., ULP. */

/*  Arguments */
/*  ========= */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          DCHKST does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, DCHKST */
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
/*          next call to DCHKST to continue the same random number */
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
/*          (e.g., if a routine returns IINFO not equal to 0.) */

/*  A       (input/workspace/output) DOUBLE PRECISION array of */
/*                                  dimension ( LDA , max(NN) ) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually */
/*          used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at */
/*          least 1 and at least max( NN ). */

/*  AP      (workspace) DOUBLE PRECISION array of */
/*                      dimension( max(NN)*max(NN+1)/2 ) */
/*          The matrix A stored in packed format. */

/*  SD      (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( max(NN) ) */
/*          The diagonal of the tridiagonal matrix computed by DSYTRD. */
/*          On exit, SD and SE contain the tridiagonal form of the */
/*          matrix in A. */

/*  SE      (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( max(NN) ) */
/*          The off-diagonal of the tridiagonal matrix computed by */
/*          DSYTRD.  On exit, SD and SE contain the tridiagonal form of */
/*          the matrix in A. */

/*  D1      (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( max(NN) ) */
/*          The eigenvalues of A, as computed by DSTEQR simlutaneously */
/*          with Z.  On exit, the eigenvalues in D1 correspond with the */
/*          matrix in A. */

/*  D2      (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( max(NN) ) */
/*          The eigenvalues of A, as computed by DSTEQR if Z is not */
/*          computed.  On exit, the eigenvalues in D2 correspond with */
/*          the matrix in A. */

/*  D3      (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( max(NN) ) */
/*          The eigenvalues of A, as computed by DSTERF.  On exit, the */
/*          eigenvalues in D3 correspond with the matrix in A. */

/*  U       (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( LDU, max(NN) ). */
/*          The orthogonal matrix computed by DSYTRD + DORGTR. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U, Z, and V.  It must be at least 1 */
/*          and at least max( NN ). */

/*  V       (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( LDU, max(NN) ). */
/*          The Housholder vectors computed by DSYTRD in reducing A to */
/*          tridiagonal form.  The vectors computed with UPLO='U' are */
/*          in the upper triangle, and the vectors computed with UPLO='L' */
/*          are in the lower triangle.  (As described in DSYTRD, the */
/*          sub- and superdiagonal are not set to 1, although the */
/*          true Householder vector has a 1 in that position.  The */
/*          routines that use V, such as DORGTR, set those entries to */
/*          1 before using them, and then restore them later.) */

/*  VP      (workspace) DOUBLE PRECISION array of */
/*                      dimension( max(NN)*max(NN+1)/2 ) */
/*          The matrix V stored in packed format. */

/*  TAU     (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( max(NN) ) */
/*          The Householder factors computed by DSYTRD in reducing A */
/*          to tridiagonal form. */

/*  Z       (workspace/output) DOUBLE PRECISION array of */
/*                             dimension( LDU, max(NN) ). */
/*          The orthogonal matrix of eigenvectors computed by DSTEQR, */
/*          DPTEQR, and DSTEIN. */

/*  WORK    (workspace/output) DOUBLE PRECISION array of */
/*                      dimension( LWORK ) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          1 + 4 * Nmax + 2 * Nmax * lg Nmax + 3 * Nmax**2 */
/*          where Nmax = max( NN(j), 2 ) and lg = log base 2. */

/*  IWORK   (workspace/output) INTEGER array, */
/*             dimension (6 + 6*Nmax + 5 * Nmax * lg Nmax ) */
/*          where Nmax = max( NN(j), 2 ) and lg = log base 2. */
/*          Workspace. */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (26) */
/*          The values computed by the tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some NN(j) < 0 */
/*           -3: NTYPES < 0 */
/*           -5: THRESH < 0 */
/*           -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*          -23: LDU < 1 or LDU < NMAX. */
/*          -29: LWORK too small. */
/*          If  DLATMR, SLATMS, DSYTRD, DORGTR, DSTEQR, SSTERF, */
/*              or DORMC2 returns an error code, the */
/*              absolute value of it is returned. */

/* ----------------------------------------------------------------------- */

/*       Some Local Variables and Parameters: */
/*       ---- ----- --------- --- ---------- */
/*       ZERO, ONE       Real 0 and 1. */
/*       MAXTYP          The number of types defined. */
/*       NTEST           The number of tests performed, or which can */
/*                       be performed so far, for the current matrix. */
/*       NTESTT          The total number of tests performed so far. */
/*       NBLOCK          Blocksize as returned by ENVIR. */
/*       NMAX            Largest value in NN. */
/*       NMATS           The number of matrices generated so far. */
/*       NERRS           The number of tests which have exceeded THRESH */
/*                       so far. */
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
    --ap;
    --sd;
    --se;
    --d1;
    --d2;
    --d3;
    --d4;
    --d5;
    --wa1;
    --wa2;
    --wa3;
    --wr;
    z_dim1 = *ldu;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    v_dim1 = *ldu;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --vp;
    --tau;
    --work;
    --iwork;
    --result;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Keep ftnchek happy */
    idumma[0] = 1;

/*     Check for errors */

    ntestt = 0;
    *info = 0;

/*     Important constants */

    badnn = FALSE_;
    tryrac = TRUE_;
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

    nblock = ilaenv_(&c__1, "DSYTRD", "L", &nmax, &c_n1, &c_n1, &c_n1);
/* Computing MIN */
    i__1 = nmax, i__2 = max(1,nblock);
    nblock = min(i__1,i__2);

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
	*info = -23;
    } else /* if(complicated condition) */ {
/* Computing 2nd power */
	i__1 = max(2,nmax);
	if (i__1 * i__1 << 1 > *lwork) {
	    *info = -29;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DCHKST", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

/*     More Important constants */

    unfl = dlamch_("Safe minimum");
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    ulp = dlamch_("Epsilon") * dlamch_("Base");
    ulpinv = 1. / ulp;
    log2ui = (integer) (log(ulpinv) / log(2.));
    rtunfl = sqrt(unfl);
    rtovfl = sqrt(ovfl);

/*     Loop over sizes, types */

    for (i__ = 1; i__ <= 4; ++i__) {
	iseed2[i__ - 1] = iseed[i__];
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
/* Computing 2nd power */
	    i__2 = n;
	    lwedc = (n << 2) + 1 + (n << 1) * lgn + i__2 * i__2 * 3;
	    liwedc = n * 6 + 6 + n * 5 * lgn;
	} else {
	    lwedc = 8;
	    liwedc = 12;
	}
	nap = n * (n + 1) / 2;
	aninv = 1. / (doublereal) max(1,n);

	if (*nsizes != 1) {
	    mtypes = min(21,*ntypes);
	} else {
	    mtypes = min(22,*ntypes);
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L300;
	    }
	    ++nmats;
	    ntest = 0;

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L30: */
	    }

/*           Compute "A" */

/*           Control parameters: */

/*               KMAGN  KMODE        KTYPE */
/*           =1  O(1)   clustered 1  zero */
/*           =2  large  clustered 2  identity */
/*           =3  small  exponential  (none) */
/*           =4         arithmetic   diagonal, (w/ eigenvalues) */
/*           =5         random log   symmetric, w/ eigenvalues */
/*           =6         random       (none) */
/*           =7                      random diagonal */
/*           =8                      random symmetric */
/*           =9                      positive definite */
/*           =10                     diagonally dominant tridiagonal */

	    if (mtypes > 21) {
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
	    anorm = 1.;
	    goto L70;

L50:
	    anorm = rtovfl * ulp * aninv;
	    goto L70;

L60:
	    anorm = rtunfl * n * ulpinv;
	    goto L70;

L70:

	    dlaset_("Full", lda, &n, &c_b25, &c_b25, &a[a_offset], lda);
	    iinfo = 0;
	    if (jtype <= 15) {
		cond = ulpinv;
	    } else {
		cond = ulpinv * aninv / 10.;
	    }

/*           Special Matrices -- Identity & Jordan block */

/*              Zero */

	    if (itype == 1) {
		iinfo = 0;

	    } else if (itype == 2) {

/*              Identity */

		i__3 = n;
		for (jc = 1; jc <= i__3; ++jc) {
		    a[jc + jc * a_dim1] = anorm;
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

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b39, 
			&c_b39, "T", "N", &work[n + 1], &c__1, &c_b39, &work[(
			n << 1) + 1], &c__1, &c_b39, "N", idumma, &c__0, &
			c__0, &c_b25, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b39, 
			&c_b39, "T", "N", &work[n + 1], &c__1, &c_b39, &work[(
			n << 1) + 1], &c__1, &c_b39, "N", idumma, &n, &n, &
			c_b25, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              Positive definite, eigenvalues specified. */

		dlatms_(&n, &n, "S", &iseed[1], "P", &work[1], &imode, &cond, 
			&anorm, &n, &n, "N", &a[a_offset], lda, &work[n + 1], 
			&iinfo);

	    } else if (itype == 10) {

/*              Positive definite tridiagonal, eigenvalues specified. */

		dlatms_(&n, &n, "S", &iseed[1], "P", &work[1], &imode, &cond, 
			&anorm, &c__1, &c__1, "N", &a[a_offset], lda, &work[n 
			+ 1], &iinfo);
		i__3 = n;
		for (i__ = 2; i__ <= i__3; ++i__) {
		    temp1 = (d__1 = a[i__ - 1 + i__ * a_dim1], abs(d__1)) / 
			    sqrt((d__2 = a[i__ - 1 + (i__ - 1) * a_dim1] * a[
			    i__ + i__ * a_dim1], abs(d__2)));
		    if (temp1 > .5) {
			a[i__ - 1 + i__ * a_dim1] = sqrt((d__1 = a[i__ - 1 + (
				i__ - 1) * a_dim1] * a[i__ + i__ * a_dim1], 
				abs(d__1))) * .5;
			a[i__ + (i__ - 1) * a_dim1] = a[i__ - 1 + i__ * 
				a_dim1];
		    }
/* L90: */
		}

	    } else {

		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___40.ciunit = *nounit;
		s_wsfe(&io___40);
		do_fio(&c__1, "Generator", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		return 0;
	    }

L100:

/*           Call DSYTRD and DORGTR to compute S and U from */
/*           upper triangle. */

	    dlacpy_("U", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

	    ntest = 1;
	    dsytrd_("U", &n, &v[v_offset], ldu, &sd[1], &se[1], &tau[1], &
		    work[1], lwork, &iinfo);

	    if (iinfo != 0) {
		io___41.ciunit = *nounit;
		s_wsfe(&io___41);
		do_fio(&c__1, "DSYTRD(U)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[1] = ulpinv;
		    goto L280;
		}
	    }

	    dlacpy_("U", &n, &n, &v[v_offset], ldu, &u[u_offset], ldu);

	    ntest = 2;
	    dorgtr_("U", &n, &u[u_offset], ldu, &tau[1], &work[1], lwork, &
		    iinfo);
	    if (iinfo != 0) {
		io___42.ciunit = *nounit;
		s_wsfe(&io___42);
		do_fio(&c__1, "DORGTR(U)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[2] = ulpinv;
		    goto L280;
		}
	    }

/*           Do tests 1 and 2 */

	    dsyt21_(&c__2, "Upper", &n, &c__1, &a[a_offset], lda, &sd[1], &se[
		    1], &u[u_offset], ldu, &v[v_offset], ldu, &tau[1], &work[
		    1], &result[1]);
	    dsyt21_(&c__3, "Upper", &n, &c__1, &a[a_offset], lda, &sd[1], &se[
		    1], &u[u_offset], ldu, &v[v_offset], ldu, &tau[1], &work[
		    1], &result[2]);

/*           Call DSYTRD and DORGTR to compute S and U from */
/*           lower triangle, do tests. */

	    dlacpy_("L", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

	    ntest = 3;
	    dsytrd_("L", &n, &v[v_offset], ldu, &sd[1], &se[1], &tau[1], &
		    work[1], lwork, &iinfo);

	    if (iinfo != 0) {
		io___43.ciunit = *nounit;
		s_wsfe(&io___43);
		do_fio(&c__1, "DSYTRD(L)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[3] = ulpinv;
		    goto L280;
		}
	    }

	    dlacpy_("L", &n, &n, &v[v_offset], ldu, &u[u_offset], ldu);

	    ntest = 4;
	    dorgtr_("L", &n, &u[u_offset], ldu, &tau[1], &work[1], lwork, &
		    iinfo);
	    if (iinfo != 0) {
		io___44.ciunit = *nounit;
		s_wsfe(&io___44);
		do_fio(&c__1, "DORGTR(L)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[4] = ulpinv;
		    goto L280;
		}
	    }

	    dsyt21_(&c__2, "Lower", &n, &c__1, &a[a_offset], lda, &sd[1], &se[
		    1], &u[u_offset], ldu, &v[v_offset], ldu, &tau[1], &work[
		    1], &result[3]);
	    dsyt21_(&c__3, "Lower", &n, &c__1, &a[a_offset], lda, &sd[1], &se[
		    1], &u[u_offset], ldu, &v[v_offset], ldu, &tau[1], &work[
		    1], &result[4]);

/*           Store the upper triangle of A in AP */

	    i__ = 0;
	    i__3 = n;
	    for (jc = 1; jc <= i__3; ++jc) {
		i__4 = jc;
		for (jr = 1; jr <= i__4; ++jr) {
		    ++i__;
		    ap[i__] = a[jr + jc * a_dim1];
/* L110: */
		}
/* L120: */
	    }

/*           Call DSPTRD and DOPGTR to compute S and U from AP */

	    dcopy_(&nap, &ap[1], &c__1, &vp[1], &c__1);

	    ntest = 5;
	    dsptrd_("U", &n, &vp[1], &sd[1], &se[1], &tau[1], &iinfo);

	    if (iinfo != 0) {
		io___46.ciunit = *nounit;
		s_wsfe(&io___46);
		do_fio(&c__1, "DSPTRD(U)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[5] = ulpinv;
		    goto L280;
		}
	    }

	    ntest = 6;
	    dopgtr_("U", &n, &vp[1], &tau[1], &u[u_offset], ldu, &work[1], &
		    iinfo);
	    if (iinfo != 0) {
		io___47.ciunit = *nounit;
		s_wsfe(&io___47);
		do_fio(&c__1, "DOPGTR(U)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[6] = ulpinv;
		    goto L280;
		}
	    }

/*           Do tests 5 and 6 */

	    dspt21_(&c__2, "Upper", &n, &c__1, &ap[1], &sd[1], &se[1], &u[
		    u_offset], ldu, &vp[1], &tau[1], &work[1], &result[5]);
	    dspt21_(&c__3, "Upper", &n, &c__1, &ap[1], &sd[1], &se[1], &u[
		    u_offset], ldu, &vp[1], &tau[1], &work[1], &result[6]);

/*           Store the lower triangle of A in AP */

	    i__ = 0;
	    i__3 = n;
	    for (jc = 1; jc <= i__3; ++jc) {
		i__4 = n;
		for (jr = jc; jr <= i__4; ++jr) {
		    ++i__;
		    ap[i__] = a[jr + jc * a_dim1];
/* L130: */
		}
/* L140: */
	    }

/*           Call DSPTRD and DOPGTR to compute S and U from AP */

	    dcopy_(&nap, &ap[1], &c__1, &vp[1], &c__1);

	    ntest = 7;
	    dsptrd_("L", &n, &vp[1], &sd[1], &se[1], &tau[1], &iinfo);

	    if (iinfo != 0) {
		io___48.ciunit = *nounit;
		s_wsfe(&io___48);
		do_fio(&c__1, "DSPTRD(L)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[7] = ulpinv;
		    goto L280;
		}
	    }

	    ntest = 8;
	    dopgtr_("L", &n, &vp[1], &tau[1], &u[u_offset], ldu, &work[1], &
		    iinfo);
	    if (iinfo != 0) {
		io___49.ciunit = *nounit;
		s_wsfe(&io___49);
		do_fio(&c__1, "DOPGTR(L)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[8] = ulpinv;
		    goto L280;
		}
	    }

	    dspt21_(&c__2, "Lower", &n, &c__1, &ap[1], &sd[1], &se[1], &u[
		    u_offset], ldu, &vp[1], &tau[1], &work[1], &result[7]);
	    dspt21_(&c__3, "Lower", &n, &c__1, &ap[1], &sd[1], &se[1], &u[
		    u_offset], ldu, &vp[1], &tau[1], &work[1], &result[8]);

/*           Call DSTEQR to compute D1, D2, and Z, do tests. */

/*           Compute D1 and Z */

	    dcopy_(&n, &sd[1], &c__1, &d1[1], &c__1);
	    if (n > 0) {
		i__3 = n - 1;
		dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
	    }
	    dlaset_("Full", &n, &n, &c_b25, &c_b39, &z__[z_offset], ldu);

	    ntest = 9;
	    dsteqr_("V", &n, &d1[1], &work[1], &z__[z_offset], ldu, &work[n + 
		    1], &iinfo);
	    if (iinfo != 0) {
		io___50.ciunit = *nounit;
		s_wsfe(&io___50);
		do_fio(&c__1, "DSTEQR(V)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[9] = ulpinv;
		    goto L280;
		}
	    }

/*           Compute D2 */

	    dcopy_(&n, &sd[1], &c__1, &d2[1], &c__1);
	    if (n > 0) {
		i__3 = n - 1;
		dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
	    }

	    ntest = 11;
	    dsteqr_("N", &n, &d2[1], &work[1], &work[n + 1], ldu, &work[n + 1]
, &iinfo);
	    if (iinfo != 0) {
		io___51.ciunit = *nounit;
		s_wsfe(&io___51);
		do_fio(&c__1, "DSTEQR(N)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[11] = ulpinv;
		    goto L280;
		}
	    }

/*           Compute D3 (using PWK method) */

	    dcopy_(&n, &sd[1], &c__1, &d3[1], &c__1);
	    if (n > 0) {
		i__3 = n - 1;
		dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
	    }

	    ntest = 12;
	    dsterf_(&n, &d3[1], &work[1], &iinfo);
	    if (iinfo != 0) {
		io___52.ciunit = *nounit;
		s_wsfe(&io___52);
		do_fio(&c__1, "DSTERF", (ftnlen)6);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[12] = ulpinv;
		    goto L280;
		}
	    }

/*           Do Tests 9 and 10 */

	    dstt21_(&n, &c__0, &sd[1], &se[1], &d1[1], dumma, &z__[z_offset], 
		    ldu, &work[1], &result[9]);

/*           Do Tests 11 and 12 */

	    temp1 = 0.;
	    temp2 = 0.;
	    temp3 = 0.;
	    temp4 = 0.;

	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = max(
			d__3,d__4), d__4 = (d__2 = d2[j], abs(d__2));
		temp1 = max(d__3,d__4);
/* Computing MAX */
		d__2 = temp2, d__3 = (d__1 = d1[j] - d2[j], abs(d__1));
		temp2 = max(d__2,d__3);
/* Computing MAX */
		d__3 = temp3, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = max(
			d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		temp3 = max(d__3,d__4);
/* Computing MAX */
		d__2 = temp4, d__3 = (d__1 = d1[j] - d3[j], abs(d__1));
		temp4 = max(d__2,d__3);
/* L150: */
	    }

/* Computing MAX */
	    d__1 = unfl, d__2 = ulp * max(temp1,temp2);
	    result[11] = temp2 / max(d__1,d__2);
/* Computing MAX */
	    d__1 = unfl, d__2 = ulp * max(temp3,temp4);
	    result[12] = temp4 / max(d__1,d__2);

/*           Do Test 13 -- Sturm Sequence Test of Eigenvalues */
/*                         Go up by factors of two until it succeeds */

	    ntest = 13;
	    temp1 = *thresh * (.5 - ulp);

	    i__3 = log2ui;
	    for (j = 0; j <= i__3; ++j) {
		dstech_(&n, &sd[1], &se[1], &d1[1], &temp1, &work[1], &iinfo);
		if (iinfo == 0) {
		    goto L170;
		}
		temp1 *= 2.;
/* L160: */
	    }

L170:
	    result[13] = temp1;

/*           For positive definite matrices ( JTYPE.GT.15 ) call DPTEQR */
/*           and do tests 14, 15, and 16 . */

	    if (jtype > 15) {

/*              Compute D4 and Z4 */

		dcopy_(&n, &sd[1], &c__1, &d4[1], &c__1);
		if (n > 0) {
		    i__3 = n - 1;
		    dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		}
		dlaset_("Full", &n, &n, &c_b25, &c_b39, &z__[z_offset], ldu);

		ntest = 14;
		dpteqr_("V", &n, &d4[1], &work[1], &z__[z_offset], ldu, &work[
			n + 1], &iinfo);
		if (iinfo != 0) {
		    io___57.ciunit = *nounit;
		    s_wsfe(&io___57);
		    do_fio(&c__1, "DPTEQR(V)", (ftnlen)9);
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
			result[14] = ulpinv;
			goto L280;
		    }
		}

/*              Do Tests 14 and 15 */

		dstt21_(&n, &c__0, &sd[1], &se[1], &d4[1], dumma, &z__[
			z_offset], ldu, &work[1], &result[14]);

/*              Compute D5 */

		dcopy_(&n, &sd[1], &c__1, &d5[1], &c__1);
		if (n > 0) {
		    i__3 = n - 1;
		    dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		}

		ntest = 16;
		dpteqr_("N", &n, &d5[1], &work[1], &z__[z_offset], ldu, &work[
			n + 1], &iinfo);
		if (iinfo != 0) {
		    io___58.ciunit = *nounit;
		    s_wsfe(&io___58);
		    do_fio(&c__1, "DPTEQR(N)", (ftnlen)9);
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
			result[16] = ulpinv;
			goto L280;
		    }
		}

/*              Do Test 16 */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d4[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d5[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d4[j] - d5[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L180: */
		}

/* Computing MAX */
		d__1 = unfl, d__2 = ulp * 100. * max(temp1,temp2);
		result[16] = temp2 / max(d__1,d__2);
	    } else {
		result[14] = 0.;
		result[15] = 0.;
		result[16] = 0.;
	    }

/*           Call DSTEBZ with different options and do tests 17-18. */

/*              If S is positive definite and diagonally dominant, */
/*              ask for all eigenvalues with high relative accuracy. */

	    vl = 0.;
	    vu = 0.;
	    il = 0;
	    iu = 0;
	    if (jtype == 21) {
		ntest = 17;
		abstol = unfl + unfl;
		dstebz_("A", "E", &n, &vl, &vu, &il, &iu, &abstol, &sd[1], &
			se[1], &m, &nsplit, &wr[1], &iwork[1], &iwork[n + 1], 
			&work[1], &iwork[(n << 1) + 1], &iinfo);
		if (iinfo != 0) {
		    io___66.ciunit = *nounit;
		    s_wsfe(&io___66);
		    do_fio(&c__1, "DSTEBZ(A,rel)", (ftnlen)13);
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
			result[17] = ulpinv;
			goto L280;
		    }
		}

/*              Do test 17 */

		temp2 = (n * 2. - 1.) * 2. * ulp * 3. / .0625;

		temp1 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__2 = d4[j] - wr[n - j + 1], abs(
			    d__2)) / (abstol + (d__1 = d4[j], abs(d__1)));
		    temp1 = max(d__3,d__4);
/* L190: */
		}

		result[17] = temp1 / temp2;
	    } else {
		result[17] = 0.;
	    }

/*           Now ask for all eigenvalues with high absolute accuracy. */

	    ntest = 18;
	    abstol = unfl + unfl;
	    dstebz_("A", "E", &n, &vl, &vu, &il, &iu, &abstol, &sd[1], &se[1], 
		     &m, &nsplit, &wa1[1], &iwork[1], &iwork[n + 1], &work[1], 
		     &iwork[(n << 1) + 1], &iinfo);
	    if (iinfo != 0) {
		io___67.ciunit = *nounit;
		s_wsfe(&io___67);
		do_fio(&c__1, "DSTEBZ(A)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[18] = ulpinv;
		    goto L280;
		}
	    }

/*           Do test 18 */

	    temp1 = 0.;
	    temp2 = 0.;
	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		d__3 = temp1, d__4 = (d__1 = d3[j], abs(d__1)), d__3 = max(
			d__3,d__4), d__4 = (d__2 = wa1[j], abs(d__2));
		temp1 = max(d__3,d__4);
/* Computing MAX */
		d__2 = temp2, d__3 = (d__1 = d3[j] - wa1[j], abs(d__1));
		temp2 = max(d__2,d__3);
/* L200: */
	    }

/* Computing MAX */
	    d__1 = unfl, d__2 = ulp * max(temp1,temp2);
	    result[18] = temp2 / max(d__1,d__2);

/*           Choose random values for IL and IU, and ask for the */
/*           IL-th through IU-th eigenvalues. */

	    ntest = 19;
	    if (n <= 1) {
		il = 1;
		iu = n;
	    } else {
		il = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		iu = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		if (iu < il) {
		    itemp = iu;
		    iu = il;
		    il = itemp;
		}
	    }

	    dstebz_("I", "E", &n, &vl, &vu, &il, &iu, &abstol, &sd[1], &se[1], 
		     &m2, &nsplit, &wa2[1], &iwork[1], &iwork[n + 1], &work[1]
, &iwork[(n << 1) + 1], &iinfo);
	    if (iinfo != 0) {
		io___70.ciunit = *nounit;
		s_wsfe(&io___70);
		do_fio(&c__1, "DSTEBZ(I)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[19] = ulpinv;
		    goto L280;
		}
	    }

/*           Determine the values VL and VU of the IL-th and IU-th */
/*           eigenvalues and ask for all eigenvalues in this range. */

	    if (n > 0) {
		if (il != 1) {
/* Computing MAX */
		    d__1 = (wa1[il] - wa1[il - 1]) * .5, d__2 = ulp * anorm, 
			    d__1 = max(d__1,d__2), d__2 = rtunfl * 2.;
		    vl = wa1[il] - max(d__1,d__2);
		} else {
/* Computing MAX */
		    d__1 = (wa1[n] - wa1[1]) * .5, d__2 = ulp * anorm, d__1 = 
			    max(d__1,d__2), d__2 = rtunfl * 2.;
		    vl = wa1[1] - max(d__1,d__2);
		}
		if (iu != n) {
/* Computing MAX */
		    d__1 = (wa1[iu + 1] - wa1[iu]) * .5, d__2 = ulp * anorm, 
			    d__1 = max(d__1,d__2), d__2 = rtunfl * 2.;
		    vu = wa1[iu] + max(d__1,d__2);
		} else {
/* Computing MAX */
		    d__1 = (wa1[n] - wa1[1]) * .5, d__2 = ulp * anorm, d__1 = 
			    max(d__1,d__2), d__2 = rtunfl * 2.;
		    vu = wa1[n] + max(d__1,d__2);
		}
	    } else {
		vl = 0.;
		vu = 1.;
	    }

	    dstebz_("V", "E", &n, &vl, &vu, &il, &iu, &abstol, &sd[1], &se[1], 
		     &m3, &nsplit, &wa3[1], &iwork[1], &iwork[n + 1], &work[1]
, &iwork[(n << 1) + 1], &iinfo);
	    if (iinfo != 0) {
		io___72.ciunit = *nounit;
		s_wsfe(&io___72);
		do_fio(&c__1, "DSTEBZ(V)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[19] = ulpinv;
		    goto L280;
		}
	    }

	    if (m3 == 0 && n != 0) {
		result[19] = ulpinv;
		goto L280;
	    }

/*           Do test 19 */

	    temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &ulp, &
		    unfl);
	    temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &ulp, &
		    unfl);
	    if (n > 0) {
/* Computing MAX */
		d__2 = (d__1 = wa1[n], abs(d__1)), d__3 = abs(wa1[1]);
		temp3 = max(d__2,d__3);
	    } else {
		temp3 = 0.;
	    }

/* Computing MAX */
	    d__1 = unfl, d__2 = temp3 * ulp;
	    result[19] = (temp1 + temp2) / max(d__1,d__2);

/*           Call DSTEIN to compute eigenvectors corresponding to */
/*           eigenvalues in WA1.  (First call DSTEBZ again, to make sure */
/*           it returns these eigenvalues in the correct order.) */

	    ntest = 21;
	    dstebz_("A", "B", &n, &vl, &vu, &il, &iu, &abstol, &sd[1], &se[1], 
		     &m, &nsplit, &wa1[1], &iwork[1], &iwork[n + 1], &work[1], 
		     &iwork[(n << 1) + 1], &iinfo);
	    if (iinfo != 0) {
		io___73.ciunit = *nounit;
		s_wsfe(&io___73);
		do_fio(&c__1, "DSTEBZ(A,B)", (ftnlen)11);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[20] = ulpinv;
		    result[21] = ulpinv;
		    goto L280;
		}
	    }

	    dstein_(&n, &sd[1], &se[1], &m, &wa1[1], &iwork[1], &iwork[n + 1], 
		     &z__[z_offset], ldu, &work[1], &iwork[(n << 1) + 1], &
		    iwork[n * 3 + 1], &iinfo);
	    if (iinfo != 0) {
		io___74.ciunit = *nounit;
		s_wsfe(&io___74);
		do_fio(&c__1, "DSTEIN", (ftnlen)6);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[20] = ulpinv;
		    result[21] = ulpinv;
		    goto L280;
		}
	    }

/*           Do tests 20 and 21 */

	    dstt21_(&n, &c__0, &sd[1], &se[1], &wa1[1], dumma, &z__[z_offset], 
		     ldu, &work[1], &result[20]);

/*           Call DSTEDC(I) to compute D1 and Z, do tests. */

/*           Compute D1 and Z */

	    dcopy_(&n, &sd[1], &c__1, &d1[1], &c__1);
	    if (n > 0) {
		i__3 = n - 1;
		dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
	    }
	    dlaset_("Full", &n, &n, &c_b25, &c_b39, &z__[z_offset], ldu);

	    ntest = 22;
	    i__3 = lwedc - n;
	    dstedc_("I", &n, &d1[1], &work[1], &z__[z_offset], ldu, &work[n + 
		    1], &i__3, &iwork[1], &liwedc, &iinfo);
	    if (iinfo != 0) {
		io___75.ciunit = *nounit;
		s_wsfe(&io___75);
		do_fio(&c__1, "DSTEDC(I)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[22] = ulpinv;
		    goto L280;
		}
	    }

/*           Do Tests 22 and 23 */

	    dstt21_(&n, &c__0, &sd[1], &se[1], &d1[1], dumma, &z__[z_offset], 
		    ldu, &work[1], &result[22]);

/*           Call DSTEDC(V) to compute D1 and Z, do tests. */

/*           Compute D1 and Z */

	    dcopy_(&n, &sd[1], &c__1, &d1[1], &c__1);
	    if (n > 0) {
		i__3 = n - 1;
		dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
	    }
	    dlaset_("Full", &n, &n, &c_b25, &c_b39, &z__[z_offset], ldu);

	    ntest = 24;
	    i__3 = lwedc - n;
	    dstedc_("V", &n, &d1[1], &work[1], &z__[z_offset], ldu, &work[n + 
		    1], &i__3, &iwork[1], &liwedc, &iinfo);
	    if (iinfo != 0) {
		io___76.ciunit = *nounit;
		s_wsfe(&io___76);
		do_fio(&c__1, "DSTEDC(V)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[24] = ulpinv;
		    goto L280;
		}
	    }

/*           Do Tests 24 and 25 */

	    dstt21_(&n, &c__0, &sd[1], &se[1], &d1[1], dumma, &z__[z_offset], 
		    ldu, &work[1], &result[24]);

/*           Call DSTEDC(N) to compute D2, do tests. */

/*           Compute D2 */

	    dcopy_(&n, &sd[1], &c__1, &d2[1], &c__1);
	    if (n > 0) {
		i__3 = n - 1;
		dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
	    }
	    dlaset_("Full", &n, &n, &c_b25, &c_b39, &z__[z_offset], ldu);

	    ntest = 26;
	    i__3 = lwedc - n;
	    dstedc_("N", &n, &d2[1], &work[1], &z__[z_offset], ldu, &work[n + 
		    1], &i__3, &iwork[1], &liwedc, &iinfo);
	    if (iinfo != 0) {
		io___77.ciunit = *nounit;
		s_wsfe(&io___77);
		do_fio(&c__1, "DSTEDC(N)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    return 0;
		} else {
		    result[26] = ulpinv;
		    goto L280;
		}
	    }

/*           Do Test 26 */

	    temp1 = 0.;
	    temp2 = 0.;

	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = max(
			d__3,d__4), d__4 = (d__2 = d2[j], abs(d__2));
		temp1 = max(d__3,d__4);
/* Computing MAX */
		d__2 = temp2, d__3 = (d__1 = d1[j] - d2[j], abs(d__1));
		temp2 = max(d__2,d__3);
/* L210: */
	    }

/* Computing MAX */
	    d__1 = unfl, d__2 = ulp * max(temp1,temp2);
	    result[26] = temp2 / max(d__1,d__2);

/*           Only test DSTEMR if IEEE compliant */

	    if (ilaenv_(&c__10, "DSTEMR", "VA", &c__1, &c__0, &c__0, &c__0) == 1 && ilaenv_(&c__11, "DSTEMR", 
		    "VA", &c__1, &c__0, &c__0, &c__0) ==
		     1) {

/*           Call DSTEMR, do test 27 (relative eigenvalue accuracy) */

/*              If S is positive definite and diagonally dominant, */
/*              ask for all eigenvalues with high relative accuracy. */

		vl = 0.;
		vu = 0.;
		il = 0;
		iu = 0;
		if (FALSE_) {
		    ntest = 27;
		    abstol = unfl + unfl;
		    i__3 = *lwork - (n << 1);
		    dstemr_("V", "A", &n, &sd[1], &se[1], &vl, &vu, &il, &iu, 
			    &m, &wr[1], &z__[z_offset], ldu, &n, &iwork[1], &
			    tryrac, &work[1], lwork, &iwork[(n << 1) + 1], &
			    i__3, &iinfo);
		    if (iinfo != 0) {
			io___78.ciunit = *nounit;
			s_wsfe(&io___78);
			do_fio(&c__1, "DSTEMR(V,A,rel)", (ftnlen)15);
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
			    result[27] = ulpinv;
			    goto L270;
			}
		    }

/*              Do test 27 */

		    temp2 = (n * 2. - 1.) * 2. * ulp * 3. / .0625;

		    temp1 = 0.;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			d__3 = temp1, d__4 = (d__2 = d4[j] - wr[n - j + 1], 
				abs(d__2)) / (abstol + (d__1 = d4[j], abs(
				d__1)));
			temp1 = max(d__3,d__4);
/* L220: */
		    }

		    result[27] = temp1 / temp2;

		    il = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		    iu = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		    if (iu < il) {
			itemp = iu;
			iu = il;
			il = itemp;
		    }

		    if (FALSE_) {
			ntest = 28;
			abstol = unfl + unfl;
			i__3 = *lwork - (n << 1);
			dstemr_("V", "I", &n, &sd[1], &se[1], &vl, &vu, &il, &
				iu, &m, &wr[1], &z__[z_offset], ldu, &n, &
				iwork[1], &tryrac, &work[1], lwork, &iwork[(n 
				<< 1) + 1], &i__3, &iinfo);

			if (iinfo != 0) {
			    io___79.ciunit = *nounit;
			    s_wsfe(&io___79);
			    do_fio(&c__1, "DSTEMR(V,I,rel)", (ftnlen)15);
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
				result[28] = ulpinv;
				goto L270;
			    }
			}


/*                 Do test 28 */

			temp2 = (n * 2. - 1.) * 2. * ulp * 3. / .0625;

			temp1 = 0.;
			i__3 = iu;
			for (j = il; j <= i__3; ++j) {
/* Computing MAX */
			    d__3 = temp1, d__4 = (d__2 = wr[j - il + 1] - d4[
				    n - j + 1], abs(d__2)) / (abstol + (d__1 =
				     wr[j - il + 1], abs(d__1)));
			    temp1 = max(d__3,d__4);
/* L230: */
			}

			result[28] = temp1 / temp2;
		    } else {
			result[28] = 0.;
		    }
		} else {
		    result[27] = 0.;
		    result[28] = 0.;
		}

/*           Call DSTEMR(V,I) to compute D1 and Z, do tests. */

/*           Compute D1 and Z */

		dcopy_(&n, &sd[1], &c__1, &d5[1], &c__1);
		if (n > 0) {
		    i__3 = n - 1;
		    dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		}
		dlaset_("Full", &n, &n, &c_b25, &c_b39, &z__[z_offset], ldu);

		if (FALSE_) {
		    ntest = 29;
		    il = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		    iu = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		    if (iu < il) {
			itemp = iu;
			iu = il;
			il = itemp;
		    }
		    i__3 = *lwork - n;
		    i__4 = *liwork - (n << 1);
		    dstemr_("V", "I", &n, &d5[1], &work[1], &vl, &vu, &il, &
			    iu, &m, &d1[1], &z__[z_offset], ldu, &n, &iwork[1]
, &tryrac, &work[n + 1], &i__3, &iwork[(n << 1) + 
			    1], &i__4, &iinfo);
		    if (iinfo != 0) {
			io___80.ciunit = *nounit;
			s_wsfe(&io___80);
			do_fio(&c__1, "DSTEMR(V,I)", (ftnlen)11);
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
			    result[29] = ulpinv;
			    goto L280;
			}
		    }

/*           Do Tests 29 and 30 */

		    dstt22_(&n, &m, &c__0, &sd[1], &se[1], &d1[1], dumma, &
			    z__[z_offset], ldu, &work[1], &m, &result[29]);

/*           Call DSTEMR to compute D2, do tests. */

/*           Compute D2 */

		    dcopy_(&n, &sd[1], &c__1, &d5[1], &c__1);
		    if (n > 0) {
			i__3 = n - 1;
			dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		    }

		    ntest = 31;
		    i__3 = *lwork - n;
		    i__4 = *liwork - (n << 1);
		    dstemr_("N", "I", &n, &d5[1], &work[1], &vl, &vu, &il, &
			    iu, &m, &d2[1], &z__[z_offset], ldu, &n, &iwork[1]
, &tryrac, &work[n + 1], &i__3, &iwork[(n << 1) + 
			    1], &i__4, &iinfo);
		    if (iinfo != 0) {
			io___81.ciunit = *nounit;
			s_wsfe(&io___81);
			do_fio(&c__1, "DSTEMR(N,I)", (ftnlen)11);
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
			    result[31] = ulpinv;
			    goto L280;
			}
		    }

/*           Do Test 31 */

		    temp1 = 0.;
		    temp2 = 0.;

		    i__3 = iu - il + 1;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 =
				 max(d__3,d__4), d__4 = (d__2 = d2[j], abs(
				d__2));
			temp1 = max(d__3,d__4);
/* Computing MAX */
			d__2 = temp2, d__3 = (d__1 = d1[j] - d2[j], abs(d__1))
				;
			temp2 = max(d__2,d__3);
/* L240: */
		    }

/* Computing MAX */
		    d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		    result[31] = temp2 / max(d__1,d__2);


/*           Call DSTEMR(V,V) to compute D1 and Z, do tests. */

/*           Compute D1 and Z */

		    dcopy_(&n, &sd[1], &c__1, &d5[1], &c__1);
		    if (n > 0) {
			i__3 = n - 1;
			dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		    }
		    dlaset_("Full", &n, &n, &c_b25, &c_b39, &z__[z_offset], 
			    ldu);

		    ntest = 32;

		    if (n > 0) {
			if (il != 1) {
/* Computing MAX */
			    d__1 = (d2[il] - d2[il - 1]) * .5, d__2 = ulp * 
				    anorm, d__1 = max(d__1,d__2), d__2 = 
				    rtunfl * 2.;
			    vl = d2[il] - max(d__1,d__2);
			} else {
/* Computing MAX */
			    d__1 = (d2[n] - d2[1]) * .5, d__2 = ulp * anorm, 
				    d__1 = max(d__1,d__2), d__2 = rtunfl * 2.;
			    vl = d2[1] - max(d__1,d__2);
			}
			if (iu != n) {
/* Computing MAX */
			    d__1 = (d2[iu + 1] - d2[iu]) * .5, d__2 = ulp * 
				    anorm, d__1 = max(d__1,d__2), d__2 = 
				    rtunfl * 2.;
			    vu = d2[iu] + max(d__1,d__2);
			} else {
/* Computing MAX */
			    d__1 = (d2[n] - d2[1]) * .5, d__2 = ulp * anorm, 
				    d__1 = max(d__1,d__2), d__2 = rtunfl * 2.;
			    vu = d2[n] + max(d__1,d__2);
			}
		    } else {
			vl = 0.;
			vu = 1.;
		    }

		    i__3 = *lwork - n;
		    i__4 = *liwork - (n << 1);
		    dstemr_("V", "V", &n, &d5[1], &work[1], &vl, &vu, &il, &
			    iu, &m, &d1[1], &z__[z_offset], ldu, &n, &iwork[1]
, &tryrac, &work[n + 1], &i__3, &iwork[(n << 1) + 
			    1], &i__4, &iinfo);
		    if (iinfo != 0) {
			io___82.ciunit = *nounit;
			s_wsfe(&io___82);
			do_fio(&c__1, "DSTEMR(V,V)", (ftnlen)11);
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
			    result[32] = ulpinv;
			    goto L280;
			}
		    }

/*           Do Tests 32 and 33 */

		    dstt22_(&n, &m, &c__0, &sd[1], &se[1], &d1[1], dumma, &
			    z__[z_offset], ldu, &work[1], &m, &result[32]);

/*           Call DSTEMR to compute D2, do tests. */

/*           Compute D2 */

		    dcopy_(&n, &sd[1], &c__1, &d5[1], &c__1);
		    if (n > 0) {
			i__3 = n - 1;
			dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		    }

		    ntest = 34;
		    i__3 = *lwork - n;
		    i__4 = *liwork - (n << 1);
		    dstemr_("N", "V", &n, &d5[1], &work[1], &vl, &vu, &il, &
			    iu, &m, &d2[1], &z__[z_offset], ldu, &n, &iwork[1]
, &tryrac, &work[n + 1], &i__3, &iwork[(n << 1) + 
			    1], &i__4, &iinfo);
		    if (iinfo != 0) {
			io___83.ciunit = *nounit;
			s_wsfe(&io___83);
			do_fio(&c__1, "DSTEMR(N,V)", (ftnlen)11);
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
			    result[34] = ulpinv;
			    goto L280;
			}
		    }

/*           Do Test 34 */

		    temp1 = 0.;
		    temp2 = 0.;

		    i__3 = iu - il + 1;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 =
				 max(d__3,d__4), d__4 = (d__2 = d2[j], abs(
				d__2));
			temp1 = max(d__3,d__4);
/* Computing MAX */
			d__2 = temp2, d__3 = (d__1 = d1[j] - d2[j], abs(d__1))
				;
			temp2 = max(d__2,d__3);
/* L250: */
		    }

/* Computing MAX */
		    d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		    result[34] = temp2 / max(d__1,d__2);
		} else {
		    result[29] = 0.;
		    result[30] = 0.;
		    result[31] = 0.;
		    result[32] = 0.;
		    result[33] = 0.;
		    result[34] = 0.;
		}


/*           Call DSTEMR(V,A) to compute D1 and Z, do tests. */

/*           Compute D1 and Z */

		dcopy_(&n, &sd[1], &c__1, &d5[1], &c__1);
		if (n > 0) {
		    i__3 = n - 1;
		    dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		}

		ntest = 35;

		i__3 = *lwork - n;
		i__4 = *liwork - (n << 1);
		dstemr_("V", "A", &n, &d5[1], &work[1], &vl, &vu, &il, &iu, &
			m, &d1[1], &z__[z_offset], ldu, &n, &iwork[1], &
			tryrac, &work[n + 1], &i__3, &iwork[(n << 1) + 1], &
			i__4, &iinfo);
		if (iinfo != 0) {
		    io___84.ciunit = *nounit;
		    s_wsfe(&io___84);
		    do_fio(&c__1, "DSTEMR(V,A)", (ftnlen)11);
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
			result[35] = ulpinv;
			goto L280;
		    }
		}

/*           Do Tests 35 and 36 */

		dstt22_(&n, &m, &c__0, &sd[1], &se[1], &d1[1], dumma, &z__[
			z_offset], ldu, &work[1], &m, &result[35]);

/*           Call DSTEMR to compute D2, do tests. */

/*           Compute D2 */

		dcopy_(&n, &sd[1], &c__1, &d5[1], &c__1);
		if (n > 0) {
		    i__3 = n - 1;
		    dcopy_(&i__3, &se[1], &c__1, &work[1], &c__1);
		}

		ntest = 37;
		i__3 = *lwork - n;
		i__4 = *liwork - (n << 1);
		dstemr_("N", "A", &n, &d5[1], &work[1], &vl, &vu, &il, &iu, &
			m, &d2[1], &z__[z_offset], ldu, &n, &iwork[1], &
			tryrac, &work[n + 1], &i__3, &iwork[(n << 1) + 1], &
			i__4, &iinfo);
		if (iinfo != 0) {
		    io___85.ciunit = *nounit;
		    s_wsfe(&io___85);
		    do_fio(&c__1, "DSTEMR(N,A)", (ftnlen)11);
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
			result[37] = ulpinv;
			goto L280;
		    }
		}

/*           Do Test 34 */

		temp1 = 0.;
		temp2 = 0.;

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = d1[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = d2[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = d1[j] - d2[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L260: */
		}

/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[37] = temp2 / max(d__1,d__2);
	    }
L270:
L280:
	    ntestt += ntest;

/*           End of Loop -- Check for RESULT(j) > THRESH */


/*           Print out tests which fail. */

	    i__3 = ntest;
	    for (jr = 1; jr <= i__3; ++jr) {
		if (result[jr] >= *thresh) {

/*                 If this is the first test to fail, */
/*                 print a header to the data file. */

		    if (nerrs == 0) {
			io___86.ciunit = *nounit;
			s_wsfe(&io___86);
			do_fio(&c__1, "DST", (ftnlen)3);
			e_wsfe();
			io___87.ciunit = *nounit;
			s_wsfe(&io___87);
			e_wsfe();
			io___88.ciunit = *nounit;
			s_wsfe(&io___88);
			e_wsfe();
			io___89.ciunit = *nounit;
			s_wsfe(&io___89);
			do_fio(&c__1, "Symmetric", (ftnlen)9);
			e_wsfe();
			io___90.ciunit = *nounit;
			s_wsfe(&io___90);
			e_wsfe();

/*                    Tests performed */

			io___91.ciunit = *nounit;
			s_wsfe(&io___91);
			e_wsfe();
		    }
		    ++nerrs;
		    io___92.ciunit = *nounit;
		    s_wsfe(&io___92);
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jr, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&result[jr], (ftnlen)sizeof(
			    doublereal));
		    e_wsfe();
		}
/* L290: */
	    }
L300:
	    ;
	}
/* L310: */
    }

/*     Summary */

    dlasum_("DST", nounit, &nerrs, &ntestt);
    return 0;




/* L9993: */
/* L9992: */
/* L9991: */
/* L9989: */

/*     End of DCHKST */

} /* dchkst_ */
