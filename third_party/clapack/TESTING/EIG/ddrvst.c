#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static integer c__2 = 2;
static doublereal c_b20 = 0.;
static integer c__0 = 0;
static integer c__6 = 6;
static doublereal c_b34 = 1.;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__3 = 3;

/* Subroutine */ int ddrvst_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, integer *nounit, 
	doublereal *a, integer *lda, doublereal *d1, doublereal *d2, 
	doublereal *d3, doublereal *d4, doublereal *eveigs, doublereal *wa1, 
	doublereal *wa2, doublereal *wa3, doublereal *u, integer *ldu, 
	doublereal *v, doublereal *tau, doublereal *z__, doublereal *work, 
	integer *lwork, integer *iwork, integer *liwork, doublereal *result, 
	integer *info)
{
    /* Initialized data */

    static integer ktype[18] = { 1,2,4,4,4,4,4,5,5,5,5,5,8,8,8,9,9,9 };
    static integer kmagn[18] = { 1,1,1,1,1,2,3,1,1,1,2,3,1,2,3,1,2,3 };
    static integer kmode[18] = { 0,0,4,3,1,4,4,4,3,1,4,4,0,0,0,4,4,4 };

    /* Format strings */
    static char fmt_9999[] = "(\002 DDRVST: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";

    /* System generated locals */
    address a__1[3];
    integer a_dim1, a_offset, u_dim1, u_offset, v_dim1, v_offset, z_dim1, 
	    z_offset, i__1, i__2, i__3, i__4, i__5, i__6[3], i__7;
    doublereal d__1, d__2, d__3, d__4;
    char ch__1[10], ch__2[13], ch__3[11];

    /* Builtin functions */
    double sqrt(doublereal), log(doublereal);
    integer pow_ii(integer *, integer *), s_wsfe(cilist *), do_fio(integer *, 
	    char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen), s_cat(char *,
	     char **, integer *, integer *, ftnlen);

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
    integer imode, lwedc;
    extern /* Subroutine */ int dsbev_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *);
    integer iinfo;
    doublereal aninv, anorm;
    integer itemp;
    extern /* Subroutine */ int dspev_(char *, char *, integer *, doublereal *
, doublereal *, doublereal *, integer *, doublereal *, integer *);
    integer nmats;
    extern /* Subroutine */ int dstt21_(integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, doublereal *);
    integer jsize;
    extern /* Subroutine */ int dstev_(char *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), dstt22_(integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *, 
	     doublereal *, integer *, doublereal *), dsyt21_(integer *, char *
, integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, doublereal *);
    integer iuplo, nerrs, itype, jtype, ntest;
    extern /* Subroutine */ int dsyev_(char *, char *, integer *, doublereal *
, integer *, doublereal *, doublereal *, integer *, integer *), dsyt22_(integer *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *);
    integer iseed2[4], iseed3[4];
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlarnd_(integer *, integer *);
    integer liwedc;
    extern /* Subroutine */ int dsbevd_(char *, char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *, integer *);
    integer idumma[1];
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ioldsd[4];
    extern /* Subroutine */ int dlafts_(char *, integer *, integer *, integer 
	    *, integer *, doublereal *, integer *, doublereal *, integer *, 
	    integer *), dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    xerbla_(char *, integer *), alasvm_(char *, integer *, 
	    integer *, integer *, integer *);
    doublereal abstol;
    extern /* Subroutine */ int dlatmr_(integer *, integer *, char *, integer 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    char *, char *, doublereal *, integer *, doublereal *, doublereal 
	    *, integer *, doublereal *, char *, integer *, integer *, integer 
	    *, doublereal *, doublereal *, char *, doublereal *, integer *, 
	    integer *, integer *), dlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublereal *, integer *, doublereal *, integer 
	    *), dspevd_(char *, char *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *, integer *, integer *, integer *), 
	    dstevd_(char *, integer *, doublereal *, doublereal *, doublereal 
	    *, integer *, doublereal *, integer *, integer *, integer *, 
	    integer *), dsbevx_(char *, char *, char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, integer *, integer *), dsyevd_(
	    char *, char *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, integer *, integer *), dstevr_(char *, char *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, integer *, integer *, integer 
	    *), dspevx_(char *, char *, char *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *);
    doublereal rtunfl, rtovfl, ulpinv;
    extern /* Subroutine */ int dstevx_(char *, char *, integer *, doublereal 
	    *, doublereal *, doublereal *, doublereal *, integer *, integer *, 
	     doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *);
    integer mtypes, ntestt;
    extern /* Subroutine */ int dsyevr_(char *, char *, char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, integer *, doublereal *, integer *, integer *, integer 
	    *, integer *), dsyevx_(char *, char *, 
	    char *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    integer *, integer *, integer *);

    /* Fortran I/O blocks */
    static cilist io___43 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___84 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___85 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___86 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___87 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___88 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___90 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___91 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___92 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___93 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___94 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___95 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___96 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___97 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___98 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___99 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___100 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___101 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___102 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___103 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___104 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___105 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___106 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___107 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___108 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___109 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*       DDRVST  checks the symmetric eigenvalue problem drivers. */

/*               DSTEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric tridiagonal matrix. */

/*               DSTEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric tridiagonal matrix. */

/*               DSTEVR computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric tridiagonal matrix */
/*               using the Relatively Robust Representation where it can. */

/*               DSYEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix. */

/*               DSYEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix. */

/*               DSYEVR computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix */
/*               using the Relatively Robust Representation where it can. */

/*               DSPEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix in packed */
/*               storage. */

/*               DSPEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix in packed */
/*               storage. */

/*               DSBEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric band matrix. */

/*               DSBEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric band matrix. */

/*               DSYEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix using */
/*               a divide and conquer algorithm. */

/*               DSPEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix in packed */
/*               storage, using a divide and conquer algorithm. */

/*               DSBEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric band matrix, */
/*               using a divide and conquer algorithm. */

/*       When DDRVST is called, a number of matrix "sizes" ("n's") and a */
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

/*       (3)  A diagonal matrix with evenly spaced eigenvalues */
/*            1, ..., ULP  and random signs. */
/*            (ULP = (first number larger than 1) - 1 ) */
/*       (4)  A diagonal matrix with geometrically spaced eigenvalues */
/*            1, ..., ULP  and random signs. */
/*       (5)  A diagonal matrix with "clustered" eigenvalues */
/*            1, ULP, ..., ULP and random signs. */

/*       (6)  Same as (4), but multiplied by SQRT( overflow threshold ) */
/*       (7)  Same as (4), but multiplied by SQRT( underflow threshold ) */

/*       (8)  A matrix of the form  U' D U, where U is orthogonal and */
/*            D has evenly spaced entries 1, ..., ULP with random signs */
/*            on the diagonal. */

