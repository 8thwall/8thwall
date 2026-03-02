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

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__1 = 1;
static integer c__4 = 4;

/* Subroutine */ int cget24_(logical *comp, integer *jtype, real *thresh, 
	integer *iseed, integer *nounit, integer *n, complex *a, integer *lda, 
	 complex *h__, complex *ht, complex *w, complex *wt, complex *wtmp, 
	complex *vs, integer *ldvs, complex *vs1, real *rcdein, real *rcdvin, 
	integer *nslct, integer *islct, integer *isrt, real *result, complex *
	work, integer *lwork, real *rwork, logical *bwork, integer *info)
{
    /* Format strings */
    static char fmt_9998[] = "(\002 CGET24: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, ISEED="
	    "(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(\002 CGET24: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, INPUT EXAMPLE NUMBER = \002,"
	    "i4)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, ht_dim1, ht_offset, vs_dim1, 
	    vs_offset, vs1_dim1, vs1_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2;
    complex q__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double r_imag(complex *);

    /* Local variables */
    integer i__, j;
    real v, eps, tol, ulp;
    integer sdim, kmin;
    complex ctmp;
    integer itmp, ipnt[20], rsub;
    char sort[1];
    integer sdim1;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    integer iinfo;
    extern /* Subroutine */ int cunt01_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *, real *, real *);
    real anorm;
    extern /* Subroutine */ int ccopy_(integer *, complex *, integer *, 
	    complex *, integer *);
    real tolin;
    integer isort;
    real wnorm, rcnde1, rcndv1;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    real rconde;
    extern /* Subroutine */ int clacpy_(char *, integer *, integer *, complex 
	    *, integer *, complex *, integer *);
    extern logical cslect_(complex *);
    extern /* Subroutine */ int cgeesx_(char *, char *, L_fp, char *, integer 
	    *, complex *, integer *, integer *, complex *, complex *, integer 
	    *, real *, real *, complex *, integer *, real *, logical *, 
	    integer *), xerbla_(char *, integer *);
    integer knteig;
    real rcondv, vricmp, vrimin, smlnum, ulpinv;

    /* Fortran I/O blocks */
    static cilist io___12 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___13 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___21 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     CGET24 checks the nonsymmetric eigenvalue (Schur form) problem */
/*     expert driver CGEESX. */

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

/*     (4)     0     if W are eigenvalues of T */
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

/*     (10)    0     if W are eigenvalues of T */
/*             1/ulp otherwise */
/*             If workspace sufficient, also compare W with and */
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
/*        computed by CGEESX and RCDEIN (the precomputed true value) */
/*        is supplied as input.  cond(RCONDE) is the condition number */
/*        of RCONDE, and takes errors in computing RCONDE into account, */
/*        so that the resulting quantity should be O(ULP). cond(RCONDE) */
/*        is essentially given by norm(A)/RCONDV. */

/*     (17)  |RCONDV - RCDVIN| / cond(RCONDV) */

/*        RCONDV is the reciprocal right invariant subspace condition */
/*        number computed by CGEESX and RCDVIN (the precomputed true */
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

/*  A       (input/output) COMPLEX array, dimension (LDA, N) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least N. */

/*  H       (workspace) COMPLEX array, dimension (LDA, N) */
/*          Another copy of the test matrix A, modified by CGEESX. */

/*  HT      (workspace) COMPLEX array, dimension (LDA, N) */
/*          Yet another copy of the test matrix A, modified by CGEESX. */

/*  W       (workspace) COMPLEX array, dimension (N) */
/*          The computed eigenvalues of A. */

/*  WT      (workspace) COMPLEX array, dimension (N) */
/*          Like W, this array contains the eigenvalues of A, */
/*          but those computed when CGEESX only computes a partial */
/*          eigendecomposition, i.e. not Schur vectors */

/*  WTMP    (workspace) COMPLEX array, dimension (N) */
/*          Like W, this array contains the eigenvalues of A, */
/*          but sorted by increasing real or imaginary part. */

/*  VS      (workspace) COMPLEX array, dimension (LDVS, N) */
/*          VS holds the computed Schur vectors. */

/*  LDVS    (input) INTEGER */
/*          Leading dimension of VS. Must be at least max(1, N). */

