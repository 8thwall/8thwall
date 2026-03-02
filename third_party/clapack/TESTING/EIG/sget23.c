#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__4 = 4;

/* Subroutine */ int sget23_(logical *comp, char *balanc, integer *jtype, 
	real *thresh, integer *iseed, integer *nounit, integer *n, real *a, 
	integer *lda, real *h__, real *wr, real *wi, real *wr1, real *wi1, 
	real *vl, integer *ldvl, real *vr, integer *ldvr, real *lre, integer *
	ldlre, real *rcondv, real *rcndv1, real *rcdvin, real *rconde, real *
	rcnde1, real *rcdein, real *scale, real *scale1, real *result, real *
	work, integer *lwork, integer *iwork, integer *info)
{
    /* Initialized data */

    static char sens[1*2] = "N" "V";

    /* Format strings */
    static char fmt_9998[] = "(\002 SGET23: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, JTYPE=\002,i6,\002, BALANC = "
	    "\002,a,\002, ISEED=(\002,3(i5,\002,\002),i5,\002)\002)";
    static char fmt_9999[] = "(\002 SGET23: \002,a,\002 returned INFO=\002,i"
	    "6,\002.\002,/9x,\002N=\002,i6,\002, INPUT EXAMPLE NUMBER = \002,"
	    "i4)";

    /* System generated locals */
    integer a_dim1, a_offset, h_dim1, h_offset, lre_dim1, lre_offset, vl_dim1,
	     vl_offset, vr_dim1, vr_offset, i__1, i__2, i__3;
    real r__1, r__2, r__3, r__4, r__5;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, j;
    real v;
    integer jj, ihi, ilo;
    real dum[1], eps, res[2], tol, ulp, vmx;
    integer ihi1, ilo1, kmin;
    real vmax, tnrm, vrmx, vtst;
    extern doublereal snrm2_(integer *, real *, integer *);
    logical balok, nobal;
    real abnrm;
    extern logical lsame_(char *, char *);
    integer iinfo;
    extern /* Subroutine */ int sget22_(char *, char *, char *, integer *, 
	    real *, integer *, real *, integer *, real *, real *, real *, 
	    real *);
    char sense[1];
    integer isens;
    real vimin, tolin, vrmin, abnrm1;
    extern doublereal slapy2_(real *, real *), slamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, integer *), slacpy_(
	    char *, integer *, integer *, real *, integer *, real *, integer *
);
    integer isensm;
    extern /* Subroutine */ int sgeevx_(char *, char *, char *, char *, 
	    integer *, real *, integer *, real *, real *, real *, integer *, 
	    real *, integer *, integer *, integer *, real *, real *, real *, 
	    real *, real *, integer *, integer *, integer *);
    real smlnum, ulpinv;

    /* Fortran I/O blocks */
    static cilist io___14 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     SGET23  checks the nonsymmetric eigenvalue problem driver SGEEVX. */
/*     If COMP = .FALSE., the first 8 of the following tests will be */
/*     performed on the input matrix A, and also test 9 if LWORK is */
/*     sufficiently large. */
/*     if COMP is .TRUE. all 11 tests will be performed. */

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

/*     (3)     | |VR(i)| - 1 | / ulp and largest component real */

/*       VR(i) denotes the i-th column of VR. */

/*     (4)     | |VL(i)| - 1 | / ulp and largest component real */

/*       VL(i) denotes the i-th column of VL. */

/*     (5)     0 if W(full) = W(partial), 1/ulp otherwise */

/*       W(full) denotes the eigenvalues computed when VR, VL, RCONDV */
/*       and RCONDE are also computed, and W(partial) denotes the */
/*       eigenvalues computed when only some of VR, VL, RCONDV, and */
/*       RCONDE are computed. */

/*     (6)     0 if VR(full) = VR(partial), 1/ulp otherwise */

/*       VR(full) denotes the right eigenvectors computed when VL, RCONDV */
/*       and RCONDE are computed, and VR(partial) denotes the result */
/*       when only some of VL and RCONDV are computed. */

/*     (7)     0 if VL(full) = VL(partial), 1/ulp otherwise */

