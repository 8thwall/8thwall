#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static integer c__1 = 1;
static integer c_n1 = -1;
static integer c__0 = 0;
static integer c__4 = 4;
static integer c__2 = 2;
static doublereal c_b39 = 1.;
static integer c__3 = 3;
static logical c_true = TRUE_;
static logical c_false = FALSE_;

/* Subroutine */ int zdrvgg_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, doublereal *
	thrshn, integer *nounit, doublecomplex *a, integer *lda, 
	doublecomplex *b, doublecomplex *s, doublecomplex *t, doublecomplex *
	s2, doublecomplex *t2, doublecomplex *q, integer *ldq, doublecomplex *
	z__, doublecomplex *alpha1, doublecomplex *beta1, doublecomplex *
	alpha2, doublecomplex *beta2, doublecomplex *vl, doublecomplex *vr, 
	doublecomplex *work, integer *lwork, doublereal *rwork, doublereal *
	result, integer *info)
{
    /* Initialized data */

    static integer kclass[26] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,
	    2,2,2,3 };
    static integer kbmagn[26] = { 1,1,1,1,1,1,1,1,3,2,3,2,2,3,1,1,1,1,1,1,1,3,
	    2,3,2,1 };
    static integer ktrian[26] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
	    1,1,1,1 };
    static logical lasign[26] = { FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,
	    TRUE_,FALSE_,TRUE_,TRUE_,FALSE_,FALSE_,TRUE_,TRUE_,TRUE_,FALSE_,
	    TRUE_,FALSE_,FALSE_,FALSE_,TRUE_,TRUE_,TRUE_,TRUE_,TRUE_,FALSE_ };
    static logical lbsign[26] = { FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,
	    FALSE_,TRUE_,FALSE_,FALSE_,TRUE_,TRUE_,FALSE_,FALSE_,TRUE_,FALSE_,
	    TRUE_,FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,FALSE_,
	    FALSE_ };
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
    static char fmt_9999[] = "(\002 ZDRVGG: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9998[] = "(\002 ZDRVGG: \002,a,\002 Eigenvectors from"
	    " \002,a,\002 incorrectly \002,\002normalized.\002,/\002 Bits of "
	    "error=\002,0p,g10.3,\002,\002,9x,\002N=\002,i6,\002, JTYPE=\002,"
	    "i6,\002, ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9997[] = "(/1x,a3,\002 -- Complex Generalized eigenvalue"
	    " problem driver\002)";
    static char fmt_9996[] = "(\002 Matrix types (see ZDRVGG for details):"
	    " \002)";
    static char fmt_9995[] = "(\002 Special Matrices:\002,23x,\002(J'=transp"
	    "osed Jordan block)\002,/\002   1=(0,0)  2=(I,0)  3=(0,I)  4=(I,I"
	    ")  5=(J',J')  \002,\0026=(diag(J',I), diag(I,J'))\002,/\002 Diag"
	    "onal Matrices:  ( \002,\002D=diag(0,1,2,...) )\002,/\002   7=(D,"
	    "I)   9=(large*D, small*I\002,\002)  11=(large*I, small*D)  13=(l"
	    "arge*D, large*I)\002,/\002   8=(I,D)  10=(small*D, large*I)  12="
	    "(small*I, large*D) \002,\002 14=(small*D, small*I)\002,/\002  15"
	    "=(D, reversed D)\002)";
    static char fmt_9994[] = "(\002 Matrices Rotated by Random \002,a,\002 M"
	    "atrices U, V:\002,/\002  16=Transposed Jordan Blocks            "
	    " 19=geometric \002,\002alpha, beta=0,1\002,/\002  17=arithm. alp"
	    "ha&beta             \002,\002      20=arithmetic alpha, beta=0,"
	    "1\002,/\002  18=clustered \002,\002alpha, beta=0,1            21"
	    "=random alpha, beta=0,1\002,/\002 Large & Small Matrices:\002,"
	    "/\002  22=(large, small)   \002,\00223=(small,large)    24=(smal"
	    "l,small)    25=(large,large)\002,/\002  26=random O(1) matrices"
	    ".\002)";
    static char fmt_9993[] = "(/\002 Tests performed:  (S is Schur, T is tri"
	    "angular, \002,\002Q and Z are \002,a,\002,\002,/20x,\002l and r "
	    "are the appropriate left and right\002,/19x,\002eigenvectors, re"
	    "sp., a is alpha, b is beta, and\002,/19x,a,\002 means \002,a,"
	    "\002.)\002,/\002 1 = | A - Q S Z\002,a,\002 | / ( |A| n ulp )   "
	    "   2 = | B - Q T Z\002,a,\002 | / ( |B| n ulp )\002,/\002 3 = | "
	    "I - QQ\002,a,\002 | / ( n ulp )             4 = | I - ZZ\002,a"
	    ",\002 | / ( n ulp )\002,/\002 5 = difference between (alpha,beta"
	    ") and diagonals of\002,\002 (S,T)\002,/\002 6 = max | ( b A - a "
	    "B )\002,a,\002 l | / const.   7 = max | ( b A - a B ) r | / cons"
	    "t.\002,/1x)";
    static char fmt_9992[] = "(\002 Matrix order=\002,i5,\002, type=\002,i2"
	    ",\002, seed=\002,4(i4,\002,\002),\002 result \002,i3,\002 is\002"
	    ",0p,f8.2)";
    static char fmt_9991[] = "(\002 Matrix order=\002,i5,\002, type=\002,i2"
	    ",\002, seed=\002,4(i4,\002,\002),\002 result \002,i3,\002 is\002"
	    ",1p,d10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, s_dim1, 
	    s_offset, s2_dim1, s2_offset, t_dim1, t_offset, t2_dim1, 
	    t2_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, z_dim1, 
	    z_offset, i__1, i__2, i__3, i__4, i__5, i__6, i__7, i__8, i__9, 
	    i__10, i__11;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, d__10, 
	    d__11, d__12, d__13, d__14, d__15, d__16;
    doublecomplex z__1, z__2, z__3, z__4;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *), z_abs(doublecomplex *);
    void d_cnjg(doublecomplex *, doublecomplex *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double d_imag(doublecomplex *);

    /* Local variables */
    integer j, n, i1, n1, jc, nb, in, jr, ns, nbz;
    doublereal ulp;
    integer iadd, nmax;
    doublereal temp1, temp2;
    logical badnn;
    doublereal dumma[4];
    integer iinfo;
    doublereal rmagn[4];
    doublecomplex ctemp;
    extern /* Subroutine */ int zgegs_(char *, char *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, integer *), zget51_(integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublereal *, doublereal *), zget52_(logical *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublecomplex *, integer *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, doublereal *, doublereal *);
    integer nmats, jsize;
    extern /* Subroutine */ int zgegv_(char *, char *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, integer *);
    integer nerrs, jtype, ntest;
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *), zlatm4_(
	    integer *, integer *, integer *, integer *, logical *, doublereal 
	    *, doublereal *, doublereal *, integer *, integer *, 
	    doublecomplex *, integer *);
    extern doublereal dlamch_(char *);
    extern /* Subroutine */ int zunm2r_(char *, char *, integer *, integer *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    doublereal safmin, safmax;
    integer ioldsd[4];
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *, 
	    integer *, integer *);
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *), xerbla_(char *, integer *), 
	    zlarfg_(integer *, doublecomplex *, doublecomplex *, integer *, 
	    doublecomplex *);
    extern /* Double Complex */ VOID zlarnd_(doublecomplex *, integer *, 
	    integer *);
    extern /* Subroutine */ int zlacpy_(char *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *), 
	    zlaset_(char *, integer *, integer *, doublecomplex *, 
	    doublecomplex *, doublecomplex *, integer *);
    doublereal ulpinv;
    integer lwkopt, mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___43 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9991, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */

