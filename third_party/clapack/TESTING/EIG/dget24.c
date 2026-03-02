#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer selopt, seldim;
    logical selval[20];
    doublereal selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

/* Table of constant values */

static integer c__1 = 1;
static integer c__4 = 4;
static doublereal c_b35 = 1.;
static doublereal c_b41 = 0.;
static doublereal c_b44 = -1.;

/* Subroutine */ int dget24_(logical *comp, integer *jtype, doublereal *
	thresh, integer *iseed, integer *nounit, integer *n, doublereal *a, 
	integer *lda, doublereal *h__, doublereal *ht, doublereal *wr, 
	doublereal *wi, doublereal *wrt, doublereal *wit, doublereal *wrtmp, 
	doublereal *witmp, doublereal *vs, integer *ldvs, doublereal *vs1, 
	doublereal *rcdein, doublereal *rcdvin, integer *nslct, integer *
	islct, doublereal *result, doublereal *work, integer *lwork, integer *
	iwork, logical *bwork, integer *info)
{
    /* Format strings */
    static char fmt_9998[] = "(\002 DGET24: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(\002 DGET24: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, INPUT EXAMPLE NUMBER = \002,"
	    "i4)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, ht_dim1, ht_offset, vs_dim1, 
	    vs_offset, vs1_dim1, vs1_offset, i__1, i__2;
    doublereal d__1, d__2, d__3, d__4;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double d_sign(doublereal *, doublereal *), sqrt(doublereal);

    /* Local variables */
    integer i__, j;
    doublereal v, eps, tol, tmp, ulp;
    integer sdim, kmin, itmp, ipnt[20], rsub;
    char sort[1];
    integer sdim1;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    integer iinfo;
    extern /* Subroutine */ int dort01_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, doublereal *);
    doublereal anorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    doublereal vimin, tolin, vrmin;
    integer isort;
    doublereal wnorm, rcnde1, rcndv1;
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    doublereal rconde;
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *);
    extern logical dslect_(doublereal *, doublereal *);
    extern /* Subroutine */ int dgeesx_(char *, char *, L_fp, char *, integer 
	    *, doublereal *, integer *, integer *, doublereal *, doublereal *, 
	     doublereal *, integer *, doublereal *, doublereal *, doublereal *
, integer *, integer *, integer *, logical *, integer *), xerbla_(char *, integer *);
    integer knteig;
    doublereal rcondv;
    integer liwork;
    doublereal smlnum, ulpinv;

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

/*     DGET24 checks the nonsymmetric eigenvalue (Schur form) problem */
/*     expert driver DGEESX. */

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
/*        computed by DGEESX and RCDEIN (the precomputed true value) */
/*        is supplied as input.  cond(RCONDE) is the condition number */
/*        of RCONDE, and takes errors in computing RCONDE into account, */
/*        so that the resulting quantity should be O(ULP). cond(RCONDE) */
/*        is essentially given by norm(A)/RCONDV. */

/*     (17)  |RCONDV - RCDVIN| / cond(RCONDV) */

/*        RCONDV is the reciprocal right invariant subspace condition */
/*        number computed by DGEESX and RCDVIN (the precomputed true */
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

/*  N       (input) INTEGER */
/*          The dimension of A. N must be at least 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least N. */

/*  H       (workspace) DOUBLE PRECISION array, dimension (LDA, N) */
/*          Another copy of the test matrix A, modified by DGEESX. */

/*  HT      (workspace) DOUBLE PRECISION array, dimension (LDA, N) */
/*          Yet another copy of the test matrix A, modified by DGEESX. */

/*  WR      (workspace) DOUBLE PRECISION array, dimension (N) */
/*  WI      (workspace) DOUBLE PRECISION array, dimension (N) */
/*          The real and imaginary parts of the eigenvalues of A. */
/*          On exit, WR + WI*i are the eigenvalues of the matrix in A. */

/*  WRT     (workspace) DOUBLE PRECISION array, dimension (N) */
/*  WIT     (workspace) DOUBLE PRECISION array, dimension (N) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but those computed when DGEESX only computes a partial */
/*          eigendecomposition, i.e. not Schur vectors */

/*  WRTMP   (workspace) DOUBLE PRECISION array, dimension (N) */
/*  WITMP   (workspace) DOUBLE PRECISION array, dimension (N) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but sorted by increasing real part. */

/*  VS      (workspace) DOUBLE PRECISION array, dimension (LDVS, N) */
/*          VS holds the computed Schur vectors. */

/*  LDVS    (input) INTEGER */
/*          Leading dimension of VS. Must be at least max(1, N). */

