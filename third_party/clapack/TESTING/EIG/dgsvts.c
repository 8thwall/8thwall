#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b17 = 1.;
static doublereal c_b18 = 0.;
static doublereal c_b44 = -1.;
static integer c__1 = 1;

/* Subroutine */ int dgsvts_(integer *m, integer *p, integer *n, doublereal *
	a, doublereal *af, integer *lda, doublereal *b, doublereal *bf, 
	integer *ldb, doublereal *u, integer *ldu, doublereal *v, integer *
	ldv, doublereal *q, integer *ldq, doublereal *alpha, doublereal *beta, 
	 doublereal *r__, integer *ldr, integer *iwork, doublereal *work, 
	integer *lwork, doublereal *rwork, doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, bf_dim1, 
	    bf_offset, q_dim1, q_offset, r_dim1, r_offset, u_dim1, u_offset, 
	    v_dim1, v_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    integer i__, j, k, l;
    doublereal ulp;
    integer info;
    doublereal unfl, temp;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    doublereal resid, anorm, bnorm;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *), dsyrk_(char *, char *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *);
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlaset_(char *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *), dggsvd_(char *, char *, char *, 
	    integer *, integer *, integer *, integer *, integer *, doublereal 
	    *, integer *, doublereal *, integer *, doublereal *, doublereal *, 
	     doublereal *, integer *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, integer *, integer *);
    extern doublereal dlansy_(char *, char *, integer *, doublereal *, 
	    integer *, doublereal *);
    doublereal ulpinv;


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGSVTS tests DGGSVD, which computes the GSVD of an M-by-N matrix A */
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

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,M) */
/*          The M-by-N matrix A. */

/*  AF      (output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          Details of the GSVD of A and B, as returned by DGGSVD, */
/*          see DGGSVD for further details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the arrays A and AF. */
/*          LDA >= max( 1,M ). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,P) */
/*          On entry, the P-by-N matrix B. */

/*  BF      (output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          Details of the GSVD of A and B, as returned by DGGSVD, */
/*          see DGGSVD for further details. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the arrays B and BF. */
/*          LDB >= max(1,P). */

/*  U       (output) DOUBLE PRECISION array, dimension(LDU,M) */
/*          The M by M orthogonal matrix U. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U. LDU >= max(1,M). */

/*  V       (output) DOUBLE PRECISION array, dimension(LDV,M) */
/*          The P by P orthogonal matrix V. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. LDV >= max(1,P). */

/*  Q       (output) DOUBLE PRECISION array, dimension(LDQ,N) */
/*          The N by N orthogonal matrix Q. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= max(1,N). */

/*  ALPHA   (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          The generalized singular value pairs of A and B, the */
/*          ``diagonal'' matrices D1 and D2 are constructed from */
/*          ALPHA and BETA, see subroutine DGGSVD for details. */

/*  R       (output) DOUBLE PRECISION array, dimension(LDQ,N) */
/*          The upper triangular matrix R. */

/*  LDR     (input) INTEGER */
/*          The leading dimension of the array R. LDR >= max(1,N). */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK) */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK, */
/*          LWORK >= max(M,P,N)*max(M,P,N). */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (max(M,P,N)) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (6) */
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
    ulp = dlamch_("Precision");
    ulpinv = 1. / ulp;
    unfl = dlamch_("Safe minimum");

/*     Copy the matrix A to the array AF. */

    dlacpy_("Full", m, n, &a[a_offset], lda, &af[af_offset], lda);
    dlacpy_("Full", p, n, &b[b_offset], ldb, &bf[bf_offset], ldb);

/* Computing MAX */
    d__1 = dlange_("1", m, n, &a[a_offset], lda, &rwork[1]);
    anorm = max(d__1,unfl);
/* Computing MAX */
    d__1 = dlange_("1", p, n, &b[b_offset], ldb, &rwork[1]);
    bnorm = max(d__1,unfl);

/*     Factorize the matrices A and B in the arrays AF and BF. */

    dggsvd_("U", "V", "Q", m, n, p, &k, &l, &af[af_offset], lda, &bf[
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

    dgemm_("No transpose", "No transpose", m, n, n, &c_b17, &a[a_offset], lda, 
	     &q[q_offset], ldq, &c_b18, &work[1], lda)
	    ;

    dgemm_("Transpose", "No transpose", m, n, m, &c_b17, &u[u_offset], ldu, &
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

    resid = dlange_("1", m, n, &a[a_offset], lda, &rwork[1]);

    if (anorm > 0.) {
/* Computing MAX */
	i__1 = max(1,*m);
	result[1] = resid / (doublereal) max(i__1,*n) / anorm / ulp;
    } else {
	result[1] = 0.;
    }

/*     Compute B := V'*B*Q - D2*R */

    dgemm_("No transpose", "No transpose", p, n, n, &c_b17, &b[b_offset], ldb, 
	     &q[q_offset], ldq, &c_b18, &work[1], ldb)
	    ;

    dgemm_("Transpose", "No transpose", p, n, p, &c_b17, &v[v_offset], ldv, &
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

    resid = dlange_("1", p, n, &b[b_offset], ldb, &rwork[1]);
    if (bnorm > 0.) {
/* Computing MAX */
	i__1 = max(1,*p);
	result[2] = resid / (doublereal) max(i__1,*n) / bnorm / ulp;
    } else {
	result[2] = 0.;
    }

/*     Compute I - U'*U */

    dlaset_("Full", m, m, &c_b18, &c_b17, &work[1], ldq);
    dsyrk_("Upper", "Transpose", m, m, &c_b44, &u[u_offset], ldu, &c_b17, &
	    work[1], ldu);

/*     Compute norm( I - U'*U ) / ( M * ULP ) . */

    resid = dlansy_("1", "Upper", m, &work[1], ldu, &rwork[1]);
    result[3] = resid / (doublereal) max(1,*m) / ulp;

/*     Compute I - V'*V */

    dlaset_("Full", p, p, &c_b18, &c_b17, &work[1], ldv);
    dsyrk_("Upper", "Transpose", p, p, &c_b44, &v[v_offset], ldv, &c_b17, &
	    work[1], ldv);

/*     Compute norm( I - V'*V ) / ( P * ULP ) . */

    resid = dlansy_("1", "Upper", p, &work[1], ldv, &rwork[1]);
    result[4] = resid / (doublereal) max(1,*p) / ulp;

/*     Compute I - Q'*Q */

    dlaset_("Full", n, n, &c_b18, &c_b17, &work[1], ldq);
    dsyrk_("Upper", "Transpose", n, n, &c_b44, &q[q_offset], ldq, &c_b17, &
	    work[1], ldq);

/*     Compute norm( I - Q'*Q ) / ( N * ULP ) . */

    resid = dlansy_("1", "Upper", n, &work[1], ldq, &rwork[1]);
    result[5] = resid / (doublereal) max(1,*n) / ulp;

/*     Check sorting */

    dcopy_(n, &alpha[1], &c__1, &work[1], &c__1);
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

    result[6] = 0.;
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

/*     End of DGSVTS */

} /* dgsvts_ */