/*  Purpose */
/*  ======= */

/*  ZDRVGG  checks the nonsymmetric generalized eigenvalue driver */
/*  routines. */
/*                                T          T        T */
/*  ZGEGS factors A and B as Q S Z  and Q T Z , where   means */
/*  transpose, T is upper triangular, S is in generalized Schur form */
/*  (upper triangular), and Q and Z are unitary.  It also */
/*  computes the generalized eigenvalues (alpha(1),beta(1)), ..., */
/*  (alpha(n),beta(n)), where alpha(j)=S(j,j) and beta(j)=T(j,j) -- */
/*  thus, w(j) = alpha(j)/beta(j) is a root of the generalized */
/*  eigenvalue problem */

/*      det( A - w(j) B ) = 0 */

/*  and m(j) = beta(j)/alpha(j) is a root of the essentially equivalent */
/*  problem */

/*      det( m(j) A - B ) = 0 */

/*  ZGEGV computes the generalized eigenvalues (alpha(1),beta(1)), ..., */
/*  (alpha(n),beta(n)), the matrix L whose columns contain the */
/*  generalized left eigenvectors l, and the matrix R whose columns */
/*  contain the generalized right eigenvectors r for the pair (A,B). */

/*  When ZDRVGG is called, a number of matrix "sizes" ("n's") and a */
/*  number of matrix "types" are specified.  For each size ("n") */
/*  and each type of matrix, one matrix will be generated and used */
/*  to test the nonsymmetric eigenroutines.  For each matrix, 7 */
/*  tests will be performed and compared with the threshhold THRESH: */

