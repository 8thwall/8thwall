#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__1 = 1;
static real c_b27 = 1.f;
static integer c__0 = 0;
static real c_b33 = 0.f;
static integer c__4 = 4;
static integer c__6 = 6;

/* Subroutine */ int cchkhs_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, real *thresh, integer *nounit, 
	complex *a, integer *lda, complex *h__, complex *t1, complex *t2, 
	complex *u, integer *ldu, complex *z__, complex *uz, complex *w1, 
	complex *w3, complex *evectl, complex *evectr, complex *evecty, 
	complex *evectx, complex *uu, complex *tau, complex *work, integer *
	nwork, real *rwork, integer *iwork, logical *select, real *result, 
	integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,3,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,9,9,9 };
    static integer kmagn[21] = { 1,1,1,1,1,1,2,3,1,1,1,1,1,1,1,1,2,3,1,2,3 };
    static integer kmode[21] = { 0,0,0,4,3,1,4,4,4,3,1,5,4,3,1,5,5,5,4,3,1 };
    static integer kconds[21] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,0,0,0 };

    /* Format strings */
    static char fmt_9999[] = "(\002 CCHKHS: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9998[] = "(\002 CCHKHS: \002,a,\002 Eigenvectors from"
	    " \002,a,\002 incorrectly \002,\002normalized.\002,/\002 Bits of "
	    "error=\002,0p,g10.3,\002,\002,9x,\002N=\002,i6,\002, JTYPE=\002,"
	    "i6,\002, ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9997[] = "(\002 CCHKHS: Selected \002,a,\002 Eigenvector"
	    "s from \002,a,\002 do not match other eigenvectors \002,9x,\002N="
	    "\002,i6,\002, JTYPE=\002,i6,\002, ISEED=(\002,3(i5,\002,\002),i5,"
	    "\002)\002)";

    /* System generated locals */
    integer a_dim1, a_offset, evectl_dim1, evectl_offset, evectr_dim1, 
	    evectr_offset, evectx_dim1, evectx_offset, evecty_dim1, 
	    evecty_offset, h_dim1, h_offset, t1_dim1, t1_offset, t2_dim1, 
	    t2_offset, u_dim1, u_offset, uu_dim1, uu_offset, uz_dim1, 
	    uz_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4, i__5, i__6;
    real r__1, r__2;
    complex q__1;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double c_abs(complex *);

    /* Local variables */
    integer i__, j, k, n, n1, jj, in, ihi, ilo;
    real ulp, cond;
    integer jcol, nmax;
    real unfl, ovfl, temp1, temp2;
    logical badnn;
    extern /* Subroutine */ int cget10_(integer *, integer *, complex *, 
	    integer *, complex *, integer *, complex *, real *, real *), 
	    cget22_(char *, char *, char *, integer *, complex *, integer *, 
	    complex *, integer *, complex *, complex *, real *, real *), cgemm_(char *, char *, integer *, 
	    integer *, integer *, complex *, complex *, integer *, complex *, 
	    integer *, complex *, complex *, integer *);
    logical match;
    integer imode;
    extern /* Subroutine */ int chst01_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *);
    real dumma[4];
    integer iinfo;
    real conds, aninv, anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *);
    integer nmats, jsize, nerrs, itype, jtype, ntest;
    real rtulp;
    extern /* Subroutine */ int slabad_(real *, real *), cgehrd_(integer *, 
	    integer *, integer *, complex *, integer *, complex *, complex *, 
	    integer *, integer *), clatme_(integer *, char *, integer *, 
	    complex *, integer *, real *, complex *, char *, char *, char *, 
	    char *, real *, integer *, real *, integer *, integer *, real *, 
	    complex *, integer *, complex *, integer *);
    complex cdumma[4];
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int chsein_(char *, char *, char *, logical *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *, integer *, complex *, real *, 
	    integer *, integer *, integer *), clacpy_(
	    char *, integer *, integer *, complex *, integer *, complex *, 
	    integer *);
    integer idumma[1];
    extern /* Subroutine */ int claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);
    integer ioldsd[4];
    extern /* Subroutine */ int xerbla_(char *, integer *), clatmr_(
	    integer *, integer *, char *, integer *, char *, complex *, 
	    integer *, real *, complex *, char *, char *, complex *, integer *
, real *, complex *, integer *, real *, char *, integer *, 
	    integer *, integer *, real *, real *, char *, complex *, integer *
, integer *, integer *), clatms_(integer *, integer *, char *, integer *, char *, 
	    real *, integer *, real *, real *, integer *, integer *, char *, 
	    complex *, integer *, complex *, integer *), chseqr_(char *, char *, integer *, integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, complex *, 
	    integer *, integer *), ctrevc_(char *, char *, 
	    logical *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, integer *, integer *, complex *, real *, 
	    integer *), cunghr_(integer *, integer *, integer 
	    *, complex *, integer *, complex *, complex *, integer *, integer 
	    *), cunmhr_(char *, char *, integer *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *), slafts_(char *, 
	    integer *, integer *, integer *, integer *, real *, integer *, 
	    real *, integer *, integer *), slasum_(char *, integer *, 
	    integer *, integer *);
    real rtunfl, rtovfl, rtulpi, ulpinv;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___35 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     February 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     CCHKHS  checks the nonsymmetric eigenvalue problem routines. */

