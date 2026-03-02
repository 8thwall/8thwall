#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__0 = 0;
static integer c__4 = 4;
static integer c__6 = 6;
static real c_b38 = 1.f;
static integer c__1 = 1;
static real c_b48 = 0.f;
static integer c__2 = 2;

/* Subroutine */ int cdrvev_(integer *nsizes, integer *nn, integer *ntypes, 
	logical *dotype, integer *iseed, real *thresh, integer *nounit, 
	complex *a, integer *lda, complex *h__, complex *w, complex *w1, 
	complex *vl, integer *ldvl, complex *vr, integer *ldvr, complex *lre, 
	integer *ldlre, real *result, complex *work, integer *nwork, real *
	rwork, integer *iwork, integer *info)
{
    /* Initialized data */

    static integer ktype[21] = { 1,2,3,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,9,9,9 };
    static integer kmagn[21] = { 1,1,1,1,1,1,2,3,1,1,1,1,1,1,1,1,2,3,1,2,3 };
    static integer kmode[21] = { 0,0,0,4,3,1,4,4,4,3,1,5,4,3,1,5,5,5,4,3,1 };
    static integer kconds[21] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,0,0,0 };

    /* Format strings */
    static char fmt_9993[] = "(\002 CDRVEV: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(/1x,a3,\002 -- Complex Eigenvalue-Eigenvect"
	    "or \002,\002Decomposition Driver\002,/\002 Matrix types (see CDR"
	    "VEV for details): \002)";
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
	    "vals. \002,\002 16=Ill-cond., random comp\002,\002lex \002,a6,"
	    "/\002 12=Well-cond., random complex \002,a6,\002   \002,\002 17="
	    "Ill-cond., large rand. complx \002,a4,/\002 13=Ill-condi\002,"
	    "\002tioned, evenly spaced.     \002,\002 18=Ill-cond., small ran"
	    "d.\002,\002 complx \002,a4)";
    static char fmt_9996[] = "(\002 19=Matrix with random O(1) entries.   "
	    " \002,\002 21=Matrix \002,\002with small random entries.\002,"
	    "/\002 20=Matrix with large ran\002,\002dom entries.   \002,/)";
    static char fmt_9995[] = "(\002 Tests performed with test threshold ="
	    "\002,f8.2,//\002 1 = | A VR - VR W | / ( n |A| ulp ) \002,/\002 "
	    "2 = | conj-trans(A) VL - VL conj-trans(W) | /\002,\002 ( n |A| u"
	    "lp ) \002,/\002 3 = | |VR(i)| - 1 | / ulp \002,/\002 4 = | |VL(i"
	    ")| - 1 | / ulp \002,/\002 5 = 0 if W same no matter if VR or VL "
	    "computed,\002,\002 1/ulp otherwise\002,/\002 6 = 0 if VR same no"
	    " matter if VL computed,\002,\002  1/ulp otherwise\002,/\002 7 = "
	    "0 if VL same no matter if VR computed,\002,\002  1/ulp otherwis"
	    "e\002,/)";
    static char fmt_9994[] = "(\002 N=\002,i5,\002, IWK=\002,i2,\002, seed"
	    "=\002,4(i4,\002,\002),\002 type \002,i2,\002, test(\002,i2,\002)="
	    "\002,g10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, lre_dim1, lre_offset, vl_dim1,
	     vl_offset, vr_dim1, vr_offset, i__1, i__2, i__3, i__4, i__5, 
	    i__6;
    real r__1, r__2, r__3, r__4, r__5;
    complex q__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double c_abs(complex *), r_imag(complex *);

    /* Local variables */
    integer j, n, jj;
    complex dum[1];
    real res[2];
    integer iwk;
    real ulp, vmx, cond;
    integer jcol;
    char path[3];
    integer nmax;
    real unfl, ovfl, tnrm, vrmx, vtst;
    logical badnn;
    extern /* Subroutine */ int cget22_(char *, char *, char *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, complex *, 
	    real *, real *);
    integer nfail;
    extern /* Subroutine */ int cgeev_(char *, char *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, integer *);
    integer imode, iinfo;
    real conds, anorm;
    integer jsize, nerrs, itype, jtype, ntest;
    real rtulp;
    extern doublereal scnrm2_(integer *, complex *, integer *);
    extern /* Subroutine */ int slabad_(real *, real *), clatme_(integer *, 
	    char *, integer *, complex *, integer *, real *, complex *, char *
, char *, char *, char *, real *, integer *, real *, integer *, 
	    integer *, real *, complex *, integer *, complex *, integer *);
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *);
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
	    complex *, integer *, complex *, integer *);
    integer ntestf;
    extern /* Subroutine */ int slasum_(char *, integer *, integer *, integer 
	    *);
    real ulpinv;
    integer nnwork;
    real rtulpi;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___31 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9994, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     CDRVEV  checks the nonsymmetric eigenvalue problem driver CGEEV. */

