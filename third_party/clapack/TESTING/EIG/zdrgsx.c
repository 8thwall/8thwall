#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer m, n, mplusn, k;
    logical fs;
} mn_;

#define mn_1 mn_

/* Table of constant values */

static doublecomplex c_b1 = {0.,0.};
static integer c__1 = 1;
static integer c__0 = 0;
static integer c_n1 = -1;
static integer c__3 = 3;
static integer c__7 = 7;
static integer c__5 = 5;

/* Subroutine */ int zdrgsx_(integer *nsize, integer *ncmax, doublereal *
	thresh, integer *nin, integer *nout, doublecomplex *a, integer *lda, 
	doublecomplex *b, doublecomplex *ai, doublecomplex *bi, doublecomplex 
	*z__, doublecomplex *q, doublecomplex *alpha, doublecomplex *beta, 
	doublecomplex *c__, integer *ldc, doublereal *s, doublecomplex *work, 
	integer *lwork, doublereal *rwork, integer *iwork, integer *liwork, 
	logical *bwork, integer *info)
{
    /* Format strings */
    static char fmt_9999[] = "(\002 ZDRGSX: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002)\002)";
    static char fmt_9997[] = "(\002 ZDRGSX: S not in Schur form at eigenvalu"
	    "e \002,i6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002"
	    ")\002)";
    static char fmt_9996[] = "(/1x,a3,\002 -- Complex Expert Generalized Sch"
	    "ur form\002,\002 problem driver\002)";
    static char fmt_9994[] = "(\002 Matrix types: \002,/\002  1:  A is a blo"
	    "ck diagonal matrix of Jordan blocks \002,\002and B is the identi"
	    "ty \002,/\002      matrix, \002,/\002  2:  A and B are upper tri"
	    "angular matrices, \002,/\002  3:  A and B are as type 2, but eac"
	    "h second diagonal \002,\002block in A_11 and \002,/\002      eac"
	    "h third diaongal block in A_22 are 2x2 blocks,\002,/\002  4:  A "
	    "and B are block diagonal matrices, \002,/\002  5:  (A,B) has pot"
	    "entially close or common \002,\002eigenvalues.\002,/)";
    static char fmt_9993[] = "(/\002 Tests performed:  (S is Schur, T is tri"
	    "angular, \002,\002Q and Z are \002,a,\002,\002,/19x,\002 a is al"
	    "pha, b is beta, and \002,a,\002 means \002,a,\002.)\002,/\002  1"
	    " = | A - Q S Z\002,a,\002 | / ( |A| n ulp )      2 = | B - Q T "
	    "Z\002,a,\002 | / ( |B| n ulp )\002,/\002  3 = | I - QQ\002,a,"
	    "\002 | / ( n ulp )             4 = | I - ZZ\002,a,\002 | / ( n u"
	    "lp )\002,/\002  5 = 1/ULP  if A is not in \002,\002Schur form "
	    "S\002,/\002  6 = difference between (alpha,beta)\002,\002 and di"
	    "agonals of (S,T)\002,/\002  7 = 1/ULP  if SDIM is not the correc"
	    "t number of \002,\002selected eigenvalues\002,/\002  8 = 1/ULP  "
	    "if DIFEST/DIFTRU > 10*THRESH or \002,\002DIFTRU/DIFEST > 10*THRE"
	    "SH\002,/\002  9 = 1/ULP  if DIFEST <> 0 or DIFTRU > ULP*norm(A,B"
	    ") \002,\002when reordering fails\002,/\002 10 = 1/ULP  if PLEST/"
	    "PLTRU > THRESH or \002,\002PLTRU/PLEST > THRESH\002,/\002    ( T"
	    "est 10 is only for input examples )\002,/)";
    static char fmt_9992[] = "(\002 Matrix order=\002,i2,\002, type=\002,i2"
	    ",\002, a=\002,d10.4,\002, order(A_11)=\002,i2,\002, result \002,"
	    "i2,\002 is \002,0p,f8.2)";
    static char fmt_9991[] = "(\002 Matrix order=\002,i2,\002, type=\002,i2"
	    ",\002, a=\002,d10.4,\002, order(A_11)=\002,i2,\002, result \002,"
	    "i2,\002 is \002,0p,d10.4)";
    static char fmt_9998[] = "(\002 ZDRGSX: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, Input Example #\002,i2,\002"
	    ")\002)";
    static char fmt_9995[] = "(\002Input Example\002)";
    static char fmt_9990[] = "(\002 Input example #\002,i2,\002, matrix orde"
	    "r=\002,i4,\002,\002,\002 result \002,i2,\002 is\002,0p,f8.2)";
    static char fmt_9989[] = "(\002 Input example #\002,i2,\002, matrix orde"
	    "r=\002,i4,\002,\002,\002 result \002,i2,\002 is\002,1p,d10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, ai_dim1, ai_offset, b_dim1, b_offset, bi_dim1, 
	    bi_offset, c_dim1, c_offset, q_dim1, q_offset, z_dim1, z_offset, 
	    i__1, i__2, i__3, i__4, i__5, i__6, i__7, i__8, i__9, i__10, 
	    i__11;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, d__10, 
	    d__11, d__12, d__13, d__14, d__15, d__16;
    doublecomplex z__1, z__2, z__3, z__4;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double d_imag(doublecomplex *);
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);

    /* Local variables */
    integer i__, j, mm;
    doublereal pl[2];
    integer mn2, qba, qbb;
    doublereal ulp, temp1, temp2, abnrm;
    integer ifunc, linfo;
    char sense[1];
    extern /* Subroutine */ int zget51_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, integer *, doublecomplex *, integer *
