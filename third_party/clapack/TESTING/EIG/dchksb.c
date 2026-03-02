#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b18 = 0.;
static integer c__0 = 0;
static integer c__6 = 6;
static doublereal c_b32 = 1.;
static integer c__1 = 1;
static integer c__4 = 4;

/* Subroutine */ int dchksb_(integer *nsizes, integer *nn, integer *nwdths, 
	integer *kk, integer *ntypes, logical *dotype, integer *iseed, 
	doublereal *thresh, integer *nounit, doublereal *a, integer *lda, 
	doublereal *sd, doublereal *se, doublereal *u, integer *ldu, 
	doublereal *work, integer *lwork, doublereal *result, integer *info)
{
    /* Initialized data */

    static integer ktype[15] = { 1,2,4,4,4,4,4,5,5,5,5,5,8,8,8 };
    static integer kmagn[15] = { 1,1,1,1,1,2,3,1,1,1,2,3,1,2,3 };
    static integer kmode[15] = { 0,0,4,3,1,4,4,4,3,1,4,4,0,0,0 };

    /* Format strings */
    static char fmt_9999[] = "(\002 DCHKSB: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9998[] = "(/1x,a3,\002 -- Real Symmetric Banded Tridiago"
	    "nal Reduction Routines\002)";
    static char fmt_9997[] = "(\002 Matrix types (see DCHKSB for details):"
	    " \002)";
    static char fmt_9996[] = "(/\002 Special Matrices:\002,/\002  1=Zero mat"
	    "rix.                        \002,\002  5=Diagonal: clustered ent"
	    "ries.\002,/\002  2=Identity matrix.                    \002,\002"
	    "  6=Diagonal: large, evenly spaced.\002,/\002  3=Diagonal: evenl"
	    "y spaced entries.    \002,\002  7=Diagonal: small, evenly spaced."
	    "\002,/\002  4=Diagonal: geometr. spaced entries.\002)";
    static char fmt_9995[] = "(\002 Dense \002,a,\002 Banded Matrices:\002,"
	    "/\002  8=Evenly spaced eigenvals.            \002,\002 12=Small,"
	    " evenly spaced eigenvals.\002,/\002  9=Geometrically spaced eige"
	    "nvals.     \002,\002 13=Matrix with random O(1) entries.\002,"
	    "/\002 10=Clustered eigenvalues.              \002,\002 14=Matrix"
	    " with large random entries.\002,/\002 11=Large, evenly spaced ei"
	    "genvals.     \002,\002 15=Matrix with small random entries.\002)";
    static char fmt_9994[] = "(/\002 Tests performed:   (S is Tridiag,  U "
	    "is \002,a,\002,\002,/20x,a,\002 means \002,a,\002.\002,/\002 UPL"
	    "O='U':\002,/\002  1= | A - U S U\002,a1,\002 | / ( |A| n ulp )  "
	    "   \002,\002  2= | I - U U\002,a1,\002 | / ( n ulp )\002,/\002 U"
	    "PLO='L':\002,/\002  3= | A - U S U\002,a1,\002 | / ( |A| n ulp )"
	    "     \002,\002  4= | I - U U\002,a1,\002 | / ( n ulp )\002)";
    static char fmt_9993[] = "(\002 N=\002,i5,\002, K=\002,i4,\002, seed="
	    "\002,4(i4,\002,\002),\002 type \002,i2,\002, test(\002,i2,\002)"
	    "=\002,g10.3)";

    /* System generated locals */
    integer a_dim1, a_offset, u_dim1, u_offset, i__1, i__2, i__3, i__4, i__5, 
	    i__6, i__7;
    doublereal d__1, d__2;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j, k, n, jc, jr;
    doublereal ulp, cond;
    integer jcol, kmax, nmax;
    doublereal unfl, ovfl, temp1;
    logical badnn;
    integer imode;
    extern /* Subroutine */ int dsbt21_(char *, integer *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, doublereal *);
    integer iinfo;
    doublereal aninv, anorm;
    integer nmats, jsize, nerrs, itype, jtype, ntest;
    logical badnnb;
    extern doublereal dlamch_(char *);
    integer idumma[1];
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    integer ioldsd[4];
    extern /* Subroutine */ int dlaset_(char *, integer *, integer *, 
	    doublereal *, doublereal *, doublereal *, integer *), 
	    xerbla_(char *, integer *), dsbtrd_(char *, char *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *), dlatmr_(integer *, integer *, char *, integer *, 
	    char *, doublereal *, integer *, doublereal *, doublereal *, char 
	    *, char *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *, doublereal *, char *, integer *, integer *, integer *, 
	    doublereal *, doublereal *, char *, doublereal *, integer *, 
	    integer *, integer *), dlatms_(integer *, integer *, char *, integer *, char *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    integer *, char *, doublereal *, integer *, doublereal *, integer 
	    *), dlasum_(char *, integer *, integer *, 
	    integer *);
    integer jwidth;
    doublereal rtunfl, rtovfl, ulpinv;
    integer mtypes, ntestt;

    /* Fortran I/O blocks */
    static cilist io___36 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9993, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DCHKSB tests the reduction of a symmetric band matrix to tridiagonal */
/*  form, used with the symmetric eigenvalue problem. */

/*  DSBTRD factors a symmetric band matrix A as  U S U' , where ' means */
/*  transpose, S is symmetric tridiagonal, and U is orthogonal. */
/*  DSBTRD can use either just the lower or just the upper triangle */
/*  of A; DCHKSB checks both cases. */

/*  When DCHKSB is called, a number of matrix "sizes" ("n's"), a number */
/*  of bandwidths ("k's"), and a number of matrix "types" are */
/*  specified.  For each size ("n"), each bandwidth ("k") less than or */
/*  equal to "n", and each type of matrix, one matrix will be generated */
/*  and used to test the symmetric banded reduction routine.  For each */
/*  matrix, a number of tests will be performed: */

/*  (1)     | A - V S V' | / ( |A| n ulp )  computed by DSBTRD with */
/*                                          UPLO='U' */

/*  (2)     | I - UU' | / ( n ulp ) */

/*  (3)     | A - V S V' | / ( |A| n ulp )  computed by DSBTRD with */
/*                                          UPLO='L' */

/*  (4)     | I - UU' | / ( n ulp ) */

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

/*  Arguments */
/*  ========= */

/*  NSIZES  (input) INTEGER */
/*          The number of sizes of matrices to use.  If it is zero, */
/*          DCHKSB does nothing.  It must be at least zero. */

/*  NN      (input) INTEGER array, dimension (NSIZES) */
/*          An array containing the sizes to be used for the matrices. */
/*          Zero values will be skipped.  The values must be at least */
/*          zero. */

/*  NWDTHS  (input) INTEGER */
/*          The number of bandwidths to use.  If it is zero, */
/*          DCHKSB does nothing.  It must be at least zero. */

/*  KK      (input) INTEGER array, dimension (NWDTHS) */
/*          An array containing the bandwidths to be used for the band */
/*          matrices.  The values must be at least zero. */

/*  NTYPES  (input) INTEGER */
/*          The number of elements in DOTYPE.   If it is zero, DCHKSB */
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
/*          next call to DCHKSB to continue the same random number */
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

/*  A       (input/workspace) DOUBLE PRECISION array, dimension */
/*                            (LDA, max(NN)) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least 2 (not 1!) */
/*          and at least max( KK )+1. */

/*  SD      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          Used to hold the diagonal of the tridiagonal matrix computed */
/*          by DSBTRD. */

/*  SE      (workspace) DOUBLE PRECISION array, dimension (max(NN)) */
/*          Used to hold the off-diagonal of the tridiagonal matrix */
/*          computed by DSBTRD. */

/*  U       (workspace) DOUBLE PRECISION array, dimension (LDU, max(NN)) */
/*          Used to hold the orthogonal matrix computed by DSBTRD. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of U.  It must be at least 1 */
/*          and at least max( NN ). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          max( LDA+1, max(NN)+1 )*max(NN). */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (4) */
/*          The values computed by the tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  INFO    (output) INTEGER */
/*          If 0, then everything ran OK. */

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
    --kk;
    --dotype;
    --iseed;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --sd;
    --se;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --work;
    --result;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

    ntestt = 0;
    *info = 0;

/*     Important constants */

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

    badnnb = FALSE_;
    kmax = 0;
    i__1 = *nsizes;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__2 = kmax, i__3 = kk[j];
	kmax = max(i__2,i__3);
	if (kk[j] < 0) {
	    badnnb = TRUE_;
	}
/* L20: */
    }
/* Computing MIN */
    i__1 = nmax - 1;
    kmax = min(i__1,kmax);

/*     Check for errors */

    if (*nsizes < 0) {
	*info = -1;
    } else if (badnn) {
	*info = -2;
    } else if (*nwdths < 0) {
	*info = -3;
    } else if (badnnb) {
	*info = -4;
    } else if (*ntypes < 0) {
	*info = -5;
    } else if (*lda < kmax + 1) {
	*info = -11;
    } else if (*ldu < nmax) {
	*info = -15;
    } else if ((max(*lda,nmax) + 1) * nmax > *lwork) {
	*info = -17;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DCHKSB", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*nsizes == 0 || *ntypes == 0 || *nwdths == 0) {
	return 0;
    }

/*     More Important constants */

    unfl = dlamch_("Safe minimum");
    ovfl = 1. / unfl;
    ulp = dlamch_("Epsilon") * dlamch_("Base");
    ulpinv = 1. / ulp;
    rtunfl = sqrt(unfl);
    rtovfl = sqrt(ovfl);

/*     Loop over sizes, types */

    nerrs = 0;
    nmats = 0;

    i__1 = *nsizes;
    for (jsize = 1; jsize <= i__1; ++jsize) {
	n = nn[jsize];
	aninv = 1. / (doublereal) max(1,n);

	i__2 = *nwdths;
	for (jwidth = 1; jwidth <= i__2; ++jwidth) {
	    k = kk[jwidth];
	    if (k > n) {
		goto L180;
	    }
/* Computing MAX */
/* Computing MIN */
	    i__5 = n - 1;
	    i__3 = 0, i__4 = min(i__5,k);
	    k = max(i__3,i__4);

	    if (*nsizes != 1) {
		mtypes = min(15,*ntypes);
	    } else {
		mtypes = min(16,*ntypes);
	    }

	    i__3 = mtypes;
	    for (jtype = 1; jtype <= i__3; ++jtype) {
		if (! dotype[jtype]) {
		    goto L170;
		}
		++nmats;
		ntest = 0;

		for (j = 1; j <= 4; ++j) {
		    ioldsd[j - 1] = iseed[j];
/* L30: */
		}

/*              Compute "A". */
/*              Store as "Upper"; later, we will copy to other format. */

/*              Control parameters: */

/*                  KMAGN  KMODE        KTYPE */
/*              =1  O(1)   clustered 1  zero */
/*              =2  large  clustered 2  identity */
/*              =3  small  exponential  (none) */
/*              =4         arithmetic   diagonal, (w/ eigenvalues) */
/*              =5         random log   symmetric, w/ eigenvalues */
/*              =6         random       (none) */
/*              =7                      random diagonal */
/*              =8                      random symmetric */
/*              =9                      positive definite */
/*              =10                     diagonally dominant tridiagonal */

		if (mtypes > 15) {
		    goto L100;
		}

		itype = ktype[jtype - 1];
		imode = kmode[jtype - 1];

/*              Compute norm */

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

		dlaset_("Full", lda, &n, &c_b18, &c_b18, &a[a_offset], lda);
		iinfo = 0;
		if (jtype <= 15) {
		    cond = ulpinv;
		} else {
		    cond = ulpinv * aninv / 10.;
		}

/*              Special Matrices -- Identity & Jordan block */

/*                 Zero */

		if (itype == 1) {
		    iinfo = 0;

		} else if (itype == 2) {

/*                 Identity */

		    i__4 = n;
		    for (jcol = 1; jcol <= i__4; ++jcol) {
			a[k + 1 + jcol * a_dim1] = anorm;
/* L80: */
		    }

		} else if (itype == 4) {

/*                 Diagonal Matrix, [Eigen]values Specified */

		    dlatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &
			    cond, &anorm, &c__0, &c__0, "Q", &a[k + 1 + 
			    a_dim1], lda, &work[n + 1], &iinfo);

		} else if (itype == 5) {

/*                 Symmetric, eigenvalues specified */

		    dlatms_(&n, &n, "S", &iseed[1], "S", &work[1], &imode, &
			    cond, &anorm, &k, &k, "Q", &a[a_offset], lda, &
			    work[n + 1], &iinfo);

		} else if (itype == 7) {

/*                 Diagonal, random eigenvalues */

		    dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &
			    c_b32, &c_b32, "T", "N", &work[n + 1], &c__1, &
			    c_b32, &work[(n << 1) + 1], &c__1, &c_b32, "N", 
			    idumma, &c__0, &c__0, &c_b18, &anorm, "Q", &a[k + 
			    1 + a_dim1], lda, idumma, &iinfo);

		} else if (itype == 8) {

/*                 Symmetric, random eigenvalues */

		    dlatmr_(&n, &n, "S", &iseed[1], "S", &work[1], &c__6, &
			    c_b32, &c_b32, "T", "N", &work[n + 1], &c__1, &
			    c_b32, &work[(n << 1) + 1], &c__1, &c_b32, "N", 
			    idumma, &k, &k, &c_b18, &anorm, "Q", &a[a_offset], 
			     lda, idumma, &iinfo);

		} else if (itype == 9) {

/*                 Positive definite, eigenvalues specified. */

		    dlatms_(&n, &n, "S", &iseed[1], "P", &work[1], &imode, &
			    cond, &anorm, &k, &k, "Q", &a[a_offset], lda, &
			    work[n + 1], &iinfo);

		} else if (itype == 10) {

/*                 Positive definite tridiagonal, eigenvalues specified. */

		    if (n > 1) {
			k = max(1,k);
		    }
		    dlatms_(&n, &n, "S", &iseed[1], "P", &work[1], &imode, &
			    cond, &anorm, &c__1, &c__1, "Q", &a[k + a_dim1], 
			    lda, &work[n + 1], &iinfo);
		    i__4 = n;
		    for (i__ = 2; i__ <= i__4; ++i__) {
			temp1 = (d__1 = a[k + i__ * a_dim1], abs(d__1)) / 
				sqrt((d__2 = a[k + 1 + (i__ - 1) * a_dim1] * 
				a[k + 1 + i__ * a_dim1], abs(d__2)));
			if (temp1 > .5) {
			    a[k + i__ * a_dim1] = sqrt((d__1 = a[k + 1 + (i__ 
				    - 1) * a_dim1] * a[k + 1 + i__ * a_dim1], 
				    abs(d__1))) * .5;
			}
/* L90: */
		    }

		} else {

		    iinfo = 1;
		}

		if (iinfo != 0) {
		    io___36.ciunit = *nounit;
		    s_wsfe(&io___36);
		    do_fio(&c__1, "Generator", (ftnlen)9);
		    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer));
		    do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(integer))
			    ;
		    e_wsfe();
		    *info = abs(iinfo);
		    return 0;
		}