/*     When CDRVEV is called, a number of matrix "sizes" ("n's") and a */
/*     number of matrix "types" are specified.  For each size ("n") */
/*     and each type of matrix, one matrix will be generated and used */
/*     to test the nonsymmetric eigenroutines.  For each matrix, 7 */
/*     tests will be performed: */

/*     (1)     | A * VR - VR * W | / ( n |A| ulp ) */

/*       Here VR is the matrix of unit right eigenvectors. */
/*       W is a diagonal matrix with diagonal entries W(j). */

/*     (2)     | A**H * VL - VL * W**H | / ( n |A| ulp ) */

/*       Here VL is the matrix of unit left eigenvectors, A**H is the */
/*       conjugate-transpose of A, and W is as above. */

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
/*          1, ..., ULP  and random complex angles. */
/*          (ULP = (first number larger than 1) - 1 ) */
/*     (5)  A diagonal matrix with geometrically spaced entries */
/*          1, ..., ULP  and random complex angles. */
/*     (6)  A diagonal matrix with "clustered" entries 1, ULP, ..., ULP */
/*          and random complex angles. */

/*     (7)  Same as (4), but multiplied by a constant near */
/*          the overflow threshold */
/*     (8)  Same as (4), but multiplied by a constant near */
/*          the underflow threshold */

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
/*          from ULP < |z| < 1 and random O(1) entries in the upper */
/*          triangle. */

/*     (17) Same as (16), but multiplied by a constant */
/*          near the overflow threshold */
/*     (18) Same as (16), but multiplied by a constant */
/*          near the underflow threshold */

/*     (19) Nonsymmetric matrix with random entries chosen from |z| < 1 */
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
/*          CDRVEV does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, CDRVEV */
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
/*          next call to CDRVEV to continue the same random number */
/*          sequence. */

/*  THRESH  (input) REAL */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error */
/*          is scaled to be O(1), so THRESH should be a reasonably */
/*          small multiple of 1, e.g., 10 or 100.  In particular, */
/*          it should not depend on the precision (single vs. double) */
/*          or the size of the matrix.  It must be at least zero. */

/*  NOUNIT  (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns INFO not equal to 0.) */

/*  A       (workspace) COMPLEX array, dimension (LDA, max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed.  On exit, A contains the last matrix actually used. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least max(NN). */

/*  H       (workspace) COMPLEX array, dimension (LDA, max(NN)) */
/*          Another copy of the test matrix A, modified by CGEEV. */

/*  W       (workspace) COMPLEX array, dimension (max(NN)) */
/*          The eigenvalues of A. On exit, W are the eigenvalues of */
/*          the matrix in A. */

/*  W1      (workspace) COMPLEX array, dimension (max(NN)) */
/*          Like W, this array contains the eigenvalues of A, */
/*          but those computed when CGEEV only computes a partial */
/*          eigendecomposition, i.e. not the eigenvalues and left */
/*          and right eigenvectors. */

/*  VL      (workspace) COMPLEX array, dimension (LDVL, max(NN)) */
/*          VL holds the computed left eigenvectors. */

/*  LDVL    (input) INTEGER */
/*          Leading dimension of VL. Must be at least max(1,max(NN)). */

/*  VR      (workspace) COMPLEX array, dimension (LDVR, max(NN)) */
/*          VR holds the computed right eigenvectors. */

/*  LDVR    (input) INTEGER */
/*          Leading dimension of VR. Must be at least max(1,max(NN)). */

/*  LRE     (workspace) COMPLEX array, dimension (LDLRE, max(NN)) */
/*          LRE holds the computed right or left eigenvectors. */

/*  LDLRE   (input) INTEGER */
/*          Leading dimension of LRE. Must be at least max(1,max(NN)). */

/*  RESULT  (output) REAL array, dimension (7) */
/*          The values computed by the seven tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  WORK    (workspace) COMPLEX array, dimension (NWORK) */