, doublecomplex *, integer *, doublecomplex *, doublereal *, 
	    doublereal *);
    integer nerrs, ntest;
    doublereal pltru;
    extern /* Subroutine */ int zlakf2_(integer *, integer *, doublecomplex *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    doublecomplex *, integer *), dlabad_(doublereal *, doublereal *);
    doublereal thrsh2;
    logical ilabad;
    extern /* Subroutine */ int zlatm5_(integer *, integer *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *, doublecomplex *, integer *, 
	    doublereal *, integer *, integer *);
    extern doublereal dlamch_(char *);
    integer bdspac;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    doublereal difest[2];
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *, 
	    integer *, integer *);
    extern doublereal zlange_(char *, integer *, integer *, doublecomplex *, 
	    integer *, doublereal *);
    doublereal bignum;
    extern /* Subroutine */ int alasvm_(char *, integer *, integer *, integer 
	    *, integer *);
    doublereal weight, diftru;
    extern /* Subroutine */ int zgesvd_(char *, char *, integer *, integer *, 
	    doublecomplex *, integer *, doublereal *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     doublereal *, integer *), zlacpy_(char *, 
	    integer *, integer *, doublecomplex *, integer *, doublecomplex *, 
	     integer *), zlaset_(char *, integer *, integer *, 
	    doublecomplex *, doublecomplex *, doublecomplex *, integer *);
    integer minwrk, maxwrk;
    extern /* Subroutine */ int zggesx_(char *, char *, char *, L_fp, char *, 
	    integer *, doublecomplex *, integer *, doublecomplex *, integer *, 
	     integer *, doublecomplex *, doublecomplex *, doublecomplex *, 
	    integer *, doublecomplex *, integer *, doublereal *, doublereal *, 
	     doublecomplex *, integer *, doublereal *, integer *, integer *, 
	    logical *, integer *);
    doublereal smlnum, ulpinv;
    integer nptknt;
    doublereal result[10];
    integer ntestt, prtype;
    extern logical zlctsx_();

    /* Fortran I/O blocks */
    static cilist io___22 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___39 = { 0, 0, 1, 0, 0 };
    static cilist io___40 = { 0, 0, 1, 0, 0 };
    static cilist io___41 = { 0, 0, 0, 0, 0 };
    static cilist io___42 = { 0, 0, 0, 0, 0 };
    static cilist io___43 = { 0, 0, 0, 0, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9990, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9989, 0 };



/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     February 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZDRGSX checks the nonsymmetric generalized eigenvalue (Schur form) */
/*  problem expert driver ZGGESX. */

/*  ZGGES factors A and B as Q*S*Z'  and Q*T*Z' , where ' means conjugate */
/*  transpose, S and T are  upper triangular (i.e., in generalized Schur */
/*  form), and Q and Z are unitary. It also computes the generalized */
/*  eigenvalues (alpha(j),beta(j)), j=1,...,n.  Thus, */
/*  w(j) = alpha(j)/beta(j) is a root of the characteristic equation */

/*                  det( A - w(j) B ) = 0 */

/*  Optionally it also reorders the eigenvalues so that a selected */
/*  cluster of eigenvalues appears in the leading diagonal block of the */
/*  Schur forms; computes a reciprocal condition number for the average */
/*  of the selected eigenvalues; and computes a reciprocal condition */
/*  number for the right and left deflating subspaces corresponding to */
/*  the selected eigenvalues. */

/*  When ZDRGSX is called with NSIZE > 0, five (5) types of built-in */
/*  matrix pairs are used to test the routine ZGGESX. */

/*  When ZDRGSX is called with NSIZE = 0, it reads in test matrix data */
/*  to test ZGGESX. */
/*  (need more details on what kind of read-in data are needed). */

/*  For each matrix pair, the following tests will be performed and */
/*  compared with the threshhold THRESH except for the tests (7) and (9): */

/*  (1)   | A - Q S Z' | / ( |A| n ulp ) */

/*  (2)   | B - Q T Z' | / ( |B| n ulp ) */

/*  (3)   | I - QQ' | / ( n ulp ) */

/*  (4)   | I - ZZ' | / ( n ulp ) */

/*  (5)   if A is in Schur form (i.e. triangular form) */

/*  (6)   maximum over j of D(j)  where: */

/*                      |alpha(j) - S(j,j)|        |beta(j) - T(j,j)| */
/*            D(j) = ------------------------ + ----------------------- */
/*                   max(|alpha(j)|,|S(j,j)|)   max(|beta(j)|,|T(j,j)|) */

/*  (7)   if sorting worked and SDIM is the number of eigenvalues */
/*        which were selected. */

/*  (8)   the estimated value DIF does not differ from the true values of */
/*        Difu and Difl more than a factor 10*THRESH. If the estimate DIF */
/*        equals zero the corresponding true values of Difu and Difl */
/*        should be less than EPS*norm(A, B). If the true value of Difu */
/*        and Difl equal zero, the estimate DIF should be less than */
/*        EPS*norm(A, B). */

/*  (9)   If INFO = N+3 is returned by ZGGESX, the reordering "failed" */
/*        and we check that DIF = PL = PR = 0 and that the true value of */
/*        Difu and Difl is < EPS*norm(A, B). We count the events when */
/*        INFO=N+3. */

/*  For read-in test matrices, the same tests are run except that the */
/*  exact value for DIF (and PL) is input data.  Additionally, there is */
/*  one more test run for read-in test matrices: */

