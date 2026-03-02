#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b17 = 0.;
static integer c__0 = 0;
static doublereal c_b31 = 1.;
static integer c__4 = 4;
static integer c__6 = 6;
static integer c__1 = 1;
static integer c__2 = 2;

/* Subroutine */ int ddrvev_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, doublereal *thresh, integer *nounit, 
	doublereal *a, integer *lda, doublereal *h__, doublereal *wr, 
	doublereal *wi, doublereal *wr1, doublereal *wi1, doublereal *vl, 
	integer *ldvl, doublereal *vr, integer *ldvr, doublereal *lre, 
	integer *ldlre, doublereal *result, doublereal *work, integer *nwork, 
	integer *iwork, integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,3,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,9,9,9 };
    static integer kmagn[21] = { 1,1,1,1,1,1,2,3,1,1,1,1,1,1,1,1,2,3,1,2,3 };
    static integer kmode[21] = { 0,0,0,4,3,1,4,4,4,3,1,5,4,3,1,5,5,5,4,3,1 };
    static integer kconds[21] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,0,0,0 };

    /* Format strings */
    static char fmt_9993[] = "(\002 DDRVEV: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(/1x,a3,\002 -- Real Eigenvalue-Eigenvector De"
	    "composition\002,\002 Driver\002,/\002 Matrix types (see DDRVEV f"
	    "or details): \002)";
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
	    " 12=Well-cond., random complex \002,6x,\002   \002,\002 17=Ill-c"
	    "ond., large rand. complx \002,/\002 13=Ill-condi\002,\002tioned,"
	    " evenly spaced.     \002,\002 18=Ill-cond., small rand.\002,\002"
	    " complx \002)";
    static char fmt_9996[] = "(\002 19=Matrix with random O(1) entries.   "
	    " \002,\002 21=Matrix \002,\002with small random entries.\002,"
	    "/\002 20=Matrix with large ran\002,\002dom entries.   \002,/)";
    static char fmt_9995[] = "(\002 Tests performed with test threshold ="
	    "\002,f8.2,//\002 1 = | A VR - VR W | / ( n |A| ulp ) \002,/\002 "
	    "2 = | transpose(A) VL - VL W | / ( n |A| ulp ) \002,/\002 3 = | "
	    "|VR(i)| - 1 | / ulp \002,/\002 4 = | |VL(i)| - 1 | / ulp \002,"
	    "/\002 5 = 0 if W same no matter if VR or VL computed,\002,\002 1"
	    "/ulp otherwise\002,/\002 6 = 0 if VR same no matter if VL comput"
	    "ed,\002,\002  1/ulp otherwise\002,/\002 7 = 0 if VL same no matt"
	    "er if VR computed,\002,\002  1/ulp otherwise\002,/)";
    static char fmt_9994[] = "(\002 N=\002,i5,\002, IWK=\002,i2,\002, seed"
	    "=\002,4(i4,\002,\002),\002 type \002,i2,\002, test(\002,i2,\002)="
	    "\002,g10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, lre_dim1, lre_offset, vl_dim1,
	     vl_offset, vr_dim1, vr_offset, i__1, i__2, i__3, i__4;
    doublereal d__1, d__2, d__3, d__4, d__5;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer j, n, jj;
    doublereal dum[1], res[2];
    integer iwk;
    doublereal ulp, vmx, cond;
    integer jcol;
    char path[3];
    integer nmax;
    doublereal unfl, ovfl, tnrm, vrmx, vtst;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    logical badnn;
    extern /* Subroutine */ int dget22_(char *, char *, char *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    integer nfail;
    extern /* Subroutine */ int dgeev_(char *, char *, integer *, doublereal *
, integer *, doublereal *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *);
    integer imode, iinfo;
    doublereal conds, anorm;
    integer jsize, nerrs, itype, jtype, ntest;
    doublereal rtulp;
    extern doublereal dlapy2_(doublereal *, doublereal *);
    extern /* Subroutine */ int dlabad_(doublereal *, doublereal *);
    extern doublereal dlamch_(char *);
    char adumma[1*1];
    extern /* Subroutine */ int dlatme_(integer *, char *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, char *, char 
	    *, char *, char *, doublereal *, integer *, doublereal *, integer 
	    *, integer *, doublereal *, doublereal *, integer *, doublereal *, 
	     integer *);
    integer idumma[1];
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ioldsd[4];
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
	    *, doublereal *, integer *), dlasum_(char 
	    *, integer *, integer *, integer *);
    integer ntestf;
    doublereal ulpinv;
    integer nnwork;
    doublereal rtulpi;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___32 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9994, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     DDRVEV  checks the nonsymmetric eigenvalue problem driver DGEEV. */