/*       (9)  A matrix of the form  U' D U, where U is orthogonal and */
/*            D has geometrically spaced entries 1, ..., ULP with random */
/*            signs on the diagonal. */

/*       (10) A matrix of the form  U' D U, where U is orthogonal and */
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
/*          DDRVST does nothing.  It must be at least zero. */
/*          Not modified. */

/*  NN      INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */
/*          Not modified. */

/*  NTYPES  INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, DDRVST */
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
/*          next call to DDRVST to continue the same random number */
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

/*  A       DOUBLE PRECISION array, dimension (LDA , max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually */
/*          used. */
/*          Modified. */

/*  LDA     INTEGER */
/*          The leading dimension of A.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  D1      DOUBLE PRECISION array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by DSTEQR simlutaneously */
/*          with Z.  On exit, the eigenvalues in D1 correspond with the */
/*          matrix in A. */
/*          Modified. */

/*  D2      DOUBLE PRECISION array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by DSTEQR if Z is not */
/*          computed.  On exit, the eigenvalues in D2 correspond with */
/*          the matrix in A. */
/*          Modified. */

/*  D3      DOUBLE PRECISION array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by DSTERF.  On exit, the */
/*          eigenvalues in D3 correspond with the matrix in A. */
/*          Modified. */

/*  D4      DOUBLE PRECISION array, dimension */

/*  EVEIGS  DOUBLE PRECISION array, dimension (max(NN)) */
/*          The eigenvalues as computed by DSTEV('N', ... ) */
/*          (I reserve the right to change this to the output of */
/*          whichever algorithm computes the most accurate eigenvalues). */

/*  WA1     DOUBLE PRECISION array, dimension */

/*  WA2     DOUBLE PRECISION array, dimension */

/*  WA3     DOUBLE PRECISION array, dimension */

/*  U       DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The orthogonal matrix computed by DSYTRD + DORGTR. */
/*          Modified. */

/*  LDU     INTEGER */
/*          The leading dimension of U, Z, and V.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  V       DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The Housholder vectors computed by DSYTRD in reducing A to */
/*          tridiagonal form. */
/*          Modified. */

/*  TAU     DOUBLE PRECISION array, dimension (max(NN)) */
/*          The Householder factors computed by DSYTRD in reducing A */
/*          to tridiagonal form. */
/*          Modified. */

/*  Z       DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The orthogonal matrix of eigenvectors computed by DSTEQR, */
/*          DPTEQR, and DSTEIN. */
/*          Modified. */

/*  WORK    DOUBLE PRECISION array, dimension (LWORK) */
/*          Workspace. */
/*          Modified. */

/*  LWORK   INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          1 + 4 * Nmax + 2 * Nmax * lg Nmax + 4 * Nmax**2 */
/*          where Nmax = max( NN(j), 2 ) and lg = log base 2. */
/*          Not modified. */

/*  IWORK   INTEGER array, */
/*             dimension (6 + 6*Nmax + 5 * Nmax * lg Nmax ) */
/*          where Nmax = max( NN(j), 2 ) and lg = log base 2. */
/*          Workspace. */
/*          Modified. */

/*  RESULT  DOUBLE PRECISION array, dimension (105) */
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
/*          If  DLATMR, DLATMS, DSYTRD, DORGTR, DSTEQR, DSTERF, */
/*              or DORMTR returns an error code, the */
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

/*     The tests performed are:                 Routine tested */
/*    1= | A - U S U' | / ( |A| n ulp )         DSTEV('V', ... ) */
/*    2= | I - U U' | / ( n ulp )               DSTEV('V', ... ) */
/*    3= |D(with Z) - D(w/o Z)| / (|D| ulp)     DSTEV('N', ... ) */
/*    4= | A - U S U' | / ( |A| n ulp )         DSTEVX('V','A', ... ) */
/*    5= | I - U U' | / ( n ulp )               DSTEVX('V','A', ... ) */
/*    6= |D(with Z) - EVEIGS| / (|D| ulp)       DSTEVX('N','A', ... ) */
/*    7= | A - U S U' | / ( |A| n ulp )         DSTEVR('V','A', ... ) */
/*    8= | I - U U' | / ( n ulp )               DSTEVR('V','A', ... ) */
/*    9= |D(with Z) - EVEIGS| / (|D| ulp)       DSTEVR('N','A', ... ) */
/*    10= | A - U S U' | / ( |A| n ulp )        DSTEVX('V','I', ... ) */
/*    11= | I - U U' | / ( n ulp )              DSTEVX('V','I', ... ) */
/*    12= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSTEVX('N','I', ... ) */
/*    13= | A - U S U' | / ( |A| n ulp )        DSTEVX('V','V', ... ) */
/*    14= | I - U U' | / ( n ulp )              DSTEVX('V','V', ... ) */
/*    15= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSTEVX('N','V', ... ) */
/*    16= | A - U S U' | / ( |A| n ulp )        DSTEVD('V', ... ) */
/*    17= | I - U U' | / ( n ulp )              DSTEVD('V', ... ) */
/*    18= |D(with Z) - EVEIGS| / (|D| ulp)      DSTEVD('N', ... ) */
/*    19= | A - U S U' | / ( |A| n ulp )        DSTEVR('V','I', ... ) */
/*    20= | I - U U' | / ( n ulp )              DSTEVR('V','I', ... ) */
/*    21= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSTEVR('N','I', ... ) */
/*    22= | A - U S U' | / ( |A| n ulp )        DSTEVR('V','V', ... ) */
/*    23= | I - U U' | / ( n ulp )              DSTEVR('V','V', ... ) */
/*    24= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSTEVR('N','V', ... ) */

