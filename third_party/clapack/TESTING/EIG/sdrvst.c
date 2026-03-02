#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    char srnamt[6];
} srnamc_;

#define srnamc_1 srnamc_

/* Table of constant values */

static integer c__2 = 2;
static real c_b20 = 0.f;
static integer c__0 = 0;
static integer c__6 = 6;
static real c_b34 = 1.f;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__3 = 3;

/* Subroutine */ int sdrvst_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, real *thresh, integer *nounit, real *
	a, integer *lda, real *d1, real *d2, real *d3, real *d4, real *eveigs, 
	 real *wa1, real *wa2, real *wa3, real *u, integer *ldu, real *v, 
	real *tau, real *z__, real *work, integer *lwork, integer *iwork, 
	integer *liwork, real *result, integer *info)
{
    /* Initialized data */

    static integer ktype[18] = { 1,2,4,4,4,4,4,5,5,5,5,5,8,8,8,9,9,9 };
    static integer kmagn[18] = { 1,1,1,1,1,2,3,1,1,1,2,3,1,2,3,1,2,3 };
    static integer kmode[18] = { 0,0,4,3,1,4,4,4,3,1,4,4,0,0,0,4,4,4 };

    /* Format strings */
    static char fmt_9999[] = "(\002 SDRVST: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";

    /* System generated locals */
    address a__1[3];
    integer a_dim1, a_offset, u_dim1, u_offset, v_dim1, v_offset, z_dim1, 
	    z_offset, i__1, i__2, i__3, i__4, i__5, i__6[3], i__7;
    real r__1, r__2, r__3, r__4;
    char ch__1[10], ch__2[13], ch__3[11];

    /* Builtin functions */
    double sqrt(doublereal), log(doublereal);
    integer pow_ii(integer *, integer *), s_wsfe(cilist *), do_fio(integer *, 
	    char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen), s_cat(char *,
	     char **, integer *, integer *, ftnlen);

    /* Local variables */
    integer i__, j, m, n, j1, j2, m2, m3, kd, il, iu;
    real vl, vu;
    integer lgn;
    real ulp, cond;
    integer jcol, ihbw, indx, nmax;
    real unfl, ovfl;
    char uplo[1];
    integer irow;
    real temp1, temp2, temp3;
    integer idiag;
    logical badnn;
    extern doublereal ssxt1_(integer *, real *, integer *, real *, integer *, 
	    real *, real *, real *);
    integer imode, lwedc, iinfo;
    real aninv, anorm;
    integer itemp, nmats;
    extern /* Subroutine */ int ssbev_(char *, char *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *, real *, integer *);
    integer jsize, iuplo, nerrs, itype, jtype, ntest;
    extern /* Subroutine */ int sspev_(char *, char *, integer *, real *, 
	    real *, real *, integer *, real *, integer *), 
	    sstt21_(integer *, integer *, real *, real *, real *, real *, 
	    real *, integer *, real *, real *), sstt22_(integer *, integer *, 
	    integer *, real *, real *, real *, real *, real *, integer *, 
	    real *, integer *, real *), sstev_(char *, integer *, real *, 
	    real *, real *, integer *, real *, integer *), ssyt21_(
	    integer *, char *, integer *, integer *, real *, integer *, real *
, real *, real *, integer *, real *, integer *, real *, real *, 
	    real *), ssyt22_(integer *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, real *, integer *, 
	    real *, integer *, real *, real *, real *), ssyev_(char *, 
	     char *, integer *, real *, integer *, real *, real *, integer *, 
	    integer *);
    integer iseed2[4], iseed3[4];
    extern /* Subroutine */ int slabad_(real *, real *);
    integer liwedc;
    extern doublereal slamch_(char *);
    integer idumma[1];
    extern /* Subroutine */ int xerbla_(char *, integer *);
    integer ioldsd[4];
    extern doublereal slarnd_(integer *, integer *);
    real abstol;
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *), ssbevd_(char *, char *, integer *, integer 
	    *, real *, integer *, real *, real *, integer *, real *, integer *
, integer *, integer *, integer *), slacpy_(char *
, integer *, integer *, real *, integer *, real *, integer *), slafts_(char *, integer *, integer *, integer *, integer 
	    *, real *, integer *, real *, integer *, integer *), 
	    slaset_(char *, integer *, integer *, real *, real *, real *, 
	    integer *), slatmr_(integer *, integer *, char *, integer 
	    *, char *, real *, integer *, real *, real *, char *, char *, 
	    real *, integer *, real *, real *, integer *, real *, char *, 
	    integer *, integer *, integer *, real *, real *, char *, real *, 
	    integer *, integer *, integer *), slatms_(integer *, integer *, char *, integer *, 
	    char *, real *, integer *, real *, real *, integer *, integer *, 
	    char *, real *, integer *, real *, integer *), sspevd_(char *, char *, integer *, real *, real *, real *
, integer *, real *, integer *, integer *, integer *, integer *), sstevd_(char *, integer *, real *, real *, real *
, integer *, real *, integer *, integer *, integer *, integer *);
    real rtunfl, rtovfl, ulpinv;
    extern /* Subroutine */ int ssbevx_(char *, char *, char *, integer *, 
	    integer *, real *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, real *, integer *, integer *, integer *)
	    ;
    integer mtypes, ntestt;
    extern /* Subroutine */ int sstevr_(char *, char *, integer *, real *, 
	    real *, real *, real *, integer *, integer *, real *, integer *, 
	    real *, real *, integer *, integer *, real *, integer *, integer *
, integer *, integer *), ssyevd_(char *, char *, 
	    integer *, real *, integer *, real *, real *, integer *, integer *
, integer *, integer *), sspevx_(char *, char *, 
	    char *, integer *, real *, real *, real *, integer *, integer *, 
	    real *, integer *, real *, real *, integer *, real *, integer *, 
	    integer *, integer *), ssyevr_(char *, 
	    char *, char *, integer *, real *, integer *, real *, real *, 
	    integer *, integer *, real *, integer *, real *, real *, integer *
, integer *, real *, integer *, integer *, integer *, integer *), sstevx_(char *, char *, integer *, real *
, real *, real *, real *, integer *, integer *, real *, integer *, 
	     real *, real *, integer *, real *, integer *, integer *, integer 
	    *), ssyevx_(char *, char *, char *, integer *, 
	    real *, integer *, real *, real *, integer *, integer *, real *, 
	    integer *, real *, real *, integer *, real *, integer *, integer *
, integer *, integer *);

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