/*  VS1     (workspace) DOUBLE PRECISION array, dimension (LDVS, N) */
/*          VS1 holds another copy of the computed Schur vectors. */

/*  RCDEIN  (input) DOUBLE PRECISION */
/*          When COMP = .TRUE. RCDEIN holds the precomputed reciprocal */
/*          condition number for the average of selected eigenvalues. */

/*  RCDVIN  (input) DOUBLE PRECISION */
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

/*  RESULT  (output) DOUBLE PRECISION array, dimension (17) */
/*          The values computed by the 17 tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK to be passed to DGEESX. This */
/*          must be at least 3*N, and N+N**2 if tests 14--16 are to */
/*          be performed. */

/*  IWORK   (workspace) INTEGER array, dimension (N*N) */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          If 0,  successful exit. */
/*          If <0, input parameter -INFO had an incorrect value. */
/*          If >0, DGEESX returned an error code, the absolute */
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
    if (*thresh < 0.) {
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
	xerbla_("DGET24", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

    for (i__ = 1; i__ <= 17; ++i__) {
	result[i__] = -1.;
/* L10: */
    }

    if (*n == 0) {
	return 0;
    }

/*     Important constants */

    smlnum = dlamch_("Safe minimum");
    ulp = dlamch_("Precision");
    ulpinv = 1. / ulp;

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

	dlacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
	dgeesx_("V", sort, (L_fp)dslect_, "N", n, &h__[h_offset], lda, &sdim, 
		&wr[1], &wi[1], &vs[vs_offset], ldvs, &rconde, &rcondv, &work[
		1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[rsub + 1] = ulpinv;
	    if (*jtype != 22) {
		io___13.ciunit = *nounit;
		s_wsfe(&io___13);
		do_fio(&c__1, "DGEESX1", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___14.ciunit = *nounit;
		s_wsfe(&io___14);
		do_fio(&c__1, "DGEESX1", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    return 0;
	}
	if (isort == 0) {
	    dcopy_(n, &wr[1], &c__1, &wrtmp[1], &c__1);
	    dcopy_(n, &wi[1], &c__1, &witmp[1], &c__1);
	}

/*        Do Test (1) or Test (7) */

	result[rsub + 1] = 0.;
	i__1 = *n - 2;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j + 2; i__ <= i__2; ++i__) {
		if (h__[i__ + j * h_dim1] != 0.) {
		    result[rsub + 1] = ulpinv;
		}
/* L20: */
	    }
/* L30: */
	}
	i__1 = *n - 2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + 1 + i__ * h_dim1] != 0. && h__[i__ + 2 + (i__ + 1) *
		     h_dim1] != 0.) {
		result[rsub + 1] = ulpinv;
	    }
/* L40: */
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + 1 + i__ * h_dim1] != 0.) {
		if (h__[i__ + i__ * h_dim1] != h__[i__ + 1 + (i__ + 1) * 
			h_dim1] || h__[i__ + (i__ + 1) * h_dim1] == 0. || 
			d_sign(&c_b35, &h__[i__ + 1 + i__ * h_dim1]) == 
			d_sign(&c_b35, &h__[i__ + (i__ + 1) * h_dim1])) {
		    result[rsub + 1] = ulpinv;
		}
	    }
/* L50: */
	}

/*        Test (2) or (8): Compute norm(A - Q*H*Q') / (norm(A) * N * ULP) */

/*        Copy A to VS1, used as workspace */

	dlacpy_(" ", n, n, &a[a_offset], lda, &vs1[vs1_offset], ldvs);

/*        Compute Q*H and store in HT. */

	dgemm_("No transpose", "No transpose", n, n, n, &c_b35, &vs[vs_offset]
, ldvs, &h__[h_offset], lda, &c_b41, &ht[ht_offset], lda);

/*        Compute A - Q*H*Q' */

	dgemm_("No transpose", "Transpose", n, n, n, &c_b44, &ht[ht_offset], 
		lda, &vs[vs_offset], ldvs, &c_b35, &vs1[vs1_offset], ldvs);

/* Computing MAX */
	d__1 = dlange_("1", n, n, &a[a_offset], lda, &work[1]);
	anorm = max(d__1,smlnum);
	wnorm = dlange_("1", n, n, &vs1[vs1_offset], ldvs, &work[1]);

	if (anorm > wnorm) {
	    result[rsub + 2] = wnorm / anorm / (*n * ulp);
	} else {
	    if (anorm < 1.) {
/* Computing MIN */
		d__1 = wnorm, d__2 = *n * anorm;
		result[rsub + 2] = min(d__1,d__2) / anorm / (*n * ulp);
	    } else {
/* Computing MIN */
		d__1 = wnorm / anorm, d__2 = (doublereal) (*n);
		result[rsub + 2] = min(d__1,d__2) / (*n * ulp);
	    }
	}

