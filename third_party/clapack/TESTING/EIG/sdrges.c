#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c_n1 = -1;
static real c_b26 = 0.f;
static integer c__2 = 2;
static real c_b32 = 1.f;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__0 = 0;

/* Subroutine */ int sdrges_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, real *thresh, integer *nounit, real *
	a, integer *lda, real *b, real *s, real *t, real *q, integer *ldq, 
	real *z__, real *alphar, real *alphai, real *beta, real *work, 
	integer *lwork, real *result, logical *bwork, integer *info)
{
    /* Initialized data */

    static integer kclass[26] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,
	    2,2,2,3 };
    static integer kbmagn[26] = { 1,1,1,1,1,1,1,1,3,2,3,2,2,3,1,1,1,1,1,1,1,3,
	    2,3,2,1 };
    static integer ktrian[26] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
	    1,1,1,1 };
    static integer iasign[26] = { 0,0,0,0,0,0,2,0,2,2,0,0,2,2,2,0,2,0,0,0,2,2,
	    2,2,2,0 };
    static integer ibsign[26] = { 0,0,0,0,0,0,0,2,0,0,2,2,0,0,2,0,2,0,0,0,0,0,
	    0,0,0,0 };
    static integer kz1[6] = { 0,1,2,1,3,3 };
    static integer kz2[6] = { 0,0,1,2,1,1 };
    static integer kadd[6] = { 0,0,0,0,3,2 };
    static integer katype[26] = { 0,1,0,1,2,3,4,1,4,4,1,1,4,4,4,2,4,5,8,7,9,4,
	    4,4,4,0 };
    static integer kbtype[26] = { 0,0,1,1,2,-3,1,4,1,1,4,4,1,1,-4,2,-4,8,8,8,
	    8,8,8,8,8,0 };
    static integer kazero[26] = { 1,1,1,1,1,1,2,1,2,2,1,1,2,2,3,1,3,5,5,5,5,3,
	    3,3,3,1 };
    static integer kbzero[26] = { 1,1,1,1,1,1,1,2,1,1,2,2,1,1,4,1,4,6,6,6,6,4,
	    4,4,4,1 };
    static integer kamagn[26] = { 1,1,1,1,1,1,1,1,2,3,2,3,2,3,1,1,1,1,1,1,1,2,
	    3,3,2,1 };

    /* Format strings */
    static char fmt_9999[] = "(\002 SDRGES: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,4(i4,\002,\002),i5,\002)\002)";
    static char fmt_9998[] = "(\002 SDRGES: SGET53 returned INFO=\002,i1,"
	    "\002 for eigenvalue \002,i6,\002.\002,/9x,\002N=\002,i6,\002, JT"
	    "YPE=\002,i6,\002, ISEED=(\002,4(i4,\002,\002),i5,\002)\002)";
    static char fmt_9997[] = "(\002 SDRGES: S not in Schur form at eigenvalu"
	    "e \002,i6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, "
	    "ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9996[] = "(/1x,a3,\002 -- Real Generalized Schur form dr"
	    "iver\002)";
    static char fmt_9995[] = "(\002 Matrix types (see SDRGES for details):"
	    " \002)";
    static char fmt_9994[] = "(\002 Special Matrices:\002,23x,\002(J'=transp"
	    "osed Jordan block)\002,/\002   1=(0,0)  2=(I,0)  3=(0,I)  4=(I,I"
	    ")  5=(J',J')  \002,\0026=(diag(J',I), diag(I,J'))\002,/\002 Diag"
	    "onal Matrices:  ( \002,\002D=diag(0,1,2,...) )\002,/\002   7=(D,"
	    "I)   9=(large*D, small*I\002,\002)  11=(large*I, small*D)  13=(l"
	    "arge*D, large*I)\002,/\002   8=(I,D)  10=(small*D, large*I)  12="
	    "(small*I, large*D) \002,\002 14=(small*D, small*I)\002,/\002  15"
	    "=(D, reversed D)\002)";
    static char fmt_9993[] = "(\002 Matrices Rotated by Random \002,a,\002 M"
	    "atrices U, V:\002,/\002  16=Transposed Jordan Blocks            "
	    " 19=geometric \002,\002alpha, beta=0,1\002,/\002  17=arithm. alp"
	    "ha&beta             \002,\002      20=arithmetic alpha, beta=0,"
	    "1\002,/\002  18=clustered \002,\002alpha, beta=0,1            21"
	    "=random alpha, beta=0,1\002,/\002 Large & Small Matrices:\002,"
	    "/\002  22=(large, small)   \002,\00223=(small,large)    24=(smal"
	    "l,small)    25=(large,large)\002,/\002  26=random O(1) matrices"
	    ".\002)";
    static char fmt_9992[] = "(/\002 Tests performed:  (S is Schur, T is tri"
	    "angular, \002,\002Q and Z are \002,a,\002,\002,/19x,\002l and r "
	    "are the appropriate left and right\002,/19x,\002eigenvectors, re"
	    "sp., a is alpha, b is beta, and\002,/19x,a,\002 means \002,a,"
	    "\002.)\002,/\002 Without ordering: \002,/\002  1 = | A - Q S "
	    "Z\002,a,\002 | / ( |A| n ulp )      2 = | B - Q T Z\002,a,\002 |"
	    " / ( |B| n ulp )\002,/\002  3 = | I - QQ\002,a,\002 | / ( n ulp "
	    ")             4 = | I - ZZ\002,a,\002 | / ( n ulp )\002,/\002  5"
	    " = A is in Schur form S\002,/\002  6 = difference between (alpha"
	    ",beta)\002,\002 and diagonals of (S,T)\002,/\002 With ordering:"
	    " \002,/\002  7 = | (A,B) - Q (S,T) Z\002,a,\002 | / ( |(A,B)| n "
	    "ulp )  \002,/\002  8 = | I - QQ\002,a,\002 | / ( n ulp )        "
	    "    9 = | I - ZZ\002,a,\002 | / ( n ulp )\002,/\002 10 = A is in"
	    " Schur form S\002,/\002 11 = difference between (alpha,beta) and"
	    " diagonals\002,\002 of (S,T)\002,/\002 12 = SDIM is the correct "
	    "number of \002,\002selected eigenvalues\002,/)";
    static char fmt_9991[] = "(\002 Matrix order=\002,i5,\002, type=\002,i2"
	    ",\002, seed=\002,4(i4,\002,\002),\002 result \002,i2,\002 is\002"
	    ",0p,f8.2)";
    static char fmt_9990[] = "(\002 Matrix order=\002,i5,\002, type=\002,i2"
	    ",\002, seed=\002,4(i4,\002,\002),\002 result \002,i2,\002 is\002"
	    ",1p,e10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, s_dim1, 
	    s_offset, t_dim1, t_offset, z_dim1, z_offset, i__1, i__2, i__3, 
	    i__4;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7, r__8, r__9, r__10;

    /* Builtin functions */
    double r_sign(real *, real *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, n, i1, n1, jc, nb, in, jr;
    real ulp;
    integer iadd, sdim, ierr, nmax, rsub;
    char sort[1];
    real temp1, temp2;
    logical badnn;
    integer iinfo;
    real rmagn[4];
    extern /* Subroutine */ int sget51_(integer *, integer *, real *, integer 
	    *, real *, integer *, real *, integer *, real *, integer *, real *
, real *), sgges_(char *, char *, char *, L_fp, integer *, real *, 
	     integer *, real *, integer *, integer *, real *, real *, real *, 
	    real *, integer *, real *, integer *, real *, integer *, logical *
, integer *), sget53_(real *, integer *, 
	    real *, integer *, real *, real *, real *, real *, integer *), 
	    sget54_(integer *, real *, integer *, real *, integer *, real *, 
	    integer *, real *, integer *, real *, integer *, real *, integer *
, real *, real *);
    integer nmats, jsize, nerrs, jtype, ntest, isort;
    extern /* Subroutine */ int slatm4_(integer *, integer *, integer *, 
	    integer *, integer *, real *, real *, real *, integer *, integer *
, real *, integer *);
    logical ilabad;
    extern /* Subroutine */ int sorm2r_(char *, char *, integer *, integer *, 
	    integer *, real *, integer *, real *, real *, integer *, real *, 
	    integer *), slabad_(real *, real *);
    extern doublereal slamch_(char *);
    real safmin;
    integer ioldsd[4];
    real safmax;
    integer knteig;
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *, 
	    integer *, integer *);
    extern doublereal slarnd_(integer *, integer *);
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *), slarfg_(integer *, real *, real *, integer 
	    *, real *), xerbla_(char *, integer *);
    extern logical slctes_(real *, real *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *);
    integer minwrk, maxwrk;
    real ulpinv;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9990, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SDRGES checks the nonsymmetric generalized eigenvalue (Schur form) */