/*  Results from ZGEGS: */

/*                   H */
/*  (1)   | A - Q S Z  | / ( |A| n ulp ) */

/*                   H */
/*  (2)   | B - Q T Z  | / ( |B| n ulp ) */

/*                H */
/*  (3)   | I - QQ  | / ( n ulp ) */

/*                H */
/*  (4)   | I - ZZ  | / ( n ulp ) */

/*  (5)   maximum over j of D(j)  where: */

/*                      |alpha(j) - S(j,j)|        |beta(j) - T(j,j)| */
/*            D(j) = ------------------------ + ----------------------- */
/*                   max(|alpha(j)|,|S(j,j)|)   max(|beta(j)|,|T(j,j)|) */

/*  Results from ZGEGV: */

/*  (6)   max over all left eigenvalue/-vector pairs (beta/alpha,l) of */

/*     | l**H * (beta A - alpha B) | / ( ulp max( |beta A|, |alpha B| ) ) */

/*        where l**H is the conjugate tranpose of l. */

/*  (7)   max over all right eigenvalue/-vector pairs (beta/alpha,r) of */

/*        | (beta A - alpha B) r | / ( ulp max( |beta A|, |alpha B| ) ) */

/*  Test Matrices */
/*  ---- -------- */

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
/*  (16) Q ( J , J ) Z     where Q and Z are random unitary matrices. */

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
/*          ZDRVGG does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, ZDRVGG */
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
/*          next call to ZDRVGG to continue the same random number */
/*          sequence. */

/*  THRESH  (input) DOUBLE PRECISION */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error is */
/*          scaled to be O(1), so THRESH should be a reasonably small */
/*          multiple of 1, e.g., 10 or 100.  In particular, it should */
/*          not depend on the precision (single vs. double) or the size */
/*          of the matrix.  It must be at least zero. */

/*  THRSHN  (input) DOUBLE PRECISION */
/*          Threshhold for reporting eigenvector normalization error. */
/*          If the normalization of any eigenvector differs from 1 by */
/*          more than THRSHN*ulp, then a special error message will be */
/*          printed.  (This is handled separately from the other tests, */
/*          since only a compiler or programming error should cause an */
/*          error message, at least if THRSHN is at least 5--10.) */

/*  NOUNIT  (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns IINFO not equal to 0.) */

/*  A       (input/workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          Used to hold the original A matrix.  Used as input only */
/*          if NTYPES=MAXTYP+1, DOTYPE(1:MAXTYP)=.FALSE., and */
/*          DOTYPE(MAXTYP+1)=.TRUE. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, B, S, T, S2, and T2. */
/*          It must be at least 1 and at least max( NN ). */

/*  B       (input/workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          Used to hold the original B matrix.  Used as input only */
/*          if NTYPES=MAXTYP+1, DOTYPE(1:MAXTYP)=.FALSE., and */
/*          DOTYPE(MAXTYP+1)=.TRUE. */

/*  S       (workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          The upper triangular matrix computed from A by ZGEGS. */

/*  T       (workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          The upper triangular matrix computed from B by ZGEGS. */

/*  S2      (workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          The matrix computed from A by ZGEGV.  This will be the */
/*          Schur (upper triangular) form of some matrix related to A, */
/*          but will not, in general, be the same as S. */

/*  T2      (workspace) COMPLEX*16 array, dimension (LDA, max(NN)) */
/*          The matrix computed from B by ZGEGV.  This will be the */
/*          Schur form of some matrix related to B, but will not, in */
/*          general, be the same as T. */

/*  Q       (workspace) COMPLEX*16 array, dimension (LDQ, max(NN)) */
/*          The (left) unitary matrix computed by ZGEGS. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of Q, Z, VL, and VR.  It must */
/*          be at least 1 and at least max( NN ). */

/*  Z       (workspace) COMPLEX*16 array, dimension (LDQ, max(NN)) */
/*          The (right) unitary matrix computed by ZGEGS. */

/*  ALPHA1  (workspace) COMPLEX*16 array, dimension (max(NN)) */
/*  BETA1   (workspace) COMPLEX*16 array, dimension (max(NN)) */
/*          The generalized eigenvalues of (A,B) computed by ZGEGS. */
/*          ALPHA1(k) / BETA1(k)  is the k-th generalized eigenvalue of */
/*          the matrices in A and B. */