/*             CGEHRD factors A as  U H U' , where ' means conjugate */
/*             transpose, H is hessenberg, and U is unitary. */

/*             CUNGHR generates the unitary matrix U. */

/*             CUNMHR multiplies a matrix by the unitary matrix U. */

/*             CHSEQR factors H as  Z T Z' , where Z is unitary and T */
/*             is upper triangular.  It also computes the eigenvalues, */
/*             w(1), ..., w(n); we define a diagonal matrix W whose */
/*             (diagonal) entries are the eigenvalues. */

/*             CTREVC computes the left eigenvector matrix L and the */
/*             right eigenvector matrix R for the matrix T.  The */
/*             columns of L are the complex conjugates of the left */
/*             eigenvectors of T.  The columns of R are the right */
/*             eigenvectors of T.  L is lower triangular, and R is */
/*             upper triangular. */

/*             CHSEIN computes the left eigenvector matrix Y and the */
/*             right eigenvector matrix X for the matrix H.  The */
/*             columns of Y are the complex conjugates of the left */
/*             eigenvectors of H.  The columns of X are the right */
/*             eigenvectors of H.  Y is lower triangular, and X is */
/*             upper triangular. */

/*     When CCHKHS is called, a number of matrix "sizes" ("n's") and a */
/*     number of matrix "types" are specified.  For each size ("n") */
/*     and each type of matrix, one matrix will be generated and used */
/*     to test the nonsymmetric eigenroutines.  For each matrix, 14 */
/*     tests will be performed: */

/*     (1)     | A - U H U**H | / ( |A| n ulp ) */

/*     (2)     | I - UU**H | / ( n ulp ) */

/*     (3)     | H - Z T Z**H | / ( |H| n ulp ) */

/*     (4)     | I - ZZ**H | / ( n ulp ) */

/*     (5)     | A - UZ H (UZ)**H | / ( |A| n ulp ) */

/*     (6)     | I - UZ (UZ)**H | / ( n ulp ) */

/*     (7)     | T(Z computed) - T(Z not computed) | / ( |T| ulp ) */

/*     (8)     | W(Z computed) - W(Z not computed) | / ( |W| ulp ) */

/*     (9)     | TR - RW | / ( |T| |R| ulp ) */

/*     (10)    | L**H T - W**H L | / ( |T| |L| ulp ) */

/*     (11)    | HX - XW | / ( |H| |X| ulp ) */

/*     (12)    | Y**H H - W**H Y | / ( |H| |Y| ulp ) */

/*     (13)    | AX - XW | / ( |A| |X| ulp ) */

/*     (14)    | Y**H A - W**H Y | / ( |A| |Y| ulp ) */

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

/*     (7)  Same as (4), but multiplied by SQRT( overflow threshold ) */
/*     (8)  Same as (4), but multiplied by SQRT( underflow threshold ) */

/*     (9)  A matrix of the form  U' T U, where U is unitary and */
/*          T has evenly spaced entries 1, ..., ULP with random complex */
/*          angles on the diagonal and random O(1) entries in the upper */
/*          triangle. */

/*     (10) A matrix of the form  U' T U, where U is unitary and */
/*          T has geometrically spaced entries 1, ..., ULP with random */
/*          complex angles on the diagonal and random O(1) entries in */
/*          the upper triangle. */

/*     (11) A matrix of the form  U' T U, where U is unitary and */
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
/*          from   ULP < |z| < 1   and random O(1) entries in the upper */
/*          triangle. */

/*     (17) Same as (16), but multiplied by SQRT( overflow threshold ) */
/*     (18) Same as (16), but multiplied by SQRT( underflow threshold ) */

/*     (19) Nonsymmetric matrix with random entries chosen from |z| < 1 */
/*     (20) Same as (19), but multiplied by SQRT( overflow threshold ) */
/*     (21) Same as (19), but multiplied by SQRT( underflow threshold ) */

/*  Arguments */
/*  ========== */

/*  NSIZES - INTEGER */
/*           The number of sizes of matrices to use.  If it is zero, */
/*           CCHKHS does nothing.  It must be at least zero. */
/*           Not modified. */

/*  NN     - INTEGER array, dimension (NSIZES) */
/*           An array containing the sizes to be used for the matrices. */
/*           Zero values will be skipped.  The values must be at least */
/*           zero. */
/*           Not modified. */

/*  NTYPES - INTEGER */
/*           The number of elements in DOTYPE.   If it is zero, CCHKHS */
/*           does nothing.  It must be at least zero.  If it is MAXTYP+1 */
/*           and NSIZES is 1, then an additional type, MAXTYP+1 is */
/*           defined, which is to use whatever matrix is in A.  This */
/*           is only useful if DOTYPE(1:MAXTYP) is .FALSE. and */
/*           DOTYPE(MAXTYP+1) is .TRUE. . */
/*           Not modified. */