/*  problem driver SGGES. */

/*  SGGES factors A and B as Q S Z'  and Q T Z' , where ' means */
/*  transpose, T is upper triangular, S is in generalized Schur form */
/*  (block upper triangular, with 1x1 and 2x2 blocks on the diagonal, */
/*  the 2x2 blocks corresponding to complex conjugate pairs of */
/*  generalized eigenvalues), and Q and Z are orthogonal. It also */
/*  computes the generalized eigenvalues (alpha(j),beta(j)), j=1,...,n, */
/*  Thus, w(j) = alpha(j)/beta(j) is a root of the characteristic */
/*  equation */
/*                  det( A - w(j) B ) = 0 */
/*  Optionally it also reorder the eigenvalues so that a selected */
/*  cluster of eigenvalues appears in the leading diagonal block of the */
/*  Schur forms. */

/*  When SDRGES is called, a number of matrix "sizes" ("N's") and a */
/*  number of matrix "TYPES" are specified.  For each size ("N") */
/*  and each TYPE of matrix, a pair of matrices (A, B) will be generated */
/*  and used for testing. For each matrix pair, the following 13 tests */
/*  will be performed and compared with the threshhold THRESH except */
/*  the tests (5), (11) and (13). */


/*  (1)   | A - Q S Z' | / ( |A| n ulp ) (no sorting of eigenvalues) */