/*       VL(full) denotes the left eigenvectors computed when VR, RCONDV */
/*       and RCONDE are computed, and VL(partial) denotes the result */
/*       when only some of VR and RCONDV are computed. */

/*     (8)     0 if SCALE, ILO, IHI, ABNRM (full) = */
/*                  SCALE, ILO, IHI, ABNRM (partial) */
/*             1/ulp otherwise */

/*       SCALE, ILO, IHI and ABNRM describe how the matrix is balanced. */
/*       (full) is when VR, VL, RCONDE and RCONDV are also computed, and */
/*       (partial) is when some are not computed. */

/*     (9)     0 if RCONDV(full) = RCONDV(partial), 1/ulp otherwise */

/*       RCONDV(full) denotes the reciprocal condition numbers of the */
/*       right eigenvectors computed when VR, VL and RCONDE are also */
/*       computed. RCONDV(partial) denotes the reciprocal condition */
/*       numbers when only some of VR, VL and RCONDE are computed. */

/*    (10)     |RCONDV - RCDVIN| / cond(RCONDV) */

/*       RCONDV is the reciprocal right eigenvector condition number */
/*       computed by SGEEVX and RCDVIN (the precomputed true value) */
/*       is supplied as input. cond(RCONDV) is the condition number of */
/*       RCONDV, and takes errors in computing RCONDV into account, so */
/*       that the resulting quantity should be O(ULP). cond(RCONDV) is */
/*       essentially given by norm(A)/RCONDE. */

/*    (11)     |RCONDE - RCDEIN| / cond(RCONDE) */

/*       RCONDE is the reciprocal eigenvalue condition number */
/*       computed by SGEEVX and RCDEIN (the precomputed true value) */
/*       is supplied as input.  cond(RCONDE) is the condition number */
/*       of RCONDE, and takes errors in computing RCONDE into account, */
/*       so that the resulting quantity should be O(ULP). cond(RCONDE) */
/*       is essentially given by norm(A)/RCONDV. */

/*  Arguments */
/*  ========= */

/*  COMP    (input) LOGICAL */
/*          COMP describes which input tests to perform: */
/*            = .FALSE. if the computed condition numbers are not to */
/*                      be tested against RCDVIN and RCDEIN */
/*            = .TRUE.  if they are to be compared */

/*  BALANC  (input) CHARACTER */
/*          Describes the balancing option to be tested. */
/*            = 'N' for no permuting or diagonal scaling */
/*            = 'P' for permuting but no diagonal scaling */
/*            = 'S' for no permuting but diagonal scaling */
/*            = 'B' for permuting and diagonal scaling */

/*  JTYPE   (input) INTEGER */
/*          Type of input matrix. Used to label output if error occurs. */

/*  THRESH  (input) REAL */
/*          A test will count as "failed" if the "error", computed as */
/*          described above, exceeds THRESH.  Note that the error */
/*          is scaled to be O(1), so THRESH should be a reasonably */
/*          small multiple of 1, e.g., 10 or 100.  In particular, */
/*          it should not depend on the precision (single vs. double) */
/*          or the size of the matrix.  It must be at least zero. */

/*  ISEED   (input) INTEGER array, dimension (4) */
/*          If COMP = .FALSE., the random number generator seed */
/*          used to produce matrix. */
/*          If COMP = .TRUE., ISEED(1) = the number of the example. */
/*          Used to label output if error occurs. */

/*  NOUNIT  (input) INTEGER */
/*          The FORTRAN unit number for printing out error messages */
/*          (e.g., if a routine returns INFO not equal to 0.) */

/*  N       (input) INTEGER */
/*          The dimension of A. N must be at least 0. */

/*  A       (input/output) REAL array, dimension (LDA,N) */
/*          Used to hold the matrix whose eigenvalues are to be */
/*          computed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A, and H. LDA must be at */
/*          least 1 and at least N. */

/*  H       (workspace) REAL array, dimension (LDA,N) */
/*          Another copy of the test matrix A, modified by SGEEVX. */

/*  WR      (workspace) REAL array, dimension (N) */
/*  WI      (workspace) REAL array, dimension (N) */
/*          The real and imaginary parts of the eigenvalues of A. */
/*          On exit, WR + WI*i are the eigenvalues of the matrix in A. */