/*  DOTYPE - LOGICAL array, dimension (NTYPES) */
/*           If DOTYPE(j) is .TRUE., then for each size in NN a */
/*           matrix of that size and of type j will be generated. */
/*           If NTYPES is smaller than the maximum number of types */
/*           defined (PARAMETER MAXTYP), then types NTYPES+1 through */
/*           MAXTYP will not be generated.  If NTYPES is larger */
/*           than MAXTYP, DOTYPE(MAXTYP+1) through DOTYPE(NTYPES) */
/*           will be ignored. */
/*           Not modified. */

/*  ISEED  - INTEGER array, dimension (4) */
/*           On entry ISEED specifies the seed of the random number */
/*           generator. The array elements should be between 0 and 4095; */
/*           if not they will be reduced mod 4096.  Also, ISEED(4) must */
/*           be odd.  The random number generator uses a linear */
/*           congruential sequence limited to small integers, and so */
/*           should produce machine independent random numbers. The */
/*           values of ISEED are changed on exit, and can be used in the */
/*           next call to CCHKHS to continue the same random number */
/*           sequence. */
/*           Modified. */

/*  THRESH - REAL */
/*           A test will count as "failed" if the "error", computed as */
/*           described above, exceeds THRESH.  Note that the error */
/*           is scaled to be O(1), so THRESH should be a reasonably */
/*           small multiple of 1, e.g., 10 or 100.  In particular, */
/*           it should not depend on the precision (single vs. double) */
/*           or the size of the matrix.  It must be at least zero. */
/*           Not modified. */

/*  NOUNIT - INTEGER */
/*           The FORTRAN unit number for printing out error messages */
/*           (e.g., if a routine returns IINFO not equal to 0.) */
/*           Not modified. */

/*  A      - COMPLEX array, dimension (LDA,max(NN)) */
/*           Used to hold the matrix whose eigenvalues are to be */
/*           computed.  On exit, A contains the last matrix actually */
/*           used. */
/*           Modified. */

/*  LDA    - INTEGER */
/*           The leading dimension of A, H, T1 and T2.  It must be at */
/*           least 1 and at least max( NN ). */
/*           Not modified. */

/*  H      - COMPLEX array, dimension (LDA,max(NN)) */
/*           The upper hessenberg matrix computed by CGEHRD.  On exit, */
/*           H contains the Hessenberg form of the matrix in A. */
/*           Modified. */

/*  T1     - COMPLEX array, dimension (LDA,max(NN)) */
/*           The Schur (="quasi-triangular") matrix computed by CHSEQR */
/*           if Z is computed.  On exit, T1 contains the Schur form of */
/*           the matrix in A. */
/*           Modified. */

/*  T2     - COMPLEX array, dimension (LDA,max(NN)) */
/*           The Schur matrix computed by CHSEQR when Z is not computed. */
/*           This should be identical to T1. */
/*           Modified. */

/*  LDU    - INTEGER */
/*           The leading dimension of U, Z, UZ and UU.  It must be at */
/*           least 1 and at least max( NN ). */
/*           Not modified. */

/*  U      - COMPLEX array, dimension (LDU,max(NN)) */
/*           The unitary matrix computed by CGEHRD. */
/*           Modified. */

/*  Z      - COMPLEX array, dimension (LDU,max(NN)) */
/*           The unitary matrix computed by CHSEQR. */
/*           Modified. */

/*  UZ     - COMPLEX array, dimension (LDU,max(NN)) */
/*           The product of U times Z. */
/*           Modified. */

/*  W1     - COMPLEX array, dimension (max(NN)) */
/*           The eigenvalues of A, as computed by a full Schur */
/*           decomposition H = Z T Z'.  On exit, W1 contains the */
/*           eigenvalues of the matrix in A. */
/*           Modified. */

/*  W3     - COMPLEX array, dimension (max(NN)) */
/*           The eigenvalues of A, as computed by a partial Schur */
/*           decomposition (Z not computed, T only computed as much */
/*           as is necessary for determining eigenvalues).  On exit, */
/*           W3 contains the eigenvalues of the matrix in A, possibly */
/*           perturbed by CHSEIN. */
/*           Modified. */

/*  EVECTL - COMPLEX array, dimension (LDU,max(NN)) */
/*           The conjugate transpose of the (upper triangular) left */
/*           eigenvector matrix for the matrix in T1. */
/*           Modified. */

/*  EVECTR - COMPLEX array, dimension (LDU,max(NN)) */
/*           The (upper triangular) right eigenvector matrix for the */
/*           matrix in T1. */
/*           Modified. */

/*  EVECTY - COMPLEX array, dimension (LDU,max(NN)) */
/*           The conjugate transpose of the left eigenvector matrix */
/*           for the matrix in H. */
/*           Modified. */

/*  EVECTX - COMPLEX array, dimension (LDU,max(NN)) */
/*           The right eigenvector matrix for the matrix in H. */
/*           Modified. */

/*  UU     - COMPLEX array, dimension (LDU,max(NN)) */
/*           Details of the unitary matrix computed by CGEHRD. */
/*           Modified. */

/*  TAU    - COMPLEX array, dimension (max(NN)) */
/*           Further details of the unitary matrix computed by CGEHRD. */
/*           Modified. */

/*  WORK   - COMPLEX array, dimension (NWORK) */
/*           Workspace. */
/*           Modified. */