/*       SDRVST  checks the symmetric eigenvalue problem drivers. */

/*               SSTEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric tridiagonal matrix. */

/*               SSTEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric tridiagonal matrix. */

/*               SSTEVR computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric tridiagonal matrix */
/*               using the Relatively Robust Representation where it can. */

/*               SSYEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix. */

/*               SSYEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix. */

/*               SSYEVR computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix */
/*               using the Relatively Robust Representation where it can. */

/*               SSPEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix in packed */
/*               storage. */

/*               SSPEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix in packed */
/*               storage. */

/*               SSBEV computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric band matrix. */

/*               SSBEVX computes selected eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric band matrix. */

/*               SSYEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix using */
/*               a divide and conquer algorithm. */

/*               SSPEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric matrix in packed */
/*               storage, using a divide and conquer algorithm. */

/*               SSBEVD computes all eigenvalues and, optionally, */
/*               eigenvectors of a real symmetric band matrix, */
/*               using a divide and conquer algorithm. */

/*       When SDRVST is called, a number of matrix "sizes" ("n's") and a */
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
/*          SDRVST does nothing.  It must be at least zero. */
/*          Not modified. */

/*  NN      INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */
/*          Not modified. */

/*  NTYPES  INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, SDRVST */
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
/*          next call to SDRVST to continue the same random number */
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

/*  A       REAL array, dimension (LDA , max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually */
/*          used. */
/*          Modified. */

/*  LDA     INTEGER */
/*          The leading dimension of A.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  D1      REAL array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by SSTEQR simlutaneously */
/*          with Z.  On exit, the eigenvalues in D1 correspond with the */
/*          matrix in A. */
/*          Modified. */

/*  D2      REAL array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by SSTEQR if Z is not */
/*          computed.  On exit, the eigenvalues in D2 correspond with */
/*          the matrix in A. */
/*          Modified. */

/*  D3      REAL array, dimension (max(NN)) */
/*          The eigenvalues of A, as computed by SSTERF.  On exit, the */
/*          eigenvalues in D3 correspond with the matrix in A. */
/*          Modified. */

/*  D4      REAL array, dimension */

/*  EVEIGS  REAL array, dimension (max(NN)) */
/*          The eigenvalues as computed by SSTEV('N', ... ) */
/*          (I reserve the right to change this to the output of */
/*          whichever algorithm computes the most accurate eigenvalues). */

/*  WA1     REAL array, dimension */

/*  WA2     REAL array, dimension */

/*  WA3     REAL array, dimension */

/*  U       REAL array, dimension (LDU, max(NN)) */
/*          The orthogonal matrix computed by SSYTRD + SORGTR. */
/*          Modified. */

/*  LDU     INTEGER */
/*          The leading dimension of U, Z, and V.  It must be at */
/*          least 1 and at least max( NN ). */
/*          Not modified. */

/*  V       REAL array, dimension (LDU, max(NN)) */
/*          The Housholder vectors computed by SSYTRD in reducing A to */
/*          tridiagonal form. */
/*          Modified. */

/*  TAU     REAL array, dimension (max(NN)) */
/*          The Householder factors computed by SSYTRD in reducing A */
/*          to tridiagonal form. */
/*          Modified. */

/*  Z       REAL array, dimension (LDU, max(NN)) */
/*          The orthogonal matrix of eigenvectors computed by SSTEQR, */
/*          SPTEQR, and SSTEIN. */
/*          Modified. */

/*  WORK    REAL array, dimension (LWORK) */
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

/*  RESULT  REAL array, dimension (105) */
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
/*          If  SLATMR, SLATMS, SSYTRD, SORGTR, SSTEQR, SSTERF, */
/*              or SORMTR returns an error code, the */
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

/*     The tests performed are:                 Routine tested */
/*    1= | A - U S U' | / ( |A| n ulp )         SSTEV('V', ... ) */
/*    2= | I - U U' | / ( n ulp )               SSTEV('V', ... ) */
/*    3= |D(with Z) - D(w/o Z)| / (|D| ulp)     SSTEV('N', ... ) */
/*    4= | A - U S U' | / ( |A| n ulp )         SSTEVX('V','A', ... ) */
/*    5= | I - U U' | / ( n ulp )               SSTEVX('V','A', ... ) */
/*    6= |D(with Z) - EVEIGS| / (|D| ulp)       SSTEVX('N','A', ... ) */
/*    7= | A - U S U' | / ( |A| n ulp )         SSTEVR('V','A', ... ) */
/*    8= | I - U U' | / ( n ulp )               SSTEVR('V','A', ... ) */
/*    9= |D(with Z) - EVEIGS| / (|D| ulp)       SSTEVR('N','A', ... ) */
/*    10= | A - U S U' | / ( |A| n ulp )        SSTEVX('V','I', ... ) */
/*    11= | I - U U' | / ( n ulp )              SSTEVX('V','I', ... ) */
/*    12= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSTEVX('N','I', ... ) */
/*    13= | A - U S U' | / ( |A| n ulp )        SSTEVX('V','V', ... ) */
/*    14= | I - U U' | / ( n ulp )              SSTEVX('V','V', ... ) */
/*    15= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSTEVX('N','V', ... ) */
/*    16= | A - U S U' | / ( |A| n ulp )        SSTEVD('V', ... ) */
/*    17= | I - U U' | / ( n ulp )              SSTEVD('V', ... ) */
/*    18= |D(with Z) - EVEIGS| / (|D| ulp)      SSTEVD('N', ... ) */
/*    19= | A - U S U' | / ( |A| n ulp )        SSTEVR('V','I', ... ) */
/*    20= | I - U U' | / ( n ulp )              SSTEVR('V','I', ... ) */
/*    21= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSTEVR('N','I', ... ) */
/*    22= | A - U S U' | / ( |A| n ulp )        SSTEVR('V','V', ... ) */
/*    23= | I - U U' | / ( n ulp )              SSTEVR('V','V', ... ) */
/*    24= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSTEVR('N','V', ... ) */