/*     When DDRVEV is called, a number of matrix "sizes" ("n's") and a */
/*     number of matrix "types" are specified.  For each size ("n") */
/*     and each type of matrix, one matrix will be generated and used */
/*     to test the nonsymmetric eigenroutines.  For each matrix, 7 */
/*     tests will be performed: */

/*     (1)     | A * VR - VR * W | / ( n |A| ulp ) */

/*       Here VR is the matrix of unit right eigenvectors. */
/*       W is a block diagonal matrix, with a 1x1 block for each */
/*       real eigenvalue and a 2x2 block for each complex conjugate */
/*       pair.  If eigenvalues j and j+1 are a complex conjugate pair, */
/*       so WR(j) = WR(j+1) = wr and WI(j) = - WI(j+1) = wi, then the */
/*       2 x 2 block corresponding to the pair will be: */

/*               (  wr  wi  ) */
/*               ( -wi  wr  ) */

/*       Such a block multiplying an n x 2 matrix  ( ur ui ) on the */
/*       right will be the same as multiplying  ur + i*ui  by  wr + i*wi. */

/*     (2)     | A**H * VL - VL * W**H | / ( n |A| ulp ) */

/*       Here VL is the matrix of unit left eigenvectors, A**H is the */
/*       conjugate transpose of A, and W is as above. */

/*     (3)     | |VR(i)| - 1 | / ulp and whether largest component real */

/*       VR(i) denotes the i-th column of VR. */

/*     (4)     | |VL(i)| - 1 | / ulp and whether largest component real */

/*       VL(i) denotes the i-th column of VL. */

/*     (5)     W(full) = W(partial) */

/*       W(full) denotes the eigenvalues computed when both VR and VL */
/*       are also computed, and W(partial) denotes the eigenvalues */
/*       computed when only W, only W and VR, or only W and VL are */
/*       computed. */

/*     (6)     VR(full) = VR(partial) */

/*       VR(full) denotes the right eigenvectors computed when both VR */
/*       and VL are computed, and VR(partial) denotes the result */
/*       when only VR is computed. */

/*      (7)     VL(full) = VL(partial) */

/*       VL(full) denotes the left eigenvectors computed when both VR */
/*       and VL are also computed, and VL(partial) denotes the result */
/*       when only VL is computed. */

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

/*  Arguments */
/*  ========== */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          DDRVEV does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, DDRVEV */
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
/*          next call to DDRVEV to continue the same random number */
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

/*  A       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least max(NN). */

/*  H       (workspace) DOUBLE PRECISION array, dimension (LDA, max(NN)) */
/*          Another copy of the test matrix A, modified by DGEEV. */

/*  WR      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  WI      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          The real and imaginary parts of the eigenvalues of A. */
/*          On exit, WR + WI*i are the eigenvalues of the matrix in A. */

/*  WR1     (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*  WI1     (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but those computed when DGEEV only computes a partial */
/*          eigendecomposition, i.e. not the eigenvalues and left */
/*          and right eigenvectors. */

/*  VL      (workspace) DOUBLE PRECISION array, dimension (LDVL, max(NN)) */
/*          VL holds the computed left eigenvectors. */

/*  LDVL    (input) INTEGER */
/*          Leading dimension of VL. Must be at least max(1,max(NN)). */

/*  VR      (workspace) DOUBLE PRECISION array, dimension (LDVR, max(NN)) */
/*          VR holds the computed right eigenvectors. */

/*  LDVR    (input) INTEGER */
/*          Leading dimension of VR. Must be at least max(1,max(NN)). */