/*  NWORK  - INTEGER */
/*           The number of entries in WORK.  NWORK >= 4*NN(j)*NN(j) + 2. */

/*  RWORK  - REAL array, dimension (max(NN)) */
/*           Workspace.  Could be equivalenced to IWORK, but not SELECT. */
/*           Modified. */

/*  IWORK  - INTEGER array, dimension (max(NN)) */
/*           Workspace. */
/*           Modified. */

/*  SELECT - LOGICAL array, dimension (max(NN)) */
/*           Workspace.  Could be equivalenced to IWORK, but not RWORK. */
/*           Modified. */

/*  RESULT - REAL array, dimension (14) */
/*           The values computed by the fourteen tests described above. */
/*           The values are currently limited to 1/ulp, to avoid */
/*           overflow. */
/*           Modified. */

/*  INFO   - INTEGER */
/*           If 0, then everything ran OK. */
/*            -1: NSIZES < 0 */
/*            -2: Some NN(j) < 0 */
/*            -3: NTYPES < 0 */
/*            -6: THRESH < 0 */
/*            -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*           -14: LDU < 1 or LDU < NMAX. */
/*           -26: NWORK too small. */
/*           If  CLATMR, CLATMS, or CLATME returns an error code, the */
/*               absolute value of it is returned. */
/*           If 1, then CHSEQR could not find all the shifts. */
/*           If 2, then the EISPACK code (for small blocks) failed. */
/*           If >2, then 30*N iterations were not enough to find an */
/*               eigenvalue or to decompose the problem. */
/*           Modified. */

/* ----------------------------------------------------------------------- */

/*     Some Local Variables and Parameters: */
/*     ---- ----- --------- --- ---------- */

/*     ZERO, ONE       Real 0 and 1. */
/*     MAXTYP          The number of types defined. */
/*     MTEST           The number of tests defined: care must be taken */
/*                     that (1) the size of RESULT, (2) the number of */
/*                     tests actually performed, and (3) MTEST agree. */
/*     NTEST           The number of tests performed on this matrix */
/*                     so far.  This should be less than MTEST, and */
/*                     equal to it by the last test.  It will be less */
/*                     if any of the routines being tested indicates */
/*                     that it could not compute the matrices that */
/*                     would be tested. */
/*     NMAX            Largest value in NN. */
/*     NMATS           The number of matrices generated so far. */
/*     NERRS           The number of tests which have exceeded THRESH */
/*                     so far (computed by SLAFTS). */
/*     COND, CONDS, */
/*     IMODE           Values to be passed to the matrix generators. */
/*     ANORM           Norm of A; passed to matrix generators. */

/*     OVFL, UNFL      Overflow and underflow thresholds. */
/*     ULP, ULPINV     Finest relative precision and its inverse. */
/*     RTOVFL, RTUNFL, */
/*     RTULP, RTULPI   Square roots of the previous 4 values. */

/*             The following four arrays decode JTYPE: */
/*     KTYPE(j)        The general type (1-10) for type "j". */
/*     KMODE(j)        The MODE value to be passed to the matrix */
/*                     generator for type "j". */
/*     KMAGN(j)        The order of magnitude ( O(1), */
/*                     O(overflow^(1/2) ), O(underflow^(1/2) ) */
/*     KCONDS(j)       Selects whether CONDS is to be 1 or */
/*                     1/sqrt(ulp).  (0 means irrelevant.) */

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
    t2_dim1 = *lda;
    t2_offset = 1 + t2_dim1;
    t2 -= t2_offset;
    t1_dim1 = *lda;
    t1_offset = 1 + t1_dim1;
    t1 -= t1_offset;
    h_dim1 = *lda;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    uu_dim1 = *ldu;
    uu_offset = 1 + uu_dim1;
    uu -= uu_offset;
    evectx_dim1 = *ldu;
    evectx_offset = 1 + evectx_dim1;
    evectx -= evectx_offset;
    evecty_dim1 = *ldu;
    evecty_offset = 1 + evecty_dim1;
    evecty -= evecty_offset;
    evectr_dim1 = *ldu;
    evectr_offset = 1 + evectr_dim1;
    evectr -= evectr_offset;
    evectl_dim1 = *ldu;
    evectl_offset = 1 + evectl_dim1;
    evectl -= evectl_offset;
    uz_dim1 = *ldu;
    uz_offset = 1 + uz_dim1;
    uz -= uz_offset;
    z_dim1 = *ldu;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --w1;
    --w3;
    --tau;
    --work;
    --rwork;
    --iwork;
    --select;
    --result;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

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
    } else if (*thresh < 0.f) {
	*info = -6;
    } else if (*lda <= 1 || *lda < nmax) {
	*info = -9;
    } else if (*ldu <= 1 || *ldu < nmax) {
	*info = -14;
    } else if ((nmax << 2) * nmax + 2 > *nwork) {
	*info = -26;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CCHKHS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

/*     More important constants */

    unfl = slamch_("Safe minimum");
    ovfl = slamch_("Overflow");
    slabad_(&unfl, &ovfl);
    ulp = slamch_("Epsilon") * slamch_("Base");
    ulpinv = 1.f / ulp;
    rtunfl = sqrt(unfl);
    rtovfl = sqrt(ovfl);
    rtulp = sqrt(ulp);
    rtulpi = 1.f / rtulp;

/*     Loop over sizes, types */

    nerrs = 0;
    nmats = 0;

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	n = nn[jsize];
	n1 = max(1,n);
	aninv = 1.f / (real) n1;

	if (*nsizes != 1) {
	    mtypes = min(21,*ntypes);
	} else {
	    mtypes = min(22,*ntypes);
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L250;
	    }
	    ++nmats;
	    ntest = 0;

/*           Save ISEED in case of an error. */

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

/*           Initialize RESULT */

	    for (j = 1; j <= 14; ++j) {
		result[j] = 0.f;
/* L30: */
	    }

/*           Compute "A" */

/*           Control parameters: */

/*           KMAGN  KCONDS  KMODE        KTYPE */
/*       =1  O(1)   1       clustered 1  zero */
/*       =2  large  large   clustered 2  identity */
/*       =3  small          exponential  Jordan */
/*       =4                 arithmetic   diagonal, (w/ eigenvalues) */
/*       =5                 random log   hermitian, w/ eigenvalues */
/*       =6                 random       general, w/ eigenvalues */
/*       =7                              random diagonal */
/*       =8                              random hermitian */
/*       =9                              random general */
/*       =10                             random triangular */

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
	    anorm = 1.f;
	    goto L70;

L50:
	    anorm = rtovfl * ulp * aninv;
	    goto L70;

L60:
	    anorm = rtunfl * n * ulpinv;
	    goto L70;

L70:

	    claset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);
	    iinfo = 0;
	    cond = ulpinv;