/*  VS1     (workspace) COMPLEX array, dimension (LDVS, N) */
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
/*          eigenvalue with the J-th largest real or imaginary part is */
/*          selected. The real part is used if ISRT = 0, and the */
/*          imaginary part if ISRT = 1. */
/*          Not referenced if COMP = .FALSE. */

/*  ISRT    (input) INTEGER */
/*          When COMP = .TRUE., ISRT describes how ISLCT is used to */
/*          choose a subset of the spectrum. */
/*          Not referenced if COMP = .FALSE. */

/*  RESULT  (output) REAL array, dimension (17) */
/*          The values computed by the 17 tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  WORK    (workspace) COMPLEX array, dimension (2*N*N) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK to be passed to CGEESX. This */
/*          must be at least 2*N, and N*(N+1)/2 if tests 14--16 are to */
/*          be performed. */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          If 0,  successful exit. */
/*          If <0, input parameter -INFO had an incorrect value. */
/*          If >0, CGEESX returned an error code, the absolute */
/*                 value of which is returned. */

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
/*     .. Arrays in Common .. */
/*     .. */
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
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
    --w;
    --wt;
    --wtmp;
    vs1_dim1 = *ldvs;
    vs1_offset = 1 + vs1_dim1;
    vs1 -= vs1_offset;
    vs_dim1 = *ldvs;
    vs_offset = 1 + vs_dim1;
    vs -= vs_offset;
    --islct;
    --result;
    --work;
    --rwork;
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
	*info = -15;
    } else if (*lwork < *n << 1) {
	*info = -24;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CGET24", &i__1);
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
    for (isort = 0; isort <= 1; ++isort) {
	if (isort == 0) {
	    *(unsigned char *)sort = 'N';
	    rsub = 0;
	} else {
	    *(unsigned char *)sort = 'S';
	    rsub = 6;
	}

/*        Compute Schur form and Schur vectors, and test them */

	clacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
	cgeesx_("V", sort, (L_fp)cslect_, "N", n, &h__[h_offset], lda, &sdim, 
		&w[1], &vs[vs_offset], ldvs, &rconde, &rcondv, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[rsub + 1] = ulpinv;
	    if (*jtype != 22) {
		io___12.ciunit = *nounit;
		s_wsfe(&io___12);
		do_fio(&c__1, "CGEESX1", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___13.ciunit = *nounit;
		s_wsfe(&io___13);
		do_fio(&c__1, "CGEESX1", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    return 0;
	}
	if (isort == 0) {
	    ccopy_(n, &w[1], &c__1, &wtmp[1], &c__1);
	}

/*        Do Test (1) or Test (7) */

	result[rsub + 1] = 0.f;
	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * h_dim1;
		if (h__[i__3].r != 0.f || h__[i__3].i != 0.f) {
		    result[rsub + 1] = ulpinv;
		}
/* L20: */
	    }
/* L30: */
	}

/*        Test (2) or (8): Compute norm(A - Q*H*Q') / (norm(A) * N * ULP) */

/*        Copy A to VS1, used as workspace */

	clacpy_(" ", n, n, &a[a_offset], lda, &vs1[vs1_offset], ldvs);

/*        Compute Q*H and store in HT. */

	cgemm_("No transpose", "No transpose", n, n, n, &c_b2, &vs[vs_offset], 
		 ldvs, &h__[h_offset], lda, &c_b1, &ht[ht_offset], lda);

/*        Compute A - Q*H*Q' */

	q__1.r = -1.f, q__1.i = -0.f;
	cgemm_("No transpose", "Conjugate transpose", n, n, n, &q__1, &ht[
		ht_offset], lda, &vs[vs_offset], ldvs, &c_b2, &vs1[vs1_offset]
, ldvs);

/* Computing MAX */
	r__1 = clange_("1", n, n, &a[a_offset], lda, &rwork[1]);
	anorm = dmax(r__1,smlnum);
	wnorm = clange_("1", n, n, &vs1[vs1_offset], ldvs, &rwork[1]);

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

	cunt01_("Columns", n, n, &vs[vs_offset], ldvs, &work[1], lwork, &
		rwork[1], &result[rsub + 3]);

/*        Do Test (4) or Test (10) */

	result[rsub + 4] = 0.f;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__ + i__ * h_dim1;
	    i__3 = i__;
	    if (h__[i__2].r != w[i__3].r || h__[i__2].i != w[i__3].i) {
		result[rsub + 4] = ulpinv;
	    }
/* L40: */
	}

/*        Do Test (5) or Test (11) */

	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("N", sort, (L_fp)cslect_, "N", n, &ht[ht_offset], lda, &sdim, 
		&wt[1], &vs[vs_offset], ldvs, &rconde, &rcondv, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[rsub + 5] = ulpinv;
	    if (*jtype != 22) {
		io___17.ciunit = *nounit;
		s_wsfe(&io___17);
		do_fio(&c__1, "CGEESX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___18.ciunit = *nounit;
		s_wsfe(&io___18);
		do_fio(&c__1, "CGEESX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L220;
	}

	result[rsub + 5] = 0.f;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		i__3 = i__ + j * h_dim1;
		i__4 = i__ + j * ht_dim1;
		if (h__[i__3].r != ht[i__4].r || h__[i__3].i != ht[i__4].i) {
		    result[rsub + 5] = ulpinv;
		}
/* L50: */
	    }
/* L60: */
	}

/*        Do Test (6) or Test (12) */

	result[rsub + 6] = 0.f;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    i__3 = i__;
	    if (w[i__2].r != wt[i__3].r || w[i__2].i != wt[i__3].i) {
		result[rsub + 6] = ulpinv;
	    }
/* L70: */
	}

/*        Do Test (13) */

	if (isort == 1) {
	    result[13] = 0.f;
	    knteig = 0;
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (cslect_(&w[i__])) {
		    ++knteig;
		}
		if (i__ < *n) {
		    if (cslect_(&w[i__ + 1]) && ! cslect_(&w[i__])) {
			result[13] = ulpinv;
		    }
		}
/* L80: */
	    }
	    if (sdim != knteig) {
		result[13] = ulpinv;
	    }
	}

/* L90: */
    }