/*  LRE     (workspace) DOUBLE PRECISION array, dimension (LDLRE,max(NN)) */
/*          LRE holds the computed right or left eigenvectors. */

/*  LDLRE   (input) INTEGER */
/*          Leading dimension of LRE. Must be at least max(1,max(NN)). */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (7) */
/*          The values computed by the seven tests described above. */
/*          The values are currently limited to 1/ulp, to avoid overflow. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (NWORK) */

/*  NWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          5*NN(j)+2*NN(j)**2 for all j. */

/*  IWORK   (workspace) INTEGER array, dimension (max(NN)) */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some NN(j) < 0 */
/*           -3: NTYPES < 0 */
/*           -6: THRESH < 0 */
/*           -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*          -16: LDVL < 1 or LDVL < NMAX, where NMAX is max( NN(j) ). */
/*          -18: LDVR < 1 or LDVR < NMAX, where NMAX is max( NN(j) ). */
/*          -20: LDLRE < 1 or LDLRE < NMAX, where NMAX is max( NN(j) ). */
/*          -23: NWORK too small. */
/*          If  DLATMR, SLATMS, SLATME or DGEEV returns an error code, */
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
/*     KCONDS(j)       Selectw whether CONDS is to be 1 or */
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
    h_dim1 = *lda;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --wr;
    --wi;
    --wr1;
    --wi1;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    lre_dim1 = *ldlre;
    lre_offset = 1 + lre_dim1;
    lre -= lre_offset;
    --result;
    --work;
    --iwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Double precision", (ftnlen)1, (ftnlen)16);
    s_copy(path + 1, "EV", (ftnlen)2, (ftnlen)2);