/*  (2)   | B - Q T Z' | / ( |B| n ulp ) (no sorting of eigenvalues) */


/*  (3)   | I - QQ' | / ( n ulp ) (no sorting of eigenvalues) */


/*  (4)   | I - ZZ' | / ( n ulp ) (no sorting of eigenvalues) */

/*  (5)   if A is in Schur form (i.e. quasi-triangular form) */
/*        (no sorting of eigenvalues) */

/*  (6)   if eigenvalues = diagonal blocks of the Schur form (S, T), */
/*        i.e., test the maximum over j of D(j)  where: */

/*        if alpha(j) is real: */
/*                      |alpha(j) - S(j,j)|        |beta(j) - T(j,j)| */
/*            D(j) = ------------------------ + ----------------------- */
/*                   max(|alpha(j)|,|S(j,j)|)   max(|beta(j)|,|T(j,j)|) */

/*        if alpha(j) is complex: */
/*                                  | det( s S - w T ) | */
/*            D(j) = --------------------------------------------------- */
/*                   ulp max( s norm(S), |w| norm(T) )*norm( s S - w T ) */

/*        and S and T are here the 2 x 2 diagonal blocks of S and T */
/*        corresponding to the j-th and j+1-th eigenvalues. */
/*        (no sorting of eigenvalues) */

/*  (7)   | (A,B) - Q (S,T) Z' | / ( | (A,B) | n ulp ) */
/*             (with sorting of eigenvalues). */

/*  (8)   | I - QQ' | / ( n ulp ) (with sorting of eigenvalues). */

/*  (9)   | I - ZZ' | / ( n ulp ) (with sorting of eigenvalues). */

/*  (10)  if A is in Schur form (i.e. quasi-triangular form) */
/*        (with sorting of eigenvalues). */

/*  (11)  if eigenvalues = diagonal blocks of the Schur form (S, T), */
/*        i.e. test the maximum over j of D(j)  where: */

/*        if alpha(j) is real: */
/*                      |alpha(j) - S(j,j)|        |beta(j) - T(j,j)| */
/*            D(j) = ------------------------ + ----------------------- */
/*                   max(|alpha(j)|,|S(j,j)|)   max(|beta(j)|,|T(j,j)|) */

/*        if alpha(j) is complex: */
/*                                  | det( s S - w T ) | */
/*            D(j) = --------------------------------------------------- */
/*                   ulp max( s norm(S), |w| norm(T) )*norm( s S - w T ) */

/*        and S and T are here the 2 x 2 diagonal blocks of S and T */
/*        corresponding to the j-th and j+1-th eigenvalues. */
/*        (with sorting of eigenvalues). */

/*  (12)  if sorting worked and SDIM is the number of eigenvalues */
/*        which were SELECTed. */

/*  Test Matrices */
/*  ============= */

/*  The sizes of the test matrices are specified by an array */
/*  NN(1:NSIZES); the value of each element NN(j) specifies one size. */
/*  The "types" are specified by a logical array DOTYPE( 1:NTYPES ); if */
/*  DOTYPE(j) is .TRUE., then matrix type "j" will be generated. */
/*  Currently, the list of possible types is: */

/*  (1)  ( 0, 0 )         (a pair of zero matrices) */

/*  (2)  ( I, 0 )         (an identity and a zero matrix) */

/*  (3)  ( 0, I )         (an identity and a zero matrix) */

/*  (4)  ( I, I )         (a pair of identity matrices) */

/*          t   t */
/*  (5)  ( J , J  )       (a pair of transposed Jordan blocks) */

/*                                      t                ( I   0  ) */
/*  (6)  ( X, Y )         where  X = ( J   0  )  and Y = (      t ) */
/*                                   ( 0   I  )          ( 0   J  ) */
/*                        and I is a k x k identity and J a (k+1)x(k+1) */
/*                        Jordan block; k=(N-1)/2 */

/*  (7)  ( D, I )         where D is diag( 0, 1,..., N-1 ) (a diagonal */
/*                        matrix with those diagonal entries.) */
/*  (8)  ( I, D ) */

/*  (9)  ( big*D, small*I ) where "big" is near overflow and small=1/big */

/*  (10) ( small*D, big*I ) */

/*  (11) ( big*I, small*D ) */

/*  (12) ( small*I, big*D ) */

/*  (13) ( big*D, big*I ) */

/*  (14) ( small*D, small*I ) */