/*     If there is enough workspace, perform tests (14) and (15) */
/*     as well as (10) through (13) */

    if (*lwork >= *n * (*n + 1) / 2) {

/*        Compute both RCONDE and RCONDV with VS */

	*(unsigned char *)sort = 'S';
	result[14] = 0.f;
	result[15] = 0.f;
	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("V", sort, (L_fp)cslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		 &wt[1], &vs1[vs1_offset], ldvs, &rconde, &rcondv, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[14] = ulpinv;
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___21.ciunit = *nounit;
		s_wsfe(&io___21);
		do_fio(&c__1, "CGEESX3", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___22.ciunit = *nounit;
		s_wsfe(&io___22);
		do_fio(&c__1, "CGEESX3", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L220;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    i__3 = i__;
	    if (w[i__2].r != wt[i__3].r || w[i__2].i != wt[i__3].i) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * h_dim1;
		i__4 = i__ + j * ht_dim1;
		if (h__[i__3].r != ht[i__4].r || h__[i__3].i != ht[i__4].i) {
		    result[11] = ulpinv;
		}
		i__3 = i__ + j * vs_dim1;
		i__4 = i__ + j * vs1_dim1;
		if (vs[i__3].r != vs1[i__4].r || vs[i__3].i != vs1[i__4].i) {
		    result[12] = ulpinv;
		}
/* L100: */
	    }
/* L110: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute both RCONDE and RCONDV without VS, and compare */

	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("N", sort, (L_fp)cslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		 &wt[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[14] = ulpinv;
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___25.ciunit = *nounit;
		s_wsfe(&io___25);
		do_fio(&c__1, "CGEESX4", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___26.ciunit = *nounit;
		s_wsfe(&io___26);
		do_fio(&c__1, "CGEESX4", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L220;
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
	    i__2 = i__;
	    i__3 = i__;
	    if (w[i__2].r != wt[i__3].r || w[i__2].i != wt[i__3].i) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * h_dim1;
		i__4 = i__ + j * ht_dim1;
		if (h__[i__3].r != ht[i__4].r || h__[i__3].i != ht[i__4].i) {
		    result[11] = ulpinv;
		}
		i__3 = i__ + j * vs_dim1;
		i__4 = i__ + j * vs1_dim1;
		if (vs[i__3].r != vs1[i__4].r || vs[i__3].i != vs1[i__4].i) {
		    result[12] = ulpinv;
		}
/* L120: */
	    }
/* L130: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDE with VS, and compare */

	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("V", sort, (L_fp)cslect_, "E", n, &ht[ht_offset], lda, &sdim1, 
		 &wt[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[14] = ulpinv;
	    if (*jtype != 22) {
		io___27.ciunit = *nounit;
		s_wsfe(&io___27);
		do_fio(&c__1, "CGEESX5", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___28.ciunit = *nounit;
		s_wsfe(&io___28);
		do_fio(&c__1, "CGEESX5", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L220;
	}

/*        Perform test (14) */

	if (rcnde1 != rconde) {
	    result[14] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    i__3 = i__;
	    if (w[i__2].r != wt[i__3].r || w[i__2].i != wt[i__3].i) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * h_dim1;
		i__4 = i__ + j * ht_dim1;
		if (h__[i__3].r != ht[i__4].r || h__[i__3].i != ht[i__4].i) {
		    result[11] = ulpinv;
		}
		i__3 = i__ + j * vs_dim1;
		i__4 = i__ + j * vs1_dim1;
		if (vs[i__3].r != vs1[i__4].r || vs[i__3].i != vs1[i__4].i) {
		    result[12] = ulpinv;
		}
/* L140: */
	    }
/* L150: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDE without VS, and compare */

	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("N", sort, (L_fp)cslect_, "E", n, &ht[ht_offset], lda, &sdim1, 
		 &wt[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[14] = ulpinv;
	    if (*jtype != 22) {
		io___29.ciunit = *nounit;
		s_wsfe(&io___29);
		do_fio(&c__1, "CGEESX6", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___30.ciunit = *nounit;
		s_wsfe(&io___30);
		do_fio(&c__1, "CGEESX6", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L220;
	}

/*        Perform test (14) */

	if (rcnde1 != rconde) {
	    result[14] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    i__3 = i__;
	    if (w[i__2].r != wt[i__3].r || w[i__2].i != wt[i__3].i) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * h_dim1;
		i__4 = i__ + j * ht_dim1;
		if (h__[i__3].r != ht[i__4].r || h__[i__3].i != ht[i__4].i) {
		    result[11] = ulpinv;
		}
		i__3 = i__ + j * vs_dim1;
		i__4 = i__ + j * vs1_dim1;
		if (vs[i__3].r != vs1[i__4].r || vs[i__3].i != vs1[i__4].i) {
		    result[12] = ulpinv;
		}
/* L160: */
	    }
/* L170: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDV with VS, and compare */

	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("V", sort, (L_fp)cslect_, "V", n, &ht[ht_offset], lda, &sdim1, 
		 &wt[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___31.ciunit = *nounit;
		s_wsfe(&io___31);
		do_fio(&c__1, "CGEESX7", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___32.ciunit = *nounit;
		s_wsfe(&io___32);
		do_fio(&c__1, "CGEESX7", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L220;
	}

/*        Perform test (15) */

	if (rcndv1 != rcondv) {
	    result[15] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    i__3 = i__;
	    if (w[i__2].r != wt[i__3].r || w[i__2].i != wt[i__3].i) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * h_dim1;
		i__4 = i__ + j * ht_dim1;
		if (h__[i__3].r != ht[i__4].r || h__[i__3].i != ht[i__4].i) {
		    result[11] = ulpinv;
		}
		i__3 = i__ + j * vs_dim1;
		i__4 = i__ + j * vs1_dim1;
		if (vs[i__3].r != vs1[i__4].r || vs[i__3].i != vs1[i__4].i) {
		    result[12] = ulpinv;
		}
/* L180: */
	    }
/* L190: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

/*        Compute RCONDV without VS, and compare */

	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("N", sort, (L_fp)cslect_, "V", n, &ht[ht_offset], lda, &sdim1, 
		 &wt[1], &vs1[vs1_offset], ldvs, &rcnde1, &rcndv1, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[15] = ulpinv;
	    if (*jtype != 22) {
		io___33.ciunit = *nounit;
		s_wsfe(&io___33);
		do_fio(&c__1, "CGEESX8", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___34.ciunit = *nounit;
		s_wsfe(&io___34);
		do_fio(&c__1, "CGEESX8", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L220;
	}

/*        Perform test (15) */

	if (rcndv1 != rcondv) {
	    result[15] = ulpinv;
	}

/*        Perform tests (10), (11), (12), and (13) */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    i__3 = i__;
	    if (w[i__2].r != wt[i__3].r || w[i__2].i != wt[i__3].i) {
		result[10] = ulpinv;
	    }
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		i__3 = i__ + j * h_dim1;
		i__4 = i__ + j * ht_dim1;
		if (h__[i__3].r != ht[i__4].r || h__[i__3].i != ht[i__4].i) {
		    result[11] = ulpinv;
		}
		i__3 = i__ + j * vs_dim1;
		i__4 = i__ + j * vs1_dim1;
		if (vs[i__3].r != vs1[i__4].r || vs[i__3].i != vs1[i__4].i) {
		    result[12] = ulpinv;
		}
/* L200: */
	    }
/* L210: */
	}
	if (sdim != sdim1) {
	    result[13] = ulpinv;
	}

    }

L220:

/*     If there are precomputed reciprocal condition numbers, compare */
/*     computed values with them. */

    if (*comp) {

/*        First set up SELOPT, SELDIM, SELVAL, SELWR and SELWI so that */
/*        the logical function CSLECT selects the eigenvalues specified */
/*        by NSLCT, ISLCT and ISRT. */

	sslct_1.seldim = *n;
	sslct_1.selopt = 1;
	eps = dmax(ulp,5.9605e-8f);
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ipnt[i__ - 1] = i__;
	    sslct_1.selval[i__ - 1] = FALSE_;
	    i__2 = i__;
	    sslct_1.selwr[i__ - 1] = wtmp[i__2].r;
	    sslct_1.selwi[i__ - 1] = r_imag(&wtmp[i__]);
/* L230: */
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    kmin = i__;
	    if (*isrt == 0) {
		i__2 = i__;
		vrimin = wtmp[i__2].r;
	    } else {
		vrimin = r_imag(&wtmp[i__]);
	    }
	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		if (*isrt == 0) {
		    i__3 = j;
		    vricmp = wtmp[i__3].r;
		} else {
		    vricmp = r_imag(&wtmp[j]);
		}
		if (vricmp < vrimin) {
		    kmin = j;
		    vrimin = vricmp;
		}
/* L240: */
	    }
	    i__2 = kmin;
	    ctmp.r = wtmp[i__2].r, ctmp.i = wtmp[i__2].i;
	    i__2 = kmin;
	    i__3 = i__;
	    wtmp[i__2].r = wtmp[i__3].r, wtmp[i__2].i = wtmp[i__3].i;
	    i__2 = i__;
	    wtmp[i__2].r = ctmp.r, wtmp[i__2].i = ctmp.i;
	    itmp = ipnt[i__ - 1];
	    ipnt[i__ - 1] = ipnt[kmin - 1];
	    ipnt[kmin - 1] = itmp;
/* L250: */
	}
	i__1 = *nslct;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    sslct_1.selval[ipnt[islct[i__] - 1] - 1] = TRUE_;
/* L260: */
	}

/*        Compute condition numbers */

	clacpy_("F", n, n, &a[a_offset], lda, &ht[ht_offset], lda);
	cgeesx_("N", "S", (L_fp)cslect_, "B", n, &ht[ht_offset], lda, &sdim1, 
		&wt[1], &vs1[vs1_offset], ldvs, &rconde, &rcondv, &work[1], 
		lwork, &rwork[1], &bwork[1], &iinfo);
	if (iinfo != 0) {
	    result[16] = ulpinv;
	    result[17] = ulpinv;
	    io___42.ciunit = *nounit;
	    s_wsfe(&io___42);
	    do_fio(&c__1, "CGEESX9", (ftnlen)7);
	    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *info = abs(iinfo);
	    goto L270;
	}

/*        Compare condition number for average of selected eigenvalues */
/*        taking its condition number into account */

	anorm = clange_("1", n, n, &a[a_offset], lda, &rwork[1]);
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

L270:

	;
    }


    return 0;

/*     End of CGET24 */

} /* cget24_ */
