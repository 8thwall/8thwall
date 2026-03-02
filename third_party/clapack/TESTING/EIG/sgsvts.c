#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static real c_b17 = 1.f;
static real c_b18 = 0.f;
static real c_b44 = -1.f;
static integer c__1 = 1;

/* Subroutine */ int sgsvts_(integer *m, integer *p, integer *n, real *a, 
	real *af, integer *lda, real *b, real *bf, integer *ldb, real *u, 
	integer *ldu, real *v, integer *ldv, real *q, integer *ldq, real *
	alpha, real *beta, real *r__, integer *ldr, integer *iwork, real *
	work, integer *lwork, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset, q_dim1, q_offset, r_dim1, r_offset, u_dim1, u_offset, 
	    v_dim1, v_offset, i__1, i__2;
    real r__1;

    /* Local variables */
    integer i__, j, k, l;
    real ulp;
    integer info;
    real unfl, temp, resid;
    extern /* Subroutine */ int sgemm_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, real *, integer *, real *, 
	    real *, integer *);
    real anorm, bnorm;
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), ssyrk_(char *, char *, integer *, integer *, real *, 
	    real *, integer *, real *, real *, integer *);
    extern doublereal slamch_(char *), slange_(char *, integer *, 
	    integer *, real *, integer *, real *);
    extern /* Subroutine */ int slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *), slaset_(char *, integer *, 
	    integer *, real *, real *, real *, integer *), sggsvd_(
	    char *, char *, char *, integer *, integer *, integer *, integer *
, integer *, real *, integer *, real *, integer *, real *, real *, 
	     real *, integer *, real *, integer *, real *, integer *, real *, 
	    integer *, integer *);
    extern doublereal slansy_(char *, char *, integer *, real *, integer *, 
	    real *);
    real ulpinv;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGSVTS tests SGGSVD, which computes the GSVD of an M-by-N matrix A */
/*  and a P-by-N matrix B: */
/*               U'*A*Q = D1*R and V'*B*Q = D2*R. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  A       (input) REAL array, dimension (LDA,M) */
/*          The M-by-N matrix A. */

/*  AF      (output) REAL array, dimension (LDA,N) */
/*          Details of the GSVD of A and B, as returned by SGGSVD, */
/*          see SGGSVD for further details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A and AF. */
/*          LDA >= max( 1,M ). */

/*  B       (input) REAL array, dimension (LDB,P) */
/*          On entry, the P-by-N matrix B. */

/*  BF      (output) REAL array, dimension (LDB,N) */
/*          Details of the GSVD of A and B, as returned by SGGSVD, */
/*          see SGGSVD for further details. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B and BF. */
/*          LDB >= max(1,P). */

/*  U       (output) REAL array, dimension(LDU,M) */
/*          The M by M orthogonal matrix U. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U. LDU >= max(1,M). */

/*  V       (output) REAL array, dimension(LDV,M) */
/*          The P by P orthogonal matrix V. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. LDV >= max(1,P). */

/*  Q       (output) REAL array, dimension(LDQ,N) */
/*          The N by N orthogonal matrix Q. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= max(1,N). */

/*  ALPHA   (output) REAL array, dimension (N) */
/*  BETA    (output) REAL array, dimension (N) */
/*          The generalized singular value pairs of A and B, the */
/*          ``diagonal'' matrices D1 and D2 are constructed from */
/*          ALPHA and BETA, see subroutine SGGSVD for details. */

/*  R       (output) REAL array, dimension(LDQ,N) */
/*          The upper triangular matrix R. */

/*  LDR     (input) INTEGER */
/*          The leading dimension of the array R. LDR >= max(1,N). */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  WORK    (workspace) REAL array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK, */
/*          LWORK >= max(M,P,N)*max(M,P,N). */

/*  RWORK   (workspace) REAL array, dimension (max(M,P,N)) */