/*        Test (3) or (9):  Compute norm( I - Q'*Q ) / ( N * ULP ) */

	dort01_("Columns", n, n, &vs[vs_offset], ldvs, &work[1], lwork, &
		result[rsub + 3]);

/*        Do Test (4) or Test (10) */

	result[rsub + 4] = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + i__ * h_dim1] != wr[i__]) {
		result[rsub + 4] = ulpinv;
	    }
/* L60: */
	}
	if (*n > 1) {
	    if (h__[h_dim1 + 2] == 0. && wi[1] != 0.) {
		result[rsub + 4] = ulpinv;
	    }
	    if (h__[*n + (*n - 1) * h_dim1] == 0. && wi[*n] != 0.) {
		result[rsub + 4] = ulpinv;
	    }
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (h__[i__ + 1 + i__ * h_dim1] != 0.) {
		tmp = sqrt((d__1 = h__[i__ + 1 + i__ * h_dim1], abs(d__1))) * 
			sqrt((d__2 = h__[i__ + (i__ + 1) * h_dim1], abs(d__2))
			);
/* Computing MAX */
/* Computing MAX */
		d__4 = ulp * tmp;
		d__2 = result[rsub + 4], d__3 = (d__1 = wi[i__] - tmp, abs(
			d__1)) / max(d__4,smlnum);
		result[rsub + 4] = max(d__2,d__3);
/* Computing MAX */
/* Computing MAX */
		d__4 = ulp * tmp;
		d__2 = result[rsub + 4], d__3 = (d__1 = wi[i__ + 1] + tmp, 
			abs(d__1)) / max(d__4,smlnum);
		result[rsub + 4] = max(d__2,d__3);
	    } else if (i__ > 1) {
		if (h__[i__ + 1 + i__ * h_dim1] == 0. && h__[i__ + (i__ - 1) *
			 h_dim1] == 0. && wi[i__] != 0.) {
		    result[rsub + 4] = ulpinv;
		}
	    }
/* L70: */
	}