/*  NWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          5*NN(j)+2*NN(j)**2 for all j. */

/*  RWORK   (workspace) REAL array, dimension (2*max(NN)) */

/*  IWORK   (workspace) INTEGER array, dimension (max(NN)) */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */
/*           -1: NSIZES < 0 */
/*           -2: Some NN(j) < 0 */
/*           -3: NTYPES < 0 */
/*           -6: THRESH < 0 */
/*           -9: LDA < 1 or LDA < NMAX, where NMAX is max( NN(j) ). */
/*          -14: LDVL < 1 or LDVL < NMAX, where NMAX is max( NN(j) ). */
/*          -16: LDVR < 1 or LDVR < NMAX, where NMAX is max( NN(j) ). */
/*          -18: LDLRE < 1 or LDLRE < NMAX, where NMAX is max( NN(j) ). */
/*          -21: NWORK too small. */
/*          If  CLATMR, CLATMS, CLATME or CGEEV returns an error code, */
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
    --w;
    --w1;
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
    --rwork;
    --iwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    s_copy(path, "Complex precision", (ftnlen)1, (ftnlen)17);
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
    } else if (*thresh < 0.f) {
	*info = -6;
    } else if (*nounit <= 0) {
	*info = -7;
    } else if (*lda < 1 || *lda < nmax) {
	*info = -9;
    } else if (*ldvl < 1 || *ldvl < nmax) {
	*info = -14;
    } else if (*ldvr < 1 || *ldvr < nmax) {
	*info = -16;
    } else if (*ldlre < 1 || *ldlre < nmax) {
	*info = -28;
    } else /* if(complicated condition) */ {
/* Computing 2nd power */
	i__1 = nmax;
	if (nmax * 5 + (i__1 * i__1 << 1) > *nwork) {
	    *info = -21;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CDRVEV", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

    if (*nsizes == 0 || *ntypes == 0) {
	return 0;
    }

/*     More Important constants */

    unfl = slamch_("Safe minimum");
    ovfl = 1.f / unfl;
    slabad_(&unfl, &ovfl);
    ulp = slamch_("Precision");
    ulpinv = 1.f / ulp;
    rtulp = sqrt(ulp);
    rtulpi = 1.f / rtulp;

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
	    anorm = 1.f;
	    goto L60;

L40:
	    anorm = ovfl * ulp;
	    goto L60;

L50:
	    anorm = unfl * ulpinv;
	    goto L60;

L60:

	    claset_("Full", lda, &n, &c_b1, &c_b1, &a[a_offset], lda);
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
		    i__4 = jcol + jcol * a_dim1;
		    q__1.r = anorm, q__1.i = 0.f;
		    a[i__4].r = q__1.r, a[i__4].i = q__1.i;
/* L70: */
		}

	    } else if (itype == 3) {

/*              Jordan Block */

		i__3 = n;
		for (jcol = 1; jcol <= i__3; ++jcol) {
		    i__4 = jcol + jcol * a_dim1;
		    q__1.r = anorm, q__1.i = 0.f;
		    a[i__4].r = q__1.r, a[i__4].i = q__1.i;
		    if (jcol > 1) {
			i__4 = jcol + (jcol - 1) * a_dim1;
			a[i__4].r = 1.f, a[i__4].i = 0.f;
		    }
/* L80: */
		}

	    } else if (itype == 4) {

/*              Diagonal Matrix, [Eigen]values Specified */

		clatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &c__0, &c__0, "N", &a[a_offset], lda, &work[
			n + 1], &iinfo);

	    } else if (itype == 5) {

/*              Hermitian, eigenvalues specified */

		clatms_(&n, &n, "S", &iseed[1], "H", &rwork[1], &imode, &cond, 
			 &anorm, &n, &n, "N", &a[a_offset], lda, &work[n + 1], 
			 &iinfo);

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
			&anorm, &a[a_offset], lda, &work[(n << 1) + 1], &
			iinfo);

	    } else if (itype == 7) {

/*              Diagonal, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &c__0, &
			c__0, &c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[
			1], &iinfo);

	    } else if (itype == 8) {

/*              Symmetric, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "H", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &n, &n, &
			c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else if (itype == 9) {

/*              General, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &n, &n, &
			c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);
		if (n >= 4) {
		    claset_("Full", &c__2, &n, &c_b1, &c_b1, &a[a_offset], 
			    lda);
		    i__3 = n - 3;
		    claset_("Full", &i__3, &c__1, &c_b1, &c_b1, &a[a_dim1 + 3]
, lda);
		    i__3 = n - 3;
		    claset_("Full", &i__3, &c__2, &c_b1, &c_b1, &a[(n - 1) * 
			    a_dim1 + 3], lda);
		    claset_("Full", &c__1, &n, &c_b1, &c_b1, &a[n + a_dim1], 
			    lda);
		}

	    } else if (itype == 10) {

/*              Triangular, random eigenvalues */

		clatmr_(&n, &n, "D", &iseed[1], "N", &work[1], &c__6, &c_b38, 
			&c_b2, "T", "N", &work[n + 1], &c__1, &c_b38, &work[(
			n << 1) + 1], &c__1, &c_b38, "N", idumma, &n, &c__0, &
			c_b48, &anorm, "NO", &a[a_offset], lda, &iwork[1], &
			iinfo);

	    } else {

		iinfo = 1;
	    }

	    if (iinfo != 0) {
		io___31.ciunit = *nounit;
		s_wsfe(&io___31);
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
		    nnwork = n << 1;
		} else {
/* Computing 2nd power */
		    i__3 = n;
		    nnwork = n * 5 + (i__3 * i__3 << 1);
		}
		nnwork = max(nnwork,1);