/*    25= | A - U S U' | / ( |A| n ulp )        DSYEV('L','V', ... ) */
/*    26= | I - U U' | / ( n ulp )              DSYEV('L','V', ... ) */
/*    27= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEV('L','N', ... ) */
/*    28= | A - U S U' | / ( |A| n ulp )        DSYEVX('L','V','A', ... ) */
/*    29= | I - U U' | / ( n ulp )              DSYEVX('L','V','A', ... ) */
/*    30= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEVX('L','N','A', ... ) */
/*    31= | A - U S U' | / ( |A| n ulp )        DSYEVX('L','V','I', ... ) */
/*    32= | I - U U' | / ( n ulp )              DSYEVX('L','V','I', ... ) */
/*    33= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEVX('L','N','I', ... ) */
/*    34= | A - U S U' | / ( |A| n ulp )        DSYEVX('L','V','V', ... ) */
/*    35= | I - U U' | / ( n ulp )              DSYEVX('L','V','V', ... ) */
/*    36= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEVX('L','N','V', ... ) */
/*    37= | A - U S U' | / ( |A| n ulp )        DSPEV('L','V', ... ) */
/*    38= | I - U U' | / ( n ulp )              DSPEV('L','V', ... ) */
/*    39= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEV('L','N', ... ) */
/*    40= | A - U S U' | / ( |A| n ulp )        DSPEVX('L','V','A', ... ) */
/*    41= | I - U U' | / ( n ulp )              DSPEVX('L','V','A', ... ) */
/*    42= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEVX('L','N','A', ... ) */
/*    43= | A - U S U' | / ( |A| n ulp )        DSPEVX('L','V','I', ... ) */
/*    44= | I - U U' | / ( n ulp )              DSPEVX('L','V','I', ... ) */
/*    45= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEVX('L','N','I', ... ) */
/*    46= | A - U S U' | / ( |A| n ulp )        DSPEVX('L','V','V', ... ) */
/*    47= | I - U U' | / ( n ulp )              DSPEVX('L','V','V', ... ) */
/*    48= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEVX('L','N','V', ... ) */
/*    49= | A - U S U' | / ( |A| n ulp )        DSBEV('L','V', ... ) */
/*    50= | I - U U' | / ( n ulp )              DSBEV('L','V', ... ) */
/*    51= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEV('L','N', ... ) */
/*    52= | A - U S U' | / ( |A| n ulp )        DSBEVX('L','V','A', ... ) */
/*    53= | I - U U' | / ( n ulp )              DSBEVX('L','V','A', ... ) */
/*    54= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEVX('L','N','A', ... ) */
/*    55= | A - U S U' | / ( |A| n ulp )        DSBEVX('L','V','I', ... ) */
/*    56= | I - U U' | / ( n ulp )              DSBEVX('L','V','I', ... ) */
/*    57= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEVX('L','N','I', ... ) */
/*    58= | A - U S U' | / ( |A| n ulp )        DSBEVX('L','V','V', ... ) */
/*    59= | I - U U' | / ( n ulp )              DSBEVX('L','V','V', ... ) */
/*    60= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEVX('L','N','V', ... ) */
/*    61= | A - U S U' | / ( |A| n ulp )        DSYEVD('L','V', ... ) */
/*    62= | I - U U' | / ( n ulp )              DSYEVD('L','V', ... ) */
/*    63= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEVD('L','N', ... ) */
/*    64= | A - U S U' | / ( |A| n ulp )        DSPEVD('L','V', ... ) */
/*    65= | I - U U' | / ( n ulp )              DSPEVD('L','V', ... ) */
/*    66= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEVD('L','N', ... ) */
/*    67= | A - U S U' | / ( |A| n ulp )        DSBEVD('L','V', ... ) */
/*    68= | I - U U' | / ( n ulp )              DSBEVD('L','V', ... ) */
/*    69= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEVD('L','N', ... ) */
/*    70= | A - U S U' | / ( |A| n ulp )        DSYEVR('L','V','A', ... ) */
/*    71= | I - U U' | / ( n ulp )              DSYEVR('L','V','A', ... ) */
/*    72= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEVR('L','N','A', ... ) */
/*    73= | A - U S U' | / ( |A| n ulp )        DSYEVR('L','V','I', ... ) */
/*    74= | I - U U' | / ( n ulp )              DSYEVR('L','V','I', ... ) */
/*    75= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEVR('L','N','I', ... ) */
/*    76= | A - U S U' | / ( |A| n ulp )        DSYEVR('L','V','V', ... ) */
/*    77= | I - U U' | / ( n ulp )              DSYEVR('L','V','V', ... ) */
/*    78= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSYEVR('L','N','V', ... ) */

/*    Tests 25 through 78 are repeated (as tests 79 through 132) */
/*    with UPLO='U' */

/*    To be added in 1999 */

/*    79= | A - U S U' | / ( |A| n ulp )        DSPEVR('L','V','A', ... ) */
/*    80= | I - U U' | / ( n ulp )              DSPEVR('L','V','A', ... ) */
/*    81= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEVR('L','N','A', ... ) */
/*    82= | A - U S U' | / ( |A| n ulp )        DSPEVR('L','V','I', ... ) */
/*    83= | I - U U' | / ( n ulp )              DSPEVR('L','V','I', ... ) */
/*    84= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEVR('L','N','I', ... ) */
/*    85= | A - U S U' | / ( |A| n ulp )        DSPEVR('L','V','V', ... ) */
/*    86= | I - U U' | / ( n ulp )              DSPEVR('L','V','V', ... ) */
/*    87= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSPEVR('L','N','V', ... ) */
/*    88= | A - U S U' | / ( |A| n ulp )        DSBEVR('L','V','A', ... ) */
/*    89= | I - U U' | / ( n ulp )              DSBEVR('L','V','A', ... ) */
/*    90= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEVR('L','N','A', ... ) */
/*    91= | A - U S U' | / ( |A| n ulp )        DSBEVR('L','V','I', ... ) */
/*    92= | I - U U' | / ( n ulp )              DSBEVR('L','V','I', ... ) */
/*    93= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEVR('L','N','I', ... ) */
/*    94= | A - U S U' | / ( |A| n ulp )        DSBEVR('L','V','V', ... ) */
/*    95= | I - U U' | / ( n ulp )              DSBEVR('L','V','V', ... ) */
/*    96= |D(with Z) - D(w/o Z)| / (|D| ulp)    DSBEVR('L','N','V', ... ) */


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
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
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
    --d4;
    --eveigs;
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
    --iwork;
    --result;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Keep ftrnchek happy */

    vl = 0.;
    vu = 0.;

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
	    *info = -21;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DDRVST", &i__1);
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
/* Computing 2nd power */
	    i__2 = n;
	    lwedc = (n << 2) + 1 + (n << 1) * lgn + (i__2 * i__2 << 2);
/*           LIWEDC = 6 + 6*N + 5*N*LGN */
	    liwedc = n * 5 + 3;
	} else {
	    lwedc = 9;
/*           LIWEDC = 12 */
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
		goto L1730;
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
/*           =5         random log   symmetric, w/ eigenvalues */
/*           =6         random       (none) */
/*           =7                      random diagonal */
/*           =8                      random symmetric */
/*           =9                      band symmetric, w/ eigenvalues */

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

	    dlaset_("Full", lda, &n, &c_b20, &c_b20, &a[a_offset], lda);
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
		    a[jcol + jcol * a_dim1] = anorm;
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

		idumma[0] = 1;
		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b34, 
			&c_b34, "T", "N", &work[n + 1], &c__1, &c_b34, &work[(
			n << 1) + 1], &c__1, &c_b34, "N", idumma, &c__0, &
			c__0, &c_b20, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		idumma[0] = 1;
		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b34, 
			&c_b34, "T", "N", &work[n + 1], &c__1, &c_b34, &work[(
			n << 1) + 1], &c__1, &c_b34, "N", idumma, &n, &n, &
			c_b20, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              Symmetric banded, eigenvalues specified */

		ihbw = (integer) ((n - 1) * dlarnd_(&c__1, iseed3));
		dlatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &cond, 
			&anorm, &ihbw, &ihbw, "Z", &u[u_offset], ldu, &work[n 
			+ 1], &iinfo);