/*  WR1     (workspace) REAL array, dimension (N) */
/*  WI1     (workspace) REAL array, dimension (N) */
/*          Like WR, WI, these arrays contain the eigenvalues of A, */
/*          but those computed when SGEEVX only computes a partial */
/*          eigendecomposition, i.e. not the eigenvalues and left */
/*          and right eigenvectors. */

/*  VL      (workspace) REAL array, dimension (LDVL,N) */
/*          VL holds the computed left eigenvectors. */

/*  LDVL    (input) INTEGER */
/*          Leading dimension of VL. Must be at least max(1,N). */

/*  VR      (workspace) REAL array, dimension (LDVR,N) */
/*          VR holds the computed right eigenvectors. */

/*  LDVR    (input) INTEGER */
/*          Leading dimension of VR. Must be at least max(1,N). */

/*  LRE     (workspace) REAL array, dimension (LDLRE,N) */
/*          LRE holds the computed right or left eigenvectors. */

/*  LDLRE   (input) INTEGER */
/*          Leading dimension of LRE. Must be at least max(1,N). */

/*  RCONDV  (workspace) REAL array, dimension (N) */
/*          RCONDV holds the computed reciprocal condition numbers */
/*          for eigenvectors. */

/*  RCNDV1  (workspace) REAL array, dimension (N) */
/*          RCNDV1 holds more computed reciprocal condition numbers */
/*          for eigenvectors. */

/*  RCDVIN  (input) REAL array, dimension (N) */
/*          When COMP = .TRUE. RCDVIN holds the precomputed reciprocal */
/*          condition numbers for eigenvectors to be compared with */
/*          RCONDV. */

/*  RCONDE  (workspace) REAL array, dimension (N) */
/*          RCONDE holds the computed reciprocal condition numbers */
/*          for eigenvalues. */

/*  RCNDE1  (workspace) REAL array, dimension (N) */
/*          RCNDE1 holds more computed reciprocal condition numbers */
/*          for eigenvalues. */

/*  RCDEIN  (input) REAL array, dimension (N) */
/*          When COMP = .TRUE. RCDEIN holds the precomputed reciprocal */
/*          condition numbers for eigenvalues to be compared with */
/*          RCONDE. */

/*  SCALE   (workspace) REAL array, dimension (N) */
/*          Holds information describing balancing of matrix. */

/*  SCALE1  (workspace) REAL array, dimension (N) */
/*          Holds information describing balancing of matrix. */

/*  RESULT  (output) REAL array, dimension (11) */
/*          The values computed by the 11 tests described above. */
/*          The values are currently limited to 1/ulp, to avoid */
/*          overflow. */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The number of entries in WORK.  This must be at least */
/*          3*N, and 6*N+N**2 if tests 9, 10 or 11 are to be performed. */

/*  IWORK   (workspace) INTEGER array, dimension (2*N) */

/*  INFO    (output) INTEGER */
/*          If 0,  successful exit. */
/*          If <0, input parameter -INFO had an incorrect value. */
/*          If >0, SGEEVX returned an error code, the absolute */
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
/*     .. Data statements .. */
    /* Parameter adjustments */
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
    --rcondv;
    --rcndv1;
    --rcdvin;
    --rconde;
    --rcnde1;
    --rcdein;
    --scale;
    --scale1;
    --result;
    --work;
    --iwork;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for errors */

    nobal = lsame_(balanc, "N");
    balok = nobal || lsame_(balanc, "P") || lsame_(
	    balanc, "S") || lsame_(balanc, "B");
    *info = 0;
    if (! balok) {
	*info = -2;
    } else if (*thresh < 0.f) {
	*info = -4;
    } else if (*nounit <= 0) {
	*info = -6;
    } else if (*n < 0) {
	*info = -7;
    } else if (*lda < 1 || *lda < *n) {
	*info = -9;
    } else if (*ldvl < 1 || *ldvl < *n) {
	*info = -16;
    } else if (*ldvr < 1 || *ldvr < *n) {
	*info = -18;
    } else if (*ldlre < 1 || *ldlre < *n) {
	*info = -20;
    } else if (*lwork < *n * 3 || *comp && *lwork < *n * 6 + *n * *n) {
	*info = -31;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SGET23", &i__1);
	return 0;
    }