/*    25= | A - U S U' | / ( |A| n ulp )        SSYEV('L','V', ... ) */
/*    26= | I - U U' | / ( n ulp )              SSYEV('L','V', ... ) */
/*    27= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEV('L','N', ... ) */
/*    28= | A - U S U' | / ( |A| n ulp )        SSYEVX('L','V','A', ... ) */
/*    29= | I - U U' | / ( n ulp )              SSYEVX('L','V','A', ... ) */
/*    30= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEVX('L','N','A', ... ) */
/*    31= | A - U S U' | / ( |A| n ulp )        SSYEVX('L','V','I', ... ) */
/*    32= | I - U U' | / ( n ulp )              SSYEVX('L','V','I', ... ) */
/*    33= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEVX('L','N','I', ... ) */
/*    34= | A - U S U' | / ( |A| n ulp )        SSYEVX('L','V','V', ... ) */
/*    35= | I - U U' | / ( n ulp )              SSYEVX('L','V','V', ... ) */
/*    36= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEVX('L','N','V', ... ) */
/*    37= | A - U S U' | / ( |A| n ulp )        SSPEV('L','V', ... ) */
/*    38= | I - U U' | / ( n ulp )              SSPEV('L','V', ... ) */
/*    39= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEV('L','N', ... ) */
/*    40= | A - U S U' | / ( |A| n ulp )        SSPEVX('L','V','A', ... ) */
/*    41= | I - U U' | / ( n ulp )              SSPEVX('L','V','A', ... ) */
/*    42= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEVX('L','N','A', ... ) */
/*    43= | A - U S U' | / ( |A| n ulp )        SSPEVX('L','V','I', ... ) */
/*    44= | I - U U' | / ( n ulp )              SSPEVX('L','V','I', ... ) */
/*    45= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEVX('L','N','I', ... ) */
/*    46= | A - U S U' | / ( |A| n ulp )        SSPEVX('L','V','V', ... ) */
/*    47= | I - U U' | / ( n ulp )              SSPEVX('L','V','V', ... ) */
/*    48= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEVX('L','N','V', ... ) */
/*    49= | A - U S U' | / ( |A| n ulp )        SSBEV('L','V', ... ) */
/*    50= | I - U U' | / ( n ulp )              SSBEV('L','V', ... ) */
/*    51= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEV('L','N', ... ) */
/*    52= | A - U S U' | / ( |A| n ulp )        SSBEVX('L','V','A', ... ) */
/*    53= | I - U U' | / ( n ulp )              SSBEVX('L','V','A', ... ) */
/*    54= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEVX('L','N','A', ... ) */
/*    55= | A - U S U' | / ( |A| n ulp )        SSBEVX('L','V','I', ... ) */
/*    56= | I - U U' | / ( n ulp )              SSBEVX('L','V','I', ... ) */
/*    57= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEVX('L','N','I', ... ) */
/*    58= | A - U S U' | / ( |A| n ulp )        SSBEVX('L','V','V', ... ) */
/*    59= | I - U U' | / ( n ulp )              SSBEVX('L','V','V', ... ) */
/*    60= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEVX('L','N','V', ... ) */
/*    61= | A - U S U' | / ( |A| n ulp )        SSYEVD('L','V', ... ) */
/*    62= | I - U U' | / ( n ulp )              SSYEVD('L','V', ... ) */
/*    63= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEVD('L','N', ... ) */
/*    64= | A - U S U' | / ( |A| n ulp )        SSPEVD('L','V', ... ) */
/*    65= | I - U U' | / ( n ulp )              SSPEVD('L','V', ... ) */
/*    66= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEVD('L','N', ... ) */
/*    67= | A - U S U' | / ( |A| n ulp )        SSBEVD('L','V', ... ) */
/*    68= | I - U U' | / ( n ulp )              SSBEVD('L','V', ... ) */
/*    69= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEVD('L','N', ... ) */
/*    70= | A - U S U' | / ( |A| n ulp )        SSYEVR('L','V','A', ... ) */
/*    71= | I - U U' | / ( n ulp )              SSYEVR('L','V','A', ... ) */
/*    72= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEVR('L','N','A', ... ) */
/*    73= | A - U S U' | / ( |A| n ulp )        SSYEVR('L','V','I', ... ) */
/*    74= | I - U U' | / ( n ulp )              SSYEVR('L','V','I', ... ) */
/*    75= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEVR('L','N','I', ... ) */
/*    76= | A - U S U' | / ( |A| n ulp )        SSYEVR('L','V','V', ... ) */
/*    77= | I - U U' | / ( n ulp )              SSYEVR('L','V','V', ... ) */
/*    78= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSYEVR('L','N','V', ... ) */

/*    Tests 25 through 78 are repeated (as tests 79 through 132) */
/*    with UPLO='U' */

/*    To be added in 1999 */