/*  (10)  the estimated value PL does not differ from the true value of */
/*        PLTRU more than a factor THRESH. If the estimate PL equals */
/*        zero the corresponding true value of PLTRU should be less than */
/*        EPS*norm(A, B). If the true value of PLTRU equal zero, the */
/*        estimate PL should be less than EPS*norm(A, B). */

/*  Note that for the built-in tests, a total of 10*NSIZE*(NSIZE-1) */
/*  matrix pairs are generated and tested. NSIZE should be kept small. */

/*  SVD (routine ZGESVD) is used for computing the true value of DIF_u */
/*  and DIF_l when testing the built-in test problems. */

/*  Built-in Test Matrices */
/*  ====================== */

/*  All built-in test matrices are the 2 by 2 block of triangular */
/*  matrices */

/*           A = [ A11 A12 ]    and      B = [ B11 B12 ] */
/*               [     A22 ]                 [     B22 ] */

/*  where for different type of A11 and A22 are given as the following. */
/*  A12 and B12 are chosen so that the generalized Sylvester equation */

/*           A11*R - L*A22 = -A12 */
/*           B11*R - L*B22 = -B12 */

/*  have prescribed solution R and L. */

/*  Type 1:  A11 = J_m(1,-1) and A_22 = J_k(1-a,1). */
/*           B11 = I_m, B22 = I_k */
/*           where J_k(a,b) is the k-by-k Jordan block with ``a'' on */
/*           diagonal and ``b'' on superdiagonal. */

/*  Type 2:  A11 = (a_ij) = ( 2(.5-sin(i)) ) and */
/*           B11 = (b_ij) = ( 2(.5-sin(ij)) ) for i=1,...,m, j=i,...,m */
/*           A22 = (a_ij) = ( 2(.5-sin(i+j)) ) and */
/*           B22 = (b_ij) = ( 2(.5-sin(ij)) ) for i=m+1,...,k, j=i,...,k */

/*  Type 3:  A11, A22 and B11, B22 are chosen as for Type 2, but each */
/*           second diagonal block in A_11 and each third diagonal block */
/*           in A_22 are made as 2 by 2 blocks. */

/*  Type 4:  A11 = ( 20(.5 - sin(ij)) ) and B22 = ( 2(.5 - sin(i+j)) ) */
/*              for i=1,...,m,  j=1,...,m and */
/*           A22 = ( 20(.5 - sin(i+j)) ) and B22 = ( 2(.5 - sin(ij)) ) */
/*              for i=m+1,...,k,  j=m+1,...,k */

/*  Type 5:  (A,B) and have potentially close or common eigenvalues and */
/*           very large departure from block diagonality A_11 is chosen */
/*           as the m x m leading submatrix of A_1: */
/*                   |  1  b                            | */
/*                   | -b  1                            | */
/*                   |        1+d  b                    | */
/*                   |         -b 1+d                   | */
/*            A_1 =  |                  d  1            | */
/*                   |                 -1  d            | */
/*                   |                        -d  1     | */
/*                   |                        -1 -d     | */
/*                   |                               1  | */
/*           and A_22 is chosen as the k x k leading submatrix of A_2: */
/*                   | -1  b                            | */
/*                   | -b -1                            | */
/*                   |       1-d  b                     | */
/*                   |       -b  1-d                    | */
/*            A_2 =  |                 d 1+b            | */
/*                   |               -1-b d             | */
/*                   |                       -d  1+b    | */
/*                   |                      -1+b  -d    | */
/*                   |                              1-d | */
/*           and matrix B are chosen as identity matrices (see DLATM5). */


/*  Arguments */
/*  ========= */

/*  NSIZE   (input) INTEGER */
/*          The maximum size of the matrices to use. NSIZE >= 0. */
/*          If NSIZE = 0, no built-in tests matrices are used, but */
/*          read-in test matrices are used to test DGGESX. */

/*  NCMAX   (input) INTEGER */
/*          Maximum allowable NMAX for generating Kroneker matrix */
/*          in call to ZLAKF2 */

/*  THRESH  (input) DOUBLE PRECISION */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error */
/*          is scaled to be O(1), so THRESH should be a reasonably */
/*          small multiple of 1, e.g., 10 or 100.  In particular, */
/*          it should not depend on the precision (single vs. double) */
/*          or the size of the matrix.  THRESH >= 0. */

/*  NIN     (input) INTEGER */
/*          The FORTRAN unit number for reading in the data file of */
/*          problems to solve. */

/*  NOUT    (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns INFO not equal to 0.) */

/*  A       (workspace) COMPLEX*16 array, dimension (LDA, NSIZE) */
/*          Used to store the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, B, AI, BI, Z and Q, */
/*          LDA >= max( 1, NSIZE ). For the read-in test, */
/*          LDA >= max( 1, N ), N is the size of the test matrices. */

/*  B       (workspace) COMPLEX*16 array, dimension (LDA, NSIZE) */
/*          Used to store the matrix whose eigenvalues are to be */
/*          computed.  On exit, B contains the last matrix actually used. */

/*  AI      (workspace) COMPLEX*16 array, dimension (LDA, NSIZE) */
/*          Copy of A, modified by ZGGESX. */

/*  BI      (workspace) COMPLEX*16 array, dimension (LDA, NSIZE) */
/*          Copy of B, modified by ZGGESX. */

/*  Z       (workspace) COMPLEX*16 array, dimension (LDA, NSIZE) */
/*          Z holds the left Schur vectors computed by ZGGESX. */