/*              Store as dense matrix for most routines. */

		dlaset_("Full", lda, &n, &c_b20, &c_b20, &a[a_offset], lda);
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
			a[i__ + j * a_dim1] = u[irow + j * u_dim1];
/* L90: */
		    }
/* L100: */
		}
	    } else {
		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___43.ciunit = *nounit;
		s_wsfe(&io___43);
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
		il = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		iu = (n - 1) * (integer) dlarnd_(&c__1, iseed2) + 1;
		if (il > iu) {
		    itemp = il;
		    il = iu;
		    iu = itemp;
		}
	    }

/*           3)      If matrix is tridiagonal, call DSTEV and DSTEVX. */

	    if (jtype <= 7) {
		ntest = 1;
		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L120: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L130: */
		}
		s_copy(srnamc_1.srnamt, "DSTEV", (ftnlen)6, (ftnlen)5);
		dstev_("V", &n, &d1[1], &d2[1], &z__[z_offset], ldu, &work[1], 
			 &iinfo);
		if (iinfo != 0) {
		    io___48.ciunit = *nounit;
		    s_wsfe(&io___48);
		    do_fio(&c__1, "DSTEV(V)", (ftnlen)8);
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
			result[1] = ulpinv;
			result[2] = ulpinv;
			result[3] = ulpinv;
			goto L180;
		    }
		}

/*              Do tests 1 and 2. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L140: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L150: */
		}
		dstt21_(&n, &c__0, &d3[1], &d4[1], &d1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[1]);

		ntest = 3;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L160: */
		}
		s_copy(srnamc_1.srnamt, "DSTEV", (ftnlen)6, (ftnlen)5);
		dstev_("N", &n, &d3[1], &d4[1], &z__[z_offset], ldu, &work[1], 
			 &iinfo);
		if (iinfo != 0) {
		    io___49.ciunit = *nounit;
		    s_wsfe(&io___49);
		    do_fio(&c__1, "DSTEV(N)", (ftnlen)8);
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
			result[3] = ulpinv;
			goto L180;
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
/* L170: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[3] = temp2 / max(d__1,d__2);

L180:

		ntest = 4;
		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    eveigs[i__] = d3[i__];
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L190: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L200: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVX", (ftnlen)6, (ftnlen)6);
		dstevx_("V", "A", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m, &wa1[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___53.ciunit = *nounit;
		    s_wsfe(&io___53);
		    do_fio(&c__1, "DSTEVX(V,A)", (ftnlen)11);
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
			result[4] = ulpinv;
			result[5] = ulpinv;
			result[6] = ulpinv;
			goto L250;
		    }
		}
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}

/*              Do tests 4 and 5. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L210: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L220: */
		}
		dstt21_(&n, &c__0, &d3[1], &d4[1], &wa1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[4]);

		ntest = 6;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L230: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVX", (ftnlen)6, (ftnlen)6);
		dstevx_("N", "A", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___56.ciunit = *nounit;
		    s_wsfe(&io___56);
		    do_fio(&c__1, "DSTEVX(N,A)", (ftnlen)11);
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
			result[6] = ulpinv;
			goto L250;
		    }
		}

/*              Do test 6. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = wa2[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = eveigs[j], abs(
			    d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = wa2[j] - eveigs[j], abs(d__1)
			    );
		    temp2 = max(d__2,d__3);
/* L240: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[6] = temp2 / max(d__1,d__2);

L250:

		ntest = 7;
		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L260: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L270: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dstevr_("V", "A", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m, &wa1[1], &z__[z_offset], ldu, &iwork[1], &
			work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___57.ciunit = *nounit;
		    s_wsfe(&io___57);
		    do_fio(&c__1, "DSTEVR(V,A)", (ftnlen)11);
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
			result[7] = ulpinv;
			result[8] = ulpinv;
			goto L320;
		    }
		}
		if (n > 0) {
/* Computing MAX */
		    d__2 = abs(wa1[1]), d__3 = (d__1 = wa1[n], abs(d__1));
		    temp3 = max(d__2,d__3);
		} else {
		    temp3 = 0.;
		}

/*              Do tests 7 and 8. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L280: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L290: */
		}
		dstt21_(&n, &c__0, &d3[1], &d4[1], &wa1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[7]);

		ntest = 9;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L300: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dstevr_("N", "A", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___58.ciunit = *nounit;
		    s_wsfe(&io___58);
		    do_fio(&c__1, "DSTEVR(N,A)", (ftnlen)11);
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
			result[9] = ulpinv;
			goto L320;
		    }
		}

/*              Do test 9. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = wa2[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = eveigs[j], abs(
			    d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = wa2[j] - eveigs[j], abs(d__1)
			    );
		    temp2 = max(d__2,d__3);
/* L310: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[9] = temp2 / max(d__1,d__2);

L320:


		ntest = 10;
		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L330: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L340: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVX", (ftnlen)6, (ftnlen)6);
		dstevx_("V", "I", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___59.ciunit = *nounit;
		    s_wsfe(&io___59);
		    do_fio(&c__1, "DSTEVX(V,I)", (ftnlen)11);
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
			result[10] = ulpinv;
			result[11] = ulpinv;
			result[12] = ulpinv;
			goto L380;
		    }
		}

/*              Do tests 10 and 11. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L350: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L360: */
		}
		i__3 = max(1,m2);
		dstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[10]);


		ntest = 12;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L370: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVX", (ftnlen)6, (ftnlen)6);
		dstevx_("N", "I", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___61.ciunit = *nounit;
		    s_wsfe(&io___61);
		    do_fio(&c__1, "DSTEVX(N,I)", (ftnlen)11);
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
			result[12] = ulpinv;
			goto L380;
		    }
		}

/*              Do test 12. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * temp3;
		result[12] = (temp1 + temp2) / max(d__1,d__2);

L380:

		ntest = 12;
		if (n > 0) {
		    if (il != 1) {
/* Computing MAX */
			d__1 = (wa1[il] - wa1[il - 1]) * .5, d__2 = ulp * 10. 
				* temp3, d__1 = max(d__1,d__2), d__2 = rtunfl 
				* 10.;
			vl = wa1[il] - max(d__1,d__2);
		    } else {
/* Computing MAX */
			d__1 = (wa1[n] - wa1[1]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vl = wa1[1] - max(d__1,d__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			d__1 = (wa1[iu + 1] - wa1[iu]) * .5, d__2 = ulp * 10. 
				* temp3, d__1 = max(d__1,d__2), d__2 = rtunfl 
				* 10.;
			vu = wa1[iu] + max(d__1,d__2);
		    } else {
/* Computing MAX */
			d__1 = (wa1[n] - wa1[1]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vu = wa1[n] + max(d__1,d__2);
		    }
		} else {
		    vl = 0.;
		    vu = 1.;
		}

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L390: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L400: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVX", (ftnlen)6, (ftnlen)6);
		dstevx_("V", "V", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___62.ciunit = *nounit;
		    s_wsfe(&io___62);
		    do_fio(&c__1, "DSTEVX(V,V)", (ftnlen)11);
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
			result[13] = ulpinv;
			result[14] = ulpinv;
			result[15] = ulpinv;
			goto L440;
		    }
		}

		if (m2 == 0 && n > 0) {
		    result[13] = ulpinv;
		    result[14] = ulpinv;
		    result[15] = ulpinv;
		    goto L440;
		}

/*              Do tests 13 and 14. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L410: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L420: */
		}
		i__3 = max(1,m2);
		dstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[13]);

		ntest = 15;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L430: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVX", (ftnlen)6, (ftnlen)6);
		dstevx_("N", "V", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___63.ciunit = *nounit;
		    s_wsfe(&io___63);
		    do_fio(&c__1, "DSTEVX(N,V)", (ftnlen)11);
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
			result[15] = ulpinv;
			goto L440;
		    }
		}