/*     Quick return if nothing to do */

    for (i__ = 1; i__ <= 11; ++i__) {
	result[i__] = -1.f;
/* L10: */
    }

    if (*n == 0) {
	return 0;
    }

/*     More Important constants */

    ulp = slamch_("Precision");
    smlnum = slamch_("S");
    ulpinv = 1.f / ulp;

/*     Compute eigenvalues and eigenvectors, and test them */

    if (*lwork >= *n * 6 + *n * *n) {
	*(unsigned char *)sense = 'B';
	isensm = 2;
    } else {
	*(unsigned char *)sense = 'E';
	isensm = 1;
    }
    slacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
    sgeevx_(balanc, "V", "V", sense, n, &h__[h_offset], lda, &wr[1], &wi[1], &
	    vl[vl_offset], ldvl, &vr[vr_offset], ldvr, &ilo, &ihi, &scale[1], 
	    &abnrm, &rconde[1], &rcondv[1], &work[1], lwork, &iwork[1], &
	    iinfo);
    if (iinfo != 0) {
	result[1] = ulpinv;
	if (*jtype != 22) {
	    io___14.ciunit = *nounit;
	    s_wsfe(&io___14);
	    do_fio(&c__1, "SGEEVX1", (ftnlen)7);
	    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
	    do_fio(&c__1, balanc, (ftnlen)1);
	    do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {
	    io___15.ciunit = *nounit;
	    s_wsfe(&io___15);
	    do_fio(&c__1, "SGEEVX1", (ftnlen)7);
	    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	*info = abs(iinfo);
	return 0;
    }

/*     Do Test (1) */

    sget22_("N", "N", "N", n, &a[a_offset], lda, &vr[vr_offset], ldvr, &wr[1], 
	     &wi[1], &work[1], res);
    result[1] = res[0];

/*     Do Test (2) */

    sget22_("T", "N", "T", n, &a[a_offset], lda, &vl[vl_offset], ldvl, &wr[1], 
	     &wi[1], &work[1], res);
    result[2] = res[0];

/*     Do Test (3) */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	tnrm = 1.f;
	if (wi[j] == 0.f) {
	    tnrm = snrm2_(n, &vr[j * vr_dim1 + 1], &c__1);
	} else if (wi[j] > 0.f) {
	    r__1 = snrm2_(n, &vr[j * vr_dim1 + 1], &c__1);
	    r__2 = snrm2_(n, &vr[(j + 1) * vr_dim1 + 1], &c__1);
	    tnrm = slapy2_(&r__1, &r__2);
	}
/* Computing MAX */
/* Computing MIN */
	r__4 = ulpinv, r__5 = (r__1 = tnrm - 1.f, dabs(r__1)) / ulp;
	r__2 = result[3], r__3 = dmin(r__4,r__5);
	result[3] = dmax(r__2,r__3);
	if (wi[j] > 0.f) {
	    vmx = 0.f;
	    vrmx = 0.f;
	    i__2 = *n;
	    for (jj = 1; jj <= i__2; ++jj) {
		vtst = slapy2_(&vr[jj + j * vr_dim1], &vr[jj + (j + 1) * 
			vr_dim1]);
		if (vtst > vmx) {
		    vmx = vtst;
		}
		if (vr[jj + (j + 1) * vr_dim1] == 0.f && (r__1 = vr[jj + j * 
			vr_dim1], dabs(r__1)) > vrmx) {
		    vrmx = (r__2 = vr[jj + j * vr_dim1], dabs(r__2));
		}
/* L20: */
	    }
	    if (vrmx / vmx < 1.f - ulp * 2.f) {
		result[3] = ulpinv;
	    }
	}
/* L30: */
    }

/*     Do Test (4) */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	tnrm = 1.f;
	if (wi[j] == 0.f) {
	    tnrm = snrm2_(n, &vl[j * vl_dim1 + 1], &c__1);
	} else if (wi[j] > 0.f) {
	    r__1 = snrm2_(n, &vl[j * vl_dim1 + 1], &c__1);
	    r__2 = snrm2_(n, &vl[(j + 1) * vl_dim1 + 1], &c__1);
	    tnrm = slapy2_(&r__1, &r__2);
	}
/* Computing MAX */
/* Computing MIN */
	r__4 = ulpinv, r__5 = (r__1 = tnrm - 1.f, dabs(r__1)) / ulp;
	r__2 = result[4], r__3 = dmin(r__4,r__5);
	result[4] = dmax(r__2,r__3);
	if (wi[j] > 0.f) {
	    vmx = 0.f;
	    vrmx = 0.f;
	    i__2 = *n;
	    for (jj = 1; jj <= i__2; ++jj) {
		vtst = slapy2_(&vl[jj + j * vl_dim1], &vl[jj + (j + 1) * 
			vl_dim1]);
		if (vtst > vmx) {
		    vmx = vtst;
		}
		if (vl[jj + (j + 1) * vl_dim1] == 0.f && (r__1 = vl[jj + j * 
			vl_dim1], dabs(r__1)) > vrmx) {
		    vrmx = (r__2 = vl[jj + j * vl_dim1], dabs(r__2));
		}
/* L40: */
	    }
	    if (vrmx / vmx < 1.f - ulp * 2.f) {
		result[4] = ulpinv;
	    }
	}
/* L50: */
    }