/*  Q       (workspace) COMPLEX*16 array, dimension (LDA, NSIZE) */
/*          Q holds the right Schur vectors computed by ZGGESX. */

/*  ALPHA   (workspace) COMPLEX*16 array, dimension (NSIZE) */
/*  BETA    (workspace) COMPLEX*16 array, dimension (NSIZE) */
/*          On exit, ALPHA/BETA are the eigenvalues. */

/*  C       (workspace) COMPLEX*16 array, dimension (LDC, LDC) */
/*          Store the matrix generated by subroutine ZLAKF2, this is the */
/*          matrix formed by Kronecker products used for estimating */
/*          DIF. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of C. LDC >= max(1, LDA*LDA/2 ). */

/*  S       (workspace) DOUBLE PRECISION array, dimension (LDC) */
/*          Singular values of C */

/*  WORK    (workspace) COMPLEX*16 array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= 3*NSIZE*NSIZE/2 */

/*  RWORK   (workspace) DOUBLE PRECISION array, */
/*                                 dimension (5*NSIZE*NSIZE/2 - 4) */

/*  IWORK   (workspace) INTEGER array, dimension (LIWORK) */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. LIWORK >= NSIZE + 2. */

/*  BWORK   (workspace) LOGICAL array, dimension (NSIZE) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  A routine returned an error code. */

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
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

    /* Parameter adjustments */
    q_dim1 = *lda;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    z_dim1 = *lda;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    bi_dim1 = *lda;
    bi_offset = 1 + bi_dim1;
    bi -= bi_offset;
    ai_dim1 = *lda;
    ai_offset = 1 + ai_dim1;
    ai -= ai_offset;
    b_dim1 = *lda;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --alpha;
    --beta;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --s;
    --work;
    --rwork;
    --iwork;
    --bwork;

    /* Function Body */
    *info = 0;
    if (*nsize < 0) {
	*info = -1;
    } else if (*thresh < 0.) {
	*info = -2;
    } else if (*nin <= 0) {
	*info = -3;
    } else if (*nout <= 0) {
	*info = -4;
    } else if (*lda < 1 || *lda < *nsize) {
	*info = -6;
    } else if (*ldc < 1 || *ldc < *nsize * *nsize / 2) {
	*info = -15;
    } else if (*liwork < *nsize + 2) {
	*info = -21;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV.) */

    minwrk = 1;
    if (*info == 0 && *lwork >= 1) {
	minwrk = *nsize * 3 * *nsize / 2;

/*        workspace for cggesx */

	maxwrk = *nsize * (ilaenv_(&c__1, "ZGEQRF", " ", nsize, &c__1, nsize, 
		&c__0) + 1);
/* Computing MAX */
	i__1 = maxwrk, i__2 = *nsize * (ilaenv_(&c__1, "ZUNGQR", " ", nsize, &
		c__1, nsize, &c_n1) + 1);
	maxwrk = max(i__1,i__2);

/*        workspace for zgesvd */

	bdspac = *nsize * 3 * *nsize / 2;
/* Computing MAX */
	i__3 = *nsize * *nsize / 2;
	i__4 = *nsize * *nsize / 2;
	i__1 = maxwrk, i__2 = *nsize * *nsize * (ilaenv_(&c__1, "ZGEBRD", 
		" ", &i__3, &i__4, &c_n1, &c_n1) + 1);
	maxwrk = max(i__1,i__2);
	maxwrk = max(maxwrk,bdspac);

	maxwrk = max(maxwrk,minwrk);

	work[1].r = (doublereal) maxwrk, work[1].i = 0.;
    }

    if (*lwork < minwrk) {
	*info = -18;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("ZDRGSX", &i__1);
	return 0;
    }

/*     Important constants */

    ulp = dlamch_("P");
    ulpinv = 1. / ulp;
    smlnum = dlamch_("S") / ulp;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    thrsh2 = *thresh * 10.;
    ntestt = 0;
    nerrs = 0;

/*     Go to the tests for read-in matrix pairs */

    ifunc = 0;
    if (*nsize == 0) {
	goto L70;
    }