/*    79= | A - U S U' | / ( |A| n ulp )        SSPEVR('L','V','A', ... ) */
/*    80= | I - U U' | / ( n ulp )              SSPEVR('L','V','A', ... ) */
/*    81= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEVR('L','N','A', ... ) */
/*    82= | A - U S U' | / ( |A| n ulp )        SSPEVR('L','V','I', ... ) */
/*    83= | I - U U' | / ( n ulp )              SSPEVR('L','V','I', ... ) */
/*    84= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEVR('L','N','I', ... ) */
/*    85= | A - U S U' | / ( |A| n ulp )        SSPEVR('L','V','V', ... ) */
/*    86= | I - U U' | / ( n ulp )              SSPEVR('L','V','V', ... ) */
/*    87= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSPEVR('L','N','V', ... ) */
/*    88= | A - U S U' | / ( |A| n ulp )        SSBEVR('L','V','A', ... ) */
/*    89= | I - U U' | / ( n ulp )              SSBEVR('L','V','A', ... ) */
/*    90= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEVR('L','N','A', ... ) */
/*    91= | A - U S U' | / ( |A| n ulp )        SSBEVR('L','V','I', ... ) */
/*    92= | I - U U' | / ( n ulp )              SSBEVR('L','V','I', ... ) */
/*    93= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEVR('L','N','I', ... ) */
/*    94= | A - U S U' | / ( |A| n ulp )        SSBEVR('L','V','V', ... ) */
/*    95= | I - U U' | / ( n ulp )              SSBEVR('L','V','V', ... ) */
/*    96= |D(with Z) - D(w/o Z)| / (|D| ulp)    SSBEVR('L','N','V', ... ) */


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

    vl = 0.f;
    vu = 0.f;

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
	xerbla_("SDRVST", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

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
	    lgn = (integer) (log((real) n) / log(2.f));
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
	aninv = 1.f / (real) max(1,n);

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
	    anorm = 1.f;
	    goto L70;

L50:
	    anorm = rtovfl * ulp * aninv;
	    goto L70;

L60:
	    anorm = rtunfl * n * ulpinv;
	    goto L70;

L70:

	    slaset_("Full", lda, &n, &c_b20, &c_b20, &a[a_offset], lda);
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

		slatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &cond, 
			&anorm, &c__0, &c__0, "N", &a[a_offset], lda, &work[n 
			+ 1], &iinfo);

	    } else if (itype == 5) {

/*              Symmetric, eigenvalues specified */

		slatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &cond, 
			&anorm, &n, &n, "N", &a[a_offset], lda, &work[n + 1], 
			&iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		idumma[0] = 1;
		slatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b34, 
			&c_b34, "T", "N", &work[n + 1], &c__1, &c_b34, &work[(
			n << 1) + 1], &c__1, &c_b34, "N", idumma, &c__0, &
			c__0, &c_b20, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		idumma[0] = 1;
		slatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b34, 
			&c_b34, "T", "N", &work[n + 1], &c__1, &c_b34, &work[(
			n << 1) + 1], &c__1, &c_b34, "N", idumma, &n, &n, &
			c_b20, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              Symmetric banded, eigenvalues specified */

		ihbw = (integer) ((n - 1) * slarnd_(&c__1, iseed3));
		slatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &cond, 
			&anorm, &ihbw, &ihbw, "Z", &u[u_offset], ldu, &work[n 
			+ 1], &iinfo);

/*              Store as dense matrix for most routines. */

		slaset_("Full", lda, &n, &c_b20, &c_b20, &a[a_offset], lda);
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
		il = (integer) ((n - 1) * slarnd_(&c__1, iseed2)) + 1;
		iu = (integer) ((n - 1) * slarnd_(&c__1, iseed2)) + 1;
		if (il > iu) {
		    itemp = il;
		    il = iu;
		    iu = itemp;
		}
	    }