/*           Special Matrices */

	    if (itype == 1) {

/*              Zero */

		iinfo = 0;
	    } else if (itype == 2) {

/*              Identity */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    a[i__4].r = anorm, a[i__4].i = 0.f;
/* L80: */
		}

	    } else if (itype == 3) {

/*              Jordan Block */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    a[i__4].r = anorm, a[i__4].i = 0.f;
		    if (jcol > 1) {
			i__4 = jcol + (jcol - 1) * a_dim1;
			a[i__4].r = 1.f, a[i__4].i = 0.f;
		    }
/* L90: */
		}

	    } else if (itype == 4) {

/*              Diagonal Matrix, [Eigen]values Specified */

		clatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &imode, &cond, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b27, &work[(
			n << 1) + 1], &c__1, &c_b27, "N", idumma, &c__0, &
			c__0, &c_b33, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 5) {

/*              Hermitian, eigenvalues specified */

		clatms_(&n, &n, "D", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &n, &n, "N", &a[a_offset], lda, &work[1], &
			iinfo);

	    } else if (itype == 6) {

/*              General, eigenvalues specified */

		if (kconds[jtype - 1] == 1) {
		    conds = 1.f;
		} else if (kconds[jtype - 1] == 2) {
		    conds = rtulpi;
		} else {
		    conds = 0.f;
		}

		clatme_(&n, "D", &iseed[1], &work[1], &imode, &cond, &c_b2, 
			" ", "T", "T", "T", &rwork[1], &c__4, &conds, &n, &n, 
			&anorm, &a[a_offset], lda, &work[n + 1], &iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b27, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b27, &work[(
			n << 1) + 1], &c__1, &c_b27, "N", idumma, &c__0, &
			c__0, &c_b33, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Hermitian, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "H", &work[1], &c__6, &c_b27, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b27, &work[(
			n << 1) + 1], &c__1, &c_b27, "N", idumma, &n, &n, &
			c_b33, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              General, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b27, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b27, &work[(
			n << 1) + 1], &c__1, &c_b27, "N", idumma, &n, &n, &
			c_b33, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 10) {

/*              Triangular, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b27, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b27, &work[(
			n << 1) + 1], &c__1, &c_b27, "N", idumma, &n, &c__0, &
			c_b33, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else {

		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___35.ciunit = *nounit;
		s_wsfe(&io___35);
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

/*           Call CGEHRD to compute H and U, do tests. */

	    clacpy_(" ", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
	    ntest = 1;

	    ilo = 1;
	    ihi = n;

	    i__3 = *nwork - n;
	    cgehrd_(&n, &ilo, &ihi, &h__[h_offset], lda, &work[1], &work[n + 
		    1], &i__3, &iinfo);

	    if (iinfo != 0) {
		result[1] = ulpinv;
		io___38.ciunit = *nounit;
		s_wsfe(&io___38);
		do_fio(&c__1, "CGEHRD", (ftnlen)6);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L240;
	    }

	    i__3 = n - 1;
	    for (j = 1; j <= i__3; ++j) {
		i__4 = j + 1 + j * uu_dim1;
		uu[i__4].r = 0.f, uu[i__4].i = 0.f;
		i__4 = n;
		for (i__ = j + 2; i__ <= i__4; ++i__) {
		    i__5 = i__ + j * u_dim1;
		    i__6 = i__ + j * h_dim1;
		    u[i__5].r = h__[i__6].r, u[i__5].i = h__[i__6].i;
		    i__5 = i__ + j * uu_dim1;
		    i__6 = i__ + j * h_dim1;
		    uu[i__5].r = h__[i__6].r, uu[i__5].i = h__[i__6].i;
		    i__5 = i__ + j * h_dim1;
		    h__[i__5].r = 0.f, h__[i__5].i = 0.f;
/* L110: */
		}
/* L120: */
	    }
	    i__3 = n - 1;
	    ccopy_(&i__3, &work[1], &c__1, &tau[1], &c__1);
	    i__3 = *nwork - n;
	    cunghr_(&n, &ilo, &ihi, &u[u_offset], ldu, &work[1], &work[n + 1], 
		     &i__3, &iinfo);
	    ntest = 2;

	    chst01_(&n, &ilo, &ihi, &a[a_offset], lda, &h__[h_offset], lda, &
		    u[u_offset], ldu, &work[1], nwork, &rwork[1], &result[1]);

/*           Call CHSEQR to compute T1, T2 and Z, do tests. */

/*           Eigenvalues only (W3) */

	    clacpy_(" ", &n, &n, &h__[h_offset], lda, &t2[t2_offset], lda);
	    ntest = 3;
	    result[3] = ulpinv;

	    chseqr_("E", "N", &n, &ilo, &ihi, &t2[t2_offset], lda, &w3[1], &
		    uz[uz_offset], ldu, &work[1], nwork, &iinfo);
	    if (iinfo != 0) {
		io___40.ciunit = *nounit;
		s_wsfe(&io___40);
		do_fio(&c__1, "CHSEQR(E)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		if (iinfo <= n + 2) {
		    *info = abs(iinfo);
		    goto L240;
		}
	    }

/*           Eigenvalues (W1) and Full Schur Form (T2) */

	    clacpy_(" ", &n, &n, &h__[h_offset], lda, &t2[t2_offset], lda);

	    chseqr_("S", "N", &n, &ilo, &ihi, &t2[t2_offset], lda, &w1[1], &
		    uz[uz_offset], ldu, &work[1], nwork, &iinfo);
	    if (iinfo != 0 && iinfo <= n + 2) {
		io___41.ciunit = *nounit;
		s_wsfe(&io___41);
		do_fio(&c__1, "CHSEQR(S)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L240;
	    }

/*           Eigenvalues (W1), Schur Form (T1), and Schur Vectors (UZ) */

	    clacpy_(" ", &n, &n, &h__[h_offset], lda, &t1[t1_offset], lda);
	    clacpy_(" ", &n, &n, &u[u_offset], ldu, &uz[uz_offset], ldu);

	    chseqr_("S", "V", &n, &ilo, &ihi, &t1[t1_offset], lda, &w1[1], &
		    uz[uz_offset], ldu, &work[1], nwork, &iinfo);
	    if (iinfo != 0 && iinfo <= n + 2) {
		io___42.ciunit = *nounit;
		s_wsfe(&io___42);
		do_fio(&c__1, "CHSEQR(V)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L240;
	    }

/*           Compute Z = U' UZ */

	    cgemm_("C", "N", &n, &n, &n, &c_b2, &u[u_offset], ldu, &uz[
		    uz_offset], ldu, &c_b1, &z__[z_offset], ldu);
	    ntest = 8;

/*           Do Tests 3: | H - Z T Z' | / ( |H| n ulp ) */
/*                and 4: | I - Z Z' | / ( n ulp ) */

	    chst01_(&n, &ilo, &ihi, &h__[h_offset], lda, &t1[t1_offset], lda, 
		    &z__[z_offset], ldu, &work[1], nwork, &rwork[1], &result[
		    3]);

/*           Do Tests 5: | A - UZ T (UZ)' | / ( |A| n ulp ) */
/*                and 6: | I - UZ (UZ)' | / ( n ulp ) */

	    chst01_(&n, &ilo, &ihi, &a[a_offset], lda, &t1[t1_offset], lda, &
		    uz[uz_offset], ldu, &work[1], nwork, &rwork[1], &result[5]
);

/*           Do Test 7: | T2 - T1 | / ( |T| n ulp ) */

	    cget10_(&n, &n, &t2[t2_offset], lda, &t1[t1_offset], lda, &work[1]
, &rwork[1], &result[7]);

/*           Do Test 8: | W3 - W1 | / ( max(|W1|,|W3|) ulp ) */

	    temp1 = 0.f;
	    temp2 = 0.f;
	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		r__1 = temp1, r__2 = c_abs(&w1[j]), r__1 = max(r__1,r__2), 
			r__2 = c_abs(&w3[j]);
		temp1 = dmax(r__1,r__2);
/* Computing MAX */
		i__4 = j;
		i__5 = j;
		q__1.r = w1[i__4].r - w3[i__5].r, q__1.i = w1[i__4].i - w3[
			i__5].i;
		r__1 = temp2, r__2 = c_abs(&q__1);
		temp2 = dmax(r__1,r__2);
/* L130: */
	    }

/* Computing MAX */
	    r__1 = unfl, r__2 = ulp * dmax(temp1,temp2);
	    result[8] = temp2 / dmax(r__1,r__2);

/*           Compute the Left and Right Eigenvectors of T */

/*           Compute the Right eigenvector Matrix: */

	    ntest = 9;
	    result[9] = ulpinv;

/*           Select every other eigenvector */

	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
		select[j] = FALSE_;
/* L140: */
	    }
	    i__3 = n;
	    for (j = 1; j <= i__3; j += 2) {
		select[j] = TRUE_;
/* L150: */
	    }
	    ctrevc_("Right", "All", &select[1], &n, &t1[t1_offset], lda, 
		    cdumma, ldu, &evectr[evectr_offset], ldu, &n, &in, &work[
		    1], &rwork[1], &iinfo);
	    if (iinfo != 0) {
		io___47.ciunit = *nounit;
		s_wsfe(&io___47);
		do_fio(&c__1, "CTREVC(R,A)", (ftnlen)11);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L240;
	    }

/*           Test 9:  | TR - RW | / ( |T| |R| ulp ) */

	    cget22_("N", "N", "N", &n, &t1[t1_offset], lda, &evectr[
		    evectr_offset], ldu, &w1[1], &work[1], &rwork[1], dumma);
	    result[9] = dumma[0];
	    if (dumma[1] > *thresh) {
		io___49.ciunit = *nounit;
		s_wsfe(&io___49);
		do_fio(&c__1, "Right", (ftnlen)5);
		do_fio(&c__1, "CTREVC", (ftnlen)6);
		do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(real));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           Compute selected right eigenvectors and confirm that */
/*           they agree with previous right eigenvectors */

	    ctrevc_("Right", "Some", &select[1], &n, &t1[t1_offset], lda, 
		    cdumma, ldu, &evectl[evectl_offset], ldu, &n, &in, &work[
		    1], &rwork[1], &iinfo);
	    if (iinfo != 0) {
		io___50.ciunit = *nounit;
		s_wsfe(&io___50);
		do_fio(&c__1, "CTREVC(R,S)", (ftnlen)11);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L240;
	    }

	    k = 1;
	    match = TRUE_;
	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
		if (select[j]) {
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			i__5 = jj + j * evectr_dim1;
			i__6 = jj + k * evectl_dim1;
			if (evectr[i__5].r != evectl[i__6].r || evectr[i__5]
				.i != evectl[i__6].i) {
			    match = FALSE_;
			    goto L180;
			}
/* L160: */
		    }
		    ++k;
		}
/* L170: */
	    }
L180:
	    if (! match) {
		io___54.ciunit = *nounit;
		s_wsfe(&io___54);
		do_fio(&c__1, "Right", (ftnlen)5);
		do_fio(&c__1, "CTREVC", (ftnlen)6);
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           Compute the Left eigenvector Matrix: */

	    ntest = 10;
	    result[10] = ulpinv;
	    ctrevc_("Left", "All", &select[1], &n, &t1[t1_offset], lda, &
		    evectl[evectl_offset], ldu, cdumma, ldu, &n, &in, &work[1]
, &rwork[1], &iinfo);
	    if (iinfo != 0) {
		io___55.ciunit = *nounit;
		s_wsfe(&io___55);
		do_fio(&c__1, "CTREVC(L,A)", (ftnlen)11);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L240;
	    }

/*           Test 10:  | LT - WL | / ( |T| |L| ulp ) */

	    cget22_("C", "N", "C", &n, &t1[t1_offset], lda, &evectl[
		    evectl_offset], ldu, &w1[1], &work[1], &rwork[1], &dumma[
		    2]);
	    result[10] = dumma[2];
	    if (dumma[3] > *thresh) {
		io___56.ciunit = *nounit;
		s_wsfe(&io___56);
		do_fio(&c__1, "Left", (ftnlen)4);
		do_fio(&c__1, "CTREVC", (ftnlen)6);
		do_fio(&c__1, (char *)&dumma[3], (ftnlen)sizeof(real));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           Compute selected left eigenvectors and confirm that */
/*           they agree with previous left eigenvectors */

	    ctrevc_("Left", "Some", &select[1], &n, &t1[t1_offset], lda, &
		    evectr[evectr_offset], ldu, cdumma, ldu, &n, &in, &work[1]
, &rwork[1], &iinfo);
	    if (iinfo != 0) {
		io___57.ciunit = *nounit;
		s_wsfe(&io___57);
		do_fio(&c__1, "CTREVC(L,S)", (ftnlen)11);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L240;
	    }

	    k = 1;
	    match = TRUE_;
	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
		if (select[j]) {
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			i__5 = jj + j * evectl_dim1;
			i__6 = jj + k * evectr_dim1;
			if (evectl[i__5].r != evectr[i__6].r || evectl[i__5]
				.i != evectr[i__6].i) {
			    match = FALSE_;
			    goto L210;
			}
/* L190: */
		    }
		    ++k;
		}
/* L200: */
	    }
L210:
	    if (! match) {
		io___58.ciunit = *nounit;
		s_wsfe(&io___58);
		do_fio(&c__1, "Left", (ftnlen)4);
		do_fio(&c__1, "CTREVC", (ftnlen)6);
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           Call CHSEIN for Right eigenvectors of H, do test 11 */

	    ntest = 11;
	    result[11] = ulpinv;
	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
		select[j] = TRUE_;
/* L220: */
	    }

	    chsein_("Right", "Qr", "Ninitv", &select[1], &n, &h__[h_offset], 
		    lda, &w3[1], cdumma, ldu, &evectx[evectx_offset], ldu, &
		    n1, &in, &work[1], &rwork[1], &iwork[1], &iwork[1], &
		    iinfo);
	    if (iinfo != 0) {
		io___59.ciunit = *nounit;
		s_wsfe(&io___59);
		do_fio(&c__1, "CHSEIN(R)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    goto L240;
		}
	    } else {

/*              Test 11:  | HX - XW | / ( |H| |X| ulp ) */

/*                        (from inverse iteration) */

		cget22_("N", "N", "N", &n, &h__[h_offset], lda, &evectx[
			evectx_offset], ldu, &w3[1], &work[1], &rwork[1], 
			dumma);
		if (dumma[0] < ulpinv) {
		    result[11] = dumma[0] * aninv;
		}
		if (dumma[1] > *thresh) {
		    io___60.ciunit = *nounit;
		    s_wsfe(&io___60);
		    do_fio(&c__1, "Right", (ftnlen)5);
		    do_fio(&c__1, "CHSEIN", (ftnlen)6);
		    do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(real));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		}
	    }

