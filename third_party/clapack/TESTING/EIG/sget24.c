#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer selopt, seldim;
    logical selval[20];
    real selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

/* Table of constant values */

static integer c__1 = 1;
static integer c__4 = 4;
static real c_b35 = 1.f;
static real c_b41 = 0.f;
static real c_b44 = -1.f;

/* Subroutine */ int sget24_(logical *comp, integer *jtype, real *thresh, 
	integer *iseed, integer *nounit, integer *n, real *a, integer *lda, 
	real *h__, real *ht, real *wr, real *wi, real *wrt, real *wit, real *
	wrtmp, real *witmp, real *vs, integer *ldvs, real *vs1, real *rcdein, 
	real *rcdvin, integer *nslct, integer *islct, real *result, real *
	work, integer *lwork, integer *iwork, logical *bwork, integer *info)
{
    /* Format strings */
    static char fmt_9998[] = "(\002 SGET24: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(\002 SGET24: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, INPUT EXAMPLE NUMBER = \002,"
	    "i4)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, ht_dim1, ht_offset, vs_dim1, 
	    vs_offset, vs1_dim1, vs1_offset, i__1, i__2;
    real r__1, r__2, r__3, r__4;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double r_sign(real *, real *), sqrt(doublereal);

    /* Local variables */
    integer i__, j;
    real v, eps, tol, tmp, ulp;
    integer sdim, kmin, itmp, ipnt[20], rsub;
    char sort[1];
    integer sdim1, iinfo;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm, vimin, tolin;
    extern /* Subroutine */ int sort01_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *, real *);
    real vrmin;
    integer isort;
    real wnorm;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *);
    real rcnde1, rcndv1;
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    real rconde;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    integer knteig;
    real rcondv;
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *);
    extern logical sslect_(real *, real *);
    extern /* Subroutine */ int sgeesx_(char *, char *, L_fp, char *, integer 
	    *, real *, integer *, integer *, real *, real *, real *, integer *
, real *, real *, real *, integer *, integer *, integer *, 
	    logical *, integer *);
    integer liwork;
    real smlnum, ulpinv;

    /* Fortran I/O blocks */
    static cilist io___13 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     SGET24 checks the nonsymmetric eigenvalue (Schur form) problem */
/*     expert driver SGEESX. */

/*     If COMP = .FALSE., the first 13 of the following tests will be */
/*     be performed on the input matrix A, and also tests 14 and 15 */
/*     if LWORK is sufficiently large. */
/*     If COMP = .TRUE., all 17 test will be performed. */

/*     (1)     0 if T is in Schur form, 1/ulp otherwise */
/*            (no sorting of eigenvalues) */

/*     (2)     | A - VS T VS' | / ( n |A| ulp ) */

/*       Here VS is the matrix of Schur eigenvectors, and T is in Schur */
/*       form  (no sorting of eigenvalues). */

/*     (3)     | I - VS VS' | / ( n ulp ) (no sorting of eigenvalues). */

/*     (4)     0     if WR+sqrt(-1)*WI are eigenvalues of T */
/*             1/ulp otherwise */
/*             (no sorting of eigenvalues) */

/*     (5)     0     if T(with VS) = T(without VS), */
/*             1/ulp otherwise */
/*             (no sorting of eigenvalues) */

/*     (6)     0     if eigenvalues(with VS) = eigenvalues(without VS), */
/*             1/ulp otherwise */
/*             (no sorting of eigenvalues) */

/*     (7)     0 if T is in Schur form, 1/ulp otherwise */
/*             (with sorting of eigenvalues) */

/*     (8)     | A - VS T VS' | / ( n |A| ulp ) */

/*       Here VS is the matrix of Schur eigenvectors, and T is in Schur */
/*       form  (with sorting of eigenvalues). */

/*     (9)     | I - VS VS' | / ( n ulp ) (with sorting of eigenvalues). */