/*     Test the built-in matrix pairs. */
/*     Loop over different functions (IFUNC) of ZGGESX, types (PRTYPE) */
/*     of test matrices, different size (M+N) */

    prtype = 0;
    qba = 3;
    qbb = 4;
    weight = sqrt(ulp);

    for (ifunc = 0; ifunc <= 3; ++ifunc) {
	for (prtype = 1; prtype <= 5; ++prtype) {
	    i__1 = *nsize - 1;
	    for (mn_1.m = 1; mn_1.m <= i__1; ++mn_1.m) {
		i__2 = *nsize - mn_1.m;
		for (mn_1.n = 1; mn_1.n <= i__2; ++mn_1.n) {

		    weight = 1. / weight;
		    mn_1.mplusn = mn_1.m + mn_1.n;

/*                 Generate test matrices */

		    mn_1.fs = TRUE_;
		    mn_1.k = 0;

		    zlaset_("Full", &mn_1.mplusn, &mn_1.mplusn, &c_b1, &c_b1, 
			    &ai[ai_offset], lda);
		    zlaset_("Full", &mn_1.mplusn, &mn_1.mplusn, &c_b1, &c_b1, 
			    &bi[bi_offset], lda);

		    zlatm5_(&prtype, &mn_1.m, &mn_1.n, &ai[ai_offset], lda, &
			    ai[mn_1.m + 1 + (mn_1.m + 1) * ai_dim1], lda, &ai[
			    (mn_1.m + 1) * ai_dim1 + 1], lda, &bi[bi_offset], 
			    lda, &bi[mn_1.m + 1 + (mn_1.m + 1) * bi_dim1], 
			    lda, &bi[(mn_1.m + 1) * bi_dim1 + 1], lda, &q[
			    q_offset], lda, &z__[z_offset], lda, &weight, &
			    qba, &qbb);

/*                 Compute the Schur factorization and swapping the */
/*                 m-by-m (1,1)-blocks with n-by-n (2,2)-blocks. */
/*                 Swapping is accomplished via the function ZLCTSX */
/*                 which is supplied below. */

		    if (ifunc == 0) {
			*(unsigned char *)sense = 'N';
		    } else if (ifunc == 1) {
			*(unsigned char *)sense = 'E';
		    } else if (ifunc == 2) {
			*(unsigned char *)sense = 'V';
		    } else if (ifunc == 3) {
			*(unsigned char *)sense = 'B';
		    }

		    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &ai[ai_offset]
, lda, &a[a_offset], lda);
		    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &bi[bi_offset]
, lda, &b[b_offset], lda);

		    zggesx_("V", "V", "S", (L_fp)zlctsx_, sense, &mn_1.mplusn, 
			     &ai[ai_offset], lda, &bi[bi_offset], lda, &mm, &
			    alpha[1], &beta[1], &q[q_offset], lda, &z__[
			    z_offset], lda, pl, difest, &work[1], lwork, &
			    rwork[1], &iwork[1], liwork, &bwork[1], &linfo);

		    if (linfo != 0 && linfo != mn_1.mplusn + 2) {
			result[0] = ulpinv;
			io___22.ciunit = *nout;
			s_wsfe(&io___22);
			do_fio(&c__1, "ZGGESX", (ftnlen)6);
			do_fio(&c__1, (char *)&linfo, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&prtype, (ftnlen)sizeof(integer)
				);
			e_wsfe();
			*info = linfo;
			goto L30;
		    }

/*                 Compute the norm(A, B) */

		    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &ai[ai_offset]
, lda, &work[1], &mn_1.mplusn);
		    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &bi[bi_offset]
, lda, &work[mn_1.mplusn * mn_1.mplusn + 1], &
			    mn_1.mplusn);
		    i__3 = mn_1.mplusn << 1;
		    abnrm = zlange_("Fro", &mn_1.mplusn, &i__3, &work[1], &
			    mn_1.mplusn, &rwork[1]);

/*                 Do tests (1) to (4) */

		    result[1] = 0.;
		    zget51_(&c__1, &mn_1.mplusn, &a[a_offset], lda, &ai[
			    ai_offset], lda, &q[q_offset], lda, &z__[z_offset]
, lda, &work[1], &rwork[1], result);
		    zget51_(&c__1, &mn_1.mplusn, &b[b_offset], lda, &bi[
			    bi_offset], lda, &q[q_offset], lda, &z__[z_offset]
, lda, &work[1], &rwork[1], &result[1]);
		    zget51_(&c__3, &mn_1.mplusn, &b[b_offset], lda, &bi[
			    bi_offset], lda, &q[q_offset], lda, &q[q_offset], 
			    lda, &work[1], &rwork[1], &result[2]);
		    zget51_(&c__3, &mn_1.mplusn, &b[b_offset], lda, &bi[
			    bi_offset], lda, &z__[z_offset], lda, &z__[
			    z_offset], lda, &work[1], &rwork[1], &result[3]);
		    ntest = 4;

/*                 Do tests (5) and (6): check Schur form of A and */
/*                 compare eigenvalues with diagonals. */

		    temp1 = 0.;
		    result[4] = 0.;
		    result[5] = 0.;

		    i__3 = mn_1.mplusn;
		    for (j = 1; j <= i__3; ++j) {
			ilabad = FALSE_;
			i__4 = j;
			i__5 = j + j * ai_dim1;
			z__2.r = alpha[i__4].r - ai[i__5].r, z__2.i = alpha[
				i__4].i - ai[i__5].i;
			z__1.r = z__2.r, z__1.i = z__2.i;
			i__6 = j;
			i__7 = j + j * bi_dim1;
			z__4.r = beta[i__6].r - bi[i__7].r, z__4.i = beta[
				i__6].i - bi[i__7].i;
			z__3.r = z__4.r, z__3.i = z__4.i;
/* Computing MAX */
			i__8 = j;
			i__9 = j + j * ai_dim1;
			d__13 = smlnum, d__14 = (d__1 = alpha[i__8].r, abs(
				d__1)) + (d__2 = d_imag(&alpha[j]), abs(d__2))
				, d__13 = max(d__13,d__14), d__14 = (d__3 = 
				ai[i__9].r, abs(d__3)) + (d__4 = d_imag(&ai[j 
				+ j * ai_dim1]), abs(d__4));
/* Computing MAX */
			i__10 = j;
			i__11 = j + j * bi_dim1;
			d__15 = smlnum, d__16 = (d__5 = beta[i__10].r, abs(
				d__5)) + (d__6 = d_imag(&beta[j]), abs(d__6)),
				 d__15 = max(d__15,d__16), d__16 = (d__7 = bi[
				i__11].r, abs(d__7)) + (d__8 = d_imag(&bi[j + 
				j * bi_dim1]), abs(d__8));
			temp2 = (((d__9 = z__1.r, abs(d__9)) + (d__10 = 
				d_imag(&z__1), abs(d__10))) / max(d__13,d__14)
				 + ((d__11 = z__3.r, abs(d__11)) + (d__12 = 
				d_imag(&z__3), abs(d__12))) / max(d__15,d__16)
				) / ulp;
			if (j < mn_1.mplusn) {
			    i__4 = j + 1 + j * ai_dim1;
			    if (ai[i__4].r != 0. || ai[i__4].i != 0.) {
				ilabad = TRUE_;
				result[4] = ulpinv;
			    }
			}
			if (j > 1) {
			    i__4 = j + (j - 1) * ai_dim1;
			    if (ai[i__4].r != 0. || ai[i__4].i != 0.) {
				ilabad = TRUE_;
				result[4] = ulpinv;
			    }
			}
			temp1 = max(temp1,temp2);
			if (ilabad) {
			    io___29.ciunit = *nout;
			    s_wsfe(&io___29);
			    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)
				    sizeof(integer));
			    do_fio(&c__1, (char *)&prtype, (ftnlen)sizeof(
				    integer));
			    e_wsfe();
			}