/*     Test for all options of computing condition numbers */

    i__1 = isensm;
    for (isens = 1; isens <= i__1; ++isens) {

	*(unsigned char *)sense = *(unsigned char *)&sens[isens - 1];

/*        Compute eigenvalues only, and test them */

	slacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
	sgeevx_(balanc, "N", "N", sense, n, &h__[h_offset], lda, &wr1[1], &
		wi1[1], dum, &c__1, dum, &c__1, &ilo1, &ihi1, &scale1[1], &
		abnrm1, &rcnde1[1], &rcndv1[1], &work[1], lwork, &iwork[1], &
		iinfo);
	if (iinfo != 0) {
	    result[1] = ulpinv;
	    if (*jtype != 22) {
		io___28.ciunit = *nounit;
		s_wsfe(&io___28);
		do_fio(&c__1, "SGEEVX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__1, balanc, (ftnlen)1);
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___29.ciunit = *nounit;
		s_wsfe(&io___29);
		do_fio(&c__1, "SGEEVX2", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L190;
	}

/*        Do Test (5) */

	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    if (wr[j] != wr1[j] || wi[j] != wi1[j]) {
		result[5] = ulpinv;
	    }
/* L60: */
	}

/*        Do Test (8) */

	if (! nobal) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (scale[j] != scale1[j]) {
		    result[8] = ulpinv;
		}
/* L70: */
	    }
	    if (ilo != ilo1) {
		result[8] = ulpinv;
	    }
	    if (ihi != ihi1) {
		result[8] = ulpinv;
	    }
	    if (abnrm != abnrm1) {
		result[8] = ulpinv;
	    }
	}

/*        Do Test (9) */

	if (isens == 2 && *n > 1) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (rcondv[j] != rcndv1[j]) {
		    result[9] = ulpinv;
		}
/* L80: */
	    }
	}

/*        Compute eigenvalues and right eigenvectors, and test them */

	slacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
	sgeevx_(balanc, "N", "V", sense, n, &h__[h_offset], lda, &wr1[1], &
		wi1[1], dum, &c__1, &lre[lre_offset], ldlre, &ilo1, &ihi1, &
		scale1[1], &abnrm1, &rcnde1[1], &rcndv1[1], &work[1], lwork, &
		iwork[1], &iinfo);
	if (iinfo != 0) {
	    result[1] = ulpinv;
	    if (*jtype != 22) {
		io___30.ciunit = *nounit;
		s_wsfe(&io___30);
		do_fio(&c__1, "SGEEVX3", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__1, balanc, (ftnlen)1);
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___31.ciunit = *nounit;
		s_wsfe(&io___31);
		do_fio(&c__1, "SGEEVX3", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L190;
	}

/*        Do Test (5) again */

	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    if (wr[j] != wr1[j] || wi[j] != wi1[j]) {
		result[5] = ulpinv;
	    }
/* L90: */
	}

/*        Do Test (6) */

	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = *n;
	    for (jj = 1; jj <= i__3; ++jj) {
		if (vr[j + jj * vr_dim1] != lre[j + jj * lre_dim1]) {
		    result[6] = ulpinv;
		}
/* L100: */
	    }
/* L110: */
	}