/*              Do test 15. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[15] = (temp1 + temp2) / max(d__1,d__2);

L440:

		ntest = 16;
		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L450: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L460: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVD", (ftnlen)6, (ftnlen)6);
		dstevd_("V", &n, &d1[1], &d2[1], &z__[z_offset], ldu, &work[1]
, &lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___64.ciunit = *nounit;
		    s_wsfe(&io___64);
		    do_fio(&c__1, "DSTEVD(V)", (ftnlen)9);
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
			result[17] = ulpinv;
			result[18] = ulpinv;
			goto L510;
		    }
		}

/*              Do tests 16 and 17. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L470: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L480: */
		}
		dstt21_(&n, &c__0, &d3[1], &d4[1], &d1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[16]);

		ntest = 18;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L490: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVD", (ftnlen)6, (ftnlen)6);
		dstevd_("N", &n, &d3[1], &d4[1], &z__[z_offset], ldu, &work[1]
, &lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___65.ciunit = *nounit;
		    s_wsfe(&io___65);
		    do_fio(&c__1, "DSTEVD(N)", (ftnlen)9);
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
			result[18] = ulpinv;
			goto L510;
		    }
		}

/*              Do test 18. */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = eveigs[j], abs(d__1)), d__3 =
			     max(d__3,d__4), d__4 = (d__2 = d3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = eveigs[j] - d3[j], abs(d__1))
			    ;
		    temp2 = max(d__2,d__3);
/* L500: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[18] = temp2 / max(d__1,d__2);

L510:

		ntest = 19;
		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L520: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L530: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dstevr_("V", "I", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___66.ciunit = *nounit;
		    s_wsfe(&io___66);
		    do_fio(&c__1, "DSTEVR(V,I)", (ftnlen)11);
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
			result[19] = ulpinv;
			result[20] = ulpinv;
			result[21] = ulpinv;
			goto L570;
		    }
		}

/*              DO tests 19 and 20. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L540: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L550: */
		}
		i__3 = max(1,m2);
		dstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[19]);


		ntest = 21;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L560: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dstevr_("N", "I", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___67.ciunit = *nounit;
		    s_wsfe(&io___67);
		    do_fio(&c__1, "DSTEVR(N,I)", (ftnlen)11);
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
			result[21] = ulpinv;
			goto L570;
		    }
		}

/*              Do test 21. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * temp3;
		result[21] = (temp1 + temp2) / max(d__1,d__2);

L570:

		ntest = 21;
		if (n > 0) {
		    if (il != 1) {
/* Computing MAX */
			d__1 = (wa1[il] - wa1[il - 1]) * .5, d__2 = ulp * 10. 
				* temp3, d__1 = max(d__1,d__2), d__2 = rtunfl 
				* 10.;
			vl = wa1[il] - max(d__1,d__2);
		    } else {
/* Computing MAX */
			d__1 = (wa1[n] - wa1[1]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vl = wa1[1] - max(d__1,d__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			d__1 = (wa1[iu + 1] - wa1[iu]) * .5, d__2 = ulp * 10. 
				* temp3, d__1 = max(d__1,d__2), d__2 = rtunfl 
				* 10.;
			vu = wa1[iu] + max(d__1,d__2);
		    } else {
/* Computing MAX */
			d__1 = (wa1[n] - wa1[1]) * .5, d__2 = ulp * 10. * 
				temp3, d__1 = max(d__1,d__2), d__2 = rtunfl * 
				10.;
			vu = wa1[n] + max(d__1,d__2);
		    }
		} else {
		    vl = 0.;
		    vu = 1.;
		}

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d1[i__] = a[i__ + i__ * a_dim1];
/* L580: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d2[i__] = a[i__ + 1 + i__ * a_dim1];
/* L590: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dstevr_("V", "V", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___68.ciunit = *nounit;
		    s_wsfe(&io___68);
		    do_fio(&c__1, "DSTEVR(V,V)", (ftnlen)11);
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
			result[22] = ulpinv;
			result[23] = ulpinv;
			result[24] = ulpinv;
			goto L630;
		    }
		}

		if (m2 == 0 && n > 0) {
		    result[22] = ulpinv;
		    result[23] = ulpinv;
		    result[24] = ulpinv;
		    goto L630;
		}

/*              Do tests 22 and 23. */

		i__3 = n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d3[i__] = a[i__ + i__ * a_dim1];
/* L600: */
		}
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L610: */
		}
		i__3 = max(1,m2);
		dstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[22]);

		ntest = 24;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L620: */
		}
		s_copy(srnamc_1.srnamt, "DSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dstevr_("N", "V", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___69.ciunit = *nounit;
		    s_wsfe(&io___69);
		    do_fio(&c__1, "DSTEVR(N,V)", (ftnlen)11);
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
			result[24] = ulpinv;
			goto L630;
		    }
		}

/*              Do test 24. */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		d__1 = unfl, d__2 = temp3 * ulp;
		result[24] = (temp1 + temp2) / max(d__1,d__2);

L630:



		;
	    } else {

		for (i__ = 1; i__ <= 24; ++i__) {
		    result[i__] = 0.;
/* L640: */
		}
		ntest = 24;
	    }

/*           Perform remaining tests storing upper or lower triangular */
/*           part of matrix. */

	    for (iuplo = 0; iuplo <= 1; ++iuplo) {
		if (iuplo == 0) {
		    *(unsigned char *)uplo = 'L';
		} else {
		    *(unsigned char *)uplo = 'U';
		}

/*              4)      Call DSYEV and DSYEVX. */

		dlacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

		++ntest;
		s_copy(srnamc_1.srnamt, "DSYEV", (ftnlen)6, (ftnlen)5);
		dsyev_("V", uplo, &n, &a[a_offset], ldu, &d1[1], &work[1], 
			lwork, &iinfo);
		if (iinfo != 0) {
		    io___72.ciunit = *nounit;
		    s_wsfe(&io___72);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "DSYEV(V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__1, (ftnlen)10);
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
			goto L660;
		    }
		}