/* L10: */
		    }
		    result[5] = temp1;
		    ntest += 2;

/*                 Test (7) (if sorting worked) */

		    result[6] = 0.;
		    if (linfo == mn_1.mplusn + 3) {
			result[6] = ulpinv;
		    } else if (mm != mn_1.n) {
			result[6] = ulpinv;
		    }
		    ++ntest;

/*                 Test (8): compare the estimated value DIF and its */
/*                 value. first, compute the exact DIF. */

		    result[7] = 0.;
		    mn2 = mm * (mn_1.mplusn - mm) << 1;
		    if (ifunc >= 2 && mn2 <= *ncmax * *ncmax) {

/*                    Note: for either following two cases, there are */
/*                    almost same number of test cases fail the test. */

			i__3 = mn_1.mplusn - mm;
			zlakf2_(&mm, &i__3, &ai[ai_offset], lda, &ai[mm + 1 + 
				(mm + 1) * ai_dim1], &bi[bi_offset], &bi[mm + 
				1 + (mm + 1) * bi_dim1], &c__[c_offset], ldc);

			i__3 = *lwork - 2;
			zgesvd_("N", "N", &mn2, &mn2, &c__[c_offset], ldc, &s[
				1], &work[1], &c__1, &work[2], &c__1, &work[3]
, &i__3, &rwork[1], info);
			diftru = s[mn2];

			if (difest[1] == 0.) {
			    if (diftru > abnrm * ulp) {
				result[7] = ulpinv;
			    }
			} else if (diftru == 0.) {
			    if (difest[1] > abnrm * ulp) {
				result[7] = ulpinv;
			    }
			} else if (diftru > thrsh2 * difest[1] || diftru * 
				thrsh2 < difest[1]) {
/* Computing MAX */
			    d__1 = diftru / difest[1], d__2 = difest[1] / 
				    diftru;
			    result[7] = max(d__1,d__2);
			}
			++ntest;
		    }

/*                 Test (9) */

		    result[8] = 0.;
		    if (linfo == mn_1.mplusn + 2) {
			if (diftru > abnrm * ulp) {
			    result[8] = ulpinv;
			}
			if (ifunc > 1 && difest[1] != 0.) {
			    result[8] = ulpinv;
			}
			if (ifunc == 1 && pl[0] != 0.) {
			    result[8] = ulpinv;
			}
			++ntest;
		    }

		    ntestt += ntest;

/*                 Print out tests which fail. */

		    for (j = 1; j <= 9; ++j) {
			if (result[j - 1] >= *thresh) {

/*                       If this is the first test to fail, */
/*                       print a header to the data file. */

			    if (nerrs == 0) {
				io___32.ciunit = *nout;
				s_wsfe(&io___32);
				do_fio(&c__1, "CGX", (ftnlen)3);
				e_wsfe();

/*                          Matrix types */

				io___33.ciunit = *nout;
				s_wsfe(&io___33);
				e_wsfe();

/*                          Tests performed */

				io___34.ciunit = *nout;
				s_wsfe(&io___34);
				do_fio(&c__1, "unitary", (ftnlen)7);
				do_fio(&c__1, "'", (ftnlen)1);
				do_fio(&c__1, "transpose", (ftnlen)9);
				for (i__ = 1; i__ <= 4; ++i__) {
				    do_fio(&c__1, "'", (ftnlen)1);
				}
				e_wsfe();

			    }
			    ++nerrs;
			    if (result[j - 1] < 1e4) {
				io___36.ciunit = *nout;
				s_wsfe(&io___36);
				do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&prtype, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&weight, (ftnlen)sizeof(
					doublereal));
				do_fio(&c__1, (char *)&mn_1.m, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&j, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[j - 1], (ftnlen)
					sizeof(doublereal));
				e_wsfe();
			    } else {
				io___37.ciunit = *nout;
				s_wsfe(&io___37);
				do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)
					sizeof(integer));
				do_fio(&c__1, (char *)&prtype, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&weight, (ftnlen)sizeof(
					doublereal));
				do_fio(&c__1, (char *)&mn_1.m, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&j, (ftnlen)sizeof(
					integer));
				do_fio(&c__1, (char *)&result[j - 1], (ftnlen)
					sizeof(doublereal));
				e_wsfe();
			    }
			}
/* L20: */
		    }

L30:
		    ;
		}
/* L40: */
	    }
/* L50: */
	}
/* L60: */
    }

    goto L150;

L70:

/*     Read in data from file to check accuracy of condition estimation */
/*     Read input data until N=0 */

    nptknt = 0;

