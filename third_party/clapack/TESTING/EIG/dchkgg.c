#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b13 = 0.;
static integer c__2 = 2;
static doublereal c_b19 = 1.;
static integer c__3 = 3;
static integer c__1 = 1;
static integer c__4 = 4;
static logical c_true = TRUE_;
static logical c_false = FALSE_;

/* Subroutine */ int dchkgg_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, logical *tstdif, 
	doublereal *thrshn, integer *nounit, doublereal *a, integer *lda, 
	doublereal *b, doublereal *h__, doublereal *t, doublereal *s1, 
	doublereal *s2, doublereal *p1, doublereal *p2, doublereal *u, 
	integer *ldu, doublereal *v, doublereal *q, doublereal *z__, 
	doublereal *alphr1, doublereal *alphi1, doublereal *beta1, doublereal 
	*alphr3, doublereal *alphi3, doublereal *beta3, doublereal *evectl, 
	doublereal *evectr, doublereal *work, integer *lwork, logical *llwork, 
	 doublereal *result, integer *info)
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
    static char fmt_9999[] = "(\002 DCHKGG: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9998[] = "(\002 DCHKGG: \002,a,\002 Eigenvectors from"
	    " \002,a,\002 incorrectly \002,\002normalized.\002,/\002 Bits of "
	    "error=\002,0p,g10.3,\002,\002,9x,\002N=\002,i6,\002, JTYPE=\002,"
	    "i6,\002, ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9997[] = "(/1x,a3,\002 -- Real Generalized eigenvalue pr"
	    "oblem\002)";
    static char fmt_9996[] = "(\002 Matrix types (see DCHKGG for details):"
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
    static char fmt_9993[] = "(/\002 Tests performed:   (H is Hessenberg, S "
	    "is Schur, B, \002,\002T, P are triangular,\002,/20x,\002U, V, Q,"
	    " and Z are \002,a,\002, l and r are the\002,/20x,\002appropriate"
	    " left and right eigenvectors, resp., a is\002,/20x,\002alpha, b "
	    "is beta, and \002,a,\002 means \002,a,\002.)\002,/\002 1 = | A -"
	    " U H V\002,a,\002 | / ( |A| n ulp )      2 = | B - U T V\002,a"
	    ",\002 | / ( |B| n ulp )\002,/\002 3 = | I - UU\002,a,\002 | / ( "
	    "n ulp )             4 = | I - VV\002,a,\002 | / ( n ulp )\002,"
	    "/\002 5 = | H - Q S Z\002,a,\002 | / ( |H| n ulp )\002,6x,\0026 "
	    "= | T - Q P Z\002,a,\002 | / ( |T| n ulp )\002,/\002 7 = | I - QQ"
	    "\002,a,\002 | / ( n ulp )             8 = | I - ZZ\002,a,\002 | "
	    "/ ( n ulp )\002,/\002 9 = max | ( b S - a P )\002,a,\002 l | / c"
	    "onst.  10 = max | ( b H - a T )\002,a,\002 l | / const.\002,/"
	    "\002 11= max | ( b S - a P ) r | / const.   12 = max | ( b H\002,"
	    "\002 - a T ) r | / const.\002,/1x)";
    static char fmt_9992[] = "(\002 Matrix order=\002,i5,\002, type=\002,i2"
	    ",\002, seed=\002,4(i4,\002,\002),\002 result \002,i2,\002 is\002"
	    ",0p,f8.2)";
    static char fmt_9991[] = "(\002 Matrix order=\002,i5,\002, type=\002,i2"
	    ",\002, seed=\002,4(i4,\002,\002),\002 result \002,i2,\002 is\002"
	    ",1p,d10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, evectl_dim1, evectl_offset, 
	    evectr_dim1, evectr_offset, h_dim1, h_offset, p1_dim1, p1_offset, 
	    p2_dim1, p2_offset, q_dim1, q_offset, s1_dim1, s1_offset, s2_dim1,
	     s2_offset, t_dim1, t_offset, u_dim1, u_offset, v_dim1, v_offset, 
	    z_dim1, z_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer j, n, i1, n1, jc, in, jr;
    doublereal ulp;
    integer iadd, nmax;
    doublereal temp1, temp2;
    logical badnn;
    extern /* Subroutine */ int dget51_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *), dget52_(
	    logical *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    doublereal dumma[4];
    integer iinfo;
    doublereal rmagn[4], anorm, bnorm;
    integer nmats, jsize, nerrs, jtype, ntest;
    extern /* Subroutine */ int dgeqr2_(integer *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *), dlatm4_(
	    integer *, integer *, integer *, integer *, integer *, doublereal 
	    *, doublereal *, doublereal *, integer *, integer *, doublereal *, 
	     integer *), dorm2r_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, integer *), dlabad_(
	    doublereal *, doublereal *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dgghrd_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *), dlarfg_(integer *, doublereal *, doublereal *, 
	    integer *, doublereal *);
    extern doublereal dlarnd_(integer *, integer *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    doublereal safmin;
    integer ioldsd[4];
    doublereal safmax;
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    dhgeqz_(char *, char *, char *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *, doublereal *, integer *, integer *), dtgevc_(char *, char *, logical *, integer *, doublereal 
	    *, integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, integer *, integer *, doublereal *, 
	    integer *), dlasum_(char *, integer *, integer *, 
	    integer *), xerbla_(char *, integer *);
    doublereal ulpinv;
    integer lwkopt, mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9991, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKGG  checks the nonsymmetric generalized eigenvalue problem */
/*  routines. */
/*                                 T          T        T */
/*  DGGHRD factors A and B as U H V  and U T V , where   means */
/*  transpose, H is hessenberg, T is triangular and U and V are */
/*  orthogonal. */
/*                                  T          T */
/*  DHGEQZ factors H and T as  Q S Z  and Q P Z , where P is upper */
/*  triangular, S is in generalized Schur form (block upper triangular, */
/*  with 1x1 and 2x2 blocks on the diagonal, the 2x2 blocks */
/*  corresponding to complex conjugate pairs of generalized */
/*  eigenvalues), and Q and Z are orthogonal.  It also computes the */
/*  generalized eigenvalues (alpha(1),beta(1)),...,(alpha(n),beta(n)), */
/*  where alpha(j)=S(j,j) and beta(j)=P(j,j) -- thus, */
/*  w(j) = alpha(j)/beta(j) is a root of the generalized eigenvalue */
/*  problem */

/*      det( A - w(j) B ) = 0 */

/*  and m(j) = beta(j)/alpha(j) is a root of the essentially equivalent */
/*  problem */

/*      det( m(j) A - B ) = 0 */

/*  DTGEVC computes the matrix L of left eigenvectors and the matrix R */
/*  of right eigenvectors for the matrix pair ( S, P ).  In the */
/*  description below,  l and r are left and right eigenvectors */
/*  corresponding to the generalized eigenvalues (alpha,beta). */

/*  When DCHKGG is called, a number of matrix "sizes" ("n's") and a */
/*  number of matrix "types" are specified.  For each size ("n") */
/*  and each type of matrix, one matrix will be generated and used */
/*  to test the nonsymmetric eigenroutines.  For each matrix, 15 */
/*  tests will be performed.  The first twelve "test ratios" should be */
/*  small -- O(1).  They will be compared with the threshhold THRESH: */

/*                   T */
/*  (1)   | A - U H V  | / ( |A| n ulp ) */

/*                   T */
/*  (2)   | B - U T V  | / ( |B| n ulp ) */

/*                T */
/*  (3)   | I - UU  | / ( n ulp ) */

/*                T */
/*  (4)   | I - VV  | / ( n ulp ) */

/*                   T */
/*  (5)   | H - Q S Z  | / ( |H| n ulp ) */

/*                   T */
/*  (6)   | T - Q P Z  | / ( |T| n ulp ) */

/*                T */
/*  (7)   | I - QQ  | / ( n ulp ) */

/*                T */
/*  (8)   | I - ZZ  | / ( n ulp ) */

/*  (9)   max over all left eigenvalue/-vector pairs (beta/alpha,l) of */

/*     | l**H * (beta S - alpha P) | / ( ulp max( |beta S|, |alpha P| ) ) */

/*  (10)  max over all left eigenvalue/-vector pairs (beta/alpha,l') of */
/*                            T */
/*    | l'**H * (beta H - alpha T) | / ( ulp max( |beta H|, |alpha T| ) ) */

/*        where the eigenvectors l' are the result of passing Q to */
/*        DTGEVC and back transforming (HOWMNY='B'). */

/*  (11)  max over all right eigenvalue/-vector pairs (beta/alpha,r) of */

/*        | (beta S - alpha T) r | / ( ulp max( |beta S|, |alpha T| ) ) */

/*  (12)  max over all right eigenvalue/-vector pairs (beta/alpha,r') of */

/*        | (beta H - alpha T) r' | / ( ulp max( |beta H|, |alpha T| ) ) */

/*        where the eigenvectors r' are the result of passing Z to */
/*        DTGEVC and back transforming (HOWMNY='B'). */

/*  The last three test ratios will usually be small, but there is no */
/*  mathematical requirement that they be so.  They are therefore */
/*  compared with THRESH only if TSTDIF is .TRUE. */

/*  (13)  | S(Q,Z computed) - S(Q,Z not computed) | / ( |S| ulp ) */

/*  (14)  | P(Q,Z computed) - P(Q,Z not computed) | / ( |P| ulp ) */

/*  (15)  max( |alpha(Q,Z computed) - alpha(Q,Z not computed)|/|S| , */
/*             |beta(Q,Z computed) - beta(Q,Z not computed)|/|P| ) / ulp */

/*  In addition, the normalization of L and R are checked, and compared */
/*  with the threshhold THRSHN. */

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
/*  (16) U ( J , J ) V     where U and V are random orthogonal matrices. */

/*  (17) U ( T1, T2 ) V    where T1 and T2 are upper triangular matrices */
/*                         with random O(1) entries above the diagonal */
/*                         and diagonal entries diag(T1) = */
/*                         ( 0, 0, 1, ..., N-3, 0 ) and diag(T2) = */
/*                         ( 0, N-3, N-4,..., 1, 0, 0 ) */

/*  (18) U ( T1, T2 ) V    diag(T1) = ( 0, 0, 1, 1, s, ..., s, 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1,..., 1, 0 ) */
/*                         s = machine precision. */

/*  (19) U ( T1, T2 ) V    diag(T1)=( 0,0,1,1, 1-d, ..., 1-(N-5)*d=s, 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1, ..., 1, 0 ) */

/*                                                         N-5 */
/*  (20) U ( T1, T2 ) V    diag(T1)=( 0, 0, 1, 1, a, ..., a   =s, 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1, ..., 1, 0, 0 ) */

/*  (21) U ( T1, T2 ) V    diag(T1)=( 0, 0, 1, r1, r2, ..., r(N-4), 0 ) */
/*                         diag(T2) = ( 0, 1, 0, 1, ..., 1, 0, 0 ) */
/*                         where r1,..., r(N-4) are random. */

/*  (22) U ( big*T1, small*T2 ) V    diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (23) U ( small*T1, big*T2 ) V    diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (24) U ( small*T1, small*T2 ) V  diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (25) U ( big*T1, big*T2 ) V      diag(T1) = ( 0, 0, 1, ..., N-3, 0 ) */
/*                                   diag(T2) = ( 0, 1, ..., 1, 0, 0 ) */

/*  (26) U ( T1, T2 ) V     where T1 and T2 are random upper-triangular */
/*                          matrices. */

/*  Arguments */
/*  ========= */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          DCHKGG does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, DCHKGG */
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
/*          next call to DCHKGG to continue the same random number */
/*          sequence. */

/*  THRESH  (input) DOUBLE PRECISION */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error is */
/*          scaled to be O(1), so THRESH should be a reasonably small */
/*          multiple of 1, e.g., 10 or 100.  In particular, it should */
/*          not depend on the precision (single vs. double) or the size */
/*          of the matrix.  It must be at least zero. */

/*  TSTDIF  (input) LOGICAL */
/*          Specifies whether test ratios 13-15 will be computed and */
/*          compared with THRESH. */
/*          = .FALSE.: Only test ratios 1-12 will be computed and tested. */
/*                     Ratios 13-15 will be set to zero. */
/*          = .TRUE.:  All the test ratios 1-15 will be computed and */
/*                     tested. */

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

/*  A       (input/workspace) DOUBLE PRECISION array, dimension */
/*                            (LDA, max(NN)) */
/*          Used to hold the original A matrix.  Used as input only */
/*          if NTYPES=MAXTYP+1, DOTYPE(1:MAXTYP)=.FALSE., and */
/*          DOTYPE(MAXTYP+1)=.TRUE. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, B, H, T, S1, P1, S2, and P2. */
/*          It must be at least 1 and at least max( NN ). */

/*  B       (input/workspace) DOUBLE PRECISION array, dimension */
/*                            (LDA, max(NN)) */
/*          Used to hold the original B matrix.  Used as input only */
/*          if NTYPES=MAXTYP+1, DOTYPE(1:MAXTYP)=.FALSE., and */
/*          DOTYPE(MAXTYP+1)=.TRUE. */

/*  H       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          The upper Hessenberg matrix computed from A by DGGHRD. */

/*  T       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          The upper triangular matrix computed from B by DGGHRD. */

/*  S1      (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          The Schur (block upper triangular) matrix computed from H by */
/*          DHGEQZ when Q and Z are also computed. */

/*  S2      (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          The Schur (block upper triangular) matrix computed from H by */
/*          DHGEQZ when Q and Z are not computed. */

/*  P1      (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          The upper triangular matrix computed from T by DHGEQZ */
/*          when Q and Z are also computed. */

/*  P2      (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          The upper triangular matrix computed from T by DHGEQZ */
/*          when Q and Z are not computed. */

/*  U       (workspace) DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The (left) orthogonal matrix computed by DGGHRD. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U, V, Q, Z, EVECTL, and EVEZTR.  It */
/*          must be at least 1 and at least max( NN ). */

/*  V       (workspace) DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The (right) orthogonal matrix computed by DGGHRD. */

/*  Q       (workspace) DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The (left) orthogonal matrix computed by DHGEQZ. */

/*  Z       (workspace) DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The (left) orthogonal matrix computed by DHGEQZ. */

/*  ALPHR1  (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  ALPHI1  (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  BETA1   (workspace) DOUBLE PRECISION array, dimension (max(NN)) */

/*          The generalized eigenvalues of (A,B) computed by DHGEQZ */
/*          when Q, Z, and the full Schur matrices are computed. */
/*          On exit, ( ALPHR1(k)+ALPHI1(k)*i ) / BETA1(k) is the k-th */
/*          generalized eigenvalue of the matrices in A and B. */

/*  ALPHR3  (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  ALPHI3  (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  BETA3   (workspace) DOUBLE PRECISION array, dimension (max(NN)) */

/*  EVECTL  (workspace) DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The (block lower triangular) left eigenvector matrix for */
/*          the matrices in S1 and P1.  (See DTGEVC for the format.) */

/*  EVEZTR  (workspace) DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          The (block upper triangular) right eigenvector matrix for */
/*          the matrices in S1 and P1.  (See DTGEVC for the format.) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          max( 2 * N**2, 6*N, 1 ), for all N=NN(j). */

/*  LLWORK  (workspace) LOGICAL array, dimension (max(NN)) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (15) */
/*          The values computed by the tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
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
    p2_dim1 = *lda;
    p2_offset = 1 + p2_dim1;
    p2 -= p2_offset;
    p1_dim1 = *lda;
    p1_offset = 1 + p1_dim1;
    p1 -= p1_offset;
    s2_dim1 = *lda;
    s2_offset = 1 + s2_dim1;
    s2 -= s2_offset;
    s1_dim1 = *lda;
    s1_offset = 1 + s1_dim1;
    s1 -= s1_offset;
    t_dim1 = *lda;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    h_dim1 = *lda;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    b_dim1 = *lda;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    evectr_dim1 = *ldu;
    evectr_offset = 1 + evectr_dim1;
    evectr -= evectr_offset;
    evectl_dim1 = *ldu;
    evectl_offset = 1 + evectl_dim1;
    evectl -= evectl_offset;
    z_dim1 = *ldu;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    q_dim1 = *ldu;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    v_dim1 = *ldu;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --alphr1;
    --alphi1;
    --beta1;
    --alphr3;
    --alphi3;
    --beta3;
    --work;
    --llwork;
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
    i__1 = nmax * 6, i__2 = (nmax << 1) * nmax, i__1 = max(i__1,i__2);
    lwkopt = max(i__1,1);

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
    } else if (*ldu <= 1 || *ldu < nmax) {
	*info = -19;
    } else if (lwkopt > *lwork) {
	*info = -30;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DCHKGG", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

    safmin = dlamch_("Safe minimum");
    ulp = dlamch_("Epsilon") * dlamch_("Base");
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
		goto L230;
	    }
	    ++nmats;
	    ntest = 0;

/*           Save ISEED in case of an error. */

	    for (j = 1; j <= 4; ++j) {
		ioldsd[j - 1] = iseed[j];
/* L20: */
	    }

/*           Initialize RESULT */

	    for (j = 1; j <= 15; ++j) {
		result[j] = 0.;
/* L30: */
	    }

/*           Compute A and B */

/*           Description of control parameters: */

/*           KZLASS: =1 means w/o rotation, =2 means w/ rotation, */
/*                   =3 means random. */
/*           KATYPE: the "type" to be passed to DLATM4 for computing A. */
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
			dlaset_("Full", &n, &n, &c_b13, &c_b13, &a[a_offset], 
				lda);
		    }
		} else {
		    in = n;
		}
		dlatm4_(&katype[jtype - 1], &in, &kz1[kazero[jtype - 1] - 1], 
			&kz2[kazero[jtype - 1] - 1], &iasign[jtype - 1], &
			rmagn[kamagn[jtype - 1]], &ulp, &rmagn[ktrian[jtype - 
			1] * kamagn[jtype - 1]], &c__2, &iseed[1], &a[
			a_offset], lda);
		iadd = kadd[kazero[jtype - 1] - 1];
		if (iadd > 0 && iadd <= n) {
		    a[iadd + iadd * a_dim1] = rmagn[kamagn[jtype - 1]];
		}