/*        Do Test (5) or Test (11) */

	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("N", sort, (L_fp)dslect_, "N", n, &ht[ht_offset], lda, &sdim, 
		&wrt[1], &wit[1], &vs[vs_offset], ldvs, &rconde, &rcondv, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[rsub + 5] = ulpinv;
	    if (*jtype != 22) {
		io___19.ciunit = *nounit;
		s_wsfe(&io___19);
		do_fio(&c__1, "DGEESX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___20.ciunit = *nounit;
		s_wsfe(&io___20);
		do_fio(&c__1, "DGEESX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L250;
	}

	result[rsub + 5] = 0.;
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

	result[rsub + 6] = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wr[i__] != wrt[i__] || wi[i__] != wit[i__]) {
		result[rsub + 6] = ulpinv;
	    }
/* L100: */
	}

/*        Do Test (13) */

	if (isort == 1) {
	    result[13] = 0.;
	    knteig = 0;
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		d__1 = -wi[i__];
		if (dslect_(&wr[i__], &wi[i__]) || dslect_(&wr[i__], &d__1)) {
		    ++knteig;
		}
		if (i__ < *n) {
		    d__1 = -wi[i__ + 1];
		    d__2 = -wi[i__];
		    if ((dslect_(&wr[i__ + 1], &wi[i__ + 1]) || dslect_(&wr[
			    i__ + 1], &d__1)) && ! (dslect_(&wr[i__], &wi[i__]
) || dslect_(&wr[i__], &d__2)) && iinfo != *n + 2)
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
	result[14] = 0.;
	result[15] = 0.;
	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("V", sort, (L_fp)dslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rconde, &rcondv, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___23.ciunit = *nounit;
		s_wsfe(&io___23);
		do_fio(&c__1, "DGEESX3", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___24.ciunit = *nounit;
		s_wsfe(&io___24);
		do_fio(&c__1, "DGEESX3", (ftnlen)7);
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

	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("N", sort, (L_fp)dslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___27.ciunit = *nounit;
		s_wsfe(&io___27);
		do_fio(&c__1, "DGEESX4", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___28.ciunit = *nounit;
		s_wsfe(&io___28);
		do_fio(&c__1, "DGEESX4", (ftnlen)7);
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

	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("V", sort, (L_fp)dslect_, "E", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    if (*jtype != 22) {
		io___29.ciunit = *nounit;
		s_wsfe(&io___29);
		do_fio(&c__1, "DGEESX5", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___30.ciunit = *nounit;
		s_wsfe(&io___30);
		do_fio(&c__1, "DGEESX5", (ftnlen)7);
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

	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("N", sort, (L_fp)dslect_, "E", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[14] = ulpinv;
	    if (*jtype != 22) {
		io___31.ciunit = *nounit;
		s_wsfe(&io___31);
		do_fio(&c__1, "DGEESX6", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___32.ciunit = *nounit;
		s_wsfe(&io___32);
		do_fio(&c__1, "DGEESX6", (ftnlen)7);
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

	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("V", sort, (L_fp)dslect_, "V", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___33.ciunit = *nounit;
		s_wsfe(&io___33);
		do_fio(&c__1, "DGEESX7", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___34.ciunit = *nounit;
		s_wsfe(&io___34);
		do_fio(&c__1, "DGEESX7", (ftnlen)7);
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

	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("N", sort, (L_fp)dslect_, "V", n, &ht[ht_offset], lda, &sdim1, 
		 &wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___35.ciunit = *nounit;
		s_wsfe(&io___35);
		do_fio(&c__1, "DGEESX8", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___36.ciunit = *nounit;
		s_wsfe(&io___36);
		do_fio(&c__1, "DGEESX8", (ftnlen)7);
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
/*        the logical function DSLECT selects the eigenvalues specified */
/*        by NSLCT and ISLCT. */

	sslct_1.seldim = *n;
	sslct_1.selopt = 1;
	eps = max(ulp,5.9605e-8);
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

	dlacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	dgeesx_("N", "S", (L_fp)dslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		&wrt[1], &wit[1], &vs1[vs1_offset], ldvs, &rconde, &rcondv, &
		work[1], lwork, &iwork[1], &liwork, &bwork[1], &iinfo);
	if (iinfo != 0 && iinfo != *n + 2) {
	    result[16] = ulpinv;
	    result[17] = ulpinv;
	    io___43.ciunit = *nounit;
	    s_wsfe(&io___43);
	    do_fio(&c__1, "DGEESX9", (ftnlen)7);
	    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *info = abs(iinfo);
	    goto L300;
	}

/*        Compare condition number for average of selected eigenvalues */
/*        taking its condition number into account */

	anorm = dlange_("1", n, n, &a[a_offset], lda, &work[1]);
/* Computing MAX */
	d__1 = (doublereal) (*n) * eps * anorm;
	v = max(d__1,smlnum);
	if (anorm == 0.) {
	    v = 1.;
	}
	if (v > rcondv) {
	    tol = 1.;
	} else {
	    tol = v / rcondv;
	}
	if (v > *rcdvin) {
	    tolin = 1.;
	} else {
	    tolin = v / *rcdvin;
	}
/* Computing MAX */
	d__1 = tol, d__2 = smlnum / eps;
	tol = max(d__1,d__2);
/* Computing MAX */
	d__1 = tolin, d__2 = smlnum / eps;
	tolin = max(d__1,d__2);
	if (eps * (*rcdein - tolin) > rconde + tol) {
	    result[16] = ulpinv;
	} else if (*rcdein - tolin > rconde + tol) {
	    result[16] = (*rcdein - tolin) / (rconde + tol);
	} else if (*rcdein + tolin < eps * (rconde - tol)) {
	    result[16] = ulpinv;
	} else if (*rcdein + tolin < rconde - tol) {
	    result[16] = (rconde - tol) / (*rcdein + tolin);
	} else {
	    result[16] = 1.;
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
	d__1 = tol, d__2 = smlnum / eps;
	tol = max(d__1,d__2);
/* Computing MAX */
	d__1 = tolin, d__2 = smlnum / eps;
	tolin = max(d__1,d__2);
	if (eps * (*rcdvin - tolin) > rcondv + tol) {
	    result[17] = ulpinv;
	} else if (*rcdvin - tolin > rcondv + tol) {
	    result[17] = (*rcdvin - tolin) / (rcondv + tol);
	} else if (*rcdvin + tolin < eps * (rcondv - tol)) {
	    result[17] = ulpinv;
	} else if (*rcdvin + tolin < rcondv - tol) {
	    result[17] = (rcondv - tol) / (*rcdvin + tolin);
	} else {
	    result[17] = 1.;
	}

L300:

	;
    }


    return 0;

/*     End of DGET24 */

} /* dget24_ */