L80:
    io___39.ciunit = *nin;
    i__1 = s_rsle(&io___39);
    if (i__1 != 0) {
	goto L140;
    }
    i__1 = do_lio(&c__3, &c__1, (char *)&mn_1.mplusn, (ftnlen)sizeof(integer))
	    ;
    if (i__1 != 0) {
	goto L140;
    }
    i__1 = e_rsle();
    if (i__1 != 0) {
	goto L140;
    }
    if (mn_1.mplusn == 0) {
	goto L140;
    }
    io___40.ciunit = *nin;
    i__1 = s_rsle(&io___40);
    if (i__1 != 0) {
	goto L140;
    }
    i__1 = do_lio(&c__3, &c__1, (char *)&mn_1.n, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L140;
    }
    i__1 = e_rsle();
    if (i__1 != 0) {
	goto L140;
    }
    i__1 = mn_1.mplusn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___41.ciunit = *nin;
	s_rsle(&io___41);
	i__2 = mn_1.mplusn;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__7, &c__1, (char *)&ai[i__ + j * ai_dim1], (ftnlen)
		    sizeof(doublecomplex));
	}
	e_rsle();
/* L90: */
    }
    i__1 = mn_1.mplusn;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___42.ciunit = *nin;
	s_rsle(&io___42);
	i__2 = mn_1.mplusn;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__7, &c__1, (char *)&bi[i__ + j * bi_dim1], (ftnlen)
		    sizeof(doublecomplex));
	}
	e_rsle();
/* L100: */
    }
    io___43.ciunit = *nin;
    s_rsle(&io___43);
    do_lio(&c__5, &c__1, (char *)&pltru, (ftnlen)sizeof(doublereal));
    do_lio(&c__5, &c__1, (char *)&diftru, (ftnlen)sizeof(doublereal));
    e_rsle();

    ++nptknt;
    mn_1.fs = TRUE_;
    mn_1.k = 0;
    mn_1.m = mn_1.mplusn - mn_1.n;

    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &ai[ai_offset], lda, &a[
	    a_offset], lda);
    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &bi[bi_offset], lda, &b[
	    b_offset], lda);

/*     Compute the Schur factorization while swaping the */
/*     m-by-m (1,1)-blocks with n-by-n (2,2)-blocks. */

    zggesx_("V", "V", "S", (L_fp)zlctsx_, "B", &mn_1.mplusn, &ai[ai_offset], 
	    lda, &bi[bi_offset], lda, &mm, &alpha[1], &beta[1], &q[q_offset], 
	    lda, &z__[z_offset], lda, pl, difest, &work[1], lwork, &rwork[1], 
	    &iwork[1], liwork, &bwork[1], &linfo);

    if (linfo != 0 && linfo != mn_1.mplusn + 2) {
	result[0] = ulpinv;
	io___45.ciunit = *nout;
	s_wsfe(&io___45);
	do_fio(&c__1, "ZGGESX", (ftnlen)6);
	do_fio(&c__1, (char *)&linfo, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nptknt, (ftnlen)sizeof(integer));
	e_wsfe();
	goto L130;
    }

/*     Compute the norm(A, B) */
/*        (should this be norm of (A,B) or (AI,BI)?) */

    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &ai[ai_offset], lda, &work[1], 
	     &mn_1.mplusn);
    zlacpy_("Full", &mn_1.mplusn, &mn_1.mplusn, &bi[bi_offset], lda, &work[
	    mn_1.mplusn * mn_1.mplusn + 1], &mn_1.mplusn);
    i__1 = mn_1.mplusn << 1;
    abnrm = zlange_("Fro", &mn_1.mplusn, &i__1, &work[1], &mn_1.mplusn, &
	    rwork[1]);

/*     Do tests (1) to (4) */

    zget51_(&c__1, &mn_1.mplusn, &a[a_offset], lda, &ai[ai_offset], lda, &q[
	    q_offset], lda, &z__[z_offset], lda, &work[1], &rwork[1], result);
    zget51_(&c__1, &mn_1.mplusn, &b[b_offset], lda, &bi[bi_offset], lda, &q[
	    q_offset], lda, &z__[z_offset], lda, &work[1], &rwork[1], &result[
	    1]);
    zget51_(&c__3, &mn_1.mplusn, &b[b_offset], lda, &bi[bi_offset], lda, &q[
	    q_offset], lda, &q[q_offset], lda, &work[1], &rwork[1], &result[2]
);
    zget51_(&c__3, &mn_1.mplusn, &b[b_offset], lda, &bi[bi_offset], lda, &z__[
	    z_offset], lda, &z__[z_offset], lda, &work[1], &rwork[1], &result[
	    3]);