/*  ALPHA2  (workspace) COMPLEX*16 array, dimension (max(NN)) */
/*  BETA2   (workspace) COMPLEX*16 array, dimension (max(NN)) */
/*          The generalized eigenvalues of (A,B) computed by ZGEGV. */
/*          ALPHA2(k) / BETA2(k)  is the k-th generalized eigenvalue of */
/*          the matrices in A and B. */

/*  VL      (workspace) COMPLEX*16 array, dimension (LDQ, max(NN)) */
/*          The (lower triangular) left eigenvector matrix for the */
/*          matrices in A and B. */

/*  VR      (workspace) COMPLEX*16 array, dimension (LDQ, max(NN)) */
/*          The (upper triangular) right eigenvector matrix for the */
/*          matrices in A and B. */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          MAX( 2*N, N*(NB+1), (k+1)*(2*k+N+1) ), where "k" is the */
/*          sum of the blocksize and number-of-shifts for ZHGEQZ, and */
/*          NB is the greatest of the blocksizes for ZGEQRF, ZUNMQR, */
/*          and ZUNGQR.  (The blocksizes and the number-of-shifts are */
/*          retrieved through calls to ILAENV.) */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (8*N) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (7) */
/*          The values computed by the tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  A routine returned an error code.  INFO is the */
/*                absolute value of the INFO value returned. */

/*  ===================================================================== */

/*     .. */
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
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    --nn;
    --dotype;
    --iseed;
    t2_dim1 = *lda;
    t2_offset = 1 + t2_dim1;
    t2 -= t2_offset;
    s2_dim1 = *lda;
    s2_offset = 1 + s2_dim1;
    s2 -= s2_offset;
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
    vr_dim1 = *ldq;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    vl_dim1 = *ldq;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    z_dim1 = *ldq;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --alpha1;
    --beta1;
    --alpha2;
    --beta2;
    --work;
    --rwork;
    --result;

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

/*     Maximum blocksize and shift -- we assume that blocksize and number */
/*     of shifts are monotone increasing functions of N. */

/* Computing MAX */
    i__1 = 1, i__2 = ilaenv_(&c__1, "ZGEQRF", " ", &nmax, &nmax, &c_n1, &c_n1), i__1 = max(i__1,i__2), i__2 = ilaenv_(&
	    c__1, "ZUNMQR", "LC", &nmax, &nmax, &nmax, &c_n1), i__1 = max(i__1,i__2), i__2 = ilaenv_(&c__1, "ZUNGQR", 
	    " ", &nmax, &nmax, &nmax, &c_n1);
    nb = max(i__1,i__2);
    nbz = ilaenv_(&c__1, "ZHGEQZ", "SII", &nmax, &c__1, &nmax, &c__0);
    ns = ilaenv_(&c__4, "ZHGEQZ", "SII", &nmax, &c__1, &nmax, &c__0);
    i1 = nbz + ns;
/* Computing MAX */
    i__1 = nmax << 1, i__2 = nmax * (nb + 1), i__1 = max(i__1,i__2), i__2 = ((
	    i1 << 1) + nmax + 1) * (i1 + 1);
    lwkopt = max(i__1,i__2);