/*  RESULT  (output) REAL array, dimension (6) */
/*          The test ratios: */
/*          RESULT(1) = norm( U'*A*Q - D1*R ) / ( MAX(M,N)*norm(A)*ULP) */
/*          RESULT(2) = norm( V'*B*Q - D2*R ) / ( MAX(P,N)*norm(B)*ULP) */
/*          RESULT(3) = norm( I - U'*U ) / ( M*ULP ) */
/*          RESULT(4) = norm( I - V'*V ) / ( P*ULP ) */
/*          RESULT(5) = norm( I - Q'*Q ) / ( N*ULP ) */
/*          RESULT(6) = 0        if ALPHA is in decreasing order; */
/*                    = ULPINV   otherwise. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    af_dim1 = *lda;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    bf_dim1 = *ldb;
    bf_offset = 1 + bf_dim1;
    bf -= bf_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --alpha;
    --beta;
    r_dim1 = *ldr;
    r_offset = 1 + r_dim1;
    r__ -= r_offset;
    --iwork;
    --work;
    --rwork;
    --result;

    /* Function Body */
    ulp = slamch_("Precision");
    ulpinv = 1.f / ulp;
    unfl = slamch_("Safe minimum");

/*     Copy the matrix A to the array AF. */

    slacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);
    slacpy_("Full", p, n, &b[b_offset], ldb, &bf[bf_offset], ldb);

/* Computing MAX */
    r__1 = slange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    anorm = dmax(r__1,unfl);
/* Computing MAX */
    r__1 = slange_("1", p, n, &b[b_offset], ldb, &rwork[1]);
    bnorm = dmax(r__1,unfl);

/*     Factorize the matrices A and B in the arrays AF and BF. */

    sggsvd_("U", "V", "Q", m, n, p, &k, &l, &af[af_offset], lda, &bf[
	    bf_offset], ldb, &alpha[1], &beta[1], &u[u_offset], ldu, &v[
	    v_offset], ldv, &q[q_offset], ldq, &work[1], &iwork[1], &info);

/*     Copy R */

/* Computing MIN */
    i__2 = k + l;
    i__1 = min(i__2,*m);
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = k + l;
	for (j = i__; j <= i__2; ++j) {
	    r__[i__ + j * r_dim1] = af[i__ + (*n - k - l + j) * af_dim1];
/* L10: */
	}
/* L20: */
    }

    if (*m - k - l < 0) {
	i__1 = k + l;
	for (i__ = *m + 1; i__ <= i__1; ++i__) {
	    i__2 = k + l;
	    for (j = i__; j <= i__2; ++j) {
		r__[i__ + j * r_dim1] = bf[i__ - k + (*n - k - l + j) * 
			bf_dim1];
/* L30: */
	    }
/* L40: */
	}
    }

/*     Compute A:= U'*A*Q - D1*R */

    sgemm_("No transpose", "No transpose", m, n, n, &c_b17, &a[a_offset], lda, 
	     &q[q_offset], ldq, &c_b18, &work[1], lda)
	    ;

    sgemm_("Transpose", "No transpose", m, n, m, &c_b17, &u[u_offset], ldu, &
	    work[1], lda, &c_b18, &a[a_offset], lda);

    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = k + l;
	for (j = i__; j <= i__2; ++j) {
	    a[i__ + (*n - k - l + j) * a_dim1] -= r__[i__ + j * r_dim1];
/* L50: */
	}
/* L60: */
    }

/* Computing MIN */
    i__2 = k + l;
    i__1 = min(i__2,*m);
    for (i__ = k + 1; i__ <= i__1; ++i__) {
	i__2 = k + l;
	for (j = i__; j <= i__2; ++j) {
	    a[i__ + (*n - k - l + j) * a_dim1] -= alpha[i__] * r__[i__ + j * 
		    r_dim1];
/* L70: */
	}
/* L80: */
    }