/*     (10)    0     if WR+sqrt(-1)*WI are eigenvalues of T */
/*             1/ulp otherwise */
/*             If workspace sufficient, also compare WR, WI with and */
/*             without reciprocal condition numbers */
/*             (with sorting of eigenvalues) */

/*     (11)    0     if T(with VS) = T(without VS), */
/*             1/ulp otherwise */
/*             If workspace sufficient, also compare T with and without */
/*             reciprocal condition numbers */
/*             (with sorting of eigenvalues) */

/*     (12)    0     if eigenvalues(with VS) = eigenvalues(without VS), */
/*             1/ulp otherwise */
/*             If workspace sufficient, also compare VS with and without */
/*             reciprocal condition numbers */
/*             (with sorting of eigenvalues) */

/*     (13)    if sorting worked and SDIM is the number of */
/*             eigenvalues which were SELECTed */
/*             If workspace sufficient, also compare SDIM with and */
/*             without reciprocal condition numbers */

/*     (14)    if RCONDE the same no matter if VS and/or RCONDV computed */

/*     (15)    if RCONDV the same no matter if VS and/or RCONDE computed */

/*     (16)  |RCONDE - RCDEIN| / cond(RCONDE) */

/*        RCONDE is the reciprocal average eigenvalue condition number */
/*        computed by SGEESX and RCDEIN (the precomputed true value) */
/*        is supplied as input.  cond(RCONDE) is the condition number */
/*        of RCONDE, and takes errors in computing RCONDE into account, */
/*        so that the resulting quantity should be O(ULP). cond(RCONDE) */
/*        is essentially given by norm(A)/RCONDV. */

/*     (17)  |RCONDV - RCDVIN| / cond(RCONDV) */

/*        RCONDV is the reciprocal right invariant subspace condition */
/*        number computed by SGEESX and RCDVIN (the precomputed true */
/*        value) is supplied as input. cond(RCONDV) is the condition */
/*        number of RCONDV, and takes errors in computing RCONDV into */
/*        account, so that the resulting quantity should be O(ULP). */
/*        cond(RCONDV) is essentially given by norm(A)/RCONDE. */

/*  Arguments */
/*  ========= */

/*  COMP    (input) LOGICAL */
/*          COMP describes which input tests to perform: */
/*            = .FALSE. if the computed condition numbers are not to */
/*                      be tested against RCDVIN and RCDEIN */
/*            = .TRUE.  if they are to be compared */

/*  JTYPE   (input) INTEGER */
/*          Type of input matrix. Used to label output if error occurs. */

/*  ISEED   (input) INTEGER array, dimension (4) */
/*          If COMP = .FALSE., the random number generator seed */
/*          used to produce matrix. */
/*          If COMP = .TRUE., ISEED(1) = the number of the example. */
/*          Used to label output if error occurs. */

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

/*  N       (input) INTEGER */
/*          The dimension of A. N must be at least 0. */

/*  A       (input/output) REAL array, dimension (LDA, N) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least N. */

/*  H       (workspace) REAL array, dimension (LDA, N) */
/*          Another copy of the test matrix A, modified by SGEESX. */

/*  HT      (workspace) REAL array, dimension (LDA, N) */
/*          Yet another copy of the test matrix A, modified by SGEESX. */

/*  WR      (workspace) REAL array, dimension (N) */
/*  WI      (workspace) REAL array, dimension (N) */
/*          The real and imaginary parts of the eigenvalues of A. */
/*          On exit, WR + WI*i are the eigenvalues of the matrix in A. */

/*  WRT     (workspace) REAL array, dimension (N) */
/*  WIT     (workspace) REAL array, dimension (N) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but those computed when SGEESX only computes a partial */
/*          eigendecomposition, i.e. not Schur vectors */

/*  WRTMP   (workspace) REAL array, dimension (N) */
/*  WITMP   (workspace) REAL array, dimension (N) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but sorted by increasing real part. */

/*  VS      (workspace) REAL array, dimension (LDVS, N) */
/*          VS holds the computed Schur vectors. */

/*  LDVS    (input) INTEGER */
/*          Leading dimension of VS. Must be at least max(1, N). */