/*              Do tests 25 and 26 (or +54) */

		dsyt21_(&c__1, uplo, &n, &c__0, &v[v_offset], ldu, &d1[1], &
			d2[1], &a[a_offset], ldu, &z__[z_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSYEV", (ftnlen)6, (ftnlen)5);
		dsyev_("N", uplo, &n, &a[a_offset], ldu, &d3[1], &work[1], 
			lwork, &iinfo);
		if (iinfo != 0) {
		    io___73.ciunit = *nounit;
		    s_wsfe(&io___73);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "DSYEV(N,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__1, (ftnlen)10);
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
			goto L660;
		    }
		}

/*              Do test 27 (or +54) */

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
/* L650: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L660:
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

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

		s_copy(srnamc_1.srnamt, "DSYEVX", (ftnlen)6, (ftnlen)6);
		dsyevx_("V", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m, &wa1[1], &z__[z_offset], ldu, &work[
			1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___74.ciunit = *nounit;
		    s_wsfe(&io___74);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVX(V,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L680;
		    }
		}

/*              Do tests 28 and 29 (or +54) */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSYEVX", (ftnlen)6, (ftnlen)6);
		dsyevx_("N", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___75.ciunit = *nounit;
		    s_wsfe(&io___75);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVX(N,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L680;
		    }
		}

/*              Do test 30 (or +54) */

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
/* L670: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L680:

		++ntest;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVX", (ftnlen)6, (ftnlen)6);
		dsyevx_("V", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___76.ciunit = *nounit;
		    s_wsfe(&io___76);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVX(V,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L690;
		    }
		}

/*              Do tests 31 and 32 (or +54) */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVX", (ftnlen)6, (ftnlen)6);
		dsyevx_("N", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___77.ciunit = *nounit;
		    s_wsfe(&io___77);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVX(N,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L690;
		    }
		}

/*              Do test 33 (or +54) */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * temp3;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);
L690:

		++ntest;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVX", (ftnlen)6, (ftnlen)6);
		dsyevx_("V", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___78.ciunit = *nounit;
		    s_wsfe(&io___78);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVX(V,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L700;
		    }
		}

/*              Do tests 34 and 35 (or +54) */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVX", (ftnlen)6, (ftnlen)6);
		dsyevx_("N", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___79.ciunit = *nounit;
		    s_wsfe(&io___79);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVX(N,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L700;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L700;
		}

/*              Do test 36 (or +54) */

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

L700:

/*              5)      Call DSPEV and DSPEVX. */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

/*              Load array WORK with the upper or lower triangular */
/*              part of the matrix in packed form. */

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L710: */
			}
/* L720: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L730: */
			}
/* L740: */
		    }
		}

		++ntest;
		s_copy(srnamc_1.srnamt, "DSPEV", (ftnlen)6, (ftnlen)5);
		dspev_("V", uplo, &n, &work[1], &d1[1], &z__[z_offset], ldu, &
			v[v_offset], &iinfo);
		if (iinfo != 0) {
		    io___81.ciunit = *nounit;
		    s_wsfe(&io___81);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "DSPEV(V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__1, (ftnlen)10);
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
			goto L800;
		    }
		}

/*              Do tests 37 and 38 (or +54) */

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L750: */
			}
/* L760: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L770: */
			}
/* L780: */
		    }
		}

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSPEV", (ftnlen)6, (ftnlen)5);
		dspev_("N", uplo, &n, &work[1], &d3[1], &z__[z_offset], ldu, &
			v[v_offset], &iinfo);
		if (iinfo != 0) {
		    io___82.ciunit = *nounit;
		    s_wsfe(&io___82);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "DSPEV(N,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__1, (ftnlen)10);
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
			goto L800;
		    }
		}

/*              Do test 39 (or +54) */

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
/* L790: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

/*              Load array WORK with the upper or lower triangular part */
/*              of the matrix in packed form. */

L800:
		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L810: */
			}
/* L820: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L830: */
			}
/* L840: */
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

		s_copy(srnamc_1.srnamt, "DSPEVX", (ftnlen)6, (ftnlen)6);
		dspevx_("V", "A", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m, &wa1[1], &z__[z_offset], ldu, &v[v_offset]
, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___83.ciunit = *nounit;
		    s_wsfe(&io___83);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSPEVX(V,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L900;
		    }
		}

/*              Do tests 40 and 41 (or +54) */

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L850: */
			}
/* L860: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L870: */
			}
/* L880: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSPEVX", (ftnlen)6, (ftnlen)6);
		dspevx_("N", "A", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___84.ciunit = *nounit;
		    s_wsfe(&io___84);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSPEVX(N,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L900;
		    }
		}

/*              Do test 42 (or +54) */

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
/* L890: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L900:
		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L910: */
			}
/* L920: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L930: */
			}
/* L940: */
		    }
		}

		++ntest;

		s_copy(srnamc_1.srnamt, "DSPEVX", (ftnlen)6, (ftnlen)6);
		dspevx_("V", "I", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___85.ciunit = *nounit;
		    s_wsfe(&io___85);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSPEVX(V,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L990;
		    }
		}

/*              Do tests 43 and 44 (or +54) */

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L950: */
			}
/* L960: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L970: */
			}
/* L980: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSPEVX", (ftnlen)6, (ftnlen)6);
		dspevx_("N", "I", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___86.ciunit = *nounit;
		    s_wsfe(&io___86);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSPEVX(N,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L990;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L990;
		}

/*              Do test 45 (or +54) */

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

L990:
		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1000: */
			}
/* L1010: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1020: */
			}
/* L1030: */
		    }
		}

		++ntest;

		s_copy(srnamc_1.srnamt, "DSPEVX", (ftnlen)6, (ftnlen)6);
		dspevx_("V", "V", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___87.ciunit = *nounit;
		    s_wsfe(&io___87);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSPEVX(V,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1080;
		    }
		}

/*              Do tests 46 and 47 (or +54) */

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = j;
			for (i__ = 1; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1040: */
			}
/* L1050: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__4 = n;
			for (i__ = j; i__ <= i__4; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1060: */
			}
/* L1070: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSPEVX", (ftnlen)6, (ftnlen)6);
		dspevx_("N", "V", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___88.ciunit = *nounit;
		    s_wsfe(&io___88);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSPEVX(N,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1080;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L1080;
		}

/*              Do test 48 (or +54) */

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

L1080:

/*              6)      Call DSBEV and DSBEVX. */

		if (jtype <= 7) {
		    kd = 1;
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
			i__7 = j;
			for (i__ = max(i__4,i__5); i__ <= i__7; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1090: */
			}
/* L1100: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__7 = min(i__4,i__5);
			for (i__ = j; i__ <= i__7; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1110: */
			}