/*              Initialize RESULT */

		for (j = 1; j <= 7; ++j) {
		    result[j] = -1.f;
/* L100: */
		}

/*              Compute eigenvalues and eigenvectors, and test them */

		clacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		cgeev_("V", "V", &n, &h__[h_offset], lda, &w[1], &vl[
			vl_offset], ldvl, &vr[vr_offset], ldvr, &work[1], &
			nnwork, &rwork[1], &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___34.ciunit = *nounit;
		    s_wsfe(&io___34);
		    do_fio(&c__1, "CGEEV1", (ftnlen)6);
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

		cget22_("N", "N", "N", &n, &a[a_offset], lda, &vr[vr_offset], 
			ldvr, &w[1], &work[1], &rwork[1], res);
		result[1] = res[0];

/*              Do Test (2) */

		cget22_("C", "N", "C", &n, &a[a_offset], lda, &vl[vl_offset], 
			ldvl, &w[1], &work[1], &rwork[1], res);
		result[2] = res[0];

/*              Do Test (3) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    tnrm = scnrm2_(&n, &vr[j * vr_dim1 + 1], &c__1);
/* Computing MAX */
/* Computing MIN */
		    r__4 = ulpinv, r__5 = (r__1 = tnrm - 1.f, dabs(r__1)) / 
			    ulp;
		    r__2 = result[3], r__3 = dmin(r__4,r__5);
		    result[3] = dmax(r__2,r__3);
		    vmx = 0.f;
		    vrmx = 0.f;
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			vtst = c_abs(&vr[jj + j * vr_dim1]);
			if (vtst > vmx) {
			    vmx = vtst;
			}
			i__5 = jj + j * vr_dim1;
			if (r_imag(&vr[jj + j * vr_dim1]) == 0.f && (r__1 = 
				vr[i__5].r, dabs(r__1)) > vrmx) {
			    i__6 = jj + j * vr_dim1;
			    vrmx = (r__2 = vr[i__6].r, dabs(r__2));
			}
/* L110: */
		    }
		    if (vrmx / vmx < 1.f - ulp * 2.f) {
			result[3] = ulpinv;
		    }
/* L120: */
		}

/*              Do Test (4) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    tnrm = scnrm2_(&n, &vl[j * vl_dim1 + 1], &c__1);
/* Computing MAX */
/* Computing MIN */
		    r__4 = ulpinv, r__5 = (r__1 = tnrm - 1.f, dabs(r__1)) / 
			    ulp;
		    r__2 = result[4], r__3 = dmin(r__4,r__5);
		    result[4] = dmax(r__2,r__3);
		    vmx = 0.f;
		    vrmx = 0.f;
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			vtst = c_abs(&vl[jj + j * vl_dim1]);
			if (vtst > vmx) {
			    vmx = vtst;
			}
			i__5 = jj + j * vl_dim1;
			if (r_imag(&vl[jj + j * vl_dim1]) == 0.f && (r__1 = 
				vl[i__5].r, dabs(r__1)) > vrmx) {
			    i__6 = jj + j * vl_dim1;
			    vrmx = (r__2 = vl[i__6].r, dabs(r__2));
			}