/*     Check for errors */

    ntestt = 0;
    ntestf = 0;
    *info = 0;

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
    } else if (*ldvl < 1 || *ldvl < nmax) {
	*info = -16;
    } else if (*ldvr < 1 || *ldvr < nmax) {
	*info = -18;
    } else if (*ldlre < 1 || *ldlre < nmax) {
	*info = -20;
    } else /* if(complicated condition) */ {
/* Computing 2nd power */
	i__1 = nmax;
	if (nmax * 5 + (i__1 * i__1 << 1) > *nwork) {
	    *info = -23;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DDRVEV", &i__1);
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
		goto L260;
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

	    dlaset_("Full", lda, &n, &c_b17, &c_b17, &a[a_offset], lda);
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
		dlatme_(&n, "S", &iseed[1], &work[1], &imode, &cond, &c_b31, 
			adumma, "T", "T", "T", &work[n + 1], &c__4, &conds, &
			n, &n, &anorm, &a[a_offset], lda, &work[(n << 1) + 1], 
			 &iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &c__0, &
			c__0, &c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &n, &n, &
			c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              General, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "N", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &n, &n, &
			c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);
		if (n >= 4) {
		    dlaset_("Full", &c__2, &n, &c_b17, &c_b17, &a[a_offset], 
			    lda);
		    i__3 = n - 3;
		    dlaset_("Full", &i__3, &c__1, &c_b17, &c_b17, &a[a_dim1 + 
			    3], lda);
		    i__3 = n - 3;
		    dlaset_("Full", &i__3, &c__2, &c_b17, &c_b17, &a[(n - 1) *
			     a_dim1 + 3], lda);
		    dlaset_("Full", &c__1, &n, &c_b17, &c_b17, &a[n + a_dim1], 
			     lda);
		}

	    } else if (itype == 10) {

/*              Triangular, random eigenvalues */

		dlatmr_(&n, &n, "S", &iseed[1], "N", &work[1], &c__6, &c_b31, 
			&c_b31, "T", "N", &work[n + 1], &c__1, &c_b31, &work[(
			n << 1) + 1], &c__1, &c_b31, "N", idumma, &n, &c__0, &
			c_b17, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
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
		    nnwork = n << 2;
		} else {
/* Computing 2nd power */
		    i__3 = n;
		    nnwork = n * 5 + (i__3 * i__3 << 1);
		}
		nnwork = max(nnwork,1);

/*              Initialize RESULT */

		for (j = 1; j <= 7; ++j) {
		    result[j] = -1.;
/* L100: */
		}

/*              Compute eigenvalues and eigenvectors, and test them */

		dlacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		dgeev_("V", "V", &n, &h__[h_offset], lda, &wr[1], &wi[1], &vl[
			vl_offset], ldvl, &vr[vr_offset], ldvr, &work[1], &
			nnwork, &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___35.ciunit = *nounit;
		    s_wsfe(&io___35);
		    do_fio(&c__1, "DGEEV1", (ftnlen)6);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    goto L220;
		}

/*              Do Test (1) */

		dget22_("N", "N", "N", &n, &a[a_offset], lda, &vr[vr_offset], 
			ldvr, &wr[1], &wi[1], &work[1], res);
		result[1] = res[0];

/*              Do Test (2) */

		dget22_("T", "N", "T", &n, &a[a_offset], lda, &vl[vl_offset], 
			ldvl, &wr[1], &wi[1], &work[1], res);
		result[2] = res[0];

/*              Do Test (3) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    tnrm = 1.;
		    if (wi[j] == 0.) {
			tnrm = dnrm2_(&n, &vr[j * vr_dim1 + 1], &c__1);
		    } else if (wi[j] > 0.) {
			d__1 = dnrm2_(&n, &vr[j * vr_dim1 + 1], &c__1);
			d__2 = dnrm2_(&n, &vr[(j + 1) * vr_dim1 + 1], &c__1);
			tnrm = dlapy2_(&d__1, &d__2);
		    }
/* Computing MAX */
/* Computing MIN */
		    d__4 = ulpinv, d__5 = (d__1 = tnrm - 1., abs(d__1)) / ulp;
		    d__2 = result[3], d__3 = min(d__4,d__5);
		    result[3] = max(d__2,d__3);
		    if (wi[j] > 0.) {
			vmx = 0.;
			vrmx = 0.;
			i__4 = n;
			for (jj = 1; jj <= i__4; ++jj) {
			    vtst = dlapy2_(&vr[jj + j * vr_dim1], &vr[jj + (j 
				    + 1) * vr_dim1]);
			    if (vtst > vmx) {
				vmx = vtst;
			    }
			    if (vr[jj + (j + 1) * vr_dim1] == 0. && (d__1 = 
				    vr[jj + j * vr_dim1], abs(d__1)) > vrmx) {
				vrmx = (d__2 = vr[jj + j * vr_dim1], abs(d__2)
					);
			    }
/* L110: */
			}
			if (vrmx / vmx < 1. - ulp * 2.) {
			    result[3] = ulpinv;
			}
		    }
/* L120: */
		}

/*              Do Test (4) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    tnrm = 1.;
		    if (wi[j] == 0.) {
			tnrm = dnrm2_(&n, &vl[j * vl_dim1 + 1], &c__1);
		    } else if (wi[j] > 0.) {
			d__1 = dnrm2_(&n, &vl[j * vl_dim1 + 1], &c__1);
			d__2 = dnrm2_(&n, &vl[(j + 1) * vl_dim1 + 1], &c__1);
			tnrm = dlapy2_(&d__1, &d__2);
		    }
/* Computing MAX */
/* Computing MIN */
		    d__4 = ulpinv, d__5 = (d__1 = tnrm - 1., abs(d__1)) / ulp;
		    d__2 = result[4], d__3 = min(d__4,d__5);
		    result[4] = max(d__2,d__3);
		    if (wi[j] > 0.) {
			vmx = 0.;
			vrmx = 0.;
			i__4 = n;
			for (jj = 1; jj <= i__4; ++jj) {
			    vtst = dlapy2_(&vl[jj + j * vl_dim1], &vl[jj + (j 
				    + 1) * vl_dim1]);
			    if (vtst > vmx) {
				vmx = vtst;
			    }
			    if (vl[jj + (j + 1) * vl_dim1] == 0. && (d__1 = 
				    vl[jj + j * vl_dim1], abs(d__1)) > vrmx) {
				vrmx = (d__2 = vl[jj + j * vl_dim1], abs(d__2)
					);
			    }
/* L130: */
			}
			if (vrmx / vmx < 1. - ulp * 2.) {
			    result[4] = ulpinv;
			}
		    }
/* L140: */
		}