/*        Do Test (8) again */

	if (! nobal) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (scale[j] != scale1[j]) {
		    result[8] = ulpinv;
		}
/* L120: */
	    }
	    if (ilo != ilo1) {
		result[8] = ulpinv;
	    }
	    if (ihi != ihi1) {
		result[8] = ulpinv;
	    }
	    if (abnrm != abnrm1) {
		result[8] = ulpinv;
	    }
	}

/*        Do Test (9) again */

	if (isens == 2 && *n > 1) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (rcondv[j] != rcndv1[j]) {
		    result[9] = ulpinv;
		}
/* L130: */
	    }
	}

/*        Compute eigenvalues and left eigenvectors, and test them */

	slacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
	sgeevx_(balanc, "V", "N", sense, n, &h__[h_offset], lda, &wr1[1], &
		wi1[1], &lre[lre_offset], ldlre, dum, &c__1, &ilo1, &ihi1, &
		scale1[1], &abnrm1, &rcnde1[1], &rcndv1[1], &work[1], lwork, &
		iwork[1], &iinfo);
	if (iinfo != 0) {
	    result[1] = ulpinv;
	    if (*jtype != 22) {
		io___32.ciunit = *nounit;
		s_wsfe(&io___32);
		do_fio(&c__1, "SGEEVX4", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*jtype), (ftnlen)sizeof(integer));
		do_fio(&c__1, balanc, (ftnlen)1);
		do_fio(&c__4, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		io___33.ciunit = *nounit;
		s_wsfe(&io___33);
		do_fio(&c__1, "SGEEVX4", (ftnlen)7);
		do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	    *info = abs(iinfo);
	    goto L190;
	}

/*        Do Test (5) again */

	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    if (wr[j] != wr1[j] || wi[j] != wi1[j]) {
		result[5] = ulpinv;
	    }
/* L140: */
	}

/*        Do Test (7) */

	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = *n;
	    for (jj = 1; jj <= i__3; ++jj) {
		if (vl[j + jj * vl_dim1] != lre[j + jj * lre_dim1]) {
		    result[7] = ulpinv;
		}
/* L150: */
	    }
/* L160: */
	}

/*        Do Test (8) again */

	if (! nobal) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (scale[j] != scale1[j]) {
		    result[8] = ulpinv;
		}
/* L170: */
	    }
	    if (ilo != ilo1) {
		result[8] = ulpinv;
	    }
	    if (ihi != ihi1) {
		result[8] = ulpinv;
	    }
	    if (abnrm != abnrm1) {
		result[8] = ulpinv;
	    }
	}

/*        Do Test (9) again */

	if (isens == 2 && *n > 1) {
	    i__2 = *n;
	    for (j = 1; j <= i__2; ++j) {
		if (rcondv[j] != rcndv1[j]) {
		    result[9] = ulpinv;
		}
/* L180: */
	    }
	}

L190:

/* L200: */
	;
    }

/*     If COMP, compare condition numbers to precomputed ones */

    if (*comp) {
	slacpy_("F", n, n, &a[a_offset], lda, &h__[h_offset], lda);
	sgeevx_("N", "V", "V", "B", n, &h__[h_offset], lda, &wr[1], &wi[1], &
		vl[vl_offset], ldvl, &vr[vr_offset], ldvr, &ilo, &ihi, &scale[
		1], &abnrm, &rconde[1], &rcondv[1], &work[1], lwork, &iwork[1]
, &iinfo);
	if (iinfo != 0) {
	    result[1] = ulpinv;
	    io___34.ciunit = *nounit;
	    s_wsfe(&io___34);
	    do_fio(&c__1, "SGEEVX5", (ftnlen)7);
	    do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*n), (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iseed[1], (ftnlen)sizeof(integer));
	    e_wsfe();
	    *info = abs(iinfo);
	    goto L250;
	}