/*  VS1     (workspace) REAL array, dimension (LDVS, N) */
/*          VS1 holds another copy of the computed Schur vectors. */

/*  RCDEIN  (input) REAL */
/*          When COMP = .TRUE. RCDEIN holds the precomputed reciprocal */
/*          condition number for the average of selected eigenvalues. */

/*  RCDVIN  (input) REAL */
/*          When COMP = .TRUE. RCDVIN holds the precomputed reciprocal */
/*          condition number for the selected right invariant subspace. */

/*  NSLCT   (input) INTEGER */
/*          When COMP = .TRUE. the number of selected eigenvalues */
/*          corresponding to the precomputed values RCDEIN and RCDVIN. */

/*  ISLCT   (input) INTEGER array, dimension (NSLCT) */
/*          When COMP = .TRUE. ISLCT selects the eigenvalues of the */
/*          input matrix corresponding to the precomputed values RCDEIN */
/*          and RCDVIN. For I=1, ... ,NSLCT, if ISLCT(I) = J, then the */
/*          eigenvalue with the J-th largest real part is selected. */
/*          Not referenced if COMP = .FALSE. */

/*  RESULT  (output) REAL array, dimension (17) */
/*          The values computed by the 17 tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK to be passed to SGEESX. This */
/*          must be at least 3*N, and N+N**2 if tests 14--16 are to */
/*          be performed. */

/*  IWORK   (workspace) INTEGER array, dimension (N*N) */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          If 0,  successful exit. */
/*          If <0, input parameter -INFO had an incorrect value. */
/*          If >0, SGEESX returned an error code, the absolute */
/*                 value of which is returned. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Arrays in Common .. */
/*     .. */
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

    /* Parameter adjustments */
    --iseed;
    ht_dim1 = *lda;
    ht_offset = 1 + ht_dim1;
    ht -= ht_offset;
    h_dim1 = *lda;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --wr;
    --wi;
    --wrt;
    --wit;
    --wrtmp;
    --witmp;
    vs1_dim1 = *ldvs;
    vs1_offset = 1 + vs1_dim1;
    vs1 -= vs1_offset;
    vs_dim1 = *ldvs;
    vs_offset = 1 + vs_dim1;
    vs -= vs_offset;
    --islct;
    --result;
    --work;
    --iwork;
    --bwork;

    /* Function Body */
    *info = 0;
    if (*thresh < 0.f) {
	*info = -3;
    } else if (*nounit <= 0) {
	*info = -5;
    } else if (*n < 0) {
	*info = -6;
    } else if (*lda < 1 || *lda < *n) {
	*info = -8;
    } else if (*ldvs < 1 || *ldvs < *n) {
	*info = -18;
    } else if (*lwork < *n * 3) {
	*info = -26;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SGET24", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

    for (i__ = 1; i__ <= 17; ++i__) {
	result[i__] = -1.f;
/* L10: */
    }

    if (*n == 0) {
	return 0;
    }

/*     Important constants */

    smlnum = slamch_("Safe minimum");
    ulp = slamch_("Precision");
    ulpinv = 1.f / ulp;

/*     Perform tests (1)-(13) */

    sslct_1.selopt = 0;
    liwork = *n * *n;
    for (isort = 0; isort <= 1; ++isort) {
	if (isort == 0) {
	    *(unsigned char *)sort = 'N';
	    rsub = 0;
	} else {
	    *(unsigned char *)sort = 'S';
	    rsub = 6;
	}

/*        Compute Schur form and Schur vectors, and test them */

	slacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
	sgeesx_("V", sort, (L_fp)sslect_, "N", n, &h__[h_offset], lda, &sdim, 
		&wr[1], &wi[1], &vs[vs_offset], ldvs, &rconde, &rcondv, &work[
		1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[rsub + 1] = ulpinv;
	    if (*jtype != 22) {
		io___13.ciunit = *nounit;
		s_wsfe(&io___13);
		do_fio(&c__1, "SGEESX1", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___14.ciunit = *nounit;
		s_wsfe(&io___14);
		do_fio(&c__1, "SGEESX1", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    return 0;
	}
	if (isort == 0) {
	    scopy_(n, &wr[1], &c__1, &wrtmp[1], &c__1);
	    scopy_(n, &wi[1], &c__1, &witmp[1], &c__1);
	}

/*        Do Test (1) or Test (7) */

	result[rsub + 1] = 0.f;
	i__1 = *n - 2;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j + 2; i__ <= i__2; ++i__) {
		if (h__[i__ + j * h_dim1] != 0.f) {
		    result[rsub + 1] = ulpinv;
		}
/* L20: */
	    }
/* L30: */
	}
	i__1 = *n - 2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + 1 + i__ * h_dim1] != 0.f && h__[i__ + 2 + (i__ + 1) 
		    * h_dim1] != 0.f) {
		result[rsub + 1] = ulpinv;
	    }
/* L40: */
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + 1 + i__ * h_dim1] != 0.f) {
		if (h__[i__ + i__ * h_dim1] != h__[i__ + 1 + (i__ + 1) * 
			h_dim1] || h__[i__ + (i__ + 1) * h_dim1] == 0.f || 
			r_sign(&c_b35, &h__[i__ + 1 + i__ * h_dim1]) == 
			r_sign(&c_b35, &h__[i__ + (i__ + 1) * h_dim1])) {
		    result[rsub + 1] = ulpinv;
		}
	    }