/*           Call CHSEIN for Left eigenvectors of H, do test 12 */

	    ntest = 12;
	    result[12] = ulpinv;
	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
		select[j] = TRUE_;
/* L230: */
	    }

	    chsein_("Left", "Qr", "Ninitv", &select[1], &n, &h__[h_offset], 
		    lda, &w3[1], &evecty[evecty_offset], ldu, cdumma, ldu, &
		    n1, &in, &work[1], &rwork[1], &iwork[1], &iwork[1], &
		    iinfo);
	    if (iinfo != 0) {
		io___61.ciunit = *nounit;
		s_wsfe(&io___61);
		do_fio(&c__1, "CHSEIN(L)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    goto L240;
		}
	    } else {

/*              Test 12:  | YH - WY | / ( |H| |Y| ulp ) */

/*                        (from inverse iteration) */

		cget22_("C", "N", "C", &n, &h__[h_offset], lda, &evecty[
			evecty_offset], ldu, &w3[1], &work[1], &rwork[1], &
			dumma[2]);
		if (dumma[2] < ulpinv) {
		    result[12] = dumma[2] * aninv;
		}
		if (dumma[3] > *thresh) {
		    io___62.ciunit = *nounit;
		    s_wsfe(&io___62);
		    do_fio(&c__1, "Left", (ftnlen)4);
		    do_fio(&c__1, "CHSEIN", (ftnlen)6);
		    do_fio(&c__1, (char *)&dumma[3], (ftnlen)sizeof(real));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		}
	    }