/*           3)      If matrix is tridiagonal, call SSTEV and SSTEVX. */

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
		s_copy(srnamc_1.srnamt, "SSTEV", (ftnlen)6, (ftnlen)5);
		sstev_("V", &n, &d1[1], &d2[1], &z__[z_offset], ldu, &work[1], 
			 &iinfo);
		if (iinfo != 0) {
		    io___48.ciunit = *nounit;
		    s_wsfe(&io___48);
		    do_fio(&c__1, "SSTEV(V)", (ftnlen)8);
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
		sstt21_(&n, &c__0, &d3[1], &d4[1], &d1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[1]);

		ntest = 3;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L160: */
		}
		s_copy(srnamc_1.srnamt, "SSTEV", (ftnlen)6, (ftnlen)5);
		sstev_("N", &n, &d3[1], &d4[1], &z__[z_offset], ldu, &work[1], 
			 &iinfo);
		if (iinfo != 0) {
		    io___49.ciunit = *nounit;
		    s_wsfe(&io___49);
		    do_fio(&c__1, "SSTEV(N)", (ftnlen)8);
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = d1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = d1[j] - d3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L170: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[3] = temp2 / dmax(r__1,r__2);

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
		s_copy(srnamc_1.srnamt, "SSTEVX", (ftnlen)6, (ftnlen)6);
		sstevx_("V", "A", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m, &wa1[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___53.ciunit = *nounit;
		    s_wsfe(&io___53);
		    do_fio(&c__1, "SSTEVX(V,A)", (ftnlen)11);
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
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
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
		sstt21_(&n, &c__0, &d3[1], &d4[1], &wa1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[4]);

		ntest = 6;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L230: */
		}
		s_copy(srnamc_1.srnamt, "SSTEVX", (ftnlen)6, (ftnlen)6);
		sstevx_("N", "A", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___56.ciunit = *nounit;
		    s_wsfe(&io___56);
		    do_fio(&c__1, "SSTEVX(N,A)", (ftnlen)11);
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = wa2[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = eveigs[j], dabs(
			    r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = wa2[j] - eveigs[j], dabs(
			    r__1));
		    temp2 = dmax(r__2,r__3);
/* L240: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[6] = temp2 / dmax(r__1,r__2);

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
		s_copy(srnamc_1.srnamt, "SSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		sstevr_("V", "A", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m, &wa1[1], &z__[z_offset], ldu, &iwork[1], &
			work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___57.ciunit = *nounit;
		    s_wsfe(&io___57);
		    do_fio(&c__1, "SSTEVR(V,A)", (ftnlen)11);
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
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
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
		sstt21_(&n, &c__0, &d3[1], &d4[1], &wa1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[7]);

		ntest = 9;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L300: */
		}
		s_copy(srnamc_1.srnamt, "SSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		sstevr_("N", "A", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___58.ciunit = *nounit;
		    s_wsfe(&io___58);
		    do_fio(&c__1, "SSTEVR(N,A)", (ftnlen)11);
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = wa2[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = eveigs[j], dabs(
			    r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = wa2[j] - eveigs[j], dabs(
			    r__1));
		    temp2 = dmax(r__2,r__3);
/* L310: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[9] = temp2 / dmax(r__1,r__2);

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
		s_copy(srnamc_1.srnamt, "SSTEVX", (ftnlen)6, (ftnlen)6);
		sstevx_("V", "I", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___59.ciunit = *nounit;
		    s_wsfe(&io___59);
		    do_fio(&c__1, "SSTEVX(V,I)", (ftnlen)11);
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
		sstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[10]);


		ntest = 12;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L370: */
		}
		s_copy(srnamc_1.srnamt, "SSTEVX", (ftnlen)6, (ftnlen)6);
		sstevx_("N", "I", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___61.ciunit = *nounit;
		    s_wsfe(&io___61);
		    do_fio(&c__1, "SSTEVX(N,I)", (ftnlen)11);
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * temp3;
		result[12] = (temp1 + temp2) / dmax(r__1,r__2);

L380:

		ntest = 12;
		if (n > 0) {
		    if (il != 1) {
/* Computing MAX */
			r__1 = (wa1[il] - wa1[il - 1]) * .5f, r__2 = ulp * 
				10.f * temp3, r__1 = max(r__1,r__2), r__2 = 
				rtunfl * 10.f;
			vl = wa1[il] - dmax(r__1,r__2);
		    } else {
/* Computing MAX */
			r__1 = (wa1[n] - wa1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vl = wa1[1] - dmax(r__1,r__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			r__1 = (wa1[iu + 1] - wa1[iu]) * .5f, r__2 = ulp * 
				10.f * temp3, r__1 = max(r__1,r__2), r__2 = 
				rtunfl * 10.f;
			vu = wa1[iu] + dmax(r__1,r__2);
		    } else {
/* Computing MAX */
			r__1 = (wa1[n] - wa1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vu = wa1[n] + dmax(r__1,r__2);
		    }
		} else {
		    vl = 0.f;
		    vu = 1.f;
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
		s_copy(srnamc_1.srnamt, "SSTEVX", (ftnlen)6, (ftnlen)6);
		sstevx_("V", "V", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___62.ciunit = *nounit;
		    s_wsfe(&io___62);
		    do_fio(&c__1, "SSTEVX(V,V)", (ftnlen)11);
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
		sstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[13]);

		ntest = 15;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L430: */
		}
		s_copy(srnamc_1.srnamt, "SSTEVX", (ftnlen)6, (ftnlen)6);
		sstevx_("N", "V", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &work[1], &
			iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___63.ciunit = *nounit;
		    s_wsfe(&io___63);
		    do_fio(&c__1, "SSTEVX(N,V)", (ftnlen)11);
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[15] = (temp1 + temp2) / dmax(r__1,r__2);

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
		s_copy(srnamc_1.srnamt, "SSTEVD", (ftnlen)6, (ftnlen)6);
		sstevd_("V", &n, &d1[1], &d2[1], &z__[z_offset], ldu, &work[1]
, &lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___64.ciunit = *nounit;
		    s_wsfe(&io___64);
		    do_fio(&c__1, "SSTEVD(V)", (ftnlen)9);
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
		sstt21_(&n, &c__0, &d3[1], &d4[1], &d1[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &result[16]);

		ntest = 18;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L490: */
		}
		s_copy(srnamc_1.srnamt, "SSTEVD", (ftnlen)6, (ftnlen)6);
		sstevd_("N", &n, &d3[1], &d4[1], &z__[z_offset], ldu, &work[1]
, &lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___65.ciunit = *nounit;
		    s_wsfe(&io___65);
		    do_fio(&c__1, "SSTEVD(N)", (ftnlen)9);
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = eveigs[j], dabs(r__1)), r__3 
			    = max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2)
			    );
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = eveigs[j] - d3[j], dabs(r__1)
			    );
		    temp2 = dmax(r__2,r__3);
/* L500: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[18] = temp2 / dmax(r__1,r__2);

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
		s_copy(srnamc_1.srnamt, "SSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		sstevr_("V", "I", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___66.ciunit = *nounit;
		    s_wsfe(&io___66);
		    do_fio(&c__1, "SSTEVR(V,I)", (ftnlen)11);
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
		sstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[19]);


		ntest = 21;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L560: */
		}
		s_copy(srnamc_1.srnamt, "SSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		sstevr_("N", "I", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___67.ciunit = *nounit;
		    s_wsfe(&io___67);
		    do_fio(&c__1, "SSTEVR(N,I)", (ftnlen)11);
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * temp3;
		result[21] = (temp1 + temp2) / dmax(r__1,r__2);

L570:

		ntest = 21;
		if (n > 0) {
		    if (il != 1) {
/* Computing MAX */
			r__1 = (wa1[il] - wa1[il - 1]) * .5f, r__2 = ulp * 
				10.f * temp3, r__1 = max(r__1,r__2), r__2 = 
				rtunfl * 10.f;
			vl = wa1[il] - dmax(r__1,r__2);
		    } else {
/* Computing MAX */
			r__1 = (wa1[n] - wa1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vl = wa1[1] - dmax(r__1,r__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			r__1 = (wa1[iu + 1] - wa1[iu]) * .5f, r__2 = ulp * 
				10.f * temp3, r__1 = max(r__1,r__2), r__2 = 
				rtunfl * 10.f;
			vu = wa1[iu] + dmax(r__1,r__2);
		    } else {
/* Computing MAX */
			r__1 = (wa1[n] - wa1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vu = wa1[n] + dmax(r__1,r__2);
		    }
		} else {
		    vl = 0.f;
		    vu = 1.f;
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
		s_copy(srnamc_1.srnamt, "SSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		sstevr_("V", "V", &n, &d1[1], &d2[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___68.ciunit = *nounit;
		    s_wsfe(&io___68);
		    do_fio(&c__1, "SSTEVR(V,V)", (ftnlen)11);
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
		sstt22_(&n, &m2, &c__0, &d3[1], &d4[1], &wa2[1], &d2[1], &z__[
			z_offset], ldu, &work[1], &i__3, &result[22]);

		ntest = 24;
		i__3 = n - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    d4[i__] = a[i__ + 1 + i__ * a_dim1];
/* L620: */
		}
		s_copy(srnamc_1.srnamt, "SSTEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		sstevr_("N", "V", &n, &d3[1], &d4[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &iwork[1], 
			&work[1], lwork, &iwork[(n << 1) + 1], &i__3, &iinfo);
		if (iinfo != 0) {
		    io___69.ciunit = *nounit;
		    s_wsfe(&io___69);
		    do_fio(&c__1, "SSTEVR(N,V)", (ftnlen)11);
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[24] = (temp1 + temp2) / dmax(r__1,r__2);

L630:



		;
	    } else {

		for (i__ = 1; i__ <= 24; ++i__) {
		    result[i__] = 0.f;
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

/*              4)      Call SSYEV and SSYEVX. */

		slacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

		++ntest;
		s_copy(srnamc_1.srnamt, "SSYEV", (ftnlen)6, (ftnlen)5);
		ssyev_("V", uplo, &n, &a[a_offset], ldu, &d1[1], &work[1], 
			lwork, &iinfo);
		if (iinfo != 0) {
		    io___72.ciunit = *nounit;
		    s_wsfe(&io___72);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "SSYEV(V,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &v[v_offset], ldu, &d1[1], &
			d2[1], &a[a_offset], ldu, &z__[z_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "SSYEV", (ftnlen)6, (ftnlen)5);
		ssyev_("N", uplo, &n, &a[a_offset], ldu, &d3[1], &work[1], 
			lwork, &iinfo);
		if (iinfo != 0) {
		    io___73.ciunit = *nounit;
		    s_wsfe(&io___73);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "SSYEV(N,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = d1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = d1[j] - d3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L650: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

L660:
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		++ntest;

		if (n > 0) {
/* Computing MAX */
		    r__2 = dabs(d1[1]), r__3 = (r__1 = d1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		    if (il != 1) {
/* Computing MAX */
			r__1 = (d1[il] - d1[il - 1]) * .5f, r__2 = ulp * 10.f 
				* temp3, r__1 = max(r__1,r__2), r__2 = rtunfl 
				* 10.f;
			vl = d1[il] - dmax(r__1,r__2);
		    } else if (n > 0) {
/* Computing MAX */
			r__1 = (d1[n] - d1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vl = d1[1] - dmax(r__1,r__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			r__1 = (d1[iu + 1] - d1[iu]) * .5f, r__2 = ulp * 10.f 
				* temp3, r__1 = max(r__1,r__2), r__2 = rtunfl 
				* 10.f;
			vu = d1[iu] + dmax(r__1,r__2);
		    } else if (n > 0) {
/* Computing MAX */
			r__1 = (d1[n] - d1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vu = d1[n] + dmax(r__1,r__2);
		    }
		} else {
		    temp3 = 0.f;
		    vl = 0.f;
		    vu = 1.f;
		}

		s_copy(srnamc_1.srnamt, "SSYEVX", (ftnlen)6, (ftnlen)6);
		ssyevx_("V", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m, &wa1[1], &z__[z_offset], ldu, &work[
			1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___74.ciunit = *nounit;
		    s_wsfe(&io___74);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVX(V,A,";
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

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &d1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "SSYEVX", (ftnlen)6, (ftnlen)6);
		ssyevx_("N", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___75.ciunit = *nounit;
		    s_wsfe(&io___75);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVX(N,A,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = wa1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = wa2[j], dabs(r__2))
			    ;
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = wa1[j] - wa2[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L670: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

L680:

		++ntest;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVX", (ftnlen)6, (ftnlen)6);
		ssyevx_("V", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___76.ciunit = *nounit;
		    s_wsfe(&io___76);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVX(V,I,";
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

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVX", (ftnlen)6, (ftnlen)6);
		ssyevx_("N", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___77.ciunit = *nounit;
		    s_wsfe(&io___77);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVX(N,I,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * temp3;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);
L690:

		++ntest;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVX", (ftnlen)6, (ftnlen)6);
		ssyevx_("V", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___78.ciunit = *nounit;
		    s_wsfe(&io___78);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVX(V,V,";
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

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVX", (ftnlen)6, (ftnlen)6);
		ssyevx_("N", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			work[1], lwork, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___79.ciunit = *nounit;
		    s_wsfe(&io___79);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVX(N,V,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
		}
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);

L700:

/*              5)      Call SSPEV and SSPEVX. */

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

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
		s_copy(srnamc_1.srnamt, "SSPEV", (ftnlen)6, (ftnlen)5);
		sspev_("V", uplo, &n, &work[1], &d1[1], &z__[z_offset], ldu, &
			v[v_offset], &iinfo);
		if (iinfo != 0) {
		    io___81.ciunit = *nounit;
		    s_wsfe(&io___81);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "SSPEV(V,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
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
		s_copy(srnamc_1.srnamt, "SSPEV", (ftnlen)6, (ftnlen)5);
		sspev_("N", uplo, &n, &work[1], &d3[1], &z__[z_offset], ldu, &
			v[v_offset], &iinfo);
		if (iinfo != 0) {
		    io___82.ciunit = *nounit;
		    s_wsfe(&io___82);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "SSPEV(N,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = d1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = d1[j] - d3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L790: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

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
		    r__2 = dabs(d1[1]), r__3 = (r__1 = d1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		    if (il != 1) {
/* Computing MAX */
			r__1 = (d1[il] - d1[il - 1]) * .5f, r__2 = ulp * 10.f 
				* temp3, r__1 = max(r__1,r__2), r__2 = rtunfl 
				* 10.f;
			vl = d1[il] - dmax(r__1,r__2);
		    } else if (n > 0) {
/* Computing MAX */
			r__1 = (d1[n] - d1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vl = d1[1] - dmax(r__1,r__2);
		    }
		    if (iu != n) {
/* Computing MAX */
			r__1 = (d1[iu + 1] - d1[iu]) * .5f, r__2 = ulp * 10.f 
				* temp3, r__1 = max(r__1,r__2), r__2 = rtunfl 
				* 10.f;
			vu = d1[iu] + dmax(r__1,r__2);
		    } else if (n > 0) {
/* Computing MAX */
			r__1 = (d1[n] - d1[1]) * .5f, r__2 = ulp * 10.f * 
				temp3, r__1 = max(r__1,r__2), r__2 = rtunfl * 
				10.f;
			vu = d1[n] + dmax(r__1,r__2);
		    }
		} else {
		    temp3 = 0.f;
		    vl = 0.f;
		    vu = 1.f;
		}

		s_copy(srnamc_1.srnamt, "SSPEVX", (ftnlen)6, (ftnlen)6);
		sspevx_("V", "A", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m, &wa1[1], &z__[z_offset], ldu, &v[v_offset]
, &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___83.ciunit = *nounit;
		    s_wsfe(&io___83);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSPEVX(V,A,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
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

		s_copy(srnamc_1.srnamt, "SSPEVX", (ftnlen)6, (ftnlen)6);
		sspevx_("N", "A", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___84.ciunit = *nounit;
		    s_wsfe(&io___84);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSPEVX(N,A,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = wa1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = wa2[j], dabs(r__2))
			    ;
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = wa1[j] - wa2[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L890: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

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

		s_copy(srnamc_1.srnamt, "SSPEVX", (ftnlen)6, (ftnlen)6);
		sspevx_("V", "I", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___85.ciunit = *nounit;
		    s_wsfe(&io___85);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSPEVX(V,I,";
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

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
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

		s_copy(srnamc_1.srnamt, "SSPEVX", (ftnlen)6, (ftnlen)6);
		sspevx_("N", "I", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___86.ciunit = *nounit;
		    s_wsfe(&io___86);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSPEVX(N,I,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
		}
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);

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

		s_copy(srnamc_1.srnamt, "SSPEVX", (ftnlen)6, (ftnlen)6);
		sspevx_("V", "V", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m2, &wa2[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___87.ciunit = *nounit;
		    s_wsfe(&io___87);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSPEVX(V,V,";
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

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
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

		s_copy(srnamc_1.srnamt, "SSPEVX", (ftnlen)6, (ftnlen)6);
		sspevx_("N", "V", uplo, &n, &work[1], &vl, &vu, &il, &iu, &
			abstol, &m3, &wa3[1], &z__[z_offset], ldu, &v[
			v_offset], &iwork[1], &iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___88.ciunit = *nounit;
		    s_wsfe(&io___88);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSPEVX(N,V,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
		}
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);

L1080:

/*              6)      Call SSBEV and SSBEVX. */

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
		s_copy(srnamc_1.srnamt, "SSBEV", (ftnlen)6, (ftnlen)5);
		ssbev_("V", uplo, &n, &kd, &v[v_offset], ldu, &d1[1], &z__[
			z_offset], ldu, &work[1], &iinfo);
		if (iinfo != 0) {
		    io___90.ciunit = *nounit;
		    s_wsfe(&io___90);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "SSBEV(V,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
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
		s_copy(srnamc_1.srnamt, "SSBEV", (ftnlen)6, (ftnlen)5);
		ssbev_("N", uplo, &n, &kd, &v[v_offset], ldu, &d3[1], &z__[
			z_offset], ldu, &work[1], &iinfo);
		if (iinfo != 0) {
		    io___91.ciunit = *nounit;
		    s_wsfe(&io___91);
/* Writing concatenation */
		    i__6[0] = 8, a__1[0] = "SSBEV(N,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = d1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = d1[j] - d3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L1170: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

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
		s_copy(srnamc_1.srnamt, "SSBEVX", (ftnlen)6, (ftnlen)6);
		ssbevx_("V", "A", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m, &wa2[
			1], &z__[z_offset], ldu, &work[1], &iwork[1], &iwork[
			n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___92.ciunit = *nounit;
		    s_wsfe(&io___92);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSBEVX(V,A,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa2[1], &
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

		s_copy(srnamc_1.srnamt, "SSBEVX", (ftnlen)6, (ftnlen)6);
		ssbevx_("N", "A", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___93.ciunit = *nounit;
		    s_wsfe(&io___93);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSBEVX(N,A,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = wa2[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = wa3[j], dabs(r__2))
			    ;
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = wa2[j] - wa3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L1270: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

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

		s_copy(srnamc_1.srnamt, "SSBEVX", (ftnlen)6, (ftnlen)6);
		ssbevx_("V", "I", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m2, &
			wa2[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___94.ciunit = *nounit;
		    s_wsfe(&io___94);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSBEVX(V,I,";
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

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
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

		s_copy(srnamc_1.srnamt, "SSBEVX", (ftnlen)6, (ftnlen)6);
		ssbevx_("N", "I", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___95.ciunit = *nounit;
		    s_wsfe(&io___95);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSBEVX(N,I,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
		}
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);

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

		s_copy(srnamc_1.srnamt, "SSBEVX", (ftnlen)6, (ftnlen)6);
		ssbevx_("V", "V", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m2, &
			wa2[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___96.ciunit = *nounit;
		    s_wsfe(&io___96);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSBEVX(V,V,";
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

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
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

		s_copy(srnamc_1.srnamt, "SSBEVX", (ftnlen)6, (ftnlen)6);
		ssbevx_("N", "V", uplo, &n, &kd, &v[v_offset], ldu, &u[
			u_offset], ldu, &vl, &vu, &il, &iu, &abstol, &m3, &
			wa3[1], &z__[z_offset], ldu, &work[1], &iwork[1], &
			iwork[n * 5 + 1], &iinfo);
		if (iinfo != 0) {
		    io___97.ciunit = *nounit;
		    s_wsfe(&io___97);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSBEVX(N,V,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
		}
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);

L1460:

/*              7)      Call SSYEVD */

		slacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);

		++ntest;
		s_copy(srnamc_1.srnamt, "SSYEVD", (ftnlen)6, (ftnlen)6);
		ssyevd_("V", uplo, &n, &a[a_offset], ldu, &d1[1], &work[1], &
			lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___98.ciunit = *nounit;
		    s_wsfe(&io___98);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "SSYEVD(V,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &v[v_offset], ldu, &d1[1], &
			d2[1], &a[a_offset], ldu, &z__[z_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "SSYEVD", (ftnlen)6, (ftnlen)6);
		ssyevd_("N", uplo, &n, &a[a_offset], ldu, &d3[1], &work[1], &
			lwedc, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___99.ciunit = *nounit;
		    s_wsfe(&io___99);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "SSYEVD(N,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = d1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = d1[j] - d3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L1470: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

L1480:

/*              8)      Call SSPEVD. */

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

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
		s_copy(srnamc_1.srnamt, "SSPEVD", (ftnlen)6, (ftnlen)6);
		i__3 = lwedc - indx + 1;
		sspevd_("V", uplo, &n, &work[1], &d1[1], &z__[z_offset], ldu, 
			&work[indx], &i__3, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___100.ciunit = *nounit;
		    s_wsfe(&io___100);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "SSPEVD(V,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
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
		s_copy(srnamc_1.srnamt, "SSPEVD", (ftnlen)6, (ftnlen)6);
		i__3 = lwedc - indx + 1;
		sspevd_("N", uplo, &n, &work[1], &d3[1], &z__[z_offset], ldu, 
			&work[indx], &i__3, &iwork[1], &liwedc, &iinfo);
		if (iinfo != 0) {
		    io___101.ciunit = *nounit;
		    s_wsfe(&io___101);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "SSPEVD(N,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = d1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = d1[j] - d3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L1570: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);
L1580:

/*              9)      Call SSBEVD. */

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
		s_copy(srnamc_1.srnamt, "SSBEVD", (ftnlen)6, (ftnlen)6);
		ssbevd_("V", uplo, &n, &kd, &v[v_offset], ldu, &d1[1], &z__[
			z_offset], ldu, &work[1], &lwedc, &iwork[1], &liwedc, 
			&iinfo);
		if (iinfo != 0) {
		    io___102.ciunit = *nounit;
		    s_wsfe(&io___102);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "SSBEVD(V,";
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

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], lda, &d1[1], &
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
		s_copy(srnamc_1.srnamt, "SSBEVD", (ftnlen)6, (ftnlen)6);
		ssbevd_("N", uplo, &n, &kd, &v[v_offset], ldu, &d3[1], &z__[
			z_offset], ldu, &work[1], &lwedc, &iwork[1], &liwedc, 
			&iinfo);
		if (iinfo != 0) {
		    io___103.ciunit = *nounit;
		    s_wsfe(&io___103);
/* Writing concatenation */
		    i__6[0] = 9, a__1[0] = "SSBEVD(N,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = d1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = d3[j], dabs(r__2));
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = d1[j] - d3[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L1670: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

L1680:


		slacpy_(" ", &n, &n, &a[a_offset], lda, &v[v_offset], ldu);
		++ntest;
		s_copy(srnamc_1.srnamt, "SSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		ssyevr_("V", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m, &wa1[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___104.ciunit = *nounit;
		    s_wsfe(&io___104);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVR(V,A,";
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

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ssyt21_(&c__1, uplo, &n, &c__0, &a[a_offset], ldu, &wa1[1], &
			d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &tau[1]
, &work[1], &result[ntest]);

		ntest += 2;
		s_copy(srnamc_1.srnamt, "SSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		ssyevr_("N", "A", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___105.ciunit = *nounit;
		    s_wsfe(&io___105);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVR(N,A,";
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

		temp1 = 0.f;
		temp2 = 0.f;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    r__3 = temp1, r__4 = (r__1 = wa1[j], dabs(r__1)), r__3 = 
			    max(r__3,r__4), r__4 = (r__2 = wa2[j], dabs(r__2))
			    ;
		    temp1 = dmax(r__3,r__4);
/* Computing MAX */
		    r__2 = temp2, r__3 = (r__1 = wa1[j] - wa2[j], dabs(r__1));
		    temp2 = dmax(r__2,r__3);
/* L1690: */
		}
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
		result[ntest] = temp2 / dmax(r__1,r__2);

L1700:

		++ntest;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		ssyevr_("V", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___106.ciunit = *nounit;
		    s_wsfe(&io___106);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVR(V,I,";
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

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		ssyevr_("N", "I", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___107.ciunit = *nounit;
		    s_wsfe(&io___107);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVR(N,I,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
/* Computing MAX */
		r__1 = unfl, r__2 = ulp * temp3;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);
L1710:

		++ntest;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		ssyevr_("V", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m2, &wa2[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___108.ciunit = *nounit;
		    s_wsfe(&io___108);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVR(V,V,";
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

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

		ssyt22_(&c__1, uplo, &n, &m2, &c__0, &a[a_offset], ldu, &wa2[
			1], &d2[1], &z__[z_offset], ldu, &v[v_offset], ldu, &
			tau[1], &work[1], &result[ntest]);

		ntest += 2;
		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);
		s_copy(srnamc_1.srnamt, "SSYEVR", (ftnlen)6, (ftnlen)6);
		i__3 = *liwork - (n << 1);
		ssyevr_("N", "V", uplo, &n, &a[a_offset], ldu, &vl, &vu, &il, 
			&iu, &abstol, &m3, &wa3[1], &z__[z_offset], ldu, &
			iwork[1], &work[1], lwork, &iwork[(n << 1) + 1], &
			i__3, &iinfo);
		if (iinfo != 0) {
		    io___109.ciunit = *nounit;
		    s_wsfe(&io___109);
/* Writing concatenation */
		    i__6[0] = 11, a__1[0] = "SSYEVR(N,V,";
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

		temp1 = ssxt1_(&c__1, &wa2[1], &m2, &wa3[1], &m3, &abstol, &
			ulp, &unfl);
		temp2 = ssxt1_(&c__1, &wa3[1], &m3, &wa2[1], &m2, &abstol, &
			ulp, &unfl);
		if (n > 0) {
/* Computing MAX */
		    r__2 = dabs(wa1[1]), r__3 = (r__1 = wa1[n], dabs(r__1));
		    temp3 = dmax(r__2,r__3);
		} else {
		    temp3 = 0.f;
		}
/* Computing MAX */
		r__1 = unfl, r__2 = temp3 * ulp;
		result[ntest] = (temp1 + temp2) / dmax(r__1,r__2);

		slacpy_(" ", &n, &n, &v[v_offset], ldu, &a[a_offset], lda);

/* L1720: */
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

	    ntestt += ntest;

	    slafts_("SST", &n, &n, &jtype, &ntest, &result[1], ioldsd, thresh, 
		     nounit, &nerrs);

L1730:
	    ;
	}
/* L1740: */
    }

/*     Summary */

    alasvm_("SST", nounit, &nerrs, &ntestt, &c__0);


    return 0;

/*     End of SDRVST */

} /* sdrvst_ */