/* L1120: */
		    }
		}

		++ntest;
		s_copy(srnamc_1.srnamt, "DSBEV", (ftnlen)6, (ftnlen)5);
		dsbev_("V", uplo, &n, &kd, &v[v_offset], ldu, &d1[1], &z__[
			z_offset], ldu, &work[1], &iinfo);
		if (iinfo != 0) {
		    io___90.ciunit = *nounit;
		    s_wsfe(&io___90);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "DSBEV(V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__1, (ftnlen)10);
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

/*              Do tests 49 and 50 (or ... ) */

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__7 = 1, i__4 = j - kd;
			i__5 = j;
			for (i__ = max(i__7,i__4); i__ <= i__5; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1130: */
			}
/* L1140: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__7 = n, i__4 = j + kd;
			i__5 = min(i__7,i__4);
			for (i__ = j; i__ <= i__5; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1150: */
			}
/* L1160: */
		    }
		}

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSBEV", (ftnlen)6, (ftnlen)5);
		dsbev_("N", uplo, &n, &kd, &v[v_offset], ldu, &d3[1], &z__[
			z_offset], ldu, &work[1], &iinfo);
		if (iinfo != 0) {
		    io___91.ciunit = *nounit;
		    s_wsfe(&io___91);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "DSBEV(N,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__1, a__1, i__6, &c__3, (ftnlen)10);
		    do_fio(&c__1, ch__1, (ftnlen)10);
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

/*              Do test 51 (or +54) */

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
/* L1170: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

/*              Load array V with the upper or lower triangular part */
/*              of the matrix in band form. */

L1180:
		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__5 = 1, i__7 = j - kd;
			i__4 = j;
			for (i__ = max(i__5,i__7); i__ <= i__4; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1190: */
			}
/* L1200: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__5 = n, i__7 = j + kd;
			i__4 = min(i__5,i__7);
			for (i__ = j; i__ <= i__4; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1210: */
			}
/* L1220: */
		    }
		}

		++ntest;
		s_copy(srnamc_1.srnamt, "DSBEVX", (ftnlen)6, (ftnlen)6);
		dsbevx_("V", "A", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m, &wa2[
			1], &z__[z_offset], ldu, &work[1], &iwork[1], &iwork[
			n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___92.ciunit = *nounit;
		    s_wsfe(&io___92);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSBEVX(V,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1280;
		    }
		}

/*              Do tests 52 and 53 (or +54) */

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa2[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__4 = 1, i__5 = j - kd;
			i__7 = j;
			for (i__ = max(i__4,i__5); i__ <= i__7; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1230: */
			}
/* L1240: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__7 = min(i__4,i__5);
			for (i__ = j; i__ <= i__7; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1250: */
			}
/* L1260: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSBEVX", (ftnlen)6, (ftnlen)6);
		dsbevx_("N", "A", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___93.ciunit = *nounit;
		    s_wsfe(&io___93);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSBEVX(N,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1280;
		    }
		}

/*              Do test 54 (or +54) */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = wa2[j], abs(d__1)), d__3 = 
			    max(d__3,d__4), d__4 = (d__2 = wa3[j], abs(d__2));
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = wa2[j] - wa3[j], abs(d__1));
		    temp2 = max(d__2,d__3);
/* L1270: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L1280:
		++ntest;
		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__7 = 1, i__4 = j - kd;
			i__5 = j;
			for (i__ = max(i__7,i__4); i__ <= i__5; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1290: */
			}
/* L1300: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__7 = n, i__4 = j + kd;
			i__5 = min(i__7,i__4);
			for (i__ = j; i__ <= i__5; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1310: */
			}
/* L1320: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSBEVX", (ftnlen)6, (ftnlen)6);
		dsbevx_("V", "I", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m2, &
			wa2[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___94.ciunit = *nounit;
		    s_wsfe(&io___94);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSBEVX(V,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1370;
		    }
		}

/*              Do tests 55 and 56 (or +54) */

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__5 = 1, i__7 = j - kd;
			i__4 = j;
			for (i__ = max(i__5,i__7); i__ <= i__4; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1330: */
			}
/* L1340: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__5 = n, i__7 = j + kd;
			i__4 = min(i__5,i__7);
			for (i__ = j; i__ <= i__4; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1350: */
			}
/* L1360: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSBEVX", (ftnlen)6, (ftnlen)6);
		dsbevx_("N", "I", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___95.ciunit = *nounit;
		    s_wsfe(&io___95);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSBEVX(N,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1370;
		    }
		}

/*              Do test 57 (or +54) */

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

L1370:
		++ntest;
		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__4 = 1, i__5 = j - kd;
			i__7 = j;
			for (i__ = max(i__4,i__5); i__ <= i__7; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1380: */
			}
/* L1390: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__7 = min(i__4,i__5);
			for (i__ = j; i__ <= i__7; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1400: */
			}
/* L1410: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSBEVX", (ftnlen)6, (ftnlen)6);
		dsbevx_("V", "V", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m2, &
			wa2[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___96.ciunit = *nounit;
		    s_wsfe(&io___96);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSBEVX(V,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1460;
		    }
		}

/*              Do tests 58 and 59 (or +54) */

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__7 = 1, i__4 = j - kd;
			i__5 = j;
			for (i__ = max(i__7,i__4); i__ <= i__5; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1420: */
			}
/* L1430: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__7 = n, i__4 = j + kd;
			i__5 = min(i__7,i__4);
			for (i__ = j; i__ <= i__5; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1440: */
			}
/* L1450: */
		    }
		}

		s_copy(srnamc_1.srnamt, "DSBEVX", (ftnlen)6, (ftnlen)6);
		dsbevx_("N", "V", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___97.ciunit = *nounit;
		    s_wsfe(&io___97);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSBEVX(N,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1460;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L1460;
		}

/*              Do test 60 (or +54) */

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

L1460:

/*              7)      Call DSYEVD */

		dlacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

		++ntest;
		s_copy(srnamc_1.srnamt, "DSYEVD", (ftnlen)6, (ftnlen)6);
		dsyevd_("V", uplo, &n, &a[a_offset], ldu, &d1[1], &work[1], &
			lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___98.ciunit = *nounit;
		    s_wsfe(&io___98);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "DSYEVD(V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__3, (ftnlen)11);
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
			goto L1480;
		    }
		}

/*              Do tests 61 and 62 (or +54) */

		dsyt21_(&c__1, uplo, &n, &c__0, &v[v_offset], ldu, &d1[1], &
			d2[1], &a[a_offset], ldu, &z__[z_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSYEVD", (ftnlen)6, (ftnlen)6);
		dsyevd_("N", uplo, &n, &a[a_offset], ldu, &d3[1], &work[1], &
			lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___99.ciunit = *nounit;
		    s_wsfe(&io___99);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "DSYEVD(N,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__3, (ftnlen)11);
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
			goto L1480;
		    }
		}

/*              Do test 63 (or +54) */

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
/* L1470: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L1480:

/*              8)      Call DSPEVD. */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

/*              Load array WORK with the upper or lower triangular */
/*              part of the matrix in packed form. */

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = j;
			for (i__ = 1; i__ <= i__5; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1490: */
			}
/* L1500: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = n;
			for (i__ = j; i__ <= i__5; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1510: */
			}