/* L130: */
		    }
		    if (vrmx / vmx < 1.f - ulp * 2.f) {
			result[4] = ulpinv;
		    }
/* L140: */
		}

/*              Compute eigenvalues only, and test them */

		clacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		cgeev_("N", "N", &n, &h__[h_offset], lda, &w1[1], dum, &c__1, 
			dum, &c__1, &work[1], &nnwork, &rwork[1], &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___42.ciunit = *nounit;
		    s_wsfe(&io___42);
		    do_fio(&c__1, "CGEEV2", (ftnlen)6);
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
		    i__4 = j;
		    i__5 = j;
		    if (w[i__4].r != w1[i__5].r || w[i__4].i != w1[i__5].i) {
			result[5] = ulpinv;
		    }
/* L150: */
		}

/*              Compute eigenvalues and right eigenvectors, and test them */

		clacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		cgeev_("N", "V", &n, &h__[h_offset], lda, &w1[1], dum, &c__1, 
			&lre[lre_offset], ldlre, &work[1], &nnwork, &rwork[1], 
			 &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___43.ciunit = *nounit;
		    s_wsfe(&io___43);
		    do_fio(&c__1, "CGEEV3", (ftnlen)6);
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
		    i__4 = j;
		    i__5 = j;
		    if (w[i__4].r != w1[i__5].r || w[i__4].i != w1[i__5].i) {
			result[5] = ulpinv;
		    }
/* L160: */
		}

/*              Do Test (6) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			i__5 = j + jj * vr_dim1;
			i__6 = j + jj * lre_dim1;
			if (vr[i__5].r != lre[i__6].r || vr[i__5].i != lre[
				i__6].i) {
			    result[6] = ulpinv;
			}
/* L170: */
		    }
/* L180: */
		}

/*              Compute eigenvalues and left eigenvectors, and test them */

		clacpy_("F", &n, &n, &a[a_offset], lda, &h__[h_offset], lda);
		cgeev_("V", "N", &n, &h__[h_offset], lda, &w1[1], &lre[
			lre_offset], ldlre, dum, &c__1, &work[1], &nnwork, &
			rwork[1], &iinfo);
		if (iinfo != 0) {
		    result[1] = ulpinv;
		    io___44.ciunit = *nounit;
		    s_wsfe(&io___44);
		    do_fio(&c__1, "CGEEV4", (ftnlen)6);
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
		    i__4 = j;
		    i__5 = j;
		    if (w[i__4].r != w1[i__5].r || w[i__4].i != w1[i__5].i) {
			result[5] = ulpinv;
		    }
/* L190: */
		}

/*              Do Test (7) */

		i__3 = n;
		for (j = 1; j <= i__3; ++j) {
		    i__4 = n;
		    for (jj = 1; jj <= i__4; ++jj) {
			i__5 = j + jj * vl_dim1;
			i__6 = j + jj * lre_dim1;
			if (vl[i__5].r != lre[i__6].r || vl[i__5].i != lre[
				i__6].i) {
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
		    if (result[j] >= 0.f) {
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
		    io___47.ciunit = *nounit;
		    s_wsfe(&io___47);
		    do_fio(&c__1, path, (ftnlen)3);
		    e_wsfe();
		    io___48.ciunit = *nounit;
		    s_wsfe(&io___48);
		    e_wsfe();
		    io___49.ciunit = *nounit;
		    s_wsfe(&io___49);
		    e_wsfe();
		    io___50.ciunit = *nounit;
		    s_wsfe(&io___50);
		    e_wsfe();
		    io___51.ciunit = *nounit;
		    s_wsfe(&io___51);
		    do_fio(&c__1, (char *)&(*thresh), (ftnlen)sizeof(real));
		    e_wsfe();
		    ntestf = 2;
		}

		for (j = 1; j <= 7; ++j) {
		    if (result[j] >= *thresh) {
			io___52.ciunit = *nounit;
			s_wsfe(&io___52);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&iwk, (ftnlen)sizeof(integer));
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[j], (ftnlen)sizeof(real)
				);
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

    slasum_(path, nounit, &nerrs, &ntestt);



    return 0;

/*     End of CDRVEV */

} /* cdrvev_ */