/* L50: */
	}

/*        Test (2) or (8): Compute norm(A - Q*H*Q') / (norm(A) * N * ULP) */

/*        Copy A to VS1, used as workspace */

	slacpy_(" ", n, n, &a[a_offset], lda, &vs1[vs1_offset], ldvs);

/*        Compute Q*H and store in HT. */

	sgemm_("No transpose", "No transpose", n, n, n, &c_b35, &vs[vs_offset]
, ldvs, &h__[h_offset], lda, &c_b41, &ht[ht_offset], lda);

/*        Compute A - Q*H*Q' */

	sgemm_("No transpose", "Transpose", n, n, n, &c_b44, &ht[ht_offset], 
		lda, &vs[vs_offset], ldvs, &c_b35, &vs1[vs1_offset], ldvs);

/* Computing MAX */
	r__1 = slange_("1", n, n, &a[a_offset], lda, &work[1]);
	anorm = dmax(r__1,smlnum);
	wnorm = slange_("1", n, n, &vs1[vs1_offset], ldvs, &work[1]);

	if (anorm > wnorm) {
	    result[rsub + 2] = wnorm / anorm / (*n * ulp);
	} else {
	    if (anorm < 1.f) {
/* Computing MIN */
		r__1 = wnorm, r__2 = *n * anorm;
		result[rsub + 2] = dmin(r__1,r__2) / anorm / (*n * ulp);
	    } else {
/* Computing MIN */
		r__1 = wnorm / anorm, r__2 = (real) (*n);
		result[rsub + 2] = dmin(r__1,r__2) / (*n * ulp);
	    }
	}

/*        Test (3) or (9):  Compute norm( I - Q'*Q ) / ( N * ULP ) */

	sort01_("Columns", n, n, &vs[vs_offset], ldvs, &work[1], lwork, &
		result[rsub + 3]);

/*        Do Test (4) or Test (10) */

	result[rsub + 4] = 0.f;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + i__ * h_dim1] != wr[i__]) {
		result[rsub + 4] = ulpinv;
	    }