/*  (15) ( D1, D2 )        where D1 is diag( 0, 0, 1, ..., N-3, 0 ) and */
/*                         D2 is diag( 0, N-3, N-4,..., 1, 0, 0 ) */
/*            t   t */
/*  (16) Q ( J , J ) Z     where Q and Z are random orthogonal matrices. */

/*  (17) Q ( T1, T2 ) Z    where T1 and T2 are upper triangular matrices */
/*                         with random O(1) entries above the diagonal */
/*                         and diagonal entries diag(T1) = */
/*                         ( 0, 0, 1, ..., N-3, 0 ) and diag(T2) = */
/*                         ( 0, N-3, N-4,..., 1, 0, 0 ) */

/*  (18) Q ( T1, T2 ) Z    diag(T1) = ( 0, 0, 1, 1, s, ..., s, 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1,..., 1, 0 ) */
/*                         s = machine precision. */

/*  (19) Q ( T1, T2 ) Z    diag(T1)=( 0,0,1,1, 1-d, ..., 1-(N-5)*d=s, 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1, ..., 1, 0 ) */

/*                                                         N-5 */
/*  (20) Q ( T1, T2 ) Z    diag(T1)=( 0, 0, 1, 1, a, ..., a   =s, 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1, ..., 1, 0, 0 ) */

/*  (21) Q ( T1, T2 ) Z    diag(T1)=( 0, 0, 1, r1, r2, ..., r(N-4), 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1, ..., 1, 0, 0 ) */
/*                         where r1,..., r(N-4) are random. */

/*  (22) Q ( big*T1, small*T2 ) Z    diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (23) Q ( small*T1, big*T2 ) Z    diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (24) Q ( small*T1, small*T2 ) Z  diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (25) Q ( big*T1, big*T2 ) Z      diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (26) Q ( T1, T2 ) Z     where T1 and T2 are random upper-triangular */
/*                          matrices. */


/*  Arguments */
/*  ========= */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          SDRGES does nothing.  NSIZES >= 0. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  NN >= 0. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, SDRGES */
/*          does nothing.  It must be at least zero.  If it is MAXTYP+1 */
/*          and NSIZES is 1, then an additional type, MAXTYP+1 is */
/*          defined, which is to use whatever matrix is in A on input. */
/*          This is only useful if DOTYPE(1:MAXTYP) is .FALSE. and */
/*          DOTYPE(MAXTYP+1) is .TRUE. . */

/*  DOTYPE  (input) LOGICAL array, dimension (NTYPES) */
/*          If DOTYPE(j) is .TRUE., then for each size in NN a */
/*          matrix of that size and of type j will be generated. */
/*          If NTYPES is smaller than the maximum number of types */
/*          defined (PARAMETER MAXTYP), then types NTYPES+1 through */
/*          MAXTYP will not be generated. If NTYPES is larger */
/*          than MAXTYP, DOTYPE(MAXTYP+1) through DOTYPE(NTYPES) */
/*          will be ignored. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry ISEED specifies the seed of the random number */
/*          generator. The array elements should be between 0 and 4095; */
/*          if not they will be reduced mod 4096. Also, ISEED(4) must */
/*          be odd.  The random number generator uses a linear */
/*          congruential sequence limited to small integers, and so */
/*          should produce machine independent random numbers. The */
/*          values of ISEED are changed on exit, and can be used in the */
/*          next call to SDRGES to continue the same random number */
/*          sequence. */

/*  THRESH  (input) REAL */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error is */
/*          scaled to be O(1), so THRESH should be a reasonably small */
/*          multiple of 1, e.g., 10 or 100.  In particular, it should */
/*          not depend on the precision (single vs. double) or the size */
/*          of the matrix.  THRESH >= 0. */

/*  NOUNIT  (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns IINFO not equal to 0.) */

/*  A       (input/workspace) REAL array, */
/*                                       dimension(LDA, max(NN)) */
/*          Used to hold the original A matrix.  Used as input only */
/*          if NTYPES=MAXTYP+1, DOTYPE(1:MAXTYP)=.FALSE., and */
/*          DOTYPE(MAXTYP+1)=.TRUE. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, B, S, and T. */
/*          It must be at least 1 and at least max( NN ). */

/*  B       (input/workspace) REAL array, */
/*                                       dimension(LDA, max(NN)) */
/*          Used to hold the original B matrix.  Used as input only */
/*          if NTYPES=MAXTYP+1, DOTYPE(1:MAXTYP)=.FALSE., and */
/*          DOTYPE(MAXTYP+1)=.TRUE. */

/*  S       (workspace) REAL array, dimension (LDA, max(NN)) */
/*          The Schur form matrix computed from A by SGGES.  On exit, S */
/*          contains the Schur form matrix corresponding to the matrix */
/*          in A. */

/*  T       (workspace) REAL array, dimension (LDA, max(NN)) */
/*          The upper triangular matrix computed from B by SGGES. */

/*  Q       (workspace) REAL array, dimension (LDQ, max(NN)) */
/*          The (left) orthogonal matrix computed by SGGES. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of Q and Z. It must */
/*          be at least 1 and at least max( NN ). */