L100:

/*              Call DSBTRD to compute S and U from upper triangle. */

		i__4 = k + 1;
		dlacpy_(" ", &i__4, &n, &a[a_offset], lda, &work[1], lda);

		ntest = 1;
		dsbtrd_("V", "U", &n, &k, &work[1], lda, &sd[1], &se[1], &u[
			u_offset], ldu, &work[*lda * n + 1], &iinfo);

		if (iinfo != 0) {
		    io___37.ciunit = *nounit;
		    s_wsfe(&io___37);
		    do_fio(&c__1, "DSBTRD(U)", (ftnlen)9);
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
			goto L150;
		    }
		}

/*              Do tests 1 and 2 */

		dsbt21_("Upper", &n, &k, &c__1, &a[a_offset], lda, &sd[1], &
			se[1], &u[u_offset], ldu, &work[1], &result[1]);

/*              Convert A from Upper-Triangle-Only storage to */
/*              Lower-Triangle-Only storage. */

		i__4 = n;
		for (jc = 1; jc <= i__4; ++jc) {
/* Computing MIN */
		    i__6 = k, i__7 = n - jc;
		    i__5 = min(i__6,i__7);
		    for (jr = 0; jr <= i__5; ++jr) {
			a[jr + 1 + jc * a_dim1] = a[k + 1 - jr + (jc + jr) * 
				a_dim1];
/* L110: */
		    }
/* L120: */
		}
		i__4 = n;
		for (jc = n + 1 - k; jc <= i__4; ++jc) {
/* Computing MIN */
		    i__5 = k, i__6 = n - jc;
		    i__7 = k;
		    for (jr = min(i__5,i__6) + 1; jr <= i__7; ++jr) {
			a[jr + 1 + jc * a_dim1] = 0.;
/* L130: */
		    }
/* L140: */
		}