/* L60: */
	}
	if (*n > 1) {
	    if (h__[h_dim1 + 2] == 0.f && wi[1] != 0.f) {
		result[rsub + 4] = ulpinv;
	    }
	    if (h__[*n + (*n - 1) * h_dim1] == 0.f && wi[*n] != 0.f) {
		result[rsub + 4] = ulpinv;
	    }
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + 1 + i__ * h_dim1] != 0.f) {
		tmp = sqrt((r__1 = h__[i__ + 1 + i__ * h_dim1], dabs(r__1))) *
			 sqrt((r__2 = h__[i__ + (i__ + 1) * h_dim1], dabs(
			r__2)));
/* Computing MAX */
/* Computing MAX */
		r__4 = ulp * tmp;
		r__2 = result[rsub + 4], r__3 = (r__1 = wi[i__] - tmp, dabs(
			r__1)) / dmax(r__4,smlnum);
		result[rsub + 4] = dmax(r__2,r__3);
/* Computing MAX */
/* Computing MAX */
		r__4 = ulp * tmp;
		r__2 = result[rsub + 4], r__3 = (r__1 = wi[i__ + 1] + tmp, 
			dabs(r__1)) / dmax(r__4,smlnum);
		result[rsub + 4] = dmax(r__2,r__3);
	    } else if (i__ > 1) {
		if (h__[i__ + 1 + i__ * h_dim1] == 0.f && h__[i__ + (i__ - 1) 
			* h_dim1] == 0.f && wi[i__] != 0.f) {
		    result[rsub + 4] = ulpinv;
		}
	    }
/* L70: */
	}

/*        Do Test (5) or Test (11) */

	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("N", sort, (L_fp)sslect_, "N", n, &ht[ht_offset], lda, &sdim, 
		&wrt[1], &wit[1], &vs[vs_offset], ldvs, &rconde, &rcondv, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[rsub + 5] = ulpinv;
	    if (*jtype != 22) {
		io___19.ciunit = *nounit;
		s_wsfe(&io___19);
		do_fio(&c__1, "SGEESX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___20.ciunit = *nounit;
		s_wsfe(&io___20);
		do_fio(&c__1, "SGEESX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

	result[rsub + 5] = 0.f;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]) {
		    result[rsub + 5] = ulpinv;
		}
/* L80: */
	    }
/* L90: */
	}

/*        Do Test (6) or Test (12) */

	result[rsub + 6] = 0.f;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[rsub + 6] = ulpinv;
	    }
/* L100: */
	}

/*        Do Test (13) */

	if (isort == 1) {
	    result[13] = 0.f;
	    knteig = 0;
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		r__1 = -wi[i__];
		if (sslect_(&wr[i__], &wi[i__]) || sslect_(&wr[i__], &r__1)) {
		    ++knteig;
		}
		if (i__ < *n) {
		    r__1 = -wi[i__ + 1];
		    r__2 = -wi[i__];
		    if ((sslect_(&wr[i__ + 1], &wi[i__ + 1]) || sslect_(&wr[
			    i__ + 1], &r__1)) && ! (sslect_(&wr[i__], &wi[i__]
) || sslect_(&wr[i__], &r__2)) && iinfo != *n + 2)
			     {
			result[13] = ulpinv;
		    }
		}
/* L110: */
	    }
	    if (sdim != knteig) {
		result[13] = ulpinv;
	    }
	}

/* L120: */
    }