/* L1520: */
		    }
		}

		++ntest;
		s_copy(srnamc_1.srnamt, "DSPEVD", (ftnlen)6, (ftnlen)6);
		i__3 = lwedc - indx + 1;
		dspevd_("V", uplo, &n, &work[1], &d1[1], &z__[z_offset], ldu, 
			&work[indx], &i__3, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___100.ciunit = *nounit;
		    s_wsfe(&io___100);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "DSPEVD(V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__3, (ftnlen)11);
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
			goto L1580;
		    }
		}

/*              Do tests 64 and 65 (or +54) */

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		if (iuplo == 1) {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = j;
			for (i__ = 1; i__ <= i__5; ++i__) {

			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1530: */
			}
/* L1540: */
		    }
		} else {
		    indx = 1;
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
			i__5 = n;
			for (i__ = j; i__ <= i__5; ++i__) {
			    work[indx] = a[i__ + j * a_dim1];
			    ++indx;
/* L1550: */
			}
/* L1560: */
		    }
		}

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSPEVD", (ftnlen)6, (ftnlen)6);
		i__3 = lwedc - indx + 1;
		dspevd_("N", uplo, &n, &work[1], &d3[1], &z__[z_offset], ldu, 
			&work[indx], &i__3, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___101.ciunit = *nounit;
		    s_wsfe(&io___101);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "DSPEVD(N,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__3, (ftnlen)11);
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
			goto L1580;
		    }
		}

/*              Do test 66 (or +54) */

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
/* L1570: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);
L1580:

/*              9)      Call DSBEVD. */

		if (jtype <= 7) {
		    kd = 1;
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
			i__5 = 1, i__7 = j - kd;
			i__4 = j;
			for (i__ = max(i__5,i__7); i__ <= i__4; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1590: */
			}
/* L1600: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__5 = n, i__7 = j + kd;
			i__4 = min(i__5,i__7);
			for (i__ = j; i__ <= i__4; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1610: */
			}
/* L1620: */
		    }
		}

		++ntest;
		s_copy(srnamc_1.srnamt, "DSBEVD", (ftnlen)6, (ftnlen)6);
		dsbevd_("V", uplo, &n, &kd, &v[v_offset], ldu, &d1[1], &z__[
			z_offset], ldu, &work[1], &lwedc, &iwork[1], &liwedc, 
			&iinfo);
		if (iinfo != 0) {
		    io___102.ciunit = *nounit;
		    s_wsfe(&io___102);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "DSBEVD(V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__3, (ftnlen)11);
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
			goto L1680;
		    }
		}

/*              Do tests 67 and 68 (or +54) */

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		if (iuplo == 1) {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
			i__4 = 1, i__5 = j - kd;
			i__7 = j;
			for (i__ = max(i__4,i__5); i__ <= i__7; ++i__) {
			    v[kd + 1 + i__ - j + j * v_dim1] = a[i__ + j * 
				    a_dim1];
/* L1630: */
			}
/* L1640: */
		    }
		} else {
		    i__3 = n;
		    for (j = 1; j <= i__3; ++j) {
/* Computing MIN */
			i__4 = n, i__5 = j + kd;
			i__7 = min(i__4,i__5);
			for (i__ = j; i__ <= i__7; ++i__) {
			    v[i__ + 1 - j + j * v_dim1] = a[i__ + j * a_dim1];
/* L1650: */
			}
/* L1660: */
		    }
		}

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSBEVD", (ftnlen)6, (ftnlen)6);
		dsbevd_("N", uplo, &n, &kd, &v[v_offset], ldu, &d3[1], &z__[
			z_offset], ldu, &work[1], &lwedc, &iwork[1], &liwedc, 
			&iinfo);
		if (iinfo != 0) {
		    io___103.ciunit = *nounit;
		    s_wsfe(&io___103);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "DSBEVD(N,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__3, a__1, i__6, &c__3, (ftnlen)11);
		    do_fio(&c__1, ch__3, (ftnlen)11);
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
			goto L1680;
		    }
		}

/*              Do test 69 (or +54) */

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
/* L1670: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L1680:


		dlacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);
		++ntest;
		s_copy(srnamc_1.srnamt, "DSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dsyevr_("V", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m, &wa1[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___104.ciunit = *nounit;
		    s_wsfe(&io___104);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVR(V,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1700;
		    }
		}

/*              Do tests 70 and 71 (or ... ) */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		dsyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "DSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dsyevr_("N", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___105.ciunit = *nounit;
		    s_wsfe(&io___105);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVR(N,A,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1700;
		    }
		}

/*              Do test 72 (or ... ) */

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
/* L1690: */
		}
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * max(temp1,temp2);
		result[ntest] = temp2 / max(d__1,d__2);

L1700:

		++ntest;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dsyevr_("V", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___106.ciunit = *nounit;
		    s_wsfe(&io___106);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVR(V,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1710;
		    }
		}

/*              Do tests 73 and 74 (or +54) */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dsyevr_("N", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___107.ciunit = *nounit;
		    s_wsfe(&io___107);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVR(N,I,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L1710;
		    }
		}

/*              Do test 75 (or +54) */

		temp1 = dsxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = dsxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		d__1 = unfl, d__2 = ulp * temp3;
		result[ntest] = (temp1 + temp2) / max(d__1,d__2);
L1710:

		++ntest;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dsyevr_("V", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___108.ciunit = *nounit;
		    s_wsfe(&io___108);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVR(V,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L700;
		    }
		}

/*              Do tests 76 and 77 (or +54) */

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		dsyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "DSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		dsyevr_("N", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___109.ciunit = *nounit;
		    s_wsfe(&io___109);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "DSYEVR(N,V,";
		    i__6[1] = 1, a__1[1] = uplo;
		    i__6[2] = 1, a__1[2] = ")";
		    s_cat(ch__2, a__1, i__6, &c__3, (ftnlen)13);
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
			goto L700;
		    }
		}

		if (m3 == 0 && n > 0) {
		    result[ntest] = ulpinv;
		    goto L700;
		}

/*              Do test 78 (or +54) */

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

		dlacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

/* L1720: */
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

	    ntestt += ntest;

	    dlafts_("DST", &n, &n, &jtype, &ntest, &result[1], ioldsd, thresh, 
		     nounit, &nerrs);

L1730:
	    ;
	}
/* L1740: */
    }

/*     Summary */

    alasvm_("DST", nounit, &nerrs, &ntestt, &c__0);


    return 0;

/*     End of DDRVST */

} /* ddrvst_ */