/*           Call CUNMHR for Right eigenvectors of A, do test 13 */

	    ntest = 13;
	    result[13] = ulpinv;

	    cunmhr_("Left", "No transpose", &n, &n, &ilo, &ihi, &uu[uu_offset]
, ldu, &tau[1], &evectx[evectx_offset], ldu, &work[1], 
		    nwork, &iinfo);
	    if (iinfo != 0) {
		io___63.ciunit = *nounit;
		s_wsfe(&io___63);
		do_fio(&c__1, "CUNMHR(L)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    goto L240;
		}
	    } else {

/*              Test 13:  | AX - XW | / ( |A| |X| ulp ) */

/*                        (from inverse iteration) */

		cget22_("N", "N", "N", &n, &a[a_offset], lda, &evectx[
			evectx_offset], ldu, &w3[1], &work[1], &rwork[1], 
			dumma);
		if (dumma[0] < ulpinv) {
		    result[13] = dumma[0] * aninv;
		}
	    }

/*           Call CUNMHR for Left eigenvectors of A, do test 14 */

	    ntest = 14;
	    result[14] = ulpinv;

	    cunmhr_("Left", "No transpose", &n, &n, &ilo, &ihi, &uu[uu_offset]
, ldu, &tau[1], &evecty[evecty_offset], ldu, &work[1], 
		    nwork, &iinfo);
	    if (iinfo != 0) {
		io___64.ciunit = *nounit;
		s_wsfe(&io___64);
		do_fio(&c__1, "CUNMHR(L)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		if (iinfo < 0) {
		    goto L240;
		}
	    } else {

/*              Test 14:  | YA - WY | / ( |A| |Y| ulp ) */

/*                        (from inverse iteration) */

		cget22_("C", "N", "C", &n, &a[a_offset], lda, &evecty[
			evecty_offset], ldu, &w3[1], &work[1], &rwork[1], &
			dumma[2]);
		if (dumma[2] < ulpinv) {
		    result[14] = dumma[2] * aninv;
		}
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

L240:

	    ntestt += ntest;
	    slafts_("CHS", &n, &n, &jtype, &ntest, &result[1], ioldsd, thresh, 
		     nounit, &nerrs);

L250:
	    ;
	}
/* L260: */
    }

/*     Summary */

    slasum_("CHS", nounit, &nerrs, &ntestt);

    return 0;


/*     End of CCHKHS */

} /* cchkhs_ */