/*     If there is enough workspace, perform tests (14) and (15) */
/*     as well as (10) through (13) */

    if (*lwork >= *n + *n * *n / 2) {

/*        Compute both RCONDE and RCONDV with VS */

	*(unsigned char *)sort = 'S';
	result[14] = 0.f;
	result[15] = 0.f;
	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("V", sort, (L_fp)sslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rconde, &rcondv, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___23.ciunit = *nounit;
		s_wsfe(&io___23);
		do_fio(&c__1, "SGEESX3", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___24.ciunit = *nounit;
		s_wsfe(&io___24);
		do_fio(&c__1, "SGEESX3", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]) {
		    result[11] = ulpinv;
		}
		if (vs[i__ + j * vs_dim1] != vs1[i__ + j * vs1_dim1]) {
		    result[12] = ulpinv;
		}
/* L130: */
	    }
/* L140: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute both RCONDE and RCONDV without VS, and compare */

	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("N", sort, (L_fp)sslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___27.ciunit = *nounit;
		s_wsfe(&io___27);
		do_fio(&c__1, "SGEESX4", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___28.ciunit = *nounit;
		s_wsfe(&io___28);
		do_fio(&c__1, "SGEESX4", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

/*        Perform tests (14) and (15) */

	if (rcnde1 != rconde) {
	    result[14] = ulpinv;
	}
	if (rcndv1 != rcondv) {
	    result[15] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]) {
		    result[11] = ulpinv;
		}
		if (vs[i__ + j * vs_dim1] != vs1[i__ + j * vs1_dim1]) {
		    result[12] = ulpinv;
		}
/* L150: */
	    }
/* L160: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDE with VS, and compare */

	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("V", sort, (L_fp)sslect_, "E", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    if (*jtype != 22) {
		io___29.ciunit = *nounit;
		s_wsfe(&io___29);
		do_fio(&c__1, "SGEESX5", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___30.ciunit = *nounit;
		s_wsfe(&io___30);
		do_fio(&c__1, "SGEESX5", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

/*        Perform test (14) */

	if (rcnde1 != rconde) {
	    result[14] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]) {
		    result[11] = ulpinv;
		}
		if (vs[i__ + j * vs_dim1] != vs1[i__ + j * vs1_dim1]) {
		    result[12] = ulpinv;
		}
/* L170: */
	    }
/* L180: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDE without VS, and compare */

	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("N", sort, (L_fp)sslect_, "E", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    if (*jtype != 22) {
		io___31.ciunit = *nounit;
		s_wsfe(&io___31);
		do_fio(&c__1, "SGEESX6", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___32.ciunit = *nounit;
		s_wsfe(&io___32);
		do_fio(&c__1, "SGEESX6", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

/*        Perform test (14) */

	if (rcnde1 != rconde) {
	    result[14] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]) {
		    result[11] = ulpinv;
		}
		if (vs[i__ + j * vs_dim1] != vs1[i__ + j * vs1_dim1]) {
		    result[12] = ulpinv;
		}
/* L190: */
	    }
/* L200: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDV with VS, and compare */

	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("V", sort, (L_fp)sslect_, "V", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___33.ciunit = *nounit;
		s_wsfe(&io___33);
		do_fio(&c__1, "SGEESX7", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___34.ciunit = *nounit;
		s_wsfe(&io___34);
		do_fio(&c__1, "SGEESX7", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

/*        Perform test (15) */

	if (rcndv1 != rcondv) {
	    result[15] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]) {
		    result[11] = ulpinv;
		}
		if (vs[i__ + j * vs_dim1] != vs1[i__ + j * vs1_dim1]) {
		    result[12] = ulpinv;
		}
/* L210: */
	    }
/* L220: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDV without VS, and compare */

	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("N", sort, (L_fp)sslect_, "V", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___35.ciunit = *nounit;
		s_wsfe(&io___35);
		do_fio(&c__1, "SGEESX8", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___36.ciunit = *nounit;
		s_wsfe(&io___36);
		do_fio(&c__1, "SGEESX8", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

/*        Perform test (15) */

	if (rcndv1 != rcondv) {
	    result[15] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (h__[i__ + j * h_dim1] != ht[i__ + j * ht_dim1]) {
		    result[11] = ulpinv;
		}
		if (vs[i__ + j * vs_dim1] != vs1[i__ + j * vs1_dim1]) {
		    result[12] = ulpinv;
		}
/* L230: */
	    }
/* L240: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

    }

L250:

/*     If there are precomputed reciprocal condition numbers, compare */
/*     computed values with them. */

    if (*comp) {

/*        First set up SELOPT, SELDIM, SELVAL, SELWR, and SELWI so that */
/*        the logical function SSLECT selects the eigenvalues specified */
/*        by NSLCT and ISLCT. */

	sslct_1.seldim = *n;
	sslct_1.selopt = 1;
	eps = dmax(ulp,5.9605e-8f);
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ipnt[i__ - 1] = i__;
	    sslct_1.selval[i__ - 1] = FALSE_;
	    sslct_1.selwr[i__ - 1] = wrtmp[i__];
	    sslct_1.selwi[i__ - 1] = witmp[i__];
/* L260: */
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    kmin = i__;
	    vrmin = wrtmp[i__];
	    vimin = witmp[i__];
	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		if (wrtmp[j] < vrmin) {
		    kmin = j;
		    vrmin = wrtmp[j];
		    vimin = witmp[j];
		}
/* L270: */
	    }
	    wrtmp[kmin] = wrtmp[i__];
	    witmp[kmin] = witmp[i__];
	    wrtmp[i__] = vrmin;
	    witmp[i__] = vimin;
	    itmp = ipnt[i__ - 1];
	    ipnt[i__ - 1] = ipnt[kmin - 1];
	    ipnt[kmin - 1] = itmp;
/* L280: */
	}
	i__1 = *nslct;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    sslct_1.selval[ipnt[islct[i__] - 1] - 1] = TRUE_;
/* L290: */
	}

/*        Compute condition numbers */

	slacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	sgeesx_("N", "S", (L_fp)sslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		&wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rconde, &rcondv, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[16] = ulpinv;
	    result[17] = ulpinv;
	    io___43.ciunit = *nounit;
	    s_wsfe(&io___43);
	    do_fio(&c__1, "SGEESX9", (ftnlen)7);
	    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *info = abs(iinfo);
	    goto L300;
	}

/*        Compare condition number for average of selected eigenvalues */
/*        taking its condition number into account */

	anorm = slange_("1", n, n, &a[a_offset], lda, &work[1]);
/* Computing MAX */
	r__1 = (real) (*n) * eps * anorm;
	v = dmax(r__1,smlnum);
	if (anorm == 0.f) {
	    v = 1.f;
	}
	if (v > rcondv) {
	    tol = 1.f;
	} else {
	    tol = v / rcondv;
	}
	if (v > *rcdvin) {
	    tolin = 1.f;
	} else {
	    tolin = v / *rcdvin;
	}
/* Computing MAX */
	r__1 = tol, r__2 = smlnum / eps;
	tol = dmax(r__1,r__2);
/* Computing MAX */
	r__1 = tolin, r__2 = smlnum / eps;
	tolin = dmax(r__1,r__2);
	if (eps * (*rcdein - tolin) > rconde + tol) {
	    result[16] = ulpinv;
	} else if (*rcdein - tolin > rconde + tol) {
	    result[16] = (*rcdein - tolin) / (rconde + tol);
	} else if (*rcdein + tolin < eps * (rconde - tol)) {
	    result[16] = ulpinv;
	} else if (*rcdein + tolin < rconde - tol) {
	    result[16] = (rconde - tol) / (*rcdein + tolin);
	} else {
	    result[16] = 1.f;
	}

/*        Compare condition numbers for right invariant subspace */
/*        taking its condition number into account */

	if (v > rcondv * rconde) {
	    tol = rcondv;
	} else {
	    tol = v / rconde;
	}
	if (v > *rcdvin * *rcdein) {
	    tolin = *rcdvin;
	} else {
	    tolin = v / *rcdein;
	}
/* Computing MAX */
	r__1 = tol, r__2 = smlnum / eps;
	tol = dmax(r__1,r__2);
/* Computing MAX */
	r__1 = tolin, r__2 = smlnum / eps;
	tolin = dmax(r__1,r__2);
	if (eps * (*rcdvin - tolin) > rcondv + tol) {
	    result[17] = ulpinv;
	} else if (*rcdvin - tolin > rcondv + tol) {
	    result[17] = (*rcdvin - tolin) / (rcondv + tol);
	} else if (*rcdvin + tolin < eps * (rcondv - tol)) {
	    result[17] = ulpinv;
	} else if (*rcdvin + tolin < rcondv - tol) {
	    result[17] = (rcondv - tol) / (*rcdvin + tolin);
	} else {
	    result[17] = 1.f;
	}

L300:

	;
    }


    return 0;

/*     End of SGET24 */

} /* sget24_ */