/*     Do tests (5) and (6): check Schur form of A and compare */
/*     eigenvalues with diagonals. */

    ntest = 6;
    temp1 = 0.;
    result[4] = 0.;
    result[5] = 0.;

    i__1 = mn_1.mplusn;
    for (j = 1; j <= i__1; ++j) {
	ilabad = FALSE_;
	i__2 = j;
	i__3 = j + j * ai_dim1;
	z__2.r = alpha[i__2].r - ai[i__3].r, z__2.i = alpha[i__2].i - ai[i__3]
		.i;
	z__1.r = z__2.r, z__1.i = z__2.i;
	i__4 = j;
	i__5 = j + j * bi_dim1;
	z__4.r = beta[i__4].r - bi[i__5].r, z__4.i = beta[i__4].i - bi[i__5]
		.i;
	z__3.r = z__4.r, z__3.i = z__4.i;
/* Computing MAX */
	i__6 = j;
	i__7 = j + j * ai_dim1;
	d__13 = smlnum, d__14 = (d__1 = alpha[i__6].r, abs(d__1)) + (d__2 = 
		d_imag(&alpha[j]), abs(d__2)), d__13 = max(d__13,d__14), 
		d__14 = (d__3 = ai[i__7].r, abs(d__3)) + (d__4 = d_imag(&ai[j 
		+ j * ai_dim1]), abs(d__4));
/* Computing MAX */
	i__8 = j;
	i__9 = j + j * bi_dim1;
	d__15 = smlnum, d__16 = (d__5 = beta[i__8].r, abs(d__5)) + (d__6 = 
		d_imag(&beta[j]), abs(d__6)), d__15 = max(d__15,d__16), d__16 
		= (d__7 = bi[i__9].r, abs(d__7)) + (d__8 = d_imag(&bi[j + j * 
		bi_dim1]), abs(d__8));
	temp2 = (((d__9 = z__1.r, abs(d__9)) + (d__10 = d_imag(&z__1), abs(
		d__10))) / max(d__13,d__14) + ((d__11 = z__3.r, abs(d__11)) + 
		(d__12 = d_imag(&z__3), abs(d__12))) / max(d__15,d__16)) / 
		ulp;
	if (j < mn_1.mplusn) {
	    i__2 = j + 1 + j * ai_dim1;
	    if (ai[i__2].r != 0. || ai[i__2].i != 0.) {
		ilabad = TRUE_;
		result[4] = ulpinv;
	    }
	}
	if (j > 1) {
	    i__2 = j + (j - 1) * ai_dim1;
	    if (ai[i__2].r != 0. || ai[i__2].i != 0.) {
		ilabad = TRUE_;
		result[4] = ulpinv;
	    }
	}
	temp1 = max(temp1,temp2);
	if (ilabad) {
	    io___46.ciunit = *nout;
	    s_wsfe(&io___46);
	    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nptknt, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/* L110: */
    }
    result[5] = temp1;

/*     Test (7) (if sorting worked)  <--------- need to be checked. */

    ntest = 7;
    result[6] = 0.;
    if (linfo == mn_1.mplusn + 3) {
	result[6] = ulpinv;
    }

/*     Test (8): compare the estimated value of DIF and its true value. */

    ntest = 8;
    result[7] = 0.;
    if (difest[1] == 0.) {
	if (diftru > abnrm * ulp) {
	    result[7] = ulpinv;
	}
    } else if (diftru == 0.) {
	if (difest[1] > abnrm * ulp) {
	    result[7] = ulpinv;
	}
    } else if (diftru > thrsh2 * difest[1] || diftru * thrsh2 < difest[1]) {
/* Computing MAX */
	d__1 = diftru / difest[1], d__2 = difest[1] / diftru;
	result[7] = max(d__1,d__2);
    }

/*     Test (9) */

    ntest = 9;
    result[8] = 0.;
    if (linfo == mn_1.mplusn + 2) {
	if (diftru > abnrm * ulp) {
	    result[8] = ulpinv;
	}
	if (ifunc > 1 && difest[1] != 0.) {
	    result[8] = ulpinv;
	}
	if (ifunc == 1 && pl[0] != 0.) {
	    result[8] = ulpinv;
	}
    }

/*     Test (10): compare the estimated value of PL and it true value. */

    ntest = 10;
    result[9] = 0.;
    if (pl[0] == 0.) {
	if (pltru > abnrm * ulp) {
	    result[9] = ulpinv;
	}
    } else if (pltru == 0.) {
	if (pl[0] > abnrm * ulp) {
	    result[9] = ulpinv;
	}
    } else if (pltru > *thresh * pl[0] || pltru * *thresh < pl[0]) {
	result[9] = ulpinv;
    }

    ntestt += ntest;

/*     Print out tests which fail. */

    i__1 = ntest;
    for (j = 1; j <= i__1; ++j) {
	if (result[j - 1] >= *thresh) {

/*           If this is the first test to fail, */
/*           print a header to the data file. */

	    if (nerrs == 0) {
		io___47.ciunit = *nout;
		s_wsfe(&io___47);
		do_fio(&c__1, "CGX", (ftnlen)3);
		e_wsfe();

/*              Matrix types */

		io___48.ciunit = *nout;
		s_wsfe(&io___48);
		e_wsfe();

/*              Tests performed */

		io___49.ciunit = *nout;
		s_wsfe(&io___49);
		do_fio(&c__1, "unitary", (ftnlen)7);
		do_fio(&c__1, "'", (ftnlen)1);
		do_fio(&c__1, "transpose", (ftnlen)9);
		for (i__ = 1; i__ <= 4; ++i__) {
		    do_fio(&c__1, "'", (ftnlen)1);
		}
		e_wsfe();

	    }
	    ++nerrs;
	    if (result[j - 1] < 1e4) {
		io___50.ciunit = *nout;
		s_wsfe(&io___50);
		do_fio(&c__1, (char *)&nptknt, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&result[j - 1], (ftnlen)sizeof(
			doublereal));
		e_wsfe();
	    } else {
		io___51.ciunit = *nout;
		s_wsfe(&io___51);
		do_fio(&c__1, (char *)&nptknt, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&mn_1.mplusn, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&result[j - 1], (ftnlen)sizeof(
			doublereal));
		e_wsfe();
	    }
	}

/* L120: */
    }

L130:
    goto L80;
L140:

L150:

/*     Summary */

    alasvm_("CGX", nout, &nerrs, &ntestt, &c__0);

    work[1].r = (doublereal) maxwrk, work[1].i = 0.;

    return 0;








/*     End of ZDRGSX */

} /* zdrgsx_ */