/*              Call DSBTRD to compute S and U from lower triangle */

		i__4 = k + 1;
		dlacpy_(" ", &i__4, &n, &a[a_offset], lda, &work[1], lda);

		ntest = 3;
		dsbtrd_("V", "L", &n, &k, &work[1], lda, &sd[1], &se[1], &u[
			u_offset], ldu, &work[*lda * n + 1], &iinfo);

		if (iinfo != 0) {
		    io___40.ciunit = *nounit;
		    s_wsfe(&io___40);
		    do_fio(&c__1, "DSBTRD(L)", (ftnlen)9);
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
			goto L150;
		    }
		}
		ntest = 4;

/*              Do tests 3 and 4 */

		dsbt21_("Lower", &n, &k, &c__1, &a[a_offset], lda, &sd[1], &
			se[1], &u[u_offset], ldu, &work[1], &result[3]);

/*              End of Loop -- Check for RESULT(j) > THRESH */

L150:
		ntestt += ntest;

/*              Print out tests which fail. */

		i__4 = ntest;
		for (jr = 1; jr <= i__4; ++jr) {
		    if (result[jr] >= *thresh) {

/*                    If this is the first test to fail, */
/*                    print a header to the data file. */

			if (nerrs == 0) {
			    io___41.ciunit = *nounit;
			    s_wsfe(&io___41);
			    do_fio(&c__1, "DSB", (ftnlen)3);
			    e_wsfe();
			    io___42.ciunit = *nounit;
			    s_wsfe(&io___42);
			    e_wsfe();
			    io___43.ciunit = *nounit;
			    s_wsfe(&io___43);
			    e_wsfe();
			    io___44.ciunit = *nounit;
			    s_wsfe(&io___44);
			    do_fio(&c__1, "Symmetric", (ftnlen)9);
			    e_wsfe();
			    io___45.ciunit = *nounit;
			    s_wsfe(&io___45);
			    do_fio(&c__1, "orthogonal", (ftnlen)10);
			    do_fio(&c__1, "'", (ftnlen)1);
			    do_fio(&c__1, "transpose", (ftnlen)9);
			    for (j = 1; j <= 4; ++j) {
				do_fio(&c__1, "'", (ftnlen)1);
			    }
			    e_wsfe();
			}
			++nerrs;
			io___46.ciunit = *nounit;
			s_wsfe(&io___46);
			do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
			do_fio(&c__4, (char *)&ioldsd[0], (ftnlen)sizeof(
				integer));
			do_fio(&c__1, (char *)&jtype, (ftnlen)sizeof(integer))
				;
			do_fio(&c__1, (char *)&jr, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&result[jr], (ftnlen)sizeof(
				doublereal));
			e_wsfe();
		    }
/* L160: */
		}

L170:
		;
	    }
L180:
	    ;
	}
/* L190: */
    }

/*     Summary */

    dlasum_("DSB", nounit, &nerrs, &ntestt);
    return 0;





/*     End of DCHKSB */

} /* dchksb_ */