/*     Check for errors */

    if (*nsizes < 0) {
	*info = -1;
    } else if (badnn) {
	*info = -2;
    } else if (*ntypes < 0) {
	*info = -3;
    } else if (*thresh < 0.) {
	*info = -6;
    } else if (*lda <= 1 || *lda < nmax) {
	*info = -10;
    } else if (*ldq <= 1 || *ldq < nmax) {
	*info = -19;
    } else if (lwkopt > *lwork) {
	*info = -30;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("ZDRVGG", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

    ulp = dlamch_("Precision");
    safmin = dlamch_("Safe minimum");
    safmin /= ulp;
    safmax = 1. / safmin;
    dlabad_(&safmin, &safmax);
    ulpinv = 1. / ulp;

/*     The values RMAGN(2:3) depend on N, see below. */

    rmagn[0] = 0.;
    rmagn[1] = 1.;

/*     Loop over sizes, types */

    ntestt = 0;
    nerrs = 0;
    nmats = 0;

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	n = nn[jsize];
	n1 = max(1,n);
	rmagn[2] = safmax * ulp / (doublereal) n1;
	rmagn[3] = safmin * ulpinv * n1;

	if (*nsizes != 1) {
	    mtypes = min(26,*ntypes);
	} else {
	    mtypes = min(27,*ntypes);
	}

	i__2 = mtypes;
	for (jtype = 1; jtype <= i__2; ++jtype) {
	    if (! dotype[jtype]) {
		goto L150;
	    }
	    ++nmats;
	    ntest = 0;

/*           Save ISEED in case of an error. */

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

/*           Initialize RESULT */

	    for (j = 1; j <= 7; ++j) {
		result[j] = 0.;
/* L30: */
	    }

/*           Compute A and B */

/*           Description of control parameters: */

/*           KZLASS: =1 means w/o rotation, =2 means w/ rotation, */
/*                   =3 means random. */
/*           KATYPE: the "type" to be passed to ZLATM4 for computing A. */
/*           KAZERO: the pattern of zeros on the diagonal for A: */
/*                   =1: ( xxx ), =2: (0, xxx ) =3: ( 0, 0, xxx, 0 ), */
/*                   =4: ( 0, xxx, 0, 0 ), =5: ( 0, 0, 1, xxx, 0 ), */
/*                   =6: ( 0, 1, 0, xxx, 0 ).  (xxx means a string of */
/*                   non-zero entries.) */
/*           KAMAGN: the magnitude of the matrix: =0: zero, =1: O(1), */
/*                   =2: large, =3: small. */
/*           LASIGN: .TRUE. if the diagonal elements of A are to be */
/*                   multiplied by a random magnitude 1 number. */
/*           KBTYPE, KBZERO, KBMAGN, IBSIGN: the same, but for B. */
/*           KTRIAN: =0: don't fill in the upper triangle, =1: do. */
/*           KZ1, KZ2, KADD: used to implement KAZERO and KBZERO. */
/*           RMAGN:  used to implement KAMAGN and KBMAGN. */

	    if (mtypes > 26) {
		goto L110;
	    }
	    iinfo = 0;
	    if (kclass[jtype - 1] < 3) {

/*              Generate A (w/o rotation) */

		if ((i__3 = katype[jtype - 1], abs(i__3)) == 3) {
		    in = ((n - 1) / 2 << 1) + 1;
		    if (in != n) {
			zlaset_("Full", &n, &n, &c_b1, &c_b1, &a[a_offset], 
				lda);
		    }
		} else {
		    in = n;
		}
		zlatm4_(&katype[jtype - 1], &in, &kz1[kazero[jtype - 1] - 1], 
			&kz2[kazero[jtype - 1] - 1], &lasign[jtype - 1], &
			rmagn[kamagn[jtype - 1]], &ulp, &rmagn[ktrian[jtype - 
			1] * kamagn[jtype - 1]], &c__2, &iseed[1], &a[
			a_offset], lda);
		iadd = kadd[kazero[jtype - 1] - 1];
		if (iadd > 0 && iadd <= n) {
		    i__3 = iadd + iadd * a_dim1;
		    i__4 = kamagn[jtype - 1];
		    a[i__3].r = rmagn[i__4], a[i__3].i = 0.;
		}

/*              Generate B (w/o rotation) */

		if ((i__3 = kbtype[jtype - 1], abs(i__3)) == 3) {
		    in = ((n - 1) / 2 << 1) + 1;
		    if (in != n) {
			zlaset_("Full", &n, &n, &c_b1, &c_b1, &b[b_offset], 
				lda);
		    }
		} else {
		    in = n;
		}
		zlatm4_(&kbtype[jtype - 1], &in, &kz1[kbzero[jtype - 1] - 1], 
			&kz2[kbzero[jtype - 1] - 1], &lbsign[jtype - 1], &
			rmagn[kbmagn[jtype - 1]], &c_b39, &rmagn[ktrian[jtype 
			- 1] * kbmagn[jtype - 1]], &c__2, &iseed[1], &b[
			b_offset], lda);
		iadd = kadd[kbzero[jtype - 1] - 1];
		if (iadd != 0 && iadd <= n) {
		    i__3 = iadd + iadd * b_dim1;
		    i__4 = kbmagn[jtype - 1];
		    b[i__3].r = rmagn[i__4], b[i__3].i = 0.;
		}

		if (kclass[jtype - 1] == 2 && n > 0) {

/*                 Include rotations */

/*                 Generate Q, Z as Householder transformations times */
/*                 a diagonal matrix. */

		    i__3 = n - 1;
		    for (jc = 1; jc <= i__3; ++jc) {
			i__4 = n;
			for (jr = jc; jr <= i__4; ++jr) {
			    i__5 = jr + jc * q_dim1;
			    zlarnd_(&z__1, &c__3, &iseed[1]);
			    q[i__5].r = z__1.r, q[i__5].i = z__1.i;
			    i__5 = jr + jc * z_dim1;
			    zlarnd_(&z__1, &c__3, &iseed[1]);
			    z__[i__5].r = z__1.r, z__[i__5].i = z__1.i;
/* L40: */
			}
			i__4 = n + 1 - jc;
			zlarfg_(&i__4, &q[jc + jc * q_dim1], &q[jc + 1 + jc * 
				q_dim1], &c__1, &work[jc]);
			i__4 = (n << 1) + jc;
			i__5 = jc + jc * q_dim1;
			d__2 = q[i__5].r;
			d__1 = d_sign(&c_b39, &d__2);
			work[i__4].r = d__1, work[i__4].i = 0.;
			i__4 = jc + jc * q_dim1;
			q[i__4].r = 1., q[i__4].i = 0.;
			i__4 = n + 1 - jc;
			zlarfg_(&i__4, &z__[jc + jc * z_dim1], &z__[jc + 1 + 
				jc * z_dim1], &c__1, &work[n + jc]);
			i__4 = n * 3 + jc;
			i__5 = jc + jc * z_dim1;
			d__2 = z__[i__5].r;
			d__1 = d_sign(&c_b39, &d__2);
			work[i__4].r = d__1, work[i__4].i = 0.;
			i__4 = jc + jc * z_dim1;
			z__[i__4].r = 1., z__[i__4].i = 0.;
/* L50: */
		    }
		    zlarnd_(&z__1, &c__3, &iseed[1]);
		    ctemp.r = z__1.r, ctemp.i = z__1.i;
		    i__3 = n + n * q_dim1;
		    q[i__3].r = 1., q[i__3].i = 0.;
		    i__3 = n;
		    work[i__3].r = 0., work[i__3].i = 0.;
		    i__3 = n * 3;
		    d__1 = z_abs(&ctemp);
		    z__1.r = ctemp.r / d__1, z__1.i = ctemp.i / d__1;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;
		    zlarnd_(&z__1, &c__3, &iseed[1]);
		    ctemp.r = z__1.r, ctemp.i = z__1.i;
		    i__3 = n + n * z_dim1;
		    z__[i__3].r = 1., z__[i__3].i = 0.;
		    i__3 = n << 1;
		    work[i__3].r = 0., work[i__3].i = 0.;
		    i__3 = n << 2;
		    d__1 = z_abs(&ctemp);
		    z__1.r = ctemp.r / d__1, z__1.i = ctemp.i / d__1;
		    work[i__3].r = z__1.r, work[i__3].i = z__1.i;

/*                 Apply the diagonal matrices */

		    i__3 = n;
		    for (jc = 1; jc <= i__3; ++jc) {
			i__4 = n;
			for (jr = 1; jr <= i__4; ++jr) {
			    i__5 = jr + jc * a_dim1;
			    i__6 = (n << 1) + jr;
			    d_cnjg(&z__3, &work[n * 3 + jc]);
			    z__2.r = work[i__6].r * z__3.r - work[i__6].i * 
				    z__3.i, z__2.i = work[i__6].r * z__3.i + 
				    work[i__6].i * z__3.r;
			    i__7 = jr + jc * a_dim1;
			    z__1.r = z__2.r * a[i__7].r - z__2.i * a[i__7].i, 
				    z__1.i = z__2.r * a[i__7].i + z__2.i * a[
				    i__7].r;
			    a[i__5].r = z__1.r, a[i__5].i = z__1.i;
			    i__5 = jr + jc * b_dim1;
			    i__6 = (n << 1) + jr;
			    d_cnjg(&z__3, &work[n * 3 + jc]);
			    z__2.r = work[i__6].r * z__3.r - work[i__6].i * 
				    z__3.i, z__2.i = work[i__6].r * z__3.i + 
				    work[i__6].i * z__3.r;
			    i__7 = jr + jc * b_dim1;
			    z__1.r = z__2.r * b[i__7].r - z__2.i * b[i__7].i, 
				    z__1.i = z__2.r * b[i__7].i + z__2.i * b[
				    i__7].r;
			    b[i__5].r = z__1.r, b[i__5].i = z__1.i;
/* L60: */
			}
/* L70: */
		    }
		    i__3 = n - 1;
		    zunm2r_("L", "N", &n, &n, &i__3, &q[q_offset], ldq, &work[
			    1], &a[a_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    zunm2r_("R", "C", &n, &n, &i__3, &z__[z_offset], ldq, &
			    work[n + 1], &a[a_offset], lda, &work[(n << 1) + 
			    1], &iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    zunm2r_("L", "N", &n, &n, &i__3, &q[q_offset], ldq, &work[
			    1], &b[b_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    zunm2r_("R", "C", &n, &n, &i__3, &z__[z_offset], ldq, &
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
			i__5 = jr + jc * a_dim1;
			i__6 = kamagn[jtype - 1];
			zlarnd_(&z__2, &c__4, &iseed[1]);
			z__1.r = rmagn[i__6] * z__2.r, z__1.i = rmagn[i__6] * 
				z__2.i;
			a[i__5].r = z__1.r, a[i__5].i = z__1.i;
			i__5 = jr + jc * b_dim1;
			i__6 = kbmagn[jtype - 1];
			zlarnd_(&z__2, &c__4, &iseed[1]);
			z__1.r = rmagn[i__6] * z__2.r, z__1.i = rmagn[i__6] * 
				z__2.i;
			b[i__5].r = z__1.r, b[i__5].i = z__1.i;
/* L80: */
		    }
/* L90: */
		}
	    }

L100:

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

/*           Call ZGEGS to compute H, T, Q, Z, alpha, and beta. */

	    zlacpy_(" ", &n, &n, &a[a_offset], lda, &s[s_offset], lda);
	    zlacpy_(" ", &n, &n, &b[b_offset], lda, &t[t_offset], lda);
	    ntest = 1;
	    result[1] = ulpinv;

	    zgegs_("V", "V", &n, &s[s_offset], lda, &t[t_offset], lda, &
		    alpha1[1], &beta1[1], &q[q_offset], ldq, &z__[z_offset], 
		    ldq, &work[1], lwork, &rwork[1], &iinfo);
	    if (iinfo != 0) {
		io___44.ciunit = *nounit;
		s_wsfe(&io___44);
		do_fio(&c__1, "ZGEGS", (ftnlen)5);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L130;
	    }

	    ntest = 4;

/*           Do tests 1--4 */

	    zget51_(&c__1, &n, &a[a_offset], lda, &s[s_offset], lda, &q[
		    q_offset], ldq, &z__[z_offset], ldq, &work[1], &rwork[1], 
		    &result[1]);
	    zget51_(&c__1, &n, &b[b_offset], lda, &t[t_offset], lda, &q[
		    q_offset], ldq, &z__[z_offset], ldq, &work[1], &rwork[1], 
		    &result[2]);
	    zget51_(&c__3, &n, &b[b_offset], lda, &t[t_offset], lda, &q[
		    q_offset], ldq, &q[q_offset], ldq, &work[1], &rwork[1], &
		    result[3]);
	    zget51_(&c__3, &n, &b[b_offset], lda, &t[t_offset], lda, &z__[
		    z_offset], ldq, &z__[z_offset], ldq, &work[1], &rwork[1], 
		    &result[4]);

/*           Do test 5: compare eigenvalues with diagonals. */

	    temp1 = 0.;

	    i__3 = n;
	    for (j = 1; j <= i__3; ++j) {
		i__4 = j;
		i__5 = j + j * s_dim1;
		z__2.r = alpha1[i__4].r - s[i__5].r, z__2.i = alpha1[i__4].i 
			- s[i__5].i;
		z__1.r = z__2.r, z__1.i = z__2.i;
		i__6 = j;
		i__7 = j + j * t_dim1;
		z__4.r = beta1[i__6].r - t[i__7].r, z__4.i = beta1[i__6].i - 
			t[i__7].i;
		z__3.r = z__4.r, z__3.i = z__4.i;
/* Computing MAX */
		i__8 = j;
		i__9 = j + j * s_dim1;
		d__13 = safmin, d__14 = (d__1 = alpha1[i__8].r, abs(d__1)) + (
			d__2 = d_imag(&alpha1[j]), abs(d__2)), d__13 = max(
			d__13,d__14), d__14 = (d__3 = s[i__9].r, abs(d__3)) + 
			(d__4 = d_imag(&s[j + j * s_dim1]), abs(d__4));
/* Computing MAX */
		i__10 = j;
		i__11 = j + j * t_dim1;
		d__15 = safmin, d__16 = (d__5 = beta1[i__10].r, abs(d__5)) + (
			d__6 = d_imag(&beta1[j]), abs(d__6)), d__15 = max(
			d__15,d__16), d__16 = (d__7 = t[i__11].r, abs(d__7)) 
			+ (d__8 = d_imag(&t[j + j * t_dim1]), abs(d__8));
		temp2 = (((d__9 = z__1.r, abs(d__9)) + (d__10 = d_imag(&z__1),
			 abs(d__10))) / max(d__13,d__14) + ((d__11 = z__3.r, 
			abs(d__11)) + (d__12 = d_imag(&z__3), abs(d__12))) / 
			max(d__15,d__16)) / ulp;
		temp1 = max(temp1,temp2);
/* L120: */
	    }
	    result[5] = temp1;

/*           Call ZGEGV to compute S2, T2, VL, and VR, do tests. */

/*           Eigenvalues and Eigenvectors */

	    zlacpy_(" ", &n, &n, &a[a_offset], lda, &s2[s2_offset], lda);
	    zlacpy_(" ", &n, &n, &b[b_offset], lda, &t2[t2_offset], lda);
	    ntest = 6;
	    result[6] = ulpinv;

	    zgegv_("V", "V", &n, &s2[s2_offset], lda, &t2[t2_offset], lda, &
		    alpha2[1], &beta2[1], &vl[vl_offset], ldq, &vr[vr_offset], 
		     ldq, &work[1], lwork, &rwork[1], &iinfo);
	    if (iinfo != 0) {
		io___47.ciunit = *nounit;
		s_wsfe(&io___47);
		do_fio(&c__1, "ZGEGV", (ftnlen)5);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L130;
	    }

	    ntest = 7;

/*           Do Tests 6 and 7 */

	    zget52_(&c_true, &n, &a[a_offset], lda, &b[b_offset], lda, &vl[
		    vl_offset], ldq, &alpha2[1], &beta2[1], &work[1], &rwork[
		    1], dumma);
	    result[6] = dumma[0];
	    if (dumma[1] > *thrshn) {
		io___49.ciunit = *nounit;
		s_wsfe(&io___49);
		do_fio(&c__1, "Left", (ftnlen)4);
		do_fio(&c__1, "ZGEGV", (ftnlen)5);
		do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

	    zget52_(&c_false, &n, &a[a_offset], lda, &b[b_offset], lda, &vr[
		    vr_offset], ldq, &alpha2[1], &beta2[1], &work[1], &rwork[
		    1], dumma);
	    result[7] = dumma[0];
	    if (dumma[1] > *thresh) {
		io___50.ciunit = *nounit;
		s_wsfe(&io___50);
		do_fio(&c__1, "Right", (ftnlen)5);
		do_fio(&c__1, "ZGEGV", (ftnlen)5);
		do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

L130:

	    ntestt += ntest;

/*           Print out tests which fail. */

	    i__3 = ntest;
	    for (jr = 1; jr <= i__3; ++jr) {
		if (result[jr] >= *thresh) {

/*                 If this is the first test to fail, */
/*                 print a header to the data file. */

		    if (nerrs == 0) {
			io___51.ciunit = *nounit;
			s_wsfe(&io___51);
			do_fio(&c__1, "ZGG", (ftnlen)3);
			e_wsfe();

/*                    Matrix types */

			io___52.ciunit = *nounit;
			s_wsfe(&io___52);
			e_wsfe();
			io___53.ciunit = *nounit;
			s_wsfe(&io___53);
			e_wsfe();
			io___54.ciunit = *nounit;
			s_wsfe(&io___54);
			do_fio(&c__1, "Unitary", (ftnlen)7);
			e_wsfe();

/*                    Tests performed */

			io___55.ciunit = *nounit;
			s_wsfe(&io___55);
			do_fio(&c__1, "unitary", (ftnlen)7);
			do_fio(&c__1, "*", (ftnlen)1);
			do_fio(&c__1, "conjugate transpose", (ftnlen)19);
			for (j = 1; j <= 5; ++j) {
			    do_fio(&c__1, "*", (ftnlen)1);
			}
			e_wsfe();

		    }
		    ++nerrs;
		    if (result[jr] < 1e4) {
			io___56.ciunit = *nounit;
			s_wsfe(&io___56);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jr, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[jr], (ftnlen)sizeof(
				doublereal));
			e_wsfe();
		    } else {
			io___57.ciunit = *nounit;
			s_wsfe(&io___57);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jr, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[jr], (ftnlen)sizeof(
				doublereal));
			e_wsfe();
		    }
		}
/* L140: */
	    }

L150:
	    ;
	}
/* L160: */
    }

/*     Summary */

    alasvm_("ZGG", nounit, &nerrs, &ntestt, &c__0);
    return 0;







/*     End of ZDRVGG */

} /* zdrvgg_ */