/*  Z       (workspace) REAL array, dimension( LDQ, max(NN) ) */
/*          The (right) orthogonal matrix computed by SGGES. */

/*  ALPHAR  (workspace) REAL array, dimension (max(NN)) */
/*  ALPHAI  (workspace) REAL array, dimension (max(NN)) */
/*  BETA    (workspace) REAL array, dimension (max(NN)) */
/*          The generalized eigenvalues of (A,B) computed by SGGES. */
/*          ( ALPHAR(k)+ALPHAI(k)*i ) / BETA(k) is the k-th */
/*          generalized eigenvalue of A and B. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          LWORK >= MAX( 10*(N+1), 3*N*N ), where N is the largest */
/*          matrix dimension. */

/*  RESULT  (output) REAL array, dimension (15) */
/*          The values computed by the tests described above. */
/*          The values are currently limited to 1/ulp, to avoid overflow. */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  A routine returned an error code.  INFO is the */
/*                absolute value of the INFO value returned. */

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
    t_dim1 = *lda;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    s_dim1 = *lda;
    s_offset = 1 + s_dim1;
    s -= s_offset;
    b_dim1 = *lda;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    z_dim1 = *ldq;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --alphar;
    --alphai;
    --beta;
    --work;
    --result;
    --bwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

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
    } else if (*ldq <= 1 || *ldq < nmax) {
	*info = -14;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV. */

    minwrk = 1;
    if (*info == 0 && *lwork >= 1) {
/* Computing MAX */
	i__1 = (nmax + 1) * 10, i__2 = nmax * 3 * nmax;
	minwrk = max(i__1,i__2);
/* Computing MAX */
	i__1 = 1, i__2 = ilaenv_(&c__1, "SGEQRF", " ", &nmax, &nmax, &c_n1, &
		c_n1), i__1 = max(i__1,i__2), i__2 = 
		ilaenv_(&c__1, "SORMQR", "LT", &nmax, &nmax, &nmax, &c_n1), i__1 = max(i__1,i__2), i__2 = ilaenv_(&
		c__1, "SORGQR", " ", &nmax, &nmax, &nmax, &c_n1);
	nb = max(i__1,i__2);
/* Computing MAX */
	i__1 = (nmax + 1) * 10, i__2 = (nmax << 1) + nmax * nb, i__1 = max(
		i__1,i__2), i__2 = nmax * 3 * nmax;
	maxwrk = max(i__1,i__2);
	work[1] = (real) maxwrk;
    }

    if (*lwork < minwrk) {
	*info = -20;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SDRGES", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

    safmin = slamch_("Safe minimum");
    ulp = slamch_("Epsilon") * slamch_("Base");
    safmin /= ulp;
    safmax = 1.f / safmin;
    slabad_(&safmin, &safmax);
    ulpinv = 1.f / ulp;

/*     The values RMAGN(2:3) depend on N, see below. */

    rmagn[0] = 0.f;
    rmagn[1] = 1.f;

/*     Loop over matrix sizes */

    ntestt = 0;
    nerrs = 0;
    nmats = 0;

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	n = nn[jsize];
	n1 = max(1,n);
	rmagn[2] = safmax * ulp / (real) n1;
	rmagn[3] = safmin * ulpinv * (real) n1;

	if (*nsizes != 1) {
	    mtypes = min(26,*ntypes);
	} else {
	    mtypes = min(27,*ntypes);
	}

/*        Loop over matrix types */

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L180;
	    }
	    ++nmats;
	    ntest = 0;

/*           Save ISEED in case of an error. */

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

/*           Initialize RESULT */

	    for (j = 1; j <= 13; ++j) {
		result[j] = 0.f;
/* L30: */
	    }

/*           Generate test matrices A and B */

/*           Description of control parameters: */

/*           KCLASS: =1 means w/o rotation, =2 means w/ rotation, */
/*                   =3 means random. */
/*           KATYPE: the "type" to be passed to SLATM4 for computing A. */
/*           KAZERO: the pattern of zeros on the diagonal for A: */
/*                   =1: ( xxx ), =2: (0, xxx ) =3: ( 0, 0, xxx, 0 ), */
/*                   =4: ( 0, xxx, 0, 0 ), =5: ( 0, 0, 1, xxx, 0 ), */
/*                   =6: ( 0, 1, 0, xxx, 0 ).  (xxx means a string of */
/*                   non-zero entries.) */
/*           KAMAGN: the magnitude of the matrix: =0: zero, =1: O(1), */
/*                   =2: large, =3: small. */
/*           IASIGN: 1 if the diagonal elements of A are to be */
/*                   multiplied by a random magnitude 1 number, =2 if */
/*                   randomly chosen diagonal blocks are to be rotated */
/*                   to form 2x2 blocks. */
/*           KBTYPE, KBZERO, KBMAGN, IBSIGN: the same, but for B. */
/*           KTRIAN: =0: don't fill in the upper triangle, =1: do. */
/*           KZ1, KZ2, KADD: used to implement KAZERO and KBZERO. */
/*           RMAGN: used to implement KAMAGN and KBMAGN. */

	    if (mtypes > 26) {
		goto L110;
	    }
	    iinfo = 0;
	    if (kclass[jtype - 1] < 3) {

/*              Generate A (w/o rotation) */

		if ((i__3 = katype[jtype - 1], abs(i__3)) == 3) {
		    in = ((n - 1) / 2 << 1) + 1;
		    if (in != n) {
			slaset_("Full", &n, &n, &c_b26, &c_b26, &a[a_offset], 
				lda);
		    }
		} else {
		    in = n;
		}
		slatm4_(&katype[jtype - 1], &in, &kz1[kazero[jtype - 1] - 1], 
			&kz2[kazero[jtype - 1] - 1], &iasign[jtype - 1], &
			rmagn[kamagn[jtype - 1]], &ulp, &rmagn[ktrian[jtype - 
			1] * kamagn[jtype - 1]], &c__2, &iseed[1], &a[
			a_offset], lda);
		iadd = kadd[kazero[jtype - 1] - 1];
		if (iadd > 0 && iadd <= n) {
		    a[iadd + iadd * a_dim1] = 1.f;
		}

/*              Generate B (w/o rotation) */

		if ((i__3 = kbtype[jtype - 1], abs(i__3)) == 3) {
		    in = ((n - 1) / 2 << 1) + 1;
		    if (in != n) {
			slaset_("Full", &n, &n, &c_b26, &c_b26, &b[b_offset], 
				lda);
		    }
		} else {
		    in = n;
		}
		slatm4_(&kbtype[jtype - 1], &in, &kz1[kbzero[jtype - 1] - 1], 
			&kz2[kbzero[jtype - 1] - 1], &ibsign[jtype - 1], &
			rmagn[kbmagn[jtype - 1]], &c_b32, &rmagn[ktrian[jtype 
			- 1] * kbmagn[jtype - 1]], &c__2, &iseed[1], &b[
			b_offset], lda);
		iadd = kadd[kbzero[jtype - 1] - 1];
		if (iadd != 0 && iadd <= n) {
		    b[iadd + iadd * b_dim1] = 1.f;
		}

		if (kclass[jtype - 1] == 2 && n > 0) {

/*                 Include rotations */

/*                 Generate Q, Z as Householder transformations times */
/*                 a diagonal matrix. */

		    i__3 = n - 1;
		    for (jc = 1; jc <= i__3; ++jc) {
			i__4 = n;
			for (jr = jc; jr <= i__4; ++jr) {
			    q[jr + jc * q_dim1] = slarnd_(&c__3, &iseed[1]);
			    z__[jr + jc * z_dim1] = slarnd_(&c__3, &iseed[1]);
/* L40: */
			}
			i__4 = n + 1 - jc;
			slarfg_(&i__4, &q[jc + jc * q_dim1], &q[jc + 1 + jc * 
				q_dim1], &c__1, &work[jc]);
			work[(n << 1) + jc] = r_sign(&c_b32, &q[jc + jc * 
				q_dim1]);
			q[jc + jc * q_dim1] = 1.f;
			i__4 = n + 1 - jc;
			slarfg_(&i__4, &z__[jc + jc * z_dim1], &z__[jc + 1 + 
				jc * z_dim1], &c__1, &work[n + jc]);
			work[n * 3 + jc] = r_sign(&c_b32, &z__[jc + jc * 
				z_dim1]);
			z__[jc + jc * z_dim1] = 1.f;
/* L50: */
		    }
		    q[n + n * q_dim1] = 1.f;
		    work[n] = 0.f;
		    r__1 = slarnd_(&c__2, &iseed[1]);
		    work[n * 3] = r_sign(&c_b32, &r__1);
		    z__[n + n * z_dim1] = 1.f;
		    work[n * 2] = 0.f;
		    r__1 = slarnd_(&c__2, &iseed[1]);
		    work[n * 4] = r_sign(&c_b32, &r__1);

/*                 Apply the diagonal matrices */

		    i__3 = n;
		    for (jc = 1; jc <= i__3; ++jc) {
			i__4 = n;
			for (jr = 1; jr <= i__4; ++jr) {
			    a[jr + jc * a_dim1] = work[(n << 1) + jr] * work[
				    n * 3 + jc] * a[jr + jc * a_dim1];
			    b[jr + jc * b_dim1] = work[(n << 1) + jr] * work[
				    n * 3 + jc] * b[jr + jc * b_dim1];
/* L60: */
			}
/* L70: */
		    }
		    i__3 = n - 1;
		    sorm2r_("L", "N", &n, &n, &i__3, &q[q_offset], ldq, &work[
			    1], &a[a_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    sorm2r_("R", "T", &n, &n, &i__3, &z__[z_offset], ldq, &
			    work[n + 1], &a[a_offset], lda, &work[(n << 1) + 
			    1], &iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    sorm2r_("L", "N", &n, &n, &i__3, &q[q_offset], ldq, &work[
			    1], &b[b_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    sorm2r_("R", "T", &n, &n, &i__3, &z__[z_offset], ldq, &
			    work[n + 1], &b[b_offset], lda, &work[(n << 1) + 
			    1], &iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		}
	    } else {

/*              Random matrices */

		i__3 = n;
		for (jc = 1; jc <= i__3; ++jc) {
		    i__4 = n;
		    for (jr = 1; jr <= i__4; ++jr) {
			a[jr + jc * a_dim1] = rmagn[kamagn[jtype - 1]] * 
				slarnd_(&c__2, &iseed[1]);
			b[jr + jc * b_dim1] = rmagn[kbmagn[jtype - 1]] * 
				slarnd_(&c__2, &iseed[1]);
/* L80: */
		    }
/* L90: */
		}
	    }

L100:

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

L110:

	    for (i__ = 1; i__ <= 13; ++i__) {
		result[i__] = -1.f;
/* L120: */
	    }

/*           Test with and without sorting of eigenvalues */

	    for (isort = 0; isort <= 1; ++isort) {
		if (isort == 0) {
		    *(unsigned char *)sort = 'N';
		    rsub = 0;
		} else {
		    *(unsigned char *)sort = 'S';
		    rsub = 5;
		}

/*              Call SGGES to compute H, T, Q, Z, alpha, and beta. */

		slacpy_("Full", &n, &n, &a[a_offset], lda, &s[s_offset], lda);
		slacpy_("Full", &n, &n, &b[b_offset], lda, &t[t_offset], lda);
		ntest = rsub + 1 + isort;
		result[rsub + 1 + isort] = ulpinv;
		sgges_("V", "V", sort, (L_fp)slctes_, &n, &s[s_offset], lda, &
			t[t_offset], lda, &sdim, &alphar[1], &alphai[1], &
			beta[1], &q[q_offset], ldq, &z__[z_offset], ldq, &
			work[1], lwork, &bwork[1], &iinfo);
		if (iinfo != 0 && iinfo != n + 2) {
		    result[rsub + 1 + isort] = ulpinv;
		    io___46.ciunit = *nounit;
		    s_wsfe(&io___46);
		    do_fio(&c__1, "SGGES", (ftnlen)5);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    goto L160;
		}

		ntest = rsub + 4;

/*              Do tests 1--4 (or tests 7--9 when reordering ) */

		if (isort == 0) {
		    sget51_(&c__1, &n, &a[a_offset], lda, &s[s_offset], lda, &
			    q[q_offset], ldq, &z__[z_offset], ldq, &work[1], &
			    result[1]);
		    sget51_(&c__1, &n, &b[b_offset], lda, &t[t_offset], lda, &
			    q[q_offset], ldq, &z__[z_offset], ldq, &work[1], &
			    result[2]);
		} else {
		    sget54_(&n, &a[a_offset], lda, &b[b_offset], lda, &s[
			    s_offset], lda, &t[t_offset], lda, &q[q_offset], 
			    ldq, &z__[z_offset], ldq, &work[1], &result[7]);
		}
		sget51_(&c__3, &n, &a[a_offset], lda, &t[t_offset], lda, &q[
			q_offset], ldq, &q[q_offset], ldq, &work[1], &result[
			rsub + 3]);
		sget51_(&c__3, &n, &b[b_offset], lda, &t[t_offset], lda, &z__[
			z_offset], ldq, &z__[z_offset], ldq, &work[1], &
			result[rsub + 4]);

/*              Do test 5 and 6 (or Tests 10 and 11 when reordering): */
/*              check Schur form of A and compare eigenvalues with */
/*              diagonals. */

		ntest = rsub + 6;
		temp1 = 0.f;

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    ilabad = FALSE_;
		    if (alphai[j] == 0.f) {
/* Computing MAX */
			r__7 = safmin, r__8 = (r__2 = alphar[j], dabs(r__2)), 
				r__7 = max(r__7,r__8), r__8 = (r__3 = s[j + j 
				* s_dim1], dabs(r__3));
/* Computing MAX */
			r__9 = safmin, r__10 = (r__5 = beta[j], dabs(r__5)), 
				r__9 = max(r__9,r__10), r__10 = (r__6 = t[j + 
				j * t_dim1], dabs(r__6));
			temp2 = ((r__1 = alphar[j] - s[j + j * s_dim1], dabs(
				r__1)) / dmax(r__7,r__8) + (r__4 = beta[j] - 
				t[j + j * t_dim1], dabs(r__4)) / dmax(r__9,
				r__10)) / ulp;

			if (j < n) {
			    if (s[j + 1 + j * s_dim1] != 0.f) {
				ilabad = TRUE_;
				result[rsub + 5] = ulpinv;
			    }
			}
			if (j > 1) {
			    if (s[j + (j - 1) * s_dim1] != 0.f) {
				ilabad = TRUE_;
				result[rsub + 5] = ulpinv;
			    }
			}

		    } else {
			if (alphai[j] > 0.f) {
			    i1 = j;
			} else {
			    i1 = j - 1;
			}
			if (i1 <= 0 || i1 >= n) {
			    ilabad = TRUE_;
			} else if (i1 < n - 1) {
			    if (s[i1 + 2 + (i1 + 1) * s_dim1] != 0.f) {
				ilabad = TRUE_;
				result[rsub + 5] = ulpinv;
			    }
			} else if (i1 > 1) {
			    if (s[i1 + (i1 - 1) * s_dim1] != 0.f) {
				ilabad = TRUE_;
				result[rsub + 5] = ulpinv;
			    }
			}
			if (! ilabad) {
			    sget53_(&s[i1 + i1 * s_dim1], lda, &t[i1 + i1 * 
				    t_dim1], lda, &beta[j], &alphar[j], &
				    alphai[j], &temp2, &ierr);
			    if (ierr >= 3) {
				io___52.ciunit = *nounit;
				s_wsfe(&io___52);
				do_fio(&c__1, (char *)&ierr, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&j, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&n, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(
					integer));
				do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)
					sizeof(integer));
				e_wsfe();
				*info = abs(ierr);
			    }
			} else {
			    temp2 = ulpinv;
			}

		    }
		    temp1 = dmax(temp1,temp2);
		    if (ilabad) {
			io___53.ciunit = *nounit;
			s_wsfe(&io___53);
			do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			e_wsfe();
		    }
/* L130: */
		}
		result[rsub + 6] = temp1;

		if (isort >= 1) {

/*                 Do test 12 */

		    ntest = 12;
		    result[12] = 0.f;
		    knteig = 0;
		    i__3 = n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			r__1 = -alphai[i__];
			if (slctes_(&alphar[i__], &alphai[i__], &beta[i__]) ||
				 slctes_(&alphar[i__], &r__1, &beta[i__])) {
			    ++knteig;
			}
			if (i__ < n) {
			    r__1 = -alphai[i__ + 1];
			    r__2 = -alphai[i__];
			    if ((slctes_(&alphar[i__ + 1], &alphai[i__ + 1], &
				    beta[i__ + 1]) || slctes_(&alphar[i__ + 1]
, &r__1, &beta[i__ + 1])) && ! (slctes_(&
				    alphar[i__], &alphai[i__], &beta[i__]) || 
				    slctes_(&alphar[i__], &r__2, &beta[i__])) 
				    && iinfo != n + 2) {
				result[12] = ulpinv;
			    }
			}
/* L140: */
		    }
		    if (sdim != knteig) {
			result[12] = ulpinv;
		    }
		}

/* L150: */
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

L160:

	    ntestt += ntest;

/*           Print out tests which fail. */

	    i__3 = ntest;
	    for (jr = 1; jr <= i__3; ++jr) {
		if (result[jr] >= *thresh) {

/*                 If this is the first test to fail, */
/*                 print a header to the data file. */

		    if (nerrs == 0) {
			io___55.ciunit = *nounit;
			s_wsfe(&io___55);
			do_fio(&c__1, "SGS", (ftnlen)3);
			e_wsfe();

/*                    Matrix types */

			io___56.ciunit = *nounit;
			s_wsfe(&io___56);
			e_wsfe();
			io___57.ciunit = *nounit;
			s_wsfe(&io___57);
			e_wsfe();
			io___58.ciunit = *nounit;
			s_wsfe(&io___58);
			do_fio(&c__1, "Orthogonal", (ftnlen)10);
			e_wsfe();

/*                    Tests performed */

			io___59.ciunit = *nounit;
			s_wsfe(&io___59);
			do_fio(&c__1, "orthogonal", (ftnlen)10);
			do_fio(&c__1, "'", (ftnlen)1);
			do_fio(&c__1, "transpose", (ftnlen)9);
			for (j = 1; j <= 8; ++j) {
			    do_fio(&c__1, "'", (ftnlen)1);
			}
			e_wsfe();

		    }
		    ++nerrs;
		    if (result[jr] < 1e4f) {
			io___60.ciunit = *nounit;
			s_wsfe(&io___60);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jr, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[jr], (ftnlen)sizeof(
				real));
			e_wsfe();
		    } else {
			io___61.ciunit = *nounit;
			s_wsfe(&io___61);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jr, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[jr], (ftnlen)sizeof(
				real));
			e_wsfe();
		    }
		}
/* L170: */
	    }

L180:
	    ;
	}
/* L190: */
    }

/*     Summary */

    alasvm_("SGS", nounit, &nerrs, &ntestt, &c__0);

    work[1] = (real) maxwrk;

    return 0;








/*     End of SDRGES */

} /* sdrges_ */