/*        Sort eigenvalues and condition numbers lexicographically */
/*        to compare with inputs */

	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    kmin = i__;
	    vrmin = wr[i__];
	    vimin = wi[i__];
	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		if (wr[j] < vrmin) {
		    kmin = j;
		    vrmin = wr[j];
		    vimin = wi[j];
		}
/* L210: */
	    }
	    wr[kmin] = wr[i__];
	    wi[kmin] = wi[i__];
	    wr[i__] = vrmin;
	    wi[i__] = vimin;
	    vrmin = rconde[kmin];
	    rconde[kmin] = rconde[i__];
	    rconde[i__] = vrmin;
	    vrmin = rcondv[kmin];
	    rcondv[kmin] = rcondv[i__];
	    rcondv[i__] = vrmin;
/* L220: */
	}

/*        Compare condition numbers for eigenvectors */
/*        taking their condition numbers into account */

	result[10] = 0.f;
	eps = dmax(5.9605e-8f,ulp);
/* Computing MAX */
	r__1 = (real) (*n) * eps * abnrm;
	v = dmax(r__1,smlnum);
	if (abnrm == 0.f) {
	    v = 1.f;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (v > rcondv[i__] * rconde[i__]) {
		tol = rcondv[i__];
	    } else {
		tol = v / rconde[i__];
	    }
	    if (v > rcdvin[i__] * rcdein[i__]) {
		tolin = rcdvin[i__];
	    } else {
		tolin = v / rcdein[i__];
	    }
/* Computing MAX */
	    r__1 = tol, r__2 = smlnum / eps;
	    tol = dmax(r__1,r__2);
/* Computing MAX */
	    r__1 = tolin, r__2 = smlnum / eps;
	    tolin = dmax(r__1,r__2);
	    if (eps * (rcdvin[i__] - tolin) > rcondv[i__] + tol) {
		vmax = 1.f / eps;
	    } else if (rcdvin[i__] - tolin > rcondv[i__] + tol) {
		vmax = (rcdvin[i__] - tolin) / (rcondv[i__] + tol);
	    } else if (rcdvin[i__] + tolin < eps * (rcondv[i__] - tol)) {
		vmax = 1.f / eps;
	    } else if (rcdvin[i__] + tolin < rcondv[i__] - tol) {
		vmax = (rcondv[i__] - tol) / (rcdvin[i__] + tolin);
	    } else {
		vmax = 1.f;
	    }
	    result[10] = dmax(result[10],vmax);
/* L230: */
	}

/*        Compare condition numbers for eigenvalues */
/*        taking their condition numbers into account */

	result[11] = 0.f;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (v > rcondv[i__]) {
		tol = 1.f;
	    } else {
		tol = v / rcondv[i__];
	    }
	    if (v > rcdvin[i__]) {
		tolin = 1.f;
	    } else {
		tolin = v / rcdvin[i__];
	    }
/* Computing MAX */
	    r__1 = tol, r__2 = smlnum / eps;
	    tol = dmax(r__1,r__2);
/* Computing MAX */
	    r__1 = tolin, r__2 = smlnum / eps;
	    tolin = dmax(r__1,r__2);
	    if (eps * (rcdein[i__] - tolin) > rconde[i__] + tol) {
		vmax = 1.f / eps;
	    } else if (rcdein[i__] - tolin > rconde[i__] + tol) {
		vmax = (rcdein[i__] - tolin) / (rconde[i__] + tol);
	    } else if (rcdein[i__] + tolin < eps * (rconde[i__] - tol)) {
		vmax = 1.f / eps;
	    } else if (rcdein[i__] + tolin < rconde[i__] - tol) {
		vmax = (rconde[i__] - tol) / (rcdein[i__] + tolin);
	    } else {
		vmax = 1.f;
	    }
	    result[11] = dmax(result[11],vmax);
/* L240: */
	}
L250:

	;
    }


    return 0;

/*     End of SGET23 */

} /* sget23_ */