/*     Compute norm( U'*A*Q - D1*R ) / ( MAX(1,M,N)*norm(A)*ULP ) . */

    resid = slange_("1", m, n, &a[a_offset], lda, &rwork[1]);

    if (anorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*m);
	result[1] = resid / (real) max(i__1,*n) / anorm / ulp;
    } else {
	result[1] = 0.f;
    }

/*     Compute B := V'*B*Q - D2*R */

    sgemm_("No transpose", "No transpose", p, n, n, &c_b17, &b[b_offset], ldb, 
	     &q[q_offset], ldq, &c_b18, &work[1], ldb)
	    ;

    sgemm_("Transpose", "No transpose", p, n, p, &c_b17, &v[v_offset], ldv, &
	    work[1], ldb, &c_b18, &b[b_offset], ldb);

    i__1 = l;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = l;
	for (j = i__; j <= i__2; ++j) {
	    b[i__ + (*n - l + j) * b_dim1] -= beta[k + i__] * r__[k + i__ + (
		    k + j) * r_dim1];
/* L90: */
	}
/* L100: */
    }

/*     Compute norm( V'*B*Q - D2*R ) / ( MAX(P,N)*norm(B)*ULP ) . */

    resid = slange_("1", p, n, &b[b_offset], ldb, &rwork[1]);
    if (bnorm > 0.f) {
/* Computing MAX */
	i__1 = max(1,*p);
	result[2] = resid / (real) max(i__1,*n) / bnorm / ulp;
    } else {
	result[2] = 0.f;
    }

/*     Compute I - U'*U */

    slaset_("Full", m, m, &c_b18, &c_b17, &work[1], ldq);
    ssyrk_("Upper", "Transpose", m, m, &c_b44, &u[u_offset], ldu, &c_b17, &
	    work[1], ldu);

/*     Compute norm( I - U'*U ) / ( M * ULP ) . */

    resid = slansy_("1", "Upper", m, &work[1], ldu, &rwork[1]);
    result[3] = resid / (real) max(1,*m) / ulp;

/*     Compute I - V'*V */

    slaset_("Full", p, p, &c_b18, &c_b17, &work[1], ldv);
    ssyrk_("Upper", "Transpose", p, p, &c_b44, &v[v_offset], ldv, &c_b17, &
	    work[1], ldv);

/*     Compute norm( I - V'*V ) / ( P * ULP ) . */

    resid = slansy_("1", "Upper", p, &work[1], ldv, &rwork[1]);
    result[4] = resid / (real) max(1,*p) / ulp;

/*     Compute I - Q'*Q */

    slaset_("Full", n, n, &c_b18, &c_b17, &work[1], ldq);
    ssyrk_("Upper", "Transpose", n, n, &c_b44, &q[q_offset], ldq, &c_b17, &
	    work[1], ldq);

/*     Compute norm( I - Q'*Q ) / ( N * ULP ) . */

    resid = slansy_("1", "Upper", n, &work[1], ldq, &rwork[1]);
    result[5] = resid / (real) max(1,*n) / ulp;

/*     Check sorting */

    scopy_(n, &alpha[1], &c__1, &work[1], &c__1);
/* Computing MIN */
    i__2 = k + l;
    i__1 = min(i__2,*m);
    for (i__ = k + 1; i__ <= i__1; ++i__) {
	j = iwork[i__];
	if (i__ != j) {
	    temp = work[i__];
	    work[i__] = work[j];
	    work[j] = temp;
	}
/* L110: */
    }

    result[6] = 0.f;
/* Computing MIN */
    i__2 = k + l;
    i__1 = min(i__2,*m) - 1;
    for (i__ = k + 1; i__ <= i__1; ++i__) {
	if (work[i__] < work[i__ + 1]) {
	    result[6] = ulpinv;
	}
/* L120: */
    }

    return 0;

/*     End of SGSVTS */

} /* sgsvts_ */