/*              Generate B (w/o rotation) */

		if ((i__3 = kbtype[jtype - 1], abs(i__3)) == 3) {
		    in = ((n - 1) / 2 << 1) + 1;
		    if (in != n) {
			dlaset_("Full", &n, &n, &c_b13, &c_b13, &b[b_offset], 
				lda);
		    }
		} else {
		    in = n;
		}
		dlatm4_(&kbtype[jtype - 1], &in, &kz1[kbzero[jtype - 1] - 1], 
			&kz2[kbzero[jtype - 1] - 1], &ibsign[jtype - 1], &
			rmagn[kbmagn[jtype - 1]], &c_b19, &rmagn[ktrian[jtype 
			- 1] * kbmagn[jtype - 1]], &c__2, &iseed[1], &b[
			b_offset], lda);
		iadd = kadd[kbzero[jtype - 1] - 1];
		if (iadd != 0 && iadd <= n) {
		    b[iadd + iadd * b_dim1] = rmagn[kbmagn[jtype - 1]];
		}

		if (kclass[jtype - 1] == 2 && n > 0) {

/*                 Include rotations */

/*                 Generate U, V as Householder transformations times */
/*                 a diagonal matrix. */

		    i__3 = n - 1;
		    for (jc = 1; jc <= i__3; ++jc) {
			i__4 = n;
			for (jr = jc; jr <= i__4; ++jr) {
			    u[jr + jc * u_dim1] = dlarnd_(&c__3, &iseed[1]);
			    v[jr + jc * v_dim1] = dlarnd_(&c__3, &iseed[1]);
/* L40: */
			}
			i__4 = n + 1 - jc;
			dlarfg_(&i__4, &u[jc + jc * u_dim1], &u[jc + 1 + jc * 
				u_dim1], &c__1, &work[jc]);
			work[(n << 1) + jc] = d_sign(&c_b19, &u[jc + jc * 
				u_dim1]);
			u[jc + jc * u_dim1] = 1.;
			i__4 = n + 1 - jc;
			dlarfg_(&i__4, &v[jc + jc * v_dim1], &v[jc + 1 + jc * 
				v_dim1], &c__1, &work[n + jc]);
			work[n * 3 + jc] = d_sign(&c_b19, &v[jc + jc * v_dim1]
				);
			v[jc + jc * v_dim1] = 1.;
/* L50: */
		    }
		    u[n + n * u_dim1] = 1.;
		    work[n] = 0.;
		    d__1 = dlarnd_(&c__2, &iseed[1]);
		    work[n * 3] = d_sign(&c_b19, &d__1);
		    v[n + n * v_dim1] = 1.;
		    work[n * 2] = 0.;
		    d__1 = dlarnd_(&c__2, &iseed[1]);
		    work[n * 4] = d_sign(&c_b19, &d__1);

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
		    dorm2r_("L", "N", &n, &n, &i__3, &u[u_offset], ldu, &work[
			    1], &a[a_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    dorm2r_("R", "T", &n, &n, &i__3, &v[v_offset], ldu, &work[
			    n + 1], &a[a_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    dorm2r_("L", "N", &n, &n, &i__3, &u[u_offset], ldu, &work[
			    1], &b[b_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
		    if (iinfo != 0) {
			goto L100;
		    }
		    i__3 = n - 1;
		    dorm2r_("R", "T", &n, &n, &i__3, &v[v_offset], ldu, &work[
			    n + 1], &b[b_offset], lda, &work[(n << 1) + 1], &
			    iinfo);
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
				dlarnd_(&c__2, &iseed[1]);
			b[jr + jc * b_dim1] = rmagn[kbmagn[jtype - 1]] * 
				dlarnd_(&c__2, &iseed[1]);
/* L80: */
		    }
/* L90: */
		}
	    }

	    anorm = dlange_("1", &n, &n, &a[a_offset], lda, &work[1]);
	    bnorm = dlange_("1", &n, &n, &b[b_offset], lda, &work[1]);

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

/*           Call DGEQR2, DORM2R, and DGGHRD to compute H, T, U, and V */

	    dlacpy_(" ", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
	    dlacpy_(" ", &n, &n, &b[b_offset], lda, &t[t_offset], lda);
	    ntest = 1;
	    result[1] = ulpinv;

	    dgeqr2_(&n, &n, &t[t_offset], lda, &work[1], &work[n + 1], &iinfo)
		    ;
	    if (iinfo != 0) {
		io___41.ciunit = *nounit;
		s_wsfe(&io___41);
		do_fio(&c__1, "DGEQR2", (ftnlen)6);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    dorm2r_("L", "T", &n, &n, &n, &t[t_offset], lda, &work[1], &h__[
		    h_offset], lda, &work[n + 1], &iinfo);
	    if (iinfo != 0) {
		io___42.ciunit = *nounit;
		s_wsfe(&io___42);
		do_fio(&c__1, "DORM2R", (ftnlen)6);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    dlaset_("Full", &n, &n, &c_b13, &c_b19, &u[u_offset], ldu);
	    dorm2r_("R", "N", &n, &n, &n, &t[t_offset], lda, &work[1], &u[
		    u_offset], ldu, &work[n + 1], &iinfo);
	    if (iinfo != 0) {
		io___43.ciunit = *nounit;
		s_wsfe(&io___43);
		do_fio(&c__1, "DORM2R", (ftnlen)6);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    dgghrd_("V", "I", &n, &c__1, &n, &h__[h_offset], lda, &t[t_offset]
, lda, &u[u_offset], ldu, &v[v_offset], ldu, &iinfo);
	    if (iinfo != 0) {
		io___44.ciunit = *nounit;
		s_wsfe(&io___44);
		do_fio(&c__1, "DGGHRD", (ftnlen)6);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }
	    ntest = 4;

/*           Do tests 1--4 */

	    dget51_(&c__1, &n, &a[a_offset], lda, &h__[h_offset], lda, &u[
		    u_offset], ldu, &v[v_offset], ldu, &work[1], &result[1]);
	    dget51_(&c__1, &n, &b[b_offset], lda, &t[t_offset], lda, &u[
		    u_offset], ldu, &v[v_offset], ldu, &work[1], &result[2]);
	    dget51_(&c__3, &n, &b[b_offset], lda, &t[t_offset], lda, &u[
		    u_offset], ldu, &u[u_offset], ldu, &work[1], &result[3]);
	    dget51_(&c__3, &n, &b[b_offset], lda, &t[t_offset], lda, &v[
		    v_offset], ldu, &v[v_offset], ldu, &work[1], &result[4]);

/*           Call DHGEQZ to compute S1, P1, S2, P2, Q, and Z, do tests. */

/*           Compute T1 and UZ */

/*           Eigenvalues only */

	    dlacpy_(" ", &n, &n, &h__[h_offset], lda, &s2[s2_offset], lda);
	    dlacpy_(" ", &n, &n, &t[t_offset], lda, &p2[p2_offset], lda);
	    ntest = 5;
	    result[5] = ulpinv;

	    dhgeqz_("E", "N", "N", &n, &c__1, &n, &s2[s2_offset], lda, &p2[
		    p2_offset], lda, &alphr3[1], &alphi3[1], &beta3[1], &q[
		    q_offset], ldu, &z__[z_offset], ldu, &work[1], lwork, &
		    iinfo);
	    if (iinfo != 0) {
		io___45.ciunit = *nounit;
		s_wsfe(&io___45);
		do_fio(&c__1, "DHGEQZ(E)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

/*           Eigenvalues and Full Schur Form */

	    dlacpy_(" ", &n, &n, &h__[h_offset], lda, &s2[s2_offset], lda);
	    dlacpy_(" ", &n, &n, &t[t_offset], lda, &p2[p2_offset], lda);

	    dhgeqz_("S", "N", "N", &n, &c__1, &n, &s2[s2_offset], lda, &p2[
		    p2_offset], lda, &alphr1[1], &alphi1[1], &beta1[1], &q[
		    q_offset], ldu, &z__[z_offset], ldu, &work[1], lwork, &
		    iinfo);
	    if (iinfo != 0) {
		io___46.ciunit = *nounit;
		s_wsfe(&io___46);
		do_fio(&c__1, "DHGEQZ(S)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

/*           Eigenvalues, Schur Form, and Schur Vectors */

	    dlacpy_(" ", &n, &n, &h__[h_offset], lda, &s1[s1_offset], lda);
	    dlacpy_(" ", &n, &n, &t[t_offset], lda, &p1[p1_offset], lda);

	    dhgeqz_("S", "I", "I", &n, &c__1, &n, &s1[s1_offset], lda, &p1[
		    p1_offset], lda, &alphr1[1], &alphi1[1], &beta1[1], &q[
		    q_offset], ldu, &z__[z_offset], ldu, &work[1], lwork, &
		    iinfo);
	    if (iinfo != 0) {
		io___47.ciunit = *nounit;
		s_wsfe(&io___47);
		do_fio(&c__1, "DHGEQZ(V)", (ftnlen)9);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    ntest = 8;

/*           Do Tests 5--8 */

	    dget51_(&c__1, &n, &h__[h_offset], lda, &s1[s1_offset], lda, &q[
		    q_offset], ldu, &z__[z_offset], ldu, &work[1], &result[5])
		    ;
	    dget51_(&c__1, &n, &t[t_offset], lda, &p1[p1_offset], lda, &q[
		    q_offset], ldu, &z__[z_offset], ldu, &work[1], &result[6])
		    ;
	    dget51_(&c__3, &n, &t[t_offset], lda, &p1[p1_offset], lda, &q[
		    q_offset], ldu, &q[q_offset], ldu, &work[1], &result[7]);
	    dget51_(&c__3, &n, &t[t_offset], lda, &p1[p1_offset], lda, &z__[
		    z_offset], ldu, &z__[z_offset], ldu, &work[1], &result[8])
		    ;

/*           Compute the Left and Right Eigenvectors of (S1,P1) */

/*           9: Compute the left eigenvector Matrix without */
/*              back transforming: */

	    ntest = 9;
	    result[9] = ulpinv;

/*           To test "SELECT" option, compute half of the eigenvectors */
/*           in one call, and half in another */

	    i1 = n / 2;
	    i__3 = i1;
	    for (j = 1; j <= i__3; ++j) {
		llwork[j] = TRUE_;
/* L120: */
	    }
	    i__3 = n;
	    for (j = i1 + 1; j <= i__3; ++j) {
		llwork[j] = FALSE_;
/* L130: */
	    }

	    dtgevc_("L", "S", &llwork[1], &n, &s1[s1_offset], lda, &p1[
		    p1_offset], lda, &evectl[evectl_offset], ldu, dumma, ldu, 
		    &n, &in, &work[1], &iinfo);
	    if (iinfo != 0) {
		io___50.ciunit = *nounit;
		s_wsfe(&io___50);
		do_fio(&c__1, "DTGEVC(L,S1)", (ftnlen)12);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    i1 = in;
	    i__3 = i1;
	    for (j = 1; j <= i__3; ++j) {
		llwork[j] = FALSE_;
/* L140: */
	    }
	    i__3 = n;
	    for (j = i1 + 1; j <= i__3; ++j) {
		llwork[j] = TRUE_;
/* L150: */
	    }

	    dtgevc_("L", "S", &llwork[1], &n, &s1[s1_offset], lda, &p1[
		    p1_offset], lda, &evectl[(i1 + 1) * evectl_dim1 + 1], ldu, 
		     dumma, ldu, &n, &in, &work[1], &iinfo);
	    if (iinfo != 0) {
		io___51.ciunit = *nounit;
		s_wsfe(&io___51);
		do_fio(&c__1, "DTGEVC(L,S2)", (ftnlen)12);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    dget52_(&c_true, &n, &s1[s1_offset], lda, &p1[p1_offset], lda, &
		    evectl[evectl_offset], ldu, &alphr1[1], &alphi1[1], &
		    beta1[1], &work[1], dumma);
	    result[9] = dumma[0];
	    if (dumma[1] > *thrshn) {
		io___52.ciunit = *nounit;
		s_wsfe(&io___52);
		do_fio(&c__1, "Left", (ftnlen)4);
		do_fio(&c__1, "DTGEVC(HOWMNY=S)", (ftnlen)16);
		do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           10: Compute the left eigenvector Matrix with */
/*               back transforming: */

	    ntest = 10;
	    result[10] = ulpinv;
	    dlacpy_("F", &n, &n, &q[q_offset], ldu, &evectl[evectl_offset], 
		    ldu);
	    dtgevc_("L", "B", &llwork[1], &n, &s1[s1_offset], lda, &p1[
		    p1_offset], lda, &evectl[evectl_offset], ldu, dumma, ldu, 
		    &n, &in, &work[1], &iinfo);
	    if (iinfo != 0) {
		io___53.ciunit = *nounit;
		s_wsfe(&io___53);
		do_fio(&c__1, "DTGEVC(L,B)", (ftnlen)11);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    dget52_(&c_true, &n, &h__[h_offset], lda, &t[t_offset], lda, &
		    evectl[evectl_offset], ldu, &alphr1[1], &alphi1[1], &
		    beta1[1], &work[1], dumma);
	    result[10] = dumma[0];
	    if (dumma[1] > *thrshn) {
		io___54.ciunit = *nounit;
		s_wsfe(&io___54);
		do_fio(&c__1, "Left", (ftnlen)4);
		do_fio(&c__1, "DTGEVC(HOWMNY=B)", (ftnlen)16);
		do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           11: Compute the right eigenvector Matrix without */
/*               back transforming: */

	    ntest = 11;
	    result[11] = ulpinv;

/*           To test "SELECT" option, compute half of the eigenvectors */
/*           in one call, and half in another */

	    i1 = n / 2;
	    i__3 = i1;
	    for (j = 1; j <= i__3; ++j) {
		llwork[j] = TRUE_;
/* L160: */
	    }
	    i__3 = n;
	    for (j = i1 + 1; j <= i__3; ++j) {
		llwork[j] = FALSE_;
/* L170: */
	    }

	    dtgevc_("R", "S", &llwork[1], &n, &s1[s1_offset], lda, &p1[
		    p1_offset], lda, dumma, ldu, &evectr[evectr_offset], ldu, 
		    &n, &in, &work[1], &iinfo);
	    if (iinfo != 0) {
		io___55.ciunit = *nounit;
		s_wsfe(&io___55);
		do_fio(&c__1, "DTGEVC(R,S1)", (ftnlen)12);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    i1 = in;
	    i__3 = i1;
	    for (j = 1; j <= i__3; ++j) {
		llwork[j] = FALSE_;
/* L180: */
	    }
	    i__3 = n;
	    for (j = i1 + 1; j <= i__3; ++j) {
		llwork[j] = TRUE_;
/* L190: */
	    }

	    dtgevc_("R", "S", &llwork[1], &n, &s1[s1_offset], lda, &p1[
		    p1_offset], lda, dumma, ldu, &evectr[(i1 + 1) * 
		    evectr_dim1 + 1], ldu, &n, &in, &work[1], &iinfo);
	    if (iinfo != 0) {
		io___56.ciunit = *nounit;
		s_wsfe(&io___56);
		do_fio(&c__1, "DTGEVC(R,S2)", (ftnlen)12);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    dget52_(&c_false, &n, &s1[s1_offset], lda, &p1[p1_offset], lda, &
		    evectr[evectr_offset], ldu, &alphr1[1], &alphi1[1], &
		    beta1[1], &work[1], dumma);
	    result[11] = dumma[0];
	    if (dumma[1] > *thresh) {
		io___57.ciunit = *nounit;
		s_wsfe(&io___57);
		do_fio(&c__1, "Right", (ftnlen)5);
		do_fio(&c__1, "DTGEVC(HOWMNY=S)", (ftnlen)16);
		do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           12: Compute the right eigenvector Matrix with */
/*               back transforming: */

	    ntest = 12;
	    result[12] = ulpinv;
	    dlacpy_("F", &n, &n, &z__[z_offset], ldu, &evectr[evectr_offset], 
		    ldu);
	    dtgevc_("R", "B", &llwork[1], &n, &s1[s1_offset], lda, &p1[
		    p1_offset], lda, dumma, ldu, &evectr[evectr_offset], ldu, 
		    &n, &in, &work[1], &iinfo);
	    if (iinfo != 0) {
		io___58.ciunit = *nounit;
		s_wsfe(&io___58);
		do_fio(&c__1, "DTGEVC(R,B)", (ftnlen)11);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
		*info = abs(iinfo);
		goto L210;
	    }

	    dget52_(&c_false, &n, &h__[h_offset], lda, &t[t_offset], lda, &
		    evectr[evectr_offset], ldu, &alphr1[1], &alphi1[1], &
		    beta1[1], &work[1], dumma);
	    result[12] = dumma[0];
	    if (dumma[1] > *thresh) {
		io___59.ciunit = *nounit;
		s_wsfe(&io___59);
		do_fio(&c__1, "Right", (ftnlen)5);
		do_fio(&c__1, "DTGEVC(HOWMNY=B)", (ftnlen)16);
		do_fio(&c__1, (char *)&dumma[1], (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer));
		e_wsfe();
	    }

/*           Tests 13--15 are done only on request */

	    if (*tstdif) {

/*              Do Tests 13--14 */

		dget51_(&c__2, &n, &s1[s1_offset], lda, &s2[s2_offset], lda, &
			q[q_offset], ldu, &z__[z_offset], ldu, &work[1], &
			result[13]);
		dget51_(&c__2, &n, &p1[p1_offset], lda, &p2[p2_offset], lda, &
			q[q_offset], ldu, &z__[z_offset], ldu, &work[1], &
			result[14]);

/*              Do Test 15 */

		temp1 = 0.;
		temp2 = 0.;
		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
/* Computing MAX */
		    d__3 = temp1, d__4 = (d__1 = alphr1[j] - alphr3[j], abs(
			    d__1)) + (d__2 = alphi1[j] - alphi3[j], abs(d__2))
			    ;
		    temp1 = max(d__3,d__4);
/* Computing MAX */
		    d__2 = temp2, d__3 = (d__1 = beta1[j] - beta3[j], abs(
			    d__1));
		    temp2 = max(d__2,d__3);
/* L200: */
		}

/* Computing MAX */
		d__1 = safmin, d__2 = ulp * max(temp1,anorm);
		temp1 /= max(d__1,d__2);
/* Computing MAX */
		d__1 = safmin, d__2 = ulp * max(temp2,bnorm);
		temp2 /= max(d__1,d__2);
		result[15] = max(temp1,temp2);
		ntest = 15;
	    } else {
		result[13] = 0.;
		result[14] = 0.;
		result[15] = 0.;
		ntest = 12;
	    }

/*           End of Loop -- Check for RESULT(j) > THRESH */

L210:

	    ntestt += ntest;

/*           Print out tests which fail. */

	    i__3 = ntest;
	    for (jr = 1; jr <= i__3; ++jr) {
		if (result[jr] >= *thresh) {

/*                 If this is the first test to fail, */
/*                 print a header to the data file. */

		    if (nerrs == 0) {
			io___62.ciunit = *nounit;
			s_wsfe(&io___62);
			do_fio(&c__1, "DGG", (ftnlen)3);
			e_wsfe();

/*                    Matrix types */

			io___63.ciunit = *nounit;
			s_wsfe(&io___63);
			e_wsfe();
			io___64.ciunit = *nounit;
			s_wsfe(&io___64);
			e_wsfe();
			io___65.ciunit = *nounit;
			s_wsfe(&io___65);
			do_fio(&c__1, "Orthogonal", (ftnlen)10);
			e_wsfe();

/*                    Tests performed */

			io___66.ciunit = *nounit;
			s_wsfe(&io___66);
			do_fio(&c__1, "orthogonal", (ftnlen)10);
			do_fio(&c__1, "'", (ftnlen)1);
			do_fio(&c__1, "transpose", (ftnlen)9);
			for (j = 1; j <= 10; ++j) {
			    do_fio(&c__1, "'", (ftnlen)1);
			}
			e_wsfe();

		    }
		    ++nerrs;
		    if (result[jr] < 1e4) {
			io___67.ciunit = *nounit;
			s_wsfe(&io___67);
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
			io___68.ciunit = *nounit;
			s_wsfe(&io___68);
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
/* L220: */
	    }

L230:
	    ;
	}
/* L240: */
    }

/*     Summary */

    dlasum_("DGG", nounit, &nerrs, &ntestt);
    return 0;








/*     End of DCHKGG */

} /* dchkgg_ */