/*              Compute eigenvalues only, and test them */

		dlacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		dgeev_("N", "N", &n, &h__[h_offset], lda, &wr1[1], &wi1[1], 
			dum, &c__1, dum, &c__1, &work[1], &nnwork, &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___43.ciunit = *nounit;
		    s_wsfe(&io___43);
		    do_fio(&c__1, "DGEEV2", (ftnlen)6);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    goto L220;
		}

/*              Do Test (5) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    if (wr[j] != wr1[j] || wi[j] != wi1[j]) {
			result[5] = ulpinv;
		    }
/* L150: */
		}

/*              Compute eigenvalues and right eigenvectors, and test them */

		dlacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		dgeev_("N", "V", &n, &h__[h_offset], lda, &wr1[1], &wi1[1], 
			dum, &c__1, &lre[lre_offset], ldlre, &work[1], &
			nnwork, &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___44.ciunit = *nounit;
		    s_wsfe(&io___44);
		    do_fio(&c__1, "DGEEV3", (ftnlen)6);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    goto L220;
		}

/*              Do Test (5) again */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    if (wr[j] != wr1[j] || wi[j] != wi1[j]) {
			result[5] = ulpinv;
		    }
/* L160: */
		}

/*              Do Test (6) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			if (vr[j + jj * vr_dim1] != lre[j + jj * lre_dim1]) {
			    result[6] = ulpinv;
			}
/* L170: */
		    }
/* L180: */
		}

/*              Compute eigenvalues and left eigenvectors, and test them */

		dlacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		dgeev_("V", "N", &n, &h__[h_offset], lda, &wr1[1], &wi1[1], &
			lre[lre_offset], ldlre, dum, &c__1, &work[1], &nnwork, 
			 &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___45.ciunit = *nounit;
		    s_wsfe(&io___45);
		    do_fio(&c__1, "DGEEV4", (ftnlen)6);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    goto L220;
		}

/*              Do Test (5) again */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    if (wr[j] != wr1[j] || wi[j] != wi1[j]) {
			result[5] = ulpinv;
		    }
/* L190: */
		}

/*              Do Test (7) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			if (vl[j + jj * vl_dim1] != lre[j + jj * lre_dim1]) {
			    result[7] = ulpinv;
			}
/* L200: */
		    }
/* L210: */
		}

/*              End of Loop -- Check for RESULT(j) > THRESH */

L220:

		ntest = 0;
		nfail = 0;
		for (j = 1; j <= 7; ++j) {
		    if (result[j] >= 0.) {
			++ntest;
		    }
		    if (result[j] >= *thresh) {
			++nfail;
		    }
/* L230: */
		}

		if (nfail > 0) {
		    ++ntestf;
		}
		if (ntestf == 1) {
		    io___48.ciunit = *nounit;
		    s_wsfe(&io___48);
		    do_fio(&c__1, path, (ftnlen)3);
		    e_wsfe();
		    io___49.ciunit = *nounit;
		    s_wsfe(&io___49);
		    e_wsfe();
		    io___50.ciunit = *nounit;
		    s_wsfe(&io___50);
		    e_wsfe();
		    io___51.ciunit = *nounit;
		    s_wsfe(&io___51);
		    e_wsfe();
		    io___52.ciunit = *nounit;
		    s_wsfe(&io___52);
		    do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(
			    doublereal));
		    e_wsfe();
		    ntestf = 2;
		}

		for (j = 1; j <= 7; ++j) {
		    if (result[j] >= *thresh) {
			io___53.ciunit = *nounit;
			s_wsfe(&io___53);
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
/* L240: */
		}

		nerrs += nfail;
		ntestt += ntest;

/* L250: */
	    }
L260:
	    ;
	}
/* L270: */
    }

/*     Summary */

    dlasum_(path, nounit, &nerrs, &ntestt);



    return 0;

/*     End of DDRVEV */

} /* ddrvev_ */
